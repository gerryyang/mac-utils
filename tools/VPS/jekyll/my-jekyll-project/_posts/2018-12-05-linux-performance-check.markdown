---
layout: post
title:  "Linux Performance诊断"
date:   2018-12-05 20:00:00 +0800
categories: Linux
---

* Do not remove this line (it will not be displayed)
{:toc}

----------------


# 性能指标

* 吞吐 
* 延时

从应用角度看，需要做到：高并发，低延时。
从资源角度看，需要关心：资源使用率，饱和度。

![linux-performance](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201812/linux-performance.jpeg)

随着应用负载的增加，系统资源的使用也会升高，甚至达到极限。而性能问题的本质，就是系统资源已经达到瓶颈，导致请求处理不过来。性能分析就是找出应用或系统的瓶颈，并设法去避免或者缓解它们，从而更高效地利用系统资源处理更多的请求。

Brendan Gregg描述的 [Linux性能工具图谱]，可以帮助我们在发现Linux出现性能问题后，应该用什么工具来定位和分析。

[Linux性能工具图谱]: http://brendangregg.com

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


## 上下文切换

* 使用`vmstat`来查询系统总体上的上下文切换情况。
* 使用`pidstat -wt`来查询每个进程的上下文切换情况。
* 使用`sysbench`来模拟系统多线程调度切换的情况。
* 上下文切换频率是多少次才算正常？这个数值取决于系统本身的CPU性能。如果系统的上下文切换次数比较稳定，从数百到一万以内，都应该算是正常的。但当超过一万次，或者出现数量级的增长时，就可能出现了性能问题。


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
			- 进程执行完终止了，它之前使用的CPU会释放出来，这个时候再从就绪队列里，拿一个新的进程来运行。
			- 为了保证所有进程可以得到公平调度，CPU时间被划分为一段段的时间片，这些时间片再被轮流分配给各个进程。当某个进程的时间片耗尽了，就会被系统挂起，切换到其他正在等待CPU的进程运行。
			- 进程在系统资源不足(比如，内存不足)时，要等到资源满足后才可以运行，这个时候进程也会被挂起，并由系统调度其他进程运行。
			- 当进程通过睡眠函数sleep这样的方法将自己主动挂起时，自然也会重新调度。
			- 当有优先级更高的进程运行时，为了保证高优先级进程的运行，当前进程会被挂起，由高优先级的进程来运行。
			- 发生硬件中断时，CPU上的进程会被中断挂起，转而执行内核中的中断服务程序。

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


## CPU使用率

* CPU使用率，是单位时间内CPU使用情况的统计，以百分比的方式展示。
	- 总体CPU使用率
		+ 逻辑上：**CPU使用率 = 1 - 空闲时间/总CPU时间**
		+ 实际计算(/proc/stat)：**平均CPU使用率 = 1 - (空闲时间_tend - 空闲时间_tbeg)/(总CPU时间_tend - 总CPU时间_tbeg)**
	- 单进程CPU使用率
		+ /proc/$pid/stat (计算方法类似)
	- 性能分析工具给出的都是**间隔一段时间的平均CPU使用率**。注意，使用不同工具对比CPU使用率结果可能不一样，比如，top默认使用3秒间隔，而ps使用却是进程的整个生命周期。
	- 相关工具：
		+ top/ps/pidstat


* `节拍率CONFIG_HZ`(内核选项)。为了维护CPU时间，Linux通过事先定义的`节拍率(内核中表示为HZ)`，触发时间中断，并使用全局变量`Jiffies`记录开机以来的节拍数。每发生一次时间中断，`Jiffies`的值就加1。节拍率HZ是内核的可配选项，可以设置为100,250,1000等，不同的系统可能设置不同数值。

```
# 通过内核选项查看节拍率的配置值，代表每秒钟触发250次时间中断
$grep "CONFIG_HZ=" /boot/config-$(uname -r)
CONFIG_HZ=250
```

