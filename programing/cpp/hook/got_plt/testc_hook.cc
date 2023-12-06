#include "testc_hook.h"
#include "stdio.h"
#include "testc.h"

void my_say_hello(TestC *obj, int a)
{
    printf("Hooked: TestC::say_hello\n");
    obj->say_hello(a);  // call real TestC::say_hello
}
