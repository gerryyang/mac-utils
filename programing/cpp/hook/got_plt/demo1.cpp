#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <stdio.h>
#include "testa.h"
#include "plthook.h"


static void (*say_hello_func)();

// This function is called instead of say_hello() called by libatest.so
static void my_say_hello()
{
	printf("say_hello, hooked\n");
	(*say_hello_func)(); // call real say_hello
}

int install_hook_function()
{
	plthook_t *plthook;

	if (plthook_open_by_address(&plthook, &say_hello_func) != 0) {
		printf("plthook_open error: %s\n", plthook_error());
		return -1;
	}
	if (plthook_replace(plthook, "say_hello", (void*)my_say_hello, (void**)&say_hello_func) != 0) {
		printf("plthook_replace error: %s\n", plthook_error());
		plthook_close(plthook);
		return -1;
	}
#ifndef WIN32
	// The address passed to the fourth argument of plthook_replace() is
	// availabe on Windows. But not on Unixes. Get the real address by dlsym().
	say_hello_func = (void (*)(void))dlsym(RTLD_DEFAULT, "say_hello");
#endif
	plthook_close(plthook);
	return 0;
}

int main()
{
	install_hook_function();
	say_hello();
	return 0;
}
