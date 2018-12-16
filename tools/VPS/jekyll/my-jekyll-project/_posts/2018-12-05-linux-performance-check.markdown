---
layout: post
title:  "Linux Performance诊断"
date:   2018-12-05 20:00:00 +0800
categories: Linux
---

* Do not remove this line (it will not be displayed)
{:toc}

## 开发语言的性能

https://github.com/famzah/langs-performance

https://blog.famzah.net/2016/09/10/cpp-vs-python-vs-php-vs-java-vs-others-performance-benchmark-2016-q3/


# 性能指标

* 吞吐 
* 延时

从应用角度看，需要做到：高并发，低延时。
从资源角度看，需要关心：资源使用率，饱和度。

![linux-performance](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201812/linux-performance.jpeg)

随着应用负载的增加，系统资源的使用也会升高，甚至达到极限。而性能问题的本质，就是系统资源已经达到瓶颈，导致请求处理不过来。性能分析就是找出应用或系统的瓶颈，并设法去避免或者缓解它们，从而更高效地利用系统资源处理更多的请求。

Brendan Gregg描述的[Linux性能工具图谱]，可以帮助我们在发现Linux出现性能问题后，应该用什么工具来定位和分析。

[Linux性能工具图谱]: brendangregg.com

![brendangregg](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201812/brendangregg.jpeg)
 
以下是一张总的Linux性能优化的思维导图，包括：

* CPU
* 内存
* 网络
* 磁盘IO
* 文件系统
* Linux内核
* 应用程序
* 架构设计
* 性能监控
* 性能测试

![linux-perf-mind](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201812/linux-perf-mind.png)

# 平均负载

每当系统变慢时，可以通过`top`或者`uptime`命令来了解系统的负载情况。

平均负载：是指单位时间内，系统处于**可运行状态**和**不可中断状态**的平均进程数，也就是**平均活跃进程数(实际上是，活跃进程数的指数衰减平均值)**，它和**CPU使用率**并没有直接关系。因此，最理想的是，每个CPU上都刚好运行着一个进程，这样每个CPU都得到了充分利用。

> 例如，当平均负载为2时。
> 在只有2个CPU的系统上，意味着所有的CPU都刚好被完全占用。
> 在4个CPU的系统上，意味着CPU有50%的空闲。
> 在只有1个CPU的系统上，意味着一半的进程竞争不到CPU。

可运行状态的进程：是指正在使用CPU或者正在等待CPU的进程，用ps命令看到处于R状态的进程。

不可中断的进程：是指正处于内核态关键流程中的进程，并且这些流程是不可打断的，比如最常见的是等待硬件设备的I/O响应。用ps命令看到处于D状态的进程。

> 比如，当一个进程向磁盘读写数据时，为了保护数据的一致性，在得到磁盘回复前，它是不能被其他进程或者中断打断的，这个时候的进程就处于不可中断状态。如果此时的进程被打断了，就容易出现磁盘数据与进程数据不一致的问题。所以，**不可中断状态实际上是系统对进程和硬件设备的一种保护机制**。

## 平均负载多少时合理

可以通过top或者从文件`/proc/cpuinfo`中获取cpu的信息。**在实际生产环境中，当平均负载高于CPU数量70%(并不绝对)的时候，就应该排查负载高的问题了。**

``` bash
grep 'model name' /proc/cpuinfo | wc -l 
```

## 平均负载与CPU使用率

平均负载，是指单位时间内，处于可运行状态和不可中断状态的进程数。所以，**它不仅包括了正在使用CPU的进程，还包括等待CPU和等待I/O的进程**。

CPU使用率，是单位时间内CPU繁忙情况的统计，和平均负载并不一定完全对应。比如：

* CPU密集型进程。使用大量CPU会导致平均负载升高，此时两者是一致的。
* I/O密集型进程。等待I/O也会导致平均负载升高，但CPU使用率不一定很高。
* 大量等待CPU的进程调度，也会导致平均负载升高，此时的CPU使用率也会比较高。

## 相关工具

* stress
	- 一个Linux系统压力测试工具。
	- apt install stress 
	- 模拟一个CPU使用率100%的场景。stress --cpu 1 --timeout 600 
	- 模拟I/O压力，不停地执行sync。stress-ng -i 1 --hdd 1 --timeout 600 (--hdd表示读写临时文件)
	- 模拟大量进程场景。stress -c 8 --timeout 600
	- 动态且高亮显示变化的区域 watch -d uptime
* mpstat
	- 一个常用的多核CPU性能分析工具，用来实时查看每个CPU的性能指标，以及所有CPU的平均指标。
	- apt install sysstat
	- 监控所有CPU且5秒输出一次 mpstat -P ALL 5 
* pidstat
	- 一个常用的进程性能分析工具，用来实时查看进程的CPU，内存，I/O以及上下文切换等性能指标。
	- apt install sysstat
	- 间隔5秒输出一组数据 pidstat -u 5 1
* htop
* atop 

# 上下文切换

TODO

# CPU

通过`cat /proc/cpuinfo`可以看到cpu的相关信息。

* CPU：独立的中央处理单元，体现在主板上是有多个CPU的`槽位`。  
* CPU cores：在每一个CPU上，都可能有多个核(core)，每一个核中都有独立的一套ALU，FPU，Cache等组件，所以这个概念也被称作`物理核`。  
* processor：这个主要得益于超线程(HTT)技术，可以让一个物理核模拟出多个`逻辑核`，即processor。  
* 超线程([Hyper-threading])，是一种用于提升CPU计算并行度的处理器技术，用一个物理核模拟两个逻辑核。
这两个逻辑核拥有自己的中断、状态，但是共用物理核的计算资源。超线程技术旨在提高CPU计算资源的使用率，从而提高计算并行度。超线程技术基于这样一个现实，那就是大多数程序运行时，CPU资源并没有得到充分的利用。比如CPU缓存未命中、分支预测错误或者等待数据时，CPU中的计算资源其实是闲置的。超线程技术，可以通过硬件指令，将这些闲置的CPU资源，调度给其他的指令，从而整体上提高CPU的资源利用率。有研究表明，超线程依赖于操作系统对CPU的调度。但是如果应用程序开启了CPU绑定功能，则有可能破坏这种调度的完整性，反而带来性能损失。

更多：

[Hyper-Threading 2]

[When is hyper-threading performance not worth the hassle]

[Superscalar processor]

[Hyper-threading]: https://en.wikipedia.org/wiki/Hyper-threading

[Hyper-Threading 2]: https://whatis.techtarget.com/definition/Hyper-Threading

[When is hyper-threading performance not worth the hassle]: https://searchservervirtualization.techtarget.com/answer/When-should-admins-avoid-using-hyper-threading-technology

[Superscalar processor]: https://en.wikipedia.org/wiki/Superscalar_processor

 # Refer

 TODO 

	
	