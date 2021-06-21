---
layout: post
title:  "Perf in Action"
date:   2021-06-21 12:00:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Linux performance events subsystem

* The perf event subsystem was merged into the Linux kernel in version `2.6.31` and introduced the `sys_perf_event_open` system call.
* Uses specail purpose registers on the CPU to count the number of "events"
* An HW(Hardware) event can be, for example, the number of cache miss suffered or mispredicted branches
* SW(Software) events, like page misses, are also supported
* Performance counters are accessed via file descriptors using the above mentioned system call
* `perf` if an user space utility that is part of the kernel repository
* Basic usage: data is collected by using the `perf-record`(Run a command and record its profile into perf.data) tool and displayed with `perf-report`(Read perf.data created by perf record and display the profile)

> WHY DO WE CARE?

* The Linux Performance Events Subsystem provides **a low overhead** way to measure the workloads of a single application or the full system
* It's at least an order of magnitude faster than an instrumenting profiler
* It provides far more information compared to statistical profiler

> WHAT IS MISSING

* Annotating the objdump output one event at a time is not enough for efficiently finding bottlenecks
* A real GUI that can display multiple events and their relations is missing
* New CPU's have a buffer that records the last taken branches but a support to exploit it is missing

> PERF EVNETS CONVERTER

* As a first step a converter tool for the perf-tools data format has been introduced
* The tool is capable to convert a perf data file to a `callgrind` one that can be displayed with `kcachegrind`
  + multiple events are supported
  + annotated source code, assembly and function list view
  + complete inline chain  

* The callgrind converter and the new GUI under development will offer an easy way to non experts to navigate and understand the profiled application

> PERF EVENTS VISUALIZER

* KCachegrind doesn't permit to show an arbitrary number of events at the same time
* A new converter and a web-based GUI is under development
* The converter reads the raw perf data file and produces spreadsheets, cycle accounting trees and call graphs
* The GUI will be able to:
  + present the available data in spreadsheets, cycle accounting trees and callgraphs
  + offer insights on the callgraph, e.g. mark as hot virtual methods with high call counts 
  + correlate different HW/SW events to gain a deeper understanding of the performance bottlenecks


# perf: Linux profiling with performance counters

> ...More than just counters...


`perf` also called `perf_events`. `perf` is powerful: it can instrument CPU performance counters, tracepoints, kprobes, and uprobes (dynamic tracing). It is capable of lightweight profiling. It is also included in the Linux kernel, under tools/perf, and is frequently updated and enhanced.

`perf` began as a tool for using the performance counters subsystem in Linux, and has had various enhancements to add tracing capabilities.

**Performance** counters are CPU hardware registers that count hardware events such as instructions executed, cache-misses suffered, or branches mispredicted. They form a basis for profiling applications to trace dynamic control flow and identify hotspots. `perf` provides rich generalized abstractions over hardware specific capabilities. Among others, it provides per task, per CPU and per-workload counters, sampling on top of these and source code event annotation.

Tracepoints are instrumentation points placed at logical locations in code, such as for system calls, TCP/IP events, file system operations, etc. These have negligible overhead when not in use, and can be enabled by the perf command to collect information including timestamps and stack traces. perf can also dynamically create tracepoints using the kprobes and uprobes frameworks, for kernel and userspace dynamic tracing. The possibilities with these are endless.

## Command

The `perf` tool offers a rich set of commands to collect and analyze performance and trace data. The command line usage is reminiscent of git in that there is a generic tool, perf, which implements a set of commands: stat, record, report, ...

The list of supported commands:

```
perf

 usage: perf [--version] [--help] COMMAND [ARGS]

 The most commonly used perf commands are:
  annotate        Read perf.data (created by perf record) and display annotated code
  archive         Create archive with object files with build-ids found in perf.data file
  bench           General framework for benchmark suites
  buildid-cache   Manage <tt>build-id</tt> cache.
  buildid-list    List the buildids in a perf.data file
  diff            Read two perf.data files and display the differential profile
  inject          Filter to augment the events stream with additional information
  kmem            Tool to trace/measure kernel memory(slab) properties
  kvm             Tool to trace/measure kvm guest os
  list            List all symbolic event types
  lock            Analyze lock events
  probe           Define new dynamic tracepoints
  record          Run a command and record its profile into perf.data
  report          Read perf.data (created by perf record) and display the profile
  sched           Tool to trace/measure scheduler properties (latencies)
  script          Read perf.data (created by perf record) and display trace output
  stat            Run a command and gather performance counter statistics
  test            Runs sanity tests.
  timechart       Tool to visualize total system behavior during a workload
  top             System profiling tool.

 See 'perf help COMMAND' for more information on a specific command.
```

