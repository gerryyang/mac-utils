// main.cpp
#include "test.h"
#include <dlfcn.h>
#include <iostream>
#include <cstdint>

int main()
{
    void* handle = dlopen("./libtest.so", RTLD_LAZY);
    if (!handle)
    {
        std::cerr << "Cannot open library: " << dlerror() << '\n';
        return 1;
    }

    // reset errors
    dlerror();

    // load the symbols
    typedef void (Test::*print_t)();

    union
    {
        void* obj_ptr;
        print_t func_ptr;
    } alias;

    alias.obj_ptr = dlsym(handle, "_ZN4Test5printEv");
    print_t print = alias.func_ptr;

    const char* dlsym_error = dlerror();
    if (dlsym_error)
    {
        std::cerr << "Cannot load symbol 'print': " << dlsym_error << '\n';
        dlclose(handle);
        return 1;
    }

    // use it to do the calculation
    Test t;
    (t.*print)();

    // close the library
    dlclose(handle);
}