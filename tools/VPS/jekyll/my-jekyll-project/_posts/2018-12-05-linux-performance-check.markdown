---
layout: post
title:  "Linux Performance诊断"
date:   2018-12-05 20:00:00 +0800
categories: Linux
---

* Do not remove this line (it will not be displayed)
{:toc}




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

# CPU相关

通过`cat /proc/cpuinfo`可以看到cpu的相关信息。

* CPU：独立的中央处理单元，体现在主板上是有多个CPU的`槽位`。  
* CPU cores：在每一个CPU上，都可能有多个核(core)，每一个核中都有独立的一套ALU，FPU，Cache等组件，所以这个概念也被称作`物理核`。  
* processor：这个主要得益于超线程(HTT)技术，可以让一个物理核模拟出多个`逻辑核`，即processor。  
* 超线程([Hyper-threading])，是一种用于提升CPU计算并行度的处理器技术，用一个物理核模拟两个逻辑核。
这两个逻辑核拥有自己的中断、状态，但是共用物理核的计算资源。超线程技术旨在提高CPU计算资源的使用率，从而提高计算并行度。超线程技术基于这样一个现实，那就是大多数程序运行时，CPU资源并没有得到充分的利用。比如CPU缓存未命中、分支预测错误或者等待数据时，CPU中的计算资源其实是闲置的。超线程技术，可以通过硬件指令，将这些闲置的CPU资源，调度给其他的指令，从而整体上提高CPU的资源利用率。有研究表明，超线程依赖于操作系统对CPU的调度。但是如果应用程序开启了CPU绑定功能，则有可能破坏这种调度的完整性，反而带来性能损失。

Refer：

[Hyper-Threading 2]

[When is hyper-threading performance not worth the hassle]

[Superscalar processor]

## 平均负载

每当系统变慢时，可以通过`top`或者`uptime`命令来了解系统的负载情况。

平均负载：是指单位时间内，系统处于**可运行状态**和**不可中断状态**的平均进程数，也就是**平均活跃进程数(实际上是，活跃进程数的指数衰减平均值)**，它和**CPU使用率**并没有直接关系。因此，最理想的是，每个CPU上都刚好运行着一个进程，这样每个CPU都得到了充分利用。

> 例如，当平均负载为2时。
> 在只有2个CPU的系统上，意味着所有的CPU都刚好被完全占用。
> 在4个CPU的系统上，意味着CPU有50%的空闲。
> 在只有1个CPU的系统上，意味着一半的进程竞争不到CPU。

可运行状态的进程：是指正在使用CPU或者正在等待CPU的进程，用ps命令看到处于R状态的进程。

不可中断的进程：是指正处于内核态关键流程中的进程，并且这些流程是不可打断的，比如最常见的是等待硬件设备的I/O响应。用ps命令看到处于D状态的进程。

> 比如，当一个进程向磁盘读写数据时，为了保护数据的一致性，在得到磁盘回复前，它是不能被其他进程或者中断打断的，这个时候的进程就处于不可中断状态。如果此时的进程被打断了，就容易出现磁盘数据与进程数据不一致的问题。所以，**不可中断状态实际上是系统对进程和硬件设备的一种保护机制**。

### 平均负载多少时合理

可以通过top或者从文件`/proc/cpuinfo`中获取cpu的信息。**在实际生产环境中，当平均负载高于CPU数量70%(并不绝对)的时候，就应该排查负载高的问题了。**

``` bash
grep 'model name' /proc/cpuinfo | wc -l 
```

### 平均负载与CPU使用率

平均负载，是指单位时间内，处于可运行状态和不可中断状态的进程数。所以，**它不仅包括了正在使用CPU的进程，还包括等待CPU和等待I/O的进程**。

CPU使用率，是单位时间内CPU繁忙情况的统计，和平均负载并不一定完全对应。比如：

* CPU密集型进程。使用大量CPU会导致平均负载升高，此时两者是一致的。
* I/O密集型进程。等待I/O也会导致平均负载升高，但CPU使用率不一定很高。
* 大量等待CPU的进程调度，也会导致平均负载升高，此时的CPU使用率也会比较高。

## 性能测试工具

* stress
	- 一个Linux系统压力测试工具。
	- apt install stress 
	- 模拟一个CPU使用率100%的场景。stress --cpu 1 --timeout 600 
	- 模拟I/O压力，不停地执行sync。stress-ng -i 1 --hdd 1 --timeout 600 (-i表示调用sync, --hdd表示读写临时文件)
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
	- CentOS的pidstat没有%wait指标(进程等待CPU的时间百分比)，这个是sysstat 11.5.5版本才引入的新指标。
	- pidstat中的%wait表示进程等待CPU的时间百分比，而top中的iowait%表示等待I/O的CPU时间百分比。两者是不同的指标。
