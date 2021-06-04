---
layout: post
title:  "Linux gettimeofday优化"
date:   2021-02-16 20:00:00 +0800
categories: 性能优化
---

* Do not remove this line (it will not be displayed)
{:toc}


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

* `RTC(Real-time Clock)`
* `PIT(Programmable Interval Timer)`
* `TSC(Time Stamp Counter)`
* `APIC Local Timer`
* `HPET(High Precision Event Timer)`
* `PMTIMER(Power Management Timer)`


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

``` c
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


# Refer

* https://stackoverflow.com/questions/9887839/how-to-count-clock-cycles-with-rdtsc-in-gcc-x86
* https://www.mcs.anl.gov/~kazutomo/rdtsc.html
* https://www.mcs.anl.gov/~kazutomo/rdtsc.h
* https://en.wikipedia.org/wiki/Time_Stamp_Counter
* http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
* https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
* https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2b-manual.pdf

