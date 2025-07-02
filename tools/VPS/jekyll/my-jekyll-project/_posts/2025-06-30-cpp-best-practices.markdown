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


# Conventions

* [CPP Style Guide](http://gerryyang.com/c/c++/2022/10/17/cpp-style-guide.html)
  + [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
  + [LLVM Coding Standards](https://clang.llvm.org/extra/clang-tidy/checks/list.html)

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

* [CPP Cpplint in Action](http://gerryyang.com/c/c++/2022/03/31/cpp-cpplint-in-action.html)

* Use [static analysis](https://en.wikipedia.org/wiki/Static_program_analysis) ([clang-tidy](https://clang.llvm.org/extra/clang-tidy/), [cppcheck](http://cppcheck.sourceforge.net/), [coverity](https://scan.coverity.com/), [PVS-Studio](https://www.viva64.com/en/pvs-studio/)).


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





# Refer

* https://rigtorp.se/cpp-best-practices/
* https://lefticus.gitbooks.io/cpp-best-practices/content/
* O'Reilly video: [Learning C++ Best Practices](http://shop.oreilly.com/product/0636920049814.do)