* vmstat
	- vmstat输出的第一行数据为什么和其他行差别巨大？回答：The  first  report  produced gives averages since the last reboot.  Additional reports give information on a sampling period of length delay. The process and memory reports are instantaneous in either case. (man vmstat) 
* sysbench
	- sysbench --threads=10 --max-time=300 threads run
* perf
* strace
* htop
* atop 

## 上下文切换

### CPU上下文

Linux是一个多任务操作系统，它支持远大于CPU数量的任务同时运行。当然这些任务实际上并不是真正的在同时运行，而是因为系统在很短的时间内，将CPU轮流分配给它们，造成多任务同时运行的错觉。而在每个任务运行前，CPU都需要知道任务从哪里加载，又从哪里开始运行，需要系统事先帮它设置好CPU寄存器和程序计数器。以上都是CPU在运行任何任务前，必须的依赖环境，因此被叫做**CPU上下文**。

* CPU寄存器：CPU内置的容量小，但速度极快的内存。
* 程序计数器：用来存储CPU正在执行的指令位置，或者即将执行的下一条指令位置。

![cpu_context](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201812/cpu_context.jpg)

### CPU上下文切换

CPU上下文切换，就是把前一个任务的CPU上下文保存起来，然后加载新任务的上下文到这些寄存器和程序计数器，最后再跳转到程序计数器所指的新位置运行新的任务。而这些保存下来的上下文，会存储在系统内核中，并在任务重新调度执行时再次加载进来。这样就能保证任务原来的状态不受影响，让任务看起来还是连续运行的。

根据任务的不同，CPU的上下文切换可以分为几个不同的场景：

