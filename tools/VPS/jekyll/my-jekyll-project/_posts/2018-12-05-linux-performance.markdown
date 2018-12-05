---
layout: post
title:  "Linux Performance"
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
 
以下是一张总的Linux性能优化的思维导图。

![linux-perf-mind](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201812/linux-perf-mind.png)

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

	
	