---
layout: post
title:  "CPP in Action"
date:   2019-05-06 17:00:00 +0800
categories: cpp
---

* Do not remove this line (it will not be displayed)
{:toc}

# 经验

1. 对于完成相同的功能，C++需要的代码行数一般是Python的三倍，而性能则可达到其十倍以上。
2. 当目标服务属于运算密集型或者内存密集型，且需要性能且愿意为性能付出额外代价的时候，应该考虑使用C++。 
3. C++是解决性能的利器，尤其在大公司和金融机构里。C++之父Bjarne Stroustrup目前就职的地方便是摩根斯坦利。
4. 学习C++就像学习一门外语，你不要期望能够掌握所有的单词和语法，因此需要多看多写掌握合适的“语感”，而不是记住所有规则。
5. Bjarne有一个洋葱理论，抽象层次就像一个洋葱，是层层嵌套的。如果想用较低的抽象层次表达较高的概念，就好比一次切过了很多层洋葱，会把自己的眼泪熏出来。因此主张学习应该自顶向下，先学习高层的抽象，再层层剥茧，丝丝入扣地一步步进入下层。

# 环境要求

* GCC 8.3
* Clang 9.0
* Visual Studio 2019 16.3 (MSVC 19.23)

# 核心概念

## 堆栈及布局

* 本地变量所需内存在栈上分配，和函数执行所需的其他数据在一起。当函数执行完这些内存也就自然释放掉。
    + 本地变量，包括简单类型`POD`(Plain Old Data)和非POD类型(有构造和析构的类型)，栈上的内存分配是一样的，只不过C++编译器会在生成代码的合适位置插入对构造和析构函数的调用
    + 编译器会自动调用析构函数，包括在函数执行发生异常时。在发生异常时对析构函数的调用，成为`栈展开(stack unwinding)`
    + 栈上的分配简单，移动栈指针而已
    + 栈上的释放也简单，函数执行结束时移动一下栈指针
    + 由于后进先出的执行过程，栈的分配不可能出现内存碎片

``` cpp
#include <cstdio>

class obj {
public:
    obj() { puts("obj()"); }
    ~obj() { puts("~obj()"); }
};

void foo(int n)
{
    obj o;
    if (n == 1) {
        throw "exception";
    }
}

int main()
{
    try {
        foo(0);
        foo(1);
    } catch (const char* err) {
        puts(err);
    }

}

/*
$ g++ -o stack_unwinding stack_unwinding.cpp 
$ ./stack_unwinding 
obj()
~obj()
obj()
~obj()
exception
*/
```


* 《程序员的自我修养》说栈的地址比堆高，栈是向下增长的，堆是向上增长的。但是在Windows上测试却不是。

``` cpp
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    double a0[4] = {1.2, 2.4, 3.6, 4.8};
    double a1[4] = {1.2, 2.4, 3.6, 4.8};

    vector<double> a2(4);
    vector<double> a3(4);

    a2[0] = 1.0/3.0;
    a2[1] = 1.0/5.0;
    a2[2] = 1.0/7.0;
    a2[3] = 1.0/9.0;
    a3[0] = 1.0/3.0;
    a3[1] = 1.0/5.0;
    a3[2] = 1.0/7.0;
    a3[3] = 1.0/9.0;

    cout << "a0[2]: " << a0[2] << " at " << &a0[2] << endl;
    cout << "a1[2]: " << a1[2] << " at " << &a1[2] << endl;
    cout << "a1[3]: " << a1[3] << " at " << &a1[3] << endl;
    cout << "a2[2]: " << a2[2] << " at " << &a2[2] << endl;
    cout << "a2[3]: " << a2[3] << " at " << &a2[3] << endl;
    cout << "a3[2]: " << a3[2] << " at " << &a3[2] << endl;
    cout << "a3[3]: " << a3[3] << " at " << &a3[3] << endl;

    return 0;
}
```

Windows的输出：

![windows_stack](/assets/images/201911/windows_stack.png)

Linux的输出：

```
a0[2]: 3.6 at 0x7ffe0bbce190
a1[2]: 3.6 at 0x7ffe0bbce170
a1[3]: 4.8 at 0x7ffe0bbce178
a2[2]: 0.142857 at 0x1b64020
a2[3]: 0.111111 at 0x1b64028
a3[2]: 0.142857 at 0x1b64050
a3[3]: 0.111111 at 0x1b64058
```

