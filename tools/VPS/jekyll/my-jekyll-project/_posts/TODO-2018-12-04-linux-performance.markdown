---
layout: post
title:  "Linux性能调优"
date:   2018-12-04 20:00:00 +0800
categories: Linux
---

* Do not remove this line (it will not be displayed)
{:toc}



# CPU

通过`cat /proc/cpuinfo`可以看到cpu的相关信息。

* CPU：独立的中央处理单元，体现在主板上是有多个CPU的`槽位`。  
* CPU cores：在每一个CPU上，都可能有多个核(core)，每一个核中都有独立的一套ALU，FPU，Cache等组件，所以这个概念也被称作`物理核`。  
* processor：这个主要得益于超线程(HTT)技术，可以让一个物理核模拟出多个`逻辑核`，即processor。  
* 超线程([Hyper-threading])，是一种用于提升CPU计算并行度的处理器技术，用一个物理核模拟两个逻辑核。
这两个逻辑核拥有自己的中断、状态，但是共用物理核的计算资源。超线程技术旨在提高CPU计算资源的使用率，从而提高计算并行度。超线程技术基于这样一个现实，那就是大多数程序运行时，CPU资源并没有得到充分的利用。比如CPU缓存未命中、分支预测错误或者等待数据时，CPU中的计算资源其实是闲置的。超线程技术，可以通过硬件指令，将这些闲置的CPU资源，调度给其他的指令，从而整体上提高CPU的资源利用率。有研究表明，超线程依赖于操作系统对CPU的调度。但是如果应用程序开启了CPU绑定功能，则有可能破坏这种调度的完整性，反而带来性能损失。

更多：
[Hyper-Threading]
[When is hyper-threading performance not worth the hassle?]
[Superscalar processor]

[Hyper-threading]: https://en.wikipedia.org/wiki/Hyper-threading
[Hyper-Threading]: https://whatis.techtarget.com/definition/Hyper-Threading
[When is hyper-threading performance not worth the hassle?]: https://searchservervirtualization.techtarget.com/answer/When-should-admins-avoid-using-hyper-threading-technology
[Superscalar processor]: https://en.wikipedia.org/wiki/Superscalar_processor

  

	
	