* `节拍率USER_HZ`(用户空间)，它固定为100(即，1/100秒)，用户空间程序并不需要关心内核中HZ被设置成了多少，因为它看到的总是固定值USER_HZ。
* `/proc/stat`提供了关于系统的CPU和任务统计信息。
	- man proc
	- 第一列是CPU编号。而第一行没有编号的cpu表示是所有cpu的累加。其他列则表示不同场景下cpu的累加节拍数，它的单位是USER_HZ(即，1/100秒，10ms)
	- 每一列的含义：
		+ `user`(缩写为`us`)，代表`用户态CPU时间`。注意，它不包括nice时间，但包括了guest时间。
		+ `nice`(缩写为`ni`)，代表`低优先级用户态CPU时间`。也就是进程的nice值被调整为1-19之间时的CPU时间。(nice可取值范围是`-20到19`，数值越大，优先级反而越低)。
		+ `system`(缩写为`sys`)，代表`内核态CPU时间`。
		+ `idle`(缩写为`id`)，代表`CPU空闲时间`。注意，它不包括等待I/O的时间(iowait)。
		+ `iowait`(缩写为`wa`)，代表`等待I/O的CPU时间`。
		+ `irq`(缩写为`hi`)，代表`处理硬中断的CPU时间`。
		+ `softirq`(缩写为`si`)，代表`处理软中断的CPU时间`。
		+ `steal`(缩写为`st`)，代表`当系统运行在虚拟机中的时候，被其他虚拟机占用的CPU时间`。
		+ `guest`(缩写为`guest`)，代表`通过虚拟化运行其他操作系统的时间，也就是运行虚拟机的CPU时间`。

```
$cat /proc/stat | grep ^cpu
cpu  578330344 13474169 274296516 29468768395 1704635 1707 8800004 559876 0 0
cpu0 76800541 3786 41365936 3662395305 1410425 514 1247852 153728 0 0
cpu1 73694193 810 36164076 3677516853 46311 1192 2354244 148069 0 0
cpu2 71767326 802 33430992 3688209240 40770 0 860220 43860 0 0
cpu3 71783262 568 32693404 3690062413 41661 0 875085 42675 0 0
cpu4 71347242 617 32365060 3691242578 39912 0 854235 42197 0 0
cpu5 71362048 566 32447788 3691034602 38925 0 893029 42947 0 0
cpu6 71066492 748 32433314 3691442169 38796 0 855324 42912 0 0
cpu7 70509238 13466270 33395943 3676865232 47833 0 860013 43484 0 0
```

## 中断

* 为什么要有中断？举个生活的例子，送快递，通过电话的方式。
* 中断是一种异步的事件处理机制，可以提高系统的并发处理能力。
* 由于中断处理程序会打断其他进程的运行，所以，为了减少对正常运行调度的影响，中断处理程序就需要尽可能快地运行。
* 中断可能会丢失。例子，多个人送快递，第二个人的电话可能占线。
* 为了解决中断处理程序执行过长和中断丢失的问题，Linux将中断处理过程分成了两个阶段：`上半部`和`下半部`。
	- 上半部用来快速处理中断。它在中断禁止模式下运行，主要处理跟硬件紧密相关的，或时间敏感的工作。
	- 下半部用来延迟处理上半部未完成的工作，通常以内核线程的方式运行。
	- 例子：取快递。上半部是接电话告诉配送员在哪里见面然后就可以挂电话了；下半部是取快递的动作。

* 上半部，直接处理硬件请求，也就是`硬中断`，特点是执行快。
* 下半部，是由内核触发，也就是`软中断(softirq)`，特点是延迟执行。
* 实际上，上半部会打断CPU正在执行的任务，然后立即执行中断处理程序。而下半部以内核线程的方式执行，并且每个CPU都对应一个软中断内核线程，名字为`ksoftirqd/CPU编号`，比如，0号CPU对应的软中断内核线程的名字就是`ksoftirqd/0`。
* 一些内核自定义的事件也属于软中断，比如，内核调度和RCU锁(Read-Copy Update)。

### 查看软中断

* `/proc/softirqs` 提供了软中断的运行情况。可以看到各种类型软中断在不同CPU上的累积运行次数。
* 软中断，包括了10个类别：
	- `NET_RX`，网络接收中断
	- `NET_TX`，网络发送中断
	- `TIMER`，定时中断
	- `SCHED`，内核调度
	- `RCU`，RCU锁
