---
layout: post
title:  "gettimeofday"
date:   2018-11-07 20:22:00 +0800
categories: https
---

* Do not remove this line (it will not be displayed)
{:toc}


```
gerryyang@qsm_cloud_dev-15:/sys/devices/system/clocksource/clocksource0$ls
available_clocksource  current_clocksource  power  subsystem  uevent
gerryyang@qsm_cloud_dev-15:/sys/devices/system/clocksource/clocksource0$cat current_clocksource 
kvm-clock
gerryyang@qsm_cloud_dev-15:/sys/devices/system/clocksource/clocksource0$cat available_clocksource 
kvm-clock tsc acpi_pm 
gerryyang@qsm_cloud_dev-15:/sys/devices/system/clocksource/clocksource0$ll
total 0
-r--r--r-- 1 root root 4096 Nov  3 10:04 available_clocksource
-rw-r--r-- 1 root root 4096 Nov  3 10:04 current_clocksource
drwxr-xr-x 2 root root    0 Nov  3 10:04 power
lrwxrwxrwx 1 root root    0 Oct 30 05:00 subsystem -> ../../../../bus/clocksource
-rw-r--r-- 1 root root 4096 Oct 29 09:33 uevent
```


系统时间对很多互联网应用来说，是一种很宝贵的资源，而一些高性能的后台服务往往因为频繁获取系统时间，使得CPU的利用率大大降低。这篇文章主要讨论Linux下对系统时间的支持和一些优化的方法。

在Linux的实现中，获得系统时间的函数主要有time()、ftime()和gettimeofday()三个函数，而前面两个基本上都是对gettimeofday()的封装，所以这里主要分析gettimeofday()这个函数。另外，从实用的角度出发，这里忽略gettimeofday()的第二个参数timezone，我个人认为这个参数对绝大多数的应用意义都不大。


Linux支持的时钟设备包括六种：RTC(Real-time Clock)、PIT(Programmable Interval Timer)、PMTIMER(Power Management Timer)、APIC Local Timer、HPET(High Precision Event Timer)和TSC(Time Stamp Counter)。



An overview on hardware clock and system timer circuits:
When it comes to talk about a system's clock, the hardware sits at the very bottom. Every typical system has several devices, usually implemented by clock chips, that provide timing features and can serve as clocks. So, which hardware is available depends on the particular architecture. The clock circuits are used both to keep track of the current time of the day and to make precise time measurements. The timer circuits are programmed by the kernel, so they issue interrupts at a fixed, and predefined, frequency. For instance, IA-32 and AMD64 systems have at least one programmable interrupt timer (PIT) as a classical timer circuit, which is usually implemented by an 8254 CMOS chip. Let's briefly describe the clock and timer circuits that are usually found with any nearly modern system of those architectures:

Real Time Clock (RTC)
The RTC is independent of the system's CPU and any other chips. As it is energized by a small battery, it continues to tick even when the system is switched off. The RTC is capable of issuing interrupts at frequencies ranging between 2 Hz and 8,192 Hz. Linux uses the RTC only to derive the time and date at boot time.
Programmable Interrupt Timer (PIT)
The PIT is a time-measuring device that can be compared to the alarm clock of a microwave oven: it makes the user aware that the cooking time interval has elapsed. Instead of ringing a bell, the PIT issues a special interrupt called timer interrupt, which notifies the kernel that one more time interval has elapsed. As the time goes by, the PIT goes on issuing interrupts forever at some fixed (architecture-specific) frequency established by the kernel.
Time Stamp Counter (TSC)
All 80x86 microprocessors include a CLK input pin, which receives the clock signal of an external oscillator. Starting with the Pentium, 80x86 microprocessors sport a counter that is increased at each clock signal, and is accessible through the TSC register which can be read by means of the rdtsc assembly instruction. When using this register the kernel has to take into consideration the frequency of the clock signal: if, for instance, the clock ticks at 1 GHz, the TSC is increased once every nanosecond. Linux may take advantage of this register to get much more accurate time measurements.
CPU Local Timer
The Local APIC (Advanced Programmable Interrupt Controller) present in recent 80x86 microprocessors provide yet another time measuring device, and it is a device, similar to the PIT, which can issue one-shot or periodic interrupts. There are, however, a few differences:
The APIC's timer counter is 32 bit long, while the PIT's timer counter is 16 bit long;
The local APIC timer sends interrupts only to its processor, while the PIT raises a global interrupt, which may be handled by any CPU in the system;
The APIC's timer is based on the bus clock signal, and it can be programmed in such way to decrease the timer counter every 1, 2, 4, 8, 16, 32, 64, or 128 bus clock signals. Conversely, the PIT, which makes use of its own clock signals, can be programmed in a more flexible way.
High Precision Event Timer (HPET)
The HPET is a timer chip that in some future time is expected to completely replace the PIT. It provides a number of hardware timers that can be exploited by the kernel. Basically the chip includes up to eight 32 bit or 64 bit independent counters. Each counter is driven by its own clock signal, whose frequency must be at least 10 MHz; therefore the counter is increased at least once in 100 nanoseconds. Any counter is associated with at most 32 timers, each of which composed by a comparator and a match register. The HPET registers allow the kernel to read and write the values of the counters and of the match registers, to program one-shot interrupts, and to enable or disable periodic interrupts on the timers that support them.
ACPI Power Management Timer (ACPI PMT)
The ACPI PMT is another clock device included in almost all ACPI-based motherboards. Its clock signal has a fixed frequency of roughly 3.58 MHz. The device is a simple counter increased at each clock tick. May introduce issues with sub-millisecond resolution. On modern hardware should be used as a clock source of last resort only.


Linux下的gettimeofday()函数及其优化
http://km.oa.com/articles/show/118505?kmref=search&from_page=1&no=2
  
https://access.redhat.com/solutions/18627
	