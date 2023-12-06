#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <stdio.h>
#include "plthook.h"
#include "testc.h"

struct FunctionPointerHelper
{
    void (TestC::*say_hello_func)(int);
};

FunctionPointerHelper helper;

void my_say_hello(TestC *obj, int a)
{
    printf("Hooked: TestC::say_hello\n");
    (obj->*helper.say_hello_func)(a);  // call real TestC::say_hello
}

int install_hook_function()
{
    plthook_t *plthook;

    if (plthook_open_by_address(&plthook, &helper.say_hello_func) != 0)
    {
        printf("plthook_open error: %s\n", plthook_error());
        return -1;
    }
    // Use the mangled name of TestC::say_hello
    if (plthook_replace(plthook, "_ZN5TestC9say_helloEi", (void *)&my_say_hello, (void **)&helper.say_hello_func) != 0)
    {
        printf("plthook_replace error: %s\n", plthook_error());
        plthook_close(plthook);
        return -1;
    }
#ifndef WIN32
    // The address passed to the fourth argument of plthook_replace() is
    // availabe on Windows. But not on Unixes. Get the real address by dlsym().
    void *func_addr = dlsym(RTLD_DEFAULT, "_ZN5TestC9say_helloEi");
    helper.say_hello_func = *((void(TestC::**)(int)) & func_addr);
#endif
    plthook_close(plthook);
    return 0;
}

int main()
{
    install_hook_function();
    TestC obj;
    obj.say_hello(123);

    return 0;
}