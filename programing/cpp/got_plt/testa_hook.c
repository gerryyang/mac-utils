#include "testa_hook.h"
#include "testa.h"
#include <stdio.h>

void say_hello_hook()
{
	printf("say_hello, hooked ...\n");

	say_hello();

	printf("call orig func\n");
}
