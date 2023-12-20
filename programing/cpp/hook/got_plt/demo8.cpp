
#include <stdio.h>
#include <dlfcn.h>
#include "testd.h"
#include "testd_hook.h"
#include "plthook.h"

int print_plt_entries(const char *filename)
{
    plthook_t *plthook;
    unsigned int pos = 0; /* This must be initialized with zero. */
    const char *name;
    void **addr;

    if (plthook_open(&plthook, filename) != 0)
    {
        printf("plthook_open error: %s\n", plthook_error());
        return -1;
    }
    while (plthook_enum(plthook, &pos, &name, &addr) == 0)
    {
        printf("%p(%p) %s\n", addr, *addr, name);
    }
    plthook_close(plthook);
    return 0;
}

int install_hook_function(const char *filename, const char *hotpatch_func)
{
    plthook_t *plthook;

    if (plthook_open(&plthook, filename) != 0)
    {
        printf("plthook_open so(%s) error: %s\n", filename, plthook_error());
        return -1;
    }
    printf("plthook_open so(%s) ok\n", filename);

    // Use the mangled name
    if (plthook_replace(plthook, hotpatch_func, (void *)&HotPatch::TestD::say_hello_hotpatch, NULL) != 0)
    {
        printf("plthook_replace func(%s) error: %s\n", hotpatch_func, plthook_error());
        plthook_close(plthook);
        return -1;
    }
    printf("plthook_replace func(%s) ok\n", hotpatch_func);

    plthook_close(plthook);
    return 0;
}

int install_hook_function_with_oldfunc(const char *filename, const char *hotpatch_func)
{
    plthook_t *plthook;

    if (plthook_open(&plthook, filename) != 0)
    {
        printf("plthook_open so(%s) error: %s\n", filename, plthook_error());
        return -1;
    }
    printf("plthook_open so(%s) ok\n", filename);

    // Use the mangled name
    if (plthook_replace(plthook, hotpatch_func, (void *)&HotPatch::TestD::say_hello_hotpatch, NULL) != 0)
    {
        printf("plthook_replace func(%s) error: %s\n", hotpatch_func, plthook_error());
        plthook_close(plthook);
        return -1;
    }
    printf("plthook_replace func(%s) ok\n", hotpatch_func);

    {
        typedef void (TestD::*say_hello_impl_func)(int);
        union
        {
            void *obj_ptr;
            say_hello_impl_func func_ptr;
        } alias;

        alias.obj_ptr = dlsym(RTLD_DEFAULT, hotpatch_func);
        say_hello_impl_func func = alias.func_ptr;

        const char *dlsym_error = dlerror();
        if (dlsym_error)
        {
            printf("dlsym err(%s)\n", dlerror());
            return -1;
        }
        printf("dlsym ok, func addr(%p)\n", alias.obj_ptr);

        TestD t;
        (t.*func)(456);
    }

    plthook_close(plthook);
    return 0;
}

/*
orignal: void TestD::say_hello_impl(int) m_a(1) a(123)
plthook_open so((null)) ok
plthook_replace func(_ZN5TestD14say_hello_implEi) ok
HotPatch: static void HotPatch::TestD::say_hello_hotpatch(HotPatch::TestD*, int) a(123)
HotPatch: void HotPatch::TestD::say_hello_hotpatch_impl(int) m_b(1) a(123)
orignal: virtual void TestD::say_hello(int) m_a(1) a(123)
orignal: void TestD::say_hello_impl(int) m_a(1) a(123)
void test1() done
*/
void test1()
{
    // 测试用例：修改可执行程序的 plt 函数跳转地址 (C++ 非虚函数)

    TestD Obj;
    Obj.say_hello_impl(123);

    const char *filename = NULL;                                // 表示修改可执行程序的 plt 表
    const char *hotpatch_func = "_ZN5TestD14say_hello_implEi";  // TestD::say_hello_impl(int)
    install_hook_function(filename, hotpatch_func);

    // 由于修改了可执行程序的 plt 函数跳转地址，此时函数调用会跳转到 libtestd_hook.so 中的替换函数
    // 因为没有修改 libtestd_hook.so 中的 plt 表，因此可以在 libtestd_hook.so 中的替换函数中直接调用被替换的原始函数
    Obj.say_hello_impl(123);

    printf("%s done\n", __PRETTY_FUNCTION__);
}

/*
orignal: virtual void TestD::say_hello(int) m_a(1) a(123)
orignal: void TestD::say_hello_impl(int) m_a(1) a(123)
plthook_open so((null)) ok
plthook_replace func(_ZN5TestD9say_helloEi) ok
orignal: virtual void TestD::say_hello(int) m_a(1) a(123)
orignal: void TestD::say_hello_impl(int) m_a(1) a(123)
void test2() done
*/
void test2()
{
    // 测试用例：修改可执行程序的 plt 函数跳转地址 (C++ 虚函数)

    Base *pObj = new TestD();
    pObj->say_hello(123);

    const char *filename = NULL;                          // 表示修改可执行程序的 plt 表
    const char *hotpatch_func = "_ZN5TestD9say_helloEi";  // TestD::say_hello(int)
    install_hook_function(filename, hotpatch_func);

    // 虚函数的调用是通过 vtable 实现的，而不是通过 PLT
    // 虽然 PLT 用于解析动态链接库中的函数地址，但对于虚函数，编译器会生成代码来查找和调用 vtable 中的函数指针
    // https://github.com/kubo/plthook/issues/31
    pObj->say_hello(123);

    delete pObj;
    printf("%s done\n", __PRETTY_FUNCTION__);
}

