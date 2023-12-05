---
layout: post
title:  "Linux gettimeofday优化"
date:   2021-02-16 20:00:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}

# time 获取 Unix 时间戳

参考：https://man7.org/linux/man-pages/man2/time.2.html

``` cpp
#include <time.h>

time_t time(time_t *tloc);
```

`time() `returns the time as the number of seconds since the **Epoch**, **1970-01-01 00:00:00 +0000 (UTC)**.

If `tloc` is non-NULL, the return value is also stored in the memory pointed to by `tloc`.

On success, the value of time in seconds since the Epoch is returned.  On error, (`(time_t) -1`) is returned, and [errno](https://man7.org/linux/man-pages/man3/errno.3.html) is set to indicate the error.

``` cpp
#include <cstdio>
#include <time.h>

int main()
{
    time_t result = time(NULL);
    printf("time(NULL): %d\n", result);
}
/*
$./a.out
time(NULL): 1673429363
*/
```

``` cpp
// std::time
// https://en.cppreference.com/w/cpp/chrono/c/time
#include <iostream>
#include <ctime>

int main()
{
    std::time_t result = std::time(nullptr);
    std::cout << std::asctime(std::localtime(&result))
            << result << " seconds since the Epoch\n";
}
/*
$./a.out
Wed Jan 11 17:04:57 2023
1673427897 seconds since the Epoch
*/
```

> gettimeofday 相比 time 除了获取 seconds 还可以获取 microseconds

``` cpp
#include <cstdio>
#include <time.h>
#include <sys/time.h>

int main()
{
    time_t result = time(NULL);
    printf("time(NULL): %d\n", result);

    struct timeval tv;
    gettimeofday(&tv , NULL);
    printf("gettimeofday: %ld.%06ld\n", tv.tv_sec, tv.tv_usec);
}
/*
$./a.out
time(NULL): 1673429363
gettimeofday: 1673429363.980282
*/
```

# gettimeofday 获取 Unix 时间戳

参考：https://man7.org/linux/man-pages/man2/settimeofday.2.html

``` cpp
#include <sys/time.h>

int gettimeofday(struct timeval *restrict tv,
                struct timezone *restrict tz);
int settimeofday(const struct timeval *tv,
                const struct timezone *tz);
```

The functions `gettimeofday()` and `settimeofday()` can get and set the **time** as well as a **timezone**.

The `tv` argument is a `struct timeval` (as specified in `<sys/time.h>`)

``` cpp
struct timeval {
    time_t      tv_sec;     /* seconds */
    suseconds_t tv_usec;    /* microseconds */
};
```

and gives the number of **seconds** and **microseconds** since the **Epoch** (see [time(2)](https://man7.org/linux/man-pages/man2/time.2.html)).

The `tz` argument is a `struct timezone`:

``` cpp
struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
};
```

If either `tv` or `tz` is `NULL`, the corresponding structure is not set or returned. (However, compilation warnings will result if `tv` is `NULL`.)

The use of the `timezone` structure is **obsolete**; the `tz` argument should normally be specified as `NULL`.

`gettimeofday()` and `settimeofday()` return 0 for success. On error, `-1` is returned and `errno` is set to indicate the error.

``` cpp
#include <cstdio>
#include <sys/time.h>

int main(void)
{
    struct timeval tv;
    gettimeofday (&tv , NULL);
    printf("%ld.%06ld\n", tv.tv_sec, tv.tv_usec);
}
/*
$./a.out
1673426536.297965
*/
```

# 时间和时区

```
$date
2023年 01月 11日 星期三 17:45:15 CST
$date -R
Wed, 11 Jan 2023 17:42:59 +0800
```

整个地球分为二十四时区，每个时区都有自己的本地时间。

* UTC 时间与 GMT 时间

格林威治时间和UTC时间都用秒数来计算的。

* UTC 时间与本地时间

UTC + 时区差 = 本地时间

时区差东为正，西为负。在此，把东八区时区差记为 +0800

UTC + (+0800) = 本地（北京）时间

* UTC 与 Unix 时间戳

在计算机中看到的 UTC 时间都是从（1970年01月01日 00:00:00) 开始计算秒数的，这个秒数就是 Unix 时间戳。

测试代码：

``` cpp
// https://en.cppreference.com/w/cpp/chrono/c/localtime
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

int main()
{
    setenv("TZ", "/usr/share/zoneinfo/America/Los_Angeles", 1); // POSIX-specific

    std::tm tm{};  // zero initialise
    tm.tm_year = 2020-1900; // 2020
    tm.tm_mon = 2-1; // February
    tm.tm_mday = 15; // 15th
    tm.tm_hour = 10;
    tm.tm_min = 15;
    tm.tm_isdst = 0; // Not daylight saving
    std::time_t t = std::mktime(&tm);

    std::cout << "UTC:   " << std::put_time(std::gmtime(&t), "%c %Z") << '\n';
    std::cout << "local: " << std::put_time(std::localtime(&t), "%c %Z") << '\n';
}
/*
UTC:   Sat Feb 15 18:15:00 2020 GMT
local: Sat Feb 15 10:15:00 2020 PST
*/
```

结论：

1. `time`和`gettimeofday`获取的 Unix 时间戳不受时区影响都是一样的，通过修改`/etc/localtime`文件修改时区，从而根据时区转换为当前的时间。
2. `gmtime`给出的是 GMT 标准时间，`localtime`给出的是根据时区转换过的本地时间。


refer:

* https://www.cnblogs.com/linuxbug/p/4887006.html

# 时钟设备

查看当前使用的时钟设备：

```
$cd /sys/devices/system/clocksource/clocksource0
$ll
total 0
-r--r--r-- 1 root root 4096 Nov  3 10:04 available_clocksource
-rw-r--r-- 1 root root 4096 Nov  3 10:04 current_clocksource
drwxr-xr-x 2 root root    0 Nov  3 10:04 power
lrwxrwxrwx 1 root root    0 Oct 30 05:00 subsystem -> ../../../../bus/clocksource
-rw-r--r-- 1 root root 4096 Oct 29 09:33 uevent
$cat current_clocksource
kvm-clock
$cat available_clocksource
kvm-clock tsc acpi_pm
```

修改时钟设备：

```
# Dynamically and temporarily (not persistent to reboots)
echo "acpi_pm" > /sys/devices/system/clocksource/clocksource0/current_clocksource
```

**Linux支持的时钟设备包括六种**：

* [RTC(Real-time Clock)](https://en.wikipedia.org/wiki/Real-time_clock)
* [PIT(Programmable Interval Timer)](https://en.wikipedia.org/wiki/Programmable_interval_timer)
* [TSC(Time Stamp Counter)](https://en.wikipedia.org/wiki/Time_Stamp_Counter)
* [APIC Local Timer](http://wiki.osdev.org/APIC_timer)
* [HPET(High Precision Event Timer)](https://en.wikipedia.org/wiki/High_Precision_Event_Timer)
* PMTIMER(Power Management Timer)

> More detailed information on each method is conveniently presented in the kernel source in [Documentation/virtual/kvm/timekeeping.txt](https://github.com/torvalds/linux/blob/v3.13/Documentation/virtual/kvm/timekeeping.txt).


An overview on hardware clock and system timer circuits:

When it comes to talk about a system's clock, the hardware sits at the very bottom. Every typical system has several devices, usually implemented by clock chips, that provide timing features and can serve as clocks. **So, which hardware is available depends on the particular architecture.** The clock circuits are used both to keep track of the current time of the day and to make precise time measurements. The timer circuits are programmed by the kernel, so they issue interrupts at a fixed, and predefined, frequency. For instance, IA-32 and AMD64 systems have at least one programmable interrupt timer (PIT) as a classical timer circuit, which is usually implemented by an 8254 CMOS chip. Let's briefly describe the clock and timer circuits that are usually found with any nearly modern system of those architectures:

> Real Time Clock (RTC)

The `RTC` is independent of the system's CPU and any other chips. As it is energized by a small battery, it continues to tick even when the system is switched off. The RTC is capable of issuing interrupts at frequencies ranging between 2 Hz and 8,192 Hz. Linux uses the RTC only to derive the time and date at boot time.

> Programmable Interrupt Timer (PIT)

The `PIT` is a time-measuring device that can be compared to the alarm clock of a microwave oven: it makes the user aware that the cooking time interval has elapsed. Instead of ringing a bell, the PIT issues a special interrupt called timer interrupt, which notifies the kernel that one more time interval has elapsed. As the time goes by, the PIT goes on issuing interrupts forever at some fixed (architecture-specific) frequency established by the kernel.

> Time Stamp Counter (TSC)

All 80x86 microprocessors include a CLK input pin, which receives the clock signal of an external oscillator. Starting with the Pentium, 80x86 microprocessors sport a counter that is increased at each clock signal, and is accessible through [the TSC register which can be read by means of the rdtsc assembly instruction](https://en.wikipedia.org/wiki/Time_Stamp_Counter). When using this register the kernel has to take into consideration the frequency of the clock signal: if, **for instance, the clock ticks at 1 GHz, the TSC is increased once every nanosecond**. Linux may take advantage of this register to get much more accurate time measurements.

> CPU Local Timer

The Local `APIC` (Advanced Programmable Interrupt Controller) present in recent 80x86 microprocessors provide yet another time measuring device, and it is a device, similar to the `PIT`, which can issue one-shot or periodic interrupts. There are, however, a few differences:

1. The APIC's timer counter is 32 bit long, while the PIT's timer counter is 16 bit long;
2. The local APIC timer sends interrupts only to its processor, while the PIT raises a global interrupt, which may be handled by any CPU in the system;
3. The APIC's timer is based on the bus clock signal, and it can be programmed in such way to decrease the timer counter every 1, 2, 4, 8, 16, 32, 64, or 128 bus clock signals. Conversely, the PIT, which makes use of its own clock signals, can be programmed in a more flexible way.

> High Precision Event Timer (HPET)

The `HPET` is a timer chip that in some future time is expected to completely replace the `PIT`. It provides a number of hardware timers that can be exploited by the kernel. Basically the chip includes up to eight 32 bit or 64 bit independent counters. Each counter is driven by its own clock signal, whose frequency must be at least 10 MHz; therefore the counter is increased at least once in 100 nanoseconds. Any counter is associated with at most 32 timers, each of which composed by a comparator and a match register. The HPET registers allow the kernel to read and write the values of the counters and of the match registers, to program one-shot interrupts, and to enable or disable periodic interrupts on the timers that support them.

> ACPI Power Management Timer (ACPI PMT)

The `ACPI PMT` is another clock device included in almost all ACPI-based motherboards. Its clock signal has a fixed frequency of roughly 3.58 MHz. The device is a simple counter increased at each clock tick. May introduce issues with sub-millisecond resolution. On modern hardware should be used as a clock source of last resort only.


refer:

* [How to change the clock source in the system](https://access.redhat.com/solutions/18627)


# gettimeofday的实现

gettimeofday定义：

``` c
// https://man7.org/linux/man-pages/man2/gettimeofday.2.html
#include <sys/time.h>

int gettimeofday(struct timeval *tv, struct timezone *tz);

struct timeval {
   time_t      tv_sec;     /* seconds */
   suseconds_t tv_usec;    /* microseconds */
};

struct timezone {
   int tz_minuteswest;     /* minutes west of Greenwich */
   int tz_dsttime;         /* type of DST correction */
};
```

系统时间对很多互联网应用来说，是一种很宝贵的资源，而一些高性能的后台服务往往因为频繁获取系统时间，使得CPU的利用率大大降低。

在Linux的实现中，获得系统时间的函数主要有`time()`，`ftime()`和`gettimeofday()`三个函数，而前面两个基本上都是对`gettimeofday()`的封装，所以这里主要分析`gettimeofday()`这个函数。另外，从实用的角度出发，这里忽略`gettimeofday()`的第二个参数`timezone`，这个参数对绝大多数的应用意义都不大。

传统的gettimeofday的实现基本上综合了Linux的墙上时间(`xtime`)和硬件相关的一些时间矫正。**gettimeofday的操作可以分解为三步：**

1. 取得墙上时间`xtime`
2. 访问时间设备，取得上次时间中断到当前的时间偏移
3. 根据这个时间偏移计算出当前的精确时间

**可见，`gettimeofday`的精度和性能由具体的时间设备所决定**。

* `TSC`是由CPU内部实现的，只要访问CPU的寄存器即可以取得TSC计数，所以，其性能无疑是最好的，但是其可靠性却是比较低的。
* 现在的很多Linux版本默认都是使用`HPET`作为时钟源的，可以满足大部分应用在时间和性能上的需求。
* 一般的情况下，gettimeofday的执行性能应该在**500万/秒左右**，如果想获得更高的性能，就必须对现有的系统调用进行优化。


# gettimeofday性能优化思路

**优化思路：**

1. `vsyscall`机制。
	* 在2.6的内核里面，实现了一种vsyscall的机制，允许应用程序不需要进入内核就可以获得内核的数据（比如系统时间），但是，这种机制只有在x86_64的Linux下才受到支持(On some architectures, an implementation of `gettimeofday()` is provided in the `vdso(7)`)
	* vsyscall的实现是通过内核映射一个专门的页面，用于存储一些对用户只读的数据（例如时间、CPU编号等），并定期更新这些数据。通过glibc提供的共享库，系统将这些页面映射到用户进程的地址空间，当用户进程调用glibc提供的接口（例如gettimeofday）时，glibc获得这些共享内存的数据，返回给用户，从而实现了原来需要进入内核才能实现的功能。
	* vsyscall可以和TSC结合使用，这个时候，gettimeofday将获得最大的性能，几乎可以认为就是简单的读内存操作。
	* **问题：vsyscall的最大不足在于只支持x86_64的系统，在32位的系统上无法使用**。虽然在网上可以找到一些内核和glibc的patch，使得32位的系统也能使用vsyscall功能，但要求重新编译内核，操作十分繁琐。

2. 在用户态下对`gettimeofday`进行优化。
	* 从最简单的情况说起，如果对精度的要求不是很高，那么，完全可以把上一次的调用结果缓存起来，当再次调用gettimeofday的时候，先判断一下两次调用的时间延时，如果延时小于1毫秒（也可以是10微妙，100毫秒等等），则返回上次调用的结果（或者加上从上次调用到现在的时延，有点像内核的gettimeofday实现，只是现在是在用户态）。
	* **问题：如果判断两次调用的时延？**

访问[TSC寄存器(Time Stamp Counter)](https://en.wikipedia.org/wiki/Time_Stamp_Counter)可以使用`rdtsc(Read Time-Stamp Counter)`指令。

![rdtsc](/assets/images/202106/rdtsc.png)

> Description:
>
> Reads the current value of the processor's time-stamp counter (a 64-bit MSR) into the EDX:EAX registers. The EDX register is loaded with the high-order 32 bits of the MSR and the EAX register is loaded with the low-order 32 bits. On processors that support the Intel 64 architecture, the high-order 32 bits of each of RAX and RDX are cleared.
>
> The processor monotonically increments the time-stamp counter MSR every clock cycle and resets it to 0 whenever the processor is reset.

refer: https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2b-manual.pdf

下面的宏定义用于读取TSC的值：

``` c
#ifdef __x86_64__
#define RDTSC() ({ unsigned int tickl, tickh; __asm__ __volatile__("rdtsc":"=a"(tickl),"=d"(tickh)); ((unsigned long long)tickh << 32)|tickl; })
#else
#define RDTSC() ({ unsigned long long tick; __asm__ __volatile__( "rdtsc" : "=A"(tick)); tick; })
#endif
```

`TSC`是一个**64位的寄存器，相当与一个计数器(It counts the number of CPU cycles since its reset)，但我们所需要的是时间，而不是计数。由于`TSC`的值是每个CPU时钟周期增加1，所以只要知道了CPU的时间频率，就可以将这个值换算成时间**。因为对精度的要求并不是很高（微秒级），我们只需要获得以兆为单位的大约值就可以了。下面的函数获得**CPU的频率**：

``` cpp
static inline int getcpuspeed_mhz(unsigned int wait_us)
{
   uint64_t tsc1, tsc2;
   struct timespec t;

    t.tv_sec = 0;
    t.tv_nsec = wait_us * 1000;

    tsc1 = RDTSC();

    // If sleep failed, result is unexpected, the caller should retry
    if (nanosleep(&t, NULL)) {
         return -1;
    }

     tsc2 = RDTSC();
     return (tsc2 - tsc1) / (wait_us);
}
```

可以看出，CPU频率和TSC的换算公式如下：

> cpu_speed_mhz = tsc_offset / time_offset_us

反过来，如果我们知道了**TSC的偏移**和**CPU频率**，就可以获得**时间偏移**了：

> time_offset_us = tsc_offset / cpu_speed_mhz


# gettimeofday性能优化实现

* 注意`RELOAD_TIME_US`这个宏的定义，它告诉`my_gettimeofday()`每隔多少微妙就重新读一些系统调用`gettimeofday()`，如果这个值是1，代码就会变得十分简洁，因为后面不需要对保留的时间做**矫正**。
* 经过测试，在频率为2G的CPU上这个函数的执行效率在`2000w/s`左右，而相同条件下gettimeofday的执行效率在`250w/s`左右，精度方面也做了大概的测试，差异在10微妙以内大概占千分之一，而差异大于10微妙的比率在百万分之一以内，这个精度应该足以满足绝大多数的应用了。
* 另外，值得一提的是，`rdtsc`的指令读取的是当前CPU的TSC值，而现在的多核系统，对TSC的处理是不太一致的，大多数的Intel CPU采用Synchronized TSC的方式，各个不同的CPU核间会同步TSC的值。但是，并不是所有的多CPU系统都会同步CPU间的TSC值，特别是，单不同CPU的主频不一样的时候，TSC是无法同步的。为此，如果要兼顾所有的情况，可以有**三种解决方法**：
	+ 使用`sched_setaffinity()`将当前进程绑定到一个CPU上
	+ 获得当前CPU的ID，当ID发生变化是重新调用`gettimeofday()`系统函数。较新的Intel系列CPU支持一个新的指令`rdtscp`，除了读取TSC的值外，还额外读了一个辅助寄存器到`ECX`，这个寄存器保留当前CPU的处理器ID（基于vsyscall的getcpu()会尝试使用rdtscp指令来读取CPU的ID，当然这个值也是内核写进去的。）有了这个指令，可以在读TSC的时候同时读出CPU ID，并将CPU ID保存在内存中，当下次读TSC的时候，将两个CPU ID进行比较，就可以检测出当前进程被调度到其它CPU的情况了。
	+ 不处理。如果对精度要求不是特别高，完全可以不处理。这是基于以下的原因：1. Linux内核不会随便将进程调度到另外的CPU，就算调度，也会优先调度到同一个物理CPU上，TSC的值也往往是一致的。2. 就算TSC的值不一致，如果差异比较小，在精度允许的范围内，是可以接受的；如果差异比较大，会被my_gettimeofday()检测出来，并重新调用gettimeofday()。
* 如果多CPU的时钟频率不一样，完全不处理也不大可行，因为如果getcpuspeed()返回错误的值，my_gettimeofday()就不准确了。可以加上一段逻辑，当TSC的差异大于某个值的时候，重新计算CPU的频率。

# 32位和64位的区别

参考[Machine Constraints](https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints)：

`A`

The `a` and `d` registers. This class is used for instructions that return **double word(2 * 32)** results in the `ax:dx` register pair. Single word values will be allocated either in `ax` or `dx`. For example on `i386` the following implements `rdtsc`:

``` c
unsigned long long rdtsc (void)
{
  unsigned long long tick;
  __asm__ __volatile__("rdtsc":"=A"(tick));
  return tick;
}
```

This is not correct on `x86-64` as it would allocate tick in either `ax` or `dx`. You have to use the following variant instead:

``` c
unsigned long long rdtsc (void)
{
  unsigned int tickl, tickh;
  __asm__ __volatile__("rdtsc":"=a"(tickl),"=d"(tickh));
  return ((unsigned long long)tickh << 32)|tickl;
}
```

![rdtsc_disassemble](/assets/images/202106/rdtsc_disassemble.png)


# 测试代码

测试环境：

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

测试代码：

[https://github.com/gerryyang/mac-utils/tree/master/programing/cpp/performance/gettimeofday](https://github.com/gerryyang/mac-utils/tree/master/programing/cpp/performance/gettimeofday)

测试结果：

```
$ time ./a.out 1
This machine does not support rdtscp.
tsc=4030837879, aux=0x0, cpu speed 2397 mhz
getspeed_010:2410
getspeed_100:2395
getspeed_500:-280931417
max diff=5626, ave diff=0, diff1 count=336682, diff10 count=1562.

real    0m5.788s
user    0m2.004s
sys     0m3.025s

$ time ./a.out 1
This machine does not support rdtscp.
tsc=3545218234, aux=0x0, cpu speed 2397 mhz
getspeed_010:2410
getspeed_100:2396
getspeed_500:-280931417
max diff=4927, ave diff=0, diff1 count=288140, diff10 count=1096.

real    0m5.686s
user    0m1.862s
sys     0m3.038s

$ time ./a.out 2

real    0m3.448s
user    0m1.114s
sys     0m2.260s

$ time ./a.out 3

real    0m0.191s
user    0m0.100s
sys     0m0.040s
```

rdtsc测试代码 (Linux, GCC)：

``` cpp
/* define this somewhere */
#ifdef __i386
__inline__ uint64_t rdtsc() {
  uint64_t x;
  __asm__ volatile ("rdtsc" : "=A" (x));
  return x;
}
#elif __amd64
__inline__ uint64_t rdtsc() {
  uint64_t a, d;
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  return (d<<32) | a;
}
#endif

/* now, in your function, do the following */
uint64_t t;
t = rdtsc();
// ... the stuff that you want to time ...
t = rdtsc() - t;
// t now contains the number of cycles elapsed
```

# Windows __rdtsc

Generates the rdtsc instruction, which returns the processor time stamp. The processor time stamp records the number of clock cycles since the last reset.

``` c
// rdtsc.cpp
// processor: x86, x64
#include <stdio.h>
#include <intrin.h>

#pragma intrinsic(__rdtsc)

int main()
{
    unsigned __int64 i;
    i = __rdtsc();
    printf_s("%I64d ticks\n", i);
}
```

https://docs.microsoft.com/en-us/cpp/intrinsics/rdtsc?view=msvc-160

# system call/vsyscall/vDSO

## system call

这是 Linux 内核一种特殊的运行机制，使得用户空间的应用程序可以请求，像写入文件和打开套接字等特权级下的任务。在 Linux 内核中发起一个系统调用是特别昂贵的操作，因为处理器需要中断当前正在执行的任务，切换内核模式的上下文，在系统调用处理完毕后跳转至用户空间。`vsyscall`和`vdso`两种机制被设计用来加速系统调用的处理。

* [The Definitive Guide to Linux System Calls](http://blog.packagecloud.io/eng/2016/04/05/the-definitive-guide-to-linux-system-calls)
* [Searchable Linux Syscall Table for x86 and x86_64](https://filippo.io/linux-syscall-table/)

## vsyscall

`vsyscall`或`virtual system call`是第一种也是最古老的一种用于加快系统调用的机制。`vsyscall`的工作原则其实十分简单。Linux 内核在用户空间映射一个包含一些变量及一些系统调用的实现的内存页。因此, 这些系统调用将在用户空间下执行，这意味着将不发生上下文切换。

```
$ cat /proc/1/maps | grep vsyscall
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]
```

## vDSO

`vDSO` (**virtual dynamic shared object**) is a kernel mechanism for exporting a carefully selected set of kernel space routines to user space applications so that applications can call these kernel space routines in-process, without incurring the performance penalty of a mode switch from user mode to kernel mode that is inherent when calling these same kernel space routines by means of the system call interface.

`vsyscall` is an obsolete concept and replaced by the `vDSO` or `virtual dynamic shared object`. The main difference between the `vsyscall` and `vDSO` mechanisms is that `vDSO` maps memory pages into each process in a shared object form, but `vsyscall` is static in memory and has the same address every time. For the x86_64 architecture it is called `linux-vdso.so.1`. All userspace applications linked with this shared library via the `glibc`.

```
 $ ldd /bin/uname
        linux-vdso.so.1 =>  (0x00007ffdbabc5000)
        /$LIB/libonion.so => /lib64/libonion.so (0x00007f846a10c000)
        libc.so.6 => /lib64/libc.so.6 (0x00007f8469c25000)
        libdl.so.2 => /lib64/libdl.so.2 (0x00007f8469a21000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f8469ff3000)

$ sudo cat /proc/1/maps | grep vdso
7fff93f0a000-7fff93f0c000 r-xp 00000000 00:00 0                          [vdso]
```

## vDSO fallback mechanism

通过[Two frequently used system calls are ~77% slower on AWS EC2](https://blog.packagecloud.io/eng/2017/03/08/system-calls-are-much-slower-on-ec2/)：

Let’s take a look at the `vDSO` code implementing `gettimeofday` for more clarity. Remember, this code is packaged with the kernel, but is actually run completely in userland.

If we examine the code in [arch/x86/vdso/vclock_gettime.c](https://github.com/torvalds/linux/blob/v3.13/arch/x86/vdso/vclock_gettime.c#L260-L282) and check the `vDSO` implementations for `gettimeofday` (`__vdso_gettimeofday`) and `clock_gettime` (`__vdso_clock_gettime`), we’ll find that both pieces of code have a similar conditional near the end of the function:

``` cpp
if (ret == VCLOCK_NONE)
  return vdso_fallback_gtod(clock, ts);
```

``` cpp
notrace int __vdso_gettimeofday(struct timeval *tv, struct timezone *tz)
{
	long ret = VCLOCK_NONE;

	if (likely(tv != NULL)) {
		BUILD_BUG_ON(offsetof(struct timeval, tv_usec) !=
			     offsetof(struct timespec, tv_nsec) ||
			     sizeof(*tv) != sizeof(struct timespec));
		ret = do_realtime((struct timespec *)tv);
		tv->tv_usec /= 1000;
	}
	if (unlikely(tz != NULL)) {
		/* Avoid memcpy. Some old compilers fail to inline it */
		tz->tz_minuteswest = gtod->sys_tz.tz_minuteswest;
		tz->tz_dsttime = gtod->sys_tz.tz_dsttime;
	}

	if (ret == VCLOCK_NONE)
		return vdso_fallback_gtod(tv, tz);
	return 0;
}
int gettimeofday(struct timeval *, struct timezone *)
	__attribute__((weak, alias("__vdso_gettimeofday")));
```

(The code for `__Vdso_clock_gettime` has the same check, but calls `vdso_fallback_gettime` instead.)

If `ret` is set to `VCLOCK_NONE` this indicates that the system’s current clocksource **does not support the vDSO**. In this case, the [vdso_fallback_gtod](https://github.com/torvalds/linux/blob/v3.13/arch/x86/vdso/vclock_gettime.c#L144-L151) function failsafe function is called which will simply executes a system call normally: by entering the kernel and incurring all the normal overhead.

``` cpp
notrace static long vdso_fallback_gtod(struct timeval *tv, struct timezone *tz)
{
	long ret;

	asm("syscall" : "=a" (ret) :
	    "0" (__NR_gettimeofday), "D" (tv), "S" (tz) : "memory");
	return ret;
}
```

But, in which cases does ret get set to `VCLOCK_NONE`?

If we follow the code backward from this point, we’ll find that `ret` is set to the `vclock_mode` field of the current clocksource. Clocksources such as:

* the [High Precision Event Timer](https://github.com/torvalds/linux/blob/v3.13/arch/x86/kernel/hpet.c#L755-L757), and

``` cpp
static struct clocksource clocksource_hpet = {
	.name		= "hpet",
	.rating		= 250,
	.read		= read_hpet,
	.mask		= HPET_MASK,
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
	.resume		= hpet_resume_counter,
#ifdef CONFIG_X86_64
	.archdata	= { .vclock_mode = VCLOCK_HPET },
#endif
};
```

* the [Time Stamp Counter](https://github.com/torvalds/linux/blob/v3.13/arch/x86/kernel/tsc.c#L789-L791),

``` cpp
static struct clocksource clocksource_tsc = {
	.name                   = "tsc",
	.rating                 = 300,
	.read                   = read_tsc,
	.resume			= resume_tsc,
	.mask                   = CLOCKSOURCE_MASK(64),
	.flags                  = CLOCK_SOURCE_IS_CONTINUOUS |
				  CLOCK_SOURCE_MUST_VERIFY,
#ifdef CONFIG_X86_64
	.archdata               = { .vclock_mode = VCLOCK_TSC },
#endif
};
```

* and in some cases the [KVM PVClock](https://github.com/torvalds/linux/blob/v3.13/arch/x86/kernel/kvmclock.c#L305)

all have their `vclock_mode` fields set to an identifier other than `VCLOCK_NONE`.

On the other hand, clocksources such as:

* the [Xen time](https://github.com/torvalds/linux/blob/v3.13/arch/x86/xen/time.c#L233-L239) implementation, and
* systems where either `CONFIG_PARAVIRT_CLOCK` is not enabled in the kernel configuration or the CPU does not provide a paravirtualized clock feature

all have their `vclock_mode` fields set to `VCLOCK_NONE (0)`.

AWS EC2 uses Xen. Xen’s default clocksource (`xen`) has its `vclock_mode` field set to `VCLOCK_NONE` which means EC2 instances will always fall back to using the slower system call path – the vDSO will never be used.

But, what effect does this have on performance?

## Profiling the performance difference between regular system calls vs vDSO system calls

The purpose of the following experiment is to measure the difference in wall clock time in a microbenchmark to test the difference in execution speed between the fast vDSO-enabled `gettimeofday` system calls and regular, slow, `gettimeofday` calls.

In order to test this, we’ll run the sample program above with three different loop counts on an EC2 instance with the clocksource set to `xen` and then again with the clocksource set to `tsc`.

> It is not safe to switch the clocksource to tsc on EC2. It is unlikely, but possible that this can lead to unexpected backwards clock drift. Do not do this on your production systems.

We’ll time the execution of the program using the `time` program. Readers may wonder: “how can you use the time program if you are potentially destabilizing the clocksource?”

Luckily, the kernel developer Ingo Molnar wrote a program for detecting time warps: [time-warp-test.c](https://people.redhat.com/mingo/time-warp-test/time-warp-test.c). Note that you will need to modify this program just slightly for 64bit x86 systems.

The results of this microbenchmark show that the regular system call method which is used on ec2 is about **77% slower than** the `vDSO` method:


A tight loop of 500 million calls to `gettimeofday`:

vDSO enabled:
real: 0m12.247s
user: 0m12.244s
sys: 0m0.000s

regular system call:
real: 0m54.606s
user: 0m13.192s
sys: 0m41.412s

The proper fix for this issue would be to add `vDSO` support to the `xen` clocksource.

## Conclusion

As expected, the vDSO system call path is measurably faster than the normal system call path. This is because the vDSO system call path prevents a context switch into the kernel. Remember: vDSO system calls will not appear in strace output if they successfully pass through the vDSO. If they are unabled to use the vDSO for some reason, they will fall back to regular system calls and will appear in strace output.

gettimeofday and clock_gettime will perform approximately 77% slower than they normally would.

Using strace on your applications incurs overhead while it is in use, but it provides invaluable insight into what exactly your applications are doing. All programmers deploying software to production environments should regularly strace their applications in development mode and question all output they find.

## 测试代码

* 通过[vDSO man page](https://man7.org/linux/man-pages/man7/vdso.7.html)可知，`gettimeofday`通过`vDSO`运行在用户态，而不需要切换到内核态。

```
One frequently used system call is gettimeofday(2).  This system
call is called both directly by user-space applications as well
as indirectly by the C library.  Think timestamps or timing loops
or polling—all of these frequently need to know what time it is
right now.  This information is also not secret—any application
in any privilege mode (root or any unprivileged user) will get
the same answer.  Thus the kernel arranges for the information
required to answer this question to be placed in memory the
process can access.  Now a call to gettimeofday(2) changes from a
system call to a normal function call and a few memory accesses.
```

the `vDSO` is essentially a shared library that is provided by the kernel which is mapped into every process’ address space. When the `gettimeofday`, `clock_gettime`, `getcpu`, or `time` system calls are made, `glibc` will attempt to call the code provided by the `vDSO`. This code will access the needed data without entering the kernel, saving the process the overhead of making a real system call.

Because system calls made via the `vDSO` do not enter the kernel, `strace` is not notified that the `vDSO` system call was made. As a result, a program which calls `gettimeofday` successfully via the `vDSO` will not show `gettimeofday` in the `strace` output. You would need to use `ltrace` instead.

* 有些时钟源，可能不支持vdso。例如，[Two frequently used system calls are ~77% slower on AWS EC2](https://blog.packagecloud.io/eng/2017/03/08/system-calls-are-much-slower-on-ec2/)

```
The two system calls listed cannot use the vDSO as they normally would on any other system. This is because the virtualized clock source on xen (and some kvm configurations) do not support reading the time in userland via the vDSO.
```

* 有些系统调用的参数选项，可能不支持vdso，比如`clock_gettime`的第一个参数。当为`CLOCK_REALTIME`, `CLOCK_MONOTONIC`, `CLOCK_REALTIME_COARSE`, `CLOCK_MONOTONIC_COARSE` 时会使用vdso，而其他选项时则不会。具体参数可见 `man 2 clock_gettime`

* gcc 4.8版本在调用`std::chrono::system_clock::now`会bypass vdso走system call，优化方法：升级到gcc 7或者改换其他用法。bug参考可见：[Bug 59177 - steady_clock::now() and system_clock::now do not use the vdso (and are therefore very slow)](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59177)

``` cpp
#include <time.h>
#include <sys/time.h>

int main()
{
  struct timespec tp;
  struct timeval tv;
  int i = 0;
  int j = 0;
  for (i = 0; i < 1000000; ++i)
  {
      // vdso
      // glibc wrapped, shouldn't actually syscall
      gettimeofday(&tv, NULL);
      j += tv.tv_usec % 2;

      // https://linux.die.net/man/3/clock_gettime

      // vdso
      //clock_gettime(CLOCK_REALTIME, &tp);
      //clock_gettime(CLOCK_MONOTONIC, &tp);
      //clock_gettime(CLOCK_REALTIME_COARSE, &tp);
      clock_gettime(CLOCK_MONOTONIC_COARSE, &tp);

      // syscall
      //clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp);
      //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);

      // vdso
      //auto t = std::time(0);
      //printf("%u\n", t);

      // syscall
      //auto t = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
      //std::chrono::system_clock::now();

      j += tp.tv_sec % 2;
  }
}
```

```
$ strace -c ./a.out
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
  0.00    0.000000           0         3           read
  0.00    0.000000           0         5           open
  0.00    0.000000           0         5           close
  0.00    0.000000           0         5           fstat
  0.00    0.000000           0        14           mmap
  0.00    0.000000           0         7           mprotect
  0.00    0.000000           0         2           munmap
  0.00    0.000000           0         1           brk
  0.00    0.000000           0         1           access
  0.00    0.000000           0         1           execve
  0.00    0.000000           0         1           readlink
  0.00    0.000000           0         1           arch_prctl
------ ----------- ----------- --------- --------- ----------------
100.00    0.000000                    46           total

$ ltrace -c ./a.out
^C% time     seconds  usecs/call     calls      function
------ ----------- ----------- --------- --------------------
 56.12    8.085410     8085410         1 __libc_start_main
 21.97    3.165150          67     46577 gettimeofday
 21.91    3.157182          67     46577 clock_gettime
------ ----------- ----------- --------- --------------------
100.00   14.407742                 93155 total
```


* [vDSO man page](https://man7.org/linux/man-pages/man7/vdso.7.html)
* https://en.wikipedia.org/wiki/VDSO
* https://www.cntofu.com/book/114/SysCall/syscall-3.md
* [gettimeofday() not using vDSO?](https://stackoverflow.com/questions/42622427/gettimeofday-not-using-vdso)
* [Two frequently used system calls are ~77% slower on AWS EC2](https://blog.packagecloud.io/eng/2017/03/08/system-calls-are-much-slower-on-ec2/)
* [Virtual system calls](https://blog.packagecloud.io/eng/2016/04/05/the-definitive-guide-to-linux-system-calls/#virtual-system-calls)
* [Searchable Linux Syscall Table for x86 and x86_64](https://filippo.io/linux-syscall-table/)
* [VMWare(Paper): Timekeeping in VMware Virtual Machines](https://www.vmware.com/pdf/vmware_timekeeping.pdf)

# Refer

* https://stackoverflow.com/questions/9887839/how-to-count-clock-cycles-with-rdtsc-in-gcc-x86
* https://www.mcs.anl.gov/~kazutomo/rdtsc.html
* https://www.mcs.anl.gov/~kazutomo/rdtsc.h
* https://en.wikipedia.org/wiki/Time_Stamp_Counter
* http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
* https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
* https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2b-manual.pdf

