#include <stdio.h>
#include <iostream>

extern void foo();
extern "C" void bar();

// local func
void foo()
{
    printf("printf bar.c foo()\n");

	// bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=42679
	std::cout << "std::cout bar.c foo()\n";
}

void bar()
{
    foo();
}
