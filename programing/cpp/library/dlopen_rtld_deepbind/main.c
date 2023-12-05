#include <dlfcn.h>
#include <stdlib.h>

extern int duplicate;

int main() {
	void *h;
	int (*run)();

	duplicate = 'm';

	h = dlopen("./dynamic.so", RTLD_LAZY | RTLD_DEEPBIND);
	if (!h)
		abort();

	run = dlsym(h, "run");
	if (!run)
		abort();

	(*run)();
}
