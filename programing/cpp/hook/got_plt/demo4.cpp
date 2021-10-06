#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "testa.h"
#include "plthook.h"

static int (*puts_hook_func)(const char* str);
static void* (*malloc_hook_func)(size_t size);

// This function is called instead of puts
static int puts_hook(const char* str)
{
    (*puts_hook_func)("puts_hook, hooked");

    (*puts_hook_func)(str);  // call real puts

    return 0;
}

// This function is called instead of malloc
static void* malloc_hook(size_t size)
{
    puts("malloc_hook");

    char* buffer = (char*)(*malloc_hook_func)(size);  // call real malloc

    return buffer;
}

int print_plt_entries(const char* filename)
{
    plthook_t* plthook;
    unsigned int pos = 0; /* This must be initialized with zero. */
    const char* name;
    void** addr;

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

int install_hook_function()
{
    plthook_t* plthook;

#if 0
	// puts hook
	if (plthook_open_by_address(&plthook, &puts_hook_func) != 0) {
		printf("plthook_open error: %s\n", plthook_error());
		return -1;
	}
	if (plthook_replace(plthook, "puts", (void*)puts_hook, (void**)&puts_hook_func) != 0) {
		printf("plthook_replace error: %s\n", plthook_error());
		plthook_close(plthook);
		return -1;
	}

#ifndef WIN32
	// The address passed to the fourth argument of plthook_replace() is
	// availabe on Windows. But not on Unixes. Get the real address by dlsym().
	puts_hook_func = (int (*)(const char * str))dlsym(RTLD_DEFAULT, "puts");
#endif

#endif

    // malloc hook
    if (plthook_open_by_address(&plthook, &malloc_hook_func) != 0)
    {
        printf("plthook_open error: %s\n", plthook_error());
        return -1;
    }
    if (plthook_replace(plthook, "malloc", (void*)malloc_hook, (void**)&malloc_hook_func) != 0)
    {
        printf("plthook_replace error: %s\n", plthook_error());
        plthook_close(plthook);
        return -1;
    }

#ifndef WIN32
    // The address passed to the fourth argument of plthook_replace() is
    // availabe on Windows. But not on Unixes. Get the real address by dlsym().
    malloc_hook_func = (void* (*)(size_t size))dlsym(RTLD_DEFAULT, "malloc");
#endif

    plthook_close(plthook);
    return 0;
}

int main(int argc, char** argv)
{
    //print_plt_entries("libtesta.so");
    //print_plt_entries("libc.so.6");

    say_hello();

    install_hook_function();

    char s[] = "hello";
    puts(s);

    char* buffer = (char*)malloc(sizeof(int));
    buffer[0] = '1';
    buffer[1] = '2';
    buffer[2] = '\0';
    puts(buffer);
    free(buffer);

    char* buffer2 = new char(sizeof(int));
    buffer2[0] = '3';
    buffer2[1] = '4';
    buffer2[2] = '\0';
    puts(buffer2);
    delete buffer2;

    return 0;
}
