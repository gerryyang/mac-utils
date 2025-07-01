---
layout: post
title:  "CPP Unit Testing Framework"
date:   2021-12-12 20:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}


# 单元测试方案对比

| Name | License | Macros | Mocks | Remarks
| -- | -- | -- | --  | --
| Boost Test Library | Boost  |  User decision | With additional library [Turtle](http://turtle.sourceforge.net/) | Part of [Boost](https://en.wikipedia.org/wiki/Boost_library). Powerful dataset concept for generating test cases. Different levels of fixtures (global, once per test suite, once per each test case in a suite). Powerful floating point comparison.
| Catch or Catch2 | Boost | Yes | No | Header only, no external dependencies, auto-registration, tdd and bdd features
| [Google Test](https://en.wikipedia.org/wiki/Google_Test) | BSD | Yes | Yes | Supports automatic test discovery, a rich set of assertions, user-defined assertions, death tests, fatal and non-fatal failures, various options for running the tests, and XML test report generation.
| doctest | N/A | Yes | No   |  A new C++ testing framework but is by far the fastest both in compile times and runtime compared to other feature-rich alternatives.


refer: https://en.wikipedia.org/wiki/List_of_unit_testing_frameworks#C++


# GoogleTest - Google Testing and Mocking Framework

> googletest helps you write better C++ tests.

googletest is a testing framework developed by the Testing Technology team with Google’s specific requirements and constraints in mind. Whether you work on Linux, Windows, or a Mac, if you write C++ code, googletest can help you. And it supports any kind of tests, not just unit tests.

refer:

* [Github GoogleTest](https://github.com/google/googletest)
* [GoogleTest User’s Guide](https://google.github.io/googletest/)



## [Quickstart: Building with Bazel](https://google.github.io/googletest/quickstart-bazel.html)

This tutorial aims to get you up and running with GoogleTest using the Bazel build system. If you’re using GoogleTest for the first time or need a refresher, we recommend this tutorial as a starting point.

### Prerequisites

To complete this tutorial, you’ll need:

* A compatible operating system (e.g. Linux, macOS, Windows).
* A compatible C++ compiler that supports at least `C++14`.
* Bazel 7.0 or higher, the preferred build system used by the GoogleTest team.

See [Supported Platforms](https://google.github.io/googletest/platforms.html) for more information about platforms compatible with GoogleTest.

If you don’t already have Bazel installed, see the [Bazel installation guide](https://bazel.build/install).

> **Note**: The terminal commands in this tutorial show a Unix shell prompt, but the commands work on the Windows command line as well.

### Set up a Bazel workspace

A [Bazel workspace](https://docs.bazel.build/versions/main/build-ref.html#workspace) is a directory on your filesystem that you use to manage source files for the software you want to build. Each workspace directory has a text file named `MODULE.bazel` which may be empty, or may contain references to external dependencies required to build the outputs.

First, create a directory for your workspace:

``` bash
$ mkdir my_workspace && cd my_workspace
```

Next, you’ll create the `MODULE.bazel` file to specify dependencies. As of **Bazel 7.0**, the recommended way to consume **GoogleTest** is through the [Bazel Central Registry](https://registry.bazel.build/modules/googletest). To do this, create a `MODULE.bazel` file in the root directory of your Bazel workspace with the following content:

```
# MODULE.bazel

# Choose the most recent version available at
# https://registry.bazel.build/modules/googletest
bazel_dep(name = "googletest", version = "1.15.2")
```

Now you’re ready to build C++ code that uses **GoogleTest**.

### Create and run a binary

With your Bazel workspace set up, you can now use **GoogleTest** code within your own project. As an example, create a file named `hello_test.cc` in your `my_workspace` directory with the following contents:

``` cpp
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
```

GoogleTest provides [assertions](https://google.github.io/googletest/primer.html#assertions) that you use to test the behavior of your code. The above sample includes the main GoogleTest header file and demonstrates some basic assertions.

To build the code, create a file named `BUILD` in the same directory with the following contents:

```
cc_test(
    name = "hello_test",
    size = "small",
    srcs = ["hello_test.cc"],
    deps = [
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
```

This `cc_test` rule declares the C++ test binary you want to build, and links to the **GoogleTest** library (`@googletest//:gtest"`) and the **GoogleTest** `main() `function (`@googletest//:gtest_main`). For more information about Bazel `BUILD` files, see the [Bazel C++ Tutorial](https://docs.bazel.build/versions/main/tutorial/cpp.html).

> **NOTE**: In the example below, we assume Clang or GCC and set --cxxopt=-std=c++14 to ensure that GoogleTest is compiled as C++14 instead of the compiler’s default setting (which could be C++11). For MSVC, the equivalent would be --cxxopt=/std:c++14. See Supported Platforms for more details on supported language versions.

Now you can build and run your test:

```
$ bazel test --cxxopt=-std=c++14 --test_output=all //:hello_test
INFO: Analyzed target //:hello_test (26 packages loaded, 362 targets configured).
INFO: Found 1 test target...
INFO: From Testing //:hello_test:
==================== Test output for //:hello_test:
Running main() from gmock_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from HelloTest
[ RUN      ] HelloTest.BasicAssertions
[       OK ] HelloTest.BasicAssertions (0 ms)
[----------] 1 test from HelloTest (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
================================================================================
Target //:hello_test up-to-date:
  bazel-bin/hello_test
INFO: Elapsed time: 4.190s, Critical Path: 3.05s
INFO: 27 processes: 8 internal, 19 linux-sandbox.
INFO: Build completed successfully, 27 total actions
//:hello_test                                                     PASSED in 0.1s

INFO: Build completed successfully, 27 total actions
```

Congratulations! You’ve successfully built and run a test binary using **GoogleTest**.

### Next steps

* [Check out the Primer](https://google.github.io/googletest/primer.html) to start learning how to write simple tests.
* [See the code samples](https://google.github.io/googletest/samples.html) for more examples showing how to use a variety of GoogleTest features.



## [Quickstart: Building with CMake](https://google.github.io/googletest/quickstart-cmake.html)

This tutorial aims to get you up and running with **GoogleTest** using **CMake**. If you’re using **GoogleTest** for the first time or need a refresher, we recommend this tutorial as a starting point. If your project uses **Bazel**, see the [Quickstart for Bazel](https://google.github.io/googletest/quickstart-bazel.html) instead.


### Prerequisites

To complete this tutorial, you’ll need:

* A compatible operating system (e.g. Linux, macOS, Windows).
* A compatible C++ compiler that supports at least `C++14`.
* [CMake](https://cmake.org/) and a compatible build tool for building the project.
   + Compatible build tools include **Make**, **Ninja**, and others - see [CMake Generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) for more information.

See [Supported Platforms](https://google.github.io/googletest/platforms.html) for more information about platforms compatible with GoogleTest.

If you don’t already have CMake installed, see the [CMake installation guide](https://cmake.org/install).

> **Note**: The terminal commands in this tutorial show a Unix shell prompt, but the commands work on the Windows command line as well.


### Set up a project

CMake uses a file named `CMakeLists.txt` to configure the build system for a project. You’ll use this file to set up your project and declare a dependency on GoogleTest.

First, create a directory for your project:

``` bash
mkdir my_project && cd my_project
```

Next, you’ll create the `CMakeLists.txt` file and declare a dependency on **GoogleTest**. There are many ways to express dependencies in the CMake ecosystem; in this quickstart, you’ll use the [FetchContent CMake module](https://cmake.org/cmake/help/latest/module/FetchContent.html). To do this, in your project directory (`my_project`), create a file named `CMakeLists.txt` with the following contents:

```
cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
```

The above configuration declares a dependency on GoogleTest which is downloaded from GitHub. In the above example, `03597a01ee50ed33e9dfd640b249b4be3799d395` is the Git commit hash of the GoogleTest version to use; we recommend updating the hash often to point to the latest version.

For more information about how to create `CMakeLists.txt` files, see the [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html).

### Create and run a binary

With GoogleTest declared as a dependency, you can use GoogleTest code within your own project.

As an example, create a file named `hello_test.cc` in your `my_project` directory with the following contents:

``` cpp
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
```

GoogleTest provides [assertions](https://google.github.io/googletest/primer.html#assertions) that you use to test the behavior of your code. The above sample includes the main GoogleTest header file and demonstrates some basic assertions.

To build the code, add the following to the end of your `CMakeLists.txt` file:

```
enable_testing()

add_executable(
  hello_test
  hello_test.cc
)
target_link_libraries(
  hello_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)
```

The above configuration enables testing in CMake, declares the C++ test binary you want to build (`hello_test`), and links it to GoogleTest (`gtest_main`). The last two lines enable CMake’s test runner to discover the tests included in the binary, using the [GoogleTest CMake module](https://cmake.org/cmake/help/git-stage/module/GoogleTest.html).

Now you can build and run your test:

```
my_project$ cmake -S . -B build
-- The C compiler identification is GNU 10.2.1
-- The CXX compiler identification is GNU 10.2.1
...
-- Build files have been written to: .../my_project/build

my_project$ cmake --build build
Scanning dependencies of target gtest
...
[100%] Built target gmock_main

my_project$ cd build && ctest
Test project .../my_project/build
    Start 1: HelloTest.BasicAssertions
1/1 Test #1: HelloTest.BasicAssertions ........   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

Congratulations! You’ve successfully built and run a test binary using GoogleTest.


### Next steps

* [Check out the Primer](https://google.github.io/googletest/primer.html) to start learning how to write simple tests.
* [See the code samples](https://google.github.io/googletest/samples.html) for more examples showing how to use a variety of GoogleTest features.




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

> Note: There might be some confusion arising from different definitions of the terms `Test`, `Test Case` and `Test Suite`, so beware of misunderstanding these.

Historically, googletest started to use the term `Test Case` for grouping related tests, whereas current publications, including International Software Testing Qualifications Board (ISTQB) materials and various textbooks on software quality, use the term `Test Suite` for this.

**googletest recently started replacing the term Test Case with Test Suite**. The preferred API is TestSuite. The older TestCase API is being slowly deprecated and refactored away.

### Basic Concepts

When using googletest, you start by writing `assertions`, which are statements that check whether a condition is true. An assertion’s result can be `success`, `nonfatal failure`, or `fatal failure`. If a fatal failure occurs, it aborts the current function; otherwise the program continues normally.

Tests use `assertions` to verify the tested code’s behavior. If a test crashes or has a failed assertion, then it fails; otherwise it succeeds.

**A test suite** contains one or many tests. You should group your tests into test suites that reflect the structure of the tested code. When multiple tests in a test suite need to share common objects and subroutines, you can put them into **a test fixture class**.

A test program can contain multiple test suites.

We’ll now explain how to write a test program, starting at the individual assertion level and building up to tests and test suites.

### Assertions

googletest assertions are macros that resemble function calls. You test a class or function by making assertions about its behavior. When an assertion fails, googletest prints the assertion’s source file and line number location, along with a failure message. You may also supply a custom failure message which will be appended to googletest’s message.

The assertions come in pairs that test the same thing but have different effects on the current function. `ASSERT_*` versions generate fatal failures when they fail, and abort the current function. `EXPECT_*` versions generate nonfatal failures, which don’t abort the current function. Usually `EXPECT_*` are preferred, as they allow more than one failure to be reported in a test. However, you should use `ASSERT_*` if it doesn’t make sense to continue when the assertion in question fails.

Since a failed `ASSERT_*` returns from the current function immediately, possibly skipping clean-up code that comes after it, it may cause a space leak. Depending on the nature of the leak, it may or may not be worth fixing - so keep this in mind if you get a heap checker error in addition to assertion errors.

To provide a custom failure message, simply stream it into the macro using the `<< operator` or a sequence of such operators. See the following example, using the [ASSERT_EQ and EXPECT_EQ macros](https://google.github.io/googletest/reference/assertions.html#EXPECT_EQ) to verify value equality:

``` cpp
ASSERT_EQ(x.size(), y.size()) << "Vectors x and y are of unequal length";

for (int i = 0; i < x.size(); ++i) {
  EXPECT_EQ(x[i], y[i]) << "Vectors x and y differ at index " << i;
}
```

Anything that can be streamed to an ostream can be streamed to an assertion macro–in particular, C strings and string objects. If a wide string (`wchar_t*`, `TCHAR*` in `UNICODE` mode on Windows, or `std::wstring`) is streamed to an assertion, it will be translated to `UTF-8` when printed.

GoogleTest provides a collection of assertions for verifying the behavior of your code in various ways. You can check Boolean conditions, compare values based on relational operators, verify string values, floating-point values, and much more. There are even assertions that enable you to verify more complex states by providing custom predicates. For the complete list of assertions provided by GoogleTest, see the [Assertions Reference](https://google.github.io/googletest/reference/assertions.html).


### Simple Tests

To create a test:

1. Use the `TEST()` macro to define and name a test function. These are ordinary C++ functions that don’t return a value.
2. In this function, along with any valid C++ statements you want to include, use the various googletest assertions to check values.
3. The test’s result is determined by the assertions; if any assertion in the test fails (either fatally or non-fatally), or if the test crashes, the entire test fails. Otherwise, it succeeds.

``` cpp
TEST(TestSuiteName, TestName) {
    // ... test body ...
}
```

`TEST()` arguments go from general to specific. The first argument is the name of the test suite, and the second argument is the test’s name within the test suite. Both names must be valid C++ identifiers, and they should not contain any underscores (`_`). A test’s full name consists of its containing test suite and its individual name. Tests from different test suites can have the same individual name.

For example, let’s take a simple integer function:

``` cpp
int Factorial(int n);  // Returns the factorial of n
```

A test suite for this function might look like:

``` cpp
// Tests factorial of 0.
TEST(FactorialTest, HandlesZeroInput) {
  EXPECT_EQ(Factorial(0), 1);
}

// Tests factorial of positive numbers.
TEST(FactorialTest, HandlesPositiveInput) {
  EXPECT_EQ(Factorial(1), 1);
  EXPECT_EQ(Factorial(2), 2);
  EXPECT_EQ(Factorial(3), 6);
  EXPECT_EQ(Factorial(8), 40320);
}
```

googletest groups the test results by test suites, so logically related tests should be in the same test suite; in other words, the first argument to their `TEST()` should be the same. In the above example, we have two tests, `HandlesZeroInput` and `HandlesPositiveInput`, that belong to the same test suite `FactorialTest`.

When naming your test suites and tests, you should follow the same convention as for [naming functions and classes](https://google.github.io/styleguide/cppguide.html#Function_Names).


### Test Fixtures: Using the Same Data Configuration for Multiple Tests

If you find yourself writing two or more tests that operate on similar data, you can use a **test fixture**. This allows you to reuse the same configuration of objects for several different tests.

To create a fixture:

1. Derive a class from `::testing::Test` . Start its body with `protected:`, as we’ll want to access fixture members from `sub-classes`.
2. Inside the class, declare any objects you plan to use.
3. If necessary, write a default constructor or `SetUp()` function to prepare the objects for each test. A common mistake is to spell `SetUp()` as *Setup()* with a small u - Use `override` in C++11 to make sure you spelled it correctly.
4. If necessary, write a destructor or `TearDown()` function to release any resources you allocated in `SetUp()` . To learn when you should use the constructor/destructor and when you should use SetUp()/TearDown(), [read the FAQ](https://google.github.io/googletest/faq.html#CtorVsSetUp).
5. If needed, define subroutines for your tests to share.

When using a **fixture**, use `TEST_F()` instead of `TEST()` as it **allows you to access objects and subroutines in the test fixture**:

``` cpp
TEST_F(TestFixtureName, TestName) {
    // ... test body ...
}
```

Like `TEST()`, the first argument is the test suite name, but for `TEST_F()` this must be the name of the test fixture class. You’ve probably guessed: `_F` is for **fixture**.

Unfortunately, the C++ macro system does not allow us to create a single macro that can handle both types of tests. Using the wrong macro causes a compiler error.

Also, you must first define a test fixture class before using it in a `TEST_F()`, or **you’ll get the compiler error “virtual outside class declaration”.**

For each test defined with `TEST_F()`, **googletest will create a fresh test fixture at runtime**, immediately initialize it via `SetUp()`, run the test, clean up by calling `TearDown()`, and then delete the test fixture. **Note that different tests in the same test suite have different test fixture objects, and googletest always deletes a test fixture before it creates the next one**. googletest does not reuse the same test fixture for multiple tests. Any changes one test makes to the fixture do not affect other tests.

As an example, let’s write tests for a FIFO queue class named `Queue`, which has the following interface:

``` cpp
template <typename E>  // E is the element type.
class Queue {
 public:
  Queue();
  void Enqueue(const E& element);
  E* Dequeue();  // Returns NULL if the queue is empty.
  size_t size() const;
  ...
};
```

First, define a fixture class. By convention, you should give it the name `FooTest` where `Foo` is the class being tested.

``` cpp
class QueueTest : public ::testing::Test {
 protected:
  void SetUp() override {
     q0_.Enqueue(1);
     q1_.Enqueue(2);
     q2_.Enqueue(3);
  }

  // void TearDown() override {}

  Queue<int> q0_;
  Queue<int> q1_;
  Queue<int> q2_;
};
```

In this case, `TearDown()` is not needed since we don’t have to clean up after each test, other than what’s already done by the destructor.

Now we’ll write tests using `TEST_F()` and this fixture.

``` cpp
TEST_F(QueueTest, IsEmptyInitially) {
  EXPECT_EQ(q0_.size(), 0);
}

TEST_F(QueueTest, DequeueWorks) {
  int* n = q0_.Dequeue();
  EXPECT_EQ(n, nullptr);

  n = q1_.Dequeue();
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(*n, 1);
  EXPECT_EQ(q1_.size(), 0);
  delete n;

  n = q2_.Dequeue();
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(*n, 2);
  EXPECT_EQ(q2_.size(), 1);
  delete n;
}
```

The above uses both `ASSERT_*` and `EXPECT_*` assertions. The rule of thumb is to use `EXPECT_*` when you want the test to continue to reveal more errors after the assertion failure, and use `ASSERT_*` when continuing after failure doesn’t make sense. For example, the second assertion in the Dequeue test is `ASSERT_NE(n, nullptr)`, as we need to dereference the pointer n later, which would lead to a segfault when n is NULL.

When these tests run, the following happens:

1. googletest constructs a `QueueTest` object (let’s call it `t1`).
2. `t1.SetUp()` initializes `t1`.
3. The first test (`IsEmptyInitially`) runs on `t1`.
4. `t1.TearDown()` cleans up after the test finishes.
5. `t1` is destructed.
6. The above steps are repeated on another `QueueTest` object, this time running the `DequeueWorks` test.

### Invoking the Tests

`TEST()` and `TEST_F()` implicitly register their tests with googletest. So, unlike with many other C++ testing frameworks, you don’t have to re-list all your defined tests in order to run them.

After defining your tests, you can run them with `RUN_ALL_TESTS()`, which returns `0` if all the tests are successful, or `1` otherwise. Note that `RUN_ALL_TESTS()` runs all tests in your link unit–they can be from different test suites, or even different source files.

When invoked, the `RUN_ALL_TESTS()` macro:

* Saves the state of all googletest flags.

* Creates a test fixture object for the first test.

* Initializes it via `SetUp()`.

* Runs the test on the fixture object.

* Cleans up the fixture via `TearDown()`.

* Deletes the fixture.

* Restores the state of all googletest flags.

* Repeats the above steps for the next test, until all tests have run.

If a fatal failure happens the subsequent steps will be skipped.

> IMPORTANT: You must not ignore the return value of `RUN_ALL_TESTS()`, or you will get a compiler error. The rationale for this design is that the automated testing service determines whether a test has passed based on its exit code, not on its stdout/stderr output; thus your `main()` function must return the value of `RUN_ALL_TESTS()`.
>
> Also, you should call `RUN_ALL_TESTS()` only **once**. Calling it more than once conflicts with some advanced googletest features (e.g., thread-safe death tests) and thus is not supported.

### Writing the main() Function

Most users should not need to write their own `main` function and instead link with `gtest_main` (as opposed to with `gtest`), which defines a suitable entry point. See the end of this section for details. The remainder of this section should only apply when you need to do something custom before the tests run that cannot be expressed within the framework of fixtures and test suites.

If you write your own `main` function, it should return the value of `RUN_ALL_TESTS()`.

You can start from this boilerplate(样板):

``` cpp
#include "this/package/foo.h"

#include "gtest/gtest.h"

namespace my {
namespace project {
namespace {

// The fixture for testing class Foo.
class FooTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  FooTest() {
     // You can do set-up work for each test here.
  }

  ~FooTest() override {
     // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
  }

  void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
  }

  // Class members declared here can be used by all tests in the test suite
  // for Foo.
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(FooTest, MethodBarDoesAbc) {
  const std::string input_filepath = "this/package/testdata/myinputfile.dat";
  const std::string output_filepath = "this/package/testdata/myoutputfile.dat";
  Foo f;
  EXPECT_EQ(f.Bar(input_filepath, output_filepath), 0);
}

// Tests that Foo does Xyz.
TEST_F(FooTest, DoesXyz) {
  // Exercises the Xyz feature of Foo.
}

}  // namespace
}  // namespace project
}  // namespace my

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
```

The `::testing::InitGoogleTest()` function parses the command line for googletest flags, and removes all recognized flags. This allows the user to control a test program’s behavior via various flags, which we’ll cover in the [AdvancedGuide](https://google.github.io/googletest/advanced.html). You must call this function before calling `RUN_ALL_TESTS()`, or the flags won’t be properly initialized.

On Windows, `InitGoogleTest()` also works with wide strings, so it can be used in programs compiled in `UNICODE` mode as well.

But maybe you think that writing all those `main` functions is too much work? We agree with you completely, and that’s why Google Test provides a basic implementation of main(). If it fits your needs, then just link your test with the `gtest_main` library and you are good to go.

> NOTE: ParseGUnitFlags() is deprecated in favor of InitGoogleTest().


### Known Limitations

**Google Test is designed to be thread-safe**. The implementation is thread-safe on systems where the pthreads library is available. It is currently unsafe to use Google Test assertions from two threads concurrently on other systems (e.g. Windows). In most tests this is not an issue as usually the assertions are done in the main thread. If you want to help, you can volunteer to implement the necessary synchronization primitives in `gtest-port.h` for your platform.


## [Googletest Samples](https://google.github.io/googletest/samples.html)

If you’re like us, you’d like to look at [googletest samples](https://github.com/google/googletest/blob/main/googletest/samples). The sample directory has a number of well-commented samples showing how to use a variety of googletest features.

* Sample #1 shows the basic steps of using googletest to test C++ functions.
* Sample #2 shows a more complex unit test for a class with multiple member functions.
* Sample #3 uses a test fixture.
* Sample #4 teaches you how to use googletest and `googletest.h` together to get the best of both libraries.
* Sample #5 puts shared testing logic in a base test fixture, and reuses it in derived fixtures.
* Sample #6 demonstrates type-parameterized tests.
* Sample #7 teaches the basics of value-parameterized tests.
* Sample #8 shows using `Combine()` in value-parameterized tests.
* Sample #9 shows use of the listener API to modify Google Test’s console output and the use of its reflection API to inspect test results.
* Sample #10 shows use of the listener API to implement a primitive memory leak checker.



## [Advanced googletest Topics](http://google.github.io/googletest/advanced.html)


### Introduction

Now that you have read the [GoogleTest Primer](https://google.github.io/googletest/primer.html) and learned how to write tests using GoogleTest, it’s time to learn some new tricks. This document will show you more assertions as well as how to construct complex failure messages, propagate fatal failures, reuse and speed up your test fixtures, and use various flags with your tests.



## [gMock for Dummies](https://google.github.io/googletest/gmock_for_dummies.html)

### What Is gMock?

When you write a prototype or test, often it’s not feasible or wise to rely on real objects entirely. A mock object implements the same interface as a real object (so it can be used as one), but lets you specify at run time how it will be used and what it should do (which methods will be called? in which order? how many times? with what arguments? what will they return? etc).

It is easy to confuse(混淆) the term **fake** objects with **mock** objects. Fakes and mocks actually mean very different things in the **Test-Driven Development** (`TDD`) community:

* **Fake** objects have working implementations, but usually take some shortcut (perhaps to make the operations less expensive), which makes them not suitable for production. An in-memory file system would be an example of a fake.

* **Mocks** are objects pre-programmed with expectations, which form a specification of the calls they are expected to receive.

If all this seems too abstract for you, don’t worry - **the most important thing to remember is that a mock allows you to check the interaction between itself and code that uses it**. The difference between fakes and mocks shall become much clearer once you start to use mocks.

**gMock** is a library (sometimes we also call it a “framework” to make it sound cool) for creating mock classes and using them. It does to C++ what **jMock**/**EasyMock** does to **Java** (well, more or less).

When using **gMock**,

1. **first**, you use some simple macros to describe the interface you want to mock, and they will expand to the implementation of your mock class;
2. **next**, you create some mock objects and specify its expectations and behavior using an intuitive syntax;
3. **then** you exercise code that uses the mock objects. **gMock** will catch any violation to the expectations as soon as it arises.

### Why gMock?

While mock objects help you remove unnecessary dependencies in tests and make them fast and reliable, using mocks manually in C++ is **hard**:

* Someone has to implement the mocks. The job is usually tedious and error-prone. No wonder people go great distance to avoid it.
* The quality of those manually written mocks is a bit, uh, unpredictable. You may see some really polished ones, but you may also see some that were hacked up in a hurry and have all sorts of ad hoc restrictions.
* The knowledge you gained from using one mock doesn’t transfer to the next one.

In contrast, **Java** and **Python** programmers have some fine mock frameworks (**jMock**, **EasyMock**, etc), which automate the creation of mocks. As a result, mocking is a proven effective technique and widely adopted practice in those communities. Having the right tool absolutely makes the difference.

**gMock** was built to help C++ programmers. It was inspired by **jMock** and **EasyMock**, but designed with C++’s specifics in mind. It is your friend if any of the following problems is bothering you:

* You are stuck with a sub-optimal design and wish you had done more prototyping before it was too late, but prototyping in C++ is by no means “rapid”.
* Your tests are slow as they depend on too many libraries or use expensive resources (e.g. a database).
* Your tests are brittle as some resources they use are unreliable (e.g. the network).
* You want to test how your code handles a failure (e.g. a file checksum error), but it’s not easy to cause one.
* You need to make sure that your module interacts with other modules in the right way, but it’s hard to observe the interaction; therefore you resort to observing the side effects at the end of the action, but it’s awkward at best.
* You want to “mock out” your dependencies, except that they don’t have mock implementations yet; and, frankly, you aren’t thrilled by some of those hand-written mocks.

We encourage you to use **gMock** as

* **a design tool, for it lets you experiment with your interface design early and often. More iterations lead to better designs!**
* **a testing tool to cut your tests’ outbound dependencies and probe the interaction between your module and its collaborators.**

### Getting Started

gMock is bundled with googletest.


### A Case for Mock Turtles

Let’s look at an example. Suppose you are developing a graphics program that relies on a [LOGO](https://en.wikipedia.org/wiki/Logo_programming_language)-like API for drawing. **How would you test that it does the right thing? Well, you can run it and compare the screen with a golden screen snapshot, but let’s admit it: tests like this are expensive to run and fragile** (What if you just upgraded to a shiny new graphics card that has better anti-aliasing? Suddenly you have to update all your golden images.). It would be too painful if all your tests are like this. **Fortunately, you learned about [Dependency Injection(依赖注入)](https://en.wikipedia.org/wiki/Dependency_injection) and know the right thing to do: instead of having your application talk to the system API directly, wrap the API in an interface (say, Turtle) and code to that interface**:

``` cpp
class Turtle {
  ...
  virtual ~Turtle() {}

  virtual void PenUp() = 0;
  virtual void PenDown() = 0;
  virtual void Forward(int distance) = 0;
  virtual void Turn(int degrees) = 0;
  virtual void GoTo(int x, int y) = 0;
  virtual int GetX() const = 0;
  virtual int GetY() const = 0;
};
```

> Note that the destructor of `Turtle` **must be virtual**, as is the case for all classes you intend to inherit from - otherwise the destructor of the derived class will not be called when you delete an object through a base pointer, and you’ll get corrupted program states like memory leaks.

You can control whether the turtle’s movement will leave a trace using `PenUp()` and `PenDown()`, and control its movement using `Forward()`, `Turn()`, and `GoTo()`. Finally, `GetX()` and `GetY()` tell you the current position of the turtle.

**Your program will normally use a real implementation of this interface. In tests, you can use a mock implementation instead.** This allows you to easily check what drawing primitives your program is calling, with what arguments, and in which order. Tests written this way are much more robust (they won’t break because your new machine does anti-aliasing differently), easier to read and maintain (the intent of a test is expressed in the code, not in some binary images), and run much, much faster.

### Writing the Mock Class

If you are lucky, the mocks you need to use have already been implemented by some nice people. If, however, you find yourself in the position to write a mock class, relax - **gMock** turns this task into a fun game! (Well, almost.)

#### How to Define It

Using the `Turtle` interface as example, here are the simple steps you need to follow:

* Derive a class `MockTurtle` from `Turtle`.

* Take a `virtual` function of `Turtle` (while it’s possible to [mock non-virtual methods using templates](https://google.github.io/googletest/gmock_cook_book.html#MockingNonVirtualMethods), it’s much more involved).

* In the public: section of the child class, write `MOCK_METHOD()`;

* Now comes the fun part: you take the function signature, cut-and-paste it into the macro, and add two commas - one between the return type and the name, another between the name and the argument list.

* If you’re mocking a **const** method, add a 4th parameter containing `(const)` (the parentheses are required).

* Since you’re overriding a `virtual` method, we suggest adding the `override` keyword. For **const** methods the 4th parameter becomes `(const, override)`, for non-const methods just `(override)`. **This isn’t mandatory**.

* Repeat until all `virtual` functions you want to mock are done. (It goes without saying that **all pure virtual methods** in your abstract class must be either mocked or overridden.)

After the process, you should have something like:

``` cpp
#include <gmock/gmock.h>  // Brings in gMock.

class MockTurtle : public Turtle {
 public:
  ...
  MOCK_METHOD(void, PenUp, (), (override));
  MOCK_METHOD(void, PenDown, (), (override));
  MOCK_METHOD(void, Forward, (int distance), (override));
  MOCK_METHOD(void, Turn, (int degrees), (override));
  MOCK_METHOD(void, GoTo, (int x, int y), (override));
  MOCK_METHOD(int, GetX, (), (const, override));
  MOCK_METHOD(int, GetY, (), (const, override));
};
```

You don’t need to define these mock methods somewhere else - the `MOCK_METHOD` macro **will generate the definitions** for you. It’s that simple!

#### Where to Put It

When you define a mock class, you need to decide where to put its definition. Some people put it in a `_test.cc`. This is fine when the interface being mocked (say, `Foo`) is owned by the same person or team. Otherwise, when the owner of `Foo` changes it, your test could break. (You can’t really expect Foo’s maintainer to fix every test that uses `Foo`, can you?)

**Generally, you should not mock classes you don’t own**. If you must mock such a class owned by others, define the mock class in Foo’s Bazel package (usually the same directory or a testing sub-directory), and put it in a `.h` and a `cc_library` with `testonly=True`. Then everyone can reference them from their tests. If `Foo` ever changes, there is only one copy of `MockFoo` to change, and only tests that depend on the changed methods need to be fixed.

Another way to do it: you can introduce a thin layer `FooAdaptor` on top of `Foo` and code to this new interface. Since you own `FooAdaptor`, you can absorb changes in `Foo` much more easily. While this is more work initially, carefully choosing the adaptor interface can make your code easier to write and more readable (a net win in the long run), as you can choose `FooAdaptor` to fit your specific domain much better than Foo does.

> 在使用 gmock 创建 mock 类时，应该如何选择放置 mock 类定义的位置？

1. **放在 `_test.cc` 文件中**：如果你正在 mock 的接口（比如 `Foo`）是由同一个人或团队拥有的，那么将 mock 类定义放在 `_test.cc` 文件中是可以的。但是，如果 `Foo` 的所有者更改了它，你的测试可能会失败。因为你不能真正期望 `Foo` 的维护者修复每一个使用 `Foo` 的测试。

2. **不要 mock 不属于你的类**：这是一个一般性的建议。如果你必须 mock 一个由其他人拥有的类，那么应该在 `Foo` 的 Bazel 包（通常是同一个目录或一个 `testing` 子目录）中定义 mock 类，并将其放在一个 `.h` 文件和一个带有 `testonly=True` 的 `cc_library` 中。这样，每个人都可以从他们的测试中引用它们。如果 `Foo` 发生了变化，只需要更改一份 `MockFoo` 的副本，只有依赖于改变的方法的测试需要被修复。

3. **引入一个薄层适配器**：你可以在 `Foo` 之上引入一个薄层适配器 `FooAdaptor`，并针对这个新接口进行编码。由于你拥有 `FooAdaptor`，你可以更容易地吸收 `Foo` 的变化。虽然这在初始阶段需要更多的工作，但是仔细选择适配器接口可以使你的代码更易于编写和阅读（从长远来看，这是一个净胜利），因为你可以选择 `FooAdaptor` 更好地适应你的特定领域，比 `Foo` 做得更好。


### Using Mocks in Tests

Once you have a mock class, using it is easy. The typical work flow is:

1. Import the `gMock` names from the `testing` namespace such that you can use them unqualified (You only have to do it once per file). Remember that namespaces are a good idea.
2. Create some mock objects.
3. Specify your expectations on them (How many times will a method be called? With what arguments? What should it do? etc.).
4. Exercise some code that uses the mocks; optionally, check the result using googletest assertions. If a mock method is called more than expected or with wrong arguments, you’ll get an error immediately.
5. When a mock is destructed, `gMock` will automatically check whether all expectations on it have been satisfied.

Here’s an example:

``` cpp
#include "path/to/mock-turtle.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::AtLeast;                         // #1

TEST(PainterTest, CanDrawSomething) {
  MockTurtle turtle;                              // #2
  EXPECT_CALL(turtle, PenDown())                  // #3
      .Times(AtLeast(1));

  Painter painter(&turtle);                       // #4

  EXPECT_TRUE(painter.DrawCircle(0, 0, 10));      // #5
}
```

As you might have guessed, this test checks that `PenDown()` is called at least once. If the painter object didn’t call this method, your test will fail with a message like this:

```
path/to/my_test.cc:119: Failure
Actual function call count doesn't match this expectation:
Actually: never called;
Expected: called at least once.
Stack trace:
...
```






## [gMock Cookbook](https://google.github.io/googletest/gmock_cook_book.html)

You can find recipes for using gMock here. If you haven’t yet, please read [the dummy guide](https://google.github.io/googletest/gmock_for_dummies.html) first to make sure you understand the basics.

> **Note**: gMock lives in the testing name space. For readability, it is recommended to write `using ::testing::Foo`; once in your file before using the name `Foo` defined by gMock. We omit such `using` statements in this section for brevity, but you should do it in your own code.

### Creating Mock Classes

Mock classes are defined as normal classes, using the `MOCK_METHOD` macro to generate mocked methods. The macro gets 3 or 4 parameters:

``` cpp
class MyMock {
 public:
  MOCK_METHOD(ReturnType, MethodName, (Args...));
  MOCK_METHOD(ReturnType, MethodName, (Args...), (Specs...));
};
```

The first 3 parameters are simply the method declaration, split into 3 parts. The 4th parameter accepts a closed list of qualifiers, which affect the generated method:

* **const** - Makes the mocked method a `const` method. Required if overriding a `const` method.
* **override** - Marks the method with `override`. Recommended if overriding a `virtual` method.
* **noexcept** - Marks the method with `noexcept`. Required if overriding a `noexcept` method.
* **Calltype(...)** - Sets the call type for the method (e.g. to `STDMETHODCALLTYPE`), useful in Windows.
* **ref(...)** - Marks the method with the reference qualification specified. Required if overriding a method that has reference qualifications. Eg `ref(&)` or `ref(&&)`.


#### Dealing with unprotected commas

Unprotected commas, i.e. commas which are not surrounded by parentheses, prevent `MOCK_METHOD` from parsing its arguments correctly:

![gtest1](/assets/images/202502/gtest1.png)


#### Mocking Private or Protected Methods

You must always put a mock method definition (`MOCK_METHOD`) in a `public`: section of the mock class, regardless of the method being mocked being `public`, `protected`, or `private` in the base class. This allows `ON_CALL` and `EXPECT_CALL` to reference the mock function from outside of the mock class. (Yes, C++ allows a subclass to change the access level of a virtual function in the base class.) Example:

``` cpp
class Foo {
 public:
  ...
  virtual bool Transform(Gadget* g) = 0;

 protected:
  virtual void Resume();

 private:
  virtual int GetTimeOut();
};

class MockFoo : public Foo {
 public:
  ...
  MOCK_METHOD(bool, Transform, (Gadget* g), (override));

  // The following must be in the public section, even though the
  // methods are protected or private in the base class.
  MOCK_METHOD(void, Resume, (), (override));
  MOCK_METHOD(int, GetTimeOut, (), (override));
};
```

#### Mocking Overloaded Methods

You can mock overloaded functions as usual. No special attention is required:

``` cpp
class Foo {
  ...

  // Must be virtual as we'll inherit from Foo.
  virtual ~Foo();

  // Overloaded on the types and/or numbers of arguments.
  virtual int Add(Element x);
  virtual int Add(int times, Element x);

  // Overloaded on the const-ness of this object.
  virtual Bar& GetBar();
  virtual const Bar& GetBar() const;
};

class MockFoo : public Foo {
  ...
  MOCK_METHOD(int, Add, (Element x), (override));
  MOCK_METHOD(int, Add, (int times, Element x), (override));

  MOCK_METHOD(Bar&, GetBar, (), (override));
  MOCK_METHOD(const Bar&, GetBar, (), (const, override));
};
```

> **Note**: if you don’t mock all versions of the overloaded method, the compiler will give you a warning about some methods in the base class being hidden. To fix that, use using to bring them in scope:

``` cpp
class MockFoo : public Foo {
  ...
  using Foo::Add;
  MOCK_METHOD(int, Add, (Element x), (override));
  // We don't want to mock int Add(int times, Element x);
  ...
};
```

#### Mocking Class Templates

You can mock class templates just like any class.

``` cpp
template <typename Elem>
class StackInterface {
  ...
  // Must be virtual as we'll inherit from StackInterface.
  virtual ~StackInterface();

  virtual int GetSize() const = 0;
  virtual void Push(const Elem& x) = 0;
};

template <typename Elem>
class MockStack : public StackInterface<Elem> {
  ...
  MOCK_METHOD(int, GetSize, (), (const, override));
  MOCK_METHOD(void, Push, (const Elem& x), (override));
};
```

#### Mocking Non-virtual Methods

gMock can mock non-virtual functions to be used in Hi-perf dependency injection.

In this case, instead of sharing a common base class with the real class, your mock class will be unrelated to the real class, but contain methods with the same signatures. The syntax for mocking non-virtual methods is the same as mocking virtual methods (just don’t add `override`):

``` cpp
// A simple packet stream class.  None of its members is virtual.
class ConcretePacketStream {
 public:
  void AppendPacket(Packet* new_packet);
  const Packet* GetPacket(size_t packet_number) const;
  size_t NumberOfPackets() const;
  ...
};

// A mock packet stream class.  It inherits from no other, but defines
// GetPacket() and NumberOfPackets().
class MockPacketStream {
 public:
  MOCK_METHOD(const Packet*, GetPacket, (size_t packet_number), (const));
  MOCK_METHOD(size_t, NumberOfPackets, (), (const));
  ...
};
```

Note that the mock class doesn’t define `AppendPacket()`, unlike the real class. **That’s fine as long as the test doesn’t need to call it**.

Next, you need a way to say that you want to use `ConcretePacketStream` in production code, and use `MockPacketStream` in tests. Since the functions are not virtual and the two classes are unrelated, you must specify your choice at compile time (as opposed to run time).

One way to do it is to templatize your code that needs to use a packet stream. More specifically, you will give your code a template type argument for the type of the packet stream. In production, you will instantiate your template with `ConcretePacketStream` as the type argument. In tests, you will instantiate the same template with `MockPacketStream`. For example, you may write:

``` cpp
template <class PacketStream>
void CreateConnection(PacketStream* stream) { ... }

template <class PacketStream>
class PacketReader {
 public:
  void ReadPackets(PacketStream* stream, size_t packet_num);
};
```

Then you can use `CreateConnection<ConcretePacketStream>()` and `PacketReader<ConcretePacketStream>` in production code, and use `CreateConnection<MockPacketStream>()` and `PacketReader<MockPacketStream>` in tests.

```
  MockPacketStream mock_stream;
  EXPECT_CALL(mock_stream, ...)...;
  .. set more expectations on mock_stream ...
  PacketReader<MockPacketStream> reader(&mock_stream);
  ... exercise reader ...
```

#### Mocking Free Functions

It is not possible to directly mock a free function (i.e. **a C-style function** or **a static method**). If you need to, you can rewrite your code to use an interface (abstract class).

Instead of calling a free function (say, `OpenFile`) directly, introduce an interface for it and have a concrete subclass that calls the free function:

``` cpp
class FileInterface {
 public:
  ...
  virtual bool Open(const char* path, const char* mode) = 0;
};

class File : public FileInterface {
 public:
  ...
  bool Open(const char* path, const char* mode) override {
     return OpenFile(path, mode);
  }
};
```

Your code should talk to `FileInterface` to open a file. Now it’s easy to mock out the function.

This may seem like a lot of **hassle**, but in practice you often have multiple related functions that you can put in the same interface, so the per-function syntactic overhead will be much lower.

If you are concerned about the performance overhead incurred by virtual functions, and profiling confirms your concern, you can combine this with the recipe for [mocking non-virtual methods](https://google.github.io/googletest/gmock_cook_book.html#MockingNonVirtualMethods).

Alternatively, instead of introducing a new interface, you can rewrite your code to accept a `std::function` **instead of the free function**, and then use [MockFunction](https://google.github.io/googletest/gmock_cook_book.html#MockFunction) to mock the `std::function`.


#### Old-Style `MOCK_METHODn` Macros

Before the generic `MOCK_METHOD` macro [was introduced in 2018](https://github.com/google/googletest/commit/c5f08bf91944ce1b19bcf414fa1760e69d20afc2), mocks where created using a family of macros collectively called `MOCK_METHODn`. These macros are still supported, though migration to the new `MOCK_METHOD` is recommended.

The macros in the `MOCK_METHODn` family differ from `MOCK_METHOD`:

* The general structure is `MOCK_METHODn(MethodName, ReturnType(Args))`, instead of `MOCK_METHOD(ReturnType, MethodName, (Args))`.

* The number `n` must equal the number of arguments.

* When mocking a `const` method, one must use `MOCK_CONST_METHODn`.

* When mocking a class template, the macro name must be suffixed with `_T`.

* In order to specify the call type, the macro name must be suffixed with `_WITH_CALLTYPE`, and the call type is the first macro argument.

Old macros and their new equivalents:

![gtest2](/assets/images/202502/gtest2.png)


## [gMock Cheat Sheet](https://google.github.io/googletest/gmock_cheat_sheet.html) (用法速查表)


gMock Cheat Sheet相比[gMock Cookbook](https://google.github.io/googletest/gmock_cook_book.html) 提供了更精简的内容。



## Demo

* https://github.com/bast/gtest-demo/tree/master
* [CMake: Project structure with unit tests](https://stackoverflow.com/questions/14446495/cmake-project-structure-with-unit-tests)


## Q&A

### [Setup/TearDown/SetUpTestCase/TearDownTestCase 的区别](https://blog.csdn.net/carolzhang8406/article/details/54668319)

### [Can you test SetUp success/failure in Google Test](https://stackoverflow.com/questions/20636539/can-you-test-setup-success-failure-in-google-test)

If you put your fixture setup code into a `SetUp` method, and it fails and issues a fatal failure (ASSERT_XXX or FAIL macros), Google Test will not run your test body. So all you have to write is

``` cpp
class MyTestCase : public testing::Test {
 protected:
  bool InitMyTestData() { ... }

  virtual void SetUp() {
    ASSERT_TRUE(InitMyTestData());
  }
};

TEST_F(MyTestCase, Foo) { ... }
```

More: [Checking for Failures in the Current Test](http://google.github.io/googletest/advanced.html#checking-for-failures-in-the-current-test)


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

## Gcov Intro

> gcov是什么


* `gcov`是一个测试代码覆盖率的工具。与`GCC`一起使用来分析程序，以帮助创建更高效、更快的运行代码，并发现程序的未测试部分。
* 是一个命令行方式的控制台程序。需要结合`lcov`，`gcovr`等前端图形工具能实现统计数据图形化。
* 伴随`GCC`发布，不需要单独下载`gcov`工具。配合`GCC`共同实现对`C/C++`文件的语句覆盖和分支覆盖测试。
* 与程序概要分析工具（Profiling tool，例如`gprof`）一起工作，可以估计程序中哪段代码最耗时。

> gcov能做什么

* 每一行代码执行的频率是多少。
* 实际执行了哪些行代码，配合测试用例达到满意的覆盖率和预期工作。
* `gcov`创建一个`sourcefile.gcov`的日志文件，此文件标识源文件`sourcefile.c`每一行执行的次数。
* `gcov`只在使用`GCC`编译的代码上工作。



[gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) is a tool you can use in conjunction(结合) with `GCC` to test code coverage in your programs.

`gcov` is a test coverage program. Use it in concert with GCC to analyze your programs to help create more efficient, faster running code and to discover untested parts of your program. You can use `gcov` as a profiling tool to help discover where your optimization efforts will best affect your code. You can also use `gcov` along with the other profiling tool, `gprof`, to assess which parts of your code use the greatest amount of computing time.

Profiling tools help you analyze your code’s performance. Using a profiler such as `gcov` or `gprof`, you can find out some basic performance statistics, such as:

* how often each line of code executes
* what lines of code are actually executed
* how much computing time each section of code uses

Once you know these things about how your code works when compiled, you can look at each module to see which modules should be optimized. **`gcov` helps you determine where to work on optimization.**

Software developers also use coverage testing in concert with `testsuites`, to make sure software is actually good enough for a release. Testsuites can verify that a program works as expected; a coverage program tests to see how much of the program is exercised by the testsuite. Developers can then determine what kinds of test cases need to be added to the testsuites to create both better testing and a better final product.

**You should compile your code without optimization if you plan to use `gcov` because the optimization, by combining some lines of code into one function, may not give you as much information as you need to look for ‘hot spots’ where the code is using a great deal of computer time.** Likewise, because `gcov` accumulates statistics by line (at the lowest resolution), **it works best with a programming style that places only one statement on each line. If you use complicated macros that expand to loops or to other control structures, the statistics are less helpful—they only report on the line where the macro call appears.** If your complex macros behave like functions, you can replace them with inline functions to solve this problem.

`gcov` creates a logfile called `sourcefile.gcov` which indicates how many times each line of a source file `sourcefile.c` has executed. You can use these logfiles along with `gprof` to aid in fine-tuning the performance of your programs. `gprof` gives timing information you can use along with the information you get from `gcov`.

**`gcov` works only on code compiled with `GCC`. It is not compatible with any other profiling or test coverage mechanism.**


```
# ubuntu
sudo apt-get install -y gcovr
```

## Procedure

![gcov_procedure](/assets/images/202203/gcov_procedure.png)

1. 编译前，在编译器中加入`-fprofile-arcs`和`-ftest-coverage`编译参数
2. 源码文件(`.c/.cc`)经过编译预处理，再编译生成汇编文件(`.s`)，在生成汇编文件的同时完成插桩
3. 执行可执行文件，在运行过程中之前插入桩点负责收集程序的执行信息。所谓桩点，其实就是一个变量，内存中的一个格子，对应的代码执行一次，则其值增加一次
4. 生成`.gcda`文件，其中有执行统计次数等，由此经过加工可得到覆盖率

PS:
* `-ftest-coverage` tells gcc to dump the notes file for each source file as SOURCE.gcno
* `-fprofile-arcs` the flag to instrument the code, basically adding a 64-bit counter in each basic-block
* `.gcno` 包含了代码计数器和源码的映射关系 (**编译时生成**)
* `.gcda` 记录了每段代码具体的执行次数（**执行时生成**）



## Invoking gcov

> gcov [options] files

refer: https://gcc.gnu.org/onlinedocs/gcc/Invoking-Gcov.html#Invoking-Gcov

`gcov` should be run with the current directory the same as that when you invoked the compiler. Otherwise it will not be able to locate the source files. `gcov` produces files called `mangledname.gcov` in the current directory. These contain the coverage information of the source file they correspond to. One `.gcov` file is produced for each source (or header) file containing code, which was compiled to produce the data files. The **mangledname** part of the output file name is usually simply the source file name, but can be something more complicated if the ‘-l’ or ‘-p’ options are given. Refer to those options for details.

If you invoke gcov with multiple input files, the contributions from each input file are summed. Typically you would invoke it with the same list of files as the final link of your executable.

The `.gcov` files contain the ‘:’ separated fields along with program source code. The format is

> execution_count:line_number:source line text

Additional block information may succeed each line, when requested by command line option. The execution_count is ‘-’ for lines containing no code. Unexecuted lines are marked ‘#####’ or ‘=====’, depending on whether they are reachable by non-exceptional paths or only exceptional paths such as C++ exception handlers, respectively. Given the ‘-a’ option, unexecuted blocks are marked ‘$$$$$’ or ‘%%%%%’, depending on whether a basic block is reachable via non-exceptional or exceptional paths. Executed basic blocks having a statement with zero execution_count end with ‘*’ character and are colored with magenta color with the -k option. This functionality is not supported in Ada.

**Note that GCC can completely remove the bodies of functions that are not needed – for instance if they are inlined everywhere.** Such functions are marked with ‘-’, which can be confusing. Use the `-fkeep-inline-functions` and `-fkeep-static-functions` options to retain these functions and allow gcov to properly show their execution_count.

Some lines of information at the start have line_number of zero. These preamble lines are of the form

> -:0:tag:value

The ordering and number of these preamble lines will be augmented as gcov development progresses — do not rely on them remaining unchanged. Use tag to locate a particular preamble line.

The additional block information is of the form

> tag information

The information is human readable, but designed to be simple enough for machine parsing too.

When printing percentages, 0% and 100% are only printed when the values are exactly 0% and 100% respectively. Other values which would conventionally be rounded to 0% or 100% are instead printed as the nearest non-boundary value.

**When using `gcov`, you must first compile your program with a special GCC option ‘--coverage’. This tells the compiler to generate additional information needed by `gcov` (basically a flow graph of the program) and also includes additional code in the object files for generating the extra profiling information needed by `gcov`. These additional files are placed in the directory where the object file is located.**

Running the program will cause profile output to be generated. For each source file compiled with `-fprofile-arcs`, an accompanying `.gcda` file will be placed in the object file directory.

Running `gcov` with your program’s source file names as arguments will now produce a listing of the code along with frequency of execution for each line. For example, if your program is called `tmp.cpp`, this is what you see when you use the basic `gcov` facility:

```
$ g++ --coverage tmp.cpp -c
$ g++ --coverage tmp.o
$ a.out
$ gcov tmp.cpp -m
File 'tmp.cpp'
Lines executed:92.86% of 14
Creating 'tmp.cpp.gcov'
```

The file `tmp.cpp.gcov` contains output from `gcov`. Here is a sample:

```
        -:    0:Source:tmp.cpp
        -:    0:Working directory:/home/gcc/testcase
        -:    0:Graph:tmp.gcno
        -:    0:Data:tmp.gcda
        -:    0:Runs:1
        -:    0:Programs:1
        -:    1:#include <stdio.h>
        -:    2:
        -:    3:template<class T>
        -:    4:class Foo
        -:    5:{
        -:    6:  public:
       1*:    7:  Foo(): b (1000) {}
------------------
Foo<char>::Foo():
    #####:    7:  Foo(): b (1000) {}
------------------
Foo<int>::Foo():
        1:    7:  Foo(): b (1000) {}
------------------
       2*:    8:  void inc () { b++; }
------------------
Foo<char>::inc():
    #####:    8:  void inc () { b++; }
------------------
Foo<int>::inc():
        2:    8:  void inc () { b++; }
------------------
        -:    9:
        -:   10:  private:
        -:   11:  int b;
        -:   12:};
        -:   13:
        -:   14:template class Foo<int>;
        -:   15:template class Foo<char>;
        -:   16:
        -:   17:int
        1:   18:main (void)
        -:   19:{
        -:   20:  int i, total;
        1:   21:  Foo<int> counter;
        -:   22:
        1:   23:  counter.inc();
        1:   24:  counter.inc();
        1:   25:  total = 0;
        -:   26:
       11:   27:  for (i = 0; i < 10; i++)
       10:   28:    total += i;
        -:   29:
       1*:   30:  int v = total > 100 ? 1 : 2;
        -:   31:
        1:   32:  if (total != 45)
    #####:   33:    printf ("Failure\n");
        -:   34:  else
        1:   35:    printf ("Success\n");
        1:   36:  return 0;
        -:   37:}
```

Note that line 7 is shown in the report multiple times. First occurrence presents total number of execution of the line and the next two belong to instances of class Foo constructors. As you can also see, line 30 contains some unexecuted basic blocks and thus execution count has asterisk symbol.

When you use the -b option, your output looks like this:

```
        -:    0:Source:tmp.cpp
        -:    0:Working directory:/home/gcc/testcase
        -:    0:Graph:tmp.gcno
        -:    0:Data:tmp.gcda
        -:    0:Runs:1
        -:    0:Programs:1
        -:    1:#include <stdio.h>
        -:    2:
        -:    3:template<class T>
        -:    4:class Foo
        -:    5:{
        -:    6:  public:
       1*:    7:  Foo(): b (1000) {}
------------------
Foo<char>::Foo():
function Foo<char>::Foo() called 0 returned 0% blocks executed 0%
    #####:    7:  Foo(): b (1000) {}
------------------
Foo<int>::Foo():
function Foo<int>::Foo() called 1 returned 100% blocks executed 100%
        1:    7:  Foo(): b (1000) {}
------------------
       2*:    8:  void inc () { b++; }
------------------
Foo<char>::inc():
function Foo<char>::inc() called 0 returned 0% blocks executed 0%
    #####:    8:  void inc () { b++; }
------------------
Foo<int>::inc():
function Foo<int>::inc() called 2 returned 100% blocks executed 100%
        2:    8:  void inc () { b++; }
------------------
        -:    9:
        -:   10:  private:
        -:   11:  int b;
        -:   12:};
        -:   13:
        -:   14:template class Foo<int>;
        -:   15:template class Foo<char>;
        -:   16:
        -:   17:int
function main called 1 returned 100% blocks executed 81%
        1:   18:main (void)
        -:   19:{
        -:   20:  int i, total;
        1:   21:  Foo<int> counter;
call    0 returned 100%
branch  1 taken 100% (fallthrough)
branch  2 taken 0% (throw)
        -:   22:
        1:   23:  counter.inc();
call    0 returned 100%
branch  1 taken 100% (fallthrough)
branch  2 taken 0% (throw)
        1:   24:  counter.inc();
call    0 returned 100%
branch  1 taken 100% (fallthrough)
branch  2 taken 0% (throw)
        1:   25:  total = 0;
        -:   26:
       11:   27:  for (i = 0; i < 10; i++)
branch  0 taken 91% (fallthrough)
branch  1 taken 9%
       10:   28:    total += i;
        -:   29:
       1*:   30:  int v = total > 100 ? 1 : 2;
branch  0 taken 0% (fallthrough)
branch  1 taken 100%
        -:   31:
        1:   32:  if (total != 45)
branch  0 taken 0% (fallthrough)
branch  1 taken 100%
    #####:   33:    printf ("Failure\n");
call    0 never executed
branch  1 never executed
branch  2 never executed
        -:   34:  else
        1:   35:    printf ("Success\n");
call    0 returned 100%
branch  1 taken 100% (fallthrough)
branch  2 taken 0% (throw)
        1:   36:  return 0;
        -:   37:}
```

For each function, a line is printed showing how many times the function is called, how many times it returns and what percentage of the function’s blocks were executed.

**The execution counts are cumulative**. If the example program were executed again without removing the `.gcda` file, the count for the number of times each line in the source was executed would be added to the results of the previous run(s). This is potentially useful in several ways. For example, it could be used to accumulate data over a number of program runs as part of a test verification suite, or to provide more accurate long-term information over a large number of program runs.

The data in the `.gcda` files is saved immediately before the program exits. For each source file compiled with `-fprofile-arcs`, the profiling code first attempts to read in an existing `.gcda` file; if the file doesn’t match the executable (differing number of basic block counts) it will ignore the contents of the file. It then adds in the new execution counts and finally writes the data to the file.

## Data File Relocation to Support Cross-Profiling

To support cross-profiling, a program compiled with -fprofile-arcs can relocate the data files based on two environment variables:

* `GCOV_PREFIX` contains the prefix to add to the absolute paths in the object file. Prefix can be absolute, or relative. The default is no prefix.
* `GCOV_PREFIX_STRIP` indicates the how many initial directory names to strip off the hardwired absolute paths. Default value is 0.

Note: If `GCOV_PREFIX_STRIP` is set without `GCOV_PREFIX` is undefined, then a relative path is made out of the hardwired absolute paths.

For example, if the object file `/user/build/foo.o` was built with `-fprofile-arcs`, the final executable will try to create the data file `/user/build/foo.gcda` when running on the target system. This will fail if the corresponding directory does not exist and it is unable to create it. This can be overcome by, for example, setting the environment as `GCOV_PREFIX=/target/run` and `GCOV_PREFIX_STRIP=1`. Such a setting will name the data file `/target/run/build/foo.gcda`.

You must move the data files to the expected directory tree in order to use them for profile directed optimizations (`-fprofile-use`), or to use the `gcov` tool.

refer:

* [Can GCOV create .gcda file in the different directory structure?](https://stackoverflow.com/questions/51414640/can-gcov-create-gcda-file-in-the-different-directory-structure)
* https://gcc.gnu.org/onlinedocs/gcc/Cross-profiling.html
* [HOWTO: Dumping gcov data at runtime - simple example](https://www.osadl.org/Dumping-gcov-data-at-runtime-simple-ex.online-coverage-analysis.0.html)

# [Clang Source-based Code Coverage](https://clang.llvm.org/docs/SourceBasedCodeCoverage.html)

This document explains how to use clang’s source-based code coverage feature. It’s called “source-based” because it operates on AST and preprocessor information directly. This allows it to generate very precise coverage data.

Clang ships two other code coverage implementations:

* [SanitizerCoverage](https://clang.llvm.org/docs/SanitizerCoverage.html) - A low-overhead tool meant for use alongside the various sanitizers. It can provide up to edge-level coverage.
* gcov - A GCC-compatible coverage implementation which operates on DebugInfo. This is enabled by `-ftest-coverage` or `--coverage`.

From this point onwards “code coverage” will refer to the source-based kind.

> 注意：clang12 使用 gcov 链接库符号冲突问题

```
[turbo client] /usr/bin/ld: /usr/lib64/clang/12.0.1/lib/linux/libclang_rt.profile-x86_64.a(GCDAProfiling.c.o): in function `__gcov_fork':
[turbo client] (.text+0xd00): multiple definition of `__gcov_fork'; /usr/lib/gcc/x86_64-redhat-linux/8/libgcov.a(_gcov_fork.o):(.text+0x0): first defin
ed here
[turbo client] /usr/bin/ld: /usr/lib64/clang/12.0.1/lib/linux/libclang_rt.profile-x86_64.a(GCDAProfiling.c.o): in function `__gcov_dump':
[turbo client] (.text+0xdd0): multiple definition of `__gcov_dump'; /usr/lib/gcc/x86_64-redhat-linux/8/libgcov.a(_gcov_dump.o):(.text+0x50): first defi
ned here
[turbo client] /usr/bin/ld: /usr/lib64/clang/12.0.1/lib/linux/libclang_rt.profile-x86_64.a(GCDAProfiling.c.o): in function `__gcov_reset':
[turbo client] (.text+0xe00): multiple definition of `__gcov_reset'; /usr/lib/gcc/x86_64-redhat-linux/8/libgcov.a(_gcov_reset.o):(.text+0x100): first d
efined here
[turbo client] clang-12: error: linker command failed with exit code 1 (use -v to see invocation)
```

gcc 的使用方式：编译选项增加 `-ftest-coverage` 和 `-fprofile-arcs`，并增加 `-lgcov` 链接选项。在 clang 3.5.2 使用同样方式也没有问题，而使用 clang12 则出现下述链接错误。后在 clang12 改为只使用编译选项 `--coverage`，功能正常。



# Q&A

## [Dozens of "profiling:invalid arc tag" when running code coverage in Xcode 5](https://stackoverflow.com/questions/22519530/dozens-of-profilinginvalid-arc-tag-when-running-code-coverage-in-xcode-5)

Most likely this is a result of the build tools failing to merge current results into the existing `.gcda` coverage files. As [Dave Meehan points out here](http://davemeehan.com/technology/xcode/how-to-fix-profiler-invalid-magic-number-in-xcode-4-6-when-generate-test-coverage-files-is-enabled), there is a brute force way of dealing with this by cleaning the product build folder, but a less hard core approach is to delete the `.gcda` files from targets generating them (for me, just the test target) as part of the build process. Dave includes a sample script to be included as a build phase -- or, at the project root by hand:

``` bash
find . -name "*.gcda" -print0 | xargs -0 rm
```





