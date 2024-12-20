---
layout: post
title:  "CPP 数字安全计算"
date:   2024-12-20 08:30:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# SafeAdd / SafeMinus

测试代码：https://godbolt.org/z/Gn1bx1d7r

``` cpp
#include <cstdint>
#include <limits>
#include <iostream>

template<typename Type1, typename Type2>
inline static void SafeAdd(Type1& val1, Type2 val2)
{
    static_assert(sizeof(val1) <= sizeof(uint64_t));
    static_assert(sizeof(val2) <= sizeof(uint64_t));
    if (0 == val2)
    {
        return;
    }
    if (val2 < 0)
    {
        std::cout << "val1=" << val1 << " val2=" << val2 << " std::numeric_limits<Type1>::min()=" << std::numeric_limits<Type1>::min() << std::endl;
        uint64_t uVal1 = (uint64_t)(val1 - std::numeric_limits<Type1>::min());
        std::cout << "uVal1=" << uVal1 << std::endl;
        std::cout << "UINT32_MAX=" << UINT32_MAX << std::endl;
        std::cout << "(uint32_t)(-val2)=" << (uint32_t)(-val2) << std::endl;

        uint64_t uVal2 = 0;
        if ((-val2) < UINT32_MAX)
        {
            uVal2 = (uint32_t)(-val2);
        }
        else
        {
            uVal2 = (uint64_t)(-val2);
        }
        std::cout << "uVal2=" << uVal2 << std::endl;

        if (uVal2 > uVal1)
        {
            val1 = std::numeric_limits<Type1>::min();
            return;
        }
    }
    else
    {
        uint64_t uVal1 = (uint64_t)(std::numeric_limits<Type1>::max() - val1);
        uint64_t uVal2 = (uint64_t)(val2);

        if (uVal2 > uVal1)
        {
            val1 = std::numeric_limits<Type1>::max();
            return;
        }
    }
    val1 += val2;
}

template<typename Type1, typename Type2>
inline static void SafeMinus(Type1& val1, Type2 val2)
{
    static_assert(sizeof(val1) <= sizeof(uint64_t));
    static_assert(sizeof(val2) <= sizeof(uint64_t));

    if (0 == val2)
    {
        return;
    }

    if (val2 < 0)
    {
        uint64_t uVal2 = 0;
        if ((-val2) < UINT32_MAX)
        {
            uVal2 = (uint32_t)(-val2);
        }
        else
        {
            uVal2 = (uint64_t)(-val2);
        }
        std::cout << "uVal2=" << uVal2 << std::endl;

        if (uVal2 > (uint64_t)(std::numeric_limits<Type1>::max() - val1))
        {
            val1 = std::numeric_limits<Type1>::max();
            return;
        }
    }
    else
    {
        if ((uint64_t)(val2) > (uint64_t)(val1 - std::numeric_limits<Type1>::min()))
        {
            val1 = std::numeric_limits<Type1>::min();
            return;
        }
    }

    val1 -= val2;
}

int main()
{
    // 4294967295 = 2^32-1
    std::cout << "UINT32_MAX=" << UINT32_MAX << std::endl;
    std::cout << "std::numeric_limits<uint32_t>::max()=" << std::numeric_limits<uint32_t>::max() << std::endl;

    // 18446744073709551615 = 2^64-1
    std::cout << "UINT64_MAX=" << UINT64_MAX << std::endl;
    std::cout << "std::numeric_limits<uint64_t>::max()=" << std::numeric_limits<uint64_t>::max() << std::endl;

    // 2147483647 = 2^31-1
    std::cout << "INT32_MAX=" << INT32_MAX << std::endl;
    std::cout << "std::numeric_limits<int32_t>::max()=" << std::numeric_limits<int32_t>::max() << std::endl;

    // -2147483648
    std::cout << "INT32_MIN=" << INT32_MIN << std::endl;
    std::cout << "std::numeric_limits<int32_t>::min()=" << std::numeric_limits<int32_t>::min() << std::endl;

    // -2147483648
    int32_t min = -2147483648;
    std::cout << "min=" << min << std::endl;

    // 测试 SafeAdd 函数
    uint64_t a = UINT64_MAX;
    int32_t b = INT32_MIN;

    // 问题：将 b 转换为无符号会溢出，期望是 2147483648 但通过强转后变为 18446744071562067968
    std::cout << "(uint64_t)(-b)=" << (uint64_t)(-b) << std::endl;

    // a + b =18446744071562067967 符合预期
    std::cout << "a + b =" << a + b << std::endl;

    // SafeAdd(a, b)=18446744071562067967 符合预期
    SafeAdd(a, b);
    std::cout << "SafeAdd(a, b)=" << a << std::endl;

    // 测试 SafeMinus 函数
    int32_t a2 = INT32_MIN;
    uint64_t b2 = UINT64_MAX;

    // a2 - b2 =18446744071562067969 出现了类型隐式提升，不符合预期
    std::cout << "a2 - b2 =" << a2 - b2 << std::endl;

    // SafeMinus(a2, b2)=-2147483648 此函数保证 a2 在执行减法后不会比 int32_t 所表示最小的数更小
    SafeMinus(a2, b2);
    std::cout << "SafeMinus(a2, b2)=" << a2 << std::endl;

    return 0;
}
```

输出：

```
UINT32_MAX=4294967295
std::numeric_limits<uint32_t>::max()=4294967295
UINT64_MAX=18446744073709551615
std::numeric_limits<uint64_t>::max()=18446744073709551615
INT32_MAX=2147483647
std::numeric_limits<int32_t>::max()=2147483647
INT32_MIN=-2147483648
std::numeric_limits<int32_t>::min()=-2147483648
min=-2147483648
(uint64_t)(-b)=18446744071562067968
a + b =18446744071562067967
val1=18446744073709551615 val2=-2147483648 std::numeric_limits<Type1>::min()=0
uVal1=18446744073709551615
UINT32_MAX=4294967295
(uint32_t)(-val2)=2147483648
uVal2=2147483648
SafeAdd(a, b)=18446744071562067967
a2 - b2 =18446744071562067969
SafeMinus(a2, b2)=-2147483648
```
