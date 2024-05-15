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

    // 暴露私有成员方法的地址
    static void (Basic::*getFunc())(int)
    {
        return &Basic::secret;
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

    void (Basic::*funcPtr)(int) = Basic::getFunc();
    // 调用私有成员函数
    (tmp.*funcPtr)(10);

    return 0;
}