* `进程地址空间的分布`取决于`操作系统`，栈向什么方向增长取决于操作系统与CPU的组合。不要把别的操作系统的实现方式套用到Windows上。x86硬件直接支持的栈确实是“向下增长”的：push指令导致sp自减一个slot，pop指令导致sp自增一个slot。其它硬件有其它硬件的情况。

* 栈的增长方向与栈帧布局。这个上下文里说的“栈”是函数调用栈，是以“栈帧”（stack frame）为单位的。每一次函数调用会在栈上分配一个新的栈帧，在这次函数调用结束时释放其空间。被调用函数（callee）的栈帧相对调用函数（caller）的栈帧的位置反映了栈的增长方向：如果被调用函数的栈帧比调用函数的在更低的地址，那么栈就是向下增长；反之则是向上增长。

* 而在一个栈帧内，局部变量是如何分布到栈帧里的（所谓栈帧布局，stack frame layout），这完全是编译器的自由。至于数组元素与栈的增长方向：C与C++语言规范都规定了数组元素是分布在连续递增的地址上的。

> An array type describes a contiguously allocated nonempty set of objects with a particular member object type, called the element type. A postfix expression followed by an expression in square brackets [] is a subscripted designation of an element of an array object. The definition of the subscript operator [] is that E1[E2] is identical to (*((E1)+(E2))). Because of the conversion rules that apply to the binary + operator, if E1 is an array object (equivalently, a pointer to the initial element of an array object) and E2 is an integer, E1[E2] designates the E2-th element of E1 (counting from zero).

