/* demo_dynamic.c -- demonstrate dynamic loading and
   use of the "hello" routine */


/* Need dlfcn.h for the routines to
   dynamically load libraries */
#include <dlfcn.h>

#include <stdlib.h>
#include <stdio.h>

/* Note that we don't have to include "libhello.h".
   However, we do need to specify something related;
   we need to specify a type that will hold the value
   we're going to get from dlsym(). */

/* The type "simple_demo_function" describes a function that
   takes no arguments, and returns no value: */

typedef void (*simple_demo_function)(void);


int main(void) {
	const char *error;
	void *module;
	simple_demo_function demo_function;

	/* Load dynamically loaded library */
	module = dlopen("libhello.so", RTLD_LAZY);
	if (!module) {
		fprintf(stderr, "Couldn't open libhello.so: %s\n",
				dlerror());
		exit(1);
	}

	/* Get symbol */
	dlerror();
	demo_function = dlsym(module, "hello");
	if ((error = dlerror())) {
		fprintf(stderr, "Couldn't find hello: %s\n", error);
		exit(1);
	}

	/* Now call the function in the DL library */
	(*demo_function)();

	/* All done, close things cleanly */
	dlclose(module);
	return 0;
}
