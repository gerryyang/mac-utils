---
layout: post
title:  "Linux in Action"
date:   2021-01-02 17:00:00 +0800
categories: Linux
---

* Do not remove this line (it will not be displayed)
{:toc}

# 网络

## Tracing socket (file) descriptor back to bound address

```
lsof | grep pid.*sd.*IP
```

* `pid` is to be replaced by the program process id
* `sd` is to be replaced by the socket descriptor as shown by strace

例子：

```
$strace -p14407 -e recvfrom
Process 14407 attached
recvfrom(55, 0x65c86000, 667104, 0, 0, 0) = -1 EAGAIN (Resource temporarily unavailable)
recvfrom(56, 0x65f1e000, 667104, 0, 0, 0) = -1 EAGAIN (Resource temporarily unavailable)
...

$lsof | grep 14407.*55.*IP
gamesvr   14407       gerryyang   55u     IPv4         1098817885        0t0        TCP qsm_cloud_dev-15:51496->9.143.64.189:8856 (ESTABLISHED)
gamesvr   14407 14409 gerryyang   55u     IPv4         1098817885        0t0        TCP qsm_cloud_dev-15:51496->9.143.64.189:8856 (ESTABLISHED)
gamesvr   14407 14410 gerryyang   55u     IPv4         1098817885        0t0        TCP qsm_cloud_dev-15:51496->9.143.64.189:8856 (ESTABLISHED)
...
```

# 进程

## Process State (ps/top)


```
PROCESS STATE CODES:

Here are the different values that the s, stat and state output specifiers
(header "STAT" or "S") will display to describe the state of a process.

D Uninterruptible sleep (usually IO)

R Running or runnable (on run queue)

S Interruptible sleep (waiting for an event to complete)

T Stopped, either by a job control signal or because it is being traced.

W paging (not valid since the 2.6.xx kernel)

X dead (should never be seen)

Z Defunct ("zombie") process, terminated but not reaped by its parent.

For BSD formats and when the stat keyword is used, additional characters may be displayed:

< high-priority (not nice to other users)

N low-priority (nice to other users)

L has pages locked into memory (for real-time and custom IO)

s is a session leader

l is multi-threaded (using CLONE_THREAD, like NPTL pthreads do) 

+ is in the foreground process group
```

# 问题定位

## dmesg

dmesg is used to examine or control the kernel ring buffer. The default action is to display all messages from the kernel ring buffer.

```
# 显示可读时间, 定位内存异常错误
$dmesg -T

[Sun Dec 13 23:41:22 2020] Out of memory: Kill process 15395 (cc1plus) score 7 or sacrifice child
[Sun Dec 13 23:41:22 2020] Killed process 15395 (cc1plus) total-vm:168940kB, anon-rss:119492kB, file-rss:0kB
```

## gstack

gstack  attaches  to  the  active  process named by the pid on the command line, and prints out an execution stack trace.  If ELF symbols exist in the binary (usually the case unless you have run strip(1)), then symbolic addresses are printed as well.

```
$gstack 31310
Thread 2 (Thread 0x7f131c596700 (LWP 31315)):
#0  0x00007f13d2a6b1bd in nanosleep () from /lib64/libc.so.6
#1  0x00007f13d2a9bed4 in usleep () from /lib64/libc.so.6
#2  0x00000000008da604 in g6::tracing::TraceThread::Run (this=0x2a4f330) at common/tracing/tracing.cc:55
#3  0x00000000008eea1a in g6::ThreadEntry (arg=<optimized out>) at common/base/thread.cpp:12
#4  0x00007f13d3597e25 in start_thread () from /lib64/libpthread.so.0
#5  0x00007f13d2aa435d in clone () from /lib64/libc.so.6
Thread 1 (Thread 0x7f13d3cb9780 (LWP 31310)):
#0  0x00007f13d359bcf2 in pthread_cond_timedwait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
#1  0x000000000066feed in Get (evt=<optimized out>, this=<optimized out>, pqueueid=<optimized out>, msec=<optimized out>) at ./source/baseagent/tmsg/queue_event_and_observer.h:449
#2  Polling (evt=<optimized out>, pqueueid=<optimized out>, msec=<optimized out>, this=<optimized out>) at ./source/baseagent/tmsg/queue_event_and_observer.h:212
#3  tmsg::InternalPolling (event_notify_id=<optimized out>, msec_timeout=1, pqueueid=<optimized out>, pevent=<optimized out>, perr_info=<optimized out>) at source/baseagent/tmsg/tmsg_internal_api.cpp:703
#4  0x000000000067a4dc in tmsg::TmsgPolling (evt_handle=819702518555607043, msec_timeout=<optimized out>, pqueue_handle=<optimized out>, pevent=0x28fdf8c, perr_info=0x28fdfa8) at source/baseagent/tmsg/tmsg_api.cpp:538
#5  0x000000000053f6fd in baseagent::PollEvent (notify_handle=819702518555607043, msec_timeout=msec_timeout@entry=1, ppdst_instance=ppdst_instance@entry=0x7fff5fe21d18, pevent_mask=pevent_mask@entry=0x28fdf8c, perr_info=0x0, perr_info@entry=0x28fdfa8) at source/baseagent/baseagent_api/base_agent_api.cpp:560
#6  0x0000000000510029 in g6::TbusppMessage::Poll (this=0x28fdef0, handle=handle@entry=0x7fff5fe21d90, event=event@entry=0x7fff5fe21d88, timeout=timeout@entry=1) at common/framework/tbuspp/tbuspp_message.cpp:770
#7  0x00000000004ee766 in g6::NameProcessorDispatcher::ProcessOnce (this=0x288e200, wait_ms=1, msg_expired_us=1608693627268946, poll_wait_time=@0x7fff5fe21e28: 0) at common/framework/name_processor.cpp:193
#8  0x00000000004dcf97 in g6::G6Framework::Process (this=this@entry=0x288a0f0, wait_ms=wait_ms@entry=1) at common/framework/framework.cpp:215
#9  0x00000000004e3342 in g6::G6Framework::Serve (this=0x288a0f0, event_handler=0x7fff5fe21f80) at common/framework/framework.cpp:158
#10 0x0000000000416cb0 in main (argc=<optimized out>, argv=<optimized out>) at base_server/base_so_loader.cc:216
```





