#include <iostream>
#include <string>

class MyClass
{
public:
    MyClass(const std::string& input) : ref(input) {}

    void print()
    {
        std::cout << "MyClass: " << ref << std::endl;
    }

private:
    const std::string& ref;
};

std::string create_temporary_string()
{
    return "temporary";
}

int main()
{
    MyClass obj(create_temporary_string());
    obj.print();  // 未定义行为，因为绑定到对象成员引用上的临时对象已被销毁
    return 0;
}
// refer: https://herbsutter.com/2008/01/01/gotw-88-a-candidate-for-the-most-important-const/