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


# 编译期计算

使用`enum hack`

``` cpp
#include <iostream>

template<int a, int b>
struct Add {
    enum {
        result = a + b
    };
};

int main()
{
    std::cout << Add<1, 2>::result << std::endl;
}
```

使用C++11的`constexpr`

``` cpp
#include <iostream>

template<typename T1, typename T2>
constexpr int add(T1 a, T2 b)
{
    return a + b;
}

int main()
{
    std::cout << add(1, 2) << std::endl;
}
```

## 数据对齐

配合现代编译器和CPU架构，可以让程序获得更好的性能。例如，CPU访问32位宽度的数据总线，就会期待数据是按照32位对齐，即4字节。这样CPU读取4字节的数据只需要对总线访问一次，但是如果要访问的数据并没有按照4字节对齐，那么CPU需要访问数据总线两次，运算速度自然就慢了。除了CPU之外，还有其他硬件也需要数据对齐，比如通过DMA访问硬盘，就要求内存必须是4K对齐的。

C++11之前的方法：使用`offsetof`宏获取member的偏移量，从而获取指定类型的对齐字节长度。

``` cpp
#include <iostream>

#define ALIGNOF(type, result) do {\
    struct alignof_trick{ char c; type member; }; \
    result = offsetof(alignof_trick, member); \
    } while (0);

typedef void (*f)(); 

int main()
{
    int len = 0;

    ALIGNOF(char, len);
    std::cout << len << std::endl; // 1
    
    ALIGNOF(int, len);
    std::cout << len << std::endl; // 4
    
    ALIGNOF(short, len);
    std::cout << len << std::endl; // 2
    
    ALIGNOF(float, len);
    std::cout << len << std::endl; // 4
    
    ALIGNOF(double, len);
    std::cout << len << std::endl; // 8
    
    ALIGNOF(long long, len);
    std::cout << len << std::endl; // 8

    ALIGNOF(f, len);
    std::cout << len << std::endl; // 8
}
```

更好的方法：

``` cpp
#include <iostream>

template<typename T>
struct alignof_trick
{
    char c;
    T member;
};

#define ALIGNOF(type) offsetof(alignof_trick<type>, member)

typedef void (*f)();

int main()
{
    std::cout << ALIGNOF(char) << std::endl; // 1
    std::cout << ALIGNOF(int) << std::endl; // 4
    std::cout << ALIGNOF(short) << std::endl; // 2
    std::cout << ALIGNOF(float) << std::endl; // 4
    std::cout << ALIGNOF(double) << std::endl; // 8
    std::cout << ALIGNOF(long long) << std::endl; // 8
    std::cout << ALIGNOF(f) << std::endl; // 8
}
```

使用编译器的扩展功能：

``` cpp
#include <iostream>

// GCC
#define ALIGNOF(type) __alignof__(type)

typedef void (*f)();

int main()
{
    std::cout << ALIGNOF(char) << std::endl; // 1
    std::cout << ALIGNOF(int) << std::endl; // 4
    std::cout << ALIGNOF(short) << std::endl; // 2
    std::cout << ALIGNOF(float) << std::endl; // 4
    std::cout << ALIGNOF(double) << std::endl; // 8
    std::cout << ALIGNOF(long long) << std::endl; // 8
    std::cout << ALIGNOF(f) << std::endl; // 8
}
```

设置数据对齐。

``` cpp
#include <iostream>

// GCC
#define ALIGNOF(type) __alignof__(type)
#define ALIGNAS(len) __attribute__((aligned(len))) 

int main()
{
    short x1;
    ALIGNAS(8) short x2; 
    std::cout << ALIGNOF(x1) << std::endl; // 2
    std::cout << ALIGNOF(x2) << std::endl; // 8
}
```

由于不同的编译器需要采用不同的扩展功能来控制类型的对齐字节长度，对可移植性不太友好，因此，C++11标准中新增了`alignof`和`alignas`两个关键字。

``` cpp
#include <iostream>

// GCC
#define ALIGNOF(type) alignof(type)
#define ALIGNAS(len) alignas(len)

int main()
{
    short x1;
    std::cout << ALIGNOF(short) << std::endl; // 2
    std::cout << ALIGNOF(decltype(x1)) << std::endl; // 2
    
    ALIGNAS(8) short x2; 
    std::cout << ALIGNOF(decltype(x2)) << std::endl; // 2 TODO
}
```

