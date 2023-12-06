
#include <stdio.h>
#include <dlfcn.h>
#include "testc.h"
#include "testc_hook.h"
#include "plthook.h"

int install_hook_function()
{
    plthook_t *plthook;

    if (plthook_open(&plthook, NULL) != 0)
    {
        printf("plthook_open error: %s\n", plthook_error());
        return -1;
    }
    // Use the mangled name of TestC::say_hello
    if (plthook_replace(plthook, "_ZN5TestC9say_helloEi", (void *)&my_say_hello, NULL) != 0)
    {
        printf("plthook_replace error: %s\n", plthook_error());
        plthook_close(plthook);
        return -1;
    }
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