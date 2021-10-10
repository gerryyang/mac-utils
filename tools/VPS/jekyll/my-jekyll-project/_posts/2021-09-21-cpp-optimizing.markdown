---
layout: post
title:  "CPP Optimzing"
date:   2021-09-21 08:30:00 +0800
categories: C/C++
---

* Do not remove this line (it will not be displayed)
{:toc}

C++性能优化实践

# TODO

1. 一个老生常谈的问题：为什么我们要用C++？ https://mk.woa.com/q/266081?newest=true
2. 锁的性能对比
3. 文章 字符串与数字的转换
4. 随机函数 glibc的random实现还加锁 https://mk.woa.com/q/122793 
5. 无锁map  基于tbb实现的线程安全的cache，不是锁整个map，而是对正在访问的node节点加自旋读写锁，锁冲突大大减小，代码也很简单。 https://mk.woa.com/q/124533
6. string_view https://mk.woa.com/q/275510?newest=true
7. 更高效的机器代码 https://mk.woa.com/q/275382?newest=true
8. FB/PB https://mk.woa.com/q/275178?newest=true
9. 内存墙
10. 日志开销

# 工具

* 在线反汇编工具：https://gcc.godbolt.org/


# 性能和易用性

例子：TDR(特指1.0版本) vs ProtocolBuffers

TDR 牺牲了易用性获取了高性能，而 ProtocolBuffers 通过部分性能开销换取了更好的易用性。

* 易用性
  + TDR 使用 LV(Length + Value) 的编码方式，通过版本剪裁方式来解决版本兼容，但只支持单向的高版本兼容低版本数据
  + ProtocolBuffers 使用 TLV(Tag + Length + Value) 的编码方式，支持前后双向兼容
  + TDR 对字段顺序有要求，而 ProtocolBuffers 不需要

* 性能
  + TDR 使用的编解码，不需要考虑 T 类型，故在效率和性能上，比 TLV 模式要高
  + ProtocolBuffers 使用了整型数据压缩，有符号整型二次压缩 (base 128 varints 及 zigzag编码) 以尽可能节省流量，但是会引入额外的 msb(most significant bit) 计算开销
  + TDR 的编码逻辑在 XML 协议确定后，一些计算可在编译期完成。而 ProtocolBuffers 需要在运行期根据 Descriptor 来确定


| 方案            | 消息的内存数据结构大小 | 序列化生成的网络消息大小 | 消息结构描述                                                    |
| --------------- | ---------------------- | ------------------------ | --------------------------------------------------------------- |
| TDR             | 1056 B                 | 984 B                    | `207 u/int32_t, 2 uint64_t, 1 float, 1 double, 1 char[33]`      |
| ProtocolBuffers | 动态分配               | 1290 B                   | `204 s/fixed32, 2 fixed64, 1 float, 1 double, 1 string(len:32)` |


| 目标函数               | TDR(Avg 次/秒) | ProtocolBuffers(Avg 次/秒) | TDR 性能增量 |
| ---------------------- | -------------- | -------------------------- | ------------ |
| SerializeToArray(pack) | 574736         | 385613                     | +49%         |
| ParseFromArray(unpack) | 528372         | 107431                     | +392%        |


取长补短，TDR2.0 引入了 metalib/enabletlv, id(`[1, 0xFFFFFFF]`) 新属性决定是否采用 TLV 模式，且由调用者决定整型数据是否使用变长编码，对比 TDR1.0 性能测试，TDR2.0 性能下降 10%-20%



# 编译优化

-O2编译优化

TODO

# 数字转换为字符串

* Before C++11：`std::stringstream`, `sprintf`
* After C++11: `std::to_string`(C++11), `std::to_chars`(C++17), `std::format`(C++20), `fmtlib`

``` cpp
// {fmt} is an open-source formatting library providing a fast and safe alternative to C stdio and C++ iostreams.
// Format string syntax similar to Python's format.
// https://github.com/fmtlib/fmt
// https://gcc.godbolt.org/z/rz6fncKn7
#include <vector>
#include <fmt/ranges.h>

int main() {
  std::string s1 = fmt::format("The answer is {}.", 42); // s1 == "The answer is 42."
  std::string s2 = fmt::format("I'd rather be {1} than {0}.", "right", "happy"); // s2 == "I'd rather be happy than right."

  std::vector<int> v = {1, 2, 3};
  fmt::print("{}\n", v); // {1, 2, 3}
}
```

