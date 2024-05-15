#include <iostream>

class Base
{
public:
    int a;
    double b;

    void setB(double value)
    {
        b = value;
    }
};

class Derived : public Base
{
public:
    int c;
    void setC(int value)
    {
        c = value;
    }
};

int main()
{
    Derived tmp;
    tmp.a = 1;
    tmp.setB(3.14);
    tmp.c = 2;
    tmp.setC(3);
    return 0;
}