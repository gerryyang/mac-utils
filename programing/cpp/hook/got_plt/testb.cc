#include "testb.h"
#include <stdio.h>

void Foo::say_world()
{
	printf("Foo::say_world\n");
}

void say_world()
{
	Foo foo;
	foo.say_world();
}
