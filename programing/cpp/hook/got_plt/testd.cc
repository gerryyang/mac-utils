#include "testd.h"
#include <stdio.h>

void TestD::say_hello(int a) /*override*/
{
    printf("orignal: %s m_a(%d) a(%d)\n", __PRETTY_FUNCTION__, m_a, a);
    say_hello_impl(a);
}

void TestD::say_hello_impl(int a)
{
    printf("orignal: %s m_a(%d) a(%d)\n", __PRETTY_FUNCTION__, m_a, a);
}