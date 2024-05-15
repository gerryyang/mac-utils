#include <iostream>

class Basic
{
public:
    int a;
    double b;

    void setB(double value)
    {
        b = value;
    }

public:
    static int c;
    static int d;
    static void show()
    {
        std::cout << c << std::endl;
    }
};

int Basic::c = 1;
int Basic::d = 0;
int main()
{
    Basic tmp;
    tmp.a = 1;
    tmp.setB(3.14);

    tmp.show();
    return 0;
}