/*
DL Library Example (Linux)
Here's an example from the man page of dlopen(3). This example loads the math library and prints the cosine of 2.0, and it checks for errors at every step (recommended)
If this program were in a file named "foo.c", you would build the program with the following command:

    gcc -o foo foo.c -ldl
 */
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char **argv) {
	void *handle;
	double (*cosine)(double);
	char *error;

	handle = dlopen ("/lib/libm.so.6", RTLD_LAZY);
	if (!handle) {
		fputs (dlerror(), stderr);
		exit(1);
	}

	cosine = dlsym(handle, "cos");
	if ((error = dlerror()) != NULL)  {
		fputs(error, stderr);
		exit(1);
	}

	printf ("%f\n", (*cosine)(2.0));
	dlclose(handle);
}