Certain commands require special support in the kernel and may not be available. To obtain the list of options for each command, simply type the command name followed by `-h`:

```
perf stat -h

 usage: perf stat [<options>] [<command>]

    -e, --event <event>   event selector. use 'perf list' to list available events
    -i, --no-inherit      child tasks do not inherit counters
    -p, --pid <n>         stat events on existing process id
    -t, --tid <n>         stat events on existing thread id
    -a, --all-cpus        system-wide collection from all CPUs
    -c, --scale           scale/normalize counters
    -v, --verbose         be more verbose (show counter open errors, etc)
    -r, --repeat <n>      repeat command and print average + stddev (max: 100)
    -n, --null            null run - dont start any counters
    -B, --big-num         print large numbers with thousands' separators
```

## Events

The `perf` tool supports a list of measurable events. The tool and underlying kernel interface can measure events coming from different sources. For instance, some event are pure kernel counters, in this case they are called **software events**. Examples include: `context-switches`, `minor-faults`.

Another source of events is the processor itself and its Performance Monitoring Unit (PMU). It provides a list of events to measure micro-architectural events such as the number of cycles, instructions retired, L1 cache misses and so on. Those events are called **PMU hardware events** or **hardware events** for short. They vary with each processor type and model.

The perf_events interface also provides a small set of common hardware events monikers. On each processor, those events get mapped onto an actual events provided by the CPU, if they exists, otherwise the event cannot be used. Somewhat confusingly, these are also called **hardware events** and **hardware cache events**.

Finally, there are also **tracepoint events** which are implemented by the kernel ftrace infrastructure. Those are only available with the 2.6.3x and newer kernels.

To obtain a list of supported events:

```
perf list

List of pre-defined events (to be used in -e):

 cpu-cycles OR cycles                       [Hardware event]
 instructions                               [Hardware event]
 cache-references                           [Hardware event]
 cache-misses                               [Hardware event]
 branch-instructions OR branches            [Hardware event]
 branch-misses                              [Hardware event]
 bus-cycles                                 [Hardware event]

 cpu-clock                                  [Software event]
 task-clock                                 [Software event]
 page-faults OR faults                      [Software event]
 minor-faults                               [Software event]
 major-faults                               [Software event]
 context-switches OR cs                     [Software event]
 cpu-migrations OR migrations               [Software event]
 alignment-faults                           [Software event]
 emulation-faults                           [Software event]

 L1-dcache-loads                            [Hardware cache event]
 L1-dcache-load-misses                      [Hardware cache event]
 L1-dcache-stores                           [Hardware cache event]
 L1-dcache-store-misses                     [Hardware cache event]
 L1-dcache-prefetches                       [Hardware cache event]
 L1-dcache-prefetch-misses                  [Hardware cache event]
 L1-icache-loads                            [Hardware cache event]
 L1-icache-load-misses                      [Hardware cache event]
 L1-icache-prefetches                       [Hardware cache event]
 L1-icache-prefetch-misses                  [Hardware cache event]
 LLC-loads                                  [Hardware cache event]
 LLC-load-misses                            [Hardware cache event]
 LLC-stores                                 [Hardware cache event]
 LLC-store-misses                           [Hardware cache event]

 LLC-prefetch-misses                        [Hardware cache event]
 dTLB-loads                                 [Hardware cache event]
 dTLB-load-misses                           [Hardware cache event]
 dTLB-stores                                [Hardware cache event]
 dTLB-store-misses                          [Hardware cache event]
 dTLB-prefetches                            [Hardware cache event]
 dTLB-prefetch-misses                       [Hardware cache event]
 iTLB-loads                                 [Hardware cache event]
 iTLB-load-misses                           [Hardware cache event]
 branch-loads                               [Hardware cache event]
 branch-load-misses                         [Hardware cache event]

 rNNN (see 'perf list --help' on how to encode it) [Raw hardware event descriptor]

 mem:<addr>[:access]                        [Hardware breakpoint]

 kvmmmu:kvm_mmu_pagetable_walk              [Tracepoint event]

 [...]

 sched:sched_stat_runtime                   [Tracepoint event]
 sched:sched_pi_setprio                     [Tracepoint event]
 syscalls:sys_enter_socket                  [Tracepoint event]
 syscalls:sys_exit_socket                   [Tracepoint event]

 [...]
```

