#include <vector>
#include <iostream>

int main(int argc, char **argv)
{
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    for (auto i : vec)
    {
        std::cout << i << std::endl;
    }
}
// ASan 检查不出来