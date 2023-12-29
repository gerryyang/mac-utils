
#include <stdio.h>
#include <dlfcn.h>
#include "testd.h"
#include "testd_hook.h"
#include "plthook.h"

/*
plthook_enum 函数的目的是遍历共享库中的程序链接表（PLT）条目

PLT 是用于实现动态链接的一种机制。当程序调用一个动态链接的函数时，它会首先跳转到 PLT 中的一个条目，然后通过这个条目找到并跳转到实际的函数地址
PLT 条目通常与 ELF（可执行和链接格式）文件中的重定位表（例如 .rela.plt 和 .rela.dyn 段）相关联

R_JUMP_SLOT 和 R_GLOBAL_DATA 是 ELF 文件中的重定位类型。它们表示如何处理与动态链接相关的重定位条目

R_JUMP_SLOT:
    这个重定位类型表示一个函数调用
    当程序调用一个动态链接的函数时，它会跳转到一个与 R_JUMP_SLOT 类型的重定位条目关联的 PLT 条目，这个条目负责查找并跳转到实际的函数地址
    R_JUMP_SLOT 通常与 .rela.plt 段中的重定位条目相关联

R_GLOBAL_DATA:
    这个重定位类型表示一个全局数据引用
    当程序访问一个动态链接的全局变量时，它会通过一个与 R_GLOBAL_DATA 类型的重定位条目关联的 PLT 条目，这个条目负责查找并返回实际的全局变量地址
    R_GLOBAL_DATA 通常与 .rela.dyn 段中的重定位条目相关联

plthook_enum 函数通过遍历 plthook->rela_plt 和 plthook->rela_dyn 数组来查找与 R_JUMP_SLOT 和 R_GLOBAL_DATA 类型相关的重定位条目
对于每个条目，它调用 check_rel 函数来检查这个条目是否符合期望的类型。如果 check_rel 返回非负值，plthook_enum 就将这个条目的名称和地址返回给调用者
*/
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

    // 替换普通非类成员函数
    //if (plthook_replace(plthook, hotpatch_func, (void *)&HotPatch::say_hello_hotpatch_impl, NULL) != 0)
    // 替换类成员函数，需要是类静态函数，第一个参数是 this 指针
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
    // 编译时已经知道了非虚函数的地址，因此可以在程序链接表（PLT）中创建一个条目来引用这个函数

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
plthook_replace func(_ZN5TestD9say_helloEi) error: no such function: _ZN5TestD9say_helloEi
orignal: virtual void TestD::say_hello(int) m_a(1) a(123)
orignal: void TestD::say_hello_impl(int) m_a(1) a(123)
void test2() done
*/
void test2()
{
    // 测试用例：修改可执行程序的 plt 函数跳转地址 (C++ 虚函数)
    // 虚函数的调用必须在运行时才能确定。编译器不知道虚函数的确切位置，因为它取决于动态类型（即运行时的类型）
    // 因此，编译器不能在 PLT 中创建一个条目来引用这个虚函数。虚函数的调用是通过虚函数表（vtable）进行的，这是一个在每个对象中存储的函数指针数组
    // 当调用一个虚函数时，程序会查找对象的 vtable，然后跳转到相应的函数地址。这就是为什么无法在 PLT 中找到 TestD::say_hello 虚函数的原因

    Base *pObj = new TestD();
    pObj->say_hello(123);

    const char *filename = NULL;                          // 表示修改可执行程序的 plt 表
    const char *hotpatch_func = "_ZN5TestD9say_helloEi";  // TestD::say_hello(int)
    install_hook_function(filename, hotpatch_func);       // 替换失败，找不到这个函数符号

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

    // 如何替换 C++ 虚函数？
    // 方案：在 C++ 虚函数中调用非虚函数 func_impl，对 func_impl 函数进行替换
    // 注意：只能对 libtestd.so 的 plt 进行修改

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
    //print_plt_entries("libtestd.so");

    // 测试用例：修改可执行程序的 plt 函数跳转地址 (C++ 非虚函数)
    test1();

    // 测试用例：修改可执行程序的 plt 函数跳转地址 (C++ 虚函数)
    //test2();

    // 测试用例：修改 libtestd.so 中的 plt 函数跳转地址
    //test3();

    // 测试用例：修改 libtestd.so 中的 plt 函数跳转地址，使用 install_hook_function_with_oldfunc
    //test4();

    // 测试用例：修改可执行程序的 plt 函数跳转地址 (C++ 非虚函数)，测试多个 plt 表是否相互影响
    //test5();

    return 0;
}