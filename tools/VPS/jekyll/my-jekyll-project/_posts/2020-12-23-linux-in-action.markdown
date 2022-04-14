---
layout: post
title:  "Linux in Action"
date:   2021-01-02 17:00:00 +0800
categories: Linux
---

* Do not remove this line (it will not be displayed)
{:toc}

# Linux操作系统

## Signal

Linux supports both POSIX reliable signals (hereinafter "standard signals") and POSIX real-time signals.

Using these system calls, a process can elect one of the following behaviors to occur on delivery of the signal: perform the default action; ignore the signal; or catch the signal with a signal handler, a programmer-defined function that is automatically invoked when the signal is delivered.

```
$kill -l
 1) SIGHUP       2) SIGINT       3) SIGQUIT      4) SIGILL       5) SIGTRAP
 2) SIGABRT      7) SIGBUS       8) SIGFPE       9) SIGKILL     10) SIGUSR1
1)  SIGSEGV     12) SIGUSR2     13) SIGPIPE     14) SIGALRM     15) SIGTERM
2)  SIGSTKFLT   17) SIGCHLD     18) SIGCONT     19) SIGSTOP     20) SIGTSTP
3)  SIGTTIN     22) SIGTTOU     23) SIGURG      24) SIGXCPU     25) SIGXFSZ
4)  SIGVTALRM   27) SIGPROF     28) SIGWINCH    29) SIGIO       30) SIGPWR
5)  SIGSYS      34) SIGRTMIN    35) SIGRTMIN+1  36) SIGRTMIN+2  37) SIGRTMIN+3
6)  SIGRTMIN+4  39) SIGRTMIN+5  40) SIGRTMIN+6  41) SIGRTMIN+7  42) SIGRTMIN+8
7)  SIGRTMIN+9  44) SIGRTMIN+10 45) SIGRTMIN+11 46) SIGRTMIN+12 47) SIGRTMIN+13
8)  SIGRTMIN+14 49) SIGRTMIN+15 50) SIGRTMAX-14 51) SIGRTMAX-13 52) SIGRTMAX-12
9)  SIGRTMAX-11 54) SIGRTMAX-10 55) SIGRTMAX-9  56) SIGRTMAX-8  57) SIGRTMAX-7
10) SIGRTMAX-6  59) SIGRTMAX-5  60) SIGRTMAX-4  61) SIGRTMAX-3  62) SIGRTMAX-2
11) SIGRTMAX-1  64) SIGRTMAX
```

