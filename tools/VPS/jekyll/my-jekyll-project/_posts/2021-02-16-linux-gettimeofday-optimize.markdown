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

Linux支持的时钟设备包括六种：

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

All 80x86 microprocessors include a CLK input pin, which receives the clock signal of an external oscillator. Starting with the Pentium, 80x86 microprocessors sport a counter that is increased at each clock signal, and is accessible through **the TSC register which can be read by means of the rdtsc assembly instruction**. When using this register the kernel has to take into consideration the frequency of the clock signal: if, for instance, the clock ticks at 1 GHz, the TSC is increased once every nanosecond. Linux may take advantage of this register to get much more accurate time measurements.

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


# gettimeofday性能优化

系统时间对很多互联网应用来说，是一种很宝贵的资源，而一些高性能的后台服务往往因为频繁获取系统时间，使得CPU的利用率大大降低。

在Linux的实现中，获得系统时间的函数主要有`time()`，`ftime()`和`gettimeofday()`三个函数，而前面两个基本上都是对`gettimeofday()`的封装，所以这里主要分析`gettimeofday()`这个函数。另外，从实用的角度出发，这里忽略`gettimeofday()`的第二个参数`timezone`，这个参数对绝大多数的应用意义都不大。

* C library/kernel differences. On some architectures, an implementation of `gettimeofday()` is provided in the `vdso(7)`.

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


