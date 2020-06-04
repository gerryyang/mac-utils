---
layout: post
title:  "CPP Floating-point Precision Issue"
date:   2020-05-31 16:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{: toc}


# 浮点数存储格式

浮点数的存储格式可参考：[Single-precision floating-point format 32 bits](https://en.wikipedia.org/wiki/Single-precision_floating-point_format)，[Double-precision floating-point format 64 bits](https://en.wikipedia.org/wiki/Double-precision_floating-point_format)

* 单精度浮点型float，通常32位，至少有`6位`有效数字，取值范围`10^-38 - 10^38`
* 双精度浮点型double，通常64位，`15-17位`有效数字，取值范围`10^-308 - 10^308`
* 多精度浮点型long double，精度更高
* A signed 32-bit integer variable has a maximum value of 2^31 − 1 = `2,147,483,647`; An **IEEE 754** 32-bit base-2 floating-point variable has a maximum value of (2 − 2^−23) × 2^127 ≈ `3.4028235 × 10^38`



# 浮点数计算精度问题


## C++


The portable way to get **epsilon** in C++ is:

``` cpp
#include <limits>
std::numeric_limits<double>::epsilon()
```

Then the comparison function becomes:

``` cpp
#include <cmath>
#include <limits>

bool AreSame(double a, double b) {
    return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}
```

* [What is the difference between float and double?](https://stackoverflow.com/questions/2386772/what-is-the-difference-between-float-and-double)
* [What is the most effective way for float and double comparison?](https://stackoverflow.com/questions/17333/what-is-the-most-effective-way-for-float-and-double-comparison/41405501)
* [Comparing Floating Point Numbers, 2012 Edition](https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/)



在`C/C++`中：

``` cpp
double a = 12.03;
double b = 22; 
long long c = a * b * 100000000L;
printf("c[%lld]\n", c);              // 26465999999
c = a * 100000000L * b;
printf("c[%lld]\n", c);              // 26466000000
```

亦或在`python`中：

```
Python 2.7.5 (default, Jun 17 2014, 18:11:42) 
[GCC 4.8.2 20140120 (Red Hat 4.8.2-16)] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> 1.1 + 0.1
1.2000000000000002
```

* Actually, the error is because there is no way to map 0.1 to a finite binary floating point number. 
* Most fractions can't be converted to a decimal with exact precision. A good explanation is here:  [Floating Point Arithmetic: Issues and Limitations](https://docs.python.org/release/2.5.1/tut/node16.html)

> **What can I do to avoid this problem?**
> That depends on what kind of calculations you’re doing.
> * If you really need your results to add up exactly, especially when you work with money: use a special decimal datatype.
> * If you just don’t want to see all those extra decimal places: simply format your result rounded to a fixed number of decimal places when displaying it.
> * If you have no decimal datatype available, an alternative is to work with integers, e.g. do money calculations entirely in cents. But this is more work and has some drawbacks.

refer:

* [What Every Programmer Should Know About Floating-Point Arithmetic](https://floating-point-gui.de/)
* [Floating Point Arithmetic: Issues and Limitations](https://docs.python.org/release/2.5.1/tut/node16.html)
* [如何理解double精度丢失问题？](https://www.zhihu.com/question/42024389/answer/93528601)
* [How to deal with floating point number precision in JavaScript?](https://stackoverflow.com/questions/1458633/how-to-deal-with-floating-point-number-precision-in-javascript)
* [The Perils of Floating Point](http://www.lahey.com/float.htm)

在`C/C++`中的一些解决方案：

* [C++ decimal data types](https://stackoverflow.com/questions/14096026/c-decimal-data-types)
* [开源库- decimal_for_cpp](https://github.com/vpiotr/decimal_for_cpp)

> If you are looking for data type supporting money / currency then try this: decimal_for_cpp

* [boost - cpp_dec_float](https://www.boost.org/doc/libs/1_68_0/libs/multiprecision/doc/html/boost_multiprecision/tut/floats/cpp_dec_float.html)

> The cpp_dec_float back-end is used in conjunction with number: It acts as an entirely C++ (header only and dependency free) floating-point number type that is a drop-in replacement for the native C++ floating-point types, but with much greater precision.

``` cpp
#include <iostream>
#include <iomanip>
#include <boost/multiprecision/cpp_dec_float.hpp>

int main()
{
    namespace mp = boost::multiprecision;
    // here I'm using a predefined type that stores 100 digits,
    // but you can create custom types very easily with any level
    // of precision you want.
    typedef mp::cpp_dec_float_100 decimal;

    decimal tiny("0.0000000000000000000000000000000000000000000001");
    decimal huge("100000000000000000000000000000000000000000000000");
    decimal a = tiny;         

    while (a != huge)
    {
        std::cout.precision(100);
        std::cout << std::fixed << a << '\n';
        a *= 10;
    }

    // (10000000000 - 5000000000) * 2.01 = 10049999999.999998
    cpp_dec_float_50 a(std::to_string(2.01));
    cpp_dec_float_50 b = ((10000000000 - 5000000000)) * a;
    long long c = b.convert_to<long long>();

}
```

* [Exact decimal datatype for C++?](https://stackoverflow.com/questions/15319967/exact-decimal-datatype-for-c)
* [The GNU Multiple Precision arithmetic library](https://gmplib.org/)
* [C++ Data Types](https://www.geeksforgeeks.org/c-data-types/)


## PHP

浮点数的精度有限。尽管取决于系统，PHP 通常使用 `IEEE 754` 双精度格式，则由于取整而导致的最大相对误差为 `1.11e-16`。非基本数学运算可能会给出更大误差，并且要考虑到进行复合运算时的误差传递。此外，以十进制能够精确表示的有理数如 `0.1` 或 `0.7`，无论有多少尾数都不能被内部所使用的二进制精确表示，因此不能在不丢失一点点精度的情况下转换为二进制的格式。这就会造成混乱的结果：例如，`floor((0.1+0.7)*10)` 通常会返回7而不是预期中的8，因为该结果内部的表示其实是类似`7.9999999999999991118...`。

所以永远不要相信浮点数结果精确到了最后一位，也永远不要比较两个浮点数是否相等。如果确实需要更高的精度，应该使用[任意精度数学函数](https://www.php.net/manual/zh/ref.bc.php)或者[gmp函数](https://www.php.net/manual/zh/ref.gmp.php)。

浮点数的字长和平台相关，尽管通常最大值是 `1.8e308` 并具有 `14` 位十进制数字的精度（64 位 IEEE 格式）。

浮点数的形式表示：

```
LNUM          [0-9]+
DNUM          ([0-9]*[\.]{LNUM}) | ({LNUM}[\.][0-9]*)
EXPONENT_DNUM [+-]?(({LNUM} | {DNUM}) [eE][+-]? {LNUM})
```

例如：
```
1.234
1.2e3;
7E-10
```

* [php - Float浮点型](https://www.php.net/manual/zh/language.types.float.php)

## Java

Java中float的精度为6-7位有效数字。double的精度为15-16位。在Java中，通常用到金钱计算的地方要用`BigDecimal`，因为正常的浮点数计算会出现精度丢失的问题。

``` java
System.out.println(0.05 + 0.01);  // 0.060000000000000005
System.out.println(1.0 - 0.42);   // 0.5800000000000001
System.out.println(4.015 * 100);  // 401.49999999999994
System.out.println(123.3 / 100);  // 1.2329999999999999
```

`BigDecimal`使用方法：

``` java
// 构造函数
BigDecimal(int);       // 创建一个具有参数，所指定整数值的对象
BigDecimal(double);    // 创建一个具有参数，所指定双精度值的对象
BigDecimal(long);      // 创建一个具有参数，所指定长整数值的对象
BigDecimal(String);    // 创建一个具有参数，所指定以字符串表示的数值的对象

// 方法                    
add(BigDecimal);       // BigDecimal对象中的值相加，然后返回这个对象
subtract(BigDecimal);  // BigDecimal对象中的值相减，然后返回这个对象
multiply(BigDecimal);  // BigDecimal对象中的值相乘，然后返回这个对象
divide(BigDecimal);    // BigDecimal对象中的值相除，然后返回这个对象
toString();            // 将BigDecimal对象的数值转换成字符串
doubleValue();         // 将BigDecimal对象中的值以双精度数返回
floatValue();          // 将BigDecimal对象中的值以单精度数返回
longValue();           // 将BigDecimal对象中的值以长整数返回
intValue();            // 将BigDecimal对象中的值以整数返回
```

注意，在使用BigDecimal时，使用它的`BigDecimal(String)`构造器创建对象才有意义。其他的如`BigDecimal b = new BigDecimal(1)`这种，还是会发生精度丢失的问题。

源码说明：

``` java
    /* The results of this constructor can be somewhat unpredictable.  
     * One might assume that writing {@codenew BigDecimal(0.1)} in  
     * Java creates a {@code BigDecimal} which is exactly equal to  
     * 0.1 (an unscaled value of 1, with a scale of 1), but it is  
     * actually equal to  
     * 0.1000000000000000055511151231257827021181583404541015625.  
     * This is because 0.1 cannot be represented exactly as a  
     * {@codedouble} (or, for that matter, as a binary fraction of  
     * any finite length).  Thus, the value that is being passed  
     * <i>in</i> to the constructor is not exactly equal to 0.1,  
     * appearances notwithstanding.  
       ……  
        * When a {@codedouble} must be used as a source for a  
     * {@code BigDecimal}, note that this constructor provides an  
     * exact conversion; it does not give the same result as  
     * converting the {@codedouble} to a {@code String} using the  
     * {@link Double#toString(double)} method and then using the  
     * {@link #BigDecimal(String)} constructor.  To get that result,  
     * use the {@codestatic} {@link #valueOf(double)} method.  
     * </ol>  
     */
public BigDecimal(double val) {  
    this(val,MathContext.UNLIMITED);  
}  
```


例子：

``` java
import java.math.BigDecimal;

public class Main {
        public static void main(String[] args) {
                System.out.println("Hello, World!");

                BigDecimal a = new BigDecimal(1.01);
                BigDecimal b = new BigDecimal(1.02);
                BigDecimal c = new BigDecimal("1.01");
                BigDecimal d = new BigDecimal("1.02");
                System.out.println(a.add(b)); // 2.0300000000000000266453525910037569701671600341796875
                System.out.println(c.add(d)); // 2.03
        }
}
```

* [BigDecimal一定不会丢失精度吗？](https://mp.weixin.qq.com/s/ctdLy0Kqd52p-PuYH2-HOw)


