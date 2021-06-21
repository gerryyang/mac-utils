---
layout: post
title:  "gperftools in Action"
date:   2021-06-18 21:00:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}


> gperftools (originally Google Performance Tools)
> 
> The fastest malloc we’ve seen; works particularly well with threads and STL. Also: thread-friendly heap-checker, heap-profiler, and cpu-profiler.

* wiki介绍：https://github.com/gperftools/gperftools/wiki
* 下载地址：https://github.com/gperftools/gperftools/releases/tag/gperftools-2.9.1
* 安装说明：https://github.com/gperftools/gperftools/blob/master/INSTALL
* API用法: 参考`gperftools/profiler.h`
* 安装依赖
  + yum install graphviz
  + yum install ghostscript  (安装 ps2pdf)
  + yum install kcachegrind 
    - http://kcachegrind.sourceforge.net/html/Download.html
  + brew install qcachegrind (MacOS)
    - https://stackoverflow.com/questions/4473185/need-kcachegrind-like-profiling-tools-for-mac

# HEAP PROFILER

See [docs/heapprofile.html](https://gperftools.github.io/gperftools/heapprofile.html) for information about how to use tcmalloc's heap profiler and analyze its output.

1. Link your executable with `-ltcmalloc`
2. Run your executable with the `HEAPPROFILE` environment var set: `HEAPPROFILE=/tmp/heapprof <path/to/binary> [binary args]`
3. Run `pprof` to analyze the heap usage

```
$ pprof --text <path/to/binary> heapprof.0045.heap
```

You can also use `LD_PRELOAD` to heap-profile an executable that you didn't compile.

测试代码：

``` cpp
#include <iostream>
using namespace std;

void f1() {
        int i;
        for (i = 0; i < 1024* 1024; ++i) {
                short* p = new short;
                //delete p;
        }
}

void f2()  { 
        int i;
        for (i = 0; i < 1024 * 1024; ++i) {
                int* p = new int;
                //delete p;
        }
}

int main() { 
        f1();
        f2();
        return 0;
}
```

编译及内存使用分析：

```
$ g++ c.cc -L$HOME/tools/gperftools-2.9.1_install/lib -ltcmalloc -lprofiler
$ HEAPPROFILE=heapprofile.out ./a.out
Starting tracking the heap
Dumping heap profile to heapprofile.out.0001.heap (Exiting, 6 MB in use)
$ pprof --text a.out heapprofile.out.0001.heap 
Using local file a.out.
Using local file heapprofile.out.0001.heap.
Total: 6.0 MB
     4.0  66.7%  66.7%      4.0  66.7% f2
     2.0  33.3% 100.0%      2.0  33.3% f1
     0.0   0.0% 100.0%      6.0 100.0% __libc_start_main
     0.0   0.0% 100.0%      6.0 100.0% _start
     0.0   0.0% 100.0%      6.0 100.0% main
```

使用`--svg`或`--pdf`的方式生成调用关系图形。其中，对于CPU profile，有向边的方向表示，调用者指向被调用者；有向边上的数字表示，被调用者所消耗的CPU时间。性能分析通过抽样的方法完成（尽量减少对原程序的运行影响），默认是1秒100个样本，一个样本是10毫秒，即时间单位是10毫秒；可以通过环境变量`CPUPROFILE_FREQUENCY`设置采样频率。

```
# 输出svg
pprof --svg a.out heapprofile.out.0001.heap > heapprofile.svg

# 输出 callgrind
# http://kcachegrind.sourceforge.net/html/Home.html
pprof --callgrind a.out heapprofile.out.0001.heap > heapprofile.callgrind
```

查看`heapprofile.svg`文件：

![gpreftools_heap](/assets/images/202106/gpreftools_heap.png)

使用可视化分析工具`qcachegrind`打开`heapprofile.callgrind`查看：

![qcachegrind_heap_prof](/assets/images/202106/qcachegrind_heap_prof.png)

# HEAP CHECKER

See [heap_checker](https://gperftools.github.io/gperftools/heap_checker.html) for information about how to detect memory leaks in C++ programs.

* Linking in the Library

Link your executable with `-ltcmalloc` or add in the profiler at run-time using `env LD_PRELOAD="/usr/lib/libtcmalloc.so"`

```
This does not turn on heap checking; it just inserts the code. For that reason, it's practical to just always link -ltcmalloc into a binary while developing; that's what we do at Google. (However, since any user can turn on the profiler by setting an environment variable, it's not necessarily recommended to install heapchecker-linked binaries into a production, running system.) 
```

* Running the Code

> Whole-program Heap Leak Checking

The recommended way to use the heap checker is in "whole program" mode. In this case, the heap-checker starts tracking memory allocations before the start of main(), and checks again at program-exit. If it finds any memory leaks -- that is, any memory not pointed to by objects that are still "live" at program-exit -- it aborts the program (via exit(1)) and prints a message describing how to track down the memory leak (using pprof).

The heap-checker records the stack trace for each allocation while it is active. This causes a significant increase in memory usage, in addition to slowing your program down.

Here's how to run a program with whole-program heap checking:

Define the environment variable `HEAPCHECK` to the [type of heap-checking](https://gperftools.github.io/gperftools/heap_checker.html#types) to do. For instance, to heap-check /usr/local/bin/my_binary_compiled_with_tcmalloc:

```
env HEAPCHECK=normal /usr/local/bin/my_binary_compiled_with_tcmalloc
```

Note that since the heap-checker uses the heap-profiling framework internally, it is not possible to run both the `heap-checker` and [heap profiler](https://gperftools.github.io/gperftools/heapprofile.html) at the same time.

These are the legal values when running a whole-program heap check:

1. minimal
2. normal
3. strict
4. draconian

"Normal" mode, as the name implies, is the one used most often at Google. It's appropriate for everyday heap-checking use.

In addition, there are two other possible modes:

* as-is
* local

`as-is` is the most flexible mode; it allows you to specify the various [knobs](https://gperftools.github.io/gperftools/heap_checker.html#options) of the heap checker explicitly. 

`local` activates the [explicit heap-check instrumentation](https://gperftools.github.io/gperftools/heap_checker.html#explicit), but does not turn on any whole-program leak checking.

> Explicit (Partial-program) Heap Leak Checking

Instead of whole-program checking, you can check certain parts of your code to verify they do not have memory leaks. This check verifies that between two parts of a program, no memory is allocated without being freed.

To use this kind of checking code, bracket the code you want checked by creating a `HeapLeakChecker` object at the beginning of the code segment, and call `NoLeaks()` at the end. These functions, and all others referred to in this file, are declared in `<gperftools/heap-checker.h>`.

``` cpp
HeapLeakChecker heap_checker("test_foo");
{
code that exercises some foo functionality;
this code should not leak memory;
}
if (!heap_checker.NoLeaks()) assert(NULL == "heap memory leak");
```

env HEAPCHECK=local /usr/local/bin/my_binary_compiled_with_tcmalloc

If you want to do whole-program leak checking in addition to this manual leak checking, you can run in normal or some other mode instead: they'll run the "local" checks in addition to the whole-program check.

> Disabling Heap-checking of Known Leaks

Sometimes your code has leaks that you know about and are willing to accept. You would like the heap checker to ignore them when checking your program. You can do this by bracketing the code in question with an appropriate heap-checking construct:

``` cpp
{
    HeapLeakChecker::Disabler disabler;
    <leaky code>
}
```

> Tuning the Heap Checker

The heap leak checker has many options, some that trade off running time and accuracy, and others that increase the sensitivity at the risk of returning false positives. For most uses, the range covered by the [heap-check flavors](https://gperftools.github.io/gperftools/heap_checker.html#types) is enough, but in specialized cases more control can be helpful.

测试代码：

``` cpp
#include <iostream>
using namespace std;

void f1() {
        int i;
        for (i = 0; i < 1024* 1024; ++i) {
                short* p = new short;
                delete p;
        }
}

void f2()  { 
        int i;
        for (i = 0; i < 1024 * 1024; ++i) {
                int* p = new int;
                //delete p;   // memory leak!
        }
}

int main() { 
        f1();
        f2();
        return 0;
}
```

编译及内存泄漏分析：

```
$ g++ c.cc -L$HOME/tools/gperftools-2.9.1_install/lib -ltcmalloc -lprofiler
$ $ env HEAPCHECK=normal ./a.out 
WARNING: Perftools heap leak checker is active -- Performance may suffer
Have memory regions w/o callers: might report false leaks
Leak check _main_ detected leaks of 4194304 bytes in 1048576 objects
The 1 largest leaks:
*** WARNING: Cannot convert addresses to symbols in output below.
*** Reason: Cannot find 'pprof' (is PPROF_PATH set correctly?)
*** If you cannot fix this, try running pprof directly.
Leak of 4194304 bytes in 1048576 objects allocated from:
        @ 4011c8 
        @ 4011e3 
        @ 7f876b3926a3 
        @ 4010ae 


If the preceding stack traces are not enough to find the leaks, try running THIS shell command:

pprof ./a.out "/tmp/a.out.154004._main_-end.heap" --inuse_objects --lines --heapcheck  --edgefraction=1e-10 --nodefraction=1e-10 --gv

If you are still puzzled about why the leaks are there, try rerunning this program with HEAP_CHECK_TEST_POINTER_ALIGNMENT=1 and/or with HEAP_CHECK_MAX_POINTER_OFFSET=-1
If the leak report occurs in a small fraction of runs, try running with TCMALLOC_MAX_FREE_QUEUE_SIZE of few hundred MB or with TCMALLOC_RECLAIM_MEMORY=false, it might help find leaks more repeata
Exiting with error code (instead of crashing) because of whole-program memory leaks
```

也可以生成svg格式输出，可以看到有`4MB`的内存泄漏：

```
$ pprof ./a.out "/tmp/a.out.154004._main_-end.heap" --inuse_objects --lines --heapcheck  --edgefraction=1e-10 --nodefraction=1e-10
Using local file ./a.out.
Using local file /tmp/a.out.154004._main_-end.heap.
$ pprof --svg a.out /tmp/a.out.154004._main_-end.heap > a.heap_check3.svg
Using local file a.out.
Using local file /tmp/a.out.154004._main_-end.heap.
Dropping nodes with <= 0.0 MB; edges with <= 0.0 abs(MB)
```

![gperftools_heap_check](/assets/images/202106/gperftools_heap_check.png)

如果项目不可以链接`tcmalloc`，也可以通过`LD_PRELOAD`的方式执行：

```
$ g++ c.cc -L$HOME/tools/gperftools-2.9.1_install/lib
$ LD_PRELOAD="/data/home/gerryyang/tools/gperftools-2.9.1_install/lib/libtcmalloc.so" HEAPCHECK=normal ./a.out
```

# CPU PROFILER

See [docs/cpuprofile.html](https://gperftools.github.io/gperftools/cpuprofile.html) for information about how to use the CPU profiler and analyze its output.

1. Link your executable with `-lprofiler`
2. Run your executable with the `CPUPROFILE` environment var set: `CPUPROFILE=/tmp/prof.out <path/to/binary> [binary args]`
3. Run pprof to analyze the CPU usage

```
$ pprof --text <path/to/binary> prof.out
```

测试代码：

``` cpp
#include <iostream>
using namespace std;
void func1() {
        int i = 0;
        while (i < 100000) {
                ++i;
        }
}
void func2() {
        int i = 0;
        while (i < 200000) {
                ++i;
        }
}
void func3() {
        for (int i = 0; i < 1000; ++i) {
                func1();
                func2();
        }
}
int main() {
        func3();
        return 0;
}
```

编译及CPU使用分析：

```
$ g++ a.cc -L$HOME/tools/gperftools-2.9.1_install/lib -ltcmalloc -lprofiler
$ CPUPROFILE=cpuprofile.out ./a.out 
PROFILE: interrupts/evictions/bytes = 65/1/344
$ pprof --text a.out cpuprofile.out 
Using local file a.out.
Using local file cpuprofile.out.
Total: 65 samples
      43  66.2%  66.2%       43  66.2% func2
      22  33.8% 100.0%       22  33.8% func1
       0   0.0% 100.0%       65 100.0% __libc_start_main
       0   0.0% 100.0%       65 100.0% _start
       0   0.0% 100.0%       65 100.0% func3
       0   0.0% 100.0%       65 100.0% main
```

其他输出方式：

```
# 输出pdf
pprof --pdf a.out cpuprofile.out > cpuprofile.pdf

# 输出svg
pprof --svg a.out cpuprofile.out > cpuprofile.svg
```

![gpreftools_cpu](/assets/images/202106/gpreftools_cpu.png)


# Q&A

## EVERYTHING IN ONE (同时支持CPU和Heap分析)

```
If you want the CPU profiler, heap profiler, and heap leak-checker to
all be available for your application, you can do:
   gcc -o myapp ... -lprofiler -ltcmalloc

However, if you have a reason to use the static versions of the
library, this two-library linking won't work:
   gcc -o myapp ... /usr/lib/libprofiler.a /usr/lib/libtcmalloc.a  # errors!

Instead, use the special libtcmalloc_and_profiler library, which we
make for just this purpose:
   gcc -o myapp ... /usr/lib/libtcmalloc_and_profiler.a
```

## ENVIRONMENT VARIABLES (环境变量)

```
The cpu profiler, heap checker, and heap profiler will lie dormant,
using no memory or CPU, until you turn them on.  (Thus, there's no
harm in linking -lprofiler into every application, and also -ltcmalloc
assuming you're ok using the non-libc malloc library.)

The easiest way to turn them on is by setting the appropriate
environment variables.  We have several variables that let you
enable/disable features as well as tweak parameters.

Here are some of the most important variables:

HEAPPROFILE=<pre> -- turns on heap profiling and dumps data using this prefix
HEAPCHECK=<type>  -- turns on heap checking with strictness 'type'
CPUPROFILE=<file> -- turns on cpu profiling and dumps data to this file.
PROFILESELECTED=1 -- if set, cpu-profiler will only profile regions of code
                     surrounded with ProfilerEnable()/ProfilerDisable().
CPUPROFILE_FREQUENCY=x-- how many interrupts/second the cpu-profiler samples.

PERFTOOLS_VERBOSE=<level> -- the higher level, the more messages malloc emits
MALLOCSTATS=<level>    -- prints memory-use stats at program-exit

For a full list of variables, see the documentation pages:
   docs/cpuprofile.html
   docs/heapprofile.html
   docs/heap_checker.html
```

## PERFORMANCE

```
If you're interested in some third-party comparisons of tcmalloc to
other malloc libraries, here are a few web pages that have been
brought to our attention.  The first discusses the effect of using
various malloc libraries on OpenLDAP.  The second compares tcmalloc to
win32's malloc.
  http://www.highlandsun.com/hyc/malloc/
  http://gaiacrtn.free.fr/articles/win32perftools.html

It's possible to build tcmalloc in a way that trades off faster
performance (particularly for deletes) at the cost of more memory
fragmentation (that is, more unusable memory on your system).  See the
INSTALL file for details.
```

## PROFILE: interrupts/evictions/bytes = 0/0/64 (没有输出profile文件)

> issue 1

According to the [gperftools documentation](https://gperftools.github.io/gperftools/cpuprofile.html), the profiler can be started using any of the following methods:

1. Setting the `CPUPROFILE` environment variable to the filename the profile information will be saved to

2. Doing the above, and also setting `CPUPROFILESIGNAL` and sending the appropriate signal to start or stop the sampling.

3. Calling `ProfilerStart(filename)` and `ProfileStop()` directly from your code

All three methods require that `libprofiler.so` be linked as well.

When I tried this, the third method worked, but when I merely set `CPUPROFILE`, no profiling information was generated.

> issue 2

My program runs for about 2 minutes, still get PROFILE: interrupts/evictions/bytes = 0/0/64...
But after add strace to monitor syscall, weirdly it begin to sample PROFILE: interrupts/evictions/bytes = 150/3/9240
Have no idea why...

Found that if I run program in foreground, gperf samples correctly. But if I run it in daemon mode, gperf fails to sample...

Well, I found this [cpuprofiler only profiler parent process (not fork child) #463](https://github.com/gperftools/gperftools/issues/463)

The workaround in that issue solved my problem.

> 解决方法

``` cpp
// CPU profile
#include "gperftools/profiler.h"

ProfilerStart("cpu.prof");
// ...
ProfilerFlush();
ProfilerStop();


// Heap profile
#include "gperftools/heap-profiler.h"

HeapProfilerStart("heap.prof");
// ...
HeapProfilerDump("over")
HeapProfilerStop();
```

* [I've set the CPUPROFILE environment variable and linked -lprofiler. Why is gperftools not starting the profiler?](https://stackoverflow.com/questions/33662765/ive-set-the-cpuprofile-environment-variable-and-linked-lprofiler-why-is-gperf)
* [PROFILE: interrupts/evictions/bytes = 0/0/64 #1012](https://github.com/gperftools/gperftools/issues/1012)
* [cpuprofiler only profiler parent process (not fork child) #463](https://github.com/gperftools/gperftools/issues/463)

## 64-BIT LINUX Issue (死锁问题)

```
NOTE FOR 64-BIT LINUX SYSTEMS

The glibc built-in stack-unwinder on 64-bit systems has some problems
with the perftools libraries.  (In particular, the cpu/heap profiler
may be in the middle of malloc, holding some malloc-related locks when
they invoke the stack unwinder.  The built-in stack unwinder may call
malloc recursively, which may require the thread to acquire a lock it
already holds: deadlock.)
```

可能的解决方法：

```
If you encounter problems, try compiling perftools with './configure
--enable-frame-pointers'.  Note you will need to compile your
application with frame pointers (via 'gcc -fno-omit-frame-pointer
...') in this case.
```

## heapcheck not work correctly

```
Two possibilities here.  One is that normal mode doesn't consider leaks in main to be
a leak -- I forget the exact rules here.  You can test it by running HEAPCHECK=strict
and seeing what happens.

The other possibility is that the heap-checker is just missing this leak.  As the output
says:
} No leaks found for check "_main_" (but no 100% guarantee that there aren't any):

In general, allocations that happen very close to program exit are most likely for
the heap-checker to miss.  (Luckily, they're also the least dangerous.)

Both possibilities could be in play. :-)

Either way, I believe this is expected behavior.
```

[heapcheck not work correctly. Is it a "live memory" problem? #384](https://github.com/gperftools/gperftools/issues/384)

# Refer

* https://github.com/gperftools/gperftools
* [C++Profile的大杀器_gperftools的使用](https://xusenqi.github.io/2020/12/06/C++Profile%E7%9A%84%E5%A4%A7%E6%9D%80%E5%99%A8_gperftools%E7%9A%84%E4%BD%BF%E7%94%A8/)
* [用gperftools对C/C++程序进行profile](https://airekans.github.io/cpp/2014/07/04/gperftools-profile)
* [Work with Google performance tools](http://alexott.net/en/writings/prog-checking/GooglePT.html)  

	
	