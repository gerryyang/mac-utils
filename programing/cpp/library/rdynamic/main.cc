#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

void foo()
{
	puts("puts: main.c foo()");
	std::cout << "std::cout main.c foo()\n";
}

int main()
{

	void* handle = dlopen("./libbar.so", RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
	//void* handle = dlopen("./libbar.so", RTLD_LAZY | RTLD_LOCAL | RTLD_DEEPBIND);
	//void* handle = dlopen("./libbar.so", RTLD_NOW);

	if (!handle) {
		fprintf(stderr, "%s\n", dlerror());
		std::cout << dlerror() << std::endl;
		exit(EXIT_FAILURE);
	}

	// https://linux.die.net/man/3/dlsym
	void (*func)(void);
	*(void **) (&func) = dlsym(handle, "bar");
	if (!func) {
		fprintf(stderr, "%s\n", dlerror());
		std::cout << dlerror() << std::endl;

		dlclose(handle);
		exit(EXIT_FAILURE);
	}

	(*func)();

	typedef void (*func_t)(void);
	void * f = dlsym(handle, "bar");
	char const * const dlsym_error = dlerror();
	if (dlsym_error) {
		fprintf(stderr, "%s\n", dlsym_error);
		std::cout << dlsym_error << std::endl;

		dlclose(handle);
		exit(EXIT_FAILURE);
	}

	reinterpret_cast<func_t>(f)();

	dlclose(handle);

}