* 正常情况，同一种中断在不同CPU上的累积次数应该相差不大。

```
$ cat /proc/softirqs  
                    CPU0       CPU1       CPU2       CPU3       CPU4       CPU5       CPU6       CPU7       
          HI:          0          0          0          0          0          0          0          0
       TIMER:  713323306  597120381  575130430  543948933  542820584  527320354  551004569  529250593
      NET_TX:    3942016       2162       9436       8941       9259       9053       9259       9201
      NET_RX: 3544603084 3397281158  702109101  680604389  682339441  658940411  701453549  697373610
       BLOCK:          0          0          0          0          0          0          0          0
BLOCK_IOPOLL:          0          0          0          0          0          0          0          0
     TASKLET:        104         93          1          0          0          0          0          0
       SCHED:  334322040  201626971  182193519  166406020  158340210  155001620  162949961  160917624
     HRTIMER:    3770790    3067883    3294058    2814030    3082644    5280147    2529713    2751330
         RCU:  505317878  414070615  405384406  383304364  379485156  369748897  386444807  374074340
```

如何查看**中断次数的变化速率**？

```
# 从高亮部分可以直观看出哪些内容变化的更快
watch -d cat /proc/softirqs
```




### 查看内核线程

内核线程的名字外面都有中括号，说明ps命令无法获取它们的命令行参数。

```
$ ps aux|grep softirq
root         3  0.0  0.0      0     0 ?        S    May15   2:56 [ksoftirqd/0]
root        29  0.0  0.0      0     0 ?        S    May15   2:08 [ksoftirqd/1]
root        34  0.0  0.0      0     0 ?        S    May15   0:39 [ksoftirqd/2]
root        39  0.0  0.0      0     0 ?        S    May15   0:36 [ksoftirqd/3]
root        44  0.0  0.0      0     0 ?        S    May15   0:33 [ksoftirqd/4]
root        49  0.0  0.0      0     0 ?        S    May15   0:37 [ksoftirqd/5]
root        54  0.0  0.0      0     0 ?        S    May15   0:36 [ksoftirqd/6]
root        59  0.0  0.0      0     0 ?        S    May15   0:34 [ksoftirqd/7]
```

### 查看硬中断

* `/proc/interrupts` 提供了硬中断的运行情况。

```
$ cat /proc/interrupts 
           CPU0       CPU1       CPU2       CPU3       CPU4       CPU5       CPU6       CPU7       
  0:         33          0          0          0          0          0          0          0   IO-APIC-edge      timer
  1:         10          0          0          0          0          0          0          0   IO-APIC-edge      i8042
  4:        325          0          0          0          0          0          0          0   IO-APIC-edge      serial
  8:          1          0          0          0          0          0          0          0   IO-APIC-edge      rtc0
  9:          0          0          0          0          0          0          0          0   IO-APIC-fasteoi   acpi
 10:          0          0          0          0          0          0          0          0   IO-APIC-fasteoi   virtio3
 40:          0          0          0          0          0          0          0          0   PCI-MSI-edge      virtio1-config
 41:   16669006          0          0          0          0          0          0          0   PCI-MSI-edge      virtio1-requests
 42:          0          0          0          0          0          0          0          0   PCI-MSI-edge      virtio2-config
 43:   59166530          0          0          0          0          0          0          0   PCI-MSI-edge      virtio2-requests
 44:          0          0          0          0          0          0          0          0   PCI-MSI-edge      virtio0-config
 45:    6689988          0          0          0          0          0          0          0   PCI-MSI-edge      virtio0-input.0
 46:          0          0          0          0          0          0          0          0   PCI-MSI-edge      virtio0-output.0
 47: 2093616484          0          0          0          0          0          0          0   PCI-MSI-edge      peth1-TxRx-0
 48:          5 2045859720          0          0          0          0          0          0   PCI-MSI-edge      peth1-TxRx-1
 49:         81          0          0          0          0          0          0          0   PCI-MSI-edge      peth1
NMI:          0          0          0          0          0          0          0          0   Non-maskable interrupts
LOC: 2936184495  965056330 1641503935 1442909354 1106218217 1180680219 1110309682 1158481374   Local timer interrupts
SPU:          0          0          0          0          0          0          0          0   Spurious interrupts
PMI:          0          0          0          0          0          0          0          0   Performance monitoring interrupts
IWI:   53775871   47387196   47737572   44243915   43451907   44050411   44488100   44338708   IRQ work interrupts
RTR:          0          0          0          0          0          0          0          0   APIC ICR read retries
RES: 1198594562  964481221  966552350  902484234  871559958  863341236  864670220  858000383   Rescheduling interrupts
CAL: 4294967071       4438  430547422  419910155  438252892  417825974  443016275  448064960   Function call interrupts
TLB: 1206563963   65932469 1378887038 1028081848  891541016  853171175  829911769  751250837   TLB shootdowns
TRM:          0          0          0          0          0          0          0          0   Thermal event interrupts
THR:          0          0          0          0          0          0          0          0   Threshold APIC interrupts
MCE:          0          0          0          0          0          0          0          0   Machine check exceptions
MCP:      65623      65623      65623      65623      65623      65623      65623      65623   Machine check polls
ERR:          0
MIS:          0
```


