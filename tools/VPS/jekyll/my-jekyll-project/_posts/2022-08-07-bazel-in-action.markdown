---
layout: post
title:  "Bazel in Action"
date:   2022-08-07 18:30:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}


> [Bazel](https://bazel.build/) is an open-source build and test tool similar to `Make`, `Maven`, and `Gradle`. **It uses a human-readable, high-level build language.** `Bazel` supports projects in multiple languages and builds outputs for multiple platforms. `Bazel` supports large codebases across multiple repositories, and large numbers of users.

# 构建基础知识

观看一段简短的历史记录，了解基于工件的构建系统如何发展，以实现规模、速度和封闭性。

* [为什么使用构建系统？](https://bazel.build/basics/build-systems)
* [基于任务的构建系统](https://bazel.build/basics/task-based-builds)
* [基于工件的构建系统](https://bazel.build/basics/artifact-based-builds)
* [分布式构建](https://bazel.build/basics/distributed-builds)
* [依赖项管理](https://bazel.build/basics/dependencies)



# [Bazel 基础概念](https://bazel.build/concepts/build-ref?hl=en)

了解源代码布局、BUILD 文件语法以及规则和依赖项类型等基本概念。

## [Workspaces, packages, and targets](https://bazel.build/concepts/build-ref) / [中文版](https://bazel.build/concepts/build-ref?hl=zh-cn)


Bazel 会根据名为“工作区”的目录树整理的源代码构建软件。工作区中的源文件以嵌套的软件包层次结构进行组织，其中每个软件包都是一个包含一组相关源文件和一个 BUILD 文件的目录。BUILD 文件指定可以从源代码构建哪些软件输出。

### Workspace

A workspace is a directory tree on your filesystem that contains the source files for the software you want to build. Each workspace has a text file named `WORKSPACE` which may be empty, or may contain references to [external dependencies](https://bazel.build/docs/external) required to build the outputs.

Directories containing a file called `WORKSPACE` are considered the root of a workspace. Therefore, Bazel ignores any directory trees in a workspace rooted at a subdirectory containing a `WORKSPACE` file, as they form another workspace.

Bazel also supports `WORKSPACE.bazel` file as an alias of `WORKSPACE` file. If both files exist, `WORKSPACE.bazel` is used.


### Repositories

Code is organized in repositories. The directory containing the `WORKSPACE` file is the root of the main repository, also called `@`. Other, (external) repositories are defined in the `WORKSPACE` file using workspace rules, or generated from modules and extensions in the Bzlmod system. See [external dependencies overview](https://bazel.build/external/overview) for more information.

The workspace rules bundled with Bazel are documented in the [Workspace Rules](https://bazel.build/reference/be/workspace) section in the [Build Encyclopedia](https://bazel.build/reference/be/overview) and the documentation on [embedded Starlark repository rules](https://bazel.build/rules/lib/repo).

As external repositories are repositories themselves, they often contain a `WORKSPACE` file as well. However, these additional `WORKSPACE` files are ignored by Bazel. In particular, repositories depended upon transitively are not added automatically.


### Packages

The primary unit of code organization in a repository is the **package**. A package is a collection of related files and a specification of how they can be used to produce output artifacts.

A package is defined as a directory containing a `BUILD` file named either `BUILD` or `BUILD.bazel`. A package includes all files in its directory, plus all subdirectories beneath it, except those which themselves contain a `BUILD` file. From this definition, no file or directory may be a part of two different packages.

For example, in the following directory tree there are two packages, `my/app`, and the subpackage `my/app/tests`. Note that `my/app/data` is not a package, but a directory belonging to package `my/app`.

```
src/my/app/BUILD
src/my/app/app.cc
src/my/app/data/input.txt
src/my/app/tests/BUILD
src/my/app/tests/test.cc
```

### Targets

A package is a container of **targets**, which are defined in the package's `BUILD` file. Most targets are one of two principal kinds, **files** and **rules**.

Files are further divided into two kinds. **Source files** are usually written by the efforts of people, and checked in to the repository. **Generated files**, sometimes called derived files or output files, are not checked in, but are generated from source files.

The second kind of target is declared with a **rule**. Each rule instance specifies the relationship between a set of input and a set of output files. The inputs to a rule may be source files, but they also may be the outputs of other rules.

Whether the input to a rule is a source file or a generated file is in most cases immaterial; what matters is only the contents of that file. This fact makes it easy to replace a complex source file with a generated file produced by a rule, such as happens when the burden of manually maintaining a highly structured file becomes too tiresome, and someone writes a program to derive it. No change is required to the consumers of that file. Conversely, a generated file may easily be replaced by a source file with only local changes.

**The inputs to a rule may also include other rules**. The precise meaning of such relationships is often quite complex and language- or rule-dependent, but intuitively it is simple: **a C++ library rule A might have another C++ library rule B for an input. The effect of this dependency is that B's header files are available to A during compilation, B's symbols are available to A during linking, and B's runtime data is available to A during execution**.

An invariant of all rules is that the files generated by a rule always belong to the same package as the rule itself; it is not possible to generate files into another package. It is not uncommon for a rule's inputs to come from another package, though.

Package groups are sets of packages whose purpose is to limit accessibility of certain rules. Package groups are defined by the `package_group` function. They have three properties: the list of packages they contain, their name, and other package groups they include. The only allowed ways to refer to them are from the `visibility` attribute of rules or from the `default_visibility` attribute of the package function; they do not generate or consume files. For more information, refer to the [package_group documentation](https://bazel.build/reference/be/functions#package_group).

## [Labels](https://bazel.build/concepts/labels?hl=en)

All targets belong to exactly one package. The name of a target is called its **label**. Every label uniquely identifies a target. A typical label in canonical form looks like:

```
@myrepo//my/app/main:app_binary
```

## [BUILD files](https://bazel.build/concepts/build-files?hl=en)

The previous sections described packages, targets and labels, and the build dependency graph abstractly. This section describes the concrete syntax used to define a package.

By definition, every package contains a `BUILD` file, **which is a short program**.

> Note: The BUILD file can be named either BUILD or BUILD.bazel. If both files exist, BUILD.bazel takes precedence over BUILD. For simplicity's sake, the documentation refers to these files simply as BUILD files.

`BUILD` files are evaluated using an imperative language, [Starlark](https://github.com/bazelbuild/starlark/).


### Loading an extension

Bazel extensions are files ending in `.bzl`. Use the `load` statement to import a symbol from an extension.

```
load("//foo/bar:file.bzl", "some_library")
```

This code loads the file `foo/bar/file.bzl` and adds the `some_library` symbol to the environment. This can be used to load new rules, functions, or constants (for example, a string or a list). Multiple symbols can be imported by using additional arguments to the call to `load`. Arguments must be string literals (no variable) and `load` statements must appear at top-level — they cannot be in a function body.

The first argument of `load` is a [label](https://bazel.build/concepts/labels) identifying a `.bzl` file. If it's a relative label, it is resolved with respect to the package (not directory) containing the current `bzl` file. Relative labels in load statements should use a leading `:`.

In a `.bzl` file, symbols starting with `_` are not exported and cannot be loaded from another file.

You can use [load visibility](https://bazel.build/concepts/visibility#load-visibility) to restrict who may load a `.bzl` file.


## Types of build rules

The majority of build rules come in families, grouped together by language. For example, `cc_binary`, `cc_library` and `cc_test` are the build rules for C++ `binaries`, `libraries`, and `tests`, respectively. Other languages use the same naming scheme, with a different prefix, such as `java_*` for Java. Some of these functions are documented in the [Build Encyclopedia](https://bazel.build/reference/be/overview), but it is possible for anyone to create new rules.


* `*_binary` rules build executable programs in a given language. After a build, the executable will reside in the build tool's binary output tree at the corresponding name for the rule's label, so `//my:program` would appear at (for example) `$(BINDIR)/my/program`.

In some languages, such rules also create a runfiles directory containing all the files mentioned in a `data` attribute belonging to the rule, or any rule in its transitive closure of dependencies; this set of files is gathered together in one place for ease of deployment to production.

* `*_test` rules are a specialization of a `*_binary` rule, used for automated testing. Tests are simply programs that return zero on success.

Like binaries, tests also have runfiles trees, and the files beneath it are the only files that a test may legitimately open at runtime. For example, a program `cc_test(name='x', data=['//foo:bar'])` may open and read `$TEST_SRCDIR/workspace/foo/bar` during execution. (Each programming language has its own utility function for accessing the value of `$TEST_SRCDIR`, but they are all equivalent to using the environment variable directly.) Failure to observe the rule will cause the test to fail when it is executed on a remote testing host.

* `*_library` rules specify separately-compiled modules in the given programming language. Libraries can depend on other libraries, and binaries and tests can depend on libraries, with the expected separate-compilation behavior.


## [Dependencies](https://bazel.build/concepts/dependencies?hl=en)

A target `A` depends upon a target `B` if `B` is needed by `A` at build or execution time. The depends upon relation induces a `Directed Acyclic Graph` (**DAG**) over targets, and it is called a dependency graph.

**A target's direct dependencies (直接依赖)** are those other targets reachable by a path of length 1 in the dependency graph. **A target's transitive dependencies (间接依赖)** are those targets upon which it depends via a path of any length through the graph.

In fact, in the context of builds, there are **two** dependency graphs, **the graph of actual dependencies (实际依赖图)** and **the graph of declared dependencies (声明依赖图)**. Most of the time, the two graphs are so similar that this distinction need not be made, but it is useful for the discussion below.

### Actual and declared dependencies

A target `X` is actually dependent on target `Y` if `Y` must be present, built, and up-to-date in order for `X` to be built correctly. Built could mean generated, processed, compiled, linked, archived, compressed, executed, or any of the other kinds of tasks that routinely occur during a build.


### Types of dependencies (依赖类型)

Most build rules have three attributes for specifying different kinds of generic dependencies: `srcs`, `deps` and `data`. These are explained below. For more details, see [Attributes common to all rules](https://bazel.build/reference/be/common-definitions).

Many rules also have additional attributes for rule-specific kinds of dependencies, for example, `compiler` or `resources`. These are detailed in the [Build Encyclopedia](https://bazel.build/reference/be).


> srcs dependencies

Files consumed directly by the rule or rules that output source files.

> deps dependencies

Rule pointing to separately-compiled modules providing header files, symbols, libraries, data, etc.

> data dependencies

A build target might need some data files to run correctly. These data files aren't source code: they don't affect how the target is built. For example, a unit test might compare a function's output to the contents of a file. When you build the unit test you don't need the file, but you do need it when you run the test. The same applies to tools that are launched during execution.

## [Visibility](https://bazel.build/concepts/visibility?hl=en)

This page covers Bazel's two visibility systems: [target visibility](https://bazel.build/concepts/visibility?hl=en#target-visibility) and [load visibility](https://bazel.build/concepts/visibility?hl=en#load-visibility).





# Bazel 安装 (CentOS)

[Bazel 安装说明](https://bazel.build/install)，本文使用方式三。

方式一：安装包

```
yum install bazel4
```

方式二：[源码编译](https://bazel.build/install/compile-source)

方式三：[使用release版本](https://github.com/bazelbuild/bazel/releases)


# Bazel 优势

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

# Bazel 使用流程

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



# [Bazel Tutorial: Build a C++ Project](https://docs.bazel.build/versions/4.2.1/tutorial/cpp.html)

Start by [installing Bazel](https://bazel.build/install), if you haven’t already. This tutorial uses Git for source control, so for best results [install Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git) as well.

Next, retrieve the sample project from Bazel's GitHub repository by running the following in your command-line tool of choice:

```
git clone https://github.com/bazelbuild/examples
```

The sample project for this tutorial is in the examples/cpp-tutorial directory.

Take a look below at how it’s structured:

There are three sets of files, each set representing a stage in this tutorial.

* In the first stage, you will build a single target residing in a single package.
* In the second stage, you will build both a binary and a library from a single package.
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

* The `WORKSPACE` file , which identifies the directory and its contents as a Bazel workspace and lives at the root of the project's directory structure.
* One or more `BUILD` files, which tell Bazel how to build different parts of the project. A directory within the workspace that contains a `BUILD` file is a [package](https://bazel.build/reference/glossary#package). (More on packages later in this tutorial.)

In future projects, to designate a directory as a Bazel workspace, create an empty file named `WORKSPACE` in that directory. For the purposes of this tutorial, a `WORKSPACE` file is already present in each stage.

> NOTE: When Bazel builds the project, all inputs must be in the same workspace. Files residing in different workspaces are independent of one another unless linked. More detailed information about workspace rules can be found in [this guide](https://bazel.build/reference/be/workspace).

## Understand the BUILD file

A `BUILD` file contains several different types of instructions for Bazel. Each `BUILD` file requires at least one [rule](https://bazel.build/reference/glossary#rule) as a set of instructions, which tells Bazel how to build the desired outputs, such as executable binaries or libraries. Each instance of a build rule in the `BUILD` file is called a [target](https://bazel.build/reference/glossary#target) and points to a specific set of source files and [dependencies](https://bazel.build/reference/glossary#dependency). A target can also point to other targets.

Take a look at the `BUILD` file in the `cpp-tutorial/stage1/main` directory:

```
cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
)
```

In our example, the `hello-world` target instantiates Bazel's built-in `cc_binary` rule. The rule tells Bazel to build a self-contained executable binary from the `hello-world.cc` source file with no dependencies.

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

In the target label, the `//main:` part is the location of the `BUILD` file relative to the root of the workspace, and `hello-world` is the target name in the `BUILD` file.

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

# [Build programs with Bazel](https://bazel.build/run/build?hl=en)


## Available commands

```
bazel help
```

* [analyze-profile](https://bazel.build/docs/user-manual#analyze-profile): Analyzes build profile data.
* [aquery](https://bazel.build/docs/user-manual#aquery): Executes a query on the [post-analysis](https://bazel.build/run/build?hl=en#analysis) action graph.
* [build](https://bazel.build/run/build?hl=en#bazel-build): Builds the specified targets.
* [canonicalize-flags](https://bazel.build/docs/user-manual#canonicalize-flags): Canonicalize Bazel flags.
* [clean](https://bazel.build/docs/user-manual#clean): Removes output files and optionally stops the server.
* [cquery](https://bazel.build/query/cquery): Executes a [post-analysis](https://bazel.build/run/build?hl=en#analysis) dependency graph query.
* [dump](https://bazel.build/docs/user-manual#dump): Dumps the internal state of the Bazel server process.
* [help](https://bazel.build/docs/user-manual#help): Prints help for commands, or the index.
* [info](https://bazel.build/docs/user-manual#info): Displays runtime info about the bazel server.
* [fetch](https://bazel.build/run/build?hl=en#fetching-external-dependencies): Fetches all external dependencies of a target.
* [mobile-install](https://bazel.build/docs/user-manual#mobile-install): Installs apps on mobile devices.
* [query](https://bazel.build/query/guide): Executes a dependency graph query.
* [run](https://bazel.build/docs/user-manual#running-executables): Runs the specified target.
* [shutdown](https://bazel.build/docs/user-manual#shutdown): Stops the Bazel server.
* [test](https://bazel.build/docs/user-manual#running-tests): Builds and runs the specified test targets.
* [version](https://bazel.build/docs/user-manual#version): Prints version information for Bazel.


## Getting help

* `bazel help command`: Prints help and options for **command**.
* `bazel help [startup_options](https://bazel.build/docs/user-manual#startup-options)`: Options for the JVM hosting Bazel.
* `bazel help [target-syntax](https://bazel.build/run/build?hl=en#specifying-build-targets)`: Explains the syntax for specifying targets.
* `bazel help info-keys`: Displays a list of keys used by the info command.

The `bazel` tool performs many functions, called commands. The most commonly used ones are `bazel build` and `bazel test`. You can browse the online help messages using `bazel help`.


## [Building one target](https://bazel.build/run/build?hl=en#bazel-build)

```
bazel build //foo
```

## [Building multiple targets](https://bazel.build/run/build?hl=en#specifying-build-targets)

Bazel allows a number of ways to specify the targets to be built. Collectively, these are known as target patterns. This syntax is used in commands like `build`, `test`, or `query`.

All target patterns starting with `//` are resolved relative to the current workspace.

![bazel_build](/assets/images/202306/bazel_build.png)

Target patterns that **do not begin with** `//` are resolved relative to the current working directory. These examples assume a working directory of `foo`:

![bazel_build2](/assets/images/202306/bazel_build2.png)


## [Fetching external dependencies](https://bazel.build/run/build?hl=en#fetching-external-dependencies)

By default, Bazel will download and symlink external dependencies during the build. However, this can be undesirable, either because you'd like to know when new external dependencies are added or because you'd like to "prefetch" dependencies (say, before a flight where you'll be offline). If you would like to prevent new dependencies from being added during builds, you can specify the `--fetch=false` flag. Note that this flag only applies to repository rules that do not point to a directory in the local file system. Changes, for example, to local_repository, new_local_repository and Android SDK and NDK repository rules will always take effect regardless of the value `--fetch` .

If you disallow fetching during builds and Bazel finds new external dependencies, your build will fail.

You can manually fetch dependencies by running `bazel fetch`. If you disallow during-build fetching, you'll need to run `bazel fetch`:

* Before you build for the first time.
* After you add a new external dependency.

Once it has been run, you should not need to run it again until the `WORKSPACE` file changes.

fetch takes a list of targets to fetch dependencies for. For example, this would fetch dependencies needed to build `//foo:bar` and `//bar:baz`:

```
bazel fetch //foo:bar //bar:baz
```

To fetch all external dependencies for a workspace, run:

```
bazel fetch //...
```


# [Commands and Options](https://bazel.build/docs/user-manual?hl=en)

This page covers the options that are available with various Bazel commands, such as `bazel build`, `bazel run`, and `bazel test`. This page is a companion to the list of Bazel's commands in [Build with Bazel](https://bazel.build/run/build).


## [Options](https://bazel.build/docs/user-manual?hl=en#build-options)

The following sections describe the options available during a build. When `--long` is used on a help command, the on-line help messages provide summary information about the meaning, type and default value for each option.

Most options can only be specified once. When specified multiple times, the last instance wins. Options that can be specified multiple times are identified in the on-line help with the text 'may be used multiple times'.



```
$bazel help build --short
                                                           [bazel release 6.2.1]
Usage: bazel build <options> <targets>

Builds the specified targets, using the options.

See 'bazel help target-syntax' for details and examples on how to
specify targets to build.

Options that appear before the command and are parsed by the client:
  --distdir
  --[no]experimental_repository_cache_hardlinks
  --[no]experimental_repository_cache_urls_as_default_canonical_id
  --[no]experimental_repository_disable_download
  --experimental_repository_downloader_retries
  --experimental_scale_timeouts
  --http_timeout_scaling
  --repository_cache
```

```
$bazel help build --long | head -n50
                                                           [bazel release 6.2.1]
Usage: bazel build <options> <targets>

Builds the specified targets, using the options.

See 'bazel help target-syntax' for details and examples on how to
specify targets to build.

Options that appear before the command and are parsed by the client:
  --distdir (a path; may be used multiple times)
    Additional places to search for archives before accessing the network to
    download them.
      Tags: bazel_internal_configuration
  --[no]experimental_repository_cache_hardlinks (a boolean; default: "false")
    If set, the repository cache will hardlink the file in case of a cache hit,
    rather than copying. This is intended to save disk space.
      Tags: bazel_internal_configuration
```


### Package location


* `--package_path`

This option specifies the set of directories that are searched to find the BUILD file for a given package.


### Error checking

These options control Bazel's error-checking and/or warnings.


* `--[no]check_visibility`
* `--output_filter=regex`


### Tool flags

These options control which options Bazel will pass to other tools.


* `--copt=cc-option`

This option takes an argument which is to be passed to the compiler. The argument will be passed to the compiler whenever it is invoked for preprocessing, compiling, and/or assembling C, C++, or assembler code. **It will not be passed when linking**.

This option can be used multiple times. For example:

```
% bazel build --copt="-g0" --copt="-fpic" //foo
```

will compile the foo library without debug tables, generating position-independent code.

> Note: Changing --copt settings will force a recompilation of all affected object files. Also note that copts values listed in specific cc_library or cc_binary build rules will be placed on the compiler command line after these options.

> Warning: C++-specific options (such as -fno-implicit-templates) should be specified in --cxxopt, not in --copt. Likewise, C-specific options (such as -Wstrict-prototypes) should be specified in --conlyopt, not in copt. Similarly, compiler options that only have an effect at link time (such as -l) should be specified in --linkopt, not in --copt.


* `--cxxopt=cc-option`

This option takes an argument which is to be passed to the compiler when compiling C++ source files.

This is similar to `--copt`, but only applies to C++ compilation, not to C compilation or linking. So you can pass C++-specific options (such as `-fpermissive` or `-fno-implicit-templates`) using `--cxxopt`.

For example:

```
% bazel build --cxxopt="-fpermissive" --cxxopt="-Wno-error" //foo/cruddy_code
```

> Note: copts parameters listed in specific cc_library or cc_binary build rules are placed on the compiler command line after these options.


* `--linkopt=linker-option`

This option takes an argument which is to be passed to the compiler when linking.

This is similar to `--copt`, but only applies to linking, not to compilation. So you can pass compiler options that only make sense at link time (such as `-lssp` or `-Wl,--wrap,abort`) using `--linkopt`. For example:

```
% bazel build --copt="-fmudflap" --linkopt="-lmudflap" //foo/buggy_code
```

Build rules can also specify link options in their attributes. This option's settings always take precedence. Also see [cc_library.linkopts](https://bazel.build/reference/be/c-cpp#cc_library.linkopts).


* `--strip (always|never|sometimes)`

This option determines whether Bazel will strip debugging information from all binaries and shared libraries, by invoking the linker with the `-Wl,--strip-debug` option. `--strip=always` means always strip debugging information. `--strip=never` means never strip debugging information. The default value of `--strip=sometimes` means strip if the `--compilation_mode` is `fastbuild`.

```
% bazel build --strip=always //foo:bar
```

will compile the target while stripping debugging information from all generated binaries.

> Note: If you want debugging information, it's not enough to disable stripping; you also need to make sure that the debugging information was generated by the compiler, which you can do by using either -c dbg or --copt -g.


### Build semantics

These options affect the build commands and/or the output file contents.

* `--compilation_mode (fastbuild|opt|dbg) (-c)`

The `--compilation_mode` option (often shortened to `-c`, especially `-c opt`) takes an argument of `fastbuild`, `dbg` or `opt`, and affects various C/C++ code-generation options, such as the level of optimization and the completeness of debug tables. Bazel uses a different output directory for each different compilation mode, so you can switch between modes without needing to do a full rebuild every time.

**fastbuild** means build as fast as possible: generate minimal debugging information (`-gmlt -Wl,-S`), and don't optimize. This is the default. Note: `-DNDEBUG` will not be set.

**dbg** means build with debugging enabled (`-g`), so that you can use `gdb` (or another debugger).

**opt** means build with optimization enabled and with `assert()` calls disabled (`-O2 -DNDEBUG`). Debugging information will not be generated in `opt` mode unless you also pass `--copt -g`.












# Tips

## bazel info

bazel info 提供了有关 Bazel 工作空间和其配置的信息。这些信息使您能够更好地了解构建环境和输出位置。

* 获取输出基本目录：此命令返回输出基本目录，该目录包含 Bazel 执行期间产生的中间文件和构建产物。

```
ls -l `bazel info output_base`
总用量 18260
drwxr-xr-x  2 gerryyang users     4096 6月  20 13:09 action_cache
-rw-r--r--  1 gerryyang users       84 6月  20 13:13 command.log
-rw-r--r--  1 gerryyang users    69253 6月  20 13:09 command.profile.gz
-rw-r--r--  1 gerryyang users       35 6月  20 12:08 DO_NOT_BUILD_HERE
drwxr-xr-x  3 gerryyang users     4096 6月  20 13:08 execroot
drwxr-xr-x 28 gerryyang users     4096 6月  20 13:08 external
lrwxrwxrwx  1 gerryyang users       91 6月  19 11:50 install -> /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/install/50a5a3eaeaa5297d31568ab6a3a702d9
lrwxrwxrwx  1 gerryyang users       76 6月  20 12:08 java.log -> java.log.gerryyang1631112241020-0.gerryyang.log.java.20230620-120813.3969149
-rw-r--r--  1 gerryyang users 15523519 6月  20 00:15 java.log.gerryyang1631112241020-0.gerryyang.log.java.20230619-115035.3573878
-rw-r--r--  1 gerryyang users  3052736 6月  20 13:13 java.log.gerryyang1631112241020-0.gerryyang.log.java.20230620-120813.3969149
-rw-r--r--  1 gerryyang users      544 6月  20 13:13 javalog.properties
-rw-r--r--  1 gerryyang users       82 6月  20 13:13 lock
-rw-r--r--  1 gerryyang users      789 6月  20 12:08 README
drwx------  2 gerryyang users     4096 6月  20 12:08 server
```

* 获取 bazel-bin 目录：此命令返回包含构建产物（如可执行文件和库）的 bazel-bin 目录。

```
$ls -l `bazel info bazel-bin`
总用量 20
drwxr-xr-x  3 gerryyang users 4096 6月  20 13:08 external
drwxr-xr-x  4 gerryyang users 4096 6月  20 13:08 frame
drwxr-xr-x  3 gerryyang users 4096 6月  20 13:08 protocol
drwxr-xr-x 14 gerryyang users 4096 6月  20 13:09 src
drwxrwxrwx 24 gerryyang users 4096 6月  20 13:08 thirdparty
```

* 获取工作空间根目录：此命令返回当前 Bazel 工作空间的根目录。

```
$ls -l `bazel info workspace`
总用量 156
lrwxrwxrwx  1 gerryyang users   128 6月  20 13:08 bazel-bin -> /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/execroot/__main__/bazel-out/k8-fastbuild/bin
-rwxr-xr-x  1 gerryyang users   719 6月  20 13:08 bazel_build.sh
lrwxrwxrwx  1 gerryyang users   101 6月  20 13:08 bazel-JLib -> /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/execroot/__main__
lrwxrwxrwx  1 gerryyang users   111 6月  20 13:08 bazel-out -> /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/execroot/__main__/bazel-out
lrwxrwxrwx  1 gerryyang users   133 6月  20 13:08 bazel-testlogs -> /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/execroot/__main__/bazel-out/k8-fastbuild/testlogs
drwxr-xr-x  6 gerryyang users  4096 6月  11 21:27 benchmark
drwxr-xr-x 22 gerryyang users  4096 6月  12 21:15 bin
drwxr-xr-x  4 gerryyang users  4096 6月  12 21:15 build
-rwxr-xr-x  1 gerryyang users  4914 6月  18 19:34 build.sh
-rw-r--r--  1 gerryyang users   501 6月  12 21:15 CHANGELOG.md
drwxr-xr-x  6 gerryyang users  4096 6月  11 21:27 charts
-rwxr-xr-x  1 gerryyang users   758 6月  11 21:27 chmod.sh
-rw-r--r--  1 gerryyang users  5789 6月  20 12:01 CMakeLists.txt
lrwxrwxrwx  1 gerryyang users    53 6月  20 12:05 compile_commands.json -> /data/home/gerryyang/JLib_Build/compile_commands.json
drwxr-xr-x 25 gerryyang users  4096 6月  12 21:15 config
drwxr-xr-x  8 gerryyang users  4096 6月  11 21:27 deploy
drwxr-xr-x  8 gerryyang users  4096 6月  11 21:27 deploy_apps
-rw-r--r--  1 gerryyang users   448 6月  11 21:27 DevopsPlan.txt
drwxr-xr-x  3 gerryyang users  4096 6月  11 21:27 doc
-rw-r--r--  1 gerryyang users   669 6月  11 21:27 Dockerfile4Yunguan
drwxr-xr-x  4 gerryyang users  4096 6月  19 20:40 frame
-rwxr-xr-x  1 gerryyang users 10117 6月  11 21:27 optools
-rwxr-xr-x  1 gerryyang users 17386 6月  11 21:27 optools2
-rw-r--r--  1 gerryyang users   780 6月  20 12:05 Project.cfg
drwxr-xr-x 21 gerryyang users  4096 6月  19 09:44 protocol
drwxr-xr-x  3 gerryyang users  4096 6月  11 21:27 resources
drwxr-xr-x 16 gerryyang users  4096 6月  12 21:15 src
drwxr-xr-x  9 gerryyang users  4096 6月  11 21:27 testcase
drwxr-xr-x 35 gerryyang users  4096 6月  19 15:35 thirdparty
drwxr-xr-x 23 gerryyang users  4096 6月  11 21:27 tools
drwxr-xr-x  5 gerryyang users  4096 6月  12 21:15 unittest
-rw-r--r--  1 gerryyang users     0 6月  19 21:05 WORKSPACE
drwxr-xr-x  7 gerryyang users  4096 6月  11 21:27 world-values
```

* 获取构建配置信息：此命令可获取有关当前 Bazel 构建配置的详细信息。

```
$bazel info --show_make_env
BINDIR: bazel-out/k8-fastbuild/bin
COMPILATION_MODE: fastbuild
GENDIR: bazel-out/k8-fastbuild/bin
TARGET_CPU: k8
bazel-bin: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/execroot/__main__/bazel-out/k8-fastbuild/bin
bazel-genfiles: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/execroot/__main__/bazel-out/k8-fastbuild/bin
bazel-testlogs: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/execroot/__main__/bazel-out/k8-fastbuild/testlogs
character-encoding: file.encoding = ISO-8859-1, defaultCharset = ISO-8859-1
command_log: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/command.log
committed-heap-size: 142MB
execution_root: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/execroot/__main__
gc-count: 126
gc-time: 3862ms
install_base: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/install/50a5a3eaeaa5297d31568ab6a3a702d9
java-home: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/install/50a5a3eaeaa5297d31568ab6a3a702d9/embedded_tools/jdk
java-runtime: OpenJDK Runtime Environment (build 11.0.6+10-LTS) by Azul Systems, Inc.
java-vm: OpenJDK 64-Bit Server VM (build 11.0.6+10-LTS, mixed mode) by Azul Systems, Inc.
max-heap-size: 16387MB
output_base: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6
output_path: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/execroot/__main__/bazel-out
package_path: %workspace%
release: release 6.2.1
repository_cache: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/cache/repos/v1
server_log: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6/java.log.gerryyang1631112241020-0.gerryyang.log.java.20230620-120813.3969149
server_pid: 3969149
used-heap-size: 69MB
workspace: /data/home/gerryyang/jlib_proj/JLib
```


## bazel aquery

bazel aquery 是一个强大的命令，用于查询关于构建操作的详细信息。它提供了有关构建过程、目标输出、依赖项和更多内容的信息。

* 查询特定构建目标的信息：此命令查询特定构建目标的详细信息。

```
bazel aquery '//path/to/package:target_name'
```

* 查询特定构建目标和其直接依赖项的信息：此命令查询指定构建目标的详细信息，并包括其直接依赖项。

```
bazel aquery 'deps(//path/to/package:target_name)'
```

* 查询具有特定属性的构建目标信息：此命令查询具有特定属性的构建目标及其直接依赖项的详细信息。

```
bazel aquery 'attr("srcs", ".*/file_to_search\\.cpp", deps(//path/to/package:target_name))'
```


# Refer

* https://bazel.build/start
* https://bazel.build/tutorials/cpp
* https://bazel.build/tutorials/cpp-use-cases
* https://bazel.build/reference?hl=zh-cn














