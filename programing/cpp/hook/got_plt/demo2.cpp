
#include <stdio.h>
#include <dlfcn.h>

#include "testa.h"
#include "testa_hook.h"
#include "plthook.h"

int install_hook_function()
{
	plthook_t *plthook;

	if (plthook_open(&plthook, NULL) != 0) {
		printf("plthook_open error: %s\n", plthook_error());
		return -1;
	}
	if (plthook_replace(plthook, "say_hello", (void*)say_hello_hook, NULL) != 0) {
		printf("plthook_replace error: %s\n", plthook_error());
		plthook_close(plthook);
		return -1;
	}
	plthook_close(plthook);

// TODO
#if 0
	// malloc hook
	plthook_t *plthook2;

	void *handle = dlopen("libc.so.6", RTLD_LAZY|RTLD_NOLOAD);
	if (nullptr == handle)
	{
		printf("dlopen err\n");
		return -1;
	}

	void *addr = dlsym(handle, "malloc");
	if (nullptr == addr)
	{
		printf("dlsym err\n");
		return -1;
	}

	int rv = plthook_open_by_address(&plthook2, addr);
	if (0 != rv)
	{ 
		printf("plthook_open_by_address err\n");
		return -1;
	}

	if (plthook_replace(plthook2, "malloc", (void*)malloc_hook, NULL) != 0) {
		printf("plthook_replace error: %s\n", plthook_error());
		plthook_close(plthook2);
		return -1;
	}

	plthook_close(plthook2);
#endif

	return 0;
}

int main()
{
	install_hook_function();
	say_hello();

	//char* buffer = (char*)malloc(sizeof(int));
	//free(buffer);


	return 0;
}