* 进程上下文切换
	- Linux按照特权等级，把进程的运行空间分为内核空间和用户空间，分别对应CPU特权等级的Ring 0和Ring 3。
	- 内核空间(Ring 0)，具有最高权限，可以直接访问所有资源。
	- 用户空间(Ring 3)，只能访问受限资源，不能直接访问内存等硬件设备，必须通过系统调用陷入到内核中，才能访问这些特权资源。
	- 进程既可以在用户空间运行，又可以在内核空间中运行。进程再用户空间运行时，被称为**进程的用户态**，而陷入内核空间时，被称为**进程的内核态**。从用户态到内核态的转变，需要通过**系统调用**来完成。
	- 系统调用的过程中，会发生CPU上下文的切换。CPU寄存器里原来是用户态的指令，需要先保存起来，接着为了执行内核态代码，CPU寄存器需要更新为内核态指令，最后才是调到内核态运行内核任务。而系统调用结束后，CPU寄存器需要恢复原来保存的用户态，然后再切换到用户空间，继续运行进程。**所以，一次系统调用的过程，发生了2次CPU上下文切换**。
	- 进程上下文切换和系统调用的区别？
		+ 进程上下文切换，是指从一个进程切换到另一个进程运行。而系统调用过程中，一直是同一个进程在运行。
		+ 进程是由内核来管理和调度的，进程的切换只能发生在内核态。所以，进程的上下文切换不仅包括了虚拟内存，栈，全局变量等用户空间的资源，还包括内核堆栈，寄存器等内核空间的状态。因此，**进程的上下文切换比系统调用时多了一步，在保存当前进程的内核状态和CPU寄存器之前，需要先把该进程的虚拟内存，栈等保存下来。而加载了下一进程的内核态后，还需要刷新进程的虚拟内存和用户栈。**
		+ 根据[Tsuna's blog - How long does it take to make a context switch?]的测试报告，**每次上下文切换都需要几十纳秒到数微秒的CPU时间**。在进程上下文切换次数较多的情况下，这个开销还是很大的，很容易导致CPU将大量时间耗费在寄存器，内核栈以及虚拟内存等资源的保存和恢复上，进而大大缩短了真正运行进程的时间。这个也是导致平均负载升高的一个重要因素。
		+ Linux通过TLB(Translation Lookaside Buffer)来管理虚拟内存到物理内存的映射关系。当虚拟内存更新后，TLB也需要刷新，内存的访问也会随之变慢。特别是在多处理器系统上，缓存是被多个处理器共享的，刷新缓存不仅会影响当前处理器的进程，还会影响共享缓存的其他处理器的进程。
	- 什么时候会切换进程上下文？
		+ 只有在进程调度的时候，才需要进程切换上下文。
		+ Linux为每个CPU都维护了一个就绪队列，将活跃进程(正在运行和正在等待CPU的进程)，按照优先级和等待CPU的时间排序，然后选择最需要CPU的进程运行。
		+ 进程调度的场景：
			++ 进程执行完终止了，它之前使用的CPU会释放出来，这个时候再从就绪队列里，拿一个新的进程来运行。
			++ 为了保证所有进程可以得到公平调度，CPU时间被划分为一段段的时间片，这些时间片再被轮流分配给各个进程。当某个进程的时间片耗尽了，就会被系统挂起，切换到其他正在等待CPU的进程运行。
			++ 进程在系统资源不足(比如，内存不足)时，要等到资源满足后才可以运行，这个时候进程也会被挂起，并由系统调度其他进程运行。
			++ 当进程通过睡眠函数sleep这样的方法将自己主动挂起时，自然也会重新调度。
			++ 当有优先级更高的进程运行时，为了保证高优先级进程的运行，当前进程会被挂起，由高优先级的进程来运行。
			++ 发生硬件中断时，CPU上的进程会被中断挂起，转而执行内核中的中断服务程序。

![process_switch](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201812/process_switch.jpg)


[Tsuna's blog - How long does it take to make a context switch?]: https://blog.tsunanet.net/2010/11/how-long-does-it-take-to-make-context.html

* 线程上下文切换
	- 线程是调度的基本单位，而进程则是资源拥有的基本单位。
	- 内核中的任务调度，实际上的调度对象是线程。
	- 进程只是给线程提供了虚拟内存，全局变量等资源。
	- 当进程只有一个线程时，可以认为进程就等于线程。
	- 当进程拥有多个线程时，这些线程会共享相同的虚拟内存和全局变量等资源，这些资源在线程上下文切换时是不需要修改的。
	- 线程也有自己的私有数据，比如栈和寄存器等，这些在线程上下文切换时也是需要保持的。
	- 线程的上下文切换分为两种情况：
		+ 若前后两个线程属于不同进程，此时，因为资源不共享，所以切换过程就跟进程上下文切换是一样的。
		+ 若前后两个线程属于相同进程，此时，因为虚拟内存是共享的，所以在切换时，虚拟内存这些资源就保持不动，只需要切换线程的私有数据，寄存器等不共享的数据。
	- 同进程内的线程切换，要比多进程间的切换消耗更少的资源。


* 中断上下文切换
	- 为了快速响应硬件的事件，中断处理会打断进程的正常调度和执行，转而调用中断处理程序，响应设备事件。
	- 在打断其他进程时，就需要将进程当前的状态保存下来，这样在中断结束后，进程仍然可以从原来的状态恢复运行。
	- 与进程上下文切换不同，中断上下文切换并不涉及到进程的用户态。即，中断过程打断了一个正处在用户态的进程，也不需要保存和恢复这个进程的虚拟内存，全局变量等用户态资源。
	- 中断上下文，其实只包括内核态中断服务程序执行所必需的状态，包括CPU寄存器，内核堆栈，硬件中断参数等。
	- 对同一个CPU来说，中断处理比进程拥有更高的优先级，所以中断上下文切换并不会与进程上下文切换同时发生。
	- 由于中断会打断正常进程的调度和执行，所以大部分中断处理程序都短小精悍，以便尽可能快的执行结束。



# 开发语言性能比较

https://github.com/famzah/langs-performance

https://blog.famzah.net/2016/09/10/cpp-vs-python-vs-php-vs-java-vs-others-performance-benchmark-2016-q3/


# Web服务性能比较

https://help.dreamhost.com/hc/en-us/articles/215945987-Web-server-performance-comparison

https://drupal.stackexchange.com/questions/71610/nginx-vs-apache-are-there-any-actual-usage-comparisons-and-statistcs-out-there

https://wiki.apache.org/httpd/FAQ#How_does_Apache_httpd_performance_compare_to_other_servers.3F


 # Refer

[Hyper-threading]: https://en.wikipedia.org/wiki/Hyper-threading

[Hyper-Threading 2]: https://whatis.techtarget.com/definition/Hyper-Threading

[When is hyper-threading performance not worth the hassle]: https://searchservervirtualization.techtarget.com/answer/When-should-admins-avoid-using-hyper-threading-technology

[Superscalar processor]: https://en.wikipedia.org/wiki/Superscalar_processor
	
	