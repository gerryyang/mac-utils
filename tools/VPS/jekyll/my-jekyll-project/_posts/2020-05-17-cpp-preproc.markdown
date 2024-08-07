---
layout: post
title:  "CPP Preproc"
date:   2020-05-17 18:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# 最佳实践

* 预处理阶段编程的操作目标是**源码**，用各种指令控制预处理器，把源码改造成另一种形式，就像是捏橡皮泥一样。
* **包含文件**`#include`，作用是包含文件，注意可以包含**任何文件**。
* 在书写头文件时，通常使用`Include Guard`机制，即`#ifndef/#define/#endif`，来保证当前的头文件只会被包含一次。
* **宏定义**`#define/#undef`，用来定义一个源码级别的**文本替换**。使用宏的时候一定要谨慎，时刻记着以简化代码、清晰易懂为目标，不要“滥用”，避免导致源码混乱不堪，降低可读性。
  + 因为宏的展开、替换发生在预处理阶段，不涉及函数调用、参数传递、指针寻址，没有任何运行期的效率损失，所以对于一些调用频繁的小代码片段来说，用宏来封装的效果比 `inline` 关键字要更好。
  + 宏是没有作用域概念的，永远是全局生效。所以，对于一些用来简化代码、起临时作用的宏，最好是用完后尽快用`#undef`取消定义，避免冲突的风险。例如：使用宏定义的类型作用域是“全局的”，而使用`using`声明的类型是有作用域限制的。
  + 可以适当使用宏来定义代码中的常量，消除**魔术数字**（magic number）。
  + 用宏来代替直接定义名字空间。

``` cpp
// 通过 #define 指令定义一个宏：#define NAME_OF_MACRO value
#define BUFFER_SIZE 1024
auto foo = (char *)malloc(BUFFER_SIZE);
```

* **条件编译**`#if/#else/#endif`。可以在预处理阶段实现分支处理，通过判断宏的数值来产生不同的源码，改变源文件的形态，这就是**条件编译**。
  + 通常编译环境都会有一些预定义宏，比如 CPU 支持的特殊指令集、操作系统 / 编译器 / 程序库的版本、语言特性等，使用它们就可以早于运行阶段，提前在预处理阶段做出各种优化，产生出最适合当前系统的源码。
  + 与优化更密切相关的底层系统信息在 C++ 语言标准里没有定义，但编译器通常都会提供，比如 GCC 可以使用一条简单的命令查看`g++ -E -dM - < /dev/null`。基于它们，你就可以更精细地根据具体的语言、编译器、系统特性来改变源码，有，就用新特性；没有，就采用变通实现。

``` cpp
// Here, expression is an expression of integer type (can be integers, characters, arithmetic expression, macros and so on).
#if expression
   // conditional codes
#endif

#if expression
   // conditional codes if expression is non-zero
#else
   // conditional if expression is 0
#endif

#if expression
    // conditional codes if expression is non-zero
#elif expression1
    // conditional codes if expression is non-zero
#elif expression2
    // conditional codes if expression is non-zero
#else
    // conditional if all expressions are 0
#endif

// Here, the conditional codes are included in the program only if MACRO is defined.
#ifdef MACRO
   // conditional codes
#endif

// The special operator #defined is used to test whether a certain macro is defined or not. It's often used with #if directive.
#if defined BUFFER_SIZE && BUFFER_SIZE >= 2048
// code

// https://en.cppreference.com/w/cpp/error/assert
#ifdef NDEBUG
#  define assert(condition) ((void)0)
#else
#  define assert(condition) /*implementation defined*/
#endif
```

* Variadic Macros (可变参宏)


https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

https://en.wikipedia.org/wiki/Variadic_macro_in_the_C_preprocessor


``` cpp
#ifdef DEBUG_THRU_UART0
#   define DEBUG(...)  printString (__VA_ARGS__)
#else
void dummyFunc(void);
#   define DEBUG(...)  dummyFunc()
#endif
DEBUG(1,2,3); //calls printString(1,2,3) or dummyFunc() depending on
              //-DDEBUG_THRU_UART0 compiler define was given or not, when compiling.
```

`##__VA_ARGS__`的作用？

``` cpp
#define FOO(...)       printf(__VA_ARGS__)
#define BAR(fmt, ...)  printf(fmt, __VA_ARGS__)

FOO("this works fine");
BAR("this breaks!");
```

refer:

* https://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick
* https://stackoverflow.com/questions/26053959/what-does-va-args-in-a-macro-mean

