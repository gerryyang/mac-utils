#include <cstdio>
#include <cstdint>
#include <string>
#include "simple_any.h"

struct Test
{
    Test() = default;

    void print()
    {
        printf("IsNull() = %d\t", ext.IsNull());
        printf("Is<void>() = %d\t", ext.Is<void>());
        printf("Is<std::string>() = %d\t", ext.Is<std::string>());
        printf("value(%p): %s\n", &ext.AnyCast<std::string>(), ext.AnyCast<std::string>().c_str());
    }

    Any ext;
};

int main()
{
    Test t;
    t.ext.Reset(std::string("abcdef"));

    // IsNull() = 0    Is<void>() = 0  Is<std::string>() = 1   value(0x5619f9488e78): abcdef
    t.print();

    for (uint32_t idx = 0; idx != 10000000; ++idx)
    {
        Test t2 = t;
    }

    return 0;
}
