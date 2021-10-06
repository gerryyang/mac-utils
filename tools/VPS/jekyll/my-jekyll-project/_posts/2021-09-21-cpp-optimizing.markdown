---
layout: post
title:  "CPP Optimzing"
date:   2021-09-21 08:30:00 +0800
categories: C/C++
---

* Do not remove this line (it will not be displayed)
{:toc}

# TODO

1. 一个老生常谈的问题：为什么我们要用C++？ https://mk.woa.com/q/266081?newest=true
2. 锁的性能对比
3. 文章 字符串与数字的转换
4. 随机函数 glibc的random实现还加锁 https://mk.woa.com/q/122793 
5. 无锁map  基于tbb实现的线程安全的cache，不是锁整个map，而是对正在访问的node节点加自旋读写锁，锁冲突大大减小，代码也很简单。 https://mk.woa.com/q/124533
6. string_view https://mk.woa.com/q/275510?newest=true
7. 更高效的机器代码 https://mk.woa.com/q/275382?newest=true
8. FB/PB https://mk.woa.com/q/275178?newest=true

# 工具

* 在线反汇编工具：https://gcc.godbolt.org/


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

| Program | Size | Size (stripped)
| -- | -- | --
| example (asm) | 984 B | 528 B 
| example (cc, -Os) | 8.2K | 6.0K 

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
  

	
	