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



# 编译相关

## 编译优化(gcc/clang)


编译环境：x86-64 gcc 4.8.5
在线编译工具：https://gcc.godbolt.org/
GCC优化选项说明：[Options That Control Optimization](https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/Optimize-Options.html#Optimize-Options)


gcc默认使用`-O0`优化级别，可以通过`gcc -Q --help=optimizers -O<number>`查看每个优化级别的不同。

```
$gcc -Q --help=optimizers -O0 | head -n20
The following options control optimizations:
  -O<number>                  
  -Ofast                      
  -Og                         
  -Os                         
  -faggressive-loop-optimizations       [enabled]
  -falign-functions                     [disabled]
  -falign-jumps                         [disabled]
  -falign-labels                        [disabled]
  -falign-loops                         [disabled]
  -fasynchronous-unwind-tables          [enabled]
  -fbranch-count-reg                    [enabled]
  -fbranch-probabilities                [disabled]
  -fbranch-target-load-optimize         [disabled]
  -fbranch-target-load-optimize2        [disabled]
  -fbtr-bb-exclusive                    [disabled]
  -fcaller-saves                        [disabled]
  -fcombine-stack-adjustments           [disabled]
  -fcommon                              [enabled]
  -fcompare-elim                        [disabled]
```


测试代码：

```
#include <cstdio>

int func()
{
        int x = 3;
        return x;
}

int func2()
{
        int y = 4;
        return y;
}

// 1M
//char str[1024 * 1024] = {0, 1}; 

int main()
{
        int a = 1;
        int b = 2;

        int c = func();

        int d = a + b + c;

        printf("d(%d)\n", d);

        return 0;
}
```

Makefile:

```
# Compare gcc and clang, default is gcc

#CC = /root/compile/llvm_install/bin/clang
#CXX = /root/compile/llvm_install/bin/clang++

CFLAGS = -Werror -Wall -g -pipe

CFLAGS += -O0
#CFLAGS += -O1
#CFLAGS += -O2
#CFLAGS += -O3

# https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/Optimize-Options.html#Optimize-Options
# https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/Link-Options.html#Link-Options
CFLAGS += -fdata-sections -ffunction-sections

INCLUDE = -I./
LIBPATH = -L./
LIBS = 

BIN = demo

OBJS = demo.o

MAP_FILE = mapfile

.PHONY: clean

all: $(BIN)

$(BIN): $(OBJS)
        #$(CXX) -o $@ $^ $(LIBPATH) $(LIBS) -Wl,-Map=$(MAP_FILE)
        $(CXX) -o $@ $^ $(LIBPATH) $(LIBS) -Wl,-Map=$(MAP_FILE) -Wl,--gc-sections 
        @echo "build $(BIN) ok"

install:
        @echo "nothing to install"

clean:
        rm -f $(OBJS) $(BIN) $(MAP_FILE)


%.o: %.cpp 
        $(CXX) $(CFLAGS) $(INCLUDE) -c $<
%.o: %.c
        $(CC) $(CFLAGS) $(INCLUDE) -c $<
```

使用gcc编译，优化选项（默认级别，不做优化）：`-O0`

```
(gdb) b main
Breakpoint 1 at 0x400605: file demo.cpp, line 20.
(gdb) r
Starting program: /root/test/cpp/cpp_strip/demo 

Breakpoint 1, main () at demo.cpp:20
20              int a = 1;
Missing separate debuginfos, use: debuginfo-install glibc-2.17-196.tl2.3.x86_64 libgcc-4.8.5-4.el7.x86_64 libstdc++-4.8.5-4.el7.x86_64
(gdb) disassemble /m main
Dump of assembler code for function main():
19      {
   0x00000000004005fd <+0>:     push   %rbp
   0x00000000004005fe <+1>:     mov    %rsp,%rbp
   0x0000000000400601 <+4>:     sub    $0x10,%rsp

20              int a = 1;
=> 0x0000000000400605 <+8>:     movl   $0x1,-0x4(%rbp)

21              int b = 2;
   0x000000000040060c <+15>:    movl   $0x2,-0x8(%rbp)

22
23              int c = func();
   0x0000000000400613 <+22>:    callq  0x4005ed <func()>
   0x0000000000400618 <+27>:    mov    %eax,-0xc(%rbp)

24
25              int d = a + b + c;
   0x000000000040061b <+30>:    mov    -0x8(%rbp),%eax
   0x000000000040061e <+33>:    mov    -0x4(%rbp),%edx
   0x0000000000400621 <+36>:    add    %eax,%edx
   0x0000000000400623 <+38>:    mov    -0xc(%rbp),%eax
   0x0000000000400626 <+41>:    add    %edx,%eax
   0x0000000000400628 <+43>:    mov    %eax,-0x10(%rbp)

26
27              printf("d(%d)\n", d);
   0x000000000040062b <+46>:    mov    -0x10(%rbp),%eax
   0x000000000040062e <+49>:    mov    %eax,%esi
   0x0000000000400630 <+51>:    mov    $0x4006cd,%edi
   0x0000000000400635 <+56>:    mov    $0x0,%eax
   0x000000000040063a <+61>:    callq  0x4004d0 <printf@plt>

28
29              return 0;
   0x000000000040063f <+66>:    mov    $0x0,%eax

30      }
   0x0000000000400644 <+71>:    leaveq 
   0x0000000000400645 <+72>:    retq   

End of assembler dump.
```

使用clang编译，优化选项（默认级别，不做优化）：`-O0`

```
(gdb) b main
Breakpoint 1 at 0x40115f: file demo.cpp, line 20.
(gdb) r
Starting program: /root/test/cpp/cpp_strip/demo 

Breakpoint 1, main () at demo.cpp:20
20              int a = 1;
Missing separate debuginfos, use: debuginfo-install glibc-2.17-196.tl2.3.x86_64 libgcc-4.8.5-4.el7.x86_64 libstdc++-4.8.5-4.el7.x86_64
(gdb) disas /m main
Dump of assembler code for function main:
19      {
   0x0000000000401150 <+0>:     push   %rbp
   0x0000000000401151 <+1>:     mov    %rsp,%rbp
   0x0000000000401154 <+4>:     sub    $0x20,%rsp
   0x0000000000401158 <+8>:     movl   $0x0,-0x4(%rbp)

20              int a = 1;
=> 0x000000000040115f <+15>:    movl   $0x1,-0x8(%rbp)

21              int b = 2;
   0x0000000000401166 <+22>:    movl   $0x2,-0xc(%rbp)

22
23              int c = func();
   0x000000000040116d <+29>:    callq  0x401140 <func()>
   0x0000000000401172 <+34>:    mov    %eax,-0x10(%rbp)

24
25              int d = a + b + c;
   0x0000000000401175 <+37>:    mov    -0x8(%rbp),%eax
   0x0000000000401178 <+40>:    add    -0xc(%rbp),%eax
   0x000000000040117b <+43>:    add    -0x10(%rbp),%eax
   0x000000000040117e <+46>:    mov    %eax,-0x14(%rbp)

26
27              printf("d(%d)\n", d);
   0x0000000000401181 <+49>:    mov    -0x14(%rbp),%esi
   0x0000000000401184 <+52>:    movabs $0x402000,%rdi
   0x000000000040118e <+62>:    mov    $0x0,%al
   0x0000000000401190 <+64>:    callq  0x401030 <printf@plt>
   0x0000000000401195 <+69>:    xor    %eax,%eax

28
29              return 0;
   0x0000000000401197 <+71>:    add    $0x20,%rsp
   0x000000000040119b <+75>:    pop    %rbp
   0x000000000040119c <+76>:    retq   

End of assembler dump.
```


也可通过[在线编译工具](https://gcc.godbolt.org/)反汇编。

![gcc_compile](/assets/images/202101/gcc_compile.png)


参考[Compilation options](https://gcc.gnu.org/onlinedocs/gnat_ugn/Compilation-options.html)通过下面两步，去除编译种没有使用的函数：

1. 添加编译选项`CFLAGS += -fdata-sections -ffunction-sections`
2. 添加链接选项`-Wl,--gc-sections`

使用gc的方式，会将函数代码生成为独立的section，而默认的方式不会生成独立的代码段。

> 注意：此选项对gcc和clang都生效。

```
$readelf -t demo.o 
There are 26 section headers, starting at offset 0x10b0:

Section Headers:
  [Nr] Name
       Type              Address          Offset            Link
       Size              EntSize          Info              Align
       Flags
  [ 0] 
       NULL                   NULL             0000000000000000  0000000000000000  0
       0000000000000000 0000000000000000  0                 0
       [0000000000000000]: 
  [ 1] .text
       PROGBITS               PROGBITS         0000000000000000  0000000000000040  0
       0000000000000000 0000000000000000  0                 4
       [0000000000000006]: ALLOC, EXEC
  [ 2] .data
       PROGBITS               PROGBITS         0000000000000000  0000000000000040  0
       0000000000000000 0000000000000000  0                 4
       [0000000000000003]: WRITE, ALLOC
  [ 3] .bss
       NOBITS                 NOBITS           0000000000000000  0000000000000040  0
       0000000000000000 0000000000000000  0                 4
       [0000000000000003]: WRITE, ALLOC
  [ 4] .text._Z4funcv
       PROGBITS               PROGBITS         0000000000000000  0000000000000040  0
       0000000000000010 0000000000000000  0                 1
       [0000000000000006]: ALLOC, EXEC
  [ 5] .text._Z5func2v
       PROGBITS               PROGBITS         0000000000000000  0000000000000050  0
       0000000000000010 0000000000000000  0                 1
       [0000000000000006]: ALLOC, EXEC
  [ 6] .rodata
       PROGBITS               PROGBITS         0000000000000000  0000000000000060  0
       0000000000000007 0000000000000000  0                 1
       [0000000000000002]: ALLOC
  [ 7] .text.main
       PROGBITS               PROGBITS         0000000000000000  0000000000000067  0
       0000000000000049 0000000000000000  0                 1
       [0000000000000006]: ALLOC, EXEC
  [ 8] .rela.text.main
       RELA                   RELA             0000000000000000  0000000000001970  24
       0000000000000048 0000000000000018  7                 8
       [0000000000000000]: 
  [ 9] .debug_info
       PROGBITS               PROGBITS         0000000000000000  00000000000000b0  0
       000000000000076f 0000000000000000  0                 1
       [0000000000000000]: 

...
```

并且最终的链接代码中，不会存在未使用函数的代码：

```
$nm -C demo | grep func
00000000004005f0 T func()
```

查看mapfile，可以看到func2被discard了：

```
Discarded input sections
...
 .text          0x0000000000000000        0x0 demo.o
 .data          0x0000000000000000        0x0 demo.o
 .bss           0x0000000000000000        0x0 demo.o
 .text._Z5func2v
                0x0000000000000000       0x10 demo.o
...               
```

```
$objdump -s -j .text._Z5func2v demo.o

demo.o:     file format elf64-x86-64

Contents of section .text._Z5func2v:
 0000 554889e5 c745fc04 0000008b 45fc5dc3  UH...E......E.].
```

refer:

* https://stackoverflow.com/questions/6687630/how-to-remove-unused-c-c-symbols-with-gcc-and-ld
* https://stackoverflow.com/questions/54996229/is-ffunction-sections-fdata-sections-and-gc-sections-not-working
* https://stackoverflow.com/questions/17710024/clang-removing-dead-code-during-static-linking-gcc-equivalent-of-wl-gc-sect



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






