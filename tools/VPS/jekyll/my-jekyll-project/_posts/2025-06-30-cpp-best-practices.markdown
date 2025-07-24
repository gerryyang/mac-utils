---
layout: post
title:  "CPP Best Practices"
date:   2025-06-30 20:30:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Prepare

查看当前环境 C++ 版本：

``` bash
$ ls -l /lib64/libstdc++.so.6
lrwxrwxrwx 1 root root 19 Aug 18  2020 /lib64/libstdc++.so.6 -> libstdc++.so.6.0.25
$ rpm -qf /lib64/libstdc++.so.6
libstdc++-8.3.1-5.el8.0.2.x86_64
```

* [C++ compiler support](https://en.cppreference.com/w/cpp/compiler_support.html)


# Conventions

* [CPP Style Guide](http://gerryyang.com/c/c++/2022/10/17/cpp-style-guide.html)
  + [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
  + [LLVM Coding Standards](https://clang.llvm.org/extra/clang-tidy/checks/list.html)
  + [Abseil Fundamentals](https://abseil.io/docs/cpp/guides/base#abseil-fundamentals)

* Use the [Pitchfork](https://github.com/vector-of-bool/pitchfork) project layout conventions.

* Consider enforcing a [coding convention](https://en.wikipedia.org/wiki/Coding_conventions):
  + [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
  + [SEI CERT C++ Coding Standard](https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=88046682)
  + [MISRA C++](https://www.misra.org.uk/)
  + [High Integrity C++](https://www.perforce.com/resources/qac/high-integrity-cpp-coding-standard)
  + [JOINT STRIKE FIGHTER AIR VEHICLE C++ CODING STANDARDS](https://www.stroustrup.com/JSF-AV-rules.pdf)

> **Coding conventions** are a set of guidelines for a specific [programming language](https://en.wikipedia.org/wiki/Programming_language) that recommend [programming style](https://en.wikipedia.org/wiki/Programming_style), practices, and methods for each aspect of a program written in that language. These conventions usually cover file organization, [indentation](https://en.wikipedia.org/wiki/Indent_style), [comments](https://en.wikipedia.org/wiki/Comment_(computer_programming)), [declarations](https://en.wikipedia.org/wiki/Declaration_(computer_science)), [statements](https://en.wikipedia.org/wiki/Statement_(programming)), [white space](https://en.wikipedia.org/wiki/Whitespace_(computer_science)), [naming conventions](https://en.wikipedia.org/wiki/Identifier_naming_convention), [programming practices](https://en.wikipedia.org/wiki/Best_Coding_Practices), [programming principles](https://en.wikipedia.org/wiki/Category:Programming_principles), programming rules of thumb, architectural best practices, etc. These are guidelines for [software structural quality](https://en.wikipedia.org/wiki/Software_quality_model). [Software programmers](https://en.wikipedia.org/wiki/Programmer) are highly recommended to follow these guidelines to help improve the [readability](https://en.wikipedia.org/wiki/Readability) of their source code and make software maintenance easier. **Coding conventions** are only applicable to the human maintainers and peer reviewers of a software project. Conventions may be formalized in a documented set of rules that an entire team or company follows, or may be as informal as the habitual coding practices of an individual. Coding conventions are not enforced by [compilers](https://en.wikipedia.org/wiki/Compiler).


# Modern C++

* https://github.com/rigtorp/awesome-modern-cpp
* [A Tour of C++](https://www.amazon.com/Tour-2nd-Depth-Bjarne-Stroustrup/dp/0134997832/) (Bjarne Stroustrup) [C++11/17/20]
* [The C++ Programming Language](https://www.amazon.com/dp/0321958322) (Bjarne Stroustrup) [C++11]
* [21st Century C++ - Presenting the key contemporary C++ mechanism designed to maintain compatibility over decades. By Bjarne Stroustrup, Posted Feb 4 2025](https://cacm.acm.org/blogcacm/21st-century-c/)
* [Effective Modern C++ - (Scott Meyers) [C++11/14]](https://drive.google.com/file/d/1LJJgIa_x6yrHQcLn1235CIJQKIotJQe0/view?pli=1)
* [C++17 - The Complete Guide](https://www.cppstd17.com/) (Nicolai M. Josuttis) [C++17] - Giving guidance on how to use Language and Library Features of C++17.
* [Modern C++ Tutorial: C++ 11/14/17/20 On the Fly](https://changkun.de/modern-cpp/) (Changkun Ou) - The book claims to be "On the Fly". Its intent is to provide a comprehensive introduction to the relevant features regarding modern C++ (before 2020s).

* [Modern CPP in Action](http://gerryyang.com/c/c++/2023/10/07/modern-cpp-in-action.html)



# Format

* Never manually format code, use [clang-format](https://clang.llvm.org/docs/ClangFormat.html) to format your code.
* [CPP ClangFormat in Action](http://gerryyang.com/c/c++/2022/04/13/cpp-clangformat-in-action.html)


# Compiler

* 编译器对 C++ 标准的支持情况：https://en.cppreference.com/w/cpp/compiler_support
  + [C++ Standards Support in GCC](https://gcc.gnu.org/projects/cxx-status.html)
* [GCC in Action](http://gerryyang.com/gcc/clang/2021/06/04/gcc-in-action.html)
* [Clang in Action](http://gerryyang.com/gcc/clang/2022/09/08/clang-in-action.html)



# Build

* Build your code using [CMake](https://cmake.org/).
* [CMake in Action](http://gerryyang.com/gcc/clang/2020/12/13/cmake-in-action.html)
* [Modern CMake in Action](http://gerryyang.com/gcc/clang/2022/07/31/modern-cmake-in-action.html)
* [Bazel in Action](http://gerryyang.com/gcc/clang/2022/08/07/bazel-in-action.html)
* [Make in Action](http://gerryyang.com/gcc/clang/2025/06/02/make-in-action.html)
* [Professional CMake](https://crascit.com/professional-cmake/) (Craig Scott) - A practical guide for using CMake to build your C++ projects.


* Enable at least `-Wall -Wextra -Wpedantic` warning options ([GCC](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html), [clang](https://clang.llvm.org/docs/DiagnosticsReference.html)).

> **-Wpedantic**
>
> Issue all the warnings demanded by strict ISO C and ISO C++; diagnose all programs that use forbidden extensions, and some other programs that do not follow ISO C and ISO C++. This follows the version of the ISO C or C++ standard specified by any `-std` option used.



* Use compiler options to harden your application from security exploits:
  + [Debian build hardening](https://wiki.debian.org/Hardening)




# Test

* Unit Testing Framework
  + Write unit tests using [Catch](https://github.com/catchorg/Catch2).
  + C++ 单元测试框架对比：https://en.wikipedia.org/wiki/List_of_unit_testing_frameworks#C++
  + [CPP Unit Testing Framework](http://gerryyang.com/c/c++/2021/12/12/cpp-unit-testing-framework.html)

* Use a [fuzzer](https://en.wikipedia.org/wiki/Fuzzing) to test handling of untrusted input ([afl](http://lcamtuf.coredump.cx/afl/), [libFuzzer](https://llvm.org/docs/LibFuzzer.html), [honggfuzz](https://google.github.io/honggfuzz/)).


# Check

* Build and run your tests with [sanitizers](https://github.com/google/sanitizers) enabled.
* [AddressSanitizer (ASan) - A Fast Memory Error Detector](http://gerryyang.com/linux%20performance/2021/10/15/address-sanitizer.html)

* [CPP Cpplint in Action](http://gerryyang.com/c/c++/2022/03/31/cpp-cpplint-in-action.html) Cpplint 是一个 Python 编写的基于 Google 代码规范的检测工具。它只是一个代码风格检测工具，其并不对代码逻辑、语法错误等进行检查。More: https://en.wikipedia.org/wiki/Cpplint

* Use [static analysis](https://en.wikipedia.org/wiki/Static_program_analysis) ([clang-tidy](https://clang.llvm.org/extra/clang-tidy/), [cppcheck](http://cppcheck.sourceforge.net/), [coverity](https://scan.coverity.com/), [PVS-Studio](https://www.viva64.com/en/pvs-studio/)).

Clang 项目也提供了其他一些工具，包括代码的静态检查工具 [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/) 。这是一个比较全面的工具，它除了会提示你危险的用法，也会告诉你如何去现代化你的代码。默认情况下，Clang-Tidy 只做基本的分析。你也可以告诉它你想现代化你的代码和提高代码的可读性：

```
clang-tidy --checks='clang-analyzer-*,modernize-*,readability-*' test.cpp
```

* Clang-Tidy 还是一个比较“重”的工具。它需要有一定的配置，需要能看到文件用到的头文件，运行的时间也会较长。而 [Cppcheck](https://github.com/danmar/cppcheck) 就是一个非常轻量的工具了。它运行速度飞快，看不到头文件、不需要配置就能使用。它跟 Clang-Tidy 的重点也不太一样：它强调的是发现代码可能出问题的地方，而不太着重代码风格问题，两者功能并不完全重叠。有条件的情况下，这两个工具可以一起使用。

* [Valgrind](https://valgrind.org/) 算是一个老牌工具了。它是一个非侵入式的排错工具。根据 Valgrind 的文档，它会导致可执行文件的速度减慢 20 至 30 倍。但它可以在不改变可执行文件的情况下，只要求你在编译时增加产生调试信息的命令行参数（`-g`），即可查出内存相关的错误。

``` cpp
int main()
{
  char* ptr = new char[20];
}
```

在 Linux 上使用 `g++ -g test.cpp` 编译之后，然后使用 `valgrind --leak-check=full ./a.out` 检查运行结果，得到的输出会如下所示：

![valgrind](/assets/images/201911/valgrind.png)

即其中包含了内存泄漏的信息，包括内存是从什么地方泄漏的。Valgrind 的功能并不只是内存查错，也包含了多线程问题分析等其他功能。要进一步了解相关信息，请查阅其文档。

* 在 [nvwa](https://github.com/adah1972/nvwa/) 项目里，我也包含了一个很小的内存泄漏检查工具。它的最大优点是小巧，并且对程序运行性能影响极小；缺点主要是不及 Valgrind 易用和强大，只能检查 new 导致的内存泄漏，并需要侵入式地对项目做修改。

``` bash
c++ test.cpp \../nvwa/nvwa/debug_new.cpp
```

* [clangd-tidy](https://github.com/lljbash/clangd-tidy), A Faster Alternative to clang-tidy

[Why is clang-tidy in clangd so much faster than (run-)clang-tidy itself?](https://stackoverflow.com/questions/76531831/why-is-clang-tidy-in-clangd-so-much-faster-than-run-clang-tidy-itself)

* [facebook/infer](https://github.com/facebook/infer) A static analyzer for Java, C, C++, and Objective-C

[Getting started with Infer](https://fbinfer.com/docs/getting-started/)



# Debug

* Build your tests with STL in debug mode:
  + [libstdc++](https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_macros.html): `-D_GLIBCXX_ASSERTIONS` or `-D_GLIBCXX_DEBUG`.
  + [libc++](https://libcxx.llvm.org/docs/DesignDocs/DebugMode.html): `-D_LIBCPP_DEBUG=1`


> **_GLIBCXX_DEBUG**
>
> Undefined by default. When defined, compiles user code using the [debug mode](https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode.html). When defined, **_GLIBCXX_ASSERTIONS** is defined automatically, so all the assertions enabled by that macro are also enabled in debug mode.


* [Debugging Assembly Code with GDB](http://gerryyang.com/debuggers/2020/05/04/debugging-assembly-code-with-gdb.html)
* [How debuggers work: Part 1 - Basics](http://gerryyang.com/debuggers/2024/07/06/how-debuggers-work-basics.html)
* [Linux Core Dumps](http://gerryyang.com/debuggers/2024/12/03/linux-core-dumps.html)



# Tools

* 模版实例化工具：https://cppinsights.io/
* 编译运行工具：https://wandbox.org/
* 代码统计工具 (Count Lines of Code)：https://github.com/AlDanial/cloc



# Manual

* [The GNU C++ Library Manual](https://gcc.gnu.org/onlinedocs/libstdc++/manual/)


# Posts

* [CPP in Action](http://gerryyang.com/c/c++/2019/05/06/cpp-in-action.html)
* [CPP Effective](http://gerryyang.com/c/c++/2020/07/13/cpp-effective.html)
* [CPP Optimzing](http://gerryyang.com/c/c++/2021/09/21/cpp-optimizing.html)
* [CPP Template in Action](http://gerryyang.com/c/c++/2021/10/07/cpp-template-in-action.html)
* [CPP Memory Order](http://gerryyang.com/c/c++/2022/03/14/cpp-memory-order.html)
* [Bjarne Stroustrup's C++ Style and Technique FAQ (Reading)](http://gerryyang.com/c/c++/2022/03/04/bjarne-stroustrup-cpp-style-and-technique-faq.html)
* [CPP Lab](http://gerryyang.com/c/c++/2021/05/11/cpp-lab.html)
* [Inside the C++ Object Model (深度探索 C++对象模型)](http://gerryyang.com/c/c++/2024/05/13/inside-the-cpp-object-model.html)
* [CPP Reflection in Action](http://gerryyang.com/c/c++/2025/05/22/cpp-reflection-in-action.html)



# Common Libraries

* [abseil](https://github.com/abseil/abseil-cpp)
  + [Abseil in Action](http://gerryyang.com/c/c++/2022/08/07/abseil-in-action.html)

* [gflags](https://github.com/gflags/example)
  + [CPP gflags in Action](http://gerryyang.com/c/c++/2025/01/15/cpp-gflags-in-action.html)

* Lock-Free
  + [Awesome Lock-Free](https://github.com/rigtorp/awesome-lockfree)
  + [SPSCQueue](https://github.com/rigtorp/SPSCQueue)
  + [MPMCQueue](https://github.com/rigtorp/MPMCQueue)
  + [Optimizing a ring buffer for throughput](https://rigtorp.se/ringbuffer/)

* HashMap
  + [HashMap - rigtorp](https://github.com/rigtorp/HashMap)


# glibc

源码：https://elixir.bootlin.com/glibc


# [GCC](https://gcc.gnu.org/) (the GNU Compiler Collection)

The GNU Compiler Collection includes front ends for C, C++, Objective-C, Fortran, Ada, Go, D, Modula-2, and COBOL as well as libraries for these languages (libstdc++,...). GCC was originally written as the compiler for the [GNU operating system](http://www.gnu.org/gnu/thegnuproject.html). The GNU system was developed to be 100% free software, free in the sense that it [respects the user's freedom](http://www.gnu.org/philosophy/free-sw.html).

We strive to provide regular, high quality [releases](https://gcc.gnu.org/releases.html), which we want to work well on a variety of native and cross targets (including GNU/Linux), and encourage everyone to [contribute](https://gcc.gnu.org/contribute.html) changes or help [testing](https://gcc.gnu.org/testing/) GCC. Our sources are readily and freely available via [Git](https://gcc.gnu.org/git.html) and weekly [snapshots](https://gcc.gnu.org/snapshots.html).

Major decisions about GCC are made by the [steering committee](https://gcc.gnu.org/steering.html), guided by the [mission statement](https://gcc.gnu.org/gccmission.html).


## [GCC Releases](https://gcc.gnu.org/releases.html)

GCC releases may be downloaded from our [mirror sites](https://gcc.gnu.org/mirrors.html).

> **Important**: these are source releases, so will be of little use if you do not already have a C++ compiler installed. As one option, there are [pre-compiled binaries](https://gcc.gnu.org/install/binaries.html). for various platforms.

You can also retrieve our sources [using Git](https://gcc.gnu.org/git.html).


# LLVM

https://github.com/llvm/llvm-project

## [Clang](https://clang.llvm.org/)

> Clang: a C language family frontend for LLVM

The Clang project provides a language front-end and tooling infrastructure for languages in the C language family (C, C++, Objective C/C++, OpenCL, and CUDA) for the [LLVM](https://www.llvm.org/) project. Both a GCC-compatible compiler driver (clang) and an MSVC-compatible compiler driver (clang-cl.exe) are provided. You can [get and build](https://clang.llvm.org/get_started.html) the source today.

### [Clang - Features and Goals](https://clang.llvm.org/features.html)

These features are:

* End-User Features:
  + [Fast compiles and low memory use](https://clang.llvm.org/features.html#performance)
  + [Expressive diagnostics](https://clang.llvm.org/features.html#expressivediags)
  + [GCC compatibility](https://clang.llvm.org/features.html#gcccompat)

* Utility and Applications:
  + [Library based architecture](https://clang.llvm.org/features.html#libraryarch)
  + [Support diverse clients](https://clang.llvm.org/features.html#diverseclients)
  + [Integration with IDEs](https://clang.llvm.org/features.html#ideintegration)
  + [Use the LLVM 'BSD' License](https://clang.llvm.org/features.html#license)

* Internal Design and Implementation:
  + [A real-world, production quality compiler](https://clang.llvm.org/features.html#real)
  + [A simple and hackable code base](https://clang.llvm.org/features.html#simplecode)
  + [A single unified parser for C, Objective C, C++, and Objective C++](https://clang.llvm.org/features.html#unifiedparser)
  + [Conformance with C/C++/ObjC and their variants](https://clang.llvm.org/features.html#conformance)

### Why?

Development of the new front-end was started out of a need for a compiler that allows better diagnostics, better integration with IDEs, a license that is compatible with commercial products, and a nimble compiler that is easy to develop and maintain. All of these were motivations for starting work on a new front-end that could meet these needs.


### Current Status

`Clang` is considered to be a production quality C, Objective-C, C++ and Objective-C++ compiler when targeting any target supported by LLVM. As example, Clang is used in production to build performance-critical software like `Chrome` or `Firefox`.

If you are looking for source analysis or source-to-source transformation tools, `Clang` is probably a great solution for you. Please see the [C++ status](https://clang.llvm.org/cxx_status.html) page or the [C status](https://clang.llvm.org/c_status.html) page for more information about what standard modes and features are supported.

#### [C++ Support in Clang](https://clang.llvm.org/cxx_status.html)

目前 Clang11 可以支持 C++17 标准，更高版本的 Clang 可支持 C++20/C++23 部分特性，可见：https://clang.llvm.org/cxx_status.html

#### [C Support in Clang](https://clang.llvm.org/c_status.html)

可见：https://clang.llvm.org/c_status.html

### Get it and get involved

Start by [getting the code, building it, and playing with it](https://clang.llvm.org/get_started.html). This will show you the sorts of things we can do today and will let you have the "Clang experience" first hand: hopefully it will "resonate" with you.

Once you've done that, please consider [getting involved in the Clang community](https://clang.llvm.org/get_involved.html). The Clang developers include numerous volunteer contributors with a variety of backgrounds. If you're interested in following the development of Clang, signing up for a mailing list is a good way to learn about how the project works.






# Refer

* https://rigtorp.se/cpp-best-practices/
* https://lefticus.gitbooks.io/cpp-best-practices/content/
* O'Reilly video: [Learning C++ Best Practices](http://shop.oreilly.com/product/0636920049814.do)