# 性能测试工具

* stress
	- 一个Linux系统压力测试工具。
	- apt install stress 
	- 模拟一个CPU使用率100%的场景。stress --cpu 1 --timeout 600 
	- 模拟I/O压力，不停地执行sync。stress-ng -i 1 --hdd 1 --timeout 600 (-i表示调用sync, --hdd表示读写临时文件)
	- 模拟大量进程场景。stress -c 8 --timeout 600
	- 动态且高亮显示变化的区域 watch -d uptime

* top
	- 显示了系统总体的CPU和内存使用情况，以及各个进程的资源使用情况。
	- top默认显示的是所有CPU的平均值，按数字1可以切换到每个CPU的使用率。('1' single/separate states)
	- `-H`显示线程。('H' Threads)
	- `-c`显示参数列表。 ('c' Cmd name/line)
	- 列字段含义：
		+ S列(Status)表示进程的状态

| 进程状态(部分) | 含义 | 发生场景
| -- | -- | --
| R (Running) | 进程在CPU的就绪队列中，正在运行或正在等待运行 | 
| D (Disk Sleep) | 不可中断状态睡眠，一般表示进程正在与硬件交互，并且交互过程不允许被其他进程或中断打断 | 系统或硬件发生了故障，进程可能会在不可中断状态保持很久，或导致系统中出现大量不可中断进程
| S (Interruptible Sleep) | 可中断状态睡眠，表示进程因为等待某个事件而被系统挂起 |
| Z (Zombie) | 僵尸进程，表示进程实际上已经结束了，但是父进程还没有回收它的资源 | 当一个进程创建了子进程后，它应该通过系统调用wait或waitpid等待子进程结束，回收子进程的资源。而子进程在结束时，会向它的父进程发送SIGCHLD信号，父进程可以注册SIGCHLD信号处理函数异步回收子进程的资源。如果父进程没有处理子进程的状态，子进程就会变成僵尸进程。(打个比喻：父亲应该一直对儿子负责，善始善终，如果不作为或者跟不上，都会导致问题少年的出现)。通常，僵尸进程持续的时间都比较短，在父进程回收它的资源后就会消亡；或者在父进程退出后，由init进程回收后也会消亡。但是，如果父进程没有处理子进程的终止还一直保持运行状态，那么子进程就会一直处于僵尸状态，大量的僵尸进程会用尽PID进程号导致新进程不能创建
| I (Idle) | 空闲状态，用在不可中断睡眠的内核线程上。注意，硬件交互导致的不可中断进程用D表示，但对某些内核线程来说，它们有可能实际上并没有任何负载，用Idle是为了区分这种情况。D状态的进程会导致平均负载升高，但I状态的进程却不会 |
| T (Stopped/Traced) | 进程处于暂停或者跟踪状态。向一个进程发送SIGSTOP信号，它会变成暂停状态(Stopped)，再向它发送SIGCONT信号，进程又会恢复运行；或者当用gdb断点中断进程后，进程会变成跟踪状态 |
| X (Dead) | 进程已经消亡，不会在top或ps中看到 | 