/*
orignal: virtual void TestD::say_hello(int) m_a(1) a(123)
orignal: void TestD::say_hello_impl(int) m_a(1) a(123)
plthook_open so(libtestd.so) ok
plthook_replace func(_ZN5TestD14say_hello_implEi) ok
orignal: virtual void TestD::say_hello(int) m_a(1) a(123)
HotPatch: static void HotPatch::TestD::say_hello_hotpatch(HotPatch::TestD*, int) a(123)
HotPatch: void HotPatch::TestD::say_hello_hotpatch_impl(int) m_a(1) a(123)
void test3() done
*/
void test3()
{
    // 测试用例：修改 libtestd.so 中的 plt 函数跳转地址

    Base *pObj = new TestD();
    pObj->say_hello(123);

    const char *filename = "libtestd.so";
    const char *hotpatch_func = "_ZN5TestD14say_hello_implEi";
    install_hook_function(filename, hotpatch_func);

    // 由于修改了 libtestd.so 中的 plt 函数跳转地址，此时函数调用会跳转到 libtestd_hook.so 中的替换函数
    pObj->say_hello(123);

    delete pObj;
    printf("%s done\n", __PRETTY_FUNCTION__);
}

/*
orignal: virtual void TestD::say_hello(int) m_a(1) a(123)
orignal: void TestD::say_hello_impl(int) m_a(1) a(123)
plthook_open so(libtestd.so) ok
plthook_replace func(_ZN5TestD14say_hello_implEi) ok
dlsym ok, func addr(0x7f5f1eea1da0)
orignal: void TestD::say_hello_impl(int) m_a(1) a(456)
orignal: virtual void TestD::say_hello(int) m_a(1) a(123)
HotPatch: static void HotPatch::TestD::say_hello_hotpatch(HotPatch::TestD*, int) a(123)
HotPatch: void HotPatch::TestD::say_hello_hotpatch_impl(int) m_a(1) a(123)
dlsym ok, func addr(0x7f5f1eea1da0)
orignal: void TestD::say_hello_impl(int) m_a(1) a(456)
void test4() done
*/
void test4()
{
    // 测试用例：修改 libtestd.so 中的 plt 函数跳转地址，使用 install_hook_function_with_oldfunc

    Base *pObj = new TestD();
    pObj->say_hello(123);

    const char *filename = "libtestd.so";
    const char *hotpatch_func = "_ZN5TestD14say_hello_implEi";
    install_hook_function_with_oldfunc(filename, hotpatch_func);

    // 由于修改了 libtestd.so 中的 plt 函数跳转地址，此时函数调用会跳转到 libtestd_hook.so 中的替换函数
    pObj->say_hello(123);

    printf("%s done\n", __PRETTY_FUNCTION__);
}

/*
orignal: void TestD::say_hello_impl(int) m_a(1) a(123)
plthook_open so((null)) ok
plthook_replace func(_ZN5TestD14say_hello_implEi) ok
HotPatch: static void HotPatch::TestD::say_hello_hotpatch(HotPatch::TestD*, int) a(123)
HotPatch: void HotPatch::TestD::say_hello_hotpatch_impl(int) m_a(1) a(123)
dlsym ok, func addr(0x7f578eb05d60)
orignal: void TestD::say_hello_impl(int) m_a(1) a(456)
orignal: void TestD::say_hello_impl(int) m_a(1) a(789)
void test5() done
*/
void test5()
{
    // 测试用例：修改可执行程序的 plt 函数跳转地址 (C++ 非虚函数)，测试多个 plt 表是否相互影响

    TestD Obj;
    Obj.say_hello_impl(123);

    const char *filename = NULL;                                // 表示修改可执行程序的 plt 表
    const char *hotpatch_func = "_ZN5TestD14say_hello_implEi";  // TestD::say_hello_impl(int)
    install_hook_function(filename, hotpatch_func);

    // 由于修改了可执行程序的 plt 函数跳转地址，此时函数调用会跳转到 libtestd_hook.so 中的替换函数
    // 因为没有修改 libtestd_hook.so 中的 plt 表，因此可以在 libtestd_hook.so 中的替换函数中直接调用被替换的原始函数
    Obj.say_hello_impl(123);

    // 在 libtestd_hook.so 中调用 TestD::say_hello_impl 不会被替换
    HotPatch::TestD Obj2;
    Obj2.call_hotpatch_function_from_another_file(789);

    printf("%s done\n", __PRETTY_FUNCTION__);
}

int main()
{
    //print_plt_entries(NULL);

    // 测试用例：修改可执行程序的 plt 函数跳转地址 (C++ 非虚函数)
    //test1();

    // 测试用例：修改可执行程序的 plt 函数跳转地址 (C++ 虚函数)
    //test2();

    // 测试用例：修改 libtestd.so 中的 plt 函数跳转地址
    //test3();

    // 测试用例：修改 libtestd.so 中的 plt 函数跳转地址，使用 install_hook_function_with_oldfunc
    //test4();

    // 测试用例：修改可执行程序的 plt 函数跳转地址 (C++ 非虚函数)，测试多个 plt 表是否相互影响
    test5();

    return 0;
}