---
layout: post
title:  "AddressSanitizer (ASan) - A Fast Memory Error Detector"
date:   2021-10-15 08:30:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}

# AddressSanitizer (ASan)

[AddressSanitizer page at clang.llvm.org](https://clang.llvm.org/docs/AddressSanitizer.html)

`AddressSanitizer` is a fast memory error detector. It consists of a compiler instrumentation module and a run-time library. The tool can detect the following types of bugs:

* Out-of-bounds accesses to heap, stack and globals
* Use-after-free
* Use-after-return (clang flag `-fsanitize-address-use-after-return=(never|runtime|always)` default: `runtime`). Enable runtime with: `ASAN_OPTIONS=detect_stack_use_after_return=1`
* Use-after-scope (clang flag `-fsanitize-address-use-after-scope`)
* Double-free, invalid free
* Memory leaks (experimental)

Typical slowdown introduced by AddressSanitizer is `2x`.

The tool consists of a compiler instrumentation module (currently, an `LLVM` pass) and a run-time library which replaces the `malloc` function.

AddressSanitizer is a part of `LLVM` starting with version `3.1` and a part of `GCC` starting with version `4.8` If you prefer to build from source, see [AddressSanitizerHowToBuild](https://github.com/google/sanitizers/wiki//AddressSanitizerHowToBuild).


* [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer)
  + [Use after free](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleUseAfterFree) (dangling pointer dereference)
  + [Heap buffer overflow](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleHeapOutOfBounds)
  + [Stack buffer overflow](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleStackOutOfBounds)
  + [Global buffer overflow](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleGlobalOutOfBounds)
  + [Use after return](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleUseAfterReturn)
  + [Use after scope](https://github.com/google/sanitizers/wiki//AddressSanitizerExampleUseAfterScope)
  + [Initialization order bugs](https://github.com/google/sanitizers/wiki//AddressSanitizerInitializationOrderFiasco)
  + [Memory leaks](https://github.com/google/sanitizers/wiki//AddressSanitizerLeakSanitizer)

## Usage (LLVM/Clang)

Simply compile and link your program with `-fsanitize=address` flag. The AddressSanitizer run-time library should be linked to the final executable, so make sure to use `clang` (not `ld`) for the final link step. When linking shared libraries, the AddressSanitizer run-time is not linked, so `-Wl,-z,defs` may cause link errors (don’t use it with AddressSanitizer). To get a reasonable performance add `-O1` or higher. To get nicer stack traces in error messages add `-fno-omit-frame-pointer`. To get perfect stack traces you may need to disable inlining (just use `-O1`) and tail call elimination (`-fno-optimize-sibling-calls`).

Note: [Clang 3.1 release uses another flag syntax](http://llvm.org/releases/3.1/tools/clang/docs/AddressSanitizer.html).


使用步骤：

1. 安装依赖 sudo yum install libasan
2. 配置编译选项，建议选项
	+ -O1 禁止优化
	+ -fno-omit-frame-pointer 不忽略堆栈信息
	+ -fsanitize=address 包括 LeakSanitizer MemorySanitizer 功能
	+ -fsanitize-recover=address 检查出错不退出继续执行
3. export LD_PRELOAD=/lib64/libasan.so.0 启用lib库的预加载
4. export ASAN_OPTIONS=halt_on_error=0 通过环境变量配置 Sanitizers，遇到错误不停止运行
5. 启动程序，观察stdout的输出

注意：LeakSanitizer 的检测结果，默认是在程序退出前打印出来的。

因此对于后台允许的Server方式，可以在代码里定期调用 LeakSanitizer 接口输出：

``` cpp
#include <thread>
#include "sanitizer/lsan_interface.h"

int main(int argc, char** argv) 
{
    // ...

    // 定期采集
    std::thread check([]() {
    while (true) {
	    __lsan_do_recoverable_leak_check();    // 此处打印检测结果到stdout
	    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }});

}
```

简单的例子：

``` bash
% cat example_UseAfterFree.cc
int main(int argc, char **argv) {
  int *array = new int[100];
  delete [] array;
  return array[argc];  // BOOM
}

# Compile and link
% clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer example_UseAfterFree.cc

# or

# Compile
% clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer -c example_UseAfterFree.cc
# Link
% clang++ -g -fsanitize=address example_UseAfterFree.o
```

If a bug is detected, the program will print an error message to `stderr` and exit with a non-zero exit code. AddressSanitizer exits on the first detected error. This is by design:

* This approach allows AddressSanitizer to produce faster and smaller generated code (both by ~5%).
* Fixing bugs becomes unavoidable. AddressSanitizer does not produce false alarms. Once a memory corruption occurs, the program is in an inconsistent state, which could lead to confusing results and potentially misleading subsequent reports.

## Symbolizing the Reports

See: https://clang.llvm.org/docs/AddressSanitizer.html#id4

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

## Issue Suppression

AddressSanitizer is not expected to produce false positives. If you see one, look again; most likely it is a true positive!

### Suppressing Reports in External Libraries

Runtime interposition allows AddressSanitizer to find bugs in code that is not being recompiled. If you run into an issue in external libraries, we recommend immediately reporting it to the library maintainer so that it gets addressed. However, you can use the following suppression mechanism to unblock yourself and continue on with the testing. 

See: https://clang.llvm.org/docs/AddressSanitizer.html#id10

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

### Suppressing memory leaks

Memory leak reports produced by [LeakSanitizer](https://clang.llvm.org/docs/LeakSanitizer.html) (if it is run as a part of AddressSanitizer) can be suppressed by a separate file passed as

``` bash
LSAN_OPTIONS=suppressions=MyLSan.supp
```

which contains lines of the form `leak:<pattern>`. Memory leak will be suppressed if pattern matches any function name, source file name, or library name in the symbolized stack trace of the leak report. See [full documentation](https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer#suppressions) for more details.


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

## More Information

See: https://github.com/google/sanitizers/wiki/AddressSanitizer

# AddressSanitizer Comparison Of MemoryTools

See: [AddressSanitizerComparisonOfMemoryTools](https://github.com/google/sanitizers/wiki/AddressSanitizerComparisonOfMemoryTools/d06210f759fec97066888e5f27c7e722832b0924)

# AddressSanitizer Algorithm

[AddressSanitizerAlgorithm](https://github.com/google/sanitizers/wiki//AddressSanitizerAlgorithm) 

Short version:

The run-time library replaces the `malloc` and `free` functions. The memory around malloc-ed regions (red zones) is poisoned. The free-ed memory is placed in quarantine and also poisoned. Every memory access in the program is transformed by the compiler in the following way:

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

# GCC 

[GCC -fsanitize=address](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#Instrumentation-Options)

-fsanitize=address

Enable AddressSanitizer, a fast memory error detector. Memory access instructions are instrumented to detect out-of-bounds and use-after-free bugs. The option enables -fsanitize-address-use-after-scope. See https://github.com/google/sanitizers/wiki/AddressSanitizer for more details. The run-time behavior can be influenced using the ASAN_OPTIONS environment variable. When set to help=1, the available options are shown at startup of the instrumented program. See https://github.com/google/sanitizers/wiki/AddressSanitizerFlags#run-time-flags for a list of supported options. The option cannot be combined with -fsanitize=thread or -fsanitize=hwaddress. Note that the only target -fsanitize=hwaddress is currently supported on is AArch64.

测试代码：

```
yum install libasan

gcc -g -fsanitize=address -fno-omit-frame-pointer demo.c 
```

![gcc_sanitize](/assets/images/202106/gcc_sanitize.png)

## Clang

```
# 设置编译选项
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak"")
# 设置链接选项
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=leak")
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


# Memory Benchmark

* [Testing Memory Allocators: ptmalloc2 vs tcmalloc vs hoard vs jemalloc While Trying to Simulate Real-World Loads](http://ithare.com/testing-memory-allocators-ptmalloc2-tcmalloc-hoard-jemalloc-while-trying-to-simulate-real-world-loads/)
* [Mimalloc-bench](https://github.com/daanx/mimalloc-bench)


# Refer 

* https://github.com/google/sanitizers/wiki/
* [No more leaks with sanitize flags in gcc and clang](https://lemire.me/blog/2016/04/20/no-more-leaks-with-sanitize-flags-in-gcc-and-clang/)
* [Building better software with better tools: sanitizers versus valgrind](https://lemire.me/blog/2019/05/16/building-better-software-with-better-tools-sanitizers-versus-valgrind/)
* [How to use gcc with fsanitize=address?](https://stackoverflow.com/questions/58262749/how-to-use-gcc-with-fsanitize-address)



  

	
	