---
layout: post
title:  "Linux Kernel in Action"
date:   2024-01-18 12:00:00 +0800
categories: Linux
---

* Do not remove this line (it will not be displayed)
{:toc}


# [Kernel Samepage Merging](https://docs.kernel.org/admin-guide/mm/ksm.html)

`KSM` is a memory-saving de-duplication feature, enabled by `CONFIG_KSM=y`, added to the Linux kernel in `2.6.32`. See `mm/ksm.c` for its implementation, and http://lwn.net/Articles/306704/ and https://lwn.net/Articles/330589/

KSM was originally developed for use with KVM (where it was known as Kernel Shared Memory), to fit more virtual machines into physical memory, by sharing the data common between them. But it can be useful to any application which generates many instances of the same data.

The KSM daemon ksmd periodically scans those areas of user memory which have been registered with it, looking for pages of identical content which can be replaced by a single write-protected page (which is automatically copied if a process later wants to update its content). The amount of pages that KSM daemon scans in a single pass and the time between the passes are configured using [sysfs interface](https://docs.kernel.org/admin-guide/mm/ksm.html#ksm-sysfs)

KSM only merges anonymous (private) pages, never pagecache (file) pages. KSM's merged pages were originally locked into kernel memory, but can now be swapped out just like other user pages (but sharing is broken when they are swapped back in: ksmd must rediscover their identity and merge again).






# Refer

* [The Linux Kernel documentation](https://www.kernel.org/doc/html/v5.8/index.html)
* [x86-specific Documentation](https://www.kernel.org/doc/html/v5.8/x86/index.html)











