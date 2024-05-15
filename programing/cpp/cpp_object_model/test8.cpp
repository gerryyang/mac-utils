#include <iostream>

class Base
{
public:
    int a;
    double b;

    virtual void printInfo()
    {
        std::cout << "Base::printInfo() a = " << a << ", b = " << b << std::endl;
    }

    virtual void printB()
    {
        std::cout << "Base::printB()" << std::endl;
    }

    void setB(double value)
    {
        b = value;
    }
};

class Derived : public Base
{
public:
    int c;

    void printInfo() override
    {
        std::cout << "Derived::printInfo() a = " << a << ", b = " << b << ", c = " << c << std::endl;
    }

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

    Base* pBase = &tmp;  // 基类指针指向派生类对象
    pBase->printInfo();  // 多态调用
    pBase->printB();     // 多态调用

    Base base = tmp;
    base.printInfo();  // 无法实现多态调用

    return 0;
}