---
layout: post
title:  "TCMalloc in Action"
date:   2022-08-07 11:00:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}

`TCMalloc` is Google's customized implementation of C's `malloc()` and C++'s `operator new` used for memory allocation within our C and C++ code. **TCMalloc is a fast, multi-threaded malloc implementation**.


# Prerequisites

## Install Bazel (CentOS)

[Bazel 安装说明](https://bazel.build/install)，本文使用方式三。

方式一：安装包

```
yum install bazel4
```

方式二：[源码编译](https://bazel.build/install/compile-source)

方式三：[使用release版本](https://github.com/bazelbuild/bazel/releases)

## Install python3

```
yum install -y python36
```

## Install java

```
yum install -y java-1.8.0-openjdk-devel.x86_64
```

> 如果需要运行java程序，只需安装JRE (Java Runtime Environment)。如果需要编写java程序，需要安装JDK (Java Development Kit)。
>
> JRE顾名思义是java运行时环境，包含了java虚拟机，java基础类库。是使用java语言编写的程序运行所需要的软件环境，是提供给想运行java程序的用户使用的。
>
> JDK顾名思义是java开发工具包，是程序员使用java语言编写java程序所需的开发工具包，是提供给程序员使用的。JDK包含了JRE，同时还包含了编译java源码的编译器javac，还包含了很多java程序调试和分析的工具：jconsole，jvisualvm等工具软件，还包含了java程序编写所需的文档和demo例子程序。

## Language Requirements

[Bazel](https://bazel.build/) is the official build system for `TCMalloc`.

`TCMalloc` requires a code base that supports C++17 and our code is C++17-compliant. C code is required to be compliant to C11.

We guarantee that our code will compile under the following compilation flags:

Linux:

gcc 9.2+, clang 9.0+: `-std=c++17`

(TL;DR; All code at this time must be built under `C++17`. We will update this list if circumstances change.)

To specify which C/C++ compiler the default C++ toolchain in Bazel should use set CC environment variable (e.g. CC=clang bazel build //...).


* [How to use clang++ instead g++ in Bazel](https://stackoverflow.com/questions/41356173/how-to-use-clang-instead-g-in-bazel)

# tcmalloc 与 gperftools 的关系

gperftools 是从一个更早的实现中分叉出来的，这个早期实现曾经是外包的，后来独立发展。gperftools 代表了 Google 内部实现的一个较新的快照（不仅仅是一个分叉，而且是主动同步的）。

参考链接：https://abseil.io/blog/20200212-tcmalloc 和 https://google.github.io/tcmalloc/overview.html



# 单独构建 tcmalloc

```
cd ~/github
git clone https://github.com/google/tcmalloc.git
cd tcmalloc
CC=clang bazel test //tcmalloc/...
```

编译构建输出：

```
WARNING: Ignoring JAVA_HOME, because it must point to a JDK, not a JRE.
WARNING: Running Bazel server needs to be killed, because the startup options are different.
Starting local Bazel server and connecting to it...
INFO: Analyzed 394 targets (83 packages loaded, 3102 targets configured).
INFO: Found 143 targets and 251 test targets...
INFO: Elapsed time: 310.730s, Critical Path: 175.06s
INFO: 4009 processes: 1798 internal, 2211 processwrapper-sandbox.
INFO: Build completed successfully, 4009 total actions
//tcmalloc:arena_test                                                    PASSED in 0.2s
//tcmalloc:central_freelist_test                                         PASSED in 45.0s
...

//tcmalloc/testing:threadcachesize_test_small_but_slow                   PASSED in 6.1s
//tcmalloc:profile_test                                                  PASSED in 86.3s
  Stats over 2 runs: max = 86.3s, min = 49.4s, avg = 67.9s, dev = 18.5s
//tcmalloc:transfer_cache_test                                           PASSED in 25.8s
  Stats over 3 runs: max = 25.8s, min = 10.7s, avg = 18.0s, dev = 6.2s

Executed 251 out of 251 tests: 251 tests pass.
INFO: Build completed successfully, 4009 total actions
```

Congratulations! You've installed TCMalloc

# 通过构建 gperftools 得到 tcmalloc

实际使用 tcmalloc 可以通过安装 [gperftools](https://github.com/gperftools/gperftools/blob/master/INSTALL) 工具会包含 tcmalloc 库。


禁用 libunwind:

```
./configure --prefix=/data/home/gerryyang/tools/gperf/gperftools-2.10-install-gcc485  --enable-frame-pointers
```

使用 libunwind:

```
LDFLAGS="-L/data/home/gerryyang/tools/libunwind/libunwind-1.5.0-install-gcc485/lib" CPPFLAGS="-I/data/home/gerryyang/tools/libunwind/libunwind-1.5.0-install-gcc485/include" ./configure --prefix=/data/home/gerryyang/tools/gperf/gperftools-2.10-install-gcc485 --enable-libunwind
```

## 构建方法优化

可能需要考虑以下几点来优化构建过程：

* 优化编译选项：为了在生产环境中获得最佳性能，可以使用高优化级别编译 tcmalloc。例如，可以使用 -O3 优化级别。要设置编译选项，可以在构建命令中添加 CFLAGS 和 CXXFLAGS：

```
CFLAGS="-O3" CXXFLAGS="-O3" ./configure --prefix=/data/home/gerryyang/tools/gperf/gperftools-2.10-install-gcc485 --enable-frame-pointers
```

* 禁用不必要的功能：在生产环境中，你可能不需要 tcmalloc 的一些诊断功能，如内存泄漏检测、CPU 分析器和堆分析器。确保在构建命令中不包含这些功能的启用选项。在 gperftools 的构建过程中，默认情况下是不启用 CPU 分析器、堆分析器、堆检查器和 debugalloc 的。因此，通常情况下，不需要显式地添加这些禁用选项。

* 适应目标平台：确保使用与生产环境相匹配的编译器和库版本构建 tcmalloc。这有助于确保二进制文件在目标平台上的兼容性和性能。

* 测试：在将 tcmalloc 部署到生产环境之前，务必在类似的测试环境中进行充分的测试。这包括性能测试、压力测试和功能测试。这有助于确保在生产环境中没有意外的问题。

综上所述，可以使用以下命令进行构建：

```
CFLAGS="-O3" CXXFLAGS="-O3" ./configure --prefix=/data/home/gerryyang/tools/gperf/gperftools-2.10-install-gcc485 --enable-frame-pointers --disable-cpu-profiler --disable-heap-profiler --disable-heap-checker --disable-debugalloc
```

在构建 gperftools 以用于生产环境时，除了前面提到的选项外，还可以考虑以下选项：

* --enable-minimal：这个选项可以用于构建一个最小功能的 tcmalloc 库，去除了 CPU 分析器、堆分析器和内存泄漏检测器。这可以减小库的大小并提高性能。请注意，这个选项会覆盖前面提到的 --disable-cpu-profiler、--disable-heap-profiler 和 --disable-heap-checker 选项。

* --enable-shared 和 --disable-static：这两个选项可以用于构建共享库（.so 文件）而不是静态库（.a 文件）。在某些情况下，使用共享库可能有助于减小可执行文件的大小并提高加载速度。但请注意，使用共享库可能会导致部署和兼容性问题。

* --with-pic：这个选项用于构建位置无关代码（PIC）。如果你计划将 tcmalloc 作为共享库链接到其他程序中，这个选项可能是必需的。

* --enable-libunwind：如果你想使用 libunwind 来获取更准确和更快速的堆栈回溯，可以启用此选项。但请注意，这可能会导致与其他库的冲突和兼容性问题。在启用此选项之前，请确保在你的环境中进行了充分的测试。


综上所述，你可以根据需要组合这些选项。例如，一个用于生产环境的完整构建命令可能如下所示：

动态库：

```
CFLAGS="-O3" CXXFLAGS="-O3" ./configure --prefix=/data/home/gerryyang/tools/gperf/gperftools-2.10-install-gcc485 --enable-frame-pointers --disable-cpu-profiler --disable-heap-profiler --disable-heap-checker --disable-debugalloc --enable-minimal --enable-shared --disable-static --with-pic
```

静态库：

```
CFLAGS="-O3" CXXFLAGS="-O3" ./configure --prefix=/data/home/gerryyang/tools/gperf/gperftools-2.10-install-gcc485 --enable-frame-pointers --disable-cpu-profiler --disable-heap-profiler --disable-heap-checker --disable-debugalloc --enable-minimal --with-pic
```

完整的功能：

```
CFLAGS="-O3" CXXFLAGS="-O3" ./configure --prefix=/data/home/gerryyang/tools/gperf/gperftools-2.10-install-gcc485 --enable-frame-pointers --with-pic
```




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

参考：https://github.com/gperftools/gperftools/blob/gperftools-2.10/INSTALL 其中 NOTE FOR 64-BIT LINUX SYSTEMS 部署说明。

# Running the TCMalloc Hello World

Once you've verified you have TCMalloc installed correctly, you can compile and run the [tcmalloc-hello](https://github.com/google/tcmalloc/blob/master/tcmalloc/testing/hello_main.cc) sample binary to see how TCMalloc is linked into a sample binary. This tiny project features proper configuration and a simple `hello_main` to demonstrate how TCMalloc works.

``` cpp
#include <iostream>
#include <memory>
#include <string>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/types/optional.h"
#include "tcmalloc/malloc_extension.h"

int main(int argc, char** argv) {
  std::string msg = absl::StrCat("hello ", argc < 2 ? "world" : argv[1], "!");

  absl::optional<size_t> heap_size =
      tcmalloc::MallocExtension::GetNumericProperty(
          "generic.current_allocated_bytes");
  if (heap_size.has_value()) {
    std::cout << "Current heap size = " << *heap_size << " bytes" << std::endl;
  }

  std::cout << msg << std::endl;

  // Allocate memory, printing the pointer to deter an optimizing compiler from
  // eliding the allocation. (防止编译器优化)
  constexpr size_t kSize = 1024 * 1024 * 1024;
  std::unique_ptr<char[]> ptr(new char[kSize]); // 动态内存分配

  std::cout << absl::StreamFormat("new'd %d bytes at %p\n", kSize, ptr.get());

  heap_size = tcmalloc::MallocExtension::GetNumericProperty(
      "generic.current_allocated_bytes");
  if (heap_size.has_value()) {
    std::cout << "Current heap size = " << *heap_size << " bytes" << std::endl;
  }

  void* ptr2 = malloc(kSize); // 动态内存分配
  std::cout << absl::StreamFormat("malloc'd %d bytes at %p\n", kSize, ptr2);

  heap_size = tcmalloc::MallocExtension::GetNumericProperty(
      "generic.current_allocated_bytes");
  if (heap_size.has_value()) {
    std::cout << "Current heap size = " << *heap_size << " bytes" << std::endl;
  }

  free(ptr2);
}
```

First, build the `tcmalloc/testing:hello_main` target:

```
~/github/tcmalloc$CC=clang bazel build tcmalloc/testing:hello_main
WARNING: Ignoring JAVA_HOME, because it must point to a JDK, not a JRE.
INFO: Analyzed target //tcmalloc/testing:hello_main (1 packages loaded, 114 targets configured).
INFO: Found 1 target...
Target //tcmalloc/testing:hello_main up-to-date:
  bazel-bin/tcmalloc/testing/hello_main
INFO: Elapsed time: 10.843s, Critical Path: 6.07s
INFO: 87 processes: 2 internal, 85 processwrapper-sandbox.
INFO: Build completed successfully, 87 total actions
```

Now, run the compiled program:

```
~/github/tcmalloc$CC=clang bazel run tcmalloc/testing:hello_main
WARNING: Ignoring JAVA_HOME, because it must point to a JDK, not a JRE.
INFO: Analyzed target //tcmalloc/testing:hello_main (1 packages loaded, 114 targets configured).
INFO: Found 1 target...
Target //tcmalloc/testing:hello_main up-to-date:
  bazel-bin/tcmalloc/testing/hello_main
INFO: Elapsed time: 6.460s, Critical Path: 3.23s
INFO: 50 processes: 2 internal, 48 processwrapper-sandbox.
INFO: Build completed successfully, 50 total actions
INFO: Build completed successfully, 50 total actions
Current heap size = 73728 bytes
hello world!
new'd 1073741824 bytes at 0x463880000000
Current heap size = 1073816576 bytes
malloc'd 1073741824 bytes at 0x4638c0000000
Current heap size = 2147558400 bytes
```

You can inspect this code within [tcmalloc/testing/hello_main.cc](https://github.com/google/tcmalloc/blob/master/tcmalloc/testing/hello_main.cc)

# Creating and Running TCMalloc

Now that you've obtained the TCMalloc code and verified that you can build, test, and run it, you're ready to use it within your own project.

## Linking Your Code to the TCMalloc Repository


## Adding Abseil

TCMalloc requires [Abseil](https://abseil.io/) which you will also need to provide as a `local_repository`, or link to a specific commit (we always recommend the latest commit) using an `http_archive` declaration in the `WORKSPACE` file:

## Creating Your Test Code

## Creating Your BUILD File



Congratulations! You've created your first binary using TCMalloc.


# TCMalloc : Thread-Caching Malloc

TCMalloc is a memory allocator designed as an alternative to the system default allocator that has the following characteristics:

* Fast, uncontended allocation and deallocation for most objects. Objects are cached, depending on mode, either per-thread, or per-logical-CPU. Most allocations do not need to take locks, so there is low contention and good scaling for multi-threaded applications.
* Flexible use of memory, so freed memory can be reused for different object sizes, or returned to the OS.
* Low per object memory overhead by allocating "pages" of objects of the same size. Leading to space-efficient representation of small objects.
* Low overhead sampling, enabling detailed insight into applications memory usage.

The following block diagram shows the rough internal structure of TCMalloc:

![tcmalloc-overview](/assets/images/202209/tcmalloc-overview.png)

We can break TCMalloc into three components. The front-end, middle-end, and back-end. We will discuss these in more details in the following sections. A rough breakdown of responsibilities is:

* The front-end is a cache that provides fast allocation and deallocation of memory to the application.
* The middle-end is responsible for refilling the front-end cache.
* The back-end handles fetching memory from the OS.

Note that the front-end can be run in either per-CPU or legacy per-thread mode, and the back-end can support either the hugepage aware pageheap or the legacy pageheap.

See more: https://github.com/google/tcmalloc/blob/master/docs/design.md



# 分配原理

![tcmalloc-ds](/assets/images/202209/tcmalloc-ds.png)

## 定长分配 (使用 freelist 进行对象分配)

假设 Page 的内存大小为 4KB，需要以 16 字节为单位进行分配。通过 freelist 的方式，将 4KB 划分为 16 字节的 Object，每个单元的前 8 个字节作为节点的指针，指向下一个单元。分配时，从链表头分配一个 Object 出去，释放时，将 Object 再插入到链表中。

## 变长分配

扩展为多种定长分配的方式。

把所有的变长记录进行“取整”，例如分配 7 字节，就分配 8 字节，31 字节分配 32 字节，得到多种规格的定长记录。这里带来了内部内存碎片的问题，即分配出去的空间不会被完全利用，有一定浪费。为了减少内部碎片，分配规则按照 8, 16, 32, 48, 64, 80 这样子来。注意到，这里并不是简单地使用 2 的幂级数，因为按照 2 的幂级数，内存碎片会相当严重，分配 65 字节，实际会分配 128 字节，接近 50% 的内存碎片。而按照这里的分配规格，只会分配 80 字节，一定程度上减轻了问题。

## 大对象的分配

多个连续的 Page 会组成一个 Span，在 Span 中记录起始 Page 的编号，以及 Page 的数量。分配对象时，小的对象从 Span 分配，大的对象直接分配 Span。(分级的思想)

## Span 的管理

使用类似的方案，用多种定长 Page 来实现变长 Page 的分配。

![tcmalloc-ds2](/assets/images/202209/tcmalloc-ds2.png)

Span 到 Page 的映射：由于 Span 中记录了起始的 Page，因而就知道了从 Span 到 Page 的映射。
Page 到 Span 的映射：使用 RadixTree 来实现 PageMap，记录 Page 所属的 Span。

从而，通过伙伴系统，实现 Span 的分裂与合并。

## 全局对象分配

有了基于 Page 的对象分配，以及 Page 的管理，从而可以得到一个简单的内存分配器了。

每种规格的对象，都有一个独立的内存分配单元 CentralCache。通过链表把所有 Span 组织起来，每次需要分配时，就找一个 Span 从中分配一个 Object。当没有空闲的 Span 时，就从 PageHeap 申请 Span。

但是，当在多线程场景下，所有线程都从 CentralCache 分配，竞争比较激烈。

## ThreadCache

每一个线程有一个 ThreadCache。ThreadCache 从 CentralCache 批量申请和释放内存。

每个线程持有一个线程局部的 ThreadCache，按照不同的对象规格，维护了对象的链表。如果 ThreadCache 的对象不够了，就从 CentralCache 批量分配；如果 CentralCache 依然没有，就从 PageHeap 申请 Span；如果 PageHeap 也没有合适的 Page，就只能从操作系统申请了。

在释放内存的时候，采用相反的顺序。ThreadCache 遵循批量释放的策略，当对象积累到一定程度时释放给 CentralCache；当 CentralCache 发现一个 Span 的内存完全释放了，就把这个 Span 归还给 PageHeap；当 PageHeap 发现一批连续的 Page 都释放了，就归还给操作系统。


# Understanding Malloc Stats

Human-readable statistics can be obtained by calling `tcmalloc::MallocExtension::GetStats()`.

The output contains a lot of information. Much of it can be considered debug info that's interesting to folks who are passingly familiar with the internals of TCMalloc, but potentially not that useful for most people.

## Summary Section

The most generally useful section is the first few lines:

```
See https://github.com/google/tcmalloc/tree/master/docs/stats.md for an explanation of this page
------------------------------------------------
MALLOC:    10858234672 (10355.2 MiB) Bytes in use by application
MALLOC: +    827129856 (  788.8 MiB) Bytes in page heap freelist
MALLOC: +    386098400 (  368.2 MiB) Bytes in central cache freelist
MALLOC: +    105330688 (  100.5 MiB) Bytes in per-CPU cache freelist
MALLOC: +      9095680 (    8.7 MiB) Bytes in transfer cache freelist
MALLOC: +       660976 (    0.6 MiB) Bytes in thread cache freelists
MALLOC: +     49333930 (   47.0 MiB) Bytes in malloc metadata
MALLOC: +       629440 (    0.6 MiB) Bytes in malloc metadata Arena unallocated
MALLOC: +      1599704 (    1.5 MiB) Bytes in malloc metadata Arena unavailable
MALLOC:   ------------
MALLOC: =  12238113346 (11671.2 MiB) Actual memory used (physical + swap)
MALLOC: +    704643072 (  672.0 MiB) Bytes released to OS (aka unmapped)
MALLOC:   ------------
MALLOC: =  12942756418 (12343.2 MiB) Virtual address space used
```

* **Bytes in use by application**: Number of bytes that the application is actively using to hold data. This is computed by the bytes requested from the OS minus any bytes that are held in caches and other internal data structures.
* **Bytes in page heap freelist**: The pageheap is a structure that holds memory ready for TCMalloc to use. This memory is not actively being used, and could be returned to the OS. See [TCMalloc tuning](https://github.com/google/tcmalloc/blob/master/docs/tuning.md)
* **Bytes in central cache freelist**: This is the amount of memory currently held in the central freelist. This is a structure that holds partially used "[spans](https://github.com/google/tcmalloc/blob/master/docs/stats.md#more-detail-on-metadata)" of memory. The spans are partially used because some memory has been allocated from them, but not entirely used - since they have some free memory on them.
* **Bytes in per-CPU cache freelist**: In per-cpu mode (which is the default) each CPU holds some memory ready to quickly hand to the application. The maximum size of this per-cpu cache is tunable. See [TCMalloc tuning](https://github.com/google/tcmalloc/blob/master/docs/tuning.md)
* **Bytes in transfer cache freelist**: The transfer cache can be considered another part of the central freelist. It holds memory that is ready to be provided to the application for use.
* **Bytes in thread cache freelists**: The TC in TCMalloc stands for thread cache. Originally each thread held its own cache of memory to provide to the application. Since the change of default to per-cpu caches, the thread caches are used by very few applications. However, TCMalloc starts in per-thread mode, so there may be some memory left in per-thread caches from before it switches into per-cpu mode.
* **Bytes in malloc metadata**: the size of the data structures used for tracking memory allocation. This will grow as the amount of memory used grows.
* **Bytes in malloc metadata Arena unallocated**: Metadata is allocated in an internal Arena. Memory requests to the OS are made in blocks which amortize several Arena allocations and this captures memory that is not yet allocated but could be by future Arena allocations.
* **Bytes in malloc metadata Arena unavailable**: The Arena allocator may fail to allocate a block fully when a subsequent Arena allocation request is made that is larger than the block's remaining space. This memory is currently unavailable for allocation.

There's a couple of summary lines:

* **Actual memory used**: This is the total amount of memory that TCMalloc thinks it is using in the various categories. This is computed from the size of the various areas, the actual contribution to RSS may be larger or smaller than this value. The true RSS may be less if memory is not mapped in. In some cases RSS can be larger if small regions end up being mapped with huge pages. This does not count memory that TCMalloc is not aware of (eg memory mapped files, text segments etc.)
* **Bytes released to OS**: TCMalloc can release memory back to the OS (see [tcmalloc tuning](https://github.com/google/tcmalloc/blob/master/docs/tuning.md)), and this is the upper bound on the amount of released memory. However, it is up to the OS as to whether the act of releasing the memory actually reduces the RSS of the application. The code uses MADV_DONTNEED/MADV_REMOVE which tells the OS that the memory is no longer needed.
* **Virtual address space used**: This is the amount of virtual address space that TCMalloc believes it is using. This should match the later section on requested memory. There are other ways that an application can increase its virtual address space, and this statistic does not capture them.


## More Detail On Metadata

The next section gives some insight into the amount of metadata that TCMalloc is using. This is really debug information, and not very actionable.

```
MALLOC:         236176               Spans in use
MALLOC:         238709 (   10.9 MiB) Spans created
MALLOC:              8               Thread heaps in use
MALLOC:             46 (    0.0 MiB) Thread heaps created
MALLOC:          13517               Stack traces in use
MALLOC:          13742 (    7.2 MiB) Stack traces created
MALLOC:              0               Table buckets in use
MALLOC:           2808 (    0.0 MiB) Table buckets created
MALLOC:       11665416 (   11.1 MiB) Pagemap bytes used
MALLOC:        4067336 (    3.9 MiB) Pagemap root resident bytes
```

* **Spans**: structures that hold multiple pages of allocatable objects.
* **Thread heaps**: These are the per-thread structures used in per-thread mode.
* **Stack traces**: These hold metadata for each sampled object.
* **Table buckets**: These hold data for stack traces for sampled events.
* **Pagemap**: This data structure supports the mapping of object addresses to information about the objects held on the page. The pagemap root is a potentially large array, and it is useful to know how much of it is actually memory resident.


See more: https://github.com/google/tcmalloc/blob/master/docs/stats.md


# 测试代码

``` cpp
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <vector>

const int kNumAllocations = 1000000;

void test_memory_allocation(int allocation_size) {
    std::vector<void*> allocated_memory;
    allocated_memory.reserve(kNumAllocations);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < kNumAllocations; ++i) {
            void* ptr = malloc(allocation_size);
            if (ptr == nullptr) {
                    std::cerr << "Memory allocation failed" << std::endl;
                    exit(1);
            }
            allocated_memory.push_back(ptr);
    }

    for (void* ptr : allocated_memory) {
            free(ptr);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Allocation size: " << allocation_size << " bytes, "
            << "Total allocations: " << kNumAllocations << ", "
            << "Duration: " << duration << " ms" << std::endl;
}

int main() {
    test_memory_allocation(16);
    test_memory_allocation(64);
    test_memory_allocation(256);
    test_memory_allocation(1024);
    test_memory_allocation(4096);
    test_memory_allocation(1572864);
    test_memory_allocation(2044723);

    return 0;
}
```

输出结果：

```
Allocation size: 16 bytes, Total allocations: 1000000, Duration: 20 ms
Allocation size: 64 bytes, Total allocations: 1000000, Duration: 32 ms
Allocation size: 256 bytes, Total allocations: 1000000, Duration: 80 ms
Allocation size: 1024 bytes, Total allocations: 1000000, Duration: 251 ms
Allocation size: 4096 bytes, Total allocations: 1000000, Duration: 847 ms
Allocation size: 1572864 bytes, Total allocations: 1000000, Duration: 4757 ms
Allocation size: 2044723 bytes, Total allocations: 1000000, Duration: 2199 ms
```


# Q&A

## tcmalloc::allocate_full_XXX

参考 gperftools 2.7，src/tcmalloc.cc 中 do_allocate_full 接口注释：

```
// tcmalloc::allocate_full_XXX is called by fast-path malloc when some
// complex handling is needed (such as fetching object from central
// freelist or malloc sampling). It contains all 'operator new' logic,
// as opposed to malloc_fast_path which only deals with important
// subset of cases.
//
// Note that this is under tcmalloc namespace so that pprof
// can automatically filter it out of growthz/heapz profiles.
//
// We have slightly fancy setup because we need to call hooks from
// function in 'google_malloc' section and we cannot place template
// into this section. Thus 3 separate functions 'built' by macros.
//
// Also note that we're carefully orchestrating for
// MallocHook::GetCallerStackTrace to work even if compiler isn't
// optimizing tail calls (e.g. -O0 is given). We still require
// ATTRIBUTE_ALWAYS_INLINE to work for that case, but it was seen to
// work for -O0 -fno-inline across both GCC and clang. I.e. in this
// case we'll get stack frame for tc_new, followed by stack frame for
// allocate_full_cpp_throw_oom, followed by hooks machinery and user
// code's stack frames. So GetCallerStackTrace will find 2
// subsequent stack frames in google_malloc section and correctly
// 'cut' stack trace just before tc_new.
```

这段注释解释了tcmalloc库中`allocate_full_XXX`函数的用途和实现细节。这些函数在内存分配的快速路径（`fast-path`）无法处理的复杂情况下被调用。以下是对这段注释的逐句解释：

1. `tcmalloc::allocate_full_XXX`在需要处理复杂情况（如从中央空闲列表获取对象或内存分配采样）时被快速路径内存分配调用。它包含了所有operator new的逻辑，而`malloc_fast_path`只处理重要的子集情况。

这句话说明`allocate_full_XXX`函数在快速路径无法处理的情况下被调用，例如从中央空闲列表获取对象或进行内存分配采样。它包含了所有与C++的operator new相关的逻辑。

2. 请注意，这是在tcmalloc命名空间下，以便`pprof`可以自动从growthz/heapz配置文件中过滤掉。

这句话说明这些函数位于tcmalloc命名空间中，这样pprof工具可以自动地从growthz和heapz分析结果中排除这些函数。

3. 我们有一个稍微复杂的设置，因为我们需要从google_malloc部分的函数中调用钩子，而我们不能将模板放入此部分。因此，由宏构建的3个独立函数。

这句话说明实现这些函数的方式略有不同，因为它们需要在google_malloc部分中调用钩子函数。为了实现这一目标，代码使用了宏来构建3个独立的函数。

4. 另请注意，我们正在仔细安排`MallocHook::GetCallerStackTrace`，以便即使编译器不优化尾调用（例如，给出`-O0`），它也可以正常工作。我们仍然需要`ATTRIBUTE_ALWAYS_INLINE`才能在这种情况下工作，但它在`-O0 -fno-inline`的GCC和Clang中都可以正常工作。也就是说，在这种情况下，我们将获得`tc_new`的堆栈帧，然后是`allocate_full_cpp_throw_oom`的堆栈帧，然后是钩子机制和用户代码的堆栈帧。因此，`GetCallerStackTrace`将在google_malloc部分找到两个连续的堆栈帧，并在tc_new之前正确地“切断”堆栈跟踪。

这句话说明，为了让`MallocHook::GetCallerStackTrace`在不同的编译优化级别下都能正常工作，代码中采用了一些策略。这些策略确保了在不同编译器和优化设置下，`GetCallerStackTrace`都能正确地获取堆栈跟踪信息。


## 使用 tcmalloc 分配 2044723 大内存的调用堆栈

```
#0  0x00007f368db70d04 in ?? () from /lib64/libgcc_s.so.1
#1  0x00007f368db71ff9 in _Unwind_Backtrace () from /lib64/libgcc_s.so.1
#2  0x000000000040bb3a in GetStackTrace_libgcc (result=<optimized out>, max_depth=<optimized out>, skip_count=<optimized out>) at src/stacktrace_libgcc-inl.h:100
#3  0x000000000040c1e4 in GetStackTrace (result=result@entry=0x1be80345780, max_depth=max_depth@entry=30, skip_count=skip_count@entry=3) at src/stacktrace.cc:294
#4  0x000000000040da2e in RecordGrowth (growth=2048000) at src/page_heap.cc:582
#5  tcmalloc::PageHeap::GrowHeap (this=this@entry=0x436960 <tcmalloc::Static::pageheap_>, n=n@entry=250) at src/page_heap.cc:608
#6  0x000000000040dcc3 in tcmalloc::PageHeap::New (this=0x436960 <tcmalloc::Static::pageheap_>, n=n@entry=250) at src/page_heap.cc:156
#7  0x0000000000403e25 in (anonymous namespace)::do_malloc_pages (heap=heap@entry=0x68bc40, size=size@entry=2044723) at src/tcmalloc.cc:1255
#8  0x0000000000420d0e in do_malloc (size=2044723) at src/tcmalloc.cc:1279
#9  do_allocate_full<tcmalloc::malloc_oom> (size=2044723) at src/tcmalloc.cc:1691
#10 tcmalloc::allocate_full_malloc_oom (size=2044723) at src/tcmalloc.cc:1707
#11 0x00000000004049ce in test_memory_allocation (allocation_size=2044723) at test.cc:16
#12 0x0000000000404bf3 in main () at test.cc:43
```

## tcmalloc::allocate_full_cpp_throw_oom

> 通过查看机器资源物理内存使用并不高，可能是什么原因导致 tcmalloc 抛出 cpp_throw_oom 异常呢？

cpp_throw_oom异常表示内存分配失败，但这并不一定意味着物理内存已用尽。以下是一些可能导致内存分配失败的原因，即使物理内存使用并不高：

1. 内存碎片：内存分配器可能无法找到足够大的连续内存块来满足分配请求，即使总的可用内存看似充足。这通常是由于内存碎片导致的。

2. 虚拟内存限制：操作系统可能会限制每个进程可以使用的虚拟内存数量。如果进程达到了这个限制，尝试分配更多内存可能会失败，即使物理内存仍有剩余。

3. 内存分配器限制：tcmalloc等内存分配器可能会在内部维护自己的内存池和限制。如果内存分配器达到了这些限制，分配请求可能会失败，即使物理内存仍有剩余。

4. 操作系统限制：操作系统可能会限制用户空间应用程序可以使用的内存数量。这可能是由于安全策略、资源配额等原因。在这种情况下，内存分配可能会失败，即使物理内存仍有剩余。

要诊断`cpp_throw_oom`异常的根本原因，可以尝试以下方法：

1. 检查操作系统的日志和错误消息，了解内存分配失败的原因。
2. 使用内存分析工具（如pprof）分析应用程序的内存使用情况，检查内存碎片和内存泄漏等问题。
3. 检查操作系统的资源限制和分配策略，确保应用程序有足够的虚拟内存和物理内存可用。
4. 考虑使用tcmalloc或其他内存分配器的调试和分析功能，以了解内存分配失败的详细原因。

------

`allocate_full_cpp_throw_oom`是tcmalloc库内部使用的一个函数，它在内存分配失败时负责抛出一个C++异常。当使用tcmalloc进行内存分配，但无法找到足够的空闲内存时，它会调用这个函数。这个函数的主要目的是在遇到内存分配失败时提供一个处理策略，通过抛出C++异常来通知调用者。

在tcmalloc的源代码中，`allocate_full_cpp_throw_oom`函数的实现如下：

``` cpp
void TCMallocImplementation::allocate_full_cpp_throw_oom(size_t size) {
  if (IsCppThrowHandlerRegistered()) {
    GetCppThrowHandler()(size);
  } else {
    // No C++ throw handler registered, so just call the default one.
    DefaultCppThrowHandler(size);
  }
}
```

这个函数的工作方式如下：

1. 首先检查是否已经注册了一个C++异常处理器。如果已经注册了一个处理器，函数会调用该处理器，并传入所请求的内存大小。
2. 如果没有注册处理器，函数会调用默认的C++异常处理器`DefaultCppThrowHandler`。

`DefaultCppThrowHandler`函数的实现如下：

``` cpp
ABSL_ATTRIBUTE_NORETURN void DefaultCppThrowHandler(size_t size) {
  // Check if we should call the new_handler first.
  std::new_handler nh = std::get_new_handler();
  if (nh != nullptr) {
    ABSL_RAW_LOG(ERROR,
                 "TCMalloc C++ Out of Memory: Trying to call new_handler.");
    nh();
    ABSL_RAW_CHECK(false,
                   "new_handler returned, but it should have thrown an "
                   "exception or terminated the program.");
  }

  // No new_handler, or new_handler returned, so just throw bad_alloc.
  ABSL_RAW_LOG(ERROR, "TCMalloc C++ Out of Memory: Throwing bad_alloc.");
  throw std::bad_alloc();
}
```

`DefaultCppThrowHandler`函数首先检查是否已经设置了C++的new_handler。如果设置了new_handler，函数会调用它。new_handler通常会尝试释放一些内存，以便分配可以成功。如果new_handler返回，说明它没有成功释放内存，函数会抛出一个std::bad_alloc异常。

在实际应用中，开发人员需要确保在使用tcmalloc进行内存分配时正确处理C++异常。这通常意味着在分配内存时使用try和catch语句，并在捕获到std::bad_alloc异常时采取适当的措施。

## tcmalloc::allocate_full_malloc_oom

`allocate_full_malloc_oom`函数是tcmalloc库内部使用的一个函数，它用于处理内存分配失败的情况。当tcmalloc尝试分配内存，但无法找到足够的空闲内存时，它会调用这个函数。这个函数的主要目的是在遇到内存分配失败时提供一个处理策略。

在tcmalloc的源代码中，`allocate_full_malloc_oom`函数的实现如下：

``` cpp
void* TCMallocImplementation::allocate_full_malloc_oom(size_t size) {
  // If TCMalloc is not allowed to release memory to the system, we
  // can't really do anything here.
  if (!IsSystemReleaseAllowed()) {
    return nullptr;
  }
  // Try to free up some memory from the central cache.
  if (ReleaseAtLeastNBytesFromFreeList(size)) {
    return nullptr;
  }
  // Give up and return nullptr.
  return nullptr;
}
```

这个函数的工作方式如下：

1. 首先检查tcmalloc是否允许将内存释放回操作系统。如果不允许，函数返回nullptr，表示分配失败。
2. 如果允许释放内存，函数尝试从中央缓存释放至少size字节的内存。如果释放成功，函数返回nullptr，表示分配失败。
3. 如果释放内存失败，函数返回nullptr，表示分配失败。

这个函数的主要作用是在遇到内存分配失败时尝试释放一些内存，以便其他分配请求可以成功。然而，如果释放内存的尝试失败，函数会返回nullptr，表示分配失败。在实际应用中，开发人员需要确保在使用tcmalloc分配内存时检查返回的指针是否为nullptr，并在遇到分配失败时采取适当的措施。













# Refer

* [TCMalloc Overview](https://google.github.io/tcmalloc/overview.html)
* [Announcing TCMalloc](https://abseil.io/blog/20200212-tcmalloc)
* https://github.com/google/tcmalloc
* https://github.com/google/tcmalloc/tree/master/docs (官方文档)
* https://google.github.io/tcmalloc/
* [TCMalloc : Thread-Caching Malloc](http://goog-perftools.sourceforge.net/doc/tcmalloc.html)
* [图解 TCMalloc](https://zhuanlan.zhihu.com/p/29216091)
* [TCMalloc 源码分析](https://dirtysalt.github.io/html/tcmalloc.html)
* [jemalloc](https://github.com/jemalloc/jemalloc/releases)
* [ptmalloc、tcmalloc与jemalloc对比分析](https://www.cyningsun.com/07-07-2018/memory-allocator-contrasts.html)
* [ptmalloc,tcmalloc和jemalloc内存分配策略研究](https://cloud.tencent.com/developer/article/1173720)
* [TCMalloc解密](https://zhuanlan.zhihu.com/p/51432385)