# Counting with perf stat

For any of the supported events, perf can keep a running count during process execution. In counting modes, the occurrences of events are simply aggregated and presented on standard output at the end of an application run. To generate these statistics, use the stat command of perf. For instance:

```
# perf stat ./perf_stat_example
pid: 24298

 Performance counter stats for './perf_stat_example':

       1251.260398      task-clock (msec)         #    0.980 CPUs utilized          
               107      context-switches          #    0.086 K/sec                  
                 0      cpu-migrations            #    0.000 K/sec                  
            48,878      page-faults               #    0.039 M/sec                  
   <not supported>      cycles                                                      
   <not supported>      instructions                                                
   <not supported>      branches                                                    
   <not supported>      branch-misses                                               

       1.276480272 seconds time elapsed
```

With no events specified, `perf stat` collects the common events listed above. Some are software events, such as context-switches, others are generic hardware events such as cycles. After the hash sign, derived metrics may be presented, such as 'IPC' (instructions per cycle).

## Options controlling event selection

It is possible to measure **one or more events** per run of the perf tool. Events are designated using their symbolic names followed by optional unit masks and modifiers. Event names, unit masks, and modifiers are **case insensitive**.

By default, events are measured at both user and kernel levels:

```
# perf stat -e context-switches,page-faults ./perf_stat_example
pid: 26956

 Performance counter stats for './perf_stat_example':

                87      context-switches                                            
            48,880      page-faults                                                 

       1.260494406 seconds time elapsed
```

To measure more than one event, simply provide a comma-separated list with no space:

```
perf stat -e cycles,instructions,cache-misses [...]
```

## Attaching to a running process

It is possible to use perf to attach to an already running thread or process. This requires the permission to attach along with the thread or process ID. To attach to a process, the `-p` option must be the process ID. To attach to the sshd service that is commonly running on many Linux machines, issue:

```
# ps ax | grep "sshd -D"
  917 ?        Ss     4:30 /usr/sbin/sshd -D
27561 pts/0    S+     0:00 grep --color=auto sshd -D
# perf stat -p 917
^C
 Performance counter stats for process id '917':

         12.132072      task-clock (msec)         #    0.002 CPUs utilized          
                45      context-switches          #    0.004 M/sec                  
                 0      cpu-migrations            #    0.000 K/sec                  
               640      page-faults               #    0.053 M/sec                  
   <not supported>      cycles                                                      
   <not supported>      instructions                                                
   <not supported>      branches                                                    
   <not supported>      branch-misses                                               

       5.384885419 seconds time elapsed
```

Even though we are attaching to a process, we can still pass the name of a command. It is used to time the measurement. Without it, perf monitors until it is killed.

Also note that when attaching to a process, all threads of the process are monitored. Furthermore, given that inheritance is on by default, child processes or threads will also be monitored. To turn this off, you must use the `-i` option. It is possible to attach a specific thread within a process. By thread, we mean kernel visible thread. In other words, a thread visible by the ps or top commands. To attach to a thread, the `-t` option must be used. 

```
$perf stat -p 39911 sleep 3

 Performance counter stats for process id '39911':

        131.862207      task-clock (msec)         #    0.044 CPUs utilized          
            12,148      context-switches          #    0.092 M/sec                  
                59      cpu-migrations            #    0.447 K/sec                  
                 4      page-faults               #    0.030 K/sec                  
                 0      cycles                    #    0.000 GHz                    
                 0      stalled-cycles-frontend   #    0.00% frontend cycles idle   
                 0      stalled-cycles-backend    #    0.00% backend  cycles idle   
                 0      instructions              #    0.00  insns per cycle        
                 0      branches                  #    0.000 K/sec                  
                 0      branch-misses             #    0.000 K/sec                  

       3.001477374 seconds time elapsed
```

We look at `rsyslogd`, because it always runs on Ubuntu, with multiple threads. (使用 `-t` 采样指定的线程)

