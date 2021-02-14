#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include "testa.h"
#include "plthook.h"

static int (*puts_hook_func)(const char * str);

// This function is called instead of puts 
static int puts_hook(const char * str)
{
	(*puts_hook_func)("puts_hook, hooked");

	(*puts_hook_func)(str); // call real puts

	return 0;
}

int print_plt_entries(const char *filename)
{
    plthook_t *plthook;
    unsigned int pos = 0; /* This must be initialized with zero. */
    const char *name;
    void **addr;

    if (plthook_open(&plthook, filename) != 0) {
        printf("plthook_open error: %s\n", plthook_error());
        return -1;
    }
    while (plthook_enum(plthook, &pos, &name, &addr) == 0) {
        printf("%p(%p) %s\n", addr, *addr, name);
    }
    plthook_close(plthook);
    return 0;
}

int install_hook_function()
{
	plthook_t *plthook;

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
	plthook_close(plthook);
	return 0;
}

int main(int argc, char **argv)
{
	print_plt_entries("libtesta.so");
	say_hello();

	install_hook_function();

	char s[] = "hello";
	puts(s);

	return 0;
}