`fmtlib`的benchmark数据：

| Library           | Method        | Run Time, s                    |
| ----------------- | ------------- | ------------------------------ |
| libc              | printf        | 1.04                           |
| libc++            | std::ostream  | 3.05                           |
| fmt 6.1.1         | fmt::print    | 0.75  (35% faster than printf) |
| Boost Format 1.67 | boost::format | 7.24                           |
| Folly Format      | folly::format | 2.23                           |


[dtoa Benchmark](https://github.com/fmtlib/dtoa-benchmark)

This benchmark evaluates the performance of conversion from double precision IEEE-754 floating point (double) to ASCII string. The function prototype is:

``` cpp
void dtoa(double value, char* buffer);
```

RandomDigit: Generates 1000 random double values, filtered out +/-inf and nan. Then convert them to limited precision (1 to 17 decimal digits in significand). Finally convert these numbers into ASCII. Each digit group is run for 100 times. The minimum time duration is measured for 10 trials.


| Function | Time (ns) | Speedup |
| -------- | --------- | ------- | 
| ostringstream |	1,187.735	| 0.75x |
| ostrstream |	1,048.512 |	0.85x |
| sprint |	887.735 |	1.00x |
| fpconv	| 119.024 |	7.46x |
| grisu2	| 101.082 |	8.78x |
| doubleconv (Google) |	84.359 |	10.52x |
| milo |	64.100	| 13.85x |
| ryu |	43.541 |	20.39x |
| fmt |	40.712	| 21.81x |
| null |	1.200 |	739.78x |


![randomdigit](/assets/images/202110/randomdigit.png)

> 1. Note that the null implementation does nothing. It measures the overheads of looping and function call.
> 
> 2. Why fast dtoa() functions is needed? They are a very common operations in writing data in text format. The standard way of sprintf(), std::stringstream, often provides poor performance. The author of this benchmark would optimize the sprintf implementation in RapidJSON. https://github.com/fmtlib/dtoa-benchmark/blob/master/src/milo/dtoa_milo.h


运行期和编译期计算对比：[https://gcc.godbolt.org/z/G6jfdcxqr](https://gcc.godbolt.org/z/G6jfdcxqr)

测试结果：

| Method                | Time (ns) | Speedup |
| --------------------- | --------- | ------- |
| std::stringstream     | 4136931   | 1x      |
| sprintf               | 738063    | 4.6x    |
| std::to_string(C++11) | 69180     | 58.8x   |
| std::to_chars(c++17)  | 42589     | 96.1x   |
| fmt::format           | 128076    | 31.3x   |
| fmt::format_int       | 24807     | 165.8x  |
| std::format(C++20)    | N/A       | N/A     |
| macro_string          | 399       | 10,367x |
| compile_string        | 425       | 9,733x  |

* `to_chars`(c++17) 操作的是 stack-allocated buffer，而`fmt::format`返回的是`std::string` 使用了堆内存。
* `to_chars`(c++17) 需要一次额外的内存拷贝，而`fmt::fmt_int`的内存由对象自己管理不需要拷贝。
* `fmt::fmt_int` > `std::to_chars`(C++17) > `std::to_string`(C++11)

```cpp
std::array<char, std::numeric_limits<int>::digits10 + 2> buffer;
auto result = std::to_chars(buffer.data(),
                            buffer.data() + buffer.size(), number);
if (result.ec == std::errc()) {
  std::string result(buffer.data(), result.ptr); // Copy the data into string.
  // Use result.
} else {
  // Handle the error.
}

auto f = fmt::format_int(42);
// f.data() is the data, f.size() is the size
```

``` cpp
namespace detail
{
	template<uint8_t... digits> struct positive_to_chars {
		static const char value[];
		static constexpr size_t size = sizeof...(digits);
	};
	template<uint8_t... digits> const char positive_to_chars<digits...>::value[] = {('0' + digits)..., 0};

	template<uint8_t... digits> struct negative_to_chars {
		static const char value[];
	};
	template<uint8_t... digits> const char negative_to_chars<digits...>::value[] = {'-', ('0' + digits)..., 0};

	template<bool neg, uint8_t... digits>
		struct to_chars : positive_to_chars<digits...> {};

	template<uint8_t... digits>
		struct to_chars<true, digits...> : negative_to_chars<digits...> {};

	// 对 num 每位进行展开，例如，num = 123 则展开为 explode<neg, 0, 1, 2, 3>
	template<bool neg, uintmax_t rem, uint8_t... digits>
		struct explode : explode<neg, rem / 10, rem % 10, digits...> {};
	
	// 展开终止
	template<bool neg, uint8_t... digits>
		struct explode<neg, 0, digits...> : to_chars<neg, digits...> {};

	template<typename T>
		constexpr uintmax_t cabs(T num) {
			return (num < 0) ? -num : num;
		}
}

template<typename T, T num>
struct string_from : ::detail::explode<num < 0, ::detail::cabs(num)> {};

int main() 
{
    auto str = string_from<unsigned, 1>::value;
}
```

更详细的测试结果:

* [A collection of formatting benchmarks](https://github.com/fmtlib/format-benchmark/tree/d0d5e141df6a8f2e60d4ba3ea718415a00ca3e5b)
* [Converting a hundred million integers to strings per second](https://www.zverovich.net/2020/06/13/fast-int-to-string-revisited.html)
* [Facebook Tech Talk by Andrei Alexandrescu: Three Optimization Tips For C++](https://archive.org/details/AndreiAlexandrescu-Three-Optimization-Tips)

# 最小的64位ELF

程序1:

``` cpp
#include <cstdio>
int main()
{
    printf("Hello world");
    return 0;
}
```

g++ -Os example.cc

程序2:

``` asm
section .data
  some_string dq "Hello world"
  some_string_size dq 11

section .text
  global _start

_start:
  ; Print the string
  mov rax, 1                  ; sys_write 
  mov rdi, 1                  ; param 1, stdout
  mov rsi, some_string        ; param 2
  mov rdx, [some_string_size] ; param 3
  syscall

  ; Exit the program
  mov rax, 60  ; sys_exit
  mov rdi, 0   ; param 1
  syscall
```

nasm -f elf64 -o example.o example.asm
ld -o example example.o
strip example

对比：

| Program           | Size  | Size (stripped) |
| ----------------- | ----- | --------------- |
| example (asm)     | 984 B | 528 B           |
| example (cc, -Os) | 8.2K  | 6.0K            |

使用`nasm`写汇编，手动构造ELF格式文件。不使用c库，系统调用使用syscall指令(x86-64)：

+ 入参：
  * eax = 系统调用号，可以在 /usr/include/asm/unistd_64.h 文件中找到
  * rdi, rsi, rdx, r10, r8, r9 分别为第 1 至 6 个参数
+ 出参：
  * rax = 返回值（如果失败，返回 -errno）
  * rcx, r11 被破坏（它们分别被 syscall 指令用来保存返回地址和 rflags）
  * 其他寄存器的值保留

更多：极客技术挑战赛[实现一个世界上最小的程序来输出自身的MD5](https://mk.woa.com/contest/3/content)，其中，[最小实现方案为299字节](https://mk.woa.com/q/262510/answer/54941)，[实现代码](https://git.woa.com/ricdli/mk-md5-challenge)。

优化策略：

* 预先计算中间状态，减少代码分支
* 避免 copy（修改`text`段为可写）
* 分支之间共用指令，代码共用
* 指令选择（早期x86的指令，使用的字节较少。例如：`lodsb` 只要一个字节，而 `mov al,[rsi]; inc rsi` 要多一些，可通过`objdump -d`比较）
* 寄存器的选择（优先使用从32位时代沿袭下来的8个寄存器 (`rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp`)，避免使用`r8-r15`。对于很多指令，使用`r8-r15`会多占一个字节）
* 用最短的代码赋值小常量给寄存器
  + 对0值，对寄存器异或自身：`xor eax, eax`
  + 对非0值，例如，将常量1赋给`eax`（方法1使用5个字节，而方法2只使用3个字节）

方法1：
```
  4000b0:       b8 01 00 00 00          mov    $0x1,%eax
```

方法2：8086 指令
```
  4000b0:       6a 01                   pushq  $0x1
  4000b2:       58                      pop    %rax
```

* 32位 or 64位寄存器（x86-64有一个特性，目标操作数是32位寄存器时，会同时清空对应64位寄存器的高32位。所以，`xor eax, eax`等价`xor rax, rax`，`mov eax, 1`等价`mov rax, 1`）。使用32位寄存器通常可以省1个字节（`r8-r15`除外），但注意，在作为地址时，情况相反，64位少1个字节。
* 进程初始状态（简化OS的初始化操作）
* 使用16位地址（修改进程初始虚拟地址`/proc/sys/vm/mmap_min_addr`）
* `pext`指令（位运算[BMI 2指令](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set)）
* 寻址优化
* 调用函数（call指令）


# Branch predictor（Instruction pipelining）

GCC内置函数[__builtin_expect](https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html)提供了分支预测（[Branch predictor](https://en.wikipedia.org/wiki/Branch_predictor)）的能力，从而有助于CPU执行[Instruction pipelining](https://en.wikipedia.org/wiki/Instruction_pipelining)优化，其中，exp 为 integral expressions，__builtin_expect 为 true 的默认概率为 90%（依赖 -O2，而 -O0 不会优化）。

> long __builtin_expect (long exp, long c) 
> long __builtin_expect_with_probability(long exp, long c, double probability)

4阶段的Instruction pipeline（fetch, decode, execute and write-back），其中，每个彩色格子表示相互独立的指令。

Four stages pipeline: fetch, decode, execute and write-back. The top gray box is the list of instructions waiting to be executed, the bottom gray box is the list of instructions that have had their execution completed, and the middle white box is the pipeline.

![instruction-pipelining](/assets/images/202109/instruction-pipelining.png)

![instruction-pipelining2](/assets/images/202109/instruction-pipelining2.png)

C++20支持了[C++ attribute: likely, unlikely](https://en.cppreference.com/w/cpp/language/attributes/likely)分支预测的属性设置，帮助编译器实现优化。

``` cpp
if (a > b) [[likely]] {
  do_something();
} else {
  do_other();
}

switch (x) {
  case 1:
    f();
    break;
  case 2:
    [[likely]] g();
    break;
  default:
    h();
}
```

https://usingstdcpp.org/2018/03/18/jacksonville18-iso-cpp-report/

例如：

``` cpp
// long __builtin_expect (long exp, long c)
// we do not expect to call foo, since we expect x to be zero
if (__builtin_expect (x, 0)) {
  foo();
}

if (__builtin_expect (ptr != NULL, 1)) {
  foo (*ptr);
}
```

为了方便使用，Linux内核代码（[include/linux/compiler.h](https://github.com/torvalds/linux/blob/master/include/linux/compiler.h)）定义了两个接口。

``` cpp
// expect x is true
# define likely(x)	__builtin_expect(!!(x), 1)
// expect x is false
# define unlikely(x)	__builtin_expect(!!(x), 0)
```

环境 x86-64 gcc 8.4.0

g++ -O2 a.cc
objdump -d a.out


程序1:

``` cpp
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <ctime>

__attribute__ ((noinline)) int func(int a)
{
  if (a) {
    usleep(1000); // 1ms
  } else {
    a = time(NULL);
  }
  return 0;
}

int main()
{ 
  auto a = std::rand();
  func(a);
  return 0;
}
```

``` asm
0000000000000710 <main>:
 710:   55                      push   %rbp
 711:   53                      push   %rbx
 712:   bb e8 03 00 00          mov    $0x3e8,%ebx
 717:   48 83 ec 08             sub    $0x8,%rsp
 71b:   e8 90 ff ff ff          callq  6b0 <rand@plt>
 720:   89 c5                   mov    %eax,%ebp
 722:   66 0f 1f 44 00 00       nopw   0x0(%rax,%rax,1)
 728:   89 ef                   mov    %ebp,%edi
 72a:   e8 31 01 00 00          callq  860 <_Z4funci>
 72f:   83 eb 01                sub    $0x1,%ebx
 732:   75 f4                   jne    728 <main+0x18>
 734:   48 83 c4 08             add    $0x8,%rsp
 738:   31 c0                   xor    %eax,%eax
 73a:   5b                      pop    %rbx
 73b:   5d                      pop    %rbp
 73c:   c3                      retq   
 73d:   0f 1f 00                nopl   (%rax)

 0000000000000860 <_Z4funci>:
 860:   48 83 ec 08             sub    $0x8,%rsp
 864:   85 ff                   test   %edi,%edi
 866:   74 18                   je     880 <_Z4funci+0x20>
 868:   bf e8 03 00 00          mov    $0x3e8,%edi
 86d:   e8 6e fe ff ff          callq  6e0 <usleep@plt>
 872:   31 c0                   xor    %eax,%eax
 874:   48 83 c4 08             add    $0x8,%rsp
 878:   c3                      retq   
 879:   0f 1f 80 00 00 00 00    nopl   0x0(%rax)
 880:   31 ff                   xor    %edi,%edi
 882:   e8 49 fe ff ff          callq  6d0 <time@plt>
 887:   31 c0                   xor    %eax,%eax
 889:   48 83 c4 08             add    $0x8,%rsp
 88d:   c3                      retq   
 88e:   66 90                   xchg   %ax,%ax
```

程序2: 

``` cpp
#define LIKELY(x)  __builtin_expect(!!(x), 1)
#define UNLIKELY(x)  __builtin_expect(!!(x), 0)

__attribute__ ((noinline)) int func(int a)
{
  if (UNLIKELY(a)) {
    usleep(1000); // 1ms
  } else {
    a = time(NULL);
  }
  return 0;
}
```

``` asm
0000000000000850 <_Z4funci>:
 850:   48 83 ec 08             sub    $0x8,%rsp
 854:   85 ff                   test   %edi,%edi
 856:   75 18                   jne    870 <_Z4funci+0x20>
 858:   31 ff                   xor    %edi,%edi
 85a:   e8 71 fe ff ff          callq  6d0 <time@plt>
 85f:   31 c0                   xor    %eax,%eax
 861:   48 83 c4 08             add    $0x8,%rsp
 865:   c3                      retq   
 866:   66 2e 0f 1f 84 00 00    nopw   %cs:0x0(%rax,%rax,1)
 86d:   00 00 00 
 870:   bf e8 03 00 00          mov    $0x3e8,%edi
 875:   e8 66 fe ff ff          callq  6e0 <usleep@plt>
 87a:   eb e3                   jmp    85f <_Z4funci+0xf>
 87c:   0f 1f 40 00             nopl   0x0(%rax)
```

关于性能的测试比较:

[How much do __builtin_expect(), likely(), and unlikely() improve performance?](http://blog.man7.org/2012/10/how-much-do-builtinexpect-likely-and.html)

https://man7.org/linux/tests/gcc/builtin_expect_test.c

``` cpp
/* builtin_expect_test.c */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE 1000000

#ifndef BINCR
#define BINCR 1
#endif

#if defined(EXPECT_RESULT) && defined(DONT_EXPECT)
#error "Specifying both EXPECT_RESULT and DONT_EXPECT makes no sense"
#endif

/* We add some seemingly unneeded complexity to the code, simply to
   make the opimizer's task tricky enough that it won't optimize away
   the effect of __builtin_expect(). In this particular program, all
   of the following are needed:

        * Calling an *non-inline* function inside the loop in main().
        * Looping over an array in main() (rather than checking a
          single variable).
        * Dynamically allocating the array with calloc(), rather than
          declaring an array and initializing with memset().
        * Acting on two different variables (m1, m2) in each branch
          of the 'if' statement in main() (if the two branches after
          the 'if' execute the same code, gcc is clever enough to
          recognize this and optimize the 'if' away).
        * Printing the resulting values of the variables modified in
          the loop (otherwise gcc may optimize away the entire loop
          inside main()).

   Compile with at least -O2 (on x86) to see a difference in
   performance due to __builtin_expect().
*/

static __attribute__ ((noinline))  int
f(int a)
{
    return a;
}

int
main(int argc, char *argv[])
{
    int *p;
    int j, k, m1, m2, nloops;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s num-loops\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    m1 = m2 = 0;
    nloops = atoi(argv[1]);

    /* calloc() allocates an array and zeros its contents */

    p = calloc(BSIZE, sizeof(int));
    if (p == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

#if defined(BREAK_STEP) && BREAK_STEP > 0

    /* This provides us with a way to inject some values into the
       array that differ from our expected test value, in order
       to get an idea of how how much the __builtin_expect()
       optimization is negatively affected by unexpected values. */

    for (k = 0, j = 0; j < BSIZE; j += BREAK_STEP) {
        p[j] += BINCR;
        k++;
    }

    printf("Adjusted %d items by %d\n", k, BINCR);

#endif

    for (j = 0; j < nloops; j++) {
        for (k = 0; k < BSIZE; k++) {
#ifdef DONT_EXPECT
            if (p[k]) {
#else
            if (__builtin_expect(p[k], EXPECT_RESULT)) {
#endif
                m1 = f(++m1);
             } else {
                m2 = f(++m2);
             }
        }
    }

    printf("%d, %d\n", m1, m2);

    exit(EXIT_SUCCESS);
}
```


[The gcc documentation contains the following advice](http://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html) regarding the use of `__builtin_expect()`:

> In general, you should prefer to use actual profile feedback for this (-fprofile-arcs), as programmers are notoriously bad at predicting how their programs actually perform. However, there are applications in which this data is hard to collect.

That's good concise advice. To put things another way, the only time you should use `__builtin_expect()` is when you can't use compiler-assisted runtime optimization and you are certain that your predicted code path is very (very) likely to be the one that will be taken.

通过选项让编译器帮助实现优化：

Building the programming now involves two steps: **a profiling phase** and **an optimized compile**. 

> -fprofile-generate
> Enable options usually used for instrumenting application to produce profile useful for later recompilation with profile feedback based optimization. You must use -fprofile-generate both when compiling and when linking your program.
> To optimize the program based on the collected profile information, use -fprofile-use. 
> Enable profile feedback-directed optimizations, and the following optimizations, many of which are generally profitable only with profile feedback available:

```
-fbranch-probabilities  -fprofile-values 
-funroll-loops  -fpeel-loops  -ftracer  -fvpt 
-finline-functions  -fipa-cp  -fipa-cp-clone  -fipa-bit-cp 
-fpredictive-commoning  -fsplit-loops  -funswitch-loops 
-fgcse-after-reload  -ftree-loop-vectorize  -ftree-slp-vectorize 
-fvect-cost-model=dynamic  -ftree-loop-distribute-patterns 
-fprofile-reorder-functions
```

```
# profiling phase
g++ -O2 -fprofile-generate a.cc -o a.prof

# optimized compile 
g++ -O2 -fprofile-use a.cc -o a.opt
```

https://gcc.gnu.org/onlinedocs/gcc-10.1.0/gcc/Instrumentation-Options.html

https://gcc.gnu.org/onlinedocs/gcc-10.1.0/gcc/Optimize-Options.html#Optimize-Options

# gettimeofday

```
#include <cstdio>
#include <time.h>
#include <sys/time.h>
#include <ctime>
#include <chrono>

int main()
{
        struct timespec tp;
        struct timeval tv;
        int i = 0;
        int j = 0;
        for (i = 0; i < 1000000; ++i)
        {
                // vdso
                gettimeofday(&tv, NULL);
                j += tv.tv_usec % 2;

                // https://linux.die.net/man/3/clock_gettime

                // vdso
                //clock_gettime(CLOCK_REALTIME, &tp);
                //clock_gettime(CLOCK_MONOTONIC, &tp);
                //clock_gettime(CLOCK_REALTIME_COARSE, &tp);
                //clock_gettime(CLOCK_MONOTONIC_COARSE, &tp);

                // syscall
                //clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp);
                //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);

                // vdso
                //auto t = std::time(0);
                //printf("%u\n", t);

                // syscall
                //auto t = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
                //std::chrono::system_clock::now();
                std::chrono::steady_clock::now();

                j += tp.tv_sec % 2;
        }
        return 0;

}
```


# Refer

* [Computer performance](https://en.wikipedia.org/wiki/Computer_performance)
* [Make a program run slowly](https://stackoverflow.com/questions/14371257/make-a-program-run-slowly/14371416)
* [How much do __builtin_expect(), likely(), and unlikely() improve performance?](http://blog.man7.org/2012/10/how-much-do-builtinexpect-likely-and.html)
* [What is the advantage of GCC's __builtin_expect in if else statements?](https://stackoverflow.com/questions/7346929/what-is-the-advantage-of-gccs-builtin-expect-in-if-else-statements)
* [Other Built-in Functions Provided by GCC](https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html)
  

	
	