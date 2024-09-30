---
layout: post
title:  "Linux Kernel in Action"
date:   2024-01-18 12:00:00 +0800
categories: Linux
---

* Do not remove this line (it will not be displayed)
{:toc}


# 内存相关

参考：[Linux 内存变低会发生什么问题](https://www.51cto.com/article/796844.html)


## 内存的分配机制

内存不是无限的，总有不够用的时候，linux 内核用**三个机制**来处理这种情况：**内存回收**、**内存规整**、**oom-kill**。

当发现内存不足时，内核会先尝试内存回收，从一些进程手里拿回一些页；如果这样还是不能满足申请需求，则触发内存规整；再不行，则触发 oom 主动 kill 掉一个不太重要的进程，释放内存。

**低内存情况下，内核的处理逻辑：**

* 内存申请的核心函数是 `__alloc_pages_nodemask`。`__alloc_pages_nodemask` 会先尝试调用 `get_page_from_freelist` 从伙伴系统的 `freelist` 里拿空闲页，如果能拿到就直接返回。
* 如果拿不到，则进入慢速路径 `__alloc_pages_slowpath`，慢速路径，顾名思义，就是拿得慢一点，需要做一些操作以后再拿。首先， `__alloc_pages_slowpath` 会唤醒 `kswapd``，kswapd` 是一个守护进程，专门进行内存回收操作，执行路径：`kswapd -> balance_pgdat -> kswapd_shrink_node -> shrink_node`，它被唤醒后，会立刻开始进行回收，效率高的话，`freelist` 上会立刻多出很多空闲页。所以 `__alloc_pages_slowpath` 会马上再次尝试从 `freelist` 获取页面，获取成功则直接返回了。
* 若还是失败， `__alloc_pages_slowpath` 则会进入 `direct_reclaim` 阶段。`direct_reclaim` 顾名思义，就是直接内存回收，回收到的页不用放回 `freelist` 再 `get_page_from_freelist` 这么麻烦了，也不用唤醒某个进程帮忙回收，而是由当前进程（current）亲自下场去回收，执行路径：`__alloc_pages_direct_reclaim -> __perform_reclaim -> try_to_free_pages -> do_try_to_free_pages -> shrink_zones -> shrink_node`。如果 `direct_reclaim` 也回收不上来， `__alloc_pages_slowpath` 还会垂死挣扎下，做一下内存规整，尝试把零散的页辗转腾挪，拼成为大 order 页（仅在申请 order>0 的页时有用）。
* 如果还是无法满足要求，则进入 oom-kill 了。

**总结：内存申请时，首先尝试直接从 freelist 里拿；失败了则先唤醒 kswapd 帮忙回收内存；若内存低到让 kswapd 也爱莫能助，则进入 direct reclaim 直接回收内存；若 direct reclaim 也无能为力，则 oom。**

> 上述过程可参考 https://github.com/torvalds/linux/blob/master/mm/page_alloc.c#L4697 代码

``` cpp
/*
 * This is the 'heart' of the zoned buddy allocator.
 */
struct page *__alloc_pages_noprof(gfp_t gfp, unsigned int order,
				      int preferred_nid, nodemask_t *nodemask)
{
	struct page *page;
	unsigned int alloc_flags = ALLOC_WMARK_LOW;
	gfp_t alloc_gfp; /* The gfp_t that was actually used for allocation */
	struct alloc_context ac = { };

	/*
	 * There are several places where we assume that the order value is sane
	 * so bail out early if the request is out of bound.
	 */
	if (WARN_ON_ONCE_GFP(order > MAX_PAGE_ORDER, gfp))
		return NULL;

	gfp &= gfp_allowed_mask;
	/*
	 * Apply scoped allocation constraints. This is mainly about GFP_NOFS
	 * resp. GFP_NOIO which has to be inherited for all allocation requests
	 * from a particular context which has been marked by
	 * memalloc_no{fs,io}_{save,restore}. And PF_MEMALLOC_PIN which ensures
	 * movable zones are not used during allocation.
	 */
	gfp = current_gfp_context(gfp);
	alloc_gfp = gfp;
	if (!prepare_alloc_pages(gfp, order, preferred_nid, nodemask, &ac,
			&alloc_gfp, &alloc_flags))
		return NULL;

	/*
	 * Forbid the first pass from falling back to types that fragment
	 * memory until all local zones are considered.
	 */
	alloc_flags |= alloc_flags_nofragment(zonelist_zone(ac.preferred_zoneref), gfp);

	/* First allocation attempt */
	page = get_page_from_freelist(alloc_gfp, order, alloc_flags, &ac);
	if (likely(page))
		goto out;

	alloc_gfp = gfp;
	ac.spread_dirty_pages = false;

	/*
	 * Restore the original nodemask if it was potentially replaced with
	 * &cpuset_current_mems_allowed to optimize the fast-path attempt.
	 */
	ac.nodemask = nodemask;

	page = __alloc_pages_slowpath(alloc_gfp, order, &ac);

out:
	if (memcg_kmem_online() && (gfp & __GFP_ACCOUNT) && page &&
	    unlikely(__memcg_kmem_charge_page(page, gfp, order) != 0)) {
		__free_pages(page, order);
		page = NULL;
	}

	trace_mm_page_alloc(page, order, alloc_gfp, ac.migratetype);
	kmsan_alloc_page(page, order, alloc_gfp);

	return page;
}
```

## 三条水线


实际上，从 `freelist` 上拿页不是简单地直接拿，而是先检查下该 zone 是否满足水线要求，不满足那就直接失败。内核给内存管理划了三条水线：**MIN**、**LOW**、**HIGH**。三者大小关系从字面即可推断，`MIN < LOW < HIGH`。在首次尝试从 `freelist` 拿页时，门槛水线是 **LOW**；唤醒 `kswapd` 后再次尝试拿页，门槛水线是 **MIN**。

所以，可以简单地认为，可用内存低于 **LOW** 水线时，唤醒 `kswapd`；低于 **MIN** 水线时，进行 `direct reclaim`；而 **HIGH** 水线，是 kswapd 的回收终止线：

![mem_water_line](/assets/images/202409/mem_water_line.png)

从这张图可以看出：

* 当内存容量小于低位水线 LOW，则开始启动 `kswapd` 回收内存
* 当内存容量小于最小水线 MIN，则开启直接回收
* 当内存容量高于高位水线 HIGH，则 `kswapd` 可以休息

refer:

* https://richardweiyang-2.gitbook.io/kernel-exploring/nei-cun-guan-li/00-index-1/02-watermark


## 为什么内存回收时，磁盘 IO 会被打满？

可以看到，`kswapd` 和 `direct_reclaim` 最终都是走到了 `shrink_node`。`shrink_node` 是**内存回收的核心函数**，顾名思义，让整个 node 进行一次“收缩”，把不要的数据清掉，空出空闲页。

![shrink_node](/assets/images/202409/shrink_node.png)

![shrink_node2](/assets/images/202409/shrink_node2.png)

`get_scan_count` 决定本次扫描多少个 **anon page** 和 **file page**。

**anon page** 就是 Anonymous Page，**匿名页**，**是进程的堆栈、数据段等**。内核回收匿名页时，将这些数据进行压缩（压缩比大概为3），然后移动到内存中的一个小角落中 swap 空间，这个过程并没有与磁盘发生交互，因此不会产生 IO，但需要压缩数据，所以耗 CPU。

**file page** 就是**文件页**，**是进程的代码段、映射的文件**。内核回收文件页时，先将“脏”数据回写到磁盘，然后释放掉这些缓存数据，干净的数据则直接释放掉。**这个过程涉及到写磁盘，因此会产生 IO**。

![get_scan_count](/assets/images/202409/get_scan_count.png)

> 所以说，不论开没开 `swap`，内存回收都是倾向于回收 **file page**。如果 **file page** 中有脏页，那内存回收大概率就会产生一些 IO，无非是 IO 量多少罢了。

**以下情况 IO 可能会打满或者暴增：**

* 当前内存不是特别紧张，但 low、min 水线设置得太低，之前一直没怎么触发过内存回收，以致于脏页已经累积到大量，一触发回收，立刻就是回写大量脏页，导致 IO 暴增。
* 内存极度紧张（free 和 available 同时很低）。这种情况下，anon page 远比 file page 多，这意味着可回收的内存很少，内核会对活跃数据下手，一些进程上一秒还用着的数据，这一秒可能就被不幸回收了，但下一秒马上又要被使用，会再次被读入内存。如此，同一份数据，内核就进行了多次回收和读入，IO 就加倍了。


## 为什么低内存有时会引发 hungtask？

低内存时，通常不是个别进程触发了 direct reclaim，而是大量进程都在 direct reclaim。大家都要回写脏页，于是 IO 被打满了。这时候，进程会频繁地被 IO 阻塞，被阻塞的进程为了不占用 CPU，会调用 io_schedule_timeout 或 io_schedule 来挂起自己，直到 IO 完成。这种等待是 D 状态 (TASK_UNINTERRUPTIBLE 状态) 的，一旦超过了 120s 就会触发 hungtask。 当然，这是非常极端的情况，IO 已经完全没救的情况。

大部分时候，IO 虽然打满了，但是总能周转过来，所以这些进程并不会等太久。然而，这些进程若是来自同一个业务，则大概率会访问同一个数据，这就需要通过 mutex、rwsem、semaphore 等同步机制来控制访问行为。而这些同步机制的基本接口都是 uninterruptible 性质的。所谓 uninterruptible 性质，即当进程获取不到同步资源时，直接进入 D 状态等待其他进程释放资源。其他同步资源，rwsem、mutex 等，都有这样的 uninterruptible 性质函数。正常情况下，只要持有同步资源的进程正常运行不卡顿，那么即使有上百个进程来争抢这些同步资源，对于排序靠后的进程来说，时间也是够的，一般不会等待超过 120s。**但在低内存情况下，大家都在等 IO，这些持有资源的进程也不能幸免，引发堵车连锁反应。如果此时同步资源的 waiter 们已累计了几十个甚至上百个，那么就算只有一瞬间的 IO 卡顿，排序靠后的 waiter 也容易等待超过 120s 从而触发 hungtask**。

## 内核中的 hungtask 机制

长期以来，处于 D 状态 (TASK_UNINTERRUPTIBLE 状态)的进程都是让人比较烦恼的问题，处于 D 状态的进程不能接收信号，kill 不掉。在一些场景下，常见到进程长期处于 D 状态，用户对此无能为力，也不知道原因，只能重启恢复。

其实进程长期处于 D 状态肯定是不正常的，内核中设计 D 状态的目的是为了让进程等待 IO 完成，正常情况下 IO 应该会顺利完成，然后唤醒相应的 D 状态进程，即使在异常情况下 (比如磁盘离或损坏、磁阵链路断开等)，IO 处理也是有超时机制的，原理上不会存在永久处于 D 状态的进程。但是就是因为内核代码流程中可能存在一些 bug，或者用户内核模块中的相关机制不合理，可能导致进程长期处于 D 状态，无法唤醒，类似于死锁状态。

针对这种情况，内核中提供了 hung task 机制用于检测系统中是否存在处于 D 状态超过 120s (时长可以设置)的进程，如果存在，则打印相关警告和进程堆栈。如果配置了 hung_task_panic（proc 或内核启动参数），则直接发起 panic，结合 kdump 可以搜集到 vmcore。从内核的角度看，如果有进程处于 D 状态的时间超过了 120s，那肯定已经出现异常了，以此机制来收集相关的异常信息，用于分析定位问题。

> 基本原理

hung task 机制的实现很简单，其基本原理为：创建一个内核线程 (khungtaskd)，定期 120s 唤醒后，遍历系统中的所有进程，检查是否存在处于 D 状态超过 120s (时长可以设置) 的进程，如果存在，则打印相关警告和进程堆栈。如果配置了 hung_task_panic（proc 或内核启动参数），则直接发起 panic。




## 优化低内存处理

我们已经知道了低内存会导致 IO 突增，甚至导致 hungtask，那要如何避免呢？可以从两方面来避免。

* 调整脏页回刷频率。

将平时的脏页回刷频率调高，这样内存回收时，需要回收的脏页就更少，降低 IO 的增量。

```
调低 /proc/sys/vm/dirty_writeback_centisecs
调低 /proc/sys/vm/dirty_background_ratio
```

``` bash
$ cat /proc/sys/vm/dirty_writeback_centisecs
500
$ cat /proc/sys/vm/dirty_background_ratio
10
```

* 调高 low 水线和 min 水线。

调高水线，可以更早地进入内存回收逻辑，这样可以将 free 维持在一个较高水平，避免陷入极端场景。 由于 low 和 min 同时受 min_free_kbytes 管控，所以可以直接调整 min_free_kbytes 值。

```
调高 /proc/sys/vm/min_free_kbytes
```

``` bash
$ cat /proc/sys/vm/min_free_kbytes
45861
```

## 参考

* [hungtask 机制分析](https://linux.laoqinren.net/kernel/hungtask/)




# [Kernel Samepage Merging](https://docs.kernel.org/admin-guide/mm/ksm.html)

`KSM` is a memory-saving de-duplication feature, enabled by `CONFIG_KSM=y`, added to the Linux kernel in `2.6.32`. See `mm/ksm.c` for its implementation, and http://lwn.net/Articles/306704/ and https://lwn.net/Articles/330589/

KSM was originally developed for use with KVM (where it was known as Kernel Shared Memory), to fit more virtual machines into physical memory, by sharing the data common between them. But it can be useful to any application which generates many instances of the same data.

The KSM daemon ksmd periodically scans those areas of user memory which have been registered with it, looking for pages of identical content which can be replaced by a single write-protected page (which is automatically copied if a process later wants to update its content). The amount of pages that KSM daemon scans in a single pass and the time between the passes are configured using [sysfs interface](https://docs.kernel.org/admin-guide/mm/ksm.html#ksm-sysfs)

KSM only merges anonymous (private) pages, never pagecache (file) pages. KSM's merged pages were originally locked into kernel memory, but can now be swapped out just like other user pages (but sharing is broken when they are swapped back in: ksmd must rediscover their identity and merge again).






# Refer

* https://github.com/torvalds/linux
* [The Linux Kernel documentation](https://www.kernel.org/doc/html/v5.8/index.html)
* [x86-specific Documentation](https://www.kernel.org/doc/html/v5.8/x86/index.html)
* https://richardweiyang-2.gitbook.io/kernel-exploring











