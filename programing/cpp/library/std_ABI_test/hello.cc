#include <iostream>
#include <unordered_map>

void hello()
{
    std::cout << __FILE__ << ", __GNUC__(" << __GNUC__ << ") sizeof(std::unordered_map<int, int>) = " << sizeof(std::unordered_map<int, int>) << std::endl;
    std::unordered_map<int, int> m = {{1, 2}, {3, 4}};
    for (auto &t : m)
    {
        std::cout << t.first << "," << t.second << std::endl;
    }
}
