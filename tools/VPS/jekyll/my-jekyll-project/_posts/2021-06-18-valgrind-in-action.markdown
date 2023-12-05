---
layout: post
title:  "Valgrind in Action"
date:   2021-06-18 22:00:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Finding Memory Leaks With Valgrind

```
# Memcheck is the default tool. The --leak-check option turns on the detailed memory leak detector.
# Your program will run much slower (eg. 20 to 30 times) than normal, and use a lot more memory. Memcheck will issue messages about memory errors and leaks that it detects.

valgrind --leak-check=yes myprog arg1 arg2

--tool=<toolname> [default: memcheck]
Run the Valgrind tool called toolname, e.g. memcheck, cachegrind, callgrind, helgrind, drd, massif, dhat, lackey, none, exp-bbv, etc.
```

The Valgrind tool suite provides a number of debugging and profiling tools that help you make your programs faster and more correct. The most popular of these tools is called `Memcheck`. It can detect many memory-related errors that are common in C and C++ programs and that can lead to crashes and unpredictable behaviour.

Compile your program with `-g` to include debugging information so that Memcheck's error messages include exact line numbers. Using `-O0` is also a good idea, if you can tolerate the slowdown. With `-O1` line numbers in error messages can be inaccurate, although generally speaking running Memcheck on code compiled at `-O1` works fairly well, and the speed improvement compared to running `-O0` is quite significant. Use of `-O2` and above is not recommended as Memcheck occasionally reports uninitialised-value errors which don't really exist.

If you're running Linux and you don't have a copy already, you can get Valgrind from the [Valgrind download page](https://www.valgrind.org/downloads/current.html).

``` cpp
#include <stdlib.h>
void f(void)
{
    int* x = malloc(10 * sizeof(int));
    x[10] = 0;       // problem 1: heap block overrun
}                    // problem 2: memory leak -- x not freed
int main(void)
{
    f();
    return 0;
}
```

编译和内存泄漏检查：

```
gcc -g demo.c
valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --log-file=valgrind.out ./a.out
```

输出：

```
==2726743== Memcheck, a memory error detector
==2726743== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==2726743== Using Valgrind-3.17.0 and LibVEX; rerun with -h for copyright info
==2726743== Command: ./a.out
==2726743==
==2726743== Invalid write of size 4
==2726743==    at 0x40054B: f (demo.c:6)
==2726743==    by 0x40055B: main (demo.c:11)
==2726743==  Address 0x5629068 is 0 bytes after a block of size 40 alloc'd
==2726743==    at 0x4C2B067: malloc (vg_replace_malloc.c:380)
==2726743==    by 0x40053E: f (demo.c:5)
==2726743==    by 0x40055B: main (demo.c:11)
==2726743==
==2726743==
==2726743== HEAP SUMMARY:
==2726743==     in use at exit: 40 bytes in 1 blocks
==2726743==   total heap usage: 1 allocs, 0 frees, 40 bytes allocated
==2726743==
==2726743== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==2726743==    at 0x4C2B067: malloc (vg_replace_malloc.c:380)
==2726743==    by 0x40053E: f (demo.c:5)
==2726743==    by 0x40055B: main (demo.c:11)
==2726743==
==2726743== LEAK SUMMARY:
==2726743==    definitely lost: 40 bytes in 1 blocks
==2726743==    indirectly lost: 0 bytes in 0 blocks
==2726743==      possibly lost: 0 bytes in 0 blocks
==2726743==    still reachable: 0 bytes in 0 blocks
==2726743==         suppressed: 0 bytes in 0 blocks
==2726743==
==2726743== For lists of detected and suppressed errors, rerun with: -s
==2726743== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```

There are several kinds of leaks; the two most important categories are:

* "definitely lost": your program is leaking memory -- fix it!
* "probably lost": your program is leaking memory, unless you're doing funny things with pointers (such as moving them to point to the middle of a heap block).

`Memcheck` also reports uses of uninitialised values, most commonly with the message "Conditional jump or move depends on uninitialised value(s)". It can be difficult to determine the root cause of these errors. Try using the `--track-origins=yes` to get extra information. This makes Memcheck run slower, but the extra information you get often saves a lot of time figuring out where the uninitialised values are coming from.

`Memcheck` cannot detect every memory error your program has. For example, it can't detect out-of-range reads or writes to arrays that are allocated statically or on the stack. But it should detect many errors that could crash your program (eg. cause a segmentation fault).

# Helgrind (a thread error detector)

安装方法：

```
sudo yum install valgrind
```

Valgrind 是一个用于内存调试、内存泄漏检测和性能分析的工具。Helgrind 是 Valgrind 的一个工具，用于检测多线程程序中的同步错误。要使用 Helgrind，首先安装 Valgrind，然后使用以下命令运行程序：

```
valgrind --tool=helgrind ./my_program
```

Helgrind 会报告潜在的线程安全问题，如数据竞争、死锁等。



# Refer

* [The Valgrind Quick Start Guide](https://valgrind.org/docs/manual/quick-start.html)
* [Using Valgrind to Find Memory Leaks and Invalid Memory Use](https://www.cprogramming.com/debugging/valgrind.html)
* [The Valgrind Quick Start Guide](https://www.valgrind.org/docs/manual/QuickStart.html)
* [Valgrind Frequently Asked Questions](https://www.valgrind.org/docs/manual/faq.html)
* [Valgrind Documentation](https://www.valgrind.org/docs/manual/index.html)
* [Valgrind User Manual](https://www.valgrind.org/docs/manual/manual.html)
* [How to find memory leak in a C++ code/project?](https://stackoverflow.com/questions/6261201/how-to-find-memory-leak-in-a-c-code-project)
* [AddressSanitizerComparisonOfMemoryTools](https://github.com/google/sanitizers/wiki/AddressSanitizerComparisonOfMemoryTools/d06210f759fec97066888e5f27c7e722832b0924)
* [Valgrind's Tool Suite](https://valgrind.org/info/tools.html)
* [How to track down a "double free or corruption" error](https://stackoverflow.com/questions/2902064/how-to-track-down-a-double-free-or-corruption-error)