* C++17引入了一个新的预处理工具`__has_include`，可以检查文件是否存在，注意不是包含。
* 有的编译器支持指令`#pragma once`，也可以实现`Include Guard`，但是它是非标准的，不推荐使用。
* C++20新增了模块`module`特性，可以实现一次性加载，但`Include Guard`在短期内还是无可替代的。
* 使用`boost.preprocessor`库可以实现复杂的预处理元编程，它提供分支，迭代等基本语言结构，甚至还有数组，链表等容器。

# 用法示例

``` cpp
#ifndef _XXX_H_INCLUDED_
#define _XXX_H_INCLUDED_

// 头文件内容

#endif // _XXX_H_INCLUDED_

// 或者，使用编译器的特性（非跨平台）：
_Pragma("once")


// 参考nginx代码
#define ngx_tolower(c)      ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define ngx_toupper(c)      ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

#define ngx_memzero(buf, n)       (void) memset(buf, 0, n)


#define CUBE(a) (a) * (a) * (a)  // 定义一个简单的求立方的宏

cout << CUBE(10) << endl;        // 使用宏简化代码
cout << CUBE(15) << endl;        // 使用宏简化代码

#undef CUBE                      // 使用完毕后立即取消定义


#ifdef AUTH_PWD                  // 检查是否已经有宏定义
#  undef AUTH_PWD                // 取消宏定义
#endif                           // 宏定义检查结束
#define AUTH_PWD "xxx"           // 重新宏定义


#define MAX_BUF_LEN    65535

// 用宏来代替直接定义名字空间

#define BEGIN_NAMESPACE(x)  namespace x {
#define END_NAMESPACE(x)    }

BEGIN_NAMESPACE(my_own)

...      // functions and classes

END_NAMESPACE(my_own)

// 条件编译

#ifdef __cplusplus                      // 定义了这个宏就是在用C++编译
    extern "C" {                        // 函数按照C的方式去处理
#endif
    void a_c_function(int a);
#ifdef __cplusplus                      // 检查是否是C++编译
    }                                   // extern "C" 结束
#endif

#if __cplusplus >= 201402                // 检查C++标准的版本号
    cout << "c++14 or later" << endl;    // 201402就是C++14
#elif __cplusplus >= 201103              // 检查C++标准的版本号
    cout << "c++11 or before" << endl;   // 201103是C++11
#else   // __cplusplus < 201103          // 199711是C++98
#   error "c++ is too old"               // 太低则预处理报错
#endif  // __cplusplus >= 201402         // 预处理语句结束

// ## 字符串拼接

#define print(x) print ## x
print(f)("hello\n"); // printf("hello\n");

```