```
$ top
top - 11:41:27 up 224 days, 19:54,  1 user,  load average: 0.01, 0.06, 0.06
Tasks: 140 total,   1 running, 139 sleeping,   0 stopped,   0 zombie
Cpu(s):  0.1%us,  0.2%sy,  0.0%ni, 99.7%id,  0.0%wa,  0.0%hi,  0.0%si,  0.0%st
Mem:  12174404k total,  4983532k used,  7190872k free,   367820k buffers
Swap:  2104508k total,        0k used,  2104508k free,  3361592k cached

  PID USER      PR  NI  VIRT  RES  SHR S %CPU %MEM    TIME+  COMMAND                                                                                                                                    
    1 root      20   0 22392 1584 1280 S  0.0  0.0   0:15.06 init                                                                                                                            
    2 root      20   0     0    0    0 S  0.0  0.0   0:01.42 kthreadd                                                                                                                        
    3 root      20   0     0    0    0 S  0.0  0.0   0:39.30 ksoftirqd/0                                                                                                                     
    5 root       0 -20     0    0    0 S  0.0  0.0   0:00.00 kworker/0:0H                                                                                                                    
    7 root      RT   0     0    0    0 S  0.0  0.0   0:36.80 migration/0                                                                                                                     
    8 root      20   0     0    0    0 S  0.0  0.0   0:00.00 rcu_bh                                                                                                                          
    9 root      20   0     0    0    0 S  0.0  0.0   0:00.00 rcuob/0         
```

* ps
	- 显示了每个进程的资源使用情况。

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
	- 数据指标：
		+ cswch (voluntary context switches)表示每秒自愿上下文切换的次数。是指进程无法获取所需资源，导致的上下文切换(例如，I/O，内存等系统资源不足时)。
		+ nvcswch (non-voluntary context switches)表示每秒非自愿上下文切换的次数。是指进程由于时间片已到等原因，被系统强制调度，而发生的上下文切换(例如，大量进程都在争抢CPU时)。

```
$ pidstat -wt 5
Linux 3.10.107-1-tlinux2_kvm_guest-0046 (VM_4_14_centos)        12/20/18        _x86_64_        (8 CPU)

09:26:57      UID      TGID       TID   cswch/s nvcswch/s  Command
09:27:02        0         1         -     10.47      0.00  systemd
09:27:02        0         -         1     10.47      0.00  |__systemd
09:27:02        0         2         -      0.20      0.00  kthreadd
09:27:02        0         -         2      0.20      0.00  |__kthreadd
09:27:02        0         3         -      7.11      0.00  ksoftirqd/0
09:27:02        0         -         3      7.11      0.00  |__ksoftirqd/0
09:27:02        0         7         -      1.19      0.00  migration/0
09:27:02        0         -         7      1.19      0.00  |__migration/0
09:27:02        0        17         -     91.90      0.00  rcu_sched
09:27:02        0         -        17     91.90      0.00  |__rcu_sched
09:27:02        0        18         -     27.87      0.00  rcuos/0
09:27:02        0         -        18     27.87      0.00  |__rcuos/0
09:27:02        0        19         -     27.87      0.00  rcuos/1
09:27:02        0         -        19     27.87      0.00  |__rcuos/1
```

* vmstat
	- vmstat输出的第一行数据为什么和其他行差别巨大？回答：The  first  report  produced gives averages since the last reboot.  Additional reports give information on a sampling period of length delay. The process and memory reports are instantaneous in either case. (man vmstat) 
	- 数据指标：
		+ cs (context switch)是每秒上下文切换的次数。
		+ in (interrupt)是每秒中断的次数。
		+ r (Running or Runnable)是就绪队列的长度，也就是正在运行和等待CPU的进程数。
		+ b (Blocked)是处于不可中断睡眠状态的进程数。

```
# 每隔5秒输出1组数据
$vmstat 5
procs -----------memory---------- ---swap-- -----io---- -system-- -----cpu------
 r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa st
 0  0 1032316 6471492 387968 5092564    0    0     0     0    0    0  0  0 99  1  0
 0  0 1032316 6471860 387984 5092548    0    0     0   122  312 10741  0  0 99  1  0
 0  0 1032316 6472252 387996 5092536    0    0     0    89  302 10531  0  0 99  1  0
 0  0 1032316 6471632 388012 5093548    0    0     0   242  326 10559  0  0 99  1  0
```

