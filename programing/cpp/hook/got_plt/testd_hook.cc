#include "testd_hook.h"
#include "stdio.h"
#include <dlfcn.h>

namespace HotPatch
{
void TestD::say_hello(int a) /*override*/
{
    printf("HotPatch: %s a(%d)\n", __PRETTY_FUNCTION__, a);
}

void TestD::say_hello_hotpatch_impl(int a)
{
    printf("HotPatch: %s m_a(%d) a(%d)\n", __PRETTY_FUNCTION__, m_a, a);
}

/*static*/ void TestD::say_hello_hotpatch(TestD *obj, int a)
{
    printf("HotPatch: %s a(%d)\n", __PRETTY_FUNCTION__, a);
    obj->say_hello_hotpatch_impl(a);

    // 调用原始函数

    // 方法 1
    // 此方法只适合，当替换的函数在未修改的文件中（即当修改可执行程序的 plt 表）的情况，否则会出现循环调用
    //obj->say_hello(a);

    // 方法 2
    // 通过新的对象调用，证明修改可执行文件的 plt 表或者 libtestd.so 的 plt 表都不会影响 libtestd_hook.so 里对原始函数的调用
    ::TestD Obj;
    Obj.say_hello_impl(123);

    // 方法 3
    // 通过函数指针访问
    {
        typedef void (TestD::*say_hello_impl_func)(int);
        union
        {
            void *obj_ptr;
            say_hello_impl_func func_ptr;
        } alias;

        const char *hotpatch_func = "_ZN5TestD14say_hello_implEi";
        alias.obj_ptr = dlsym(RTLD_DEFAULT, hotpatch_func);
        say_hello_impl_func func = alias.func_ptr;

        const char *dlsym_error = dlerror();
        if (dlsym_error)
        {
            printf("dlsym err(%s)\n", dlerror());
            return;
        }
        printf("dlsym ok, func addr(%p)\n", alias.obj_ptr);

        (obj->*func)(456);
    }
}
}

void HotPatch::TestD::call_hotpatch_function_from_another_file(int a)
{
    ::TestD t;
    t.say_hello_impl(a);
}

void HotPatch::say_hello_hotpatch_impl(void *thisptr, int a)
{
    printf("HotPatch: %s a(%d)\n", __PRETTY_FUNCTION__, a);
    ((::TestD *)thisptr)->say_hello_impl(a);
}