``` c
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

/*
 * Checked against the Intel manual and GCC --hpreg
 *
 * volatile because the tsc always changes without the compiler knowing it.
 */
#define RDTSC() ({ register uint64_t tim; __asm__ __volatile__( "rdtsc" : "=A" (tim) ); tim; })
#define RDTSCP(aux) ({ register uint64_t tim; __asm__ __volatile__( "rdtscp" : "=A" (tim),"=c" (aux) ); tim; })

// Static variables are initialized to 0
static struct timeval walltime;
static uint64_t walltick;
static int cpuspeed_mhz;
static int has_rdtscp;
static int cpuid;

static inline int test_rdtscp()
{
	register uint32_t edx;
	__asm__ __volatile__(
		"movl $0x80000001, %%eax \n\t"
		"cpuid"
		: "=d" (edx) : : "eax","ebx","ecx"
	);
	return (edx & (1U<<27));
}


/*
 * Returns CPU clock in mhz
 * Notice that the function will cost the calling thread to sleep wait_us us
 */
static inline int getcpuspeed_mhz(unsigned int wait_us)
{
	uint64_t tsc1, tsc2;
	struct timespec t;

	t.tv_sec = 0;
	t.tv_nsec = wait_us * 1000;

	tsc1 = RDTSC();

	// If sleep failed, result is unexpected
	if(nanosleep(&t, NULL))
		return -1;

	tsc2 = RDTSC();

	return (tsc2 - tsc1)/(wait_us);
}

static int getcpuspeed()
{
	static int speed = -1;

	while(speed<100)
		speed = getcpuspeed_mhz(50*1000);

	return speed;
}

#define REGET_TIME_US	1

//#define getcpuspeed() 2000

# define TIME_ADD_US(a, usec)                      \
  do {                                             \
      (a)->tv_usec += usec;                        \
      while((a)->tv_usec >= 1000000)               \
      {                                            \
        (a)->tv_sec ++;                            \
        (a)->tv_usec -= 1000000;                   \
      }                                            \
  } while (0)


//
// Compile with -O2 to optimize mul/div instructions
// The performance is restricted by 2 factors:
//    1, the rdtsc instruction
//    2, division
//
// Another restriction for this function:
//    The caller thread should run on one CPU or on SMP with sinchronized TSCs,
// otherwise, rdtsc instruction will differ between multiple CPUs.
//    The good thing is that most multicore CPUs are shipped with sinchronized TSCs.
//
static int my_gettimeofday(struct timeval *tv)
{
	uint64_t tick = 0;
	// max_time_us = max_ticks / cpuspeed_mhz > RELOAD_TIME_US us
	static unsigned int max_ticks = 2000*REGET_TIME_US;

	if(walltime.tv_sec==0 || cpuspeed_mhz==0 ||
		// If we are on a different cpu with unsynchronized tsc,
		// RDTSC() may be smaller than walltick
		// in this case tick will be a negative number,
		// whose unsigned value is much larger than max_ticks
	 	(tick=RDTSC()-walltick) > max_ticks)
	{
		if(tick==0 || cpuspeed_mhz==0)
		{
			cpuspeed_mhz = getcpuspeed();
			max_ticks = cpuspeed_mhz*REGET_TIME_US;
		}

		gettimeofday(tv, NULL);
		memcpy(&walltime, tv, sizeof(walltime));
		walltick = RDTSC();
		return 0;
	}

	memcpy(tv, &walltime, sizeof(walltime));

	// if REGET_TIME_US==1, we are currently in the same us, no need to adjust tv
#if REGET_TIME_US > 1
	{
		uint32_t t;
		t = ((uint32_t)tick) / cpuspeed_mhz;
		TIME_ADD_US(tv, t);
	}
#endif
	return 0;
}


int main(int argc, char *argv[])
{
	int i;
	unsigned int loops = 10000000;
	struct timeval t1, t2, t3;

	if(argc<2)
	{
		printf("Please input an argument:\n\t1 compare time\n\t2 syscall\n\t3 fastcall\n");
		return 0;
	}

	if(argc>2)
		loops = strtoul(argv[2], NULL, 10);

	if(argv[1][0]=='1')
	{
		int spd = getcpuspeed();
		uint64_t max = 0, diff = 0, nr_diff1 = 0, nr_diff10 = 0;
		uint64_t tsc = RDTSC();
		uint64_t af = 1;
		int hastscp = 0;
		uint32_t aux = 0;

		if((hastscp=test_rdtscp()))
			printf("This machine supports rdtscp.\n");
		else
			printf("This machine does not support rdtscp.\n");

		if(hastscp)
			tsc = RDTSCP(aux);
		else
			tsc = RDTSC();

		if(sched_setaffinity(0, sizeof(af), &af))
			perror("failed to set affinity");

		printf("tsc=%llu, aux=0x%x, cpu speed %d mhz\n", tsc, aux, spd);
		printf("getspeed_010:%d\n", getcpuspeed_mhz(10*1000));
		printf("getspeed_100:%d\n", getcpuspeed_mhz(100*1000));
		printf("getspeed_500:%d\n", getcpuspeed_mhz(500*1000));
	
		for(i=0; i<loops; i++)
		{
		#if 1
			my_gettimeofday(&t1);
		#else
			gettimeofday(&t1, NULL);
		#endif
			gettimeofday(&t2, NULL);

			if(timercmp(&t1, &t2, >))
				timersub(&t1, &t2, &t3);
			else
				timersub(&t2, &t1, &t3);

//			printf("t1=%u.%06u\t", t1.tv_sec, t1.tv_usec);
//			printf("t2=%u.%06u\t", t2.tv_sec, t2.tv_usec);
//			printf("diff=%u.%06u\n", t3.tv_sec, t3.tv_usec);

			if(max<t3.tv_usec) max = t3.tv_usec;
			if(t3.tv_usec>1) nr_diff1 ++;
			if(t3.tv_usec>10) nr_diff10 ++;
			diff += t3.tv_usec;
//			if(i%20==0) usleep((i%10)<<5);
		}
		printf("max diff=%llu, ave diff=%llu, diff1 count=%llu, diff10 count=%llu.\n", max, diff/i, nr_diff1, nr_diff10);
	}
	else if(argv[1][0]=='2')
	{
		for(i=0; i<loops; i++)
			gettimeofday(&t1, NULL);
	}
	else if(argv[1][0]=='3')
	{
		for(i=0; i<loops; i++)
			my_gettimeofday(&t1);
	}
}
```

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
  
