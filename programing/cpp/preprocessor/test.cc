#include <iostream>

// https://en.cppreference.com/w/cpp/preprocessor/replace
#define showlist(...) puts(#__VA_ARGS__)
#define f(a) puts("hello " #a)

int main()
{
    showlist();
    showlist(1, "x", int);

    f(gerry);

    return 0;
}
