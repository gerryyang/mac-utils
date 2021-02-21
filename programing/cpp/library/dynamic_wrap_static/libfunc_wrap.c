#include <stdio.h>
#include "libfunc.h"

void func_wrap(void) {
	printf("this is func_wrap\n");
	func();
}
