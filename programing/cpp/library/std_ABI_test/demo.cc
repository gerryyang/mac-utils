#include <iostream>
#include <unordered_map>
#include "hello.h"

int main()
{
    std::cout << __FILE__ << ", __GNUC__(" << __GNUC__ << ") sizeof(std::unordered_map<int, int>) = " << sizeof(std::unordered_map<int, int>) << std::endl;
    hello();
    return 0;
}
