---
layout: post
title:  "CPP Error Prone"
date:   2022-11-17 12:30:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# 类型隐士转换问题

## 问题描述

有符号类型与无符号类型比较，导致隐式提升为无符号类型，计算结果与正常期望不符。

``` cpp

auto a = (uint32_t(0) < -1);
std::cout << std::boolalpha << a << std::endl;  // true
```

## 优化方法

* `gcc` 默认对于隐式类型转换会提示 warning，但是 `clang` 需要显式添加 `-Wsign-conversion` 告警选项。
* 另外对于第三方库的告警，可以通过 [Diagnostic Pragmas](https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/Diagnostic-Pragmas.html#Diagnostic-Pragmas) 方式进行屏蔽。实际使用中，可以定义两个头文件 `no_warning_prefix.h` 和 `no_warning_suffix.h`，用于屏蔽第三方库的告警。

> GCC allows the user to selectively enable or disable certain types of diagnostics, and change the kind of the diagnostic. For example, a project's policy might require that all sources compile with -Werror but certain files might have exceptions allowing specific types of warnings. Or, a project might selectively enable diagnostics and treat them as errors depending on which preprocessor macros are defined.

关于编译器更多 [pragmas](https://gcc.gnu.org/onlinedocs/gcc/Pragmas.html#Pragmas) 的用法：

GCC supports several types of `pragmas`, primarily in order to compile code originally written for other compilers. Note that in general we do not recommend the use of `pragmas`; See [Function Attributes](https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html#Function-Attributes), for further explanation.

The GNU C preprocessor recognizes several `pragmas` in addition to the compiler `pragmas` documented here. Refer to the CPP manual for more information.

• [AArch64 Pragmas](https://gcc.gnu.org/onlinedocs/gcc/AArch64-Pragmas.html#AArch64-Pragmas)
• [ARM Pragmas](https://gcc.gnu.org/onlinedocs/gcc/ARM-Pragmas.html#ARM-Pragmas)
• [M32C Pragmas](https://gcc.gnu.org/onlinedocs/gcc/M32C-Pragmas.html#M32C-Pragmas)
• [PRU Pragmas](https://gcc.gnu.org/onlinedocs/gcc/PRU-Pragmas.html#PRU-Pragmas)
• [RS/6000 and PowerPC Pragmas](https://gcc.gnu.org/onlinedocs/gcc/RS_002f6000-and-PowerPC-Pragmas.html#RS_002f6000-and-PowerPC-Pragmas)
• [S/390 Pragmas](https://gcc.gnu.org/onlinedocs/gcc/S_002f390-Pragmas.html#S_002f390-Pragmas)
• [Darwin Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Darwin-Pragmas.html#Darwin-Pragmas)
• [Solaris Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Solaris-Pragmas.html#Solaris-Pragmas)
• [Symbol-Renaming Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Symbol-Renaming-Pragmas.html#Symbol-Renaming-Pragmas)
• [Structure-Layout Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Structure-Layout-Pragmas.html#Structure-Layout-Pragmas)
• [Weak Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Weak-Pragmas.html#Weak-Pragmas)
• [Diagnostic Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html#Diagnostic-Pragmas)
• [Visibility Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Visibility-Pragmas.html#Visibility-Pragmas)
• [Push/Pop Macro Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Push_002fPop-Macro-Pragmas.html#Push_002fPop-Macro-Pragmas)
• [Function Specific Option Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Function-Specific-Option-Pragmas.html#Function-Specific-Option-Pragmas)
• [Loop-Specific Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Loop-Specific-Pragmas.html#Loop-Specific-Pragmas)



示例代码：https://wandbox.org/permlink/U4CUnRy09abxOSHy

``` cpp
#include <iostream>
#include <stdint.h>

int main()
{
#if defined(__GNUC__) && !defined(__clang__) && !defined(__apple_build_version__)
#  if (__GNUC__ * 100 + __GNUC_MINOR__ * 10) >= 460
#    pragma GCC diagnostic push
#  endif
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wtype-limits"
#elif defined(__clang__) || defined(__apple_build_version__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wtype-limits"
#endif

  auto a = (uint32_t(0) < -1);
  std::cout << std::boolalpha <<  a << std::endl;

#if defined(__GNUC__) && !defined(__clang__) && !defined(__apple_build_version__)
#  if (__GNUC__ * 100 + __GNUC_MINOR__ * 10) >= 460
#    pragma GCC diagnostic pop
#  endif
#elif defined(__clang__) || defined(__apple_build_version__)
#  pragma clang diagnostic pop
#endif

  auto b = (uint32_t(0) < -1);
  std::cout << std::boolalpha <<  b << std::endl;

}
```

# 类型隐式转换 wraparound 问题

## 问题描述

在循环比较判断中，出现 unsigned 类型隐士转换，无符号变量出现 wraparound，导致死循环。

``` cpp
uint32 uFieldNumFromCfg = 256;

for (uint8 i = 0; i < uFieldNumFromCfg; ++i)
{
  // Oops! Dead loop ...
}
```

类似的问题：https://stackoverflow.com/questions/48272745/how-to-generate-a-warning-on-type-conversion-like-int32-int64


``` cpp
auto x = uint8_t(0);
auto y = uint32_t(1);
//x = y;               // gcc -Werror=conversion
auto c = (x < y);      // no warning
std::cout << std::boolalpha <<  c << std::endl;
```

## 问题分析

* 通过编译器产生告警，gcc 和 clang 都不会针对 x < y 这种比较的形式产生告警。g++ prog.cc -Wall -Wextra -std=c++11
* 参考 [How to get a warning in GCC for unsigned integer overflow instead of wrap-around?](https://stackoverflow.com/questions/54837695/how-to-get-a-warning-in-gcc-for-unsigned-integer-overflow-instead-of-wrap-around)  对于无符号类型的 [wraparound](https://wiki.sei.cmu.edu/confluence/display/c/BB.+Definitions#BB.Definitions-unsignedintegerwrapping) 是合法的，这种情况不属于 overflow，因此，编译器不会告警。
* 无符号类型可能发生 [unsigned integer wrapping](https://wiki.sei.cmu.edu/confluence/display/c/BB.+Definitions#BB.Definitions-unsignedintegerwrapping) 的场景。
* clang https://clang.llvm.org/docs/DiagnosticsReference.html#diagnostic-flags

![unsigned-integer-wrapping](/assets/images/202211/unsigned-integer-wrapping.png)


> unsigned integer wrapping 的含义：
>
> Computation involving unsigned operands whose result is reduced modulo the number that is one greater than the largest value that can be represented by the resulting type.


> Overflow of signed integers invokes undefined behavior, while unsigned integer overflow is well defined.
>
> For unsigned integers, overflow occurs as if the values were computed modulo one more than the maximum value of the given type. Put another way, if the type is n bits wide, then only the low order n bits of the result are retained. This is not actually overflow but is referred to as wraparound.

This is spelled out in [section 6.5p9](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf):

> The range of nonnegative values of a signed integer type is a subrange of the corresponding unsigned integer type, and the representation of the same value in each type is the same. A computation involving unsigned operands can never overflow, because a result that cannot be represented by the resulting unsigned integer type is reduced modulo the number that is one greater than the largest value that can be represented by the resulting type.

Because this behavior is well defined, it doesn't make sense for the compiler to trigger a warning.

In the case of the example:

``` cpp
long x = 2147483647U * 3U;
```

The multiplication is done on unsigned types, so the mathematical result 6442450941 wraps around to 2147483645, which is within the range of a long. There's no overflow (just wraparound) and no out-of-range conversion, so no warning.


## 优化方法

参考 [INT30-C. Ensure that unsigned integer operations do not wrap](https://wiki.sei.cmu.edu/confluence/display/c/INT30-C.+Ensure+that+unsigned+integer+operations+do+not+wrap) 的方法。


Noncompliant Code Example:

``` cpp
void func(unsigned int ui_a, unsigned int ui_b) {
  unsigned int usum = ui_a + ui_b;
  /* ... */
}
```

This noncompliant code example can result in an unsigned integer wrap during the addition of the unsigned operands ui_a and ui_b. If this behavior is unexpected, the resulting value may be used to allocate insufficient memory for a subsequent operation or in some other manner that can lead to an exploitable vulnerability.

Compliant Solution (Precondition Test):

This compliant solution performs a precondition test of the operands of the addition to guarantee there is no possibility of unsigned wrap.


``` cpp
#include <limits.h>

void func(unsigned int ui_a, unsigned int ui_b) {
unsigned int usum;
if (UINT_MAX - ui_a < ui_b) {
/* Handle error */
} else {
usum = ui_a + ui_b;
}
/* ... */
}
```


Compliant Solution (Postcondition Test):

This compliant solution performs a postcondition test to ensure that the result of the unsigned addition operation usum is not less than the first operand.

``` cpp
void func(unsigned int ui_a, unsigned int ui_b) {
  unsigned int usum = ui_a + ui_b;
  if (usum < ui_a) {
    /* Handle error */
  }
  /* ... */
}
```


# [Order of evaluation](https://en.cppreference.com/w/cpp/language/eval_order) (未定义行为)


Order of evaluation of any part of any expression, including order of evaluation of function arguments is **unspecified** (with some exceptions listed below). The compiler can evaluate operands and other subexpressions in any order, and may choose another order when the same expression is evaluated again.

> There is no concept of left-to-right or right-to-left evaluation in C++.

``` cpp
#include <cstdio>

int a() { return std::puts("a"); }
int b() { return std::puts("b"); }
int c() { return std::puts("c"); }

void z(int, int, int) {}

int main()
{
    z(a(), b(), c());       // all 6 permutations of output are allowed
    return a() + b() + c(); // all 6 permutations of output are allowed
}
```

Possible output:

```
b
c
a
c
a
b
```

## Undefined behavior

* If a side effect on a memory location is unsequenced relative to another side effect on the same memory location, [the behavior is undefined](https://en.cppreference.com/w/cpp/language/ub).

``` cpp
i = ++i + 2;       // well-defined
i = i++ + 2;       // undefined behavior until C++17
f(i = -2, i = -2); // undefined behavior until C++17
f(++i, ++i);       // undefined behavior until C++17, unspecified after C++17
i = ++i + i++;     // undefined behavior
```

* If a side effect on a memory location is unsequenced relative to a value computation using the value of any object in the same memory location, [the behavior is undefined](https://en.cppreference.com/w/cpp/language/ub).

``` cpp
cout << i << i++; // undefined behavior until C++17
a[i] = i++;       // undefined behavior until C++17
n = ++i + i;      // undefined behavior
```

## 业务代码中遇到的错误场景


``` cpp
#include <iostream>

const char* f(const char* s)
{
    static std::string* g_pStr = NULL;
    if (NULL == g_pStr)
    {
        g_pStr = new std::string;
    }

    *g_pStr = std::string(s);
    return g_pStr->c_str();
}

const char* g(const char* s)
{
    static std::string str;
    str = s;
    return str.c_str();
}

void print(const char* s1, const char* s2)
{
    std::cout << s1 << " " << s2 << std::endl;
}

int main()
{
    print(f("a"), f("b"));
    print(g("a"), g("b"));
}
```

输出结果对比：(未定义行为)

```
  // -std=c++11

  // f()
  // gcc 4.8.5: a b
  // gcc 5.1.0:  a a
  // clang 3.5.2: b b
  // clang 11.0.0: b b

  // g()
  // gcc 4.8.5: a a
  // gcc 5.1.0:  a a
  // clang 3.5.2: b b
  // clang 11.0.0: b b



  // -std=c++17

  // f()
  // gcc 5.1.0:  a a
  // clang 3.5.2: b b
  // clang 11.0.0: b b


  // g()
  // gcc 5.1.0:  a a
  // clang 3.5.2: b b
  // clang 11.0.0:  b b
```

修改为：

``` cpp
#include <iostream>
#include <string>

std::string f(const char* s)
{
    return s;
}

void print(std::string s1, std::string s2)
{
    std::cout << s1 << " " << s2 << std::endl;
}

int main()
{
    print(f("a"), f("b"));
}
```

# Other

## [Understanding "corrupted size vs. prev_size" glibc error](https://stackoverflow.com/questions/49628615/understanding-corrupted-size-vs-prev-size-glibc-error)

First of all - A practical cause to "corrupted size vs. prev_size" is quite simple - memory chunk control structure fields in the adjacent following chunk are being overwritten due to out-of-bounds access by the code. if you allocate x bytes for pointer p but wind up writing beyond x in regards to the same pointer, you might get this error, indicating the current memory allocation (chunk) size is not the same as what's found in the next chunk control structure (due to it being overwritten).

As for the cause for this memory leak - structure mapping done in the Java/JNA layer implied different `#pragma` related padding/alignment from what dll/so was compiled with. This in turn, caused data to be written beyond the allocated structure boundary. Disabling that alignment made the issues go away. (Thousands of executions without a single crash!).