也提供了一些模板方法：例如，`std::alignment_of`

``` cpp
#include <iostream>

int main()
{
    std::cout << std::alignment_of<int>::value << std::endl; // 4
    std::cout << std::alignment_of<int>() << std::endl; // 4
}
```

性能测试对比：(10000000 * 10000 B = 100 GB)

| Case | Time (s) | Memory bandwidth (GB)
| -- | -- | --
| 1字节对齐 (memcpy) | 9.32968 | 10.7
| 32字节对齐 (memcpy) | 8.15827 | 12.3
| 1字节对齐 (rep movsb) | 2.45423 | 40.7
| 32字节对齐 (rep movsb) | 1.27497 | 78.4

> REP MOVSD/MOVSQ is the universal solution that works relatively well on all Intel processors for large memory blocks of at least 4KB (no ERMSB required) if the destination is aligned by at least 64 bytes. REP MOVSD/MOVSQ works even better on newer processors, starting from Skylake. And, for Ice Lake or newer microarchitectures, it works perfectly for even very small strings of at least 64 bytes. refer: https://stackoverflow.com/questions/43343231/enhanced-rep-movsb-for-memcpy

* https://stackoverflow.com/questions/43343231/enhanced-rep-movsb-for-memcpy
* https://groups.google.com/g/gnu.gcc.help/c/-Bmlm_EG_fE
* https://www-ssl.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-optimization-manual.html
* https://en.cppreference.com/w/cpp/memory/align