* https://man7.org/linux/man-pages/man7/signal.7.html
* [Why there are only two user defined signals?](https://unix.stackexchange.com/questions/48582/why-there-are-only-two-user-defined-signals)

## Linux System Calls

How Linux programs call functions in the Linux kernel.

使用nasm写汇编，手动构造ELF格式文件。不使用c库，系统调用使用syscall指令(x86-64)：

+ 入参：
  * eax = 系统调用号，可以在 /usr/include/asm/unistd_64.h 文件中找到
  * rdi, rsi, rdx, r10, r8, r9 分别为第 1 至 6 个参数
+ 出参：
  * rax = 返回值（如果失败，返回 -errno）
  * rcx, r11 被破坏（它们分别被 syscall 指令用来保存返回地址和 rflags）
  * 其他寄存器的值保留


* [The Definitive Guide to Linux System Calls](http://blog.packagecloud.io/eng/2016/04/05/the-definitive-guide-to-linux-system-calls)
* [Searchable Linux Syscall Table for x86 and x86_64](https://filippo.io/linux-syscall-table/)

## tar (compress/extract files)

* Compress an entire directory or a single file

```
tar -czvf name-of-archive.tar.gz /path/to/directory-or-file

-c: Create an archive.
-z: Compress the archive with gzip.
-v: Display progress in the terminal while creating the archive, also known as “verbose” mode. The v is always optional in these commands, but it’s helpful.
-f: Allows you to specify the filename of the archive.
```
* Compress multiple directories or files at once

```
tar -czvf archive.tar.gz /home/ubuntu/Downloads /usr/local/stuff /home/ubuntu/Documents/notes.txt
```

* Exclude directories and files

In some cases, you may wish to compress an entire directory, but not include certain files and directories. You can do so by appending an `--exclude` switch for each directory or file you want to exclude.

```
tar -czvf archive.tar.gz /home/ubuntu --exclude=/home/ubuntu/Downloads --exclude=/home/ubuntu/.cache
```

The `--exclude` switch is very powerful. It doesn’t take names of directories and files–it actually accepts patterns. There’s a lot more you can do with it. For example, you could archive an entire directory and exclude all .mp4 files with the following command:

```
tar -czvf archive.tar.gz /home/ubuntu --exclude=*.mp4
```

* Use bzip2 compression instead

While `gzip` compression is most frequently used to create `.tar.gz` or `.tgz` files, tar also supports `bzip2` compression. This allows you to create bzip2-compressed files, often named `.tar.bz2`, `.tar.bz`, or `.tbz` files. To do so, just replace the `-z` for gzip in the commands here with a `-j` for `bzip2`.

`Gzip` is faster, but it generally compresses a bit less, so you get a somewhat larger file. `Bzip2` is slower, but it compresses a bit more, so you get a somewhat smaller file

```
tar -cjvf archive.tar.bz2 stuff
```

* Extract an archive

Once you have an archive, you can extract it with the tar command. The following command will extract the contents of archive.tar.gz to the current directory. It’s the same as the archive creation command we used above, except the `-x` switch replaces the `-c` switch. This specifies you want to e**x**tract an archive instead of create one.

```
tar -xzvf archive.tar.gz
```

You may want to extract the contents of the archive to a specific directory. You can do so by appending the `-C` switch to the end of the command. For example, the following command will extract the contents of the archive.tar.gz file to the /tmp directory.

```
tar -xzvf archive.tar.gz -C /tmp
```

https://www.howtogeek.com/248780/how-to-compress-and-extract-files-using-the-tar-command-on-linux/

## /proc

`/proc/$pid/exe`: 可执行文件软链接

## kill

``` bash
> pgrep firefox
6316
6565
> pidof firefox
6565 6316

pkill firefox
killall <name>
kill -9 `pidof firefox`
kill -9 `pgrep firefox`
ps ax | grep <snippet> | grep -v grep | awk '{print $1}' | xargs kill
```

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

# CPU

## CPU个数

```
# 查看物理CPU个数
cat /proc/cpuinfo| grep "physical id"| sort| uniq| wc -l

# 查看每个物理CPU中core的个数(即核数)
cat /proc/cpuinfo| grep "cpu cores"| uniq

# 查看逻辑CPU的个数
cat /proc/cpuinfo| grep "processor"| wc -l
```

## CPU负载

通过`uptime`命令可以查看机器的CPU负载，这三个数据分别是CPU 1分钟、5分钟、15分钟内系统的平均负载。当CPU完全空闲的时候，平均负载为0；当CPU工作量饱和的时候，平均负载为1。

```
$ uptime
 14:32:18 up 58 days, 23:20,  2 users,  load average: 5.55, 3.91, 3.78
```

> 理解CPU负载

首先，假设最简单的情况，你的电脑只有一个CPU，所有的运算都必须由这个CPU来完成。那么，我们不妨把这个CPU想象成一座大桥，桥上只有一根车道，所有车辆都必须从这根车道上通过。（很显然，这座桥只能单向通行。）

* 系统负载为0，意味着大桥上一辆车也没有。
* 系统负载为0.5，意味着大桥一半的路段有车。
* 系统负载为1.0，意味着大桥的所有路段都有车，也就是说大桥已经"满"了。但是必须注意的是，直到此时大桥还是能顺畅通行的。
* 系统负载为1.7，意味着车辆太多了，大桥已经被占满了（100%），后面等着上桥的车辆为桥面车辆的70%。以此类推，系统负载2.0，意味着等待上桥的车辆与桥面的车辆一样多；系统负载3.0，意味着等待上桥的车辆是桥面车辆的2倍。总之，当系统负载大于1，后面的车辆就必须等待了；系统负载越大，过桥就必须等得越久。

CPU的系统负载，基本上等同于上面的类比。大桥的通行能力，就是CPU的最大工作量；桥梁上的车辆，就是一个个等待CPU处理的进程（process）。为了顺畅运行，系统负载最好不要超过1.0，这样就没有进程需要等待了，所有进程都能第一时间得到处理。很显然，1.0是一个关键值，超过这个值，系统就不在最佳状态了。

> 经验法则

1.0是系统负载的理想值吗？不一定，系统管理员往往会留一点余地，当这个值达到0.7，就应当引起注意了。经验法则是这样的：

* 当系统负载持续大于0.7，你必须开始调查了，问题出在哪里，防止情况恶化。
* 当系统负载持续大于1.0，你必须动手寻找解决办法，把这个值降下来。
* 当系统负载达到5.0，就表明你的系统有很严重的问题，长时间没有响应，或者接近死机了。你不应该让系统达到这个值。

> 多处理器

如果你的电脑装了2个CPU，会发生什么情况呢？

* 2个CPU，意味着电脑的处理能力翻了一倍，能够同时处理的进程数量也翻了一倍。还是用大桥来类比，两个CPU就意味着大桥有两根车道了，通车能力翻倍了。
* 所以，`2`个CPU表明系统负载可以达到`2.0`，此时每个CPU都达到100%的工作量。推广开来，`n`个CPU的电脑，可接受的系统负载最大为`n.0`。

> 多核处理器

芯片厂商往往在一个CPU内部，包含多个CPU核心，这被称为多核CPU。在系统负载方面，多核CPU与多CPU效果类似，所以考虑系统负载的时候，必须考虑这台电脑有几个CPU、每个CPU有几个核心。然后，把系统负载除以总的核心数，只要每个核心的负载不超过1.0，就表明系统正常运行。

怎么知道有多少个CPU核心呢？

```
$ grep -c 'model name' /proc/cpuinfo
48
```

> 观察时长

"load average"一共返回三个平均值：1分钟系统负载、5分钟系统负载，15分钟系统负载。应该参考哪个值？

* 如果只有1分钟的系统负载大于1.0，其他两个时间段都小于1.0，这表明只是暂时现象，问题不大。
* 如果15分钟内，平均系统负载大于1.0（调整CPU核心数之后），表明问题持续存在，不是暂时现象。所以，你应该主要观察"15分钟系统负载"，将它作为系统正常运行的指标。

## CPU负载采集算法

通过读取`/proc/loadavg`文件来得到CPU的1分钟、5分钟、15分钟平均负载。一般来说CPU负载带了两位的小数。为了保留精度，会把CPU负载值乘以100再上报。

```
$ cat /proc/loadavg
5.26 4.80 4.65 4/8728 1837325
```

## CPU使用率采集算法

CPU使用率衡量的是程序运行占用的CPU百分比。Linux的CPU使用率信息可以通过`/proc/stat`文件计算得到。`/proc/stat`包含了所有CPU活动的信息，该文件中的所有值都是从系统启动开始累计的，单位为`jiffies`。

```
$ cat /proc/stat | grep 'cpu'
 
cpu  4409701839 5860491 3043372756 11777957443 471600199 13606335 49392558 0
cpu0 980245201 1554799 596504303 3214215192 126029552 6603537 17697344 0
cpu1 1209283591 1411942 861982464 2749190858 113506249 255348 7220138 0
cpu2 971403569 1530154 624934033 3195318936 125767475 6491354 17450205 0
cpu3 1248769476 1363594 959951956 2619232456 106296922 256096 7024869 0
```

cpu一行指的是总的CPU信息，cpu0、cpu1、cpu2、cpu3几行指的是CPU各个核的CPU信息。从这里也可以看出这台服务器共有4个核。每列从左到右的意思为：

* `user`：从系统启动开始累计到当前时刻，用户态的CPU时间 ，不包含nice值为负进程。
* `nice`：从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间
* `system`：从系统启动开始累计到当前时刻，内核态时间
* `idle`：从系统启动开始累计到当前时刻，除硬盘IO等待时间以外其它等待时间
* `iowait`：从系统启动开始累计到当前时刻，硬盘IO等待时间
* `irq`：从系统启动开始累计到当前时刻，硬中断时间
* `softirq`：从系统启动开始累计到当前时刻，软中断时间
* `steal`：在虚拟环境下 CPU 花在处理其他作业系统的时间，Linux 2.6.11 开始才开始支持。
* `guest`：在 Linux 内核控制下 CPU 为 guest 作业系统运行虚拟 CPU 的时间，Linux 2.6.24 开始才开始支持。（因为内核版本不支持，上面的示例没有这一列）

根据这些信息，就可以计算出CPU使用率。CPU使用率采集算法如下（以CPU0为例）：

``` bash
# 得到cpu0的信息
cat /proc/stat | grep 'cpu0'
 
cpu_total1 = user + nice + system + idle + iowait + irq + softirq
cpu_used1 = user + nice + system + irq + softirq
 
# 等待15s
sleep 15
 
# 再次检查cpu信息
cat /proc/stat | grep 'cpu0'
 
cpu_total2 = user + nice + system + idle + iowait + irq + softirq
cpu_used2 = user + nice + system + irq + softirq
 
# 得到cpu0在15秒内的平均使用率
(cpu_used2 - cpu_used1) / (cpu_total2 - cpu_total1) * 100%
```

采集策略：每分钟会采集4次15秒内的CPU平均使用率。为了避免漏采集CPU峰值，取这一分钟内四次采集的最大值上报。


# 内存

通过`free`命令，可以看到服务器内存的使用情况。 (数值都默认以字节为单位，`-m` 表示Display the amount of memory in megabytes)

```
# free
             total       used       free     shared    buffers     cached
Mem:       1017796     819720     198076      16784      46240     468880
-/+ buffers/cache:     304600     713196
Swap:            0          0          0
```

Mem含义：
* `total`: 总内存
* `used`: 已经使用的内存
* `free`: 空闲内存

Swap含义：交换分区。

> 在很多Linux服务器上运行free命令，会发现剩余内存（Mem:行的free列）很少，但实际服务器上的进程并没有占用很大的内存。这是因为Linux特殊的内存管理机制。Linux内核会把空闲的内存用作buffer/cached，用于提高文件读取性能。当应用程序需要用到内存时，buffer/cached内存是可以马上回收的。所以，对应用程序来说，buffer/cached是可用的，可用内存应该是free+buffers+cached。因为这个原因，free命令也才有第三行的-/+ buffers/cache。

通过`top`命令查看内存。

```
KiB Mem :  1009184 total,    98908 free,   399864 used,   510412 buff/cache
KiB Swap:        0 total,        0 free,        0 used.   453712 avail Mem 

PID USER    PR  NI    VIRT    RES    SHR S %CPU %MEM     TIME+  COMMAND                                                                                                                        
1 root      20   0   77908   6184   3808 S  0.0  0.6   2:25.47 systemd 
```

* VIRT，进程虚拟内存的大小，虚拟内存并不会全部分配物理内存
* RES，常驻内存的大小，是进程实际使用的物理内存大小，但不包括Swap和共享内存
* SHR，共享内存大小，比如，与其他进程共同使用的共享内存，加载的动态链接库以及程序的代码段等
* %MEM，进程使用物理内存占系统总内存的百分比

可以使用`pmap -xp $pid`分析下进程的内存分布，更详细的信息可以通过`cat /proc/$pid/smaps` (since Linux 2.6.14)来查看每个进程的内存消耗情况。

## 整个内存采集算法

采集`free`命令的这几个结果（跟free命令一样，都是通过读取`/proc/meminfo`来得到的），然后每4分钟上报一次：

* MEM使用量：Mem：的`total - free`
* MEM总大小：Mem：的`total`
* 应用程序使用内存：-/+ buffers/cache：的`used`

## 某进程内存采集算法

从进程的角度来判断服务器的内存占用。Linux内核2.6.14及以上版本增加了`/proc/(进程ID)/smaps`文件，通过smaps文件可以分析进程具体占用的每一段内存。

```
cat /proc/`pidof friendsvr`/smaps > smaps.out

00400000-02afb000 r-xp 00000000 fc:20 3934363                            /data/home/gerryyang/speedgame/bin/friendsvr/friendsvr (deleted)
   2 Size:              39916 kB
   3 Rss:               10232 kB
   4 Pss:               10232 kB
   5 Shared_Clean:          0 kB
   6 Shared_Dirty:          0 kB
   7 Private_Clean:     10232 kB
   8 Private_Dirty:         0 kB
   9 Referenced:        10232 kB
  10 Anonymous:             0 kB
  11 AnonHugePages:         0 kB
  12 Swap:                  0 kB
  13 KernelPageSize:        4 kB
  14 MMUPageSize:           4 kB
  15 Locked:                0 kB
  16 VmFlags: rd ex mr mw me dw
```

通过smaps文件，可以计算出两个指标：

* 进程Virtual内存：通过把smaps文件所有的`Size`的大小加起来得到
* 进程Private内存：通过把smaps 文件所有的`Private_Clean`、`Private_Dirty`大小加起来得到

通过此方法，也可以计算所有进程的内存使用：

通过smaps文件计算所有进程的Virtual内存总和、Private内存总和，并计算共享内存总和，得到下面3个指标并上报。每4分钟上报一次。

* Virtual内存占用：通过计算所有进程的Virtual内存总和得到。可以用来判断进程是否存在内存泄漏。如果一台机器Virtual内存占用持续上涨，便很有可能发生了内存泄漏。
* Private内存占用：通过计算所有进程的Private内存总和得到。Private内存都是映射在物理内存中的，因此通过总Private内存，我们可以知道机器至少需要多少物理内存。
* Private内存+共享内存占用：通过Private内存占用，再加上机器上的共享内存，得到的指标。可以用来粗略衡量机器实际的内存占用。

## 常用命令

### pmap

```
$pmap -x `pidof gamesvr`
...
Address           Kbytes     RSS   Dirty Mode  Mapping
0000000000400000   51732   30036       0 r-x-- gamesvr
0000000003885000     520     424      20 r-x-- gamesvr
0000000003907000     452     252      84 rwx-- gamesvr
0000000003978000   25832    8044    8044 rwx--   [ anon ]
0000000005ac2000   12288    4264    4264 rwx--   [ anon ]
00000000066c2000  505840  344500  344500 rwx--   [ anon ]
...
---------------- ------- ------- ------- 
total kB         1372708  402092  367472
```

## dump memory (gdb)

根据`pmap`输出的进程地址可以通过`gdb`将内存的内容dump出来。

```
(gdb) dump memory memory.dump 0x66c2000 0x66d2000
(gdb) !strings memory.dump | head -n10
%%%%%%%%%%%%%%%%
```

或者：

gdb --batch --pid {PID} -ex "dump memory native_memory.dump 0x66c2000 0x66d2000" 

## refer

* https://techtalk.intersec.com/2013/07/memory-part-1-memory-types/
* https://techtalk.intersec.com/2013/07/memory-part-2-understanding-process-memory/
* https://techtalk.intersec.com/2013/08/memory-part-3-managing-memory/
* https://techtalk.intersec.com/2013/10/memory-part-4-intersecs-custom-allocators/
* https://techtalk.intersec.com/2013/12/memory-part-5-debugging-tools/

# 网络

## 流量和包量统计

通过`/proc/net/dev`文件，可以计算出服务器的**流量(bytes)**及**包量(packets)**。其中，`/proc/net/dev`的数值是从系统启动后一直累加的。

> 注意：在32位系统上，/proc/net/dev中的数值系统使用4个字节的无符号整型保存，当数值达到2^32-1，即4294967295之后，数值会溢出，计算流量包量时需要把这一点考虑进去。而64位系统中/proc/net/dev使用的是8个字节的无符号整型，因此就不需要注意溢出的问题。

```
$ cat /proc/net/dev
Inter-|   Receive                                                |  Transmit
 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
   br0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  eth1: 20762575244 116974442    0    0    0     0          0         0 27863156472 128930710    0    0    0     0       0          0
    lo: 1857145146 24970433    0    0    0     0          0         0 1857145146 24970433    0    0    0     0       0          0
docker0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
```

如何计算网卡流量，以`eth1`为例：

1. 读取`/proc/net/dev`文件，获取`eth1`的`Recevie bytes`、`Receive packets`、`Transmit bytes`、`Transmit packets`，分别记为`receive_bytes0`、`receive_packets0`、`transmit_bytes0`、`transmit_packets0`
2. sleep 60秒
3. 再次读取`/proc/net/dev`文件，获取`eth1`的`Receive bytes`、`Receive packets`、`Transmit bytes`、`Transmit packets`，分别记为`receive_bytes1`、`receive_packets1`、`transmit_bytes1`、`transmit_packets1`
4. 根据60秒前后的`/proc/net/dev`文件，便可计算出下面的指标：

* 60秒内平均每秒入流量：`(receive_bytes1 - receive_bytes0) * 8 / 60 / 1000 （kbps）` （乘以8是为了把`bytes`转成`bit`，除以1000是为了把单位转成`k`，除以60则是取60秒内的平均值）
* 60秒内平均每秒出流量：`(transmit_bytes1 - transmit_bytes0) * 8 / 60 / 1000 （kbps）`
* 60秒内平均每秒入包数：`(receive_packets1 - receive_packets0) / 60 （个）`
* 60秒内平均每秒出包数：`(transmit_packets1 - transmit_packets0) / 60 （个）`

## TCP链接数

`/proc/net/snmp`记录了一些TCP信息，其中比较有用的是`CurrEstab`字段，即当前已建立的TCP连接数。

```
$ cat /proc/net/snmp
Ip: Forwarding DefaultTTL InReceives InHdrErrors InAddrErrors ForwDatagrams InUnknownProtos InDiscards InDelivers OutRequests OutDiscards OutNoRoutes ReasmTimeout ReasmReqds ReasmOKs ReasmFails FragOKs FragFails FragCreates
Ip: 1 64 134291507 0 2 0 0 0 134291505 146203684 362 218 0 0 0 0 6 0 18
Icmp: InMsgs InErrors InCsumErrors InDestUnreachs InTimeExcds InParmProbs InSrcQuenchs InRedirects InEchos InEchoReps InTimestamps InTimestampReps InAddrMasks InAddrMaskReps OutMsgs OutErrors OutDestUnreachs OutTimeExcds OutParmProbs OutSrcQuenchs OutRedirects OutEchos OutEchoReps OutTimestamps OutTimestampReps OutAddrMasks OutAddrMaskReps
Icmp: 106100 2 0 737 0 0 0 0 105328 35 0 0 0 0 106102 0 738 0 0 0 0 36 105328 0 0 0 0
IcmpMsg: InType0 InType3 InType8 OutType0 OutType3 OutType8
IcmpMsg: 35 737 105328 105328 738 36
Tcp: RtoAlgorithm RtoMin RtoMax MaxConn ActiveOpens PassiveOpens AttemptFails EstabResets CurrEstab InSegs OutSegs RetransSegs InErrs OutRsts InCsumErrors
Tcp: 1 200 120000 -1 105094 27940 218 10479 39 134181152 143798757 1057 0 7716 0
Udp: InDatagrams NoPorts InErrors OutDatagrams RcvbufErrors SndbufErrors InCsumErrors IgnoredMulti
Udp: 1435 738 0 10020862 0 0 0 1
UdpLite: InDatagrams NoPorts InErrors OutDatagrams RcvbufErrors SndbufErrors InCsumErrors IgnoredMulti
UdpLite: 0 0 0 0 0 0 0 0
```

## UDP接收和发送数据报

`/proc/net/snmp`还记录了一些UDP信息，其中比较有用的是`InDatagrams`及`OutDatagrams`字段。

UDP接收和发送数据报计算方法与`/proc/net/dev`类似，步骤如下：

1. 读取`/proc/net/snmp`得到`InDatagrams`及`OutDatagrams`，分别记为`in_data0`和`out_data0`
2. sleep 60秒
3. 再次读取`/proc/net/snmp`得到`InDatagrams`及`OutDatagrams`，分别记为`in_data1`和`out_data1`
4. 根据60秒前后的`/proc/net/snmp`文件，便可计算下面两个指标：

* 60秒内平均每秒UDP入数据报：`(in_data1 - in_data0) / 60`
* 60秒内平均每秒UDP出数据报：`(out_data1 - out_data0) / 60`


## 根据socket查找ip信息

Tracing socket (file) descriptor back to bound address

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

## 根据fd找到ip信息 

```
ls -lrt /proc/24748/fd | grep 854
netstat -e | grep 169393703

# or
lsof -i -a -p 20640
```

## TIME_WAIT

```
echo 1 > /proc/sys/net/ipv4/tcp_tw_reuse
echo 1 > /proc/sys/net/ipv4/tcp_tw_recycle
```

* [tcp_tw_reuse、tcp_tw_recycle 使用场景及注意事项](https://www.cnblogs.com/lulu/p/4149312.html)
* [Linux tcp_tw_reuse = 2 — how is this set and what is the significance?](https://forum.vyos.io/t/linux-tcp-tw-reuse-2-how-is-this-set-and-what-is-the-significance/5286/4)
* [net-tcp: extend tcp_tw_reuse sysctl to enable loopback only optimization](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=79e9fed460385a3d8ba0b5782e9e74405cb199b1)

## 本地端口范围设置

注意，如果服务器有较多程序开放端口，请注意程序的端口开放范围，以避免随机端口跟设置的端口范围冲突。

```
$cat /proc/sys/net/ipv4/ip_local_port_range
32768   61000
```

* [linux下设置ip_local_port_range参数](https://cloud.tencent.com/developer/article/1691625)

## 常用命令
### ss (another utility to investigate sockets)

`ss` is used to dump socket statistics. It allows showing information similar to `netstat`.  It can display more TCP and state information than other tools.

```
$ ss -s     
Total: 2144
TCP:   2000 (estab 1757, closed 200, orphaned 0, timewait 200)

Transport Total     IP        IPv6
RAW       2         1         1        
UDP       13        11        2        
TCP       1800      1795      5        
INET      1815      1807      8        
FRAG      0         0         0    
```

```
$ ss -ltupn
State       Recv-Q Send-Q    Local Address:Port     Peer Address:Port                
LISTEN      0      128       *:15434                *:*                     users:(("tconnd",pid=3304405,fd=35))
```

### nslookup 

query Internet name servers interactively

```
# nslookup 8.8.8.8
Server:         8.8.8.8
Address:        8.8.8.8#53

Non-authoritative answer:
8.8.8.8.in-addr.arpa    name = google-public-dns-a.google.com.

Authoritative answers can be found from:
```

### nc

```
ncat [options] [hostname] [port]

# test UDP port was open or not
nc -vzu <host> <port>

on server listen UDP port: `nc -ul 6111`
on client: `nc -u <server> 6111`
```

https://serverfault.com/questions/416205/testing-udp-port-connectivity

https://en.wikipedia.org/wiki/Netcat#Test_if_UDP_port_is_open:_simple_UDP_server_and_client

# 磁盘IO

```
$ iostat -x 10

avg-cpu:  %user   %nice %system %iowait  %steal   %idle
           3.74    0.01    3.88    0.01    0.00   92.36

Device:         rrqm/s   wrqm/s     r/s     w/s    rkB/s    wkB/s avgrq-sz avgqu-sz   await r_await w_await  svctm  %util
vda               0.00     5.90    0.00    2.10     0.00    66.40    63.24     0.00    0.57    0.00    0.57   0.57   0.12
vdb               0.00     0.60    0.00   14.90     0.00   204.40    27.44     0.05    3.30    0.00    3.30   0.16   0.24
scd0              0.00     0.00    0.00    0.00     0.00     0.00     0.00     0.00    0.00    0.00    0.00   0.00   0.00
vdd               0.00     1.10    0.00    0.40     0.00     6.00    30.00     0.00    0.00    0.00    0.00   0.00   0.00
vdc               0.00     7.70    0.00    6.90     0.00    96.40    27.94     0.00    0.52    0.00    0.52   0.52   0.36
vdf               0.00     1.20    0.00    2.10     0.00    14.00    13.33     0.00    0.00    0.00    0.00   0.00   0.00
vdg               0.00     0.70    0.00    0.40     0.00     4.40    22.00     0.00    2.00    0.00    2.00   2.00   0.08
vdl               0.00     3.80    0.00    1.40     0.00    25.60    36.57     0.00    0.57    0.00    0.57   0.57   0.08
vdi               0.00     0.50    0.00    0.20     0.00     2.80    28.00     0.00    0.00    0.00    0.00   0.00   0.00
vdm               0.00     1.50    0.00    0.60     0.00     9.60    32.00     0.00    0.67    0.00    0.67   0.67   0.04
vdn               0.00     0.50    0.00    0.20     0.00     2.80    28.00     0.00    0.00    0.00    0.00   0.00   0.00
vdo               0.00     0.50    0.00    0.20     0.00     2.80    28.00     0.00    0.00    0.00    0.00   0.00   0.00
vdr               0.00     0.50    0.00    0.20     0.00     2.80    28.00     0.00    0.00    0.00    0.00   0.00   0.00
vdj               0.00     0.50    0.00    0.20     0.00     2.80    28.00     0.00    0.00    0.00    0.00   0.00   0.00
vdt               0.00     0.50    0.00    0.20     0.00     2.80    28.00     0.00    0.00    0.00    0.00   0.00   0.00
vde               0.00     0.70    0.00    0.40     0.00     4.40    22.00     0.00    2.00    0.00    2.00   2.00   0.08
vdq               0.00     0.50    0.00    0.20     0.00     2.80    28.00     0.00    0.00    0.00    0.00   0.00   0.00
vdp               0.00     0.50    0.00    0.20     0.00     2.80    28.00     0.00    0.00    0.00    0.00   0.00   0.00
vds               0.00     0.50    0.00    0.20     0.00     2.80    28.00     0.00    0.00    0.00    0.00   0.00   0.00
vdh               0.00     5.30    0.00    1.50     0.00   258.80   345.07     0.00    2.40    0.00    2.40   1.07   0.16
vdk               0.00     0.50    0.00    0.20     0.00     2.80    28.00     0.00    0.00    0.00    0.00   0.00   0.00
```

每一列的含义如下：

* `rrqm/s`: 每秒进行merge的读操作数目。
* `wrqm/s`: 每秒进行merge的写操作数目。
* `r/s`: 每秒完成的读 I/O 设备次数。
* `w/s`: 每秒完成的写 I/O 设备次数。
* `rsec/s`: 每秒读扇区数。
* `wsec/s`: 每秒写扇区数。
* `rkB/s`: 每秒读K字节数。是 `rsec/s` 的一半，因为扇区大小为512字节
* `wkB/s`: 每秒写K字节数。是 `wsec/s` 的一半
* `avgrq-sz`: 平均每次设备I/O操作的数据大小 (扇区)
* `avgqu-sz`: 平均I/O队列长度。
* `await`: 平均每次设备I/O操作的等待时间 (毫秒)
* `svctm`: 平均每次设备I/O操作的服务时间 (毫秒)
* `%util`: 一秒中有百分之多少的时间用于 I/O 操作，或者说一秒中有多少时间 I/O 队列是非空的。  

怎么理解这里的字段呢？下面以超市结账的例子来说明。

* `r/s+w/s` 类似于交款人的总数
* `avgqu-sz`（平均队列长度）：类似于单位时间里平均排队的人数
* `svctm`（平均服务时间）类似于收银员的收款速度
* `await`（平均等待时间）类似于平均每人的等待时间
* `avgrq-sz`（平均IO数据）类似于平均每人所买的东西多少
* `%util`（磁盘IO使用率）类似于收款台前有人排队的时间比例

可以根据这些数据分析出 I/O 请求的模式，以及 I/O 的速度和响应时间：

* 如果`%util`接近100%，说明产生的I/O请求太多，I/O系统已经满负荷，该磁盘可能存在瓶颈
* `svctm`的大小一般和磁盘性能有关，CPU/内存的负荷也会对其有影响，请求过多也会间接导致 `svctm` 的增加。
* `await`的大小一般取决于服务时间（`svctm`） 以及 I/O 队列的长度和 I/O 请求的发出模式。一般来说`svctm` < `await`，因为同时等待的请求的等待时间被重复计算了。如果`svctm`比较接近`await`，说明I/O 几乎没有等待时间
* 如果`await`远大于`svctm`，说明I/O队列太长，应用得到的响应时间变慢
* 队列长度（`avgqu-sz`）也可作为衡量系统 I/O 负荷的指标，但由于 `avgqu-sz` 是按照单位时间的平均值，所以不能反映瞬间的 I/O 洪水。
* 如果响应时间超过了用户可以容许的范围，这时可以考虑更换更快的磁盘，调整内核elevator算法，优化应用，或者升级 CPU。
* 如果`%util`很大，而`rkB/s`和`wkB/s`很小，一般是因为磁盘存在较多的磁盘随机读写，最好把磁盘随机读写优化成顺序读写。

## 磁盘IO采集算法

通过`/proc/diskstats`文件计算得到。

```
252       0 vda 58964 2845 2864110 163028 56607103 29847638 1634532676 260114388 0 13189912 260253616
252       1 vda1 58883 2407 2859958 162956 55731294 29847638 1634532668 260028636 0 13104408 260168496
252      16 vdb 5743072 3056 239591974 19078076 66105709 19855513 4542946783 973079144 0 30841824 992260604
```

The `/proc/diskstats` file displays the I/O statistics of block devices. Each line contains the following 14 fields:

```
1 - major number
2 - minor mumber
3 - device name
4 - reads completed
5 - reads merged                                        （
6 - sectors read
7 - time spent reading (ms)
8 - writes completed
9 - writes merged
10 - sectors written
11 - time spent writing (ms)
12 - I/Os currently in progress
13 - time spent doing I/Os (ms)
14 - weighted time spent doing I/Os (ms)
```

`/proc/diskstats`中每个字段的数值也是从系统启动后一直累加的。我们用`delta`来表示在时间`t`内某个字段的增量。

```
delta(reads merged) = reads merged的值 - t秒前reads merged的值

svctm的计算方式：
delta(time spent doing I/Os) / (delta(reads completed) + delta(writes completed))
```

# Linux/UNIX Programming Interface


## clock_gettime

https://linux.die.net/man/3/clock_gettime

## pthread_setname_np

By default, all the threads created using `pthread_create()` inherit the program name.  The `pthread_setname_np()` function can be used to set a unique name for a thread, which can be useful for debugging multithreaded applications.  The thread name is a meaningful C language string, whose length is restricted to `16 characters`, including the terminating null byte ('\0').  The thread argument specifies the thread whose name is to be changed; name specifies the new name.

`pthread_setname_np()` internally writes to the thread-specific comm file under the /proc filesystem: `/proc/self/task/[tid]/comm`.

> Note: These functions are nonstandard GNU extensions; hence the suffix "_np" (nonportable) in the names.


* https://man7.org/linux/man-pages/man3/pthread_setname_np.3.html

# 问题定位

## top (display Linux processes)

| 常用命令 | 含义 | 备注
| -- | -- | 
| Z,B,E,e |  Global: 'Z' colors; 'B' bold; 'E'/'e' summary/task memory scale | 显示相关的设置
| l,t,m  |    Toggle Summary: 'l' load avg; 't' task/cpu stats; 'm' memory info | 最顶部的三部分信息切换(Toggle)：负载，cpu，memory
| 0,1,2,3,I  | Toggle: '0' zeros; '1/2/3' cpus or numa node views; 'I' Irix mode | 查看cpu，numa node
| f,F,X   |  Fields: 'f'/'F' add/remove/order/sort; 'X' increase fixed-width | 设置显示哪些 fields
| L,&,<,> | Locate: 'L'/'&' find/again; Move sort column: '<'/'>' left/right | 字符串查找，通过左右健移动查看
| R,H,V,J | Toggle: 'R' Sort; 'H' Threads; 'V' Forest view; 'J' Num justify | 查看线程
| c,i,S,j | Toggle: 'c' Cmd name/line; 'i' Idle; 'S' Time; 'j' Str justify | 查看程序命令行参数
| x,y     | Toggle highlights: 'x' sort field; 'y' running tasks | 
| z,b     | Toggle: 'z' color/mono; 'b' bold/reverse (only if 'x' or 'y') | 切换颜色
| u,U,o,O | Filter by: 'u'/'U' effective/any user; 'o'/'O' other criteria | 按用户过滤
| n,#,^O  | Set: 'n'/'#' max tasks displayed; Show: Ctrl+'O' other filter(s) | 设置显示 task 的数量
| C,...   | Toggle scroll coordinates msg for: up,down,left,right,home,end | 
| k,r     |  Manipulate tasks: 'k' kill; 'r' renice | 向进程发送信号，调整进程优先级
| d or s  |  Set update interval | 设置更新频率
| W,Y     |  Write configuration file 'W'; Inspect other output 'Y' | 保存配置文件 $HOME/.toprc
| q       |  Quit | 退出

More: man top

## coredump

```
ulimit -c unlimited

# 自定义路径格式
echo "/data/corefile/core_uid%u_%e_%t" > /proc/sys/kernel/core_pattern
echo "1" > /proc/sys/kernel/core_uses_pid

# 默认当前执行目录
echo "core" > /proc/sys/kernel/core_pattern
```

## dmesg

dmesg is used to examine or control the kernel ring buffer. The default action is to display all messages from the kernel ring buffer.

```
# 显示可读时间, 定位内存异常错误
$dmesg -T

[Sun Dec 13 23:41:22 2020] Out of memory: Kill process 15395 (cc1plus) score 7 or sacrifice child
[Sun Dec 13 23:41:22 2020] Killed process 15395 (cc1plus) total-vm:168940kB, anon-rss:119492kB, file-rss:0kB
```

## strace(relies on ptrace system call)

* [How does strace work?](https://blog.packagecloud.io/eng/2016/02/29/how-does-strace-work/)

## ltrace

* [How does ltrace work?](https://blog.packagecloud.io/eng/2016/03/14/how-does-ltrace-work/)

## gdb

当发现进程异常时，比如CPU过高可以通过`gdb`attach到进程上查看具体情况，如果是多线程程序可以通过`info threads`选择异常的线程查看。

```
gdb -p `pidof program`

info threads
thread $id
```

## gstack

`gstack` attaches to the active process named by the pid on the command line, and prints out an execution stack trace. If ELF symbols exist in the binary (usually the case unless you have run strip(1)), then symbolic addresses are printed as well.

```
$gstack 31310
Thread 2 (Thread 0x7f131c596700 (LWP 31315)):
#0  0x00007f13d2a6b1bd in nanosleep () from /lib64/libc.so.6
#1  0x00007f13d2a9bed4 in usleep () from /lib64/libc.so.6
...
#10 0x0000000000416cb0 in main (argc=<optimized out>, argv=<optimized out>) at base_server/base_so_loader.cc:216
```

## disassemble

```
objdump -d /path/to/binary

gdb a.out
info functions
disassemble /m main
```

* https://stackoverflow.com/questions/5125896/how-to-disassemble-a-binary-executable-in-linux-to-get-the-assembly-code


## backtrace

[backtrace()](https://man7.org/linux/man-pages/man3/backtrace.3.html) returns a backtrace for the calling program, in the array pointed to by buffer.

``` cpp
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BT_BUF_SIZE 100

void myfunc3()
{
    int nptrs;
    void *buffer[BT_BUF_SIZE];
    char **strings;

    nptrs = backtrace(buffer, BT_BUF_SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

    //--nptrs;
	//memmove(buffer, buffer + 1, sizeof(void*) * nptrs);

    for (int i = 0; i < nptrs; ++i) {
        printf("%s\n", buffer[i]);
    }

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
              would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < nptrs; ++j)
        printf("%s\n", strings[j]);

    free(strings);
}

/* "static" means don't export the symbol... */
static void myfunc2()
{
    myfunc3();
}

void myfunc(int ncalls)
{
    if (ncalls > 1)
        myfunc(ncalls - 1);
    else
        myfunc2();
}

int main(int argc, char *argv[])
{
    /*if (argc != 2) {
        fprintf(stderr, "%s num-calls\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    myfunc(atoi(argv[1]));*/

    myfunc(3);
    exit(EXIT_SUCCESS);
}
/*
g++ backtrace.cc

$ ./a.out 
backtrace() returned 8 addresses
./a.out(_Z7myfunc3v+0x2e) [0x5568d100da93]
./a.out(+0xb65) [0x5568d100db65]
./a.out(_Z6myfunci+0x25) [0x5568d100db8d]
./a.out(_Z6myfunci+0x1e) [0x5568d100db86]
./a.out(_Z6myfunci+0x1e) [0x5568d100db86]
./a.out(main+0x19) [0x5568d100dba9]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xe7) [0x7f57275c9bf7]
./a.out(_start+0x2a) [0x5568d100d9aa]
*/
```

使用GDB调试：

```
(gdb) p buffer[0]
$2 = (void *) 0x555555554873 <myfunc3()+46>
(gdb) p buffer[1]
$3 = (void *) 0x55555555497e <myfunc2()+9>
(gdb) p buffer[2]
$4 = (void *) 0x5555555549a6 <myfunc(int)+37>
(gdb) p buffer[3]
$5 = (void *) 0x55555555499f <myfunc(int)+30>
(gdb) p buffer[4]
$6 = (void *) 0x55555555499f <myfunc(int)+30>
(gdb) p buffer[5]
$7 = (void *) 0x5555555549c2 <main(int, char**)+25>
(gdb) p buffer[6]
$8 = (void *) 0x7ffff7a03bf7 <__libc_start_main+231>
(gdb) p buffer[7]
$9 = (void *) 0x55555555478a <_start+42>
(gdb) p buffer[8]
$10 = (void *) 0x0
```

## assert

The definition of the macro assert depends on another macro, `NDEBUG`, which is not defined by the standard library.

* If `NDEBUG` is defined as a macro name at the point in the source code where `<cassert>` is included, then assert does nothing.
* If `NDEBUG` is not defined, then assert checks if its argument (which must have scalar type) compares equal to zero. If it does, assert outputs implementation-specific diagnostic information on the standard error output and calls `std::abort`. The diagnostic information is required to include the text of expression, as well as the values of the standard macros `__FILE__`, `__LINE__`, and the standard variable `__func__` (since C++11).

``` cpp
#include <iostream>
// uncomment to disable assert()
// #define NDEBUG
#include <cassert>
 
// Use (void) to silent unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))
 
int main()
{
    assert(2+2==4);
    std::cout << "Execution continues past the first assert\n";
    assertm(2+2==5, "There are five lights");
    std::cout << "Execution continues past the second assert\n";
    assert((2*2==4) && "Yet another way to add assert message");
}
/*
prog.exe: prog.cc:13: int main(): Assertion `((void)"There are five lights", 2+2==5)' failed.
Execution continues past the first assert
Aborted
*/
```

``` cpp
#include <stdio.h>
#include <exception>
#include <cassert>

int main()
{
    try
    {
        assert(0);
    }
    catch (std::exception &e)
    {
        printf("std::exception\n");
    }
    catch (...)
    {
        printf("...\n");
    }
}
/*
prog.exe: prog.cc:9: int main(): Assertion `0' failed.
Aborted
*/
```

* https://en.cppreference.com/w/cpp/error/assert

## lsof

```
# 查看进程打开文件的情况
lsof -p pid 
```

## c++filt (Demangle C++ and Java symbols)

使用`c++filt`对符号进行 demangle 得到可读的符号名：

```
dlopen(./liballocatesvr_plugin.so) failed(./liballocatesvr_plugin.so: undefined symbol: _ZTIN6google8protobuf7MessageE)
```

```
$c++filt _ZTIN6google8protobuf7MessageE
typeinfo for google::protobuf::Message
```

## addr2line (convert addresses into file names and line numbers)

addr2line translates addresses into file names and line numbers. Given an address in an executable or an offset in a section of a relocatable object, it uses the debugging information to figure out which file name and line number are associated with it.

安装方法：https://command-not-found.com/addr2line

```
# ubuntu
apt-get install binutils
```

https://linux.die.net/man/1/addr2line


测试代码：


``` cpp
// backtrace.c
#include <execinfo.h>               
#include <stdio.h>
#include <stdlib.h>

/* Obtain a backtrace and print it to stdout. */
void
print_trace (void)
{
        void *array[10];
        size_t size;
        char **strings;
        size_t i;

        size = backtrace (array, 10);
        strings = backtrace_symbols (array, size);

        printf ("Obtained %zd stack frames.\n", size);

        for (i = 0; i < size; i++)
                printf ("%s\n", strings[i]);

        free (strings);
}

/* A dummy function to make the backtrace more interesting. */
void
dummy_function (void)
{
        print_trace (); 
}

int
main (void)
{
        dummy_function (); 
        return 0;
}
```

默认，符号全部导出，编译构建：

gcc -rdynamic backtrace.c

执行结果，可以显示堆栈的符号名称：

```
$./a.out 
Obtained 5 stack frames.
./a.out(print_trace+0x19) [0x400896]
./a.out(dummy_function+0x9) [0x400918]
./a.out(main+0x9) [0x400923]
/lib64/libc.so.6(__libc_start_main+0xf5) [0x7f0e5326e555]
./a.out() [0x4007b9]
```


使用`version-script`控制不导出符号，`symbol.txt`配置为（即，只导出 foo* 开头的符号）：

```
{
    global: foo*;
    local: *;
};
```

编译构建：

gcc -rdynamic backtrace.c -Wl,--version-script=symbol.txt

执行结果，只显示了地址信息，而没有符号信息：

```
$./a.out 
Obtained 5 stack frames.
./a.out() [0x400676]
./a.out() [0x4006f8]
./a.out() [0x400703]
/lib64/libc.so.6(__libc_start_main+0xf5) [0x7fbec9745555]
./a.out() [0x400599]
```

使用 addr2line 对地址进行翻译：

```
$addr2line 0x400676 -f -e a.out 
print_trace
:?
```

查看 elf 头部信息：

```
$readelf -h a.out 
ELF 头：
  Magic：  7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  类别:                              ELF64
  数据:                              2 补码，小端序 (little endian)
  版本:                              1 (current)
  OS/ABI:                            UNIX - System V
  ABI 版本:                          0
  类型:                              EXEC (可执行文件)
  系统架构:                          Advanced Micro Devices X86-64
  版本:                              0x1
  入口点地址：              0x400570
  程序头起点：              64 (bytes into file)
  Start of section headers:          6776 (bytes into file)
  标志：             0x0
  本头的大小：       64 (字节)
  程序头大小：       56 (字节)
  Number of program headers:         9
  节头大小：         64 (字节)
  节头数量：         30
  字符串表索引节头： 29
```

注意，在 ubuntu 18.04 上测试上述程序，使用 addr2line 无法翻译地址信息。具体原因可参考：[addr2line not woking on Ubuntu 16.10?](https://stackoverflow.com/questions/41890103/addr2line-not-woking-on-ubuntu-16-10)

```
$ addr2line 0x55c43fa639fd -f -e a.out 
??
??:0
```

elf 的头部信息显示：The entry point doesn't start at `0x400000`

```
$ readelf -h a.out
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              DYN (Shared object file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x8f0
  Start of program headers:          64 (bytes into file)
  Start of section headers:          6760 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         9
  Size of section headers:           64 (bytes)
  Number of section headers:         29
  Section header string table index: 28
```

## LD_DEBUG ./a.out

通过`LD_DEBUG`显示符号链接过程。

```
$ LD_DEBUG=all ./prog 2>&1 | grep cout
       919:     symbol=_ZSt5wcout;  lookup in file=./prog [0]
       919:     symbol=_ZSt5wcout;  lookup in file=/lib/x86_64-linux-gnu/libdl.so.2 [0]
       919:     symbol=_ZSt5wcout;  lookup in file=/usr/lib/x86_64-linux-gnu/libstdc++.so.6 [0]
       919:     binding file /usr/lib/x86_64-linux-gnu/libstdc++.so.6 [0] to /usr/lib/x86_64-linux-gnu/libstdc++.so.6 [0]: normal symbol `_ZSt5wcout' [GLIBCXX_3.4]
       919:     symbol=_ZSt4cout;  lookup in file=./prog [0]
       919:     binding file /usr/lib/x86_64-linux-gnu/libstdc++.so.6 [0] to ./prog [0]: normal symbol `_ZSt4cout' [GLIBCXX_3.4]
       919:     symbol=_ZSt4cout;  lookup in file=/lib/x86_64-linux-gnu/libdl.so.2 [0]
       919:     symbol=_ZSt4cout;  lookup in file=/usr/lib/x86_64-linux-gnu/libstdc++.so.6 [0]
       919:     binding file ./prog [0] to /usr/lib/x86_64-linux-gnu/libstdc++.so.6 [0]: normal symbol `_ZSt4cout' [GLIBCXX_3.4]
       919:     symbol=_ZSt4cout;  lookup in file=./libbar.so [0]
       919:     symbol=_ZSt4cout;  lookup in file=/lib/x86_64-linux-gnu/libc.so.6 [0]
       919:     symbol=_ZSt4cout;  lookup in file=/lib64/ld-linux-x86-64.so.2 [0]
       919:     symbol=_ZSt4cout;  lookup in file=./prog [0]
       919:     binding file ./libbar.so [0] to ./prog [0]: normal symbol `_ZSt4cout'
```

