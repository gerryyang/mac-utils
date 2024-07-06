#include <iostream>
using namespace std;

void f1()
{
    int i;
    for (i = 0; i < 1024 * 1024; ++i)
    {
        short* p = new short;
        //delete p;
    }
}

void f2()
{
    int i;
    for (i = 0; i < 1024 * 1024; ++i)
    {
        int* p = new int;
        //delete p;
    }
}

int main()
{
    f1();
    f2();
    return 0;
}