使用[tinymembench](https://github.com/ssvb/tinymembench)测试内存的性能：


```
$ lscpu
Architecture:        x86_64
CPU op-mode(s):      32-bit, 64-bit
Byte Order:          Little Endian
CPU(s):              1
On-line CPU(s) list: 0
Thread(s) per core:  1
Core(s) per socket:  1
Socket(s):           1
NUMA node(s):        1
Vendor ID:           GenuineIntel
CPU family:          6
Model:               63
Model name:          Intel(R) Xeon(R) CPU E5-26xx v3
Stepping:            2
CPU MHz:             2394.446
BogoMIPS:            4788.89
Hypervisor vendor:   KVM
Virtualization type: full
L1d cache:           32K
L1i cache:           32K
L2 cache:            4096K
NUMA node0 CPU(s):   0
Flags:               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ss ht syscall nx lm constant_tsc rep_good nopl cpuid pni pclmulqdq ssse3 fma cx16 pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand hypervisor lahf_lm abm pti bmi1 avx2 bmi2 xsaveopt
```

``` 
$ ./tinymembench 
tinymembench v0.4.9 (simple benchmark for memory throughput and latency)

==========================================================================
== Memory bandwidth tests                                               ==
==                                                                      ==
== Note 1: 1MB = 1000000 bytes                                          ==
== Note 2: Results for 'copy' tests show how many bytes can be          ==
==         copied per second (adding together read and writen           ==
==         bytes would have provided twice higher numbers)              ==
== Note 3: 2-pass copy means that we are using a small temporary buffer ==
==         to first fetch data into it, and only then write it to the   ==
==         destination (source -> L1 cache, L1 cache -> destination)    ==
== Note 4: If sample standard deviation exceeds 0.1%, it is shown in    ==
==         brackets                                                     ==
==========================================================================

 C copy backwards                                     :   4867.4 MB/s (2.8%)
 C copy backwards (32 byte blocks)                    :   4834.6 MB/s (1.9%)
 C copy backwards (64 byte blocks)                    :   4876.6 MB/s (1.2%)
 C copy                                               :   4832.9 MB/s (1.7%)
 C copy prefetched (32 bytes step)                    :   4581.1 MB/s (1.8%)
 C copy prefetched (64 bytes step)                    :   4522.8 MB/s (2.9%)
 C 2-pass copy                                        :   4372.2 MB/s (1.5%)
 C 2-pass copy prefetched (32 bytes step)             :   4465.1 MB/s (2.2%)
 C 2-pass copy prefetched (64 bytes step)             :   4431.3 MB/s (2.1%)
 C fill                                               :   8415.8 MB/s (3.1%)
 C fill (shuffle within 16 byte blocks)               :   8432.8 MB/s (3.2%)
 C fill (shuffle within 32 byte blocks)               :   8460.3 MB/s (3.4%)
 C fill (shuffle within 64 byte blocks)               :   8452.6 MB/s (2.9%)
 ---
 standard memcpy                                      :   9922.0 MB/s (3.0%)
 standard memset                                      :   8481.7 MB/s (5.3%)
 ---
 MOVSB copy                                           :   4314.7 MB/s (1.2%)
 MOVSD copy                                           :   4443.8 MB/s (1.0%)
 SSE2 copy                                            :   4783.6 MB/s (2.0%)
 SSE2 nontemporal copy                                :  10040.4 MB/s (4.7%)
 SSE2 copy prefetched (32 bytes step)                 :   4631.6 MB/s (1.7%)
 SSE2 copy prefetched (64 bytes step)                 :   4610.8 MB/s (3.3%)
 SSE2 nontemporal copy prefetched (32 bytes step)     :   9247.9 MB/s (2.4%)
 SSE2 nontemporal copy prefetched (64 bytes step)     :   9515.9 MB/s (3.4%)
 SSE2 2-pass copy                                     :   4538.2 MB/s (1.6%)
 SSE2 2-pass copy prefetched (32 bytes step)          :   4361.0 MB/s (2.0%)
 SSE2 2-pass copy prefetched (64 bytes step)          :   4341.9 MB/s (2.3%)
 SSE2 2-pass nontemporal copy                         :   3177.7 MB/s (1.9%)
 SSE2 fill                                            :   8482.4 MB/s (3.2%)
 SSE2 nontemporal fill                                :  16543.7 MB/s (0.8%)

```

``` cpp
#include <iostream>
#include <memory>
#include <chrono>

static inline void *__movsb(void *d, const void *s, size_t n)
{
    asm volatile ("rep movsb"
                  : "=D" (d),
                    "=S" (s),
                    "=c" (n)
                  : "0" (d),
                    "1" (s),
                    "2" (n)
                  : "memory");
    return d;
}

int main(int argc, char *argv[])
{
    constexpr int align_size = 32;
    constexpr int alloc_size = 10001;
    constexpr int buff_size = align_size + alloc_size;
    
    char dst[buff_size] = {0};
    char src[buff_size] = {0};
    
    void *dst_ori_ptr = dst;
    void *src_ori_ptr = src;
    
    size_t dst_size = sizeof(dst);
    size_t src_size = sizeof(src);
    
    char *dst_ptr = static_cast<char *>(std::align(align_size, alloc_size, dst_ori_ptr, dst_size));
    char *src_ptr = static_cast<char *>(std::align(align_size, alloc_size, src_ori_ptr, src_size));
    
    if (argc == 2 && argv[1][0] == '1') {
        ++dst_ptr;
        ++src_ptr;
    }
    
    auto beg = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; ++i) {
        __movsb(dst_ptr, src_ptr, alloc_size - 1);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - beg;
    std::cout << "elapsed time: " << diff.count() << std::endl;
}
```

# 性能和易用性

例子：TDR (特指1.0版本) vs ProtocolBuffers

TDR 牺牲了易用性获取了高性能，而 ProtocolBuffers 通过部分性能开销换取了更好的易用性。

* 易用性
  + TDR 使用 `LV` (Length + Value) 的编码方式，通过版本剪裁方式来解决版本兼容，但只支持单向的高版本兼容低版本数据
  + ProtocolBuffers 使用 `TLV` (Tag + Length + Value) 的编码方式，支持前后双向兼容
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



# 数字转换为字符串

* Before C++11：`std::stringstream`, `sprintf`
* After C++11: `std::to_string`(C++11), `std::to_chars`(C++17), `std::format`(C++20), `fmtlib`

其中，`std::format`目前还没有编译器支持，此处使用`fmtlib`代替。

* https://en.cppreference.com/w/cpp/compiler_support 
* https://stackoverflow.com/questions/65083544/format-no-such-file-or-directory

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


| Function            | Time (ns) | Speedup |
| ------------------- | --------- | ------- |
| ostringstream       | 1,187.735 | 0.75x   |
| ostrstream          | 1,048.512 | 0.85x   |
| sprint              | 887.735   | 1.00x   |
| fpconv              | 119.024   | 7.46x   |
| grisu2              | 101.082   | 8.78x   |
| doubleconv (Google) | 84.359    | 10.52x  |
| milo (Tencent)      | 64.100    | 13.85x  |
| ryu                 | 43.541    | 20.39x  |
| fmt                 | 40.712    | 21.81x  |
| null                | 1.200     | 739.78x |


![randomdigit](/assets/images/202110/randomdigit.png)

> 1. Note that the null implementation does nothing. It measures the overheads of looping and function call.
> 
> 2. Why fast dtoa() functions is needed? They are a very common operations in writing data in text format. The standard way of sprintf(), std::stringstream, often provides poor performance. The author of this benchmark would optimize the sprintf implementation in RapidJSON. https://github.com/fmtlib/dtoa-benchmark/blob/master/src/milo/dtoa_milo.h


运行期和编译期计算对比：[https://gcc.godbolt.org/z/M5Pb18PMb](https://gcc.godbolt.org/z/M5Pb18PMb)

``` cpp
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <charconv>
#include <chrono>

// https://en.cppreference.com/w/cpp/compiler_support
// there are currently no compilers that support: Text formatting (P0645R10, std::format)
// #include <format>

// https://github.com/fmtlib/fmt
#define FMT_HEADER_ONLY
#include <fmt/core.h>

const int MAXCNT = 10000;
const int LEN = 8;

class ScopedTimer {
public:
	ScopedTimer(const char* name): m_name(name), m_beg(std::chrono::high_resolution_clock::now()) { }
	~ScopedTimer() {
		auto end = std::chrono::high_resolution_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_beg);
		std::cout << m_name << " : " << dur.count() << " ns\n";
	}
private:
	const char* m_name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_beg;
};

void sprintf_string() 
{
	ScopedTimer timer("sprintf_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		char str[LEN] = {0};
		sprintf(str, "%d", i);
	}
}

void ss_string() 
{
	ScopedTimer timer("ss_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		std::stringstream ss;
		ss << i;
	}
}

void to_string()
{
	ScopedTimer timer("to_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto str = std::to_string(i);
	}
}

void fmt_string() 
{
	ScopedTimer timer("fmt_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto str = fmt::format("{}", i);
	}
}

void fmt_int_string() 
{
	ScopedTimer timer("fmt_int_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto fi = fmt::format_int(i);
        // fi.data() is the data, fi.size() is the size
        auto str = fi.data();
		(void)str;
	}
}

#if 0
void std_fmt_string() 
{
	ScopedTimer timer("std_fmt_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto str = std::format("{}", i);
	}
}
#endif

void tc_string() 
{
	ScopedTimer timer("tc_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		char str[LEN] = {0};
		std::to_chars(str, str + LEN, i);
	}
}

void compilea_string()
{
#define STR(x) #x
#define TOSTRING(x) STR(x)

	ScopedTimer timer("compilea_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto str = TOSTRING(10000);
		(void)str;
	}
#undef TOSTRING
#undef STR
}

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

void compileb_string() 
{
	ScopedTimer timer("compileb_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto str = string_from<unsigned, 10000>::value;
		(void)str;
	}
}

int main() 
{
	sprintf_string();
	ss_string();
	to_string();
	fmt_string();
	fmt_int_string();
	//std_fmt_string();
	tc_string();

	compilea_string();
	compileb_string();
}
```

测试结果：

| Method                | Time (ns) | Speedup |
| --------------------- | --------- | ------- |
| std::stringstream     | 4136931   | 1x      |
| sprintf               | 738063    | 4.6x    |
| std::to_string(C++11) | 69180     | 58.8x   |
| std::to_chars(C++17)  | 42589     | 96.1x   |
| fmt::format           | 128076    | 31.3x   |
| fmt::format_int       | 24807     | 165.8x  |
| std::format(C++20)    | N/A       | N/A     |
| macro_string          | 399       | 10,367x |
| compile_string        | 425       | 9,733x  |

* `to_chars`(c++17) 操作的是 stack-allocated buffer，而`fmt::format`返回的是`std::string` 使用了堆内存。
* `to_chars`(c++17) 需要一次额外的内存拷贝，而`fmt::fmt_int`的内存由对象自己管理不需要拷贝。
* `fmt::fmt_int` > `std::to_chars`(C++17) > `std::to_string`(C++11)

代码分析：

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

编译期计算：

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


# Instruction pipelining

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
  

	
	