```
# ps -L ax | fgrep rsyslogd 
  914   914 ?        Ssl    0:00 /usr/sbin/rsyslogd -n
  914   942 ?        Ssl    1:44 /usr/sbin/rsyslogd -n
  914   943 ?        Ssl    0:00 /usr/sbin/rsyslogd -n
  914   944 ?        Ssl    1:39 /usr/sbin/rsyslogd -n
29083 29083 pts/0    S+     0:00 grep -F --color=auto rsyslogd
# perf stat -t 942 sleep 2

 Performance counter stats for thread id '942':

          0.045408      task-clock (msec)         #    0.000 CPUs utilized          
                 3      context-switches          #    0.066 M/sec                  
                 0      cpu-migrations            #    0.000 K/sec                  
                 0      page-faults               #    0.000 K/sec                  
   <not supported>      cycles                                                      
   <not supported>      instructions                                                
   <not supported>      branches                                                    
   <not supported>      branch-misses                                               

       2.000827441 seconds time elapsed
```

In this example, the thread 942 did not run during the 2s of the measurement. Otherwise, we would see a count value. Attaching to kernel threads is possible, though not really recommended. Given that kernel threads tend to be pinned to a specific CPU, it is best to use the cpu-wide mode.

## Options controlling output

`perf stat` can modify output to suit different needs.

* Pretty printing large numbers

For most people, it is hard to read large numbers. With perf stat, it is possible to print large numbers using the comma separator for thousands (US-style). For that the `-B` option and the correct locale for `LC_NUMERIC` must be set. 

* Machine readable output

perf stat can also print counts in a format that can easily be imported into a spreadsheet or parsed by scripts. The `-x` option alters the format of the output and allows users to pass a field delimiter. This makes is easy to produce CSV-style output. Note that the `-x` option is not compatible with `-B`.

```
# perf stat -x, date
Mon Jun 21 16:37:07 CST 2021
0.524149,,task-clock,524149,100.00,0.046,CPUs utilized
3,,context-switches,524149,100.00,0.006,M/sec
0,,cpu-migrations,524149,100.00,0.000,K/sec
66,,page-faults,524149,100.00,0.126,M/sec
<not supported>,,cycles,0,100.00,,
<not supported>,,instructions,0,100.00,,
<not supported>,,branches,0,100.00,,
<not supported>,,branch-misses,0,100.00,,
```

# Sampling with perf record

The perf tool can be used to collect profiles on per-thread, per-process and per-cpu basis.

There are several commands associated with sampling: `record`, `report`, `annotate`. You must first collect the samples using `perf record`. This generates an output file called `perf.data`. That file can then be analyzed, possibly on another machine, using the `perf report` and `perf annotate` commands. 

## Period and rate

The perf_events interface allows two modes to express the sampling period:

1. the number of occurrences of the event (`period`) (即，`-c 200`) 
2. the average rate of samples/sec (`frequency`) (即，`-F 1000`) 

The perf tool defaults to the average rate. It is set to `1000Hz`, or `1000 samples/sec`. That means that the kernel is dynamically adjusting the sampling period to achieve the target average rate. The adjustment in period is reported in the raw profile data. In contrast, with the other mode, the sampling period is set by the user and does not vary between samples. There is currently no support for sampling period randomization.

## Collecting samples

By default, perf record operates in per-thread mode, with inherit mode enabled. The simplest mode looks as follows, when executing a simple program that busy loops:

```
perf record ./noploop 1

[ perf record: Woken up 1 times to write data ]
[ perf record: Captured and wrote 0.002 MB perf.data (~89 samples) ]
```

The example above collects samples for event cycles at an average target rate of `1000Hz`. The resulting samples are saved into the perf.data file. If the file already existed, you may be prompted to pass `-f` to overwrite it. To put the results in a specific file, use the `-o` option.

> WARNING: The number of reported samples is only an estimate. It does not reflect the actual number of samples collected. The estimate is based on the number of bytes written to the perf.data file and the minimal sample size. But the size of each sample depends on the type of measurement. Some samples are generated by the counters themselves but others are recorded to support symbol correlation during post-processing, e.g., mmap() information.

To get an accurate number of samples for the `perf.data` file, it is possible to use the `perf report` command:

```
perf record ./noploop 1

[ perf record: Woken up 1 times to write data ]
[ perf record: Captured and wrote 0.058 MB perf.data (~2526 samples) ]
perf report -D -i perf.data | fgrep RECORD_SAMPLE | wc -l

1280
```

To specify a custom rate, it is necessary to use the `-F` option. For instance, to sample on event instructions only at the user level and at an average rate of `250 samples/sec`:

```
perf record -e instructions:u -F 250 ./noploop 4

[ perf record: Woken up 1 times to write data ]
[ perf record: Captured and wrote 0.049 MB perf.data (~2160 samples) ]
```

To specify a sampling period, instead, the `-c` option must be used. For instance, to collect a sample every 2000 occurrences of event instructions only at the user level only:

```
perf record -e retired_instructions:u -c 2000 ./noploop 4

[ perf record: Woken up 55 times to write data ]
[ perf record: Captured and wrote 13.514 MB perf.data (~590431 samples) ]
```

# Sample analysis with perf report

Samples collected by perf record are saved into a binary file called, by default, `perf.data`. The perf report command reads this file and generates a concise execution profile. By default, samples are sorted by functions with the most samples first. It is possible to customize the sorting order and therefore to view the data differently.

```
perf report

# Events: 1K cycles
#
# Overhead          Command                   Shared Object  Symbol
# ........  ...............  ..............................  .....................................
#
    28.15%      firefox-bin  libxul.so                       [.] 0xd10b45
     4.45%          swapper  [kernel.kallsyms]               [k] mwait_idle_with_hints
     4.26%          swapper  [kernel.kallsyms]               [k] read_hpet
     2.13%      firefox-bin  firefox-bin                     [.] 0x1e3d
     1.40%  unity-panel-ser  libglib-2.0.so.0.2800.6         [.] 0x886f1
     [...]
```

* The column 'Overhead' indicates the percentage of the overall samples collected in the corresponding function. 
* The second column reports the process from which the samples were collected. In per-thread/per-process mode, this is always the name of the monitored command. But in cpu-wide mode, the command can vary. 
* The third column shows the name of the ELF image where the samples came from. If a program is dynamically linked, then this may show the name of a shared library. When the samples come from the kernel, then the pseudo ELF image name `kernel.kallsyms` is used. 
* The fourth column indicates the privilege level at which the sample was taken, i.e. when the program was running when it was interrupted:

```
[.] user level
[k] kernel level
[g] guest kernel level (virtualization)
[u] guest os user space
[H] hypervisor
```

* The final column shows the symbol name.

## Options controlling output

To make the output easier to parse, it is possible to change the column separator to a single character:

```
# perf report -t ","

Samples: 302K of event 'cpu-clock', Event count (approx.): 302077                                                                                                                                        
  4.48%,gamesvr,[vdso]                      ,[.] 0x0000000000000e6c
  4.37%,gamesvr,libc-2.17.so                ,[.] _dl_addr
  3.78%,gamesvr,gamesvr                     ,[.] JLib::my_gettimeofday(timeval*)
```

# Source level analysis with perf annotate

It is possible to drill down to the instruction level with `perf annotate`. For that, you need to invoke perf annotate with the name of the command to annotate. All the functions with samples will be **disassembled** and each instruction will have its relative percentage of samples reported:

```
# perf record -c 1000 ./perf_top_example
# perf annotate
```

`perf annotate` can generate sourcecode level information if the application is compiled with `-ggdb`. 

![perf_annotate](/assets/images/202106/perf_annotate.png)

通过`perf top`选择对应的函数回车也可以调用`perf annotate`显示代码。

![perf_annotate2](/assets/images/202106/perf_annotate2.png)

# Live analysis with perf top

The perf tool can operate in a mode similar to the Linux `top` tool, printing sampled functions in real time. The default sampling event is cycles and default order is descending number of samples per symbol, thus `perf top` shows the functions where most of the time is spent. By default, `perf top` operates in processor-wide mode, monitoring all online CPUs at both user and kernel levels. It is possible to monitor only a subset of the CPUS using the `-C` option.

```
perf top
-------------------------------------------------------------------------------------------------------------------------------------------------------
  PerfTop:     260 irqs/sec  kernel:61.5%  exact:  0.0% [1000Hz
cycles],  (all, 2 CPUs)
-------------------------------------------------------------------------------------------------------------------------------------------------------

            samples  pcnt function                       DSO
            _______ _____ ______________________________ ___________________________________________________________

              80.00 23.7% read_hpet                      [kernel.kallsyms]
              14.00  4.2% system_call                    [kernel.kallsyms]
              14.00  4.2% __ticket_spin_lock             [kernel.kallsyms]
              14.00  4.2% __ticket_spin_unlock           [kernel.kallsyms]
               8.00  2.4% hpet_legacy_next_event         [kernel.kallsyms]
               7.00  2.1% i8042_interrupt                [kernel.kallsyms]
               7.00  2.1% strcmp                         [kernel.kallsyms]
               6.00  1.8% _raw_spin_unlock_irqrestore    [kernel.kallsyms]
               6.00  1.8% pthread_mutex_lock             /lib/i386-linux-gnu/libpthread-2.13.so
               6.00  1.8% fget_light                     [kernel.kallsyms]
               6.00  1.8% __pthread_mutex_unlock_usercnt /lib/i386-linux-gnu/libpthread-2.13.so
               5.00  1.5% native_sched_clock             [kernel.kallsyms]
               5.00  1.5% drm_addbufs_sg                 /lib/modules/2.6.38-8-generic/kernel/drivers/gpu/drm/drm.ko
```

