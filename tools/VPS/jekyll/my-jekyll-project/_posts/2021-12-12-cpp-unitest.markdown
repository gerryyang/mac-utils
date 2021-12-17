---
layout: post
title:  "CPP Unitest"
date:   2021-12-12 20:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}


[TOC]

# 单元测试方案对比

| Name | License | Macros | Mocks | Remarks
| -- | -- | -- | --  | --
| Boost Test Library | Boost  |  User decision | With additional library [Turtle](http://turtle.sourceforge.net/) | Part of [Boost](https://en.wikipedia.org/wiki/Boost_library). Powerful dataset concept for generating test cases. Different levels of fixtures (global, once per test suite, once per each test case in a suite). Powerful floating point comparison.
| Catch or Catch2 | Boost | Yes | No | Header only, no external dependencies, auto-registration, tdd and bdd features
| [Google Test](https://en.wikipedia.org/wiki/Google_Test) | BSD | Yes | Yes | Supports automatic test discovery, a rich set of assertions, user-defined assertions, death tests, fatal and non-fatal failures, various options for running the tests, and XML test report generation.
| doctest | N/A | Yes | No   |  A new C++ testing framework but is by far the fastest both in compile times and runtime compared to other feature-rich alternatives. 


refer: https://en.wikipedia.org/wiki/List_of_unit_testing_frameworks#C++

# CMake with unitest

[CMake: Project structure with unit tests](https://stackoverflow.com/questions/14446495/cmake-project-structure-with-unit-tests)


# GoogleTest - Google Testing and Mocking Framework

> googletest helps you write better C++ tests.

googletest is a testing framework developed by the Testing Technology team with Google’s specific requirements and constraints in mind. Whether you work on Linux, Windows, or a Mac, if you write C++ code, googletest can help you. And it supports any kind of tests, not just unit tests.

refer: 

* [Github GoogleTest](https://github.com/google/googletest)
* [GoogleTest User’s Guide](https://google.github.io/googletest/)


## [Googletest Primer](https://google.github.io/googletest/primer.html)

### Introduction: Why googletest?

So what makes a good test, and how does googletest fit in? We believe:

1. **Tests should be independent and repeatable**. It’s a pain to debug a test that succeeds or fails as a result of other tests. googletest isolates the tests by running each of them on a different object. When a test fails, googletest allows you to run it in isolation for quick debugging.

2. **Tests should be well organized and reflect the structure of the tested code**. googletest groups related tests into test suites that can share data and subroutines. This common pattern is easy to recognize and makes tests easy to maintain. Such consistency is especially helpful when people switch projects and start to work on a new code base.

3. **Tests should be portable and reusable**. Google has a lot of code that is platform-neutral; its tests should also be platform-neutral. googletest works on different OSes, with different compilers, with or without exceptions, so googletest tests can work with a variety of configurations.

4. **When tests fail, they should provide as much information about the problem as possible**. googletest doesn’t stop at the first test failure. Instead, it only stops the current test and continues with the next. You can also set up tests that report non-fatal failures after which the current test continues. Thus, you can detect and fix multiple bugs in a single run-edit-compile cycle.

5. **The testing framework should liberate test writers from housekeeping chores and let them focus on the test content**. googletest automatically keeps track of all tests defined, and doesn’t require the user to enumerate them in order to run them.

6. **Tests should be fast**. With googletest, you can reuse shared resources across tests and pay for the set-up/tear-down only once, without making tests depend on each other.

### Beware of the nomenclature(命名法)

Note: There might be some confusion arising from different definitions of the terms `Test`, `Test Case` and `Test Suite`, so beware of misunderstanding these.

Historically, googletest started to use the term `Test Case` for grouping related tests, whereas current publications, including International Software Testing Qualifications Board (ISTQB) materials and various textbooks on software quality, use the term `Test Suite` for this.

**googletest recently started replacing the term Test Case with Test Suite**. The preferred API is TestSuite. The older TestCase API is being slowly deprecated and refactored away.






# doctest

[doctest](https://github.com/doctest/doctest) is a new C++ testing framework but is by far the fastest both in compile times (by [**orders of magnitude**](https://github.com/doctest/doctest/blob/master/doc/markdown/benchmarks.md)) and runtime compared to other feature-rich alternatives. It brings the ability of compiled languages such as [**D**](https://dlang.org/spec/unittest.html), [**Rust**](https://doc.rust-lang.org/book/second-edition/ch11-00-testing.html), [**Nim**](https://nim-lang.org/docs/unittest.html) to have tests written directly in the production code thanks to a fast, transparent and flexible test runner with a clean interface.

[Reference](https://github.com/doctest/doctest/blob/master/doc/markdown/readme.md#reference)


## [特性](https://github.com/doctest/doctest/blob/master/doc/markdown/features.md)

> **doctest** has been designed from the start to be as **light** and **unintrusive** as possible.

## Unintrusive (transparent)

* everything testing-related can be removed from the binary executable by defining the `DOCTEST_CONFIG_DISABLE` identifier
* very small and easy to integrate - single header
* Extremely low footprint on compile times - [**around 25ms**](https://github.com/doctest/doctest/blob/master/doc/markdown/benchmarks.md#cost-of-including-the-header) of compile time overhead for including the header in a file
* The [**fastest possible**](https://github.com/doctest/doctest/blob/master/doc/markdown/benchmarks.md#cost-of-an-assertion-macro) assertion macros - 50k asserts can compile for under 30 seconds (even under 10 sec)
* doesn't drag any headers when included (except for in the translation unit where the library gets implemented)
* everything is in the  `doctest`  namespace (and the implementation details are in a nested  `detail`  namespace)
* all macros have prefixes - some by default have unprefixed versions as well but that is optional - see [**configuration**](https://github.com/doctest/doctest/blob/master/doc/markdown/configuration.md)
* 0 warnings even with the most aggressive flags (on all tested compilers!)
	-  `-Weverything -pedantic`  for **clang**
	-  `-Wall -Wextra -pedantic`  and **>> over 35 <<** other warnings **not** covered by these flags for **GCC** - see [**here**](https://github.com/doctest/doctest/blob/master/scripts/cmake/common.cmake#L84)
	-  `/Wall`  for **MSVC** (except for:  `C4514` ,  `C4571` ,  `C4710` ,  `C4711` )
* doesn't error on unrecognized [**command line**](https://github.com/doctest/doctest/blob/master/doc/markdown/commandline.md) options and supports prefixes for interop with client command line parsing
* can set options [**procedurally**](https://github.com/doctest/doctest/blob/master/doc/markdown/main.md) and not deal with passing  `argc` / `argv`  from the command line
* doesn't leave warnings disabled after itself

## [配置](https://github.com/doctest/doctest/blob/master/doc/markdown/configuration.md)

## [Benchmarks](https://github.com/doctest/doctest/blob/master/doc/markdown/benchmarks.md)

## [Tutorial](https://github.com/doctest/doctest/blob/master/doc/markdown/tutorial.md)

## [Testcases](https://github.com/doctest/doctest/blob/master/doc/markdown/testcases.md)

## [Assertions](https://github.com/doctest/doctest/blob/master/doc/markdown/assertions.md)


# gcov - a Test Coverage Program

[gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) is a tool you can use in conjunction with `GCC` to test code coverage in your programs.

`gcov` is a test coverage program. Use it in concert with GCC to analyze your programs to help create more efficient, faster running code and to discover untested parts of your program. You can use gcov as a profiling tool to help discover where your optimization efforts will best affect your code. You can also use gcov along with the other profiling tool, gprof, to assess which parts of your code use the greatest amount of computing time.

Profiling tools help you analyze your code’s performance. Using a profiler such as `gcov` or `gprof`, you can find out some basic performance statistics, such as:

* how often each line of code executes
* what lines of code are actually executed
* how much computing time each section of code uses

Once you know these things about how your code works when compiled, you can look at each module to see which modules should be optimized. `gcov` helps you determine where to work on optimization.

Software developers also use coverage testing in concert with testsuites, to make sure software is actually good enough for a release. Testsuites can verify that a program works as expected; a coverage program tests to see how much of the program is exercised by the testsuite. Developers can then determine what kinds of test cases need to be added to the testsuites to create both better testing and a better final product.

**You should compile your code without optimization if you plan to use gcov because the optimization, by combining some lines of code into one function, may not give you as much information as you need to look for ‘hot spots’ where the code is using a great deal of computer time.** Likewise, because gcov accumulates statistics by line (at the lowest resolution), it works best with a programming style that places only one statement on each line. If you use complicated macros that expand to loops or to other control structures, the statistics are less helpful—they only report on the line where the macro call appears. If your complex macros behave like functions, you can replace them with inline functions to solve this problem.

`gcov` creates a logfile called `sourcefile.gcov` which indicates how many times each line of a source file `sourcefile.c` has executed. You can use these logfiles along with `gprof` to aid in fine-tuning the performance of your programs. `gprof` gives timing information you can use along with the information you get from `gcov`.

`gcov` works only on code compiled with `GCC`. It is not compatible with any other profiling or test coverage mechanism.

  
```
# ubuntu
sudo apt-get install -y gcovr
```
	
	