* sysbench
	- sysbench --num-threads=10 --max-time=300 --max-requests=1000000 --test=threads --thread-yields=1000 --thread-locks=8 run (CentOS)

* proc
	- watch -d cat /proc/interrupts 查看中断使用情况，其中`RES`表示**重调度中断(唤醒空闲状态的CPU来调度新的任务运行)**  


![proc_interrupts](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201812/proc_interrupts.png)



* perf
	+ 实时统计。
		- perf top (实时显示占用CPU时钟最多的函数或指令，用来查找热点函数，缺点是无法离线保存数据用于事后分析)
		- 第一行包含三个数据，分别是：采样数(Samples)，事件类型(event)，事件总量(Event count)
		- 表格中，第一行包含四列：
			= 第一列`Overhead`，是该符号的性能事件在所有采样中的比例，用百分比来表示。
			= 第二列`Shared`，是该函数或指令所在的动态共享对象(Dynamic Shared Object)，如内核，进程名，动态链接库名，内核模块名等。
			= 第三列`Object`，是动态共享对象的类型。比如，`[.]`表示用户空间的可执行程序，或者动态链接库；`[k]`表示内核空间。
			= 第四列`Symbol`，是符号名(即，函数名)。当函数名未知时，用十六进制的地址来表示。
	+ 离线分析。
		- perf record -ag 记录性能事件，等待x秒后Ctrl+C退出
		- perf report 查看报告

![perf_top](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201812/perf_top.png)

![perf_call_graph](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201812/perf_call_graph.png)


refer: [brendangregg: perf Examples]


[brendangregg: perf Examples]: http://www.brendangregg.com/perf.html

* strace

* ab
	- ab -c 10 -n 10000 http://ip:port

* sar
	+ Collect, report, or save system activity information.
	+ 一个系统活动报告工具，既可以实时查看系统的当前活动，又可以配置保存和报告历史统计数

```
# 查看CPU使用率
sar -u 1 

# -n DEV表示显示网络收发的报告,  间隔1秒输出一组数据
$sar -n DEV 1
Linux 3.10.94-1-tlinux2_kvm_guest-0019.tl2 (TENCENT64.site)     12/31/18        _x86_64_        (8 CPU)

17:41:55        IFACE   rxpck/s   txpck/s    rxkB/s    txkB/s   rxcmp/s   txcmp/s  rxmcst/s
17:41:56         eth0      0.00      0.00      0.00      0.00      0.00      0.00      0.00
17:41:56        peth1      7.00     20.00      0.85      4.85      0.00      0.00      0.00
17:41:56         eth1      7.00     20.00      0.85      4.85      0.00      0.00      0.00
17:41:56       peth21      0.00      0.00      0.00      0.00      0.00      0.00      0.00
17:41:56           lo      0.00      0.00      0.00      0.00      0.00      0.00      0.00
```
第一列，是报告的时间
IFACE，是网卡
rxpck/s和txpck/s，分别是每秒接收，发送的网络帧数
rxkB/s和txkB/s，分别是每秒接收，发送的千字节数

* hping3
	+ 一个可以构造TCP/IP协议数据包的工具，可以对系统进行安全审计，防火墙测试等

```
# 模拟SYN FLOOD攻击
# -S参数表示设置TCP协议的SYN(同步序列号)，-p表示目的端口为80
# -i u100表示每隔100微妙发送一个网络帧
# 注意：如果在实践过程中现象不明显，可以尝试把100调小
$ hping3 -S -p 80 -i u100 192.168.0.30
```

PS: SYN FLOOD问题最简单的解决方法，是从交换机或硬件防火墙中封掉来源IP，这样SYN FLOOD网络帧就不会发送到服务器中。

* tcpdump
	+ 一个常用的网络抓包工具，用于分析网络问题	
	+ tcpdump -i eth0 -n tcp  port 80

* htop
* atop 


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
	
	