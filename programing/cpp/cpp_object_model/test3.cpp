#include <iostream>

class Basic
{
public:
    int a;
    double b;

    void setB(double value)
    {
        b = value;
        secret(b);
    }

private:
    int c;
    double d;

    void secret(int t)
    {
        d = t + c;
    }
};

int main()
{
    Basic tmp;
    tmp.a = 1;
    tmp.setB(3.14);
    return 0;
}