* 以简化的Linux/x86模型为例。在简化的32位Linux/x86进程地址空间模型里，（主线程的）栈空间确实比堆空间的地址要高——它已经占据了用户态地址空间的最高可分配的区域，并且向下（向低地址）增长。借用Gustavo Duarte的[Anatomy of a Program in Memory](https://link.zhihu.com/?target=http%3A//duartes.org/gustavo/blog/post/anatomy-of-a-program-in-memory/)里的图。

![linuxFlexibleAddressSpaceLayout](/assets/images/201911/linuxFlexibleAddressSpaceLayout.png)

* 虽然传统上Linux上的malloc实现会使用brk()/sbrk()来实现malloc()（这俩构成了上图中“Heap”所示的部分，这也是Linux自身所认为是heap的地方——用pmap看可以看到这里被标记为[heap]），但这并不是必须的——一个malloc()实现完全可以只用或基本上只用mmap()来实现malloc()，此时一般说的“Heap”（malloc-heap）就不一定在上图“Heap”（Linux heap）所示部分，而会在“Memory Mapping Segment”部分散布开来。不同版本的Linux在分配未指定起始地址的mmap()时用的顺序不一样，并不保证某种顺序。而且mmap()分配到的空间是有可能出现在低于主可执行程序映射进来的text Segment所在的位置。

* Linux上多线程进程中，“线程”其实是一组共享虚拟地址空间的进程。只有主线程的栈是按照上面图示分布，其它线程的栈的位置其实是“随机”的——它们可以由pthread_create()调用mmap()来分配，也可以由程序自己调用mmap()之后把地址传给pthread_create()。既然是mmap()来的，其它线程的栈出现在Memory Mapping Segment的任意位置都不出奇，与用于实现malloc()用的mmap()空间很可能是交错出现的。

* Windows的进程地址空间。Windows的进程地址空间分布跟上面说的简化的Linux/x86模型颇不一样。就算在没有ASLR的老Windows上也已经很不一样，有了ASLR之后就更加不一样了。在Windows上不应该对栈和堆的相对位置做任何假设。
要想看个清楚Windows的进程地址空间长啥样，可以用Sysinternals出品的[VMMap](https://link.zhihu.com/?target=https%3A//technet.microsoft.com/en-us/library/dd535533.aspx)看看。该工具简介请见：[VMMap - A Peek Inside Virtual Memory](https://link.zhihu.com/?target=http%3A//blogs.technet.com/b/askperf/archive/2010/01/29/vmmap-a-peek-inside-virtual-memory.aspx)。

refer:
* [堆、栈的地址高低？ 栈的增长方向？](https://www.zhihu.com/question/36103513)
* [Anatomy of a Program in Memory](https://manybutfinite.com/post/anatomy-of-a-program-in-memory/)

## RAII

RAII(Resource Acquisition Is Initialization)，是C++用于管理资源的方式。

问题示例，在不使用RAII的方法时存在的问题：
1. 若delete之前的代码出现异常时，导致delete无法执行从而产生内存泄露。
2. 不符合C++的惯用法，通常应该使用栈内存分配。

``` cpp
void foo()
{
    bar* ptr = new bar();
    
    // some operations and throw an exception

    delete ptr;
}
```


# STL

[range-for](https://zh.cppreference.com/w/cpp/language/range-for)

[lambda](https://zh.cppreference.com/w/cpp/language/lambda)

[std::chrono](https://blog.csdn.net/u013390476/article/details/50209603)


# 代码片段

## Time

## DateTime and UnixTime

``` cpp
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main()
{
        struct tm tm = {0};
        char *res = strptime("2019-10-01 18:00:00", "%Y-%m-%d %H:%M:%S", &tm);
        if (res == NULL) {
                printf("strptime err\n");
                return -1;
        }
        time_t t = mktime(&tm); 

        // equal to `date -d "2019-10-01 18:00:00" +%s`
        printf("t[%ld]\n", t);

        return 0;
}
```

``` cpp
int datetime_to_unixtime(std::string &end_time, time_t &expired_time, std::string &errinfo)
{
    struct tm tm = {0};
    // end_time's format is like "2019-10-01 18:00:00"
    char *res = strptime(end_time.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    if (res == NULL) {
        errinfo = "tc_subscribe ans, end_time is invalid";
        printf("%s", errinfo.c_str());
        return 1;
    }
    expired_time = mktime(&tm);

    return 0;
}

int get_unixtime(time_t &unixtime, std::string &datetime)
{
    unixtime = time(NULL);

    char buf[64] = {0};
    struct timeval tv;
    struct tm tm_obj;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm_obj);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_obj);

    datetime = buf;

    return 0;
}
```


refer:
* [strptime](https://linux.die.net/man/3/strptime)
* [mktime](http://www.cplusplus.com/reference/ctime/mktime/)
* [How to convert a string variable containing time to time_t type in c++?](https://stackoverflow.com/questions/11213326/how-to-convert-a-string-variable-containing-time-to-time-t-type-in-c/11213640#11213640)
* [Date/time conversion: string representation to time_t](https://stackoverflow.com/questions/321793/date-time-conversion-string-representation-to-time-t/321847#321847)


## Regex

[How do I write a regular expression that matches an IPv4 dotted address?](https://devblogs.microsoft.com/oldnewthing/20060522-08/?p=31113)

``` cpp
#include <boost/regex.hpp>

/*
作用: 使用boost库, 实现IPv4匹配
10.0.0.0 - 10.255.255.255
172.16.0.0 - 172.31.255.255
192.168.0.0 - 192.168.255.255
*/
void test_regex_ip()
{
	std::string ip[] = {
		std::string("10.0.0.1"),
		std::string("172.16.0.1"),
		std::string("192.168.0.1"),
		std::string("10.*.*.*"),
		std::string("172.16.*.*"),
		std::string("192.168.*.*")
	};

	for (int i = 0; i != sizeof(ip) / sizeof(ip[0]); ++i) {
		auto match = [&] {
			//boost::regex pattern("(\\d{1,3}(\\.\\d{1,3}){3})");
			//boost::regex pattern("(\\d{1,3}(\\..*){3})");
			boost::regex pattern("(172(\\.\\d{1,3}){2}(\\..*))");
			boost::smatch match;
			try {
				if (boost::regex_search(ip[i], match, pattern)) {
					std::cout << "ip: " << match[1] << std::endl;
				} else {
					std::cout << ip[i] << " did not match\n";
				}

			} catch (boost::regex_error &e) {
				std::cout << "err: " << e.what() << std::endl;
			}
		};

		match();
	}
}
```

# 编译器

[is-pragma-once-a-safe-include-guard](https://stackoverflow.com/questions/787533/is-pragma-once-a-safe-include-guard)


# 文档

1. [GCC online documentation]


[GCC online documentation]: https://gcc.gnu.org/onlinedocs/




