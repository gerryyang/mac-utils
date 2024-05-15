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

    int* p = reinterpret_cast<int*>(reinterpret_cast<char*>(&tmp) + 16);
    *p = 12;  // 绕过编译器检查，修改 c 的值

    return 0;
}