``` cpp
$ g++ -E -dM - < /dev/null
#define __SSP_STRONG__ 3
#define __DBL_MIN_EXP__ (-1021)
#define __FLT32X_MAX_EXP__ 1024
#define __UINT_LEAST16_MAX__ 0xffff
#define __ATOMIC_ACQUIRE 2
#define __FLT128_MAX_10_EXP__ 4932
#define __FLT_MIN__ 1.17549435082228750796873653722224568e-38F
#define __GCC_IEC_559_COMPLEX 2
#define __UINT_LEAST8_TYPE__ unsigned char
#define __SIZEOF_FLOAT80__ 16
#define __INTMAX_C(c) c ## L
#define __CHAR_BIT__ 8
#define __UINT8_MAX__ 0xff
#define __WINT_MAX__ 0xffffffffU
#define __FLT32_MIN_EXP__ (-125)
#define __ORDER_LITTLE_ENDIAN__ 1234
#define __SIZE_MAX__ 0xffffffffffffffffUL
#define __WCHAR_MAX__ 0x7fffffff
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 1
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 1
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 1
#define __DBL_DENORM_MIN__ ((double)4.94065645841246544176568792868221372e-324L)
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8 1
#define __GCC_ATOMIC_CHAR_LOCK_FREE 2
#define __GCC_IEC_559 2
#define __FLT32X_DECIMAL_DIG__ 17
#define __FLT_EVAL_METHOD__ 0
#define __unix__ 1
#define __FLT64_DECIMAL_DIG__ 17
#define __GCC_ATOMIC_CHAR32_T_LOCK_FREE 2
#define __x86_64 1
#define __UINT_FAST64_MAX__ 0xffffffffffffffffUL
#define __SIG_ATOMIC_TYPE__ int
#define __DBL_MIN_10_EXP__ (-307)
#define __FINITE_MATH_ONLY__ 0
#define __GNUC_PATCHLEVEL__ 0
#define __FLT32_HAS_DENORM__ 1
#define __UINT_FAST8_MAX__ 0xff
#define __has_include(STR) __has_include__(STR)
#define __DEC64_MAX_EXP__ 385
#define __INT8_C(c) c
#define __INT_LEAST8_WIDTH__ 8
#define __UINT_LEAST64_MAX__ 0xffffffffffffffffUL
#define __SHRT_MAX__ 0x7fff
#define __LDBL_MAX__ 1.18973149535723176502126385303097021e+4932L
#define __FLT64X_MAX_10_EXP__ 4932
#define __UINT_LEAST8_MAX__ 0xff
#define __GCC_ATOMIC_BOOL_LOCK_FREE 2
#define __FLT128_DENORM_MIN__ 6.47517511943802511092443895822764655e-4966F128
#define __UINTMAX_TYPE__ long unsigned int
#define __linux 1
#define __DEC32_EPSILON__ 1E-6DF
#define __FLT_EVAL_METHOD_TS_18661_3__ 0
#define __unix 1
#define __UINT32_MAX__ 0xffffffffU
#define __LDBL_MAX_EXP__ 16384
#define __FLT128_MIN_EXP__ (-16381)
#define __WINT_MIN__ 0U
#define __linux__ 1
#define __FLT128_MIN_10_EXP__ (-4931)
#define __INT_LEAST16_WIDTH__ 16
#define __SCHAR_MAX__ 0x7f
#define __FLT128_MANT_DIG__ 113
#define __WCHAR_MIN__ (-__WCHAR_MAX__ - 1)
#define __INT64_C(c) c ## L
#define __DBL_DIG__ 15
#define __GCC_ATOMIC_POINTER_LOCK_FREE 2
#define __FLT64X_MANT_DIG__ 64
#define __SIZEOF_INT__ 4
#define __SIZEOF_POINTER__ 8
#define __USER_LABEL_PREFIX__
#define __FLT64X_EPSILON__ 1.08420217248550443400745280086994171e-19F64x
#define __STDC_HOSTED__ 1
#define __LDBL_HAS_INFINITY__ 1
#define __FLT32_DIG__ 6
#define __FLT_EPSILON__ 1.19209289550781250000000000000000000e-7F
#define __SHRT_WIDTH__ 16
#define __LDBL_MIN__ 3.36210314311209350626267781732175260e-4932L
#define __STDC_UTF_16__ 1
#define __DEC32_MAX__ 9.999999E96DF
#define __FLT64X_DENORM_MIN__ 3.64519953188247460252840593361941982e-4951F64x
#define __FLT32X_HAS_INFINITY__ 1
#define __INT32_MAX__ 0x7fffffff
#define __INT_WIDTH__ 32
#define __SIZEOF_LONG__ 8
#define __STDC_IEC_559__ 1
#define __STDC_ISO_10646__ 201706L
#define __UINT16_C(c) c
#define __PTRDIFF_WIDTH__ 64
#define __DECIMAL_DIG__ 21
#define __FLT64_EPSILON__ 2.22044604925031308084726333618164062e-16F64
#define __gnu_linux__ 1
#define __INTMAX_WIDTH__ 64
#define __has_include_next(STR) __has_include_next__(STR)
#define __FLT64X_MIN_10_EXP__ (-4931)
#define __LDBL_HAS_QUIET_NAN__ 1
#define __FLT64_MANT_DIG__ 53
#define __GNUC__ 7
#define __pie__ 2
#define __MMX__ 1
#define __FLT_HAS_DENORM__ 1
#define __SIZEOF_LONG_DOUBLE__ 16
#define __BIGGEST_ALIGNMENT__ 16
#define __FLT64_MAX_10_EXP__ 308
#define __DBL_MAX__ ((double)1.79769313486231570814527423731704357e+308L)
#define __INT_FAST32_MAX__ 0x7fffffffffffffffL
#define __DBL_HAS_INFINITY__ 1
#define __DEC32_MIN_EXP__ (-94)
#define __INTPTR_WIDTH__ 64
#define __FLT32X_HAS_DENORM__ 1
#define __INT_FAST16_TYPE__ long int
#define __LDBL_HAS_DENORM__ 1
#define __FLT128_HAS_INFINITY__ 1
#define __DEC128_MAX__ 9.999999999999999999999999999999999E6144DL
#define __INT_LEAST32_MAX__ 0x7fffffff
#define __DEC32_MIN__ 1E-95DF
#define __DBL_MAX_EXP__ 1024
#define __WCHAR_WIDTH__ 32
#define __FLT32_MAX__ 3.40282346638528859811704183484516925e+38F32
#define __DEC128_EPSILON__ 1E-33DL
#define __SSE2_MATH__ 1
#define __ATOMIC_HLE_RELEASE 131072
#define __PTRDIFF_MAX__ 0x7fffffffffffffffL
#define __amd64 1
#define __STDC_NO_THREADS__ 1
#define __ATOMIC_HLE_ACQUIRE 65536
#define __FLT32_HAS_QUIET_NAN__ 1
#define __LONG_LONG_MAX__ 0x7fffffffffffffffLL
#define __SIZEOF_SIZE_T__ 8
#define __FLT64X_MIN_EXP__ (-16381)
#define __SIZEOF_WINT_T__ 4
#define __LONG_LONG_WIDTH__ 64
#define __FLT32_MAX_EXP__ 128
#define __GCC_HAVE_DWARF2_CFI_ASM 1
#define __GXX_ABI_VERSION 1011
#define __FLT_MIN_EXP__ (-125)
#define __FLT64X_HAS_QUIET_NAN__ 1
#define __INT_FAST64_TYPE__ long int
#define __FLT64_DENORM_MIN__ 4.94065645841246544176568792868221372e-324F64
#define __DBL_MIN__ ((double)2.22507385850720138309023271733240406e-308L)
#define __PIE__ 2
#define __LP64__ 1
#define __FLT32X_EPSILON__ 2.22044604925031308084726333618164062e-16F32x
#define __DECIMAL_BID_FORMAT__ 1
#define __FLT64_MIN_EXP__ (-1021)
#define __FLT64_MIN_10_EXP__ (-307)
#define __FLT64X_DECIMAL_DIG__ 21
#define __DEC128_MIN__ 1E-6143DL
#define __REGISTER_PREFIX__
#define __UINT16_MAX__ 0xffff
#define __DBL_HAS_DENORM__ 1
#define __FLT32_MIN__ 1.17549435082228750796873653722224568e-38F32
#define __UINT8_TYPE__ unsigned char
#define __NO_INLINE__ 1
#define __FLT_MANT_DIG__ 24
#define __LDBL_DECIMAL_DIG__ 21
#define __VERSION__ "7.3.0"
#define __UINT64_C(c) c ## UL
#define _STDC_PREDEF_H 1
#define __GCC_ATOMIC_INT_LOCK_FREE 2
#define __FLT128_MAX_EXP__ 16384
#define __FLT32_MANT_DIG__ 24
#define __FLOAT_WORD_ORDER__ __ORDER_LITTLE_ENDIAN__
#define __STDC_IEC_559_COMPLEX__ 1
#define __FLT128_HAS_DENORM__ 1
#define __FLT128_DIG__ 33
#define __SCHAR_WIDTH__ 8
#define __INT32_C(c) c
#define __DEC64_EPSILON__ 1E-15DD
#define __ORDER_PDP_ENDIAN__ 3412
#define __DEC128_MIN_EXP__ (-6142)
#define __FLT32_MAX_10_EXP__ 38
#define __INT_FAST32_TYPE__ long int
#define __UINT_LEAST16_TYPE__ short unsigned int
#define __FLT64X_HAS_INFINITY__ 1
#define unix 1
#define __INT16_MAX__ 0x7fff
#define __SIZE_TYPE__ long unsigned int
#define __UINT64_MAX__ 0xffffffffffffffffUL
#define __FLT64X_DIG__ 18
#define __INT8_TYPE__ signed char
#define __ELF__ 1
#define __GCC_ASM_FLAG_OUTPUTS__ 1
#define __FLT_RADIX__ 2
#define __INT_LEAST16_TYPE__ short int
#define __LDBL_EPSILON__ 1.08420217248550443400745280086994171e-19L
#define __UINTMAX_C(c) c ## UL
#define __SSE_MATH__ 1
#define __k8 1
#define __SIG_ATOMIC_MAX__ 0x7fffffff
#define __GCC_ATOMIC_WCHAR_T_LOCK_FREE 2
#define __SIZEOF_PTRDIFF_T__ 8
#define __FLT32X_MANT_DIG__ 53
#define __x86_64__ 1
#define __FLT32X_MIN_EXP__ (-1021)
#define __DEC32_SUBNORMAL_MIN__ 0.000001E-95DF
#define __INT_FAST16_MAX__ 0x7fffffffffffffffL
#define __FLT64_DIG__ 15
#define __UINT_FAST32_MAX__ 0xffffffffffffffffUL
#define __UINT_LEAST64_TYPE__ long unsigned int
#define __FLT_HAS_QUIET_NAN__ 1
#define __FLT_MAX_10_EXP__ 38
#define __LONG_MAX__ 0x7fffffffffffffffL
#define __FLT64X_HAS_DENORM__ 1
#define __DEC128_SUBNORMAL_MIN__ 0.000000000000000000000000000000001E-6143DL
#define __FLT_HAS_INFINITY__ 1
#define __UINT_FAST16_TYPE__ long unsigned int
#define __DEC64_MAX__ 9.999999999999999E384DD
#define __INT_FAST32_WIDTH__ 64
#define __CHAR16_TYPE__ short unsigned int
#define __PRAGMA_REDEFINE_EXTNAME 1
#define __SIZE_WIDTH__ 64
#define __SEG_FS 1
#define __INT_LEAST16_MAX__ 0x7fff
#define __DEC64_MANT_DIG__ 16
#define __INT64_MAX__ 0x7fffffffffffffffL
#define __UINT_LEAST32_MAX__ 0xffffffffU
#define __SEG_GS 1
#define __FLT32_DENORM_MIN__ 1.40129846432481707092372958328991613e-45F32
#define __GCC_ATOMIC_LONG_LOCK_FREE 2
#define __SIG_ATOMIC_WIDTH__ 32
#define __INT_LEAST64_TYPE__ long int
#define __INT16_TYPE__ short int
#define __INT_LEAST8_TYPE__ signed char
#define __STDC_VERSION__ 201112L
#define __DEC32_MAX_EXP__ 97
#define __INT_FAST8_MAX__ 0x7f
#define __FLT128_MAX__ 1.18973149535723176508575932662800702e+4932F128
#define __INTPTR_MAX__ 0x7fffffffffffffffL
#define linux 1
#define __FLT64_HAS_QUIET_NAN__ 1
#define __FLT32_MIN_10_EXP__ (-37)
#define __SSE2__ 1
#define __FLT32X_DIG__ 15
#define __LDBL_MANT_DIG__ 64
#define __DBL_HAS_QUIET_NAN__ 1
#define __FLT64_HAS_INFINITY__ 1
#define __FLT64X_MAX__ 1.18973149535723176502126385303097021e+4932F64x
#define __SIG_ATOMIC_MIN__ (-__SIG_ATOMIC_MAX__ - 1)
#define __code_model_small__ 1
#define __k8__ 1
#define __INTPTR_TYPE__ long int
#define __UINT16_TYPE__ short unsigned int
#define __WCHAR_TYPE__ int
#define __SIZEOF_FLOAT__ 4
#define __pic__ 2
#define __UINTPTR_MAX__ 0xffffffffffffffffUL
#define __INT_FAST64_WIDTH__ 64
#define __DEC64_MIN_EXP__ (-382)
#define __FLT32_DECIMAL_DIG__ 9
#define __INT_FAST64_MAX__ 0x7fffffffffffffffL
#define __GCC_ATOMIC_TEST_AND_SET_TRUEVAL 1
#define __FLT_DIG__ 6
#define __FLT32_HAS_INFINITY__ 1
#define __FLT64X_MAX_EXP__ 16384
#define __UINT_FAST64_TYPE__ long unsigned int
#define __INT_MAX__ 0x7fffffff
#define __amd64__ 1
#define __INT64_TYPE__ long int
#define __FLT_MAX_EXP__ 128
#define __ORDER_BIG_ENDIAN__ 4321
#define __DBL_MANT_DIG__ 53
#define __SIZEOF_FLOAT128__ 16
#define __INT_LEAST64_MAX__ 0x7fffffffffffffffL
#define __GCC_ATOMIC_CHAR16_T_LOCK_FREE 2
#define __DEC64_MIN__ 1E-383DD
#define __WINT_TYPE__ unsigned int
#define __UINT_LEAST32_TYPE__ unsigned int
#define __SIZEOF_SHORT__ 2
#define __SSE__ 1
#define __LDBL_MIN_EXP__ (-16381)
#define __FLT64_MAX__ 1.79769313486231570814527423731704357e+308F64
#define __WINT_WIDTH__ 32
#define __INT_LEAST8_MAX__ 0x7f
#define __FLT32X_MAX_10_EXP__ 308
#define __SIZEOF_INT128__ 16
#define __LDBL_MAX_10_EXP__ 4932
#define __ATOMIC_RELAXED 0
#define __DBL_EPSILON__ ((double)2.22044604925031308084726333618164062e-16L)
#define __FLT128_MIN__ 3.36210314311209350626267781732175260e-4932F128
#define _LP64 1
#define __UINT8_C(c) c
#define __FLT64_MAX_EXP__ 1024
#define __INT_LEAST32_TYPE__ int
#define __SIZEOF_WCHAR_T__ 4
#define __UINT64_TYPE__ long unsigned int
#define __FLT128_HAS_QUIET_NAN__ 1
#define __INT_FAST8_TYPE__ signed char
#define __FLT64X_MIN__ 3.36210314311209350626267781732175260e-4932F64x
#define __GNUC_STDC_INLINE__ 1
#define __FLT64_HAS_DENORM__ 1
#define __FLT32_EPSILON__ 1.19209289550781250000000000000000000e-7F32
#define __DBL_DECIMAL_DIG__ 17
#define __STDC_UTF_32__ 1
#define __INT_FAST8_WIDTH__ 8
#define __FXSR__ 1
#define __DEC_EVAL_METHOD__ 2
#define __FLT32X_MAX__ 1.79769313486231570814527423731704357e+308F32x
#define __UINT32_C(c) c ## U
#define __INTMAX_MAX__ 0x7fffffffffffffffL
#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#define __FLT_DENORM_MIN__ 1.40129846432481707092372958328991613e-45F
#define __INT8_MAX__ 0x7f
#define __LONG_WIDTH__ 64
#define __PIC__ 2
#define __UINT_FAST32_TYPE__ long unsigned int
#define __CHAR32_TYPE__ unsigned int
#define __FLT_MAX__ 3.40282346638528859811704183484516925e+38F
#define __INT32_TYPE__ int
#define __SIZEOF_DOUBLE__ 8
#define __FLT_MIN_10_EXP__ (-37)
#define __FLT64_MIN__ 2.22507385850720138309023271733240406e-308F64
#define __INT_LEAST32_WIDTH__ 32
#define __INTMAX_TYPE__ long int
#define __DEC128_MAX_EXP__ 6145
#define __FLT32X_HAS_QUIET_NAN__ 1
#define __ATOMIC_CONSUME 1
#define __GNUC_MINOR__ 3
#define __INT_FAST16_WIDTH__ 64
#define __UINTMAX_MAX__ 0xffffffffffffffffUL
#define __DEC32_MANT_DIG__ 7
#define __FLT32X_DENORM_MIN__ 4.94065645841246544176568792868221372e-324F32x
#define __DBL_MAX_10_EXP__ 308
#define __LDBL_DENORM_MIN__ 3.64519953188247460252840593361941982e-4951L
#define __INT16_C(c) c
#define __STDC__ 1
#define __PTRDIFF_TYPE__ long int
#define __ATOMIC_SEQ_CST 5
#define __UINT32_TYPE__ unsigned int
#define __FLT32X_MIN_10_EXP__ (-307)
#define __UINTPTR_TYPE__ long unsigned int
#define __DEC64_SUBNORMAL_MIN__ 0.000000000000001E-383DD
#define __DEC128_MANT_DIG__ 34
#define __LDBL_MIN_10_EXP__ (-4931)
#define __FLT128_EPSILON__ 1.92592994438723585305597794258492732e-34F128
#define __SIZEOF_LONG_LONG__ 8
#define __FLT128_DECIMAL_DIG__ 36
#define __GCC_ATOMIC_LLONG_LOCK_FREE 2
#define __FLT32X_MIN__ 2.22507385850720138309023271733240406e-308F32x
#define __LDBL_DIG__ 18
#define __FLT_DECIMAL_DIG__ 9
#define __UINT_FAST16_MAX__ 0xffffffffffffffffUL
#define __GCC_ATOMIC_SHORT_LOCK_FREE 2
#define __INT_LEAST64_WIDTH__ 64
#define __UINT_FAST8_TYPE__ unsigned char
#define __ATOMIC_ACQ_REL 4
#define __ATOMIC_RELEASE 3
```


``` cpp
#if defined(__cpp_decltype_auto)        //检查是否支持decltype(auto)
    cout << "decltype(auto) enable" << endl;
#else
    cout << "decltype(auto) disable" << endl;
#endif  //__cpp_decltype_auto

#if __GNUC__ <= 4
    cout << "gcc is too old" << endl;
#else   // __GNUC__ > 4
    cout << "gcc is good enough" << endl;
#endif  // __GNUC__ <= 4

#if defined(__SSE4_2__) && defined(__x86_64)
    cout << "we can do more optimization" << endl;
#endif  // defined(__SSE4_2__) && defined(__x86_64)


```

# Tips

## `__FUNCTION__ / __func__ / __PRETTY_FUNCTION__` (predefined identifiers, not macros)

[__func__ or __FUNCTION__ or manual const char* id?](https://stackoverflow.com/questions/7008485/func-or-function-or-manual-const-char-id)

The predefined identifier `__func__` was added to the 1999 ISO C standard; the older 1990 C standard doesn't have it.

Support for `__func__` in pre-C99 compilers will depend on which compiler you're using.

Modern versions of gcc support `__func__` even in C90 mode (-ansi or -std=c89).

Before `__func__` was added to the standard, gcc implemented its own equivalent extension, but with the name `__FUNCTION__`. (gcc also supports `__PRETTY_FUNCTION__`, which is identical to `__func__` and `__FUNCTION__` for C, **but provides more information in C++**.)

The [gcc manual](https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/Function-Names.html) gives some advice:

> `__FUNCTION__` is another name for `__func__`. Older versions of GCC recognize only this name. However, it is not standardized. For maximum portability, we recommend you use `__func__`, but provide a fallback definition with the preprocessor:

Note that since `__func__` and `__FUNCTION__` **are predefined identifiers, not macros**, you only need the `#define` once, not in each function. On the other hand, you **can't use** `#ifdef __func__` or `#ifdef __FUNCTION__` to detect the level of support.

``` cpp
 #if __STDC_VERSION__ < 199901L
 # if __GNUC__ >= 2
 #  define __func__ __FUNCTION__
 # else
 #  define __func__ "<unknown>"
 # endif
 #endif
```

测试代码

``` cpp
#include <iostream>

template<typename T>
void func(T&& param)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::cout << param << std::endl;
}

void f()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::cout << __FUNCTION__ << std::endl;
}

int main()
{
    f();

    int a{100};
    func(a); // ok
    func(100); // ok
}
/*
void f()
f
void func(T&&) [with T = int&]
100
void func(T&&) [with T = int]
100
*/
```

``` cpp
#include <iostream>

void myFunction(int a, double b)
{
    std::cout << "Function name: " << __func__ << std::endl;
}

int main()
{
    myFunction(42, 3.14);
    return 0;
}
// Function name: myFunction
```

## 通过编译期提取类名和函数名

``` cpp
#include <iostream>
#include <string_view>

constexpr std::string_view extractClassNameAndFunctionName(std::string_view prettyFunction)
{
    auto start = prettyFunction.find(" ");
    auto end = prettyFunction.find("(");
    return prettyFunction.substr(start + 1, end - start - 1);
}

class A
{
public:
    void myFunction(int a, double b)
    {
            constexpr std::string_view funcName = extractClassNameAndFunctionName(__PRETTY_FUNCTION__);
            std::cout << "Function name: " << funcName << std::endl;
    }
};

int main()
{
    A a;
    a.myFunction(1, 2.0);
}
/*
Function name: A::myFunction
*/
```


# 测试代码

``` cpp
// demo.cpp
#include <iostream>

int main()
{
#
#if __linux__
#   define HAS_LINUX 1
#endif
#
  std::cout << "HAS_LINUX: " << HAS_LINUX << std::endl;
  return 0;
}
```

执行`g++ -E demo.cpp -o demo.cxx`，可以得到预处理后的cpp代码，然后查看`demo.cxx`(文件比较大，28160L, 665409C)

可以看到，

* `# 229 "/usr/include/x86_64-linux-gnu/c++/7/bits/c++config.h" 3`表示将`bits/c++config.h`第229行包含展开在这里。
* `533 "/usr/include/x86_64-linux-gnu/c++/7/bits/c++config.h" 3`表示将`bits/c++config.h`第533行的`#include <bits/os_defines.h>`，通过`#include`包含展开另一个文件的内容。


``` cpp
// ...

# 229 "/usr/include/x86_64-linux-gnu/c++/7/bits/c++config.h" 3
namespace std
{
  typedef long unsigned int size_t;
  typedef long int ptrdiff_t;


  typedef decltype(nullptr) nullptr_t;

}
# 251 "/usr/include/x86_64-linux-gnu/c++/7/bits/c++config.h" 3
namespace std
{
  inline namespace __cxx11 __attribute__((__abi_tag__ ("cxx11"))) { }
}
namespace __gnu_cxx
{
  inline namespace __cxx11 __attribute__((__abi_tag__ ("cxx11"))) { }


# 533 "/usr/include/x86_64-linux-gnu/c++/7/bits/c++config.h" 3
# 1 "/usr/include/x86_64-linux-gnu/c++/7/bits/os_defines.h" 1 3
# 39 "/usr/include/x86_64-linux-gnu/c++/7/bits/os_defines.h" 3
# 1 "/usr/include/features.h" 1 3 4

// ...

# 3 "demo.cpp"
int main()
{
 std::cout << "HAS_LINUX: " << 1 << std::endl;

 return 0;
}
```




下面以`bits/c++config.h`为例，看下其内容。

* `/usr/include/x86_64-linux-gnu/c++/7/bits/c++config.h`是`GNU ISO C++ Library`的一部分文件，定义了C++的符号和宏（`Predefined symbols and macros -*- C++ -*-`）
* `_GLIBCXX_RELEASE`对应`gcc -v`查看的gcc版本号


``` cpp
/** @file bits/c++config.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{iosfwd}
 */

#ifndef _GLIBCXX_CXX_CONFIG_H
#define _GLIBCXX_CXX_CONFIG_H 1

// The major release number for the GCC release the C++ library belongs to.
#define _GLIBCXX_RELEASE 7

// The datestamp of the C++ library in compressed ISO date format.
#define __GLIBCXX__ 20180720

// Macros for various attributes.
//   _GLIBCXX_PURE
//   _GLIBCXX_CONST
//   _GLIBCXX_NORETURN
//   _GLIBCXX_NOTHROW
//   _GLIBCXX_VISIBILITY
#ifndef _GLIBCXX_PURE
# define _GLIBCXX_PURE __attribute__ ((__pure__))
#endif

#ifndef _GLIBCXX_CONST
# define _GLIBCXX_CONST __attribute__ ((__const__))
#endif

#ifndef _GLIBCXX_NORETURN
# define _GLIBCXX_NORETURN __attribute__ ((__noreturn__))
#endif

// See below for C++
#ifndef _GLIBCXX_NOTHROW
# ifndef __cplusplus
#  define _GLIBCXX_NOTHROW __attribute__((__nothrow__))
# endif
#endif

// ...


// Macros for deprecated attributes.
//   _GLIBCXX_USE_DEPRECATED
//   _GLIBCXX_DEPRECATED
#ifndef _GLIBCXX_USE_DEPRECATED
# define _GLIBCXX_USE_DEPRECATED 1
#endif

#if defined(__DEPRECATED) && (__cplusplus >= 201103L)
# define _GLIBCXX_DEPRECATED __attribute__ ((__deprecated__))
#else
# define _GLIBCXX_DEPRECATED
#endif

// ...

#if __cplusplus

// Macro for constexpr, to support in mixed 03/0x mode.
#ifndef _GLIBCXX_CONSTEXPR
# if __cplusplus >= 201103L
#  define _GLIBCXX_CONSTEXPR constexpr
#  define _GLIBCXX_USE_CONSTEXPR constexpr
# else
#  define _GLIBCXX_CONSTEXPR
#  define _GLIBCXX_USE_CONSTEXPR const
# endif
#endif

#ifndef _GLIBCXX14_CONSTEXPR
# if __cplusplus >= 201402L
#  define _GLIBCXX14_CONSTEXPR constexpr
# else
#  define _GLIBCXX14_CONSTEXPR
# endif
#endif

#ifndef _GLIBCXX17_CONSTEXPR
# if __cplusplus > 201402L
#  define _GLIBCXX17_CONSTEXPR constexpr
# else
#  define _GLIBCXX17_CONSTEXPR
# endif
#endif

// ...

namespace std
{
  typedef __SIZE_TYPE__         size_t;
  typedef __PTRDIFF_TYPE__      ptrdiff_t;

#if __cplusplus >= 201103L
  typedef decltype(nullptr)     nullptr_t;
#endif
}

// ...

// First includes.

// Pick up any OS-specific definitions.
#include <bits/os_defines.h>

// Pick up any CPU-specific definitions.
#include <bits/cpu_defines.h>

// ...


#endif // _GLIBCXX_CXX_CONFIG_H
```




# Refer

* [The C Preprocessor](https://gcc.gnu.org/onlinedocs/gcc-3.0.2/cpp.html#SEC_Top)
* [C/C++ preprocessor reference](https://docs.microsoft.com/en-us/cpp/preprocessor/c-cpp-preprocessor-reference?view=msvc-160)
* [C/C++ 预处理器参考](https://learn.microsoft.com/zh-cn/cpp/preprocessor/c-cpp-preprocessor-reference?view=msvc-170)
* [THE C PREPROCESSOR](https://devarea.com/the-c-preprocessor/#.ZAmoiexBw0Q)