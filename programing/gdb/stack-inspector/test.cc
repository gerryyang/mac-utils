#include <iostream>
#include <array>

void func1()
{
    std::cout << __func__ << std::endl;
    std::array<int, 100> arr1;
}

void func2()
{
    std::cout << __func__ << std::endl;
    std::array<int, 200> arr2;
    func1();
}

void func3()
{
    std::cout << __func__ << std::endl;
    std::array<int, 300> arr3;
    func2();
}

int main()
{
    std::cout << __func__ << std::endl;
    func3();
    return 0;
}
