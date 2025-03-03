---
layout: post
title:  "AddressSanitizer (ASan) - A Fast Memory Error Detector"
date:   2021-10-15 08:30:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}

# TL;DR

[AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) is a fast memory error detector. It consists of a compiler instrumentation module and a run-time library. The tool can detect the following types of bugs:

1. Out-of-bounds accesses to heap, stack and globals
2. Use-after-free
3. Use-after-return (clang flag `-fsanitize-address-use-after-return=(never|runtime|always)` default: `runtime`)
   1. Enable with: `ASAN_OPTIONS=detect_stack_use_after_return=1` (already enabled on Linux).
   2. Disable with: `ASAN_OPTIONS=detect_stack_use_after_return=0`.
4. Use-after-scope (clang flag `-fsanitize-address-use-after-scope`)
5. Double-free, invalid free
6. Memory leaks (experimental)

Typical slowdown introduced by AddressSanitizer is `2x`. 性能指标可参考：[AddressSanitizerComparisonOfMemoryTools](https://github.com/google/sanitizers/wiki/AddressSanitizerComparisonOfMemoryTools) 和 [AddressSanitizerPerformanceNumbers](https://github.com/google/sanitizers/wiki/AddressSanitizerPerformanceNumbers)



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


### ASAN_OPTIONS Run-time flags 参考 (ASAN_OPTIONS=help=1)

```
Available flags for AddressSanitizer:

        quarantine_size
                - Deprecated, please use quarantine_size_mb. (Current Value: -1)
        quarantine_size_mb
                - Size (in Mb) of quarantine used to detect use-after-free errors. Lower value may reduce memory usage but increase the chance of false negatives. (Current Value: -1)
        thread_local_quarantine_size_kb
                - Size (in Kb) of thread local quarantine used to detect use-after-free errors. Lower value may reduce memory usage but increase the chance of false negatives. It is not advised to go lower than 64Kb, otherwise frequent transfers to global quarantine might affect performance. (Current Value: -1)
        redzone
                - Minimal size (in bytes) of redzones around heap objects. Requirement: redzone >= 16, is a power of two. (Current Value: 16)
        max_redzone
                - Maximal size (in bytes) of redzones around heap objects. (Current Value: 2048)
        debug
                - If set, prints some debugging information and does additional checks. (Current Value: false)
        report_globals
                - Controls the way to handle globals (0 - don't detect buffer overflow on globals, 1 - detect buffer overflow, 2 - print data about registered globals). (Current Value: 1)
        check_initialization_order
                - If set, attempts to catch initialization order issues. (Current Value: false)
        replace_str
                - If set, uses custom wrappers and replacements for libc string functions to find more errors. (Current Value: true)
        replace_intrin
                - If set, uses custom wrappers for memset/memcpy/memmove intrinsics. (Current Value: true)
        detect_stack_use_after_return
                - Enables stack-use-after-return checking at run-time. (Current Value: false)
        min_uar_stack_size_log
                - Minimum fake stack size log. (Current Value: 16)
        max_uar_stack_size_log
                - Maximum fake stack size log. (Current Value: 20)
        uar_noreserve
                - Use mmap with 'noreserve' flag to allocate fake stack. (Current Value: false)
        max_malloc_fill_size
                - ASan allocator flag. max_malloc_fill_size is the maximal amount of bytes that will be filled with malloc_fill_byte on malloc. (Current Value: 4096)
        max_free_fill_size
                - ASan allocator flag. max_free_fill_size is the maximal amount of bytes that will be filled with free_fill_byte during free. (Current Value: 0)
        malloc_fill_byte
                - Value used to fill the newly allocated memory. (Current Value: 190)
        free_fill_byte
                - Value used to fill deallocated memory. (Current Value: 85)
        allow_user_poisoning
                - If set, user may manually mark memory regions as poisoned or unpoisoned. (Current Value: true)
        sleep_before_dying
                - Number of seconds to sleep between printing an error report and terminating the program. Useful for debugging purposes (e.g. when one needs to attach gdb). (Current Value: 0)
        sleep_after_init
                - Number of seconds to sleep after AddressSanitizer is initialized. Useful for debugging purposes (e.g. when one needs to attach gdb). (Current Value: 0)
        check_malloc_usable_size
                - Allows the users to work around the bug in Nvidia drivers prior to 295.*. (Current Value: true)
        unmap_shadow_on_exit
                - If set, explicitly unmaps the (huge) shadow at exit. (Current Value: false)
        protect_shadow_gap
                - If set, mprotect the shadow gap (Current Value: true)
        print_stats
                - Print various statistics after printing an error message or if atexit=1. (Current Value: false)
        print_legend
                - Print the legend for the shadow bytes. (Current Value: true)
        print_scariness
                - Print the scariness score. Experimental. (Current Value: false)
        atexit
                - If set, prints ASan exit stats even after program terminates successfully. (Current Value: false)
        print_full_thread_history
                - If set, prints thread creation stacks for the threads involved in the report and their ancestors up to the main thread. (Current Value: true)
        poison_heap
                - Poison (or not) the heap memory on [de]allocation. Zero value is useful for benchmarking the allocator or instrumentator. (Current Value: true)
        poison_partial
                - If true, poison partially addressable 8-byte aligned words (default=true). This flag affects heap and global buffers, but not stack buffers. (Current Value: true)
        poison_array_cookie
                - Poison (or not) the array cookie after operator new[]. (Current Value: true)
        alloc_dealloc_mismatch
                - Report errors on malloc/delete, new/free, new/delete[], etc. (Current Value: true)
        new_delete_type_mismatch
                - Report errors on mismatch between size of new and delete. (Current Value: true)
        strict_init_order
                - If true, assume that dynamic initializers can never access globals from other modules, even if the latter are already initialized. (Current Value: false)
        start_deactivated
                - If true, ASan tweaks a bunch of other flags (quarantine, redzone, heap poisoning) to reduce memory consumption as much as possible, and restores them to original values when the first instrumented module is loaded into the process. This is mainly intended to be used on Android.  (Current Value: false)
        detect_invalid_pointer_pairs
                - If >= 2, detect operations like <, <=, >, >= and - on invalid pointer pairs (e.g. when pointers belong to different objects); If == 1, detect invalid operations only when both pointers are non-null. (Current Value: 0)
        detect_container_overflow
                - If true, honor the container overflow annotations. See https://github.com/google/sanitizers/wiki/AddressSanitizerContainerOverflow (Current Value: true)
        detect_odr_violation
                - If >=2, detect violation of One-Definition-Rule (ODR); If ==1, detect ODR-violation only if the two variables have different sizes (Current Value: 2)
        suppressions
                - Suppressions file name. (Current Value: )
        halt_on_error
                - Crash the program after printing the first error report (WARNING: USE AT YOUR OWN RISK!) (Current Value: true)
        allocator_frees_and_returns_null_on_realloc_zero
                - realloc(p, 0) is equivalent to free(p) by default (Same as the POSIX standard). If set to false, realloc(p, 0) will return a pointer to an allocated space which can not be used. (Current Value: true)
        verify_asan_link_order
                - Check position of ASan runtime in library list (needs to be disabled when other library has to be preloaded system-wide) (Current Value: true)
        windows_hook_rtl_allocators
                - (Windows only) enable hooking of Rtl(Allocate|Free|Size|ReAllocate)Heap. (Current Value: false)
        symbolize
                - If set, use the online symbolizer from common sanitizer runtime to turn virtual addresses to file/line locations. (Current Value: true)
        external_symbolizer_path
                - Path to external symbolizer. If empty, the tool will search $PATH for the symbolizer. (Current Value: <null>)
        allow_addr2line
                - If set, allows online symbolizer to run addr2line binary to symbolize stack traces (addr2line will only be used if llvm-symbolizer binary is unavailable. (Current Value: false)
        strip_path_prefix
                - Strips this prefix from file paths in error reports. (Current Value: )
        fast_unwind_on_check
                - If available, use the fast frame-pointer-based unwinder on internal CHECK failures. (Current Value: false)
        fast_unwind_on_fatal
                - If available, use the fast frame-pointer-based unwinder on fatal errors. (Current Value: false)
        fast_unwind_on_malloc
                - If available, use the fast frame-pointer-based unwinder on malloc/free. (Current Value: true)
        handle_ioctl
                - Intercept and handle ioctl requests. (Current Value: false)
        malloc_context_size
                - Max number of stack frames kept for each allocation/deallocation. (Current Value: 30)
        log_path
                - Write logs to "log_path.pid". The special values are "stdout" and "stderr". The default is "stderr". (Current Value: stderr)
        log_exe_name
                - Mention name of executable when reporting error and append executable name to logs (as in "log_path.exe_name.pid"). (Current Value: false)
        log_to_syslog
                - Write all sanitizer output to syslog in addition to other means of logging. (Current Value: false)
        verbosity
                - Verbosity level (0 - silent, 1 - a bit of output, 2+ - more output). (Current Value: 0)
        strip_env
                - Whether to remove the sanitizer from DYLD_INSERT_LIBRARIES to avoid passing it to children. Default is true. (Current Value: true)
        detect_leaks
                - Enable memory leak detection. (Current Value: true)
        leak_check_at_exit
                - Invoke leak checking in an atexit handler. Has no effect if detect_leaks=false, or if __lsan_do_leak_check() is called before the handler has a chance to run. (Current Value: true)
        allocator_may_return_null
                - If false, the allocator will crash instead of returning 0 on out-of-memory. (Current Value: false)
        print_summary
                - If false, disable printing error summaries in addition to error reports. (Current Value: true)
        print_module_map
                - OS X only (0 - don't print, 1 - print only once before process exits, 2 - print after each report). (Current Value: 0)
        check_printf
                - Check printf arguments. (Current Value: true)
        handle_segv
                - Controls custom tool's SIGSEGV handler (0 - do not registers the handler, 1 - register the handler and allow user to set own, 2 - registers the handler and block user from changing it).  (Current Value: 1)
        handle_sigbus
                - Controls custom tool's SIGBUS handler (0 - do not registers the handler, 1 - register the handler and allow user to set own, 2 - registers the handler and block user from changing it).  (Current Value: 1)
        handle_abort
                - Controls custom tool's SIGABRT handler (0 - do not registers the handler, 1 - register the handler and allow user to set own, 2 - registers the handler and block user from changing it).  (Current Value: 0)
        handle_sigill
                - Controls custom tool's SIGILL handler (0 - do not registers the handler, 1 - register the handler and allow user to set own, 2 - registers the handler and block user from changing it).  (Current Value: 0)
        handle_sigtrap
                - Controls custom tool's SIGTRAP handler (0 - do not registers the handler, 1 - register the handler and allow user to set own, 2 - registers the handler and block user from changing it).  (Current Value: 0)
        handle_sigfpe
                - Controls custom tool's SIGFPE handler (0 - do not registers the handler, 1 - register the handler and allow user to set own, 2 - registers the handler and block user from changing it).  (Current Value: 1)
        allow_user_segv_handler
                - Deprecated. True has no effect, use handle_sigbus=1. If false, handle_*=1 will be upgraded to handle_*=2. (Current Value: true)
        use_sigaltstack
                - If set, uses alternate stack for signal handling. (Current Value: true)
        detect_deadlocks
                - If set, deadlock detection is enabled. (Current Value: true)
        clear_shadow_mmap_threshold
                - Large shadow regions are zero-filled using mmap(NORESERVE) instead of memset(). This is the threshold size in bytes. (Current Value: 0x000000010000)
        color
                - Colorize reports: (always|never|auto). (Current Value: auto)
        legacy_pthread_cond
                - Enables support for dynamic libraries linked with libpthread 2.2.5. (Current Value: false)
        intercept_tls_get_addr
                - Intercept __tls_get_addr. (Current Value: true)
        help
                - Print the flag descriptions. (Current Value: true)
        mmap_limit_mb
                - Limit the amount of mmap-ed memory (excluding shadow) in Mb; not a user-facing flag, used mosly for testing the tools (Current Value: 0x000000000000)
        hard_rss_limit_mb
                - Hard RSS limit in Mb. If non-zero, a background thread is spawned at startup which periodically reads RSS and aborts the process if the limit is reached (Current Value: 0x000000000000)
        soft_rss_limit_mb
                - Soft RSS limit in Mb. If non-zero, a background thread is spawned at startup which periodically reads RSS. If the limit is reached all subsequent malloc/new calls will fail or return NULL (depending on the value of allocator_may_return_null) until the RSS goes below the soft limit. This limit does not affect memory allocations other than malloc/new. (Current Value: 0x000000000000)
        max_allocation_size_mb
                - If non-zero, malloc/new calls larger than this size will return nullptr (or crash if allocator_may_return_null=false). (Current Value: 0x000000000000)
        heap_profile
                - Experimental heap profiler, asan-only (Current Value: false)
        allocator_release_to_os_interval_ms
                - Only affects a 64-bit allocator. If set, tries to release unused memory to the OS, but not more often than this interval (in milliseconds). Negative values mean do not attempt to release memory to the OS.
 (Current Value: 5000)
        can_use_proc_maps_statm
                - If false, do not attempt to read /proc/maps/statm. Mostly useful for testing sanitizers. (Current Value: true)
        coverage
                - If set, coverage information will be dumped at program shutdown (if the coverage instrumentation was enabled at compile time). (Current Value: false)
        coverage_dir
                - Target directory for coverage dumps. Defaults to the current directory. (Current Value: .)
        full_address_space
                - Sanitize complete address space; by default kernel area on 32-bit platforms will not be sanitized (Current Value: false)
        print_suppressions
                - Print matched suppressions at exit. (Current Value: true)
        disable_coredump
                - Disable core dumping. By default, disable_coredump=1 on 64-bit to avoid dumping a 16T+ core file. Ignored on OSes that don't dump core by default and for sanitizers that don't reserve lots of virtual memory. (Current Value: true)
        use_madv_dontdump
                - If set, instructs kernel to not store the (huge) shadow in core file. (Current Value: true)
        symbolize_inline_frames
                - Print inlined frames in stacktraces. Defaults to true. (Current Value: true)
        symbolize_vs_style
                - Print file locations in Visual Studio style (e.g:  file(10,42): ... (Current Value: false)
        dedup_token_length
                - If positive, after printing a stack trace also print a short string token based on this number of frames that will simplify deduplication of the reports. Example: 'DEDUP_TOKEN: foo-bar-main'. Default is 0. (Current Value: 0)
        stack_trace_format
                - Format string used to render stack frames. See sanitizer_stacktrace_printer.h for the format description. Use DEFAULT to get default format. (Current Value: DEFAULT)
        no_huge_pages_for_shadow
                - If true, the shadow is not allowed to use huge pages.  (Current Value: true)
        strict_string_checks
                - If set check that string arguments are properly null-terminated (Current Value: false)
        intercept_strstr
                - If set, uses custom wrappers for strstr and strcasestr functions to find more errors. (Current Value: true)
        intercept_strspn
                - If set, uses custom wrappers for strspn and strcspn function to find more errors. (Current Value: true)
        intercept_strtok
                - If set, uses a custom wrapper for the strtok function to find more errors. (Current Value: true)
        intercept_strpbrk
                - If set, uses custom wrappers for strpbrk function to find more errors. (Current Value: true)
        intercept_strlen
                - If set, uses custom wrappers for strlen and strnlen functions to find more errors. (Current Value: true)
        intercept_strndup
                - If set, uses custom wrappers for strndup functions to find more errors. (Current Value: true)
        intercept_strchr
                - If set, uses custom wrappers for strchr, strchrnul, and strrchr functions to find more errors. (Current Value: true)
        intercept_memcmp
                - If set, uses custom wrappers for memcmp function to find more errors. (Current Value: true)
        strict_memcmp
                - If true, assume that memcmp(p1, p2, n) always reads n bytes before comparing p1 and p2. (Current Value: true)
        intercept_memmem
                - If set, uses a wrapper for memmem() to find more errors. (Current Value: true)
        intercept_intrin
                - If set, uses custom wrappers for memset/memcpy/memmove intrinsics to find more errors. (Current Value: true)
        intercept_stat
                - If set, uses custom wrappers for *stat functions to find more errors. (Current Value: true)
        intercept_send
                - If set, uses custom wrappers for send* functions to find more errors. (Current Value: true)
        decorate_proc_maps
                - If set, decorate sanitizer mappings in /proc/self/maps with user-readable names (Current Value: false)
        exitcode
                - Override the program exit status if the tool found an error (Current Value: 1)
        abort_on_error
                - If set, the tool calls abort() instead of _exit() after printing the error report. (Current Value: false)
        suppress_equal_pcs
                - Deduplicate multiple reports for single source location in halt_on_error=false mode (asan only). (Current Value: true)
        print_cmdline
                - Print command line on crash (asan only). (Current Value: false)
        html_cov_report
                - Generate html coverage report. (Current Value: false)
        sancov_path
                - Sancov tool location. (Current Value: sancov)
        dump_instruction_bytes
                - If true, dump 16 bytes starting at the instruction that caused SEGV (Current Value: false)
        dump_registers
                - If true, dump values of CPU registers when SEGV happens. Only available on OS X for now. (Current Value: true)
        detect_write_exec
                - If true, triggers warning when writable-executable pages requests are being made (Current Value: false)
        test_only_emulate_no_memorymap
                - TEST ONLY fail to read memory mappings to emulate sanitized "init" (Current Value: false)
        include
                - read more options from the given file (Current Value: )
        include_if_exists
                - read more options from the given file (if it exists) (Current Value: )
```




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




## [AddressSanitizerUseAfterReturn](https://github.com/google/sanitizers/wiki/AddressSanitizerUseAfterReturn)

`Stack-use-after-return` bug appears when a stack object is used after the function where this object is defined has returned. Example (see also [AddressSanitizerExampleUseAfterReturn](https://github.com/google/sanitizers/wiki/AddressSanitizerExampleUseAfterReturn)):

``` cpp
// RUN: clang -O -g -fsanitize=address %t && ./a.out
// By default, AddressSanitizer does not try to detect
// stack-use-after-return bugs.
// It may still find such bugs occasionally
// and report them as a hard-to-explain stack-buffer-overflow.

// You need to run the test with ASAN_OPTIONS=detect_stack_use_after_return=1

int *ptr;
__attribute__((noinline))
void FunctionThatEscapesLocalObject() {
  int local[100];
  ptr = &local[0];
}

int main(int argc, char **argv) {
  FunctionThatEscapesLocalObject();
  return ptr[argc];
}
```

```
=================================================================
==6268== ERROR: AddressSanitizer: stack-use-after-return on address 0x7fa19a8fc024 at pc 0x4180d5 bp 0x7fff73c3fc50 sp 0x7fff73c3fc48
READ of size 4 at 0x7fa19a8fc024 thread T0
    #0 0x4180d4 in main example_UseAfterReturn.cc:17
    #1 0x7fa19b11d76c (/lib/x86_64-linux-gnu/libc.so.6+0x2176c)
    #2 0x417f34 (a.out+0x417f34)
Address 0x7fa19a8fc024 is located at offset 36 in frame <_Z30FunctionThatEscapesLocalObjectv> of T0's stack:
  This frame has 1 object(s):
    [32, 432) 'local'
HINT: this may be a false positive if your program uses some custom stack unwind mechanism or swapcontext
      (longjmp and C++ exceptions *are* supported)
Shadow bytes around the buggy address:
  0x1ff43351f7b0: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe
  0x1ff43351f7c0: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe
  0x1ff43351f7d0: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe
  0x1ff43351f7e0: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe
  0x1ff43351f7f0: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe
=>0x1ff43351f800: f5 f5 f5 f5[f5]f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x1ff43351f810: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x1ff43351f820: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x1ff43351f830: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 00 00 00 00
  0x1ff43351f840: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1ff43351f850: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:     fa
  Heap righ redzone:     fb
  Freed Heap region:     fd
  Stack left redzone:    f1
  Stack mid redzone:     f2
  Stack right redzone:   f3
  Stack partial redzone: f4
  Stack after return:    f5
  Stack use after scope: f8
  Global redzone:        f9
  Global init order:     f6
  Poisoned by user:      f7
  ASan internal:         fe
==6268== ABORTING
```

AddressSanitizer currently does not attempt to detect these bugs by default, only with an additional flag run-time: `ASAN_OPTIONS=detect_stack_use_after_return=1`

**Algorithm**

Detection of stack-use-after-return is similar to detection of heap-use-after-free, but the **quarantine** should be implemented in a different way.

Once a function has returned, its stack memory is reused by the next call instruction. So in order to implement quarantine for the stack memory we need to promote stack to heap. The current implementation does it like this:

Before:

``` cpp
void foo() {
  int local;
  escape_addr(&local);
}
```

After:

``` cpp
void foo() {
  char redzone1[32];
  int local;
  char redzone2[32+28];
  char *fake_stack = __asan_stack_malloc(&local, 96);
  poison_redzones(fake_stack);  // Done by the inlined instrumentation code.
  escape_addr(fake_stack + 32);
  __asan_stack_free(stack, &local, 96)
}
```

`__asan_stack_malloc(real_stack, frame_size)` allocates a fake frame (`frame_size` bytes) from **a thread-local heap-like structure** (**fake stack**). Every fake frame comes unpoisoned and then the redzones are poisoned in the instrumented function code.

`__asan_stack_free(fake_stack, real_stack, frame_size)` poisons the entire fake frame and deallocates it.

ASan 检测"栈返回后使用"（stack-use-after-return）漏洞的机制。**其核心原理是通过将栈分配转移到堆内存，并添加毒化区域（redzone）来检测非法访问**。具体分析如下：

* 内存分配
  + `__asan_stack_malloc(&local, 96)` **将原本的栈分配转换为堆分配，返回的 fake_stack 是堆内存地址**
  + `redzone1[32]` 和 `redzone2[32+28]` 是 ASan 添加的毒化区域，用于检测越界访问
* 漏洞触发点
  + `escape_addr(fake_stack + 32)` **将分配的堆地址泄露到函数外部**
  + 当 `foo()` 函数返回后，`__asan_stack_free` 会释放该堆内存
  + 如果外部代码继续使用 `fake_stack + 32` 地址，就会触发 `use-after-return` 错误
* 检测原理
  + ASan 通过替换栈分配为堆分配，使得函数返回后相关内存立即被释放
  + 毒化区域 `poison_redzones(fake_stack)` 会标记内存边界，任何越界访问都会被立即检测到
  + 典型的错误表现是：`ERROR: AddressSanitizer: stack-use-after-return`

**Memory consumption**

The **Fake Stack** allocator uses a fixed amount of memory per each thread. The allocator has 11 size classes (from `2**6` bytes to `2**16` bytes), every size class has fixed amount of chunks. If the given size class is fully used, the allocator will return 0 and thus regular stack will be used (i.e. `stack-use-after-return` detection will not work for the given function call). The amount of memory given to every size class is proportional to the size of thread's real stack, but not more than `2**max_uar_stack_size_log` (by default, `2**20`) and not less than `2**min_uar_stack_size_log` (by default, `2**16`). See also: [AddressSanitizerFlags](https://github.com/google/sanitizers/wiki/AddressSanitizerFlags).

So, with the default `8Mb` stack size and default AddressSanitizerFlags each size class will get `2**20` bytes and thus every thread will mmap ~11Mb for Fake Stack.

**The bigger the Fake Stack the better your chances to catch a stack-use-after-return and get a correct report, but the greater is the memory consumption.**

ASan 的 **Fake Stack** 分配器通过内存预分配机制检测"栈返回后使用"（stack-use-after-return）漏洞，其内存消耗机制如下：

* 内存分配架构
  + 固定大小类别：分配器包含 2^6 到 2^16 字节共 11 个大小类别，每个类别有固定数量的内存块（chunk）
  + 线程独立内存池：每个线程独立分配 Fake Stack 内存池，避免多线程竞争
  + 动态调整规则
    - 每个类别的内存总量与线程实际栈大小成比例
    - 受上下限约束：上限 2^20（默认 1MB），下限 2^16（默认 64KB）
  + 默认配置下的计算 (以 8MB 线程栈和默认参数为例：)
    - 每个大小类别分配：2^20 字节（1MB）
    - 总内存消耗：11 × 1MB = 11MB 虚拟内存（通过 `mmap` 分配）
  + 性能与内存的权衡 (当某大小类别的内存耗尽时，ASan 会回退到常规栈分配，此时无法检测 stack-use-after-return)
    - 增大 Fake Stack -> 提高检测成功率，错误报告更精准 -> 内存消耗增加（线性增长）
    - 减小 Fake Stack -> 降低内存占用 -> 可能漏检部分 use-after-return
  + 设计意义
    - 堆模拟栈：**将栈对象转移到堆内存，通过毒化（poison）标记边界（redzone），实现越界访问检测**
    - 效率优化：固定大小类别减少内存碎片，预分配策略降低运行时开销（相比 Valgrind 性能损失减少 10 倍以上）

**Performance**

Detecting `stack-use-after-return` is expensive in both `CPU` and `RAM`:

* **Allocating fake frames** introduces two function calls per every function with non-empty frame. (多了一次内存分配，一次内存回收的函数调用)
* **The entire fake frames should be unpoisoned on entry and poisoned on exit**, as opposed to poisoning just the redzones in the default mode. (poisoning 的内存范围更大)





Read about [AddressSanitizerCallStack](https://github.com/google/sanitizers/wiki/AddressSanitizerCallStack)

Read more about [AddressSanitizerUseAfterReturn](https://github.com/google/sanitizers/wiki/AddressSanitizerUseAfterReturn)







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



# 问题记录

## WARNING: ASan is ignoring requested __asan_handle_no_return: ... False positive error reports may follow. For details see https://github.com/google/sanitizers/issues/189

**问题描述：**

当编译开启 ASan 内存检查，并设置编译选项 `--copt=-asan-stack=1` 和运行时选项 `ASAN_OPTIONS=detect_stack_use_after_return=1` 时，在处理请求时 ASan 的处理会出现下面错误：

![asan4](/assets/images/202502/asan4.png)

**ASan 官方给出的问题参考：**

* http://code.google.com/p/address-sanitizer/issues/detail?id=189
* https://address-sanitizer.narkive.com/U0Fo97M7/issue-189-in-support-swapcontext

![asan5](/assets/images/202502/asan5.png)

在技术文档中，"w/o" 是英文 "without" 的常见缩写，表示 "没有" 或 "无需"。"the location of the stack changes w/o asan noticing it" 表示栈的位置发生改变，而没有被 ASan 检测到。

1. **问题背景**
   + ASan 通过监控内存访问来检测错误（如越界、释放后使用等）。当使用 `swapcontext` 这类手动切换协程上下文的函数时，栈指针（Stack Pointer）会被直接修改，但 ASan 无法自动感知这种变化。
2. **关键原因**
   + `swapcontext` 直接操作寄存器和栈指针，绕过了 ASan 的监控机制。
   + ASan 依赖对栈的连续性和生命周期的假设，手动切换栈会破坏这些假设，导致：
      - **Shadow Memory 污染**：ASan 可能误判整个内存区域（如 16TB 的 Shadow Memory）为需要处理的状态。
      - **性能问题**：大量 Shadow Memory 的无效操作会导致程序卡死或运行极慢。
3. **解决方案**
    + 在使用协程或手动切换栈时，需显式通知 ASan 栈的变化：通过接口 `__sanitizer_start_switch_fiber` 和 `__sanitizer_finish_switch_fiber` 明确标记栈切换的边界。

``` cpp
// 切换前：保存旧栈，准备新栈
__sanitizer_start_switch_fiber(&old_fake_stack, new_stack_base, new_stack_size);
swapcontext(&old_ctx, &new_ctx);
// 切换后：激活新栈
__sanitizer_finish_switch_fiber(new_fake_stack, nullptr, nullptr);
```

注意这两个接口必须成对调用，且遵循以下规则：

* start 在旧协程中调用：保存旧协程的 Fake Stack 状态，并为新协程准备栈范围。
* finish 在新协程中调用：激活新协程的 Fake Stack，标记其栈内存为合法。

若连续调用 start 而不切换上下文并调用 finish，会导致 ASan 内部状态混乱，可能引发 `stack_top == stack_bottom` 等断言错误。


**问题结论：**

1. 在使用协程或手动切换栈时，需显式通知 ASan 栈的变化，通过接口 `__sanitizer_start_switch_fiber` 和 `__sanitizer_finish_switch_fiber` 明确标记栈切换的边界。可参考 [support swapcontext #189](http://code.google.com/p/address-sanitizer/issues/detail?id=189)

2. 若使用的是共享栈 RunStack，在切换协程时会多一次拷贝（即将 CCoroutineData 里保存的要执行的协程堆栈拷贝到 RunStack），需要通过接口 `__asan_unpoison_memory_region` 对 RunStack 执行 Save 和 Restore 时需指定 RunStack 的内存区间为可访问的地址区间，以防止 ASan 出现误报。可参考 [AddressSanitizerManualPoisoning](https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning) 和 [Suspicious stack-overflow message that points to a valid stack range #1533](https://github.com/google/sanitizers/issues/1533)。


**__asan_poison_memory_region 和 __asan_unpoison_memory_region 接口说明**

源码参考：https://github.com/llvm/llvm-project/blob/a2ef44a5d65932c7bb0f483217826856325b60df/compiler-rt/lib/asan/asan_poisoning.cpp#L103-L171

**__sanitizer_start_switch_fiber 和 __sanitizer_finish_switch_fiber 接口说明**

源码参考：https://github.com/llvm/llvm-project/blob/a2ef44a5d65932c7bb0f483217826856325b60df/compiler-rt/lib/asan/asan_thread.cpp#L526-L551 和 https://github.com/llvm/llvm-project/blob/a2ef44a5d65932c7bb0f483217826856325b60df/compiler-rt/lib/asan/asan_thread.cpp#L125C1-L168


**Concurrent 代码参考：**

https://github.com/kurocha/concurrent/blob/663aacb14430777fc61c86c03b5eb10b8a93611c/source/Concurrent/Fiber.cpp#L46-L70

**boost 代码参考：**

https://github.com/boostorg/context/blob/master/include/boost/context/continuation_ucontext.hpp#L130

**ruby 代码参考：**

https://github.com/ruby/ruby/blob/a15e4d405ba6cafbe2f63921bd771b1241049841/cont.c#L830-L839


### 参考 [Suspicious stack-overflow message that points to a valid stack range](https://github.com/google/sanitizers/issues/1533)

This is `stack-buffer-overflow`, **not** `stack-overflow`. **Meaning read or write out of bounds of some local variable on stack**. If you are sure that this particular access is within bounds and within lifetime of whatever location it targets, then the common cause of **false positives** would be moving the stack pointer up (i.e. taking a function frame or several off the stack, like `longjmp` would) without telling asan about it. Sounds like your "breaking out" of the handler is exactly that.

If it's not done with `longjmp-in-libc` (the one that ASan intercepts), you'd need to call `__asan_unpoison` on the relevant portion of the stack.

Switching between coroutines would generally be fine with ASan. It's undeclared returns from functions (longjmp, exceptions, etc) that asan has
problems with because we rely on each function to clear the stack shadow after itself.

`__asan_unpoison_memory_region` does solve the issue.


ASan 通过在内存分配和释放时添加“毒药”（poison）来检测内存错误。当你试图访问已经被“毒化”的内存时，ASan 就会报告一个错误。在某些情况下，ASan 可能会误报错误。例如，当使用 longjmp 或其他非标准的方式来改变控制流时，ASan 可能会误认为正在访问已经被释放的内存，从而报告一个错误。这就是所谓的假阳性。`__asan_unpoison_memory_region` 是一个 ASan 提供的函数，它可以用来移除内存区域的“毒药”。当你确定一个内存访问是合法的，但 ASan 误报了错误时，你可以使用这个函数来解决问题。

`__asan_poison_memory_region` 操作的调用时机？

ASan 主要用于检测内存错误，比如缓冲区溢出、使用释放后的内存等。它通过在内存周围插入“红区”（redzones）来检测越界访问。**对于堆内存**，ASan 在分配时会自动添加红区，并在释放时标记为中毒状态。**而栈内存的处理可能有所不同，因为栈变量的生命周期由编译器管理**。当使用栈检查功能时，ASan 会在栈变量周围插入红区。这些红区在变量作用域结束时会被标记为中毒，以防止后续访问导致的问题。例如，当一个函数中的局部变量超出作用域后，ASan 可能会将对应的栈内存区域中毒，这样任何后续的访问（比如通过悬空指针）都会被检测到。具体来说，当函数返回时，栈上的局部变量会被销毁，ASan 此时会调用`__asan_poison_memory_region`来标记这些变量的内存区域为中毒状态。这样可以防止 `use-after-return` 错误。此外，在变量作用域结束的时候，比如在块结束时，ASan 也可能立即中毒该变量的内存，以检测 `use-after-scope` 问题。另外，ASan 可能在变量初始化之前就预先中毒整个栈帧，然后在变量初始化时解毒对应的区域。当变量不再使用时，再次中毒。这样可以检测到未初始化的访问或越界访问。需要注意的是，栈内存的中毒和解毒操作是由编译器插桩自动完成的，而不是显式调用。**因此，__asan_poison_memory_region的调用是在编译后的代码中自动插入的，而不是由程序员手动调用**。

总结一下，ASan 在以下情况会调用`__asan_poison_memory_region`操作来中毒栈内存：

1. 函数返回时，标记所有局部变量的内存为中毒，防止 use-after-return。
2. 局部变量离开其作用域时，立即中毒其内存，检测 use-after-scope。
3. 在变量初始化前，整个栈帧可能被预先中毒，初始化时解毒，释放时再次中毒。
4. 在栈内存分配时，插入红区并标记为中毒，以检测越界访问。

需要确认这些情况是否准确，可能需要查阅 ASan 的文档或源码。例如，use-after-scope 检测是通过在变量作用域结束时中毒其内存来实现的，而栈变量周围的红区在编译时插入，并在变量生命周期结束后被标记为中毒状态。此外，ASan 的栈保护还包括对 alloca 等动态栈分配的处理，这些情况下也会使用中毒操作来保护内存。

ASan 在栈内存管理中使用 `__asan_poison_memory_region` 的典型场景如下：

**Case1: 栈变量作用域结束**

触发条件：变量离开作用域（如函数返回、代码块结束）
目的：检测 Use-after-Scope 错误（悬空指针访问已释放的栈内存）

``` cpp
void func() {
  {
    int x[10]; // 栈变量
    ASAN_UNPOISON_MEMORY_REGION(x, sizeof(x)); // 初始化时解毒
  } // 作用域结束
  // ASan 自动调用 __asan_poison_memory_region(x, ...)
}
```

**Case2: 栈内存红区保护**

触发条件：栈帧初始化时
目的：检测缓冲区溢出（如 `buffer[32]` 访问会触及红区）

``` cpp
void func() {
  char buffer[32];
  // 编译器自动插入红区：
  // [红区][buffer][红区]
  // ASan 初始化时调用 __asan_poison_memory_region 标记红区为中毒
}
```

**Case3: 动态栈分配 alloca**

触发条件：[alloca](https://man7.org/linux/man-pages/man3/alloca.3.html) 分配的动态栈内存释放时
目的：检测动态栈内存的越界访问

``` cpp
void func(int n) {
  char *buf = (char*)alloca(n);
  // ASan 调用 __asan_poison_memory_region 标记未初始化区域
  // 并在 buf 释放时再次中毒
}
```

**Case4: 栈内存重用优化**

触发条件：编译器优化导致栈内存复用
目的：防止残留数据被错误访问

``` cpp
void func() {
  int a[100];
  // a 使用结束后...
  double b[50];
  // ASan 可能重用 a 的内存区域
  // 调用 __asan_poison_memory_region 标记 a 的旧区域
}
```

**底层实现机制：**

1. 编译器插桩：Clang/GCC 在编译时插入对 `__asan_poison_memory_region` 的调用
2. 影子内存映射：通过 `0xF1F1F1F1` 等魔数标记中毒区域
3. 作用域追踪：利用 LLVM 的 `LifetimeSanitizer` 跟踪变量生命周期

例如，对于以下代码，ASan 会在 `x` 离开作用域时标记其内存为中毒状态，后续通过 `ptr` 的访问将被拦截。

``` cpp
void test() {
  int *ptr;
  {
    int x[4];
    ptr = &x[0];
  } // __asan_poison_memory_region(x, 16) 在此处调用
  *ptr = 42; // 触发 ASan 错误
}
```


### 参考：[AddressSanitizerManualPoisoning](https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning)

A user may **poison**/**unpoison** a region of memory manually. Use this feature with caution. In many cases good old malloc+free is a better way to find heap bugs than using custom allocators with manual poisoning.

From `compiler-rt/include/sanitizer/asan_interface.h`:

``` cpp
  // Marks memory region [addr, addr+size) as unaddressable.
  // This memory must be previously allocated by the user program. Accessing
  // addresses in this region from instrumented code is forbidden until
  // this region is unpoisoned. This function is not guaranteed to poison
  // the whole region - it may poison only subregion of [addr, addr+size) due
  // to ASan alignment restrictions.
  // Method is NOT thread-safe in the sense that no two threads can
  // (un)poison memory in the same memory region simultaneously.
  void __asan_poison_memory_region(void const volatile *addr, size_t size);
  // Marks memory region [addr, addr+size) as addressable.
  // This memory must be previously allocated by the user program. Accessing
  // addresses in this region is allowed until this region is poisoned again.
  // This function may unpoison a superregion of [addr, addr+size) due to
  // ASan alignment restrictions.
  // Method is NOT thread-safe in the sense that no two threads can
  // (un)poison memory in the same memory region simultaneously.
  void __asan_unpoison_memory_region(void const volatile *addr, size_t size);

// User code should use macros instead of functions.
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#define ASAN_POISON_MEMORY_REGION(addr, size) \
  __asan_poison_memory_region((addr), (size))
#define ASAN_UNPOISON_MEMORY_REGION(addr, size) \
  __asan_unpoison_memory_region((addr), (size))
#else
#define ASAN_POISON_MEMORY_REGION(addr, size) \
  ((void)(addr), (void)(size))
#define ASAN_UNPOISON_MEMORY_REGION(addr, size) \
  ((void)(addr), (void)(size))
#endif
```

**If you have a custom allocation arena,** the typical workflow would be to poison the entire arena first, and then unpoison allocated chunks of memory leaving poisoned redzones between them. The allocated chunks should start with 8-aligned addresses.

If a [run-time flag](https://github.com/google/sanitizers/wiki/AddressSanitizerFlags) `allow_user_poisoning` is set to 0, the manual poisoning callbacks are no-ops.

源码：https://github.com/llvm-mirror/compiler-rt/blob/master/include/sanitizer/asan_interface.h


### 参考 [Boost Coroutine AddressSanitizer support](https://github.com/boostorg/coroutine/issues/30)

Q1:

Can we please add something like `<asan>on` (similar to `<valgrind>on`) which informs AddressSanitizer of the stack changes? It is described here:

https://github.com/google/sanitizers/issues/189#issuecomment-312914329
https://github.com/llvm-mirror/compiler-rt/blob/master/include/sanitizer/common_interface_defs.h#L166

A1:

already contained in boost.context (boost-1.65 release) -> BOOST_USE_ASAN
but without bjam property (will be obsolete after transission to cmake)

Q2:

I tried this, but noted that `BOOST_USE_ASAN` is used only in `boost/context/continuation_ucontext.hpp` and should therefore only be available when Boost is compiled with `context-impl=ucontext` (as opposed to `fcontext`).

However, `Boost.Coroutine` includes `boost/context/detail/fcontext.hpp` directly, which implies that it uses some `fcontext` functions and cannot fully make use of `BOOST_USE_ASAN`.

Indeed, using `BOOST_USE_ASAN` I still get an ASan warning about false positive error reports followed by an ASan error whose stack trace begins with `make_fcontext` (see below).

Can I assume that to get AddressSanitizer support I need to switch to Boost.Coroutine2? Or did I miss something?

```
==20341==WARNING: ASan is ignoring requested __asan_handle_no_return: stack top: 0x7ffffffff000; bottom 0x7fffeb7a8000; size: 0x000014857000 (344289280)
False positive error reports may follow
For details see http://code.google.com/p/address-sanitizer/issues/detail?id=189
=================================================================
==20341==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7fffeb7a9820 at pc 0x0000004477eb bp 0x7fffeb7a9650 sp 0x7fffeb7a9648
WRITE of size 4 at 0x7fffeb7a9820 thread T0
[... omitted ...]
    #17 0x454d45 in void boost::coroutines::detail::trampoline_void<boost::coroutines::detail::symmetric_coroutine_impl<void> >(boost::context::detail::transfer_t) /usr/local/include/boost/coroutine/detail/trampoline.hpp:60
    #18 0x7fffef797fae in make_fcontext (/usr/local/lib/libboost_context.so.1.65.0+0x2fae)
```

Here is a simple test case:

``` cpp
#include <exception>
#include <iostream>
#include <boost/coroutine/coroutine.hpp>

void throw_something() {
  // The stack frame of this function is not cleaned up after the throw().
  // [ASan is ignoring requested __asan_handle_no_return]
  // Make sure we have something to not clean up.
  int a[10];
  a[0] = 0;
  std::cout << a[0];

  throw std::runtime_error("hello world");
}

// The calling sequence looks as follows:
// > my_coroutine
// >> throw_something() -- stack frame not cleaned up
// >> write_to_stack
//
// If write_to_stack's stack frame is bigger than that of throw_something,
// it will contain the (still poisoned) stack frame of throw_something.
void write_to_stack() {
  constexpr size_t kArraySize = 10000;
  int a[kArraySize];
  for (size_t i = 0; i < kArraySize; i++) {
    a[i] = i;
  }
  std::cout << a[0];
}

void my_coroutine(boost::coroutines::symmetric_coroutine<void>::yield_type& yield) {
  try {
    throw_something();
  } catch(...) {
    write_to_stack();
  }
}

int main() {
  boost::coroutines::symmetric_coroutine<void>::call_type coroutine(my_coroutine);
  coroutine();
  return 0;
}
```

Compiled with `$ g++ -Wall -Werror -std=c++11 -fsanitize=address -DBOOST_USE_ASAN -DBOOST_COROUTINES_NO_DEPRECATION_WARNING -lboost_system -lboost_coroutine -o test_coroutine2_asan -O0 -gdwarf -fno-omit-frame-pointer -fno-rtti test_coroutine2.cpp` on both **g++ 6.3.0** and **g++ 7.1.1** (edit: reproducible with **clang 4.0.1** as well when changing the array size in `throw_something` to 80):

What happens is that ASan is not notified about the stack switch and therefore refuses to clean up the stack frame of throw_something (because stack top and bottom differ too much, see [here](https://github.com/clang-omp/compiler-rt_trunk/blob/93a50dbdb518d19d5120af03a8479d42ed951c48/lib/asan/asan_rtl.cc#L562)). The writes in write_to_stack then hit the poisoned region and cause the crash.

`BOOST_USE_ASAN` should enable notifying ASan about stack switches, but again it only seems to work when using ucontext.

A2:

Yes, ASAN is only supported for `ucontext` (see `continuation_ucontext.hpp`) - that's because the assembler implementation (`continuation_fcontext.hpp`) does not use global pointers, so it has no way to store restore stack bottoms.

Q3:

Thanks for the confirmation. It looks like Boost.Coroutine is using some fcontext functionality directly (see here), without the library user being given a choice to use ucontext instead. This would mean Boost.Coroutine cannot be used with ASan at the moment, correct?

A3:

`Boost.Coroutine` is deprecated - so it will never benefit from new features.
You could use `Boost.Coroutine2` instead in order to get ASAN working (re-build with `context-impl=ucontext` + `BOOST_USE_ASAN`).


### 参考：[boost Coroutine resume 代码](https://github.com/boostorg/context/blob/master/include/boost/context/continuation_ucontext.hpp#L130)


``` cpp
activation_record * resume() {
        from = current();
        // store `this` in static, thread local pointer
        // `this` will become the active (running) context
        current() = this;
#if defined(BOOST_USE_SEGMENTED_STACKS)
        // adjust segmented stack properties
        __splitstack_getcontext( from->sctx.segments_ctx);
        __splitstack_setcontext( sctx.segments_ctx);
#endif
#if defined(BOOST_USE_ASAN)
        if ( terminated) {
            __sanitizer_start_switch_fiber( nullptr, stack_bottom, stack_size);
        } else {
            __sanitizer_start_switch_fiber( & from->fake_stack, stack_bottom, stack_size);
        }
#endif
        // context switch from parent context to `this`-context
        ::swapcontext( & from->uctx, & uctx);
#if defined(BOOST_USE_ASAN)
        __sanitizer_finish_switch_fiber( current()->fake_stack,
                                         (const void **) & current()->from->stack_bottom,
                                         & current()->from->stack_size);
#endif
#if defined(BOOST_NO_CXX14_STD_EXCHANGE)
        return exchange( current()->from, nullptr);
#else
        return std::exchange( current()->from, nullptr);
#endif
}
```


### 参考：[issue: support swapcontext](https://github.com/google/sanitizers/issues/189)

> Compatible issue between ASAN and swapcontext()

* Phenomenon

ASAN does not fully support swapcontext technology, as asan has indicated in log：==1000==WARNING: ASan doesn't fully support makecontext/swapcontext functions and may produce false positives in some cases!

Under this constraint, if function swapcontext() is introduced in your program, there will be some false positives reported after coroutine was changed. The detection capability of ASAN is almost ineffective, and even seriously affects the normal operation of the program.

* Mechanism of asan

To solve this problem, we need to understand why these false positives occur.

And to understand why these false positives occur, we need to learn how asan works: **ASAN needs to allocate and store a shadow stack for each fiber, to track usage. You should also poison the stack when it’s no longer in use (e.g. if you track a high water mark, or completely free it)**.

* Way to make swapcontext() compatible with asan

Note: The flag 'ASAN_OPTIONS=detect_stack_use_after_return=true' is necessary when the swapcontext() function is used on your program.

Therefore, we need to find a way to notify ASAN before/after we exchange the fiber.

To make things easier, I recommend adding `fake_stack` pointer for every fiber when ASAN is enabled.

For this `fake_stack`:

* **The fake stack is required per context/fiber/coroutine for the purpose of tracking memory usage in ASAN.**
* **ASAN will allocate the fake stack.**

And when we try to jump to new(target) coroutine by executing swapcontext(), we need to store the `fake_stack` of old(current) fiber, so that when we try to return to the old fiber, we can restore the stack of old fiber with the `fake_stack` we ever stored before.

Here introduce two function provided by ASAN to manage the `fake_stack`:

``` cpp
// Fiber annotation interface.
// Before switching to a different stack, one must call
// __sanitizer_start_switch_fiber with a pointer to the bottom of the
// destination stack and its size. When code starts running on the new stack,
// it must call __sanitizer_finish_switch_fiber to finalize the switch.
// The start_switch function takes a void** to store the current fake stack if
// there is one (it is needed when detect_stack_use_after_return is enabled).
// When restoring a stack, this pointer must be given to the finish_switch
// function. In most cases, this void* can be stored on the stack just before
// switching. When leaving a fiber definitely, null must be passed as first
// argument to the start_switch function so that the fake stack is destroyed.
// If you do not want support for stack use-after-return detection, you can
// always pass null to these two functions.
// Note that the fake stack mechanism is disabled during fiber switch, so if a
// signal callback runs during the switch, it will not benefit from the stack
// use-after-return detection.
void __sanitizer_start_switch_fiber(void **fake_stack_save,
                                    const void *bottom, size_t size);

void __sanitizer_finish_switch_fiber(void *fake_stack_save,
                                     const void **bottom_old,
                                     size_t *size_old);
```

这两个接口是用于在支持纤程（Fiber）切换时，配合内存检测工具（如 AddressSanitizer）管理伪栈（Fake Stack）的机制。以下是它们的详细用法和设计逻辑：

**核心作用**：**当使用纤程（或协程）时，栈的切换会绕过常规的函数调用约定，导致 AddressSanitizer 无法自动追踪栈内存的使用（尤其是 detect_stack_use_after_return 功能）。这两个接口通过显式管理伪栈状态，确保内存检测的准确性**。

**`__sanitizer_start_switch_fiber` 何时调用？在切换到新纤程的栈之前调用，用于保存当前纤程的伪栈状态。**

参数解释：

`void **fake_stack_save` 用于保存当前纤程的伪栈指针。如果当前启用了 `detect_stack_use_after_return`，工具会为函数返回后的栈内存模拟一个伪栈，此参数用于存储该伪栈的指针，以便后续恢复。如果永久离开当前纤程（不再返回），需传递 `nullptr`，这会触发伪栈的销毁。如果不需要检测 `stack-use-after-return`，可直接传递 `nullptr`。

`const void *bottom, size_t size` 目标纤程的新栈的底部地址和大小。AddressSanitizer 用此信息初始化新栈的内存检测。

示例场景：

``` cpp
// 切换到新纤程前
void* saved_fake_stack = nullptr;
__sanitizer_start_switch_fiber(&saved_fake_stack, new_stack_bottom, new_stack_size);
// 保存 saved_fake_stack 到当前栈的某个位置（例如局部变量）
perform_context_switch_to_new_stack();
```

**`__sanitizer_finish_switch_fiber` 何时调用？在新栈上开始运行后立即调用，用于恢复或清理旧纤程的伪栈状态。**

参数解释：

`void *fake_stack_save` 从 `__sanitizer_start_switch_fiber` 保存的伪栈指针。如果是首次切换到某个纤程（无旧伪栈），或不需要恢复伪栈，传递 `nullptr`。

`const void **bottom_old, size_t *size_old` 可选参数，用于获取旧纤程的栈底部地址和大小。如果不需要这些信息，可传递 `nullptr`。

示例场景：

``` cpp
// 在新栈上运行后
const void* old_stack_bottom;
size_t old_stack_size;
__sanitizer_finish_switch_fiber(saved_fake_stack, &old_stack_bottom, &old_stack_size);
// 此后 AddressSanitizer 会在新栈上检测内存
```

常规纤程切换：

1. 保存旧栈状态 → 调用 start_switch_fiber。
2. 切换栈（汇编或上下文切换函数）。
3. 在新栈上调用 finish_switch_fiber，恢复或清理旧状态。

永久退出纤程：

``` cpp
// 结束当前纤程，不再返回
__sanitizer_start_switch_fiber(nullptr, new_stack_bottom, new_stack_size);
perform_context_switch_to_new_stack();
// 新栈上调用时，无需恢复旧栈
__sanitizer_finish_switch_fiber(nullptr, nullptr, nullptr);
```

禁用 `stack-use-after-return` 检测：(参考：[AddressSanitizerUseAfterReturn](https://github.com/google/sanitizers/wiki/AddressSanitizerUseAfterReturn))

``` cpp
// 所有调用均传递 nullptr，伪栈机制完全禁用
__sanitizer_start_switch_fiber(nullptr, new_stack_bottom, new_stack_size);
perform_switch();
__sanitizer_finish_switch_fiber(nullptr, nullptr, nullptr);
```

注意事项：

1. 伪栈的存储位置。fake_stack_save 应存储在旧栈上（例如局部变量），因为切换到新栈后旧栈可能被覆盖。
2. 信号处理。在切换期间伪栈机制被暂停，若信号处理函数在此期间运行，stack-use-after-return 检测可能失效。
3. 性能与兼容性。如果纤程切换不涉及栈溢出或 use-after-return 问题，可禁用伪栈以提升性能。

**底层逻辑：**

* **AddressSanitizer 为每个函数栈帧生成伪栈，防止返回后栈内存重用导致的误判。**
* **纤程切换会绕过常规栈操作，因此需手动通知工具栈的切换，确保伪栈与实际栈一致。**

通过这两个接口，开发者可以确保 AddressSanitizer 在纤程环境下仍能精确检测内存错误。





The implementation of these two function is in here: https://github.com/llvm/llvm-project/blob/a2ef44a5d65932c7bb0f483217826856325b60df/compiler-rt/lib/asan/asan_thread.cpp#L526-L551

From the source code, we can see that, `__sanitizer_start_switch_fiber` will assign the fake_stack IF and ONLY IF you provide a pointer.

This is how I handle swapcontext() issue:


``` cpp
//vthctx: Context of main fiber/coroutine
//vth:  Context of fiber/coroutine A

Step1: Try to exchange from main fiber to fiber A =========================================================================:
//On the main fiber.

//Argument0: The container for asan to allocate the fake_stack for current fiber.
//           - If we want the current fiber to stay still(we are going to jump back later),then one valid pointer(&vthctx->fake_stack here) shall be passed to argument 0 to store the fake_stack of current fiber;
//           - If we don't want to keep the current fiber alive(we won't jump back), 'NULL' shall be passed to argument 0 to notify asan to delete the fake_stack of current fiber.
//Argument1: The info of target fiber we are going to jump to.
//Argument2: The info of target fiber we are going to jump to.
__sanitizer_start_switch_fiber(&vthctx->fake_stack, vth->uctx.uc_stack.ss_sp, vth->uctx.uc_stack.ss_size);

//exchange to target fiber A.
swapcontext(&vthctx->tmp_outer_uctx, &vth->uctx);

Step2: On the trigger function of fiber A =========================================================================:
//On the fiber A
const void *from_stack;
size_t from_stacksize;

//Argument0: We are the first time to jump into this fiber, so NULL shall be set as argument 0;
//           - Set argument 0 to 'NULL' means that we have no historical stack to restore for this fiber;
//           - If we have been to this fiber and have historical stack to restore for this fiber, then set the historical stack to argument 0.
//Argument1: The container for asan to return the info of old fiber we were in before we jumped over.
//Argument2: The container for asan to return the info of old fiber we were in before we jumped over.
__sanitizer_finish_switch_fiber(NULL, &from_stack, &from_stacksize);

 Step3: jump back from fiber A to main fiber=========================================================================:
//Argument0: To store the fake_stack of old fiber before jumping out.
//           - Pass 'NULL' to argument 0 if we won't jump back to fiber A, then asan will delete the fake_stack of fiber A for us.
//           - Pass '&vth->fake_stack'to argument 0 if we plan to keep fiber A alive and we will jump back in the future，and asan will keep the fake_stack of fiber A for us.
//Argument1: The info of target fiber we are going to jump to.
//Argument2: The info of target fiber we are going to jump to.
__sanitizer_start_switch_fiber(NULL, vthctx->tmp_outer_uctx.uc_stack.ss_sp, vthctx->tmp_outer_uctx.uc_stack.ss_size);

//exchange to main fiber.
swapcontext(&vth->uctx, &vthctx->tmp_outer_uctx);

Step4: Restore the fake_stack on main fiber =========================================================================:
//At the point of the main fiber we're jumping back to

//Argument0: The fake_stack sotred before(see Step1),also the one we try to restore for this fiber.
//Argument1: The container for asan to return the info of old fiber we were in before we jumped over.
//Argument2: The container for asan to return the info of old fiber we were in before we jumped over.
__sanitizer_finish_switch_fiber(vthctx->fake_stack, &from_stack, &from_stacksize);
```

ASAN only cares about tracking the stack swapping, so as long as you wrap the stack exchange operation (coroutine transfer) correctly, ASAN should work well with swapcontext().



源码参考：sanitizer/include/common_interface_defs.h

``` cpp
/// Notify ASan that a fiber switch has started (required only if implementing
/// your own fiber library).
///
/// Before switching to a different stack, you must call
/// <c>__sanitizer_start_switch_fiber()</c> with a pointer to the bottom of the
/// destination stack and with its size. When code starts running on the new
/// stack, it must call <c>__sanitizer_finish_switch_fiber()</c> to finalize
/// the switch. The <c>__sanitizer_start_switch_fiber()</c> function takes a
/// <c>void**</c> pointer argument to store the current fake stack if there is
/// one (it is necessary when the runtime option
/// <c>detect_stack_use_after_return</c> is enabled).
///
/// When restoring a stack, this <c>void**</c> pointer must be given to the
/// <c>__sanitizer_finish_switch_fiber()</c> function. In most cases, this
/// pointer can be stored on the stack immediately before switching. When
/// leaving a fiber definitely, NULL must be passed as the first argument to
/// the <c>__sanitizer_start_switch_fiber()</c> function so that the fake stack
/// is destroyed. If your program does not need stack use-after-return
/// detection, you can always pass NULL to these two functions.
///
/// \note The fake stack mechanism is disabled during fiber switch, so if a
/// signal callback runs during the switch, it will not benefit from stack
/// use-after-return detection.
///
/// \param fake_stack_save [out] Fake stack save location.
/// \param bottom Bottom address of stack.
/// \param size Size of stack in bytes.
void __sanitizer_start_switch_fiber(void **fake_stack_save,
                                    const void *bottom, size_t size);

/// Notify ASan that a fiber switch has completed (required only if
/// implementing your own fiber library).
///
/// When code starts running on the new stack, it must call
/// <c>__sanitizer_finish_switch_fiber()</c> to finalize
/// the switch. For usage details, see the description of
/// <c>__sanitizer_start_switch_fiber()</c>.
///
/// \param fake_stack_save Fake stack save location.
/// \param bottom_old [out] Bottom address of old stack.
/// \param size_old [out] Size of old stack in bytes.
void __sanitizer_finish_switch_fiber(void *fake_stack_save,
                                     const void **bottom_old,
                                     size_t *size_old);
```

### 参考：[[asan] add primitives that allow coroutine implementations](https://reviews.llvm.org/D20913)

This patch adds the `sanitizer_start_switch_fiber` and `sanitizer_finish_switch_fiber` methods inspired from what can be found here https://github.com/facebook/folly/commit/2ea64dd24946cbc9f3f4ac3f6c6b98a486c56e73 .

These methods are needed when the compiled software needs to implement coroutines, fibers or the like. **Without a way to annotate them, when the program jumps to a stack that is not the thread stack**, `__asan_handle_no_return` shows a warning about that, and the fake stack mechanism may free fake frames that are
still in use.


### 参考：[__sanitizer_start_switch_fiber() unmaps per-thread "fake" stack #1760](https://github.com/google/sanitizers/issues/1760)

https://github.com/ruby/ruby/blob/a15e4d405ba6cafbe2f63921bd771b1241049841/cont.c#L830-L839

![asan3](/assets/images/202502/asan3.png)



![asan1](/assets/images/202502/asan1.png)

![asan2](/assets/images/202502/asan2.png)






# 单元测试 __asan_poison_memory_region

BUILD 构建脚本：

```
load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_test(
    name = "AsanTest",
    size = "small",

    srcs = glob(
        [
            "**/*.cpp",
            "**/*.cc",
            "**/*.c",
        ],
        exclude = [
        ],
    ),

    copts = [
        "-g",
        "-ggdb",
        "-fno-omit-frame-pointer",
        "-fno-optimize-sibling-calls",
        "-fsanitize=address",
        "-fsanitize-recover=address", # 设置遇到错误时继续运行
        "-fno-common",
        "-mllvm",
        "-asan-stack=1",
    ],

    linkopts = [
        "-fuse-ld=lld",
        "-fsanitize=address",
    ],

    includes = [
    ],

    deps = [
        "//unittest/utils:unittest_utils_hdrs",
        "//thirdparty/googletest:googletest",
    ],
)
```

单元测试代码：

``` cpp
#include "unittest/utils/Utils.h"
#include "gtest/gtest.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// 接口定义 compiler-rt/include/sanitizer/asan_interface.h
// https://github.com/google/sanitizers/issues/1533

#include "unittest/utils/Utils.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /// Marks a memory region (<c>[addr, addr+size)</c>) as unaddressable.
    ///
    /// This memory must be previously allocated by your program. Instrumented
    /// code is forbidden from accessing addresses in this region until it is
    /// unpoisoned. This function is not guaranteed to poison the entire region -
    /// it could poison only a subregion of <c>[addr, addr+size)</c> due to ASan
    /// alignment restrictions.
    ///
    /// \note This function is not thread-safe because no two threads can poison or
    /// unpoison memory in the same memory region simultaneously.
    ///
    /// \param addr Start of memory region.
    /// \param size Size of memory region.
    void __asan_poison_memory_region(void const volatile* addr, size_t size);

    /// Marks a memory region (<c>[addr, addr+size)</c>) as addressable.
    ///
    /// This memory must be previously allocated by your program. Accessing
    /// addresses in this region is allowed until this region is poisoned again.
    /// This function could unpoison a super-region of <c>[addr, addr+size)</c> due
    /// to ASan alignment restrictions.
    ///
    /// \note This function is not thread-safe because no two threads can
    /// poison or unpoison memory in the same memory region simultaneously.
    ///
    /// \param addr Start of memory region.
    /// \param size Size of memory region.
    void __asan_unpoison_memory_region(void const volatile* addr, size_t size);

    /// Checks if an address is poisoned.
    ///
    /// Returns 1 if <c><i>addr</i></c> is poisoned (that is, 1-byte read/write
    /// access to this address would result in an error report from ASan).
    /// Otherwise returns 0.
    ///
    /// \param addr Address to check.
    ///
    /// \retval 1 Address is poisoned.
    /// \retval 0 Address is not poisoned.
    int __asan_address_is_poisoned(void const volatile* addr);

    /// Checks if a region is poisoned.
    ///
    /// If at least one byte in <c>[beg, beg+size)</c> is poisoned, returns the
    /// address of the first such byte. Otherwise returns 0.
    ///
    /// \param beg Start of memory region.
    /// \param size Start of memory region.
    /// \returns Address of first poisoned byte.
    void* __asan_region_is_poisoned(void* beg, size_t size);

#ifdef __cplusplus
}  // extern "C"
#endif

static const char* kUseAfterPoisonErrorMessage = "use-after-poison";

#define GOOD_ACCESS(ptr, offset) EXPECT_FALSE(__asan_address_is_poisoned(ptr + offset))
#define BAD_ACCESS(ptr, offset) EXPECT_TRUE(__asan_address_is_poisoned(ptr + offset))

// 验证 ASan 的手动内存标记接口在边界条件、错误检测等方面的正确性
TEST(AddressSanitizerInterface, SimplePoisonMemoryRegionTest)
{
    // 分配 120 字节内存
    char* array = Ident((char*)malloc(120));

    // 使用 __asan_poison_memory_region 将偏移 40-80 字节标记为中毒（禁止访问）
    // poison array[40..80)
    __asan_poison_memory_region(array + 40, 40);

    // 未中毒区域允许访问
    GOOD_ACCESS(array, 39);
    GOOD_ACCESS(array, 80);

    // 中毒区域禁止访问
    BAD_ACCESS(array, 40);
    BAD_ACCESS(array, 60);
    BAD_ACCESS(array, 79);

    // 验证访问中毒内存会触发崩溃，预期错误消息为 "use-after-poison"
    char value;
    EXPECT_DEATH(value = Ident(array[40]), kUseAfterPoisonErrorMessage);

    // 解毒验证
    __asan_unpoison_memory_region(array + 40, 40);

    // 再次验证原中毒区域可正常访问
    // access previously poisoned memory.
    GOOD_ACCESS(array, 40);
    GOOD_ACCESS(array, 79);

    free(array);
}

// 验证
// 1. 非连续区域的中毒
// 2. 重叠中毒操作
// 3. 部分解毒操作
TEST(AddressSanitizerInterface, OverlappingPoisonMemoryRegionTest)
{
    char* array = Ident((char*)malloc(120));

    // Poison [0..40) and [80..120)
    __asan_poison_memory_region(array, 40);
    __asan_poison_memory_region(array + 80, 40);

    BAD_ACCESS(array, 20);
    GOOD_ACCESS(array, 60);
    BAD_ACCESS(array, 100);

    // Poison whole array - [0..120)
    __asan_poison_memory_region(array, 120);

    BAD_ACCESS(array, 60);

    // Unpoison [24..96)
    __asan_unpoison_memory_region(array + 24, 72);

    BAD_ACCESS(array, 23);
    GOOD_ACCESS(array, 24);
    GOOD_ACCESS(array, 60);
    GOOD_ACCESS(array, 95);
    BAD_ACCESS(array, 96);

    free(array);
}
```

测试输出结果：

```
$ bazel test //unittest/asan/...
INFO: Analyzed target //unittest/asan:AsanTest (1 packages loaded, 2 targets configured).
INFO: Found 1 test target...
INFO: From Linking unittest/asan/AsanTest:
/bin/ld.gold: warning: Cannot export local symbol '__asan_extra_spill_area'
/bin/ld.gold: warning: Cannot export local symbol '__lsan_current_stage'
Target //unittest/asan:AsanTest up-to-date:
  bazel-bin/unittest/asan/AsanTest
INFO: Elapsed time: 2.272s, Critical Path: 2.15s
INFO: 4 processes: 1 internal, 3 local.
INFO: Build completed successfully, 4 total actions
//unittest/asan:AsanTest                                                 PASSED in 0.6s
```

```
$ bazel-bin/unittest/asan/AsanTest
Running main() from /thirdparty/googletest-1.15.2/googletest/src/gtest_main.cc
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from AddressSanitizerInterface
[ RUN      ] AddressSanitizerInterface.SimplePoisonMemoryRegionTest
[       OK ] AddressSanitizerInterface.SimplePoisonMemoryRegionTest (588 ms)
[ RUN      ] AddressSanitizerInterface.OverlappingPoisonMemoryRegionTest
[       OK ] AddressSanitizerInterface.OverlappingPoisonMemoryRegionTest (0 ms)
[----------] 2 tests from AddressSanitizerInterface (588 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (589 ms total)
[  PASSED  ] 2 tests.
```






# Refer

* 源码 https://github.com/llvm-mirror/compiler-rt/tree/master/include/sanitizer
* https://github.com/google/sanitizers/wiki/
* https://www.usenix.org/system/files/conference/atc12/atc12-final39.pdf
* [No more leaks with sanitize flags in gcc and clang](https://lemire.me/blog/2016/04/20/no-more-leaks-with-sanitize-flags-in-gcc-and-clang/)
* [Building better software with better tools: sanitizers versus valgrind](https://lemire.me/blog/2019/05/16/building-better-software-with-better-tools-sanitizers-versus-valgrind/)
* [How to use gcc with fsanitize=address?](https://stackoverflow.com/questions/58262749/how-to-use-gcc-with-fsanitize-address)
* http://gavinchou.github.io/experience/summary/syntax/gcc-address-sanitizer/
* [All about LeakSanitizer](https://maskray.me/blog/2023-02-12-all-about-leak-sanitizer)





