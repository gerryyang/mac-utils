---
layout: post
title:  "Linux in Action"
date:   2021-01-02 17:00:00 +0800
categories: cpp
---

* Do not remove this line (it will not be displayed)
{:toc}

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



# 开发相关

## strip & objcopy (对可执行文件瘦身)

```
strip - Discard symbols from object files
```

```
objcopy - copy and translate object file

-g
--strip-debug
Do not copy debugging symbols or sections from the source file.

--only-keep-debug
Strip a file, removing contents of any sections that would not be stripped by --strip-debug and leaving the debugging sections intact. In ELF files, this preserves all note sections in the output.

Note - the section headers of the stripped sections are preserved, including their sizes, but the contents of the section are discarded. The section headers are preserved so that other tools can match up the debuginfo file with the real executable, even if that executable has been relocated to a different address space.

The intention is that this option will be used in conjunction with --add-gnu-debuglink to create a two part executable. One a stripped binary which will occupy less space in RAM and in a distribution and the second a debugging information file which is only needed if debugging abilities are required. The suggested procedure to create these files is as follows:

Link the executable as normal. Assuming that it is called foo then...
Run objcopy --only-keep-debug foo foo.dbg to create a file containing the debugging info.
Run objcopy --strip-debug foo to create a stripped executable.
Run objcopy --add-gnu-debuglink=foo.dbg foo to add a link to the debugging info into the stripped executable.
Note—the choice of .dbg as an extension for the debug info file is arbitrary. Also the --only-keep-debug step is optional. You could instead do this:

Link the executable as normal.
Copy foo to foo.full
Run objcopy --strip-debug foo
Run objcopy --add-gnu-debuglink=foo.full foo
i.e., the file pointed to by the --add-gnu-debuglink can be the full executable. It does not have to be a file created by the --only-keep-debug switch.

Note—this switch is only intended for use on fully linked files. It does not make sense to use it on object files where the debugging information may be incomplete. Besides the gnu_debuglink feature currently only supports the presence of one filename containing debugging information, not multiple filenames on a one-per-object-file basis.

--add-gnu-debuglink=path-to-file
Creates a .gnu_debuglink section which contains a reference to path-to-file and adds it to the output file. Note: the file at path-to-file must exist. Part of the process of adding the .gnu_debuglink section involves embedding a checksum of the contents of the debug info file into the section.

If the debug info file is built in one location but it is going to be installed at a later time into a different location then do not use the path to the installed location. The --add-gnu-debuglink option will fail because the installed file does not exist yet. Instead put the debug info file in the current directory and use the --add-gnu-debuglink option without any directory components, like this:

 objcopy --add-gnu-debuglink=foo.debug
At debug time the debugger will attempt to look for the separate debug info file in a set of known locations. The exact set of these locations varies depending upon the distribution being used, but it typically includes:

* The same directory as the executable.
* A sub-directory of the directory containing the executable
called .debug

* A global debug directory such as /usr/lib/debug.
As long as the debug info file has been installed into one of these locations before the debugger is run everything should work correctly.
```

例如：

```
# 去除目标文件中的符号
$strip objfile
$nm objfile
nm: objfile: no symbols

# 拷贝出一个符号表文件
$objcopy --only-keep-debug mainO3 mainO3.symbol       

# 拷贝出一个不包含调试信息的执行文件
$objcopy --strip-debug mainO3 mainO3.bin


$objcopy --add-gnu-debuglink=mainO3.symbol mainO3
```

[linux中的strip命令简介------给文件脱衣服](https://blog.csdn.net/stpeace/article/details/47090255)






