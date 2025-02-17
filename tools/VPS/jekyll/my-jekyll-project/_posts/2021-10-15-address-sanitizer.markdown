---
layout: post
title:  "AddressSanitizer (ASan) - A Fast Memory Error Detector"
date:   2021-10-15 08:30:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}

# TL;DR

`AddressSanitizer` is a fast memory error detector. It consists of a compiler instrumentation module and a run-time library. The tool can detect the following types of bugs:

1. Out-of-bounds accesses to heap, stack and globals
2. Use-after-free
3. Use-after-return (clang flag `-fsanitize-address-use-after-return=(never|runtime|always)` default: `runtime`)
   1. Enable with: `ASAN_OPTIONS=detect_stack_use_after_return=1` (already enabled on Linux).
   2. Disable with: `ASAN_OPTIONS=detect_stack_use_after_return=0`.
4. Use-after-scope (clang flag `-fsanitize-address-use-after-scope`)
5. Double-free, invalid free
6. Memory leaks (experimental)

Typical slowdown introduced by AddressSanitizer is `2x`.

Simply **compile** and **link** your program with `-fsanitize=address` flag. The AddressSanitizer run-time library should be linked to the final executable, so make sure to use `clang` (not `ld`) for the final link step. When linking shared libraries, the AddressSanitizer run-time is not linked, so `-Wl,-z,defs` may cause link errors (don’t use it with AddressSanitizer). To get a reasonable performance add `-O1` or higher. To get nicer stack traces in error messages add `-fno-omit-frame-pointer`. To get perfect stack traces you may need to disable inlining (just use `-O1`) and tail call elimination (`-fno-optimize-sibling-calls`).

``` cpp
// cat example_UseAfterFree.cc
int main(int argc, char **argv) {
  int *array = new int[100];
  delete [] array;
  return array[argc];  // BOOM
}

// Compile and link
// clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer example_UseAfterFree.cc
```

or:

``` bash
# Compile
% clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer -c example_UseAfterFree.cc

# Link
% clang++ -g -fsanitize=address example_UseAfterFree.o
```

If a bug is detected, the program will print an error message to **stderr** and exit with a non-zero exit code. **AddressSanitizer exits on the first detected error. This is by design**:

* This approach allows AddressSanitizer to produce faster and smaller generated code (both by ~5%).

* **Fixing bugs becomes unavoidable**. **AddressSanitizer does not produce false alarms**. Once a memory corruption occurs, the program is in an inconsistent state, which could lead to confusing results and potentially misleading subsequent reports.

编译完成后执行上面示例代码的程序，ASan 会检测出内存问题并在终端输出如下信息：

![asan-check](/assets/images/202405/asan-check.png)

ASan 的输出报告了一个 `heap-use-after-free` 错误，即在释放堆内存后继续使用它。下面是 ASan 输出的详细解释：

* 错误类型：heap-use-after-free，表示在释放堆内存后继续使用它。
* 错误发生的地址：0x614000000044
* 错误发生时执行的操作：READ of size 4，表示在错误地址上进行了一个 4 字节的读操作。
* 错误发生的线程：thread T0，表示错误发生在主线程中。
* 错误发生的位置：在源代码文件 `test.cc` 的第 6 行（`return array[argc];`）。
* 调用栈信息：ASan 提供了错误发生时的调用栈信息，包括 main 函数、C 库函数 __libc_start_main 和程序入口点 _start。
* 内存区域信息：0x614000000044 位于一个 400 字节的内存区域 `[0x614000000040,0x6140000001d0)` 内，距离区域起始地址有 4 字节。
* 内存释放信息：这个内存区域是由主线程释放的，在 `test.cc` 的第 5 行（`delete[] array;`）。
* 内存分配信息：这个内存区域是由主线程分配的，在 `test.cc` 的第 4 行（`int *array = new int[100];`）。
* `Shadow bytes` 信息：ASan 使用 shadow memory（影子内存）来记录内存状态。输出中的 `Shadow bytes` 提供了错误地址周围的内存状态信息。例如，`fa` 表示 heap left redzone（堆左保护区），`fd` 表示 freed heap region（已释放的堆区域）。

综上，ASan 的输出表明，在 `test.cc` 的第 6 行，程序试图访问一个已经释放的堆内存区域，导致 heap-use-after-free 错误。为了解决这个问题，需要确保在释放内存后不再使用它。

AddressSanitizer (ASan) 使用一种称为 "Shadow Memory" 的技术来追踪程序的内存访问。

* Shadow Memory 是原始内存的一个映射，它用来存储关于原始内存的额外信息。每个 Shadow 字节表示 8 个应用程序字节的状态。这意味着 Shadow Memory 的大小是原始内存大小的 1/8。
* 这个 Shadow Memory 字节用来存储原始字节的状态，例如它是否是可寻址的、是否已经被释放等。
* ASan 使用 Shadow Memory 来检测各种内存错误。当程序访问一个内存字节时，ASan 会首先查看该字节的 Shadow Memory 字节，以确定这个访问是否合法。如果 Shadow Memory 字节表示这个访问是非法的（例如，访问了已经被释放的内存），ASan 就会报告一个错误。Shadow Memory 是 ASan 能够高效地检测内存错误的关键。它使得 ASan 能够在每个内存访问时快速地检查内存状态，而无需进行昂贵的查找或者遍历操作。

在报告末尾，有一个图例来解释每个 Shadow 字节值的含义：

* Addressable (00): 表示对应的应用程序内存是可访问的。
* Partially addressable (01-07): 表示对应的应用程序内存部分可访问，值表示可访问字节数。
* Heap left redzone (fa): 表示堆分配的内存区域的左侧保护区，用于检测堆内存下溢。
* Freed heap region (fd): 表示已释放的堆内存区域。
* Stack left redzone (f1): 表示栈内存的左侧保护区，用于检测栈内存下溢。
* Stack mid redzone (f2): 表示栈内存的中间保护区，用于检测大的栈内存溢出。
* Stack right redzone (f3): 表示栈内存的右侧保护区，用于检测栈内存上溢。
* Stack after return (f5): 表示函数返回后的栈内存区域，用于检测返回后使用的错误。
* Stack use after scope (f8): 表示作用域结束后的栈内存区域，用于检测作用域后使用的错误。
* Global redzone (f9): 表示全局变量的保护区，用于检测全局变量的内存错误。
* Global init order (f6): 表示全局变量的初始化顺序。
* Poisoned by user (f7): 表示用户显式标记为不可访问的内存区域。
* Container overflow (fc): 表示容器（如 C++ 的 std::vector）的溢出。
* Array cookie (ac): 表示数组的元数据（如元素数量）。
* Intra object redzone (bb): 表示对象内部的保护区，用于检测对象内部的内存错误。
* ASan internal (fe): 表示ASan内部使用的内存。
* Left alloca redzone (ca): 表示栈上分配的内存的左侧保护区。
* Right alloca redzone (cb): 表示栈上分配的内存的右侧保护区。
* Shadow gap (cc): 表示shadow memory和应用程序内存之间的间隙。




# Introduction

[AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) (aka `ASan`) is a memory error detector for C/C++. It finds:

+ [Use after free (dangling pointer dereference)](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleUseAfterFree) (dangling pointer dereference)
+ [Heap buffer overflow](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleHeapOutOfBounds)
+ [Stack buffer overflow](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleStackOutOfBounds)
+ [Global buffer overflow](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleGlobalOutOfBounds)
+ [Use after return](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleUseAfterReturn)
+ [Use after scope](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleUseAfterScope)
+ [Initialization order bugs](https://github.com/google/sanitizers/wiki//AddressSanitizerInitializationOrderFiasco)
+ [Memory leaks](https://github.com/google/sanitizers/wiki//AddressSanitizerLeakSanitizer)

This tool is very fast. The average slowdown of the instrumented program is `~2x` (see [AddressSanitizerPerformanceNumbers](https://github.com/google/sanitizers/wiki/AddressSanitizerPerformanceNumbers)).

The tool consists of a compiler instrumentation module (currently, an `LLVM` pass) and a run-time library which replaces the `malloc` function.

See also:

* [AddressSanitizerAlgorithm](https://github.com/google/sanitizers/wiki/AddressSanitizerAlgorithm) -- if you are curious how it works.
* [AddressSanitizerComparisonOfMemoryTools](https://github.com/google/sanitizers/wiki/AddressSanitizerComparisonOfMemoryTools)

## Getting AddressSanitizer

[AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) is a part of [LLVM](http://llvm.org/) starting with version **3.1** and a part of [GCC](http://gcc.gnu.org/) starting with version **4.8** If you prefer to build from source, see [AddressSanitizerHowToBuild](https://github.com/google/sanitizers/wiki/AddressSanitizerHowToBuild).


## Using AddressSanitizer

In order to use `AddressSanitizer` you will need to compile and link your program using `clang` with the `-fsanitize=address` switch. To get a reasonable performance add `-O1` or higher. To get nicer stack traces in error messages add `-fno-omit-frame-pointer`. Note: [Clang 3.1 release uses another flag syntax](http://llvm.org/releases/3.1/tools/clang/docs/AddressSanitizer.html).


``` cpp
% cat tests/use-after-free.c
#include <stdlib.h>
int main() {
  char *x = (char*)malloc(10 * sizeof(char*));
  free(x);
  return x[5];
}
% ../clang_build_Linux/Release+Asserts/bin/clang -fsanitize=address -O1 -fno-omit-frame-pointer -g   tests/use-after-free.c
```

Now, run the executable. [AddressSanitizerCallStack](https://github.com/google/sanitizers/wiki/AddressSanitizerCallStack) page describes how to obtain symbolized stack traces.

```
% ./a.out
==9901==ERROR: AddressSanitizer: heap-use-after-free on address 0x60700000dfb5 at pc 0x45917b bp 0x7fff4490c700 sp 0x7fff4490c6f8
READ of size 1 at 0x60700000dfb5 thread T0
    #0 0x45917a in main use-after-free.c:5
    #1 0x7fce9f25e76c in __libc_start_main /build/buildd/eglibc-2.15/csu/libc-start.c:226
    #2 0x459074 in _start (a.out+0x459074)
0x60700000dfb5 is located 5 bytes inside of 80-byte region [0x60700000dfb0,0x60700000e000)
freed by thread T0 here:
    #0 0x4441ee in __interceptor_free projects/compiler-rt/lib/asan/asan_malloc_linux.cc:64
    #1 0x45914a in main use-after-free.c:4
    #2 0x7fce9f25e76c in __libc_start_main /build/buildd/eglibc-2.15/csu/libc-start.c:226
previously allocated by thread T0 here:
    #0 0x44436e in __interceptor_malloc projects/compiler-rt/lib/asan/asan_malloc_linux.cc:74
    #1 0x45913f in main use-after-free.c:3
    #2 0x7fce9f25e76c in __libc_start_main /build/buildd/eglibc-2.15/csu/libc-start.c:226
SUMMARY: AddressSanitizer: heap-use-after-free use-after-free.c:5 main
```

If a bug is detected, the program will print an error message to `stderr` and exit with a non-zero exit code. AddressSanitizer exits on the first detected error. This is by design:

* This approach allows AddressSanitizer to produce faster and smaller generated code (both by ~5%).
* Fixing bugs becomes unavoidable. AddressSanitizer does not produce false alarms. Once a memory corruption occurs, the program is in an inconsistent state, which could lead to confusing results and potentially misleading subsequent reports.


## Symbolizing the Reports

To make AddressSanitizer symbolize its output you need to set the `ASAN_SYMBOLIZER_PATH` environment variable to point to the `llvm-symbolizer` binary (or make sure `llvm-symbolizer` is in your `$PATH`):

```
% ASAN_SYMBOLIZER_PATH=/usr/local/bin/llvm-symbolizer ./a.out
==9442== ERROR: AddressSanitizer heap-use-after-free on address 0x7f7ddab8c084 at pc 0x403c8c bp 0x7fff87fb82d0 sp 0x7fff87fb82c8
READ of size 4 at 0x7f7ddab8c084 thread T0
    #0 0x403c8c in main example_UseAfterFree.cc:4
    #1 0x7f7ddabcac4d in __libc_start_main ??:0
0x7f7ddab8c084 is located 4 bytes inside of 400-byte region [0x7f7ddab8c080,0x7f7ddab8c210)
freed by thread T0 here:
    #0 0x404704 in operator delete[](void*) ??:0
    #1 0x403c53 in main example_UseAfterFree.cc:4
    #2 0x7f7ddabcac4d in __libc_start_main ??:0
previously allocated by thread T0 here:
    #0 0x404544 in operator new[](unsigned long) ??:0
    #1 0x403c43 in main example_UseAfterFree.cc:2
    #2 0x7f7ddabcac4d in __libc_start_main ??:0
==9442== ABORTING
```


参考：[Clang 18.0.0 - AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)


## Interaction with other tools

### gdb

See [AddressSanitizerAndDebugger](https://github.com/google/sanitizers/wiki/AddressSanitizerAndDebugger)

You can use `gdb` with binaries built by `AddressSanitizer` in a usual way. When `AddressSanitizer` finds a bug it calls one of the functions `__asan_report_{load,store}{1,2,4,8,16}` which in turn calls `__asan::ReportGenericError`.

If you want gdb to stop before asan reports an error, set a breakpoint on `__asan::ReportGenericError`.

If you want gdb to stop after asan has reported an error, set a breakpoint on `__sanitizer::Die` or use `ASAN_OPTIONS=abort_on_error=1`.

Inside gdb you can ask asan to describe a memory location:

```
(gdb) set overload-resolution off
(gdb) p __asan_describe_address(0x7ffff73c3f80)
0x7ffff73c3f80 is located 0 bytes inside of 10-byte region [0x7ffff73c3f80,0x7ffff73c3f8a)
freed by thread T0 here:
...
```

## AddressSanitizerFlags

See the separate [AddressSanitizerFlags](https://github.com/google/sanitizers/wiki/AddressSanitizerFlags) page.

**Compiler flags:**

| flag | description
| -- | --
| -fsanitize=address | Enable [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer)
| -fno-omit-frame-pointer | Leave frame pointers. Allows the fast unwinder to function properly.
| -fsanitize-blacklist=path | Pass a [blacklist file](https://github.com/google/sanitizers/wiki/AddressSanitizer#turning-off-instrumentation)
| -fno-common | Do not treat global variable in C as common variables (allows ASan to instrument them)

ASan-specific compile-time flags are passed via clang flag `-mllvm <flag>`. In most cases you don't need them.

| flag | default | description
| -- | -- | --
| -asan-stack | 1 | Detect overflow/underflow for stack objects
| -asan-globals | 1 | Detect overflow/underflow for global objects
| -asan-use-private-alias	 | 0 | Use private aliases for global objects

**Run-time flags:**

Most run-time flags are passed to `AddressSanitizer` via `ASAN_OPTIONS` environment variable like this:

```
ASAN_OPTIONS=verbosity=1:malloc_context_size=20 ./a.out
```

but you could also embed default flags in the source code by implementing `__asan_default_options` function:

``` cpp
const char *__asan_default_options() {
  return "verbosity=1:malloc_context_size=20";
}
```

Note that the list below list may be (and probably is) **incomplete**. Also older versions of ASan may not support some of the listed flags. To get the idea of what's supported in your version, run

```
ASAN_OPTIONS=help=1 ./a.out
```

| Flag | Default value | Description
| -- | -- | --
| redzone | 16 | Minimal size (in bytes) of redzones around heap objects. Requirement: redzone >= 16, is a power of two.
| max_redzone | 2048 | Maximal size (in bytes) of redzones around heap objects.
| sleep_before_dying | 0 | Number of seconds to sleep between printing an error report and terminating the program. Useful for debugging purposes (e.g. when one needs to attach gdb).
| print_stats | false | Print various statistics after printing an error message or if atexit=1.
| atexit | false | If set, prints ASan exit stats even after program terminates successfully.
| halt_on_error | true | Crash the program after printing the first error report (WARNING: USE AT YOUR OWN RISK!). The flag has effect only if code was compiled with -fsanitize-recover=address compile option.
| log_path | stderr | Write logs to log_path.pid. The special values are stdout and stderr


## [SanitizerCommonFlags](https://github.com/google/sanitizers/wiki/SanitizerCommonFlags)

Each tool parses the common options from the corresponding environment variable (`ASAN_OPTIONS`, `TSAN_OPTIONS`, `MSAN_OPTIONS`, `LSAN_OPTIONS`) together with the tool-specific options.

**Run-time flags:**

| Flag | Default value | Description
| -- | -- | --
| symbolize | true | If set, use the online symbolizer from common sanitizer runtime to turn virtual addresses to file/line locations.
| malloc_context_size | 30 | Max number of stack frames kept for each allocation/deallocation.
| log_path | stderr | Write logs to "log_path.pid". The special values are "stdout" and "stderr". The default is "stderr".
| log_exe_name | false | Mention name of executable when reporting error and append executable name to logs (as in "log_path.exe_name.pid").
| verbosity | 0 | Verbosity level (0 - silent, 1 - a bit of output, 2+ - more output).
| detect_leaks | true | Enable memory leak detection.
| leak_check_at_exit | true | Invoke leak checking in an atexit handler. Has no effect if detect_leaks=false, or if __lsan_do_leak_check() is called before the handler has a chance to run.
| allocator_may_return_null | false | If false, the allocator will crash instead of returning 0 on out-of-memory.
| print_summary | true | If false, disable printing error summaries in addition to error reports.
| handle_abort | false | Controls custom tool's SIGABRT handler (0 - do not registers the handler, 1 - register the handler and allow user to set own, 2 - registers the handler and block user from changing it). Ignored on Windows.
| help | false | Print the flag ptions.
| coverage | false | If set, coverage information will be dumped at program shutdown (if the coverage instrumentation was enabled at compile time).
| disable_coredump | true (false on non-64-bit systems) | Disable core dumping. By default, disable_coredump=1 on 64-bit to avoid dumping a 16T+ core file. Ignored on OSes that don't dump core by default and for sanitizers that don't reserve lots of virtual memory.
| exitcode | 1 | Override the program exit status if the tool found an error
| abort_on_error | false (true on Darwin) | If set, the tool calls abort() instead of _exit() after printing the error report.



## Call stack

See the separate [AddressSanitizerCallStack](https://github.com/google/sanitizers/wiki/AddressSanitizerCallStack) page.

`AddressSanitizer` collects call stacks on the following events:

* `malloc` and `free`
* thread creation
* failure


## Incompatibility

Sometimes an `AddressSanitizer` build may behave differently than the regular one. See [AddressSanitizerIncompatiblity](https://github.com/google/sanitizers/wiki/AddressSanitizerIncompatiblity) for details.


### malloc

AddressSanitizer uses its own memory allocator (`malloc`, `free`, etc). If your code depends on a particular feature or extension of glibc malloc, tcmalloc or some other malloc, it may not work as you expect.

### Virtual memory

`AddressSanitizer` uses a lot of virtual address space (`20T` on x86_64 Linux)


## Turning off instrumentation

In some cases a particular function should be ignored (not instrumented) by `AddressSanitizer`:

* Ignore a very hot function known to be correct to speedup the app.
* Ignore a function that does some low-level magic (e.g. walking through the thread's stack bypassing the frame boundaries).
* Don't report a known problem. In either case, be very careful.

To ignore certain functions, one can use the `no_sanitize_address` attribute supported by Clang (3.3+) and GCC (4.8+). You can define the following macro:

``` cpp
#if defined(__clang__) || defined (__GNUC__)
# define ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
#else
# define ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif
...
ATTRIBUTE_NO_SANITIZE_ADDRESS
void ThisFunctionWillNotBeInstrumented() {...}
```

Clang 3.1 and 3.2 supported `__attribute__((no_address_safety_analysis))` instead.

You may also ignore certain functions using a blacklist: create a file `my_ignores.txt` and pass it to `AddressSanitizer` at compile time using `-fsanitize-blacklist=my_ignores.txt` (This flag is new and is only supported by Clang now):

```
# Ignore exactly this function (the names are mangled)
fun:MyFooBar
# Ignore MyFooBar(void) if it is in C++:
fun:_Z8MyFooBarv
# Ignore all function containing MyFooBar
fun:*MyFooBar*
```

## Talks and papers

* Watch the presentation from the [LLVM Developer's meeting](http://llvm.org/devmtg/2011-11/) (Nov 18, 2011): [Video](http://www.youtube.com/watch?v=CPnRS1nv3_s), [slides](http://llvm.org/devmtg/2011-11/Serebryany_FindingRacesMemoryErrors.pdf).
* Read the [USENIX ATC '2012 paper](http://research.google.com/pubs/pub37752.html).




## Additional Checks

### Initialization order checking

AddressSanitizer can optionally detect dynamic initialization order problems, when initialization of globals defined in one translation unit uses globals defined in another translation unit. To enable this check at runtime, you should set environment variable `ASAN_OPTIONS=check_initialization_order=1`.

Note that this option is not supported on macOS.


### Stack Use After Return (UAR)

AddressSanitizer can optionally detect stack use after return problems. This is available by default, or explicitly (`-fsanitize-address-use-after-return=runtime`). To enable this check at runtime, set the environment variable `ASAN_OPTIONS=detect_stack_use_after_return=1`.

Enabling this check (`-fsanitize-address-use-after-return=always`) will reduce code size. The code size may be reduced further by completely eliminating this check (`-fsanitize-address-use-after-return=never`).

To summarize: `-fsanitize-address-use-after-return=<mode>`

* `never`: Completely disables detection of UAR errors (reduces code size).
* `runtime`: Adds the code for detection, but must be enabled via the runtime environment (`ASAN_OPTIONS=detect_stack_use_after_return=1`).
* `always`: Enables detection of UAR errors in all cases. (reduces code size, but not as much as never).

### Memory leak detection

For more information on leak detector in AddressSanitizer, see [LeakSanitizer](https://clang.llvm.org/docs/LeakSanitizer.html). The leak detection is turned on by default on Linux, and can be enabled using `ASAN_OPTIONS=detect_leaks=1` on macOS; however, it is not yet supported on other platforms.

## Issue Suppression (抑制错误)

**AddressSanitizer is not expected to produce false positives. If you see one, look again; most likely it is a true positive!**

### Suppressing Reports in External Libraries

Runtime interposition allows AddressSanitizer to find bugs in code that is not being recompiled. If you run into an issue in external libraries, we recommend immediately reporting it to the library maintainer so that it gets addressed. However, you can use the following suppression mechanism to unblock yourself and continue on with the testing.

This suppression mechanism should only be used for suppressing issues in external code; it does not work on code recompiled with AddressSanitizer. To suppress errors in external libraries, set the `ASAN_OPTIONS` environment variable to point to a suppression file. You can either specify the full path to the file or the path of the file relative to the location of your executable.

```
ASAN_OPTIONS=suppressions=MyASan.supp
```

Use the following format to specify the names of the functions or libraries you want to suppress. You can see these in the error report. Remember that the narrower the scope of the suppression, the more bugs you will be able to catch.

```
interceptor_via_fun:NameOfCFunctionToSuppress
interceptor_via_fun:-[ClassName objCMethodToSuppress:]
interceptor_via_lib:NameOfTheLibraryToSuppress
```

例子：

``` bash
-----------------------------------------------------
Suppressions used:
  count      bytes template
     63       2168 instance
     29        928 JLib::CCfgAgent::SetAutoSync
   8984    8932728 Update
     16        440 PrintMsg
    737      45352 GetMsgStat
      8        192 GetMsgMemMgr
      1        104 GetCfg
     16        568 GetLevelStat
      4         64 STThreadData
    291      14184 Allocate
     28        448 JLib::CJLibMonitor::InitMetrics
     52   17034728 tss_sdk_init_with_proc
   3152     136233 tss_sdk_get_busi_interf
      9    2223532 tdr_load_metalib_fp
    349      22336 pbL_DescriptorProto
    118       6777 call_init.part.0
 326637   70005062 start_thread
-----------------------------------------------------
```

ASan（AddressSanitizer）输出的 "Suppressions" 部分提供了有关在程序运行过程中使用的内存泄漏抑制的信息。抑制是一种机制，用于告诉 ASan 忽略特定类型的内存泄漏。这在某些情况下可能是有用的，例如当知道某些内存泄漏是由第三方库引起的，而无法修复它们时。

"Suppressions used" 部分列出了在程序运行过程中应用的抑制。每一行包含以下信息：

* `count`：使用该抑制的次数。
* `bytes`：由该抑制隐藏的总字节数。
* `template`：抑制的名称或模板。
*
在提供的输出中，例如，有 63 个 "instance" 抑制，共隐藏了 2168 字节的内存泄漏。另一个例子是 "Update" 抑制，共有 8984 个，隐藏了 8932728 字节的内存泄漏。

需要注意的是，抑制不应该用于隐藏程序中的实际内存泄漏。它们应该谨慎使用，以避免误导开发人员。在大多数情况下，建议修复导致内存泄漏的代码，而不是使用抑制。



### Conditional Compilation with __has_feature(address_sanitizer)

In some cases one may need to execute different code depending on whether AddressSanitizer is enabled. `__has_feature` can be used for this purpose.

``` cpp
#if defined(__has_feature)
#  if __has_feature(address_sanitizer)
// code that builds only under AddressSanitizer
#  endif
#endif
```

### Disabling Instrumentation with `__attribute__((no_sanitize("address")))`

Some code should not be instrumented by AddressSanitizer. One may use the attribute `__attribute__((no_sanitize("address")))` (which has deprecated synonyms no_sanitize_address and no_address_safety_analysis) to disable instrumentation of a particular function. This attribute may not be supported by other compilers, so we suggest to use it together with `__has_feature(address_sanitizer)`.

The same attribute used on a global variable prevents AddressSanitizer from adding redzones around it and detecting out of bounds accesses.

### Suppressing Errors in Recompiled Code (Ignorelist)

AddressSanitizer supports `src` and `fun` entity types in [Sanitizer special case list](https://clang.llvm.org/docs/SanitizerSpecialCaseList.html), that can be used to suppress error reports in the specified source files or functions. Additionally, AddressSanitizer introduces `global` and `type` entity types that can be used to suppress error reports for out-of-bound access to globals with certain names and types (you may only specify class or struct types).

You may use an `init` category to suppress reports about initialization-order problems happening in certain source files or with certain global variables.

``` bash
# Suppress error reports for code in a file or in a function:
src:bad_file.cpp
# Ignore all functions with names containing MyFooBar:
fun:*MyFooBar*
# Disable out-of-bound checks for global:
global:bad_array
# Disable out-of-bound checks for global instances of a given class ...
type:Namespace::BadClassName
# ... or a given struct. Use wildcard to deal with anonymous namespace.
type:Namespace2::*::BadStructName
# Disable initialization-order checks for globals:
global:bad_init_global=init
type:*BadInitClassSubstring*=init
src:bad/init/files/*=init
```

#### Sanitizer special case list

This document describes the way to disable or alter the behavior of sanitizer tools for certain source-level entities by providing a special file at compile-time.

User of sanitizer tools, such as [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html), [ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html) or [MemorySanitizer](https://clang.llvm.org/docs/MemorySanitizer.html) may want to disable or alter some checks for certain source-level entities to:

* speedup hot function, which is known to be correct;
* ignore a function that does some low-level magic (e.g. walks through the thread stack, bypassing the frame boundaries);
* ignore a known problem.

To achieve this, user may create a file listing the entities they want to ignore, and pass it to clang at compile-time using `-fsanitize-ignorelist` flag. See [Clang Compiler User’s Manual](https://clang.llvm.org/docs/UsersManual.html) for details.

![asan3](/assets/images/202308/asan3.png)

![asan4](/assets/images/202308/asan4.png)

```
$ cat foo.c
#include <stdlib.h>
void bad_foo() {
  int *a = (int*)malloc(40);
  a[10] = 1;
}
int main() { bad_foo(); }
$ cat ignorelist.txt
# Ignore reports from bad_foo function.
fun:bad_foo
$ clang -fsanitize=address foo.c ; ./a.out
# AddressSanitizer prints an error report.
$ clang -fsanitize=address -fsanitize-ignorelist=ignorelist.txt foo.c ; ./a.out
# No error report here.
```

> NOTE: clang11 的选项是 `-fsanitize-blacklist`，可通过 `clang --help | grep sanitize` 查看具体的选项

Format

Ignorelists consist of entries, optionally grouped into sections. Empty lines and lines starting with “#” are ignored.

Section names are regular expressions written in square brackets that denote which sanitizer the following entries apply to. For example, `[address]` specifies AddressSanitizer while `[cfi-vcall|cfi-icall]` specifies Control Flow Integrity virtual and indirect call checking. Entries without a section will be placed under the `[*]` section applying to all enabled sanitizers.

Entries contain an entity type, followed by a colon and a regular expression, specifying the names of the entities, optionally followed by an equals sign and a tool-specific category, e.g. `fun:*ExampleFunc=example_category`. The meaning of `*` in regular expression for entity names is different - it is treated as in shell wildcarding. Two generic entity types are `src` and `fun`, which allow users to specify source files and functions, respectively. Some sanitizer tools may introduce custom entity types and categories - refer to tool-specific docs.


``` bash
# Lines starting with # are ignored.
# Turn off checks for the source file (use absolute path or path relative
# to the current working directory):
src:/path/to/source/file.c
# Turn off checks for this main file, including files included by it.
# Useful when the main file instead of an included file should be ignored.
mainfile:file.c
# Turn off checks for a particular functions (use mangled names):
fun:MyFooBar
fun:_Z8MyFooBarv
# Extended regular expressions are supported:
fun:bad_(foo|bar)
src:bad_source[1-9].c
# Shell like usage of * is supported (* is treated as .*):
src:bad/sources/*
fun:*BadFunction*
# Specific sanitizer tools may introduce categories.
src:/special/path/*=special_sources
# Sections can be used to limit ignorelist entries to specific sanitizers
[address]
fun:*BadASanFunc*
# Section names are regular expressions
[cfi-vcall|cfi-icall]
fun:*BadCfiCall
# Entries without sections are placed into [*] and apply to all sanitizers
```






### Suppressing memory leaks

Memory leak reports produced by [LeakSanitizer](https://clang.llvm.org/docs/LeakSanitizer.html) (if it is run as a part of AddressSanitizer) can be suppressed by a separate file passed as

``` bash
LSAN_OPTIONS=suppressions=MyLSan.supp
```

which contains lines of the form `leak:<pattern>`. Memory leak will be suppressed if pattern matches any function name, source file name, or library name in the symbolized stack trace of the leak report. See [full documentation](https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer#suppressions) for more details.


You can instruct `LeakSanitizer` to ignore certain leaks by passing in **a suppressions file**. The file must contain one suppression rule per line, each rule being of the form `leak:<pattern>`. The pattern will be substring-matched against the symbolized stack trace of the leak. If either function name, source file name or binary file name matches, the leak report will be suppressed.

```
$ cat suppr.txt
# This is a known leak.
leak:FooBar
$ cat lsan-suppressed.cc
#include <stdlib.h>

void FooBar() {
  malloc(7);
}

void Baz() {
  malloc(5);
}

int main() {
  FooBar();
  Baz();
  return 0;
}
$ clang++ lsan-suppressed.cc -fsanitize=address
$ ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=suppr.txt ./a.out

=================================================================
==26475==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 5 byte(s) in 1 object(s) allocated from:
    #0 0x44f2de in malloc /usr/home/hacker/llvm/projects/compiler-rt/lib/asan/asan_malloc_linux.cc:74
    #1 0x464e86 in Baz() (/usr/home/hacker/a.out+0x464e86)
    #2 0x464fb4 in main (/usr/home/hacker/a.out+0x464fb4)
    #3 0x7f7e760b476c in __libc_start_main /build/buildd/eglibc-2.15/csu/libc-start.c:226

-----------------------------------------------------
Suppressions used:[design document](AddressSanitizerLeakSanitizerDesignDocument)
  count      bytes template
      1          7 FooBar
-----------------------------------------------------

SUMMARY: AddressSanitizer: 5 byte(s) leaked in 1 allocation(s).
```

The special symbols `^` and `$` match the beginning and the end of string.


## Code generation control

Instrumentation code outlining

By default AddressSanitizer inlines the instrumentation code to improve the run-time performance, which leads to increased binary size. Using the (clang flag `-fsanitize-address-outline-instrumentation` default: `false`) flag forces all code instrumentation to be outlined, which reduces the size of the generated code, but also reduces the run-time performance.

## Limitations

* AddressSanitizer uses more real memory than a native run. Exact overhead depends on the allocations sizes. The smaller the allocations you make the bigger the overhead is.
* AddressSanitizer uses more stack memory. We have seen up to 3x increase.
* On 64-bit platforms AddressSanitizer maps (but not reserves) 16+ Terabytes of virtual address space. This means that tools like ulimit may not work as usually expected.
* Static linking of executables is not supported.

## Supported Platforms

AddressSanitizer is supported on:

* Linux i386/x86_64 (tested on Ubuntu 12.04)
* macOS 10.7 - 10.11 (i386/x86_64)
* iOS Simulator
* Android ARM
* NetBSD i386/x86_64
* FreeBSD i386/x86_64 (tested on FreeBSD 11-current)
* Windows 8.1+ (i386/x86_64)

Ports to various other platforms are in progress.


## Current Status

AddressSanitizer is fully functional on supported platforms starting from LLVM 3.1. The test suite is integrated into CMake build and can be run with make check-asan command.

The Windows port is functional and is used by Chrome and Firefox, but it is not as well supported as the other ports.


# LeakSanitizer

`LeakSanitizer` is a memory leak detector which is integrated into `AddressSanitizer`. The tool is supported on x86_64 Linux and OS X.

`LeakSanitizer` is enabled **by default** in ASan builds of x86_64 Linux, and can be enabled with `ASAN_OPTIONS=detect_leaks=1` on x86_64 OS X. **`LSan` lies dormant until the very end of the process, at which point there is an extra leak detection phase.** In performance-critical scenarios, `LSan` can also be used without ASan instrumentation.

See also: [design document](https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizerDesignDocument), [comparison with tcmalloc's heap leak checker](https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizerVsHeapChecker)

## Using LeakSanitizer

To use `LSan`, simply build your program with `AddressSanitizer`:

``` cpp
$ cat memory-leak.c
#include <stdlib.h>

void *p;

int main() {
  p = malloc(7);
  p = 0; // The memory is leaked here.
  return 0;
}
```

```
$ clang -fsanitize=address -g memory-leak.c
$ ./a.out

=================================================================
==7829==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 7 byte(s) in 1 object(s) allocated from:
    #0 0x42c0c5 in __interceptor_malloc /usr/home/hacker/llvm/projects/compiler-rt/lib/asan/asan_malloc_linux.cc:74
    #1 0x43ef81 in main /usr/home/hacker/memory-leak.c:6
    #2 0x7fef044b876c in __libc_start_main /build/buildd/eglibc-2.15/csu/libc-start.c:226

SUMMARY: AddressSanitizer: 7 byte(s) leaked in 1 allocation(s).
```

If you want to run an ASan-instrumented program without leak detection, you can pass `detect_leaks=0` in the `ASAN_OPTIONS` environment variable.

### Stand-alone mode

If you just need leak detection, and don't want to bear the ASan slowdown, you can build with `-fsanitize=leak` instead of `-fsanitize=address`. This will link your program against a runtime library containing just the bare necessities required for `LeakSanitizer` to work. **No compile-time instrumentation will be applied**.

Be aware that the stand-alone mode is less well tested compared to running `LSan` on top of `ASan`.

### Flags

You can fine-tune LeakSanitizer's behavior through the `LSAN_OPTIONS` environment variable.

| flag | default | description
| -- | -- | --
| exitcode | 23 | If non-zero, `LSan` will call `_exit(exitcode)` upon detecting leaks. This can be different from the exit code used to signal ASan errors.
| max_leaks | 0 | If non-zero, report only this many top leaks.

### Suppressions

You can instruct `LeakSanitizer` to ignore certain leaks by passing in a suppressions file. The file must contain one suppression rule per line, each rule being of the form `leak:<pattern>`. The pattern will be substring-matched against the symbolized stack trace of the leak. If either function name, source file name or binary file name matches, the leak report will be suppressed.

The special symbols `^` and `$` match the beginning and the end of string.

```
$ cat suppr.txt
# This is a known leak.
leak:FooBar
$ cat lsan-suppressed.cc
#include <stdlib.h>

void FooBar() {
  malloc(7);
}

void Baz() {
  malloc(5);
}

int main() {
  FooBar();
  Baz();
  return 0;
}
$ clang++ lsan-suppressed.cc -fsanitize=address
$ ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=suppr.txt ./a.out

=================================================================
==26475==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 5 byte(s) in 1 object(s) allocated from:
    #0 0x44f2de in malloc /usr/home/hacker/llvm/projects/compiler-rt/lib/asan/asan_malloc_linux.cc:74
    #1 0x464e86 in Baz() (/usr/home/hacker/a.out+0x464e86)
    #2 0x464fb4 in main (/usr/home/hacker/a.out+0x464fb4)
    #3 0x7f7e760b476c in __libc_start_main /build/buildd/eglibc-2.15/csu/libc-start.c:226

-----------------------------------------------------
Suppressions used:[design document](AddressSanitizerLeakSanitizerDesignDocument)
  count      bytes template
      1          7 FooBar
-----------------------------------------------------

SUMMARY: AddressSanitizer: 5 byte(s) leaked in 1 allocation(s).
```


## 测试示例

### stl 容器添加内容

``` cpp
#include <vector>
#include <iostream>

int main(int argc, char **argv)
{
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    for (auto i : vec)
    {
        std::cout << i << std::endl;
    }
}
// ASan 检查不出来
```

### shared_ptr 循环引用

``` cpp
#include <iostream>
#include <memory>

class B;

class A
{
public:
    std::shared_ptr<B> b_ptr;
    ~A()
    {
        std::cout << "A destructor called" << std::endl;
    }
};

class B
{
public:
    std::shared_ptr<A> a_ptr;
    ~B()
    {
        std::cout << "B destructor called" << std::endl;
    }
};

int main()
{
    {
        std::shared_ptr<A> a = std::make_shared<A>();
        std::shared_ptr<B> b = std::make_shared<B>();

        std::cout << "sizeof(A): " << sizeof(a) << std::endl;

        a->b_ptr = b;
        b->a_ptr = a;
    }  // a 和 b 的析构函数在这里应该被调用，但由于循环引用，它们不会被调用

    std::cout << "Main function ends" << std::endl;
    return 0;
}
```

clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer test3.cc -o test3

```
$ ./test3
sizeof(A): 16
Main function ends

=================================================================
==4167602==ERROR: LeakSanitizer: detected memory leaks

Indirect leak of 32 byte(s) in 1 object(s) allocated from:
    #0 0x4f2c18 in operator new(unsigned long) /data/home/gerryyang/tools/clang/llvm-project-11.0.0/compiler-rt/lib/asan/asan_new_delete.cpp:99:3
    #1 0x4f763e in __gnu_cxx::new_allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> >::allocate(unsigned long, void const*) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/ext/new_allocator.h:111:27
    #2 0x4f75ba in std::allocator_traits<std::allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> > >::allocate(std::allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> >&, unsigned long) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/alloc_traits.h:436:20
    #3 0x4f72e9 in std::__allocated_ptr<std::allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> > > std::__allocate_guarded<std::allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> > >(std::allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> >&) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/allocated_ptr.h:97:21
    #4 0x4f7122 in std::__shared_count<(__gnu_cxx::_Lock_policy)2>::__shared_count<B, std::allocator<B> >(B*&, std::_Sp_alloc_shared_tag<std::allocator<B> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr_base.h:675:19
    #5 0x4f7020 in std::__shared_ptr<B, (__gnu_cxx::_Lock_policy)2>::__shared_ptr<std::allocator<B> >(std::_Sp_alloc_shared_tag<std::allocator<B> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr_base.h:1342:14
    #6 0x4f6fe8 in std::shared_ptr<B>::shared_ptr<std::allocator<B> >(std::_Sp_alloc_shared_tag<std::allocator<B> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:359:4
    #7 0x4f6fbd in std::shared_ptr<B> std::allocate_shared<B, std::allocator<B> >(std::allocator<B> const&) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:705:14
    #8 0x4f5ed0 in std::shared_ptr<B> std::make_shared<B>() /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:721:14
    #9 0x4f59f3 in main /data/home/gerryyang/github/mac-utils/programing/cpp/asan/test3.cc:30:32
    #10 0x7f8ac3206f92 in __libc_start_main (/lib64/libc.so.6+0x26f92)

Indirect leak of 32 byte(s) in 1 object(s) allocated from:
    #0 0x4f2c18 in operator new(unsigned long) /data/home/gerryyang/tools/clang/llvm-project-11.0.0/compiler-rt/lib/asan/asan_new_delete.cpp:99:3
    #1 0x4f69fe in __gnu_cxx::new_allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> >::allocate(unsigned long, void const*) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/ext/new_allocator.h:111:27
    #2 0x4f697a in std::allocator_traits<std::allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> > >::allocate(std::allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> >&, unsigned long) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/alloc_traits.h:436:20
    #3 0x4f66a9 in std::__allocated_ptr<std::allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> > > std::__allocate_guarded<std::allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> > >(std::allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> >&) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/allocated_ptr.h:97:21
    #4 0x4f64e2 in std::__shared_count<(__gnu_cxx::_Lock_policy)2>::__shared_count<A, std::allocator<A> >(A*&, std::_Sp_alloc_shared_tag<std::allocator<A> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr_base.h:675:19
    #5 0x4f63e0 in std::__shared_ptr<A, (__gnu_cxx::_Lock_policy)2>::__shared_ptr<std::allocator<A> >(std::_Sp_alloc_shared_tag<std::allocator<A> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr_base.h:1342:14
    #6 0x4f63a8 in std::shared_ptr<A>::shared_ptr<std::allocator<A> >(std::_Sp_alloc_shared_tag<std::allocator<A> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:359:4
    #7 0x4f636d in std::shared_ptr<A> std::allocate_shared<A, std::allocator<A> >(std::allocator<A> const&) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:705:14
    #8 0x4f5d80 in std::shared_ptr<A> std::make_shared<A>() /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:721:14
    #9 0x4f59e1 in main /data/home/gerryyang/github/mac-utils/programing/cpp/asan/test3.cc:29:32
    #10 0x7f8ac3206f92 in __libc_start_main (/lib64/libc.so.6+0x26f92)

SUMMARY: AddressSanitizer: 64 byte(s) leaked in 2 allocation(s).
```


## 总结

Clang and [GCC 4.9](https://gcc.gnu.org/PR59061) implemented LeakSanitizer in 2013. [LeakSanitizer](https://clang.llvm.org/docs/LeakSanitizer.html) (`LSan`) is a memory leak detector. It intercepts memory allocation functions and by default detects memory leaks at `atexit` time. The implementation is purely in the runtime (`compiler-rt/lib/lsan`) and no instrumentation is needed.

`LSan` has very little architecture-specific code and supports many 64-bit targets. Some 32-bit targets (e.g. Linux arm/x86-32) are supported as well, but there may be high false negatives because pointers with fewer bits are more easily confused with integers/floating points/other data of a similar pattern. Every supported operating system needs to provide some way to "stop the world".

* LeakSanitizer 的实现不依赖于特定的硬件架构，因此它可以支持许多64位目标平台。
* 也支持一些32位目标平台，如 Linux arm 和 x86-32
* 在32位目标平台上，LeakSanitizer 可能会出现较高的假阴性（即未检测到实际存在的内存泄漏）。这是因为32位指针的位数较少，更容易与整数、浮点数或其他类似模式的数据混淆。
* 每个受支持的操作系统都需要提供一种方法来“暂停世界”，即暂停所有线程以便 LeakSanitizer 可以安全地检查内存泄漏。这是因为在检查内存泄漏时，需要确保没有线程正在访问内存，以避免数据竞争和不一致的检测结果。

LSan can be used in 3 ways.

1. Standalone (`-fsanitize=leak`)
2. AddressSanitizer (`-fsanitize=address`)
3. HWAddressSanitizer (`-fsanitize=hwaddress`)

The most common way to use `LSan` is `clang -fsanitize=address` (or `gcc -fsanitize=address`). For LSan-supported targets (`#define CAN_SANITIZE_LEAKS 1`), the AddressSanitizer (`ASan`) runtime enables `LSan` by default.

``` cpp
#include <stdlib.h>
int main()
{
    void **p = malloc(42); // leak (categorized as "Direct leak")
    *p = malloc(43);       // leak (categorized as "Indirect leak")
    p = 0;
}
```


![asan](/assets/images/202308/asan.png)

![asan2](/assets/images/202308/asan2.png)

参考：

* [All about LeakSanitizer](https://maskray.me/blog/2023-02-12-all-about-leak-sanitizer)


# ThreadSanitizer

> **ThreadSanitizer** is part of clang 3.2 and gcc 4.8. To build the freshest version see `ThreadSanitizerDevelopment` page.

`ThreadSanitizer` (detects data races and deadlocks) for [C++](https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual) and [Go](https://github.com/google/sanitizers/wiki/ThreadSanitizerGoManual)

`ThreadSanitizer` (aka `TSan`) is a data race detector for C/C++. Data races are one of the most common and hardest to debug types of bugs in concurrent systems. A data race occurs when two threads access the same variable concurrently and at least one of the accesses is write. [C++11](http://en.wikipedia.org/wiki/C%2B%2B11) standard officially bans data races as **undefined behavior**.

Here is an example of a data race that can lead to crashes and memory corruptions:

``` cpp
#include <pthread.h>
#include <stdio.h>
#include <string>
#include <map>

typedef std::map<std::string, std::string> map_t;

void *threadfunc(void *p) {
  map_t& m = *(map_t*)p;
  m["foo"] = "bar";
  return 0;
}

int main() {
  map_t m;
  pthread_t t;
  pthread_create(&t, 0, threadfunc, &m);
  printf("foo=%s\n", m["foo"].c_str());
  pthread_join(t, 0);
}
```

There are a lot of various ways to trigger a data race in C++, see [ThreadSanitizerPopularDataRaces](https://github.com/google/sanitizers/wiki/ThreadSanitizerPopularDataRaces), `TSan` detects all of them and more -- [ThreadSanitizerDetectableBugs](https://github.com/google/sanitizers/wiki/ThreadSanitizerDetectableBugs).


## Supported Platforms

`TSan` is supported on:

* Linux: x86_64, mips64 (40-bit VMA), aarch64 (39/42-bit VMA), powerpc64 (44/46/47-bit VMA)
* Mac: x86_64, aarch64 (39-bit VMA)
* FreeBSD: x86_64
* NetBSD: x86_64

This list is last updated on Dec 2018 and is related to `clang` compiler, see up-to-date list [here](https://github.com/llvm-mirror/compiler-rt/blob/master/lib/tsan/rtl/tsan_platform.h). Platforms supported by `gcc` may differ. Older compiler versions may not support some of these platforms.


## Usage

Simply compile your program with `-fsanitize=thread` and link it with `-fsanitize=thread`. To get a reasonable performance add `-O2`. Use `-g` to get file names and line numbers in the warning messages.

When you run the program, `TSan` will print a report if it finds a data race. Here is an example:

``` cpp
$ cat simple_race.cc
#include <pthread.h>
#include <stdio.h>

int Global;

void *Thread1(void *x) {
  Global++;
  return NULL;
}

void *Thread2(void *x) {
  Global--;
  return NULL;
}

int main() {
  pthread_t t[2];
  pthread_create(&t[0], NULL, Thread1, NULL);
  pthread_create(&t[1], NULL, Thread2, NULL);
  pthread_join(t[0], NULL);
  pthread_join(t[1], NULL);
}
```

```
$ clang++ simple_race.cc -fsanitize=thread -fPIE -pie -g
$ ./a.out
==================
WARNING: ThreadSanitizer: data race (pid=26327)
  Write of size 4 at 0x7f89554701d0 by thread T1:
    #0 Thread1(void*) simple_race.cc:8 (exe+0x000000006e66)

  Previous write of size 4 at 0x7f89554701d0 by thread T2:
    #0 Thread2(void*) simple_race.cc:13 (exe+0x000000006ed6)

  Thread T1 (tid=26328, running) created at:
    #0 pthread_create tsan_interceptors.cc:683 (exe+0x00000001108b)
    #1 main simple_race.cc:19 (exe+0x000000006f39)

  Thread T2 (tid=26329, running) created at:
    #0 pthread_create tsan_interceptors.cc:683 (exe+0x00000001108b)
    #1 main simple_race.cc:20 (exe+0x000000006f63)
==================
ThreadSanitizer: reported 1 warnings
```

Refer to [ThreadSanitizerReportFormat](https://github.com/google/sanitizers/wiki/ThreadSanitizerReportFormat) for explanation of reports format.

There is a bunch of runtime and compiler flags to tune behavior of TSan -- see [ThreadSanitizerFlags](https://github.com/google/sanitizers/wiki/ThreadSanitizerFlags).


## Suppressing Reports

Sometimes you can't fix the race (e.g. in third-party code) or don't want to do it straight away. There are several options how you can suppress known reports:

* [Suppressions](https://github.com/google/sanitizers/wiki/ThreadSanitizerSuppressions) files (runtime mechanism).
* [Blacklist](https://github.com/google/sanitizers/wiki/ThreadSanitizerFlags) files (compile-time mechanism).
* Exclude problematic code/test under `TSan` with `#if defined(__has_feature) && __has_feature(thread_sanitizer)`.

## How To Test

To start, run your tests using `ThreadSanitizer`. The race detector only finds races that happen at runtime, so it can't find races in code paths that are not executed. If your tests have incomplete coverage, you may find more races by running a binary built with `-fsanitize=thread` under a realistic workload.

## Runtime Overhead

The cost of race detection varies by program, but for a typical program, memory usage may increase by `5-10x` and execution time by `2-20x`.

## Non-instrumented code

`ThreadSanitizer` generally requires all code to be compiled with `-fsanitize=thread`. If some code (e.g. **dynamic libraries**) is not compiled with the flag, it can lead to false positive race reports, false negative race reports and/or missed stack frames in reports depending on the nature of non-instrumented code. To not produce false positive reports `ThreadSanitizer` has to see all synchronization in the program, some synchronization operations (namely, atomic operations and thread-safe static initialization) are intercepted during compilation (and can only be intercepted during compilation). `ThreadSanitizer` stack trace collection also relies on compiler instrumentation (unwinding stack on each memory access is too expensive).

There are some precedents of making `ThreadSanitizer` work with non-instrumented libraries. Success of this highly depends on what exactly these libraries are doing. Besides suppressions, one can use `ignore_interceptors_accesses` flag which ignores memory accesses in all interceptors, or `ignore_noninstrumented_modules` flag which makes `ThreadSanitizer` ignore all interceptors called from the given set of (non-instrumented) libraries.

## FAQ

https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual#faq



# AddressSanitizer Algorithm

参考 [AddressSanitizerAlgorithm](https://github.com/google/sanitizers/wiki//AddressSanitizerAlgorithm)

## Short version

The run-time library replaces the `malloc` and `free` functions. The memory around malloc-ed regions (red zones) is poisoned. The `free`-ed memory is placed in quarantine(隔离) and also poisoned. Every memory access in the program is transformed by the compiler in the following way:

Before:

``` cpp
*address = ...;  // or: ... = *address;
```

After:

``` cpp
if (IsPoisoned(address)) {
  ReportError(address, kAccessSize, kIsWrite);
}
*address = ...;  // or: ... = *address;
```

The tricky part is how to implement `IsPoisoned` very fast and `ReportError` very compact. Also, instrumenting some of the accesses may be [proven redundant](https://github.com/google/sanitizers/wiki//AddressSanitizerCompileTimeOptimizations).

## Memory mapping and Instrumentation

The **virtual address space** is divided into 2 disjoint(不相交的) classes:

* Main application memory (`Mem`): this memory is used by the regular application code.
* Shadow memory (`Shadow`): this memory contains the shadow values (or metadata). There is a correspondence(关联) between the shadow and the main application memory. **Poisoning a byte in the main memory means writing some special value into the corresponding shadow memory**.

These 2 classes of memory should be organized in such a way that computing the shadow memory (`MemToShadow`) is fast.

The **instrumentation(插桩)** performed by the compiler:

``` cpp
shadow_address = MemToShadow(address);
if (ShadowIsPoisoned(shadow_address)) {
  ReportError(address, kAccessSize, kIsWrite);
}
```

### Mapping

**`AddressSanitizer` maps `8` bytes of the application memory into `1` byte of the shadow memory**.

There are only **`9` different values** for **any aligned `8` bytes** of the application memory:

* All 8 bytes in qword are unpoisoned (i.e. addressable). The shadow value is 0.
* All 8 bytes in qword are poisoned (i.e. not addressable). The shadow value is negative.
* First `k` bytes are unpoisoned, the rest `8-k` are poisoned. The shadow value is `k`. This is guaranteed by the fact that `malloc` returns `8-byte` aligned chunks of memory. The only case where different bytes of an aligned qword have different state is the tail of a malloc-ed region. For example, if we call `malloc(13)`, we will have one full unpoisoned qword and one qword where 5 first bytes are unpoisoned.

The instrumentation looks like this:

``` cpp
byte *shadow_address = MemToShadow(address);
byte shadow_value = *shadow_address;
if (shadow_value) {
  if (SlowPathCheck(shadow_value, address, kAccessSize)) {
    ReportError(address, kAccessSize, kIsWrite);
  }
}
```


``` cpp
// Check the cases where we access first k bytes of the qword
// and these k bytes are unpoisoned.
bool SlowPathCheck(shadow_value, address, kAccessSize) {
  last_accessed_byte = (address & 7) + kAccessSize - 1;
  return (last_accessed_byte >= shadow_value);
}
```

`MemToShadow(ShadowAddr)` falls into the `ShadowGap` region which is unaddressable. So, if the program tries to directly access a memory location in the shadow region, it will crash.

示例：

```
|| `[0x10007fff8000, 0x7fffffffffff]` || HighMem    ||
|| `[0x02008fff7000, 0x10007fff7fff]` || HighShadow ||
|| `[0x00008fff7000, 0x02008fff6fff]` || ShadowGap  ||
|| `[0x00007fff8000, 0x00008fff6fff]` || LowShadow  ||
|| `[0x000000000000, 0x00007fff7fff]` || LowMem     ||
MemToShadow(shadow): 0x00008fff7000 0x000091ff6dff 0x004091ff6e00 0x02008fff6fff
redzone=16
max_redzone=2048
quarantine_size_mb=256M
thread_local_quarantine_size_kb=1024K
malloc_context_size=30
SHADOW_SCALE: 3
SHADOW_GRANULARITY: 8
SHADOW_OFFSET: 0x7fff8000
```

**64-bit**

> `Shadow = (Mem >> 3) + 0x7fff8000;`

```
[0x10007fff8000, 0x7fffffffffff]	HighMem
[0x02008fff7000, 0x10007fff7fff]	HighShadow
[0x00008fff7000, 0x02008fff6fff]	ShadowGap
[0x00007fff8000, 0x00008fff6fff]	LowShadow
[0x000000000000, 0x00007fff7fff]	LowMem
```

**32 bit**

> `Shadow = (Mem >> 3) + 0x20000000;`

```
[0x40000000, 0xffffffff]	HighMem
[0x28000000, 0x3fffffff]	HighShadow
[0x24000000, 0x27ffffff]	ShadowGap
[0x20000000, 0x23ffffff]	LowShadow
[0x00000000, 0x1fffffff]	LowMem
```

### Ultra compact shadow

It is possible to use even more compact shadow memory, e.g.

> `Shadow = (Mem >> 7) | kOffset;`

Experiments are in flight.


### Report Error

The `ReportError` could be implemented as a call (this is the default now), but there are some other, slightly more efficient and/or more compact solutions. At some point the default behaviour **was**:

* copy the failure address to `%rax` (`%eax`).
* execute `ud2` (generates SIGILL)
* Encode access type and size in a one-byte instruction which follows `ud2`. Overall these 3 instructions require 5-6 bytes of machine code.

It is possible to use just a single instruction (e.g. `ud2`), but this will require to have a full disassembler in the run-time library (or some other hacks).

### Stack

In order to catch **stack buffer overflow**, `AddressSanitizer` instruments the code like this:

Original code:

``` cpp
void foo() {
  char a[8];
  ...
  return;
}
```

Instrumented code:

``` cpp
void foo() {
  char redzone1[32];  // 32-byte aligned
  char a[8];          // 32-byte aligned
  char redzone2[24];
  char redzone3[32];  // 32-byte aligned
  int  *shadow_base = MemToShadow(redzone1);
  shadow_base[0] = 0xffffffff;  // poison redzone1
  shadow_base[1] = 0xffffff00;  // poison redzone2, unpoison 'a'
  shadow_base[2] = 0xffffffff;  // poison redzone3
  ...
  shadow_base[0] = shadow_base[1] = shadow_base[2] = 0; // unpoison all
  return;
}
```

### Examples of instrumented code (x86_64)

```
# long load8(long *a) { return *a; }
0000000000000030 <load8>:
  30:	48 89 f8             	mov    %rdi,%rax
  33:	48 c1 e8 03          	shr    $0x3,%rax
  37:	80 b8 00 80 ff 7f 00 	cmpb   $0x0,0x7fff8000(%rax)
  3e:	75 04                	jne    44 <load8+0x14>
  40:	48 8b 07             	mov    (%rdi),%rax   <<<<<< original load
  43:	c3                   	retq
  44:	52                   	push   %rdx
  45:	e8 00 00 00 00       	callq  __asan_report_load8
```

```
# int  load4(int *a)  { return *a; }
0000000000000000 <load4>:
   0:	48 89 f8             	mov    %rdi,%rax
   3:	48 89 fa             	mov    %rdi,%rdx
   6:	48 c1 e8 03          	shr    $0x3,%rax
   a:	83 e2 07             	and    $0x7,%edx
   d:	0f b6 80 00 80 ff 7f 	movzbl 0x7fff8000(%rax),%eax
  14:	83 c2 03             	add    $0x3,%edx
  17:	38 c2                	cmp    %al,%dl
  19:	7d 03                	jge    1e <load4+0x1e>
  1b:	8b 07                	mov    (%rdi),%eax    <<<<<< original load
  1d:	c3                   	retq
  1e:	84 c0                	test   %al,%al
  20:	74 f9                	je     1b <load4+0x1b>
  22:	50                   	push   %rax
  23:	e8 00 00 00 00       	callq  __asan_report_load4
```

### Unaligned accesses

The current compact mapping will not catch unaligned partially out-of-bound accesses:

``` cpp
int *x = new int[2]; // 8 bytes: [0,7].
int *u = (int*)((char*)x + 6);
*u = 1;  // Access to range [6-9]
```

A viable solution is described in https://github.com/google/sanitizers/issues/100 but it comes at a performance cost.

## Run-time library


### Malloc

The run-time library replaces `malloc`/`free` and provides error reporting functions like `__asan_report_load8`.

`malloc` allocates the requested amount of memory with **redzones** around it. The shadow values corresponding to the redzones are poisoned and the shadow values for the main memory region are cleared.

`free` poisons shadow values for the entire region and puts the chunk of memory into a quarantine(检疫) queue (such that this chunk will not be returned again by malloc during some period of time).





# 性能测试对比

This tool is very fast. The average slowdown of the instrumented program is `~2x` (see [AddressSanitizerPerformanceNumbers](https://github.com/google/sanitizers/wiki/AddressSanitizerPerformanceNumbers)).


不同工具的性能比较：[AddressSanitizerComparisonOfMemoryTools](https://github.com/google/sanitizers/wiki/AddressSanitizerComparisonOfMemoryTools)



# 不同编译器


## GCC

参考 GCC [-fsanitize=address](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#Instrumentation-Options) 使用说明如下：

Enable AddressSanitizer, a fast memory error detector. Memory access instructions are instrumented to detect out-of-bounds and use-after-free bugs. The option enables `-fsanitize-address-use-after-scope`. See https://github.com/google/sanitizers/wiki/AddressSanitizer for more details.

The run-time behavior can be influenced using the `ASAN_OPTIONS` environment variable. When set to `help=1`, the available options are shown at startup of the instrumented program. See https://github.com/google/sanitizers/wiki/AddressSanitizerFlags#run-time-flags for a list of supported options.

The option cannot be combined with `-fsanitize=thread` or `-fsanitize=hwaddress`. Note that the only target `-fsanitize=hwaddress` is currently supported on is AArch64.

测试代码：

``` cpp
#include <stdlib.h>

void f(void)
{
    int *x = malloc(10* sizeof(int));
    x[10] = 0;  // problem 1: heap block overrun
}               // problem 2: memory leak -- x not freed

int main(void)
{
    f();
}
```

```
yum install libasan

gcc -g -fsanitize=address -fno-omit-frame-pointer demo.c
```

![gcc_sanitize](/assets/images/202106/gcc_sanitize.png)

## Clang

编译选项：

```
# AddressSanitizer
IF(USE_ASAN EQUAL 1)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-optimize-sibling-calls")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize-recover=address")  # 设置遇到错误时继续运行
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -asan-stack=0")     # 关闭 stack 检查
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -asan-globals=1")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -asan-use-private-alias=0")

    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
ENDIF()
```

启动选项：

``` bash
#!/bin/bash

# ASAN_OPTIONS=detect_odr_violation=0 不检查 ODR 错误

ASAN_SYMBOLIZER_PATH=../../tools/llvm/llvm-symbolizer \
LSAN_OPTIONS=suppressions=../../suppressions.lsan \
ASAN_OPTIONS=debug=true:\
verbosity=2:\
check_initialization_order=1:\
detect_odr_violation=0:\
print_stats=true:\
disable_coredump=false:\
print_suppressions=true:\
abort_on_error=true:\
halt_on_error=false:\
log_exe_name=true:\
log_path=asan.log \
./your_program
```


# Valgrind vs Sanitizers

1. With the `caveat` that `valgrind` needs support for all the instructions your software is using, `valgrind` can run pretty much any software, even when you do not have the source code. `Sanitizers` work at the compiler level, so you need the source code. Thus if you need to debug a closed source library, sanitizers are unhelpful.

2. `Sanitizers` can catch problems that `valgrind` will not catch. For example, it will catch undesirable undefined behaviour: code that may work right now but may not work if you use a different compiler or a different processor. They can catch unsafe memory accesses that will look safe to valgrind.

3. `Sanitizers` are more precise. You often can turn on or off specific sanitizers for specific functions.

4. If your compiler has sanitizers, you can run your tests with the sanitizers on simply by turning on some flags.

5. `Valgrind` is slow. Like debuggers, it often does not scale. If you are working over large data sets, it might take a really long time. People often dismiss “execution time”, and it is easy to do if you work on toy problems, but performance is an essential quality-of-life attribute. I do not think you can run valgrind in a simulated production setting. However, you can compile your code with sanitizers and emulate a production setting. Sure, your throughput is going to be impacted, but the effect is not large. Code with sanitizers is not 10x slower, valgrind is.

6. `Sanitizers` are relatively new and so the support is sometimes missing.

See: https://lemire.me/blog/2019/05/16/building-better-software-with-better-tools-sanitizers-versus-valgrind/


# FAQ

## Q: Can AddressSanitizer continue running after reporting first error?

A: Yes it can, AddressSanitizer has recently got `continue-after-error` mode. This is somewhat experimental so may not yet be as reliable as default setting (and not as timely supported). Also keep in mind that errors after the first one may actually be spurious(假的). To enable `continue-after-error`, compile with `-fsanitize-recover=address` and then run your code with `ASAN_OPTIONS=halt_on_error=0`.

* [Address Sanitizer option "-fsanitize-recover=address" is not supported](https://stackoverflow.com/questions/53391286/address-sanitizer-option-fsanitize-recover-address-is-not-supported)

## Q: 如何让后台服务定期采集内存泄漏，而不是在程序退出的时候

LeakSanitizer 的检测结果，默认是在程序退出前输出的，因此对于后台服务可以在代码里定期调用接口输出。

``` cpp
#include <thread>
#include "sanitizer/lsan_interface.h"

int main(int argc, char** argv)
{
    // ...

    // 定期采集
    std::thread check([]() {
    while (true) {
      __lsan_do_recoverable_leak_check();
      std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }});

}
```

``` cpp
  // Check for leaks now. Returns zero if no leaks have been found or if leak
  // detection is disabled, non-zero otherwise.
  // This function may be called repeatedly, e.g. to periodically check a
  // long-running process. It prints a leak report if appropriate, but does not
  // terminate the process. It does not affect the behavior of
  // __lsan_do_leak_check() or the end-of-process leak check, and is not
  // affected by them.
  int __lsan_do_recoverable_leak_check(void);
```


## Q: Why didn't ASan report an obviously invalid memory access in my code?

A1: If your errors is too obvious, compiler might have already optimized it out by the time Asan runs.

A2: Another, C-only option is accesses to global common symbols which are not protected by Asan (you can use `-fno-common` to disable generation of common symbols and hopefully detect more bugs).

A3: If `_FORTIFY_SOURCE` is enabled, ASan may have false positives, see next question.

## Q: When I link my shared library with -fsanitize=address, it fails due to some undefined ASan symbols (e.g. asan_init_v4)?

A: Most probably you link with `-Wl,-z,defs` or `-Wl,--no-undefined`. These flags don't work with ASan unless you also use `-shared-libasan` (which is the default mode for `GCC`, but not for `Clang`).

## Q: My malloc stacktraces are too short or do not make sense?

A: Try to compile your code with `-fno-omit-frame-pointer` or set `ASAN_OPTIONS=fast_unwind_on_malloc=0` (the latter would be a performance killer though unless you also specify `malloc_context_size=2` or lower). Note that frame-pointer-based unwinding does not work on Thumb.

## Q: My new() and delete() stacktraces are too short or do not make sense?

A: This may happen when the C++ standard library is linked statically. Prebuilt `libstdc++`/`libc++` often do not use frame pointers, and it breaks fast (frame-pointer-based) unwinding. Either switch to the shared library with the `-shared-libstdc++` flag, or use `ASAN_OPTIONS=fast_unwind_on_malloc=0`. The latter could be very slow.


## Q: [What does it mean if you're getting a "addresssanitizer: segv on unknown address" error (c++, runtime error, development)?](https://www.quora.com/What-does-it-mean-if-youre-getting-a-addresssanitizer-segv-on-unknown-address-error-c-runtime-error-development)

It means that you contains at least one serious bug, that under certain circumstances it dereferences a pointer, or accesses an array element, that does not point to a valid memory address.

## 更多问题

* https://github.com/google/sanitizers/wiki/AddressSanitizer
* https://github.com/google/sanitizers/issues



# 误报场景

## stack-buffer-overflow

参考 [Suspicious stack-overflow message that points to a valid stack range](https://github.com/google/sanitizers/issues/1533)

This is `stack-buffer-overflow`, **not** `stack-overflow`. **Meaning read or write out of bounds of some local variable on stack**. If you are sure that this particular access is within bounds and within lifetime of whatever location it targets, then the common cause of **false positives** would be moving the stack pointer up (i.e. taking a function frame or several off the stack, like `longjmp` would) without telling asan about it. Sounds like your "breaking out" of the handler is exactly that.

If it's not done with `longjmp-in-libc` (the one that ASan intercepts), you'd need to call `__asan_unpoison` on the relevant portion of the stack.

Switching between coroutines would generally be fine with ASan. It's undeclared returns from functions (longjmp, exceptions, etc) that asan has
problems with because we rely on each function to clear the stack shadow after itself.

`__asan_unpoison_memory_region` does solve the issue.





# Refer

* https://github.com/google/sanitizers/wiki/
* https://www.usenix.org/system/files/conference/atc12/atc12-final39.pdf
* [No more leaks with sanitize flags in gcc and clang](https://lemire.me/blog/2016/04/20/no-more-leaks-with-sanitize-flags-in-gcc-and-clang/)
* [Building better software with better tools: sanitizers versus valgrind](https://lemire.me/blog/2019/05/16/building-better-software-with-better-tools-sanitizers-versus-valgrind/)
* [How to use gcc with fsanitize=address?](https://stackoverflow.com/questions/58262749/how-to-use-gcc-with-fsanitize-address)
* http://gavinchou.github.io/experience/summary/syntax/gcc-address-sanitizer/
* [All about LeakSanitizer](https://maskray.me/blog/2023-02-12-all-about-leak-sanitizer)