![perf_top_help](/assets/images/202106/perf_top_help.png)


# Example Usage

系统环境：

```
$ cat /etc/issue
Ubuntu 18.04.1 LTS \n \l
$ uname -r
4.15.0-29-generic
$ cat "/boot/config-`uname -r`" | grep "PERF_EVENT"
CONFIG_HAVE_PERF_EVENTS=y
CONFIG_PERF_EVENTS=y
CONFIG_HAVE_PERF_EVENTS_NMI=y
CONFIG_PERF_EVENTS_INTEL_UNCORE=y
CONFIG_PERF_EVENTS_INTEL_RAPL=m
CONFIG_PERF_EVENTS_INTEL_CSTATE=m
# CONFIG_PERF_EVENTS_AMD_POWER is not set
CONFIG_SECURITY_PERF_EVENTS_RESTRICT=y
```

常用命令：

```
perf --help
perf record --help
perf report --help

perf top -p $pid

perf stat -p $pid
perf stat --repeat 5 -e cache-misses,cache-references,instructions,cycles ./perf_stat_example

perf record ./target
perf report
```

测试代码：[perf demo](https://github.com/gerryyang/mac-utils/tree/master/programing/cpp/performance/perf/demo)

```
# perf record -c 1000 ./perf_top_example
pid: 13985
[ perf record: Woken up 16 times to write data ]
[ perf record: Captured and wrote 3.948 MB perf.data (128706 samples) ]
# perf report -U --stdio | head -n 20
# To display the perf.data header info, please use --header/--header-only options.
#
#
# Total Lost Samples: 0
#
# Samples: 128K of event 'cpu-clock'
# Event count (approx.): 128706000
#
# Overhead  Command          Shared Object      Symbol                            
# ........  ...............  .................  ..................................
#
    99.90%  perf_top_exampl  perf_top_example   [.] _Z19compute_pi_baselinem
     0.01%  perf_top_exampl  [kernel.kallsyms]  [k] exit_to_usermode_loop
     0.01%  perf_top_exampl  [kernel.kallsyms]  [k] unmap_page_range
     0.00%  perf_top_exampl  [kernel.kallsyms]  [k] filemap_map_pages
     0.00%  perf_top_exampl  [kernel.kallsyms]  [k] __softirqentry_text_start
     0.00%  perf_top_exampl  [kernel.kallsyms]  [k] finish_task_switch
     0.00%  perf_top_exampl  [kernel.kallsyms]  [k] __do_page_fault
     0.00%  perf_top_exampl  [kernel.kallsyms]  [k] _raw_spin_unlock_irqrestore
     0.00%  perf_top_exampl  [kernel.kallsyms]  [k] iowrite16
```

* Options included `-a` to trace all CPUs, and `-g` to capture call graphs (stack traces). 

更多用法：[Brendan Gregg's perf examples](http://www.brendangregg.com/perf.html)


# Refer

* [Brendan Gregg's perf examples](http://www.brendangregg.com/perf.html)
* [https://perf.wiki.kernel.org/index.php/Main_Page](https://perf.wiki.kernel.org/index.php/Main_Page)
* [Tutorial - Linux kernel profiling with perf](https://perf.wiki.kernel.org/index.php/Tutorial)
* [PERFORMANCE TOOLS DEVELOPMENTS](https://indico.cern.ch/event/141309/contributions/1369454/attachments/126021/178987/RobertoVitillo_FutureTech_EDI.pdf)
* [CppCon 2015: Chandler Carruth "Tuning C++: Benchmarks, and CPUs, and Compilers! Oh My!"](https://www.youtube.com/watch?v=nXaxk27zwlk)