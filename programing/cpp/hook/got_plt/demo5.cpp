
#include <stdio.h>

#include "testb.h"
#include "testb_hook.h"
#include "plthook.h"

int install_hook_function()
{
	plthook_t *plthook;

	if (plthook_open(&plthook, NULL) != 0) {
		printf("plthook_open error: %s\n", plthook_error());
		return -1;
	}
	if (plthook_replace(plthook, "say_world", (void*)say_world_hook, NULL) != 0) {
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
	say_world();

	return 0;
}
