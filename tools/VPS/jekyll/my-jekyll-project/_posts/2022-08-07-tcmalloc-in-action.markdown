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


# Build

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
  // eliding the allocation.
  constexpr size_t kSize = 1024 * 1024 * 1024;
  std::unique_ptr<char[]> ptr(new char[kSize]);

  std::cout << absl::StreamFormat("new'd %d bytes at %p\n", kSize, ptr.get());

  heap_size = tcmalloc::MallocExtension::GetNumericProperty(
      "generic.current_allocated_bytes");
  if (heap_size.has_value()) {
    std::cout << "Current heap size = " << *heap_size << " bytes" << std::endl;
  }

  void* ptr2 = malloc(kSize);
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





# Refer

* https://github.com/google/tcmalloc










