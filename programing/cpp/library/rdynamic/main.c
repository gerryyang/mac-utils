#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void foo()
{
	puts("Hello world");
}

int main()
{
	//void* dlh = dlopen("./libbar.so", RTLD_NOW | RTLD_LOCAL);
	void* dlh = dlopen("./libbar.so", RTLD_NOW);
	if (!dlh) {
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	void (*bar)(void) = dlsym(dlh, "bar");
	if (!bar) {
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	bar();
}
