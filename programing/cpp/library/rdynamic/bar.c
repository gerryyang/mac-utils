#include <stdio.h>

extern void foo();

// local func
void foo()
{
    printf("bar.c foo()\n");
}

void bar()
{
    foo();
}
