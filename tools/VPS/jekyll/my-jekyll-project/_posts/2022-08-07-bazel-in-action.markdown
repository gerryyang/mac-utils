---
layout: post
title:  "Bazel in Action"
date:   2022-08-07 18:30:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}

# 构建基础知识

观看一段简短的历史记录，了解基于工件的构建系统如何发展，以实现规模、速度和封闭性。

* [为什么使用构建系统？](https://bazel.build/basics/build-systems)
* [基于任务的构建系统](https://bazel.build/basics/task-based-builds)
* [基于工件的构建系统](https://bazel.build/basics/artifact-based-builds)
* [分布式构建](https://bazel.build/basics/distributed-builds)
* [依赖项管理](https://bazel.build/basics/dependencies)

# 基本概念

了解源代码布局、BUILD 文件语法以及规则和依赖项类型等基本概念。

* [工作区、软件包和目标](https://bazel.build/concepts/build-ref)

Bazel 会根据名为“工作区”的目录树整理的源代码构建软件。工作区中的源文件以嵌套的软件包层次结构进行组织，其中每个软件包都是一个包含一组相关源文件和一个 BUILD 文件的目录。BUILD 文件指定可以从源代码构建哪些软件输出。



# Bazel

[Bazel](https://bazel.build/) is an open-source build and test tool similar to `Make`, `Maven`, and `Gradle`. **It uses a human-readable, high-level build language.** `Bazel` supports projects in multiple languages and builds outputs for multiple platforms. `Bazel` supports large codebases across multiple repositories, and large numbers of users.


# Install Bazel (CentOS)

[Bazel 安装说明](https://bazel.build/install)，本文使用方式三。

方式一：安装包

```
yum install bazel4
```

方式二：[源码编译](https://bazel.build/install/compile-source)

方式三：[使用release版本](https://github.com/bazelbuild/bazel/releases)


# Bazel 的优势

Bazel offers the following advantages:

* **High-level build language**. Bazel uses an abstract, human-readable language to describe the build properties of your project at a high semantical level. Unlike other tools, Bazel operates on the concepts of libraries, binaries, scripts, and data sets, shielding you from the complexity of writing individual calls to tools such as compilers and linkers.
* **Bazel is fast and reliable**. Bazel caches all previously done work and tracks changes to both file content and build commands. This way, Bazel knows when something needs to be rebuilt, and rebuilds only that. To further speed up your builds, you can set up your project to build in a highly parallel and incremental fashion.
* **Bazel is multi-platform**. Bazel runs on Linux, macOS, and Windows. Bazel can build binaries and deployable packages for multiple platforms, including desktop, server, and mobile, from the same project.
* **Bazel scales**. Bazel maintains agility(敏捷) while handling builds with `100k+` source files. It works with multiple repositories and user bases in the tens of thousands.
* **Bazel is extensible**. Many languages are supported, and you can extend Bazel to support any other language or framework.

一些测试结论：

* `bazel`默认会限制并发度到其估计的机器性能上限，实际使用需要通过--local_cpu_resources=9999999等参数绕过这一限制
* 已知（部分版本的）bazel在并发度过高（如`-j320`）下，bazel自身性能存在瓶颈。这具体表现为机器空闲但不会启动更多编译任务，同时bazel自身CPU（`400~500%`）、内存（几G）占用很高。
* 如果机器资源充足且对并发度有较高要求（几百并发），可以考虑使用其他构建系统构建。

# 使用 Bazel

To build or test a project with Bazel, you typically do the following:

1. **Set up Bazel**. [Download and install Bazel](https://docs.bazel.build/versions/4.2.1/install.html).
2. **Set up a project [workspace](https://docs.bazel.build/versions/4.2.1/build-ref.html#workspaces)**, which is a directory where Bazel looks for build inputs and `BUILD` files, and where it stores build outputs.
3. **Write a BUILD file**, which tells Bazel what to build and how to build it.
  + You write your `BUILD` file by declaring build targets using [Starlark](https://docs.bazel.build/versions/4.2.1/skylark/language.html), a domain-specific language. (See example [here](https://github.com/bazelbuild/bazel/blob/master/examples/cpp/BUILD).)
  + A build target specifies a set of input artifacts that Bazel will build plus their dependencies, the build rule Bazel will use to build it, and options that configure the build rule.
  + A build rule specifies the build tools Bazel will use, such as compilers and linkers, and their configurations. Bazel ships with a number of build rules covering the most common artifact types in the supported languages on supported platforms.
4. **Run Bazel** from the command line. Bazel places your outputs within the workspace.

In addition to building, you can also use Bazel to run [tests](https://docs.bazel.build/versions/4.2.1/test-encyclopedia.html) and [query](https://docs.bazel.build/versions/4.2.1/query-how-to.html) the build to trace dependencies in your code.

# Bazel 构建流程

When running a build or a test, Bazel does the following:

1. **Loads** the `BUILD` files relevant to the target.
2. **Analyzes** the inputs and their dependencies, applies the specified build rules, and produces an action graph.
3. **Executes** the build actions on the inputs until the final build outputs are produced.


Since all previous build work is cached, Bazel can identify and reuse cached artifacts and only rebuild or retest what’s changed. To further enforce correctness, you can set up Bazel to run builds and tests hermetically through sandboxing, minimizing skew and maximizing reproducibility.


* [C/C++ Rules](https://docs.bazel.build/versions/master/be/c-cpp.html)


# Action Graph

The action graph represents the build artifacts, the relationships between them, and the build actions that Bazel will perform. Thanks to this graph, Bazel can track changes to file content as well as changes to actions, such as build or test commands, and know what build work has previously been done. The graph also enables you to easily trace dependencies in your code.


# Getting Started

To get started with Bazel, see [Getting Started](https://docs.bazel.build/versions/4.2.1/getting-started.html) or jump directly to the Bazel tutorials:

* [Tutorial: Build a C++ Project](https://docs.bazel.build/versions/4.2.1/tutorial/cpp.html)


# Bazel Tutorial: Build a C++ Project

Start by [installing Bazel](https://bazel.build/install), if you haven’t already. This tutorial uses Git for source control, so for best results [install Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git) as well.

Next, retrieve the sample project from Bazel's GitHub repository by running the following in your command-line tool of choice:

```
git clone https://github.com/bazelbuild/examples
```

The sample project for this tutorial is in the examples/cpp-tutorial directory.

Take a look below at how it’s structured:

There are three sets of files, each set representing a stage in this tutorial.

* In the first stage, you will build a single target residing in a single package.
* In the second stage, you will you will build both a binary and a library from a single package.
* In the third and final stage, you will build a project with multiple packages and build it with multiple targets.


```
~/github/examples$tree cpp-tutorial/
cpp-tutorial/
├── README.md
├── stage1
│   ├── main
│   │   ├── BUILD
│   │   └── hello-world.cc
│   ├── README.md
│   └── WORKSPACE
├── stage2
│   ├── main
│   │   ├── BUILD
│   │   ├── hello-greet.cc
│   │   ├── hello-greet.h
│   │   └── hello-world.cc
│   ├── README.md
│   └── WORKSPACE
└── stage3
    ├── lib
    │   ├── BUILD
    │   ├── hello-time.cc
    │   └── hello-time.h
    ├── main
    │   ├── BUILD
    │   ├── hello-greet.cc
    │   ├── hello-greet.h
    │   └── hello-world.cc
    ├── README.md
    └── WORKSPACE

7 directories, 20 files
```

## Set up the workspace

Before you can build a project, you need to set up its workspace. A workspace is a directory that holds your project's source files and Bazel's build outputs. It also contains these significant files:

* The `WORKSPACE file` , which identifies the directory and its contents as a Bazel workspace and lives at the root of the project's directory structure.
* One or more `BUILD files` , which tell Bazel how to build different parts of the project. A directory within the workspace that contains a `BUILD` file is a [package](https://bazel.build/reference/glossary#package). (More on packages later in this tutorial.)

In future projects, to designate a directory as a Bazel workspace, create an empty file named `WORKSPACE` in that directory. For the purposes of this tutorial, a `WORKSPACE` file is already present in each stage.

NOTE: When Bazel builds the project, all inputs must be in the same workspace. Files residing in different workspaces are independent of one another unless linked. More detailed information about workspace rules can be found in [this guide](https://bazel.build/reference/be/workspace).

## Understand the BUILD file

A `BUILD` file contains several different types of instructions for Bazel. Each `BUILD` file requires at least one [rule](https://bazel.build/reference/glossary#rule) as a set of instructions, which tells Bazel how to build the desired outputs, such as executable binaries or libraries. Each instance of a build rule in the `BUILD` file is called a [target](https://bazel.build/reference/glossary#target) and points to a specific set of source files and [dependencies](https://bazel.build/reference/glossary#dependency). A target can also point to other targets.

Take a look at the `BUILD` file in the `cpp-tutorial/stage1/main` directory:

```
cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
)
```

In our example, the `hello-world` target instantiates Bazel's built-in `cc_binary rule`. The rule tells Bazel to build a self-contained executable binary from the `hello-world.cc` source file with no dependencies.

Now you are familiar with some key terms, and what they mean in the context of this project and Bazel in general. In the next section, you will build and test Stage 1 of the project.

## Stage 1: single target, single package

It’s time to build the first part of the project. For a visual reference, the structure of the Stage 1 section of the project is:

```
examples
└── cpp-tutorial
    └──stage1
       ├── main
       │   ├── BUILD
       │   └── hello-world.cc
       └── WORKSPACE
```

Run the following to move to the `cpp-tutorial/stage1` directory:

```
$ cd  ../cpp-tutorial/stage1
```

Next, run:

```
$ bazel build //main:hello-world
```

In the target label, the `//main`: part is the location of the `BUILD` file relative to the root of the workspace, and `hello-world` is the target name in the `BUILD` file.

```
~/github/examples/cpp-tutorial/stage1/main$cat BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
)
```

``` cpp
// ~/github/examples/cpp-tutorial/stage1/main$cat hello-world.cc

#include <ctime>
#include <string>
#include <iostream>

std::string get_greet(const std::string& who) {
  return "Hello " + who;
}

void print_localtime() {
  std::time_t result = std::time(nullptr);
  std::cout << std::asctime(std::localtime(&result));
}

int main(int argc, char** argv) {
  std::string who = "world";
  if (argc > 1) {
    who = argv[1];
  }
  std::cout << get_greet(who) << std::endl;
  print_localtime();
  return 0;
}
```

Bazel produces something that looks like this:

```
~/github/examples/cpp-tutorial/stage1$bazel build //main:hello-world
WARNING: Ignoring JAVA_HOME, because it must point to a JDK, not a JRE.
Starting local Bazel server and connecting to it...
INFO: Analyzed target //main:hello-world (34 packages loaded, 149 targets configured).
INFO: Found 1 target...
Target //main:hello-world up-to-date:
  bazel-bin/main/hello-world
INFO: Elapsed time: 7.903s, Critical Path: 0.62s
INFO: 6 processes: 4 internal, 2 processwrapper-sandbox.
INFO: Build completed successfully, 6 total actions
```

You just built your first Bazel target. Bazel places build outputs in the `bazel-bin` directory at the root of the workspace.

```
~/github/examples/cpp-tutorial/stage1$tree
.
├── bazel-bin -> /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/0fa77b61e0ee4749e8c3bda193f05839/execroot/__main__/bazel-out/k8-fastbuild/bin
├── bazel-out -> /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/0fa77b61e0ee4749e8c3bda193f05839/execroot/__main__/bazel-out
├── bazel-stage1 -> /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/0fa77b61e0ee4749e8c3bda193f05839/execroot/__main__
├── bazel-testlogs -> /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/0fa77b61e0ee4749e8c3bda193f05839/execroot/__main__/bazel-out/k8-fastbuild/testlogs
├── main
│   ├── BUILD
│   └── hello-world.cc
├── README.md
└── WORKSPACE

5 directories, 4 files
```

```
~/github/examples/cpp-tutorial/stage1/bazel-bin$tree
.
└── main
    ├── hello-world
    ├── hello-world-2.params
    ├── hello-world.runfiles
    │   ├── __main__
    │   │   └── main
    │   │       └── hello-world -> /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/0fa77b61e0ee4749e8c3bda193f05839/execroot/__main__/bazel-out/k8-fastbuild/bin/main/hello-world
    │   └── MANIFEST
    ├── hello-world.runfiles_manifest
    └── _objs
        └── hello-world
            ├── hello-world.pic.d
            └── hello-world.pic.o

6 directories, 7 files
```

Now test your freshly built binary, which is:

```
$ bazel-bin/main/hello-world
```

This results in a printed “Hello world” message.

Here’s the dependency graph of Stage 1:

![cpp-tutorial-stage1](/assets/images/202208/cpp-tutorial-stage1.png)


> Summary: stage 1
>
> Now that you have completed your first build, you have a basic idea of how a build is structured. In the next stage, you will add complexity by adding another target.


## Stage 2: multiple build targets

While a single target is sufficient for small projects, you may want to split larger projects into multiple targets and packages. This allows for fast incremental builds – that is, Bazel only rebuilds what's changed – and speeds up your builds by building multiple parts of a project at once. This stage of the tutorial adds a target, and the next adds a package.

This is the directory you are working with for Stage 2:

```
    ├──stage2
    │  ├── main
    │  │   ├── BUILD
    │  │   ├── hello-world.cc
    │  │   ├── hello-greet.cc
    │  │   └── hello-greet.h
    │  └── WORKSPACE
```

Take a look below at the `BUILD` file in the `cpp-tutorial/stage2/main` directory:

```
cc_library(
    name = "hello-greet",
    srcs = ["hello-greet.cc"],
    hdrs = ["hello-greet.h"],
)

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
    deps = [
        ":hello-greet",
    ],
)
```

With this `BUILD` file, Bazel first builds the `hello-greet` library (using Bazel's built-in `cc_library rule`), then the `hello-world` binary. The `deps` attribute in the `hello-world` target tells Bazel that the `hello-greet` library is required to build the `hello-world` binary.

Before you can build this new version of the project, you need to change directories, switching to the `cpp-tutorial/stage2` directory by running:

```
$ cd  ../cpp-tutorial/stage2
```

Now you can build the new binary using the following familiar command:

```
$ bazel build //main:hello-world
```

Once again, Bazel produces something that looks like this:

```
~/github/examples/cpp-tutorial/stage2$bazel build //main:hello-world
WARNING: Ignoring JAVA_HOME, because it must point to a JDK, not a JRE.
Starting local Bazel server and connecting to it...
INFO: Analyzed target //main:hello-world (34 packages loaded, 152 targets configured).
INFO: Found 1 target...
Target //main:hello-world up-to-date:
  bazel-bin/main/hello-world
INFO: Elapsed time: 3.981s, Critical Path: 0.23s
INFO: 7 processes: 4 internal, 3 processwrapper-sandbox.
INFO: Build completed successfully, 7 total actions
```

Now you can test your freshly built binary, which returns another “Hello world”:

```
$ bazel-bin/main/hello-world
```

If you now modify `hello-greet.cc` and rebuild the project, **Bazel only recompiles that file**.

Looking at the dependency graph, you can see that hello-world depends on the same inputs as it did before, but the structure of the build is different:

![cpp-tutorial-stage2](/assets/images/202208/cpp-tutorial-stage2.png)

> Summary: stage 2
>
> You've now built the project with two targets. The `hello-world` target builds one source file and depends on one other target (`//main:hello-greet`), which builds two additional source files. In the next section, take it a step further and add another package.

## Stage 3: multiple packages

This next stage adds another layer of complication and builds a project with multiple packages. Take a look below at the structure and contents of the `cpp-tutorial/stage3` directory:

```
──stage3
   ├── main
   │   ├── BUILD
   │   ├── hello-world.cc
   │   ├── hello-greet.cc
   │   └── hello-greet.h
   ├── lib
   │   ├── BUILD
   │   ├── hello-time.cc
   │   └── hello-time.h
   └── WORKSPACE
```

You can see that now there are two sub-directories, and each contains a `BUILD` file. Therefore, to Bazel, the workspace now contains two packages: `lib` and `main`.

Take a look at the `lib/BUILD` file:

```
cc_library(
    name = "hello-time",
    srcs = ["hello-time.cc"],
    hdrs = ["hello-time.h"],
    visibility = ["//main:__pkg__"],
)
```

And at the `main/BUILD` file:

```
cc_library(
    name = "hello-greet",
    srcs = ["hello-greet.cc"],
    hdrs = ["hello-greet.h"],
)

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
    deps = [
        ":hello-greet",
        "//lib:hello-time",
    ],
)
```

The `hello-world` target in the main package depends on the `hello-time` target in the `lib` package (hence the target label `//lib:hello-time`) - Bazel knows this through the `deps` attribute. You can see this reflected in the dependency graph:

![cpp-tutorial-stage3](/assets/images/202208/cpp-tutorial-stage3.png)


For the build to succeed, you make the `//lib:hello-time` target in `lib/BUILD` explicitly visible to targets in `main/BUILD` using the visibility attribute. This is because by default targets are only visible to other targets in the same `BUILD` file. Bazel uses target visibility to prevent issues such as libraries containing implementation details leaking into public APIs.

Now build this final version of the project. Switch to the `cpp-tutorial/stage3` directory by running:

```
$ cd  ../cpp-tutorial/stage3
```

Once again, run the following command:

```
$ bazel build //main:hello-world
```

Bazel produces something that looks like this:

```
~/github/examples/cpp-tutorial/stage3$bazel build //main:hello-world
WARNING: Ignoring JAVA_HOME, because it must point to a JDK, not a JRE.
INFO: Analyzed target //main:hello-world (35 packages loaded, 155 targets configured).
INFO: Found 1 target...
Target //main:hello-world up-to-date:
  bazel-bin/main/hello-world
INFO: Elapsed time: 2.172s, Critical Path: 0.25s
INFO: 8 processes: 4 internal, 4 processwrapper-sandbox.
INFO: Build completed successfully, 8 total actions
```

Now test the last binary of this tutorial for a final Hello world message:

```
$ bazel-bin/main/hello-world
```

> Summary: stage 3
>
> You've now built the project as two packages with three targets and understand the dependencies between them, which equips you to go forth and build future projects with Bazel. In the next section, take a look at how to continue your Bazel journey.


## Next steps

You’ve now completed your first basic build with Bazel, but this is just the start. Here are some more resources to continue learning with Bazel:

* To keep focusing on C++, read about common [C++ build use cases](https://bazel.build/tutorials/cpp-use-cases).
* To get started with building other applications with Bazel, see the tutorials for [Java](https://bazel.build/tutorials/java), [Android application](https://bazel.build/tutorials/android-app), or [iOS application](https://bazel.build/tutorials/ios-app).
* To learn more about working with local and remote repositories, read about [external dependencies](https://bazel.build/docs/external).
* To learn more about Bazel’s other rules, see this [reference guide](https://bazel.build/rules).

> Happy building!


# Common C++ Build Use Cases

Here you will find some of the most common use cases for building C++ projects with Bazel. If you have not done so already, get started with building C++ projects with Bazel by completing the tutorial [Introduction to Bazel: Build a C++ Project](https://bazel.build/tutorials/cpp).

For information on cc_library and hdrs header files, see [cc_library](https://bazel.build/reference/be/c-cpp#cc_library).

## Including multiple files in a target

You can include multiple files in a single target with [glob](https://bazel.build/reference/be/functions#glob). For example:

```
cc_library(
    name = "build-all-the-files",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
)
```

With this target, Bazel will build all the `.cc` and `.h` files it finds in the same directory as the `BUILD` file that contains this target (excluding subdirectories).


## Using transitive includes

If a file includes a header, then any rule with that file as a source (that is, having that file in the `srcs`, `hdrs`, or `textual_hdrs` attribute) should depend on the included header's library rule. Conversely, only direct dependencies need to be specified as dependencies.

For example, suppose `sandwich.h` includes `bread.h` and `bread.h` includes `flour.h`. `sandwich.h` doesn't include `flour.h` (who wants flour in their sandwich?), so the `BUILD` file would look like this:

```
cc_library(
    name = "sandwich",
    srcs = ["sandwich.cc"],
    hdrs = ["sandwich.h"],
    deps = [":bread"],
)

cc_library(
    name = "bread",
    srcs = ["bread.cc"],
    hdrs = ["bread.h"],
    deps = [":flour"],
)

cc_library(
    name = "flour",
    srcs = ["flour.cc"],
    hdrs = ["flour.h"],
)
```

Here, the `sandwich` library depends on the `bread` library, which depends on the `flour` library.

## Adding include paths

Sometimes you cannot (or do not want to) root include paths at the workspace root. Existing libraries might already have an include directory that doesn't match its path in your workspace. For example, suppose you have the following directory structure:

```
└── my-project
    ├── legacy
    │   └── some_lib
    │       ├── BUILD
    │       ├── include
    │       │   └── some_lib.h
    │       └── some_lib.cc
    └── WORKSPACE
```

Bazel will expect `some_lib.h` to be included as `legacy/some_lib/include/some_lib.h`, but suppose `some_lib.cc` includes "`some_lib.h`". To make that include path valid, `legacy/some_lib/BUILD` will need to specify that the `some_lib/include` directory is an include directory:

```
cc_library(
    name = "some_lib",
    srcs = ["some_lib.cc"],
    hdrs = ["include/some_lib.h"],
    copts = ["-Ilegacy/some_lib/include"],
)
```

This is especially useful for external dependencies, as their header files must otherwise be included with a `/` prefix.

## Including external libraries

Suppose you are using [Google Test](https://github.com/google/googletest). You can use one of the repository functions in the `WORKSPACE` file to download Google Test and make it available in your repository:

```
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "gtest",
    url = "https://github.com/google/googletest/archive/release-1.10.0.zip",
    sha256 = "94c634d499558a76fa649edb13721dce6e98fb1e7018dfaeba3cd7a083945e91",
    build_file = "@//:gtest.BUILD",
)
```

> Note: If the destination already contains a `BUILD` file, you can leave out the `build_file` attribute.

Then create `gtest.BUILD`, a `BUILD` file used to compile Google Test. Google Test has several "special" requirements that make its `cc_library` rule more complicated:

* `googletest-release-1.10.0/src/gtest-all.cc` `#includes` all other files in `googletest-release-1.10.0/src/:` exclude it from the compile to prevent link errors for duplicate symbols.
* It uses header files that are relative to the `googletest-release-1.10.0/include/` directory ("`gtest/gtest.h`"), so you must add that directory to the include paths.
* It needs to link in `pthread`, so add that as a `linkopt`.

The final rule therefore looks like this:

```
cc_library(
    name = "main",
    srcs = glob(
        ["googletest-release-1.10.0/src/*.cc"],
        exclude = ["googletest-release-1.10.0/src/gtest-all.cc"]
    ),
    hdrs = glob([
        "googletest-release-1.10.0/include/**/*.h",
        "googletest-release-1.10.0/src/*.h"
    ]),
    copts = [
        "-Iexternal/gtest/googletest-release-1.10.0/include",
        "-Iexternal/gtest/googletest-release-1.10.0"
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)
```

This is somewhat messy(凌乱的): everything is prefixed with `googletest-release-1.10.0` as a byproduct of the archive's structure. You can make `http_archive` strip this prefix by adding the `strip_prefix` attribute:

```
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "gtest",
    url = "https://github.com/google/googletest/archive/release-1.10.0.zip",
    sha256 = "94c634d499558a76fa649edb13721dce6e98fb1e7018dfaeba3cd7a083945e91",
    build_file = "@//:gtest.BUILD",
    strip_prefix = "googletest-release-1.10.0",
)
```

Then `gtest.BUILD` would look like this:

```
cc_library(
    name = "main",
    srcs = glob(
        ["src/*.cc"],
        exclude = ["src/gtest-all.cc"]
    ),
    hdrs = glob([
        "include/**/*.h",
        "src/*.h"
    ]),
    copts = ["-Iexternal/gtest/include"],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)
```

Now `cc_` rules can depend on `@gtest//:main`.

## Writing and running C++ tests

For example, you could create a test `./test/hello-test.cc`, such as:

``` cpp
#include "gtest/gtest.h"
#include "main/hello-greet.h"

TEST(HelloTest, GetGreet) {
  EXPECT_EQ(get_greet("Bazel"), "Hello Bazel");
}
```

Then create `./test/BUILD` file for your tests:

```
cc_test(
    name = "hello-test",
    srcs = ["hello-test.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//main:hello-greet",
    ],
)
```

To make `hello-greet` visible to `hello-test`, you must add `"//test:__pkg__"`, to the visibility attribute in `./main/BUILD`.

Now you can use `bazel test` to run the test.

```
bazel test test:hello-test
```

This produces the following output:

```
INFO: Found 1 test target...
Target //test:hello-test up-to-date:
  bazel-bin/test/hello-test
INFO: Elapsed time: 4.497s, Critical Path: 2.53s
//test:hello-test PASSED in 0.3s

Executed 1 out of 1 tests: 1 test passes.
```

## Adding dependencies on precompiled libraries

If you want to use a library of which you only have a compiled version (for example, `headers` and a `.so` file) wrap it in a `cc_library` rule:

```
cc_library(
    name = "mylib",
    srcs = ["mylib.so"],
    hdrs = ["mylib.h"],
)
```

This way, other C++ targets in your workspace can depend on this rule.




# Refer

* https://bazel.build/start
* https://bazel.build/tutorials/cpp
* https://bazel.build/tutorials/cpp-use-cases












