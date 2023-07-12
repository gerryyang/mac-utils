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

# TL;DR (Bazel 构建需要完成的三件事)

* 修改头文件的路径，以 `WORKSPACE` 所在目录为基准
* 编写 `BUILD` (对当前 `package` 软件包的描述)
  + 我是谁 (`name`)
  + 我有什么 (`srcs` / `hdrs`)
  + 我依赖什么 (`deps`)
  + 其他选项 (`copts` / `linkopts` / ...)
* 解决链接问题 (依赖顺序)



# Bazel 安装 (CentOS 环境)

[Bazel 安装说明](https://bazel.build/install)，本文使用方式三。

方式一：安装包

```
yum install bazel4
```

方式二：[源码编译](https://bazel.build/install/compile-source)

方式三：[使用release版本](https://github.com/bazelbuild/bazel/releases)


方式四：基于 [Bazelisk](https://github.com/bazelbuild/bazelisk/releases/) 的安装

> Bazelisk is a wrapper for Bazel written in Go. It automatically picks a good version of Bazel given your current working directory, downloads it from the official server (if required) and then transparently passes through all command-line arguments to the real Bazel binary. You can call it just like you would call Bazel.


``` bash
sudo wget -O /usr/local/bin/bazel https://github.com/bazelbuild/bazelisk/releases/download/v1.17.0/bazelisk-linux-amd64
sudo chmod +x /usr/local/bin/bazel
```

完整的安装脚本：先检查本地是否有，没有则下载安装

``` bash
#!/bin/bash

CUR_DIR=$(dirname $(readlink -f $0))

# Specify the Bazelisk version to install
BAZELISK_VERSION="1.17.0"
BAZELISK_DOWNLOAD_URL="https://github.com/bazelbuild/bazelisk/releases/download/v${BAZELISK_VERSION}/bazelisk-linux-amd64"
LOCAL_BAZELISK_FILE="$CUR_DIR/bazelisk-linux-amd64-1.17.0/bazelisk-linux-amd64"

# Check if Bazelisk is already installed
if command -v bazel &> /dev/null; then
    echo "Bazelisk is already installed, version: $(bazel --version)"
else
    echo "Installing Bazelisk version ${BAZELISK_VERSION}..."

    if [[ -f "${LOCAL_BAZELISK_FILE}" ]]; then
        echo "Using local Bazelisk file: ${LOCAL_BAZELISK_FILE}"
        cp "${LOCAL_BAZELISK_FILE}" "bazelisk-linux-amd64"

    elif wget -O "bazelisk-linux-amd64" --tries=1 "${BAZELISK_DOWNLOAD_URL}"; then
        echo "Downloaded Bazelisk from ${BAZELISK_DOWNLOAD_URL}"

    else
        echo "Error: Could not download Bazelisk and local file not found"
        exit 1
    fi

    # Move Bazelisk to the /usr/local/bin directory and make it executable
    sudo mv "bazelisk-linux-amd64" /usr/local/bin/bazel
    sudo chmod +x /usr/local/bin/bazel

    echo "Bazelisk installation completed, version:"
    echo "-----------------------------------------"
    echo "$(bazel version)"
    echo "-----------------------------------------"
fi
```


# Bazel 优势

Bazel offers the following advantages:

* **High-level build language**. Bazel uses an abstract, human-readable language to describe the build properties of your project at a high semantical level. Unlike other tools, Bazel operates on the concepts of libraries, binaries, scripts, and data sets, shielding you from the complexity of writing individual calls to tools such as compilers and linkers.
* **Bazel is fast and reliable**. Bazel caches all previously done work and tracks changes to both file content and build commands. This way, Bazel knows when something needs to be rebuilt, and rebuilds only that. To further speed up your builds, you can set up your project to build in a highly parallel and incremental fashion.
* **Bazel is multi-platform**. Bazel runs on Linux, macOS, and Windows. Bazel can build binaries and deployable packages for multiple platforms, including desktop, server, and mobile, from the same project.
* **Bazel scales**. Bazel maintains agility(敏捷) while handling builds with `100k+` source files. It works with multiple repositories and user bases in the tens of thousands.
* **Bazel is extensible**. Many languages are supported, and you can extend Bazel to support any other language or framework.

----

1. 多语言，可并行
2. 保证构建结果准确性
3. 支持多 CPU 架构
4. C/S 架构，Memory / Local / Remote 三级缓存，构建速度快
5. 全面接管编译目标间依赖关系，支持分布式远程构建
6. 全面追踪构建详情



一些测试结论：

* `bazel` 默认会限制并发度到其估计的机器性能上限，实际使用需要通过 `--local_cpu_resources=9999999` 等参数绕过这一限制
* 已知（部分版本的）bazel 在并发度过高（如`-j320`）下，bazel 自身性能存在瓶颈。这具体表现为机器空闲但不会启动更多编译任务，同时 bazel 自身 CPU（`400~500%`）、内存（几G）占用很高。
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

Bazel 构建分为三个阶段：`Loading`, `Analysising` and `Executing`

![bazel_build6](/assets/images/202306/bazel_build6.png)

When running a build or a test, Bazel does the following:

1. **Loads** the `BUILD` files relevant to the target.
2. **Analyzes** the inputs and their dependencies, applies the specified build rules, and produces an action graph.
3. **Executes** the build actions on the inputs until the final build outputs are produced.


Since all previous build work is cached, Bazel can identify and reuse cached artifacts and only rebuild or retest what’s changed. To further enforce correctness, you can set up Bazel to run builds and tests hermetically through sandboxing, minimizing skew and maximizing reproducibility.

下面针对这三个过程进行详细解释。

> Loads

Bazel 在这个阶段会加载 `WORKSPACE` 和 `BUILD`，同时会加载相关的 `.bzl` 文件。这些文件可以来自本地，也可以来自 git_repository 或者 github 上面的其他第三方包。加载完毕之后，如果有macro 的定义，会处理相关 macro，替换成真正的 rules。Bazel 加载所有需要的文件完毕之后，下次重新构建就可以复用这些文件，而无须重复加载。

总结：在此阶段结束后，Bazel 已经形成了一个 target dependency 的体系，同时所有 target 所需要的文件都加载成功，target 分析成功，所有 macro 都展开成了 rules，并且都缓存到了内存中，下次重新运行规则，不需要重新加载文件、分析这些依赖关系。

> Analyzes

分析阶段主要为执行这些函数，生成一个静态有向图`Action Direct acyclic Graph`，这个图表示需要构建的目标、目标之间的关系，以及为了构建目标需要执行的动作。每一个函数都生成静态图中的一个结点，根据结点中定义的 label 和其他信息，构建结点之间的依赖关系。另外 Bazel 会缓存 Action Graph，对于没有变化的构建，会直接从缓存中读取，也就是增量构建。

在这个过程中，主要是执行规则，生成 Action，并未真正开始执行任何读写 inputs ouputs 等操作。通过 bazel query --output graph，可以查看生成的 Action Graph 图形。

总结：在此阶段结束后，所有需要用到的规则都会被执行一次，Bazel 已经形成了一个 action dependency 体系，存放在内存中。

> Executes

主要是根据 Analyzes 阶段生成的 Action Direct Acyclic Graph，结合它们的依赖关系进行调度执行。对于不依赖别人的对象可直接执行，而依赖其他 Action 的就等其他执行好了才可以继续。也就是在这个阶段才会开始真正 compile link 项目，输出各种文件，执行各种 IO 操作。

总结：所有需要用到的 action 均执行完毕。

# Bazel 语法

```
$bazel help target-syntax
                                                           [bazel release 6.2.1]
Target pattern syntax
=====================

The BUILD file label syntax is used to specify a single target. Target
patterns generalize this syntax to sets of targets, and also support
working-directory-relative forms, recursion, subtraction and filtering.
Examples:

Specifying a single target:

  //foo/bar:wiz     The single target '//foo/bar:wiz'.
  foo/bar/wiz       Equivalent to:
                      '//foo/bar/wiz:wiz' if foo/bar/wiz is a package,
                      '//foo/bar:wiz' if foo/bar is a package,
                      '//foo:bar/wiz' otherwise.
  //foo/bar         Equivalent to '//foo/bar:bar'.

Specifying all rules in a package:

  //foo/bar:all       Matches all rules in package 'foo/bar'.

Specifying all rules recursively beneath a package:

  //foo/...:all     Matches all rules in all packages beneath directory 'foo'.
  //foo/...           (ditto)

  By default, directory symlinks are followed when performing this recursive traversal, except
  those that point to under the output base (for example, the convenience symlinks that are created
  in the root directory of the workspace) But we understand that your workspace may intentionally
  contain directories with weird symlink structures that you don't want consumed. As such, if a
  directory has a file named
  'DONT_FOLLOW_SYMLINKS_WHEN_TRAVERSING_THIS_DIRECTORY_VIA_A_RECURSIVE_TARGET_PATTERN'
  then symlinks in that directory won't be followed when evaluating recursive
  target patterns.

Working-directory relative forms:  (assume cwd = 'workspace/foo')

  Target patterns which do not begin with '//' are taken relative to
  the working directory.  Patterns which begin with '//' are always
  absolute.

  ...:all           Equivalent to  '//foo/...:all'.
  ...                 (ditto)

  bar/...:all       Equivalent to  '//foo/bar/...:all'.
  bar/...             (ditto)

  bar:wiz           Equivalent to '//foo/bar:wiz'.
  :foo              Equivalent to '//foo:foo'.

  bar               Equivalent to '//foo/bar:bar'.
  foo/bar           Equivalent to '//foo/foo/bar:bar'.

  bar:all           Equivalent to '//foo/bar:all'.
  :all              Equivalent to '//foo:all'.

Summary of target wildcards:

  :all,             Match all rules in the specified packages.
  :*, :all-targets  Match all targets (rules and files) in the specified
                      packages, including .par and _deploy.jar files.

Subtractive patterns:

  Target patterns may be preceded by '-', meaning they should be
  subtracted from the set of targets accumulated by preceding
  patterns. (Note that this means order matters.) For example:

    % bazel build -- foo/... -foo/contrib/...

  builds everything in 'foo', except 'contrib'.  In case a target not
  under 'contrib' depends on something under 'contrib' though, in order to
  build the former bazel has to build the latter too. As usual, the '--' is
  required to prevent '-f' from being interpreted as an option.

  When running the test command, test suite expansion is applied to each target
  pattern in sequence as the set of targets is evaluated. This means that
  individual tests from a test suite can be excluded by a later target pattern.
  It also means that an exclusion target pattern which matches a test suite will
  exclude all tests which that test suite references. (Targets that would be
  matched by the list of target patterns without any test suite expansion are
  also built unless --build_tests_only is set.)

(Use 'help --long' for full details or --short to just enumerate options.)
```



# Action Graph (依赖图)

The action graph represents the build artifacts, the relationships between them, and the build actions that Bazel will perform. Thanks to this graph, Bazel can track changes to file content as well as changes to actions, such as build or test commands, and know what build work has previously been done. The graph also enables you to easily trace dependencies in your code.


通过 `bazel query` 输出 `graphviz` 格式数据，然后在 [GraphvizOnline](https://dreampuf.github.io/GraphvizOnline) 查看依赖图。

```
~/github/bazelbuild/examples/cpp-tutorial/stage1$ bazel query --nohost_deps --noimplicit_deps 'deps(//main:hello-world)' --output graph
digraph mygraph {
  node [shape=box];
  "//main:hello-world"
  "//main:hello-world" -> "//main:hello-world.cc"
  "//main:hello-world.cc"
}
Loading: 0 packages loaded
```


![bazel_build5](/assets/images/202306/bazel_build5.png)

完整的依赖图：

```
~/github/bazelbuild/examples/cpp-tutorial/stage1$bazel query 'deps(//main:hello-world)' --output graph
digraph mygraph {
  node [shape=box];
  "//main:hello-world"
  "//main:hello-world" -> "//main:hello-world.cc"
  "//main:hello-world" -> "@bazel_tools//tools/cpp:malloc"
  "//main:hello-world" -> "@bazel_tools//tools/cpp:current_cc_toolchain"
  "//main:hello-world" -> "@bazel_tools//tools/def_parser:def_parser"
  "//main:hello-world" -> "@bazel_tools//tools/cpp:toolchain_type"
  "//main:hello-world.cc"
  "@bazel_tools//tools/def_parser:def_parser"
  "@bazel_tools//tools/def_parser:def_parser" -> "@bazel_tools//tools/def_parser:def_parser_windows"
  [label="@bazel_tools//src/conditions:host_windows"];
  "@bazel_tools//tools/def_parser:def_parser" -> "@bazel_tools//src/conditions:host_windows"
  "@bazel_tools//tools/def_parser:def_parser" -> "@bazel_tools//tools/def_parser:no_op.bat"
  [label="//conditions:default"];
  "@bazel_tools//tools/def_parser:no_op.bat"
  "@bazel_tools//src/conditions:host_windows"
  "@bazel_tools//src/conditions:host_windows" -> "@bazel_tools//src/conditions:host_windows_x64_constraint\n@bazel_tools//src/conditions:host_windows_arm64_constraint"
  [label="//conditions:default@bazel_tools//src/conditions:host_windows_arm64_constraint"];
  "@bazel_tools//src/conditions:host_windows_x64_constraint\n@bazel_tools//src/conditions:host_windows_arm64_constraint"
  "@bazel_tools//tools/def_parser:def_parser_windows"
  "@bazel_tools//tools/def_parser:def_parser_windows" -> "@bazel_tools//tools/def_parser:def_parser.exe\n@bazel_tools//src/conditions:remote"
  [label="//conditions:default"];
  "@bazel_tools//tools/def_parser:def_parser_windows" -> "@bazel_tools//third_party/def_parser:def_parser"
  [label="@bazel_tools//src/conditions:remote"];
  "@bazel_tools//third_party/def_parser:def_parser"
  "@bazel_tools//third_party/def_parser:def_parser" -> "@bazel_tools//third_party/def_parser:def_parser_main.cc"
  "@bazel_tools//third_party/def_parser:def_parser" -> "@bazel_tools//third_party/def_parser:def_parser_lib"
  "@bazel_tools//third_party/def_parser:def_parser" -> "@bazel_tools//tools/cpp:malloc"
  "@bazel_tools//third_party/def_parser:def_parser" -> "@bazel_tools//tools/cpp:current_cc_toolchain"
  "@bazel_tools//third_party/def_parser:def_parser" -> "@bazel_tools//tools/cpp:toolchain_type"
  "@bazel_tools//tools/cpp:toolchain_type"
  "@bazel_tools//tools/cpp:malloc"
  "@bazel_tools//tools/cpp:malloc" -> "@bazel_tools//tools/cpp:grep-includes"
  "@bazel_tools//tools/cpp:malloc" -> "@bazel_tools//tools/cpp:current_cc_toolchain"
  "@bazel_tools//third_party/def_parser:def_parser_lib"
  "@bazel_tools//third_party/def_parser:def_parser_lib" -> "@bazel_tools//third_party/def_parser:def_parser.cc\n@bazel_tools//third_party/def_parser:def_parser.h"
  "@bazel_tools//third_party/def_parser:def_parser_lib" -> "@bazel_tools//tools/cpp:grep-includes"
  "@bazel_tools//third_party/def_parser:def_parser_lib" -> "@bazel_tools//tools/cpp:current_cc_toolchain"
  "@bazel_tools//tools/cpp:current_cc_toolchain"
  "@bazel_tools//tools/cpp:current_cc_toolchain" -> "@bazel_tools//tools/cpp:toolchain"
  "@bazel_tools//tools/cpp:toolchain"
  "@bazel_tools//tools/cpp:toolchain" -> "@local_config_cc//:toolchain"
  "@local_config_cc//:toolchain"
  "@local_config_cc//:toolchain" -> "@local_config_cc//:cc-compiler-k8"
  "@local_config_cc//:toolchain" -> "@local_config_cc//:cc-compiler-armeabi-v7a"
  "@local_config_cc//:cc-compiler-armeabi-v7a"
  "@local_config_cc//:cc-compiler-armeabi-v7a" -> "@bazel_tools//tools/build_defs/cc/whitelists/parse_headers_and_layering_check:disabling_parse_headers_and_layering_check_allowed\n@local_config_cc//:empty\n@bazel_tools//tools/build_defs/cc/whitelists/starlark_hdrs_check:loose_header_check_allowed_in_toolchain"
  "@local_config_cc//:cc-compiler-armeabi-v7a" -> "@local_config_cc//:stub_armeabi-v7a"
  "@local_config_cc//:cc-compiler-armeabi-v7a" -> "@bazel_tools//tools/cpp:link_dynamic_library"
  "@local_config_cc//:cc-compiler-armeabi-v7a" -> "@bazel_tools//tools/cpp:interface_library_builder"
  "@local_config_cc//:stub_armeabi-v7a"
  "@local_config_cc//:cc-compiler-k8"
  "@local_config_cc//:cc-compiler-k8" -> "@local_config_cc//:compiler_deps"
  "@local_config_cc//:cc-compiler-k8" -> "@bazel_tools//tools/build_defs/cc/whitelists/parse_headers_and_layering_check:disabling_parse_headers_and_layering_check_allowed\n@local_config_cc//:empty\n@bazel_tools//tools/build_defs/cc/whitelists/starlark_hdrs_check:loose_header_check_allowed_in_toolchain"
  "@local_config_cc//:cc-compiler-k8" -> "@local_config_cc//:local"
  "@local_config_cc//:cc-compiler-k8" -> "@bazel_tools//tools/cpp:interface_library_builder"
  "@local_config_cc//:cc-compiler-k8" -> "@bazel_tools//tools/cpp:link_dynamic_library"
  "@bazel_tools//tools/cpp:link_dynamic_library"
  "@bazel_tools//tools/cpp:link_dynamic_library" -> "@bazel_tools//tools/cpp:link_dynamic_library.sh"
  "@bazel_tools//tools/cpp:link_dynamic_library.sh"
  "@bazel_tools//tools/cpp:interface_library_builder"
  "@bazel_tools//tools/cpp:interface_library_builder" -> "@bazel_tools//tools/cpp:build_interface_so"
  "@local_config_cc//:local"
  "@bazel_tools//tools/cpp:grep-includes"
  "@bazel_tools//tools/cpp:grep-includes" -> "@bazel_tools//tools/cpp:grep-includes.sh"
  "@bazel_tools//tools/cpp:grep-includes.sh"
  "@bazel_tools//third_party/def_parser:def_parser.cc\n@bazel_tools//third_party/def_parser:def_parser.h"
  "@bazel_tools//third_party/def_parser:def_parser_main.cc"
  "@bazel_tools//tools/def_parser:def_parser.exe\n@bazel_tools//src/conditions:remote"
  "@local_config_cc//:compiler_deps"
  "@local_config_cc//:compiler_deps" -> "@local_config_cc//:builtin_include_directory_paths"
  "@local_config_cc//:builtin_include_directory_paths"
  "@bazel_tools//tools/build_defs/cc/whitelists/parse_headers_and_layering_check:disabling_parse_headers_and_layering_check_allowed\n@local_config_cc//:empty\n@bazel_tools//tools/build_defs/cc/whitelists/starlark_hdrs_check:loose_header_check_allowed_in_toolchain"
  "@bazel_tools//tools/cpp:build_interface_so"
}
Loading: 0 packages loaded
```

也可安装 [xdot](https://pypi.org/project/xdot/#files) 直接显示：`bazel query --nohost_deps --noimplicit_deps 'deps(//main:hello-world)' --output graph | xdot`

```
~/tools/xdot/xdot-1.2$./setup.py install
```



![graphviz](/assets/images/202306/graphviz.svg)


# [Bazel 基础概念](https://bazel.build/concepts/build-ref?hl=en)

了解源代码布局，`BUILD` 文件语法以及规则和依赖项类型等基本概念。

![bazel_build4](/assets/images/202306/bazel_build4.png)


## [Workspaces, packages, and targets](https://bazel.build/concepts/build-ref) / [中文版](https://bazel.build/concepts/build-ref?hl=zh-cn)


Bazel 会根据名为“工作区”的目录树整理的源代码构建软件。工作区中的源文件以嵌套的软件包层次结构进行组织，其中每个软件包都是一个包含一组相关源文件和一个 BUILD 文件的目录。BUILD 文件指定可以从源代码构建哪些软件输出。

### Workspace (**工作空间**位于根目录)

A workspace is a directory tree on your filesystem that contains the source files for the software you want to build. Each workspace has a text file named `WORKSPACE` which may be empty, or may contain references to [external dependencies](https://bazel.build/docs/external) required to build the outputs.

Directories containing a file called `WORKSPACE` are considered the root of a workspace. Therefore, Bazel ignores any directory trees in a workspace rooted at a subdirectory containing a `WORKSPACE` file, as they form another workspace.

Bazel also supports `WORKSPACE.bazel` file as an alias of `WORKSPACE` file. If both files exist, `WORKSPACE.bazel` is used.


### Repositories

Code is organized in repositories. The directory containing the `WORKSPACE` file is the root of the main repository, also called `@`. Other, (external) repositories are defined in the `WORKSPACE` file using workspace rules, or generated from modules and extensions in the Bzlmod system. See [external dependencies overview](https://bazel.build/external/overview) for more information.

The workspace rules bundled with Bazel are documented in the [Workspace Rules](https://bazel.build/reference/be/workspace) section in the [Build Encyclopedia](https://bazel.build/reference/be/overview) and the documentation on [embedded Starlark repository rules](https://bazel.build/rules/lib/repo).

As external repositories are repositories themselves, they often contain a `WORKSPACE` file as well. However, these additional `WORKSPACE` files are ignored by Bazel. In particular, repositories depended upon transitively are not added automatically.


### Packages (包含 BUILD 文件的目录称为**包**)

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

### Targets (BUILD 中的每个构建规则称为**目标**)

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


### Types of build rules

The majority of build rules come in families, grouped together by language. For example, `cc_binary`, `cc_library` and `cc_test` are the build rules for C++ `binaries`, `libraries`, and `tests`, respectively. Other languages use the same naming scheme, with a different prefix, such as `java_*` for Java. Some of these functions are documented in the [Build Encyclopedia](https://bazel.build/reference/be/overview), but it is possible for anyone to create new rules.

> *_binary

`*_binary` rules build executable programs in a given language. After a build, the executable will reside in the build tool's binary output tree at the corresponding name for the rule's label, so `//my:program` would appear at (for example) `$(BINDIR)/my/program`.

In some languages, such rules also create a runfiles directory containing all the files mentioned in a `data` attribute belonging to the rule, or any rule in its transitive closure of dependencies; this set of files is gathered together in one place for ease of deployment to production.

> *_test

`*_test` rules are a specialization of a `*_binary` rule, used for automated testing. Tests are simply programs that return zero on success.

Like binaries, tests also have runfiles trees, and the files beneath it are the only files that a test may legitimately open at runtime. For example, a program `cc_test(name='x', data=['//foo:bar'])` may open and read `$TEST_SRCDIR/workspace/foo/bar` during execution. (Each programming language has its own utility function for accessing the value of `$TEST_SRCDIR`, but they are all equivalent to using the environment variable directly.) Failure to observe the rule will cause the test to fail when it is executed on a remote testing host.

> *_library

`*_library` rules specify separately-compiled modules in the given programming language. Libraries can depend on other libraries, and binaries and tests can depend on libraries, with the expected separate-compilation behavior.


## [Dependencies](https://bazel.build/concepts/dependencies?hl=en)

A target `A` depends upon a target `B` if `B` is needed by `A` at build or execution time. The depends upon relation induces a `Directed Acyclic Graph` (**DAG 有向无环图**) over targets, and it is called a dependency graph.

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



# [Bazel Tutorial: Build a C++ Project](https://bazel.build/start/cpp?hl=en)

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


# [Common C++ Build Use Cases](https://bazel.build/tutorials/cpp-use-cases?hl=en)

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

With this target, Bazel will build all the `.cc` and `.h` files it finds in the same directory as the `BUILD` file that contains this target (**excluding subdirectories**).

考虑包含子目录的情况：

```
cc_library(
    name = "build-all-the-files-include-subdirectories",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"]),
)
```


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
* bazel help [startup_options](https://bazel.build/docs/user-manual#startup-options): Options for the JVM hosting Bazel.
* bazel help [target-syntax](https://bazel.build/run/build?hl=en#specifying-build-targets): Explains the syntax for specifying targets.
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


* `--action_env=VAR=VALUE`

Specifies the set of environment variables available during the execution of all actions. Variables can be either specified by name, in which case the value will be taken from the invocation environment, or by the `name=value` pair which sets the value independent of the invocation environment.

This `--action_env` flag can be specified multiple times. If a value is assigned to the same variable across multiple `--action_env` flags, the latest assignment wins.

### Execution strategy

These options affect how Bazel will execute the build. They should not have any significant effect on the output files generated by the build. **Typically their main effect is on the speed of the build**.

* `--spawn_strategy=strategy`

This option controls where and how commands are executed.

**standalone** causes commands to be executed as local subprocesses. **This value is deprecated**. Please use **local** instead.

**sandboxed** causes commands to be executed inside a sandbox on the local machine. This requires that all input files, data dependencies and tools are listed as direct dependencies in the `srcs`, `data` and `tools` attributes. Bazel enables local sandboxing by default, on systems that support sandboxed execution.

**local** causes commands to be executed as local subprocesses.

**worker** causes commands to be executed using a persistent worker, if available.

**docker** causes commands to be executed inside a docker sandbox on the local machine. This requires that docker is installed.

**remote** causes commands to be executed remotely; this is only available if a remote executor has been configured separately.


* `--jobs=n (-j)`

This option, which takes an integer argument, specifies a limit on the number of jobs that should be executed concurrently during the execution phase of the build.

> Note: The number of concurrent jobs that Bazel will run is determined not only by the --jobs setting, but also by Bazel's scheduler, which tries to avoid running concurrent jobs that will use up more resources (RAM or CPU) than are available, based on some (very crude) estimates of the resource consumption of each job. The behavior of the scheduler can be controlled by the --local_ram_resources option.

* `--local_{ram,cpu}_resources resources or resource expression`

These options specify the amount of local resources (RAM in MB and number of CPU logical cores) that Bazel can take into consideration when scheduling build and test activities to run locally. They take an integer, or a keyword (`HOST_RAM` or `HOST_CPUS`) optionally followed by `[-|*float]` (for example, `--local_cpu_resources=2`, `--local_ram_resources=HOST_RAM*.5`, `--local_cpu_resources=HOST_CPUS-1`). The flags are independent; one or both may be set. By default, Bazel estimates the amount of RAM and number of CPU cores directly from the local system's configuration.



### Verbosity

These options control the verbosity of Bazel's output, either to the terminal, or to additional log files.

* `--explain=logfile`

This option, which requires a filename argument, causes the dependency checker in bazel build's execution phase to explain, for each build step, either why it is being executed, or that it is up-to-date. The explanation is written to logfile.

If you are encountering unexpected rebuilds, this option can help to understand the reason. Add it to your .bazelrc so that logging occurs for all subsequent builds, and then inspect the log when you see an execution step executed unexpectedly. This option may carry a small performance penalty, so you might want to remove it when it is no longer needed.


* `--verbose_explanations`

This option increases the verbosity of the explanations generated when the `--explain` option is enabled.

In particular, if verbose explanations are enabled, and an output file is rebuilt because the command used to build it has changed, then the output in the explanation file will include the full details of the new command (at least for most commands).

Using this option may significantly increase the length of the generated explanation file and the performance penalty of using `--explain`.

If `--explain` is not enabled, then `--verbose_explanations` has no effect.


* `--profile=file`

This option, which takes a filename argument, causes Bazel to write profiling data into a file. The data then can be analyzed or parsed using the `bazel analyze-profile` command. The Build profile can be useful in understanding where Bazel's `build` command is spending its time.

* `--show_result=n`

This option controls the printing of result information at the end of a `bazel build` command. By default, if a single build target was specified, Bazel prints a message stating whether or not the target was successfully brought up-to-date, and if so, the list of output files that the target created. If multiple targets were specified, result information is not displayed.

While the result information may be useful for builds of a single target or a few targets, for large builds (such as an entire top-level project tree), this information can be overwhelming and distracting; this option allows it to be controlled. `--show_result` takes an integer argument, which is the maximum number of targets for which full result information should be printed. By default, the value is 1. Above this threshold, no result information is shown for individual targets. Thus zero causes the result information to be suppressed always, and a very large value causes the result to be printed always.

* `--subcommands (-s)`

This option causes Bazel's execution phase to print the full command line for each command prior to executing it.

`--subcommands=pretty_print` may be passed to print the arguments of the command as a list rather than as a single line. This may help make long command lines more readable.

* `--verbose_failures`

This option causes Bazel's execution phase to print the full command line for commands that failed. **This can be invaluable(非常有用的) for debugging a failing build**.

Failing commands are printed in a Bourne shell compatible syntax, suitable for copying and pasting to a shell prompt.


### Workspace status

Use these options to "stamp" Bazel-built binaries: to embed additional information into the binaries, such as the source control revision or other workspace-related information. You can use this mechanism with rules that support the `stamp` attribute, such as `genrule`, `cc_binary`, and more.


* `--workspace_status_command=program`

This flag lets you specify a binary that Bazel runs before each build. The program can report information about the status of the workspace, such as the current source control revision.

The flag's value must be a path to a native program. On Linux/macOS this may be any executable.

The program should print zero or more key/value pairs to standard output, one entry on each line, then exit with zero (otherwise the build fails). The key names can be anything but they may only use upper case letters and underscores. The first space after the key name separates it from the value. The value is the rest of the line (including additional whitespaces). Neither the key nor the value may span multiple lines. Keys must not be duplicated.

Bazel partitions the keys into two buckets: "stable" and "volatile". (The names "stable" and "volatile" are a bit counter-intuitive, so don't think much about them.)

Bazel then writes the key-value pairs into two files:

1. `bazel-out/stable-status.txt` contains all keys and values where the key's name starts with `STABLE_`
2. `bazel-out/volatile-status.txt` contains the rest of the keys and their values

> The contract is:

![bazel_build3](/assets/images/202306/bazel_build3.png)


On Linux/macOS you can pass `--workspace_status_command=/bin/true` to **disable** retrieving workspace status, because `true` does nothing, successfully (exits with zero) and prints no output.

Example program on Linux using Git:

``` bash
#!/bin/bash
echo "CURRENT_TIME $(date +%s)"
echo "RANDOM_HASH $(cat /proc/sys/kernel/random/uuid)"
echo "STABLE_GIT_COMMIT $(git rev-parse HEAD)"
echo "STABLE_USER_NAME $USER"
```

Pass this program's path with `--workspace_status_command`, and the stable status file will include the STABLE lines and the volatile status file will include the rest of the lines.

```
bazel-out$ls
_actions  k8-fastbuild  stable-status.txt  _tmp  volatile-status.txt
bazel-out$cat stable-status.txt
BUILD_EMBED_LABEL
BUILD_HOST gerryyang1631112241020-0
BUILD_USER gerryyang
STABLE_GIT_COMMIT 1aaf03f07effdb6fb80ca3225187d64305b4230a
STABLE_USER_NAME gerryyang
bazel-out$cat volatile-status.txt
BUILD_TIMESTAMP 1687319715
CURRENT_TIME 1687319715
RANDOM_HASH df30a1c8-ecd4-47f9-a6b7-e0f8ba640cae
```


refer:

* https://github.com/envoyproxy/envoy/blob/release/v1.22/bazel/get_workspace_status
* https://github.com/envoyproxy/envoy/blob/release/v1.22/source/common/version/generate_version_linkstamp.sh
* https://github.com/envoyproxy/envoy/blob/release/v1.22/source/common/version/BUILD
* https://bazel.build/docs/user-manual?hl=en#workspace-status


## Cleaning build outputs


Bazel has a `clean` command, analogous to that of Make. It deletes the output directories for all build configurations performed by this Bazel instance, or the entire working tree created by this Bazel instance, and resets internal caches. If executed without any command-line options, then the output directory for all configurations will be cleaned.

Recall that each Bazel instance is associated with a single workspace, thus the `clean` command will delete all outputs from all builds you've done with that Bazel instance in that workspace.

To completely remove the entire working tree created by a Bazel instance, you can specify the `--expunge` option. When executed with `--expunge`, the clean command simply removes the entire output base tree which, in addition to the build output, contains all temp files created by Bazel. It also stops the Bazel server after the clean, equivalent to the [shutdown](https://bazel.build/docs/user-manual?hl=en#shutdown) command. For example, to clean up all disk and memory traces of a Bazel instance, you could specify:

```
% bazel clean --expunge
```

Alternatively, you can expunge in the background by using `--expunge_async`. It is safe to invoke a Bazel command in the same client while the asynchronous expunge continues to run.

> Note: This may introduce IO contention.

The `clean` command is provided primarily as a means of reclaiming disk space for workspaces that are no longer needed. Bazel's incremental rebuilds may not be perfect so `clean` can be used to recover a consistent state when problems arise.

Bazel's design is such that these problems are fixable and these bugs are a high priority to be fixed. If you ever find an incorrect incremental build, file a bug report, and report bugs in the tools rather than using `clean`.



# [C++ and Bazel](https://bazel.build/docs/bazel-and-cpp?hl=en)

This page contains resources that help you use Bazel with C++ projects. It links to a tutorial, build rules, and other information specific to building C++ projects with Bazel.

## Working with Bazel (C++)

The following resources will help you work with Bazel on C++ projects:

* [Tutorial: Building a C++ project](https://bazel.build/start/cpp?hl=zh-cn)
* [C++ common use cases](https://bazel.build/tutorials/cpp-use-cases?hl=en)
* [C/C++ rules](https://bazel.build/reference/be/c-cpp?hl=en)
* Essential Libraries
  + [Abseil](https://abseil.io/docs/cpp/quickstart)
  + [Boost](https://github.com/nelhage/rules_boost)
  + [HTTPS Requests: CPR and libcurl](https://github.com/hedronvision/bazel-make-cc-https-easy)
* [C++ toolchain configuration](https://bazel.build/docs/cc-toolchain-config-reference)
* [Tutorial: Configuring C++ toolchains](https://bazel.build/tutorials/ccp-toolchain-config)
* [Integrating with C++ rules](https://bazel.build/configure/integrate-cpp?hl=zh-cn)


## [C/C++ rules](https://bazel.build/reference/be/c-cpp?hl=en)

### [cc_binary](https://bazel.build/reference/be/c-cpp?hl=en#cc_binary)

```
cc_binary(name, deps, srcs, data, additional_linker_inputs, args, compatible_with, copts, defines, deprecation, distribs, env, exec_compatible_with, exec_properties, features, includes, licenses, linkopts, linkshared, linkstatic, local_defines, malloc, nocopts, output_licenses, restricted_to, stamp, tags, target_compatible_with, testonly, toolchains, visibility, win_def_file)
```

### [cc_import](https://bazel.build/reference/be/c-cpp?hl=en#cc_import)

```
cc_import(name, deps, data, hdrs, alwayslink, compatible_with, deprecation, distribs, features, interface_library, licenses, restricted_to, shared_library, static_library, system_provided, tags, target_compatible_with, testonly, visibility)
```

### [cc_library](https://bazel.build/reference/be/c-cpp?hl=en#cc_library)

```
cc_library(name, deps, srcs, data, hdrs, alwayslink, compatible_with, copts, defines, deprecation, distribs, exec_compatible_with, exec_properties, features, implementation_deps, include_prefix, includes, licenses, linkopts, linkstamp, linkstatic, local_defines, nocopts, restricted_to, strip_include_prefix, tags, target_compatible_with, testonly, textual_hdrs, toolchains, visibility, win_def_file)
```

### [cc_proto_library](https://bazel.build/reference/be/c-cpp?hl=en#cc_proto_library)

```
cc_proto_library(name, deps, data, compatible_with, deprecation, distribs, exec_compatible_with, exec_properties, features, licenses, restricted_to, tags, target_compatible_with, testonly, visibility)
```

### [cc_shared_library](https://bazel.build/reference/be/c-cpp?hl=en#cc_shared_library)

```
cc_shared_library(name, deps, additional_linker_inputs, dynamic_deps, exports_filter, shared_lib_name, tags, user_link_flags, win_def_file)
```

### [fdo_prefetch_hints](https://bazel.build/reference/be/c-cpp?hl=en#fdo_prefetch_hints)

```
fdo_prefetch_hints(name, compatible_with, deprecation, distribs, features, licenses, profile, restricted_to, tags, target_compatible_with, testonly, visibility)
```

### [fdo_profile](https://bazel.build/reference/be/c-cpp?hl=en#fdo_profile)

```
fdo_profile(name, absolute_path_profile, compatible_with, deprecation, distribs, features, licenses, profile, proto_profile, restricted_to, tags, target_compatible_with, testonly, visibility)
```

### [propeller_optimize](https://bazel.build/reference/be/c-cpp?hl=en#propeller_optimize)

```
propeller_optimize(name, compatible_with, deprecation, distribs, features, ld_profile, licenses, restricted_to, tags, target_compatible_with, testonly, visibility)
```

### [cc_test](https://bazel.build/reference/be/c-cpp?hl=en#cc_test)

```
cc_test(name, deps, srcs, data, additional_linker_inputs, args, compatible_with, copts, defines, deprecation, distribs, env, env_inherit, exec_compatible_with, exec_properties, features, flaky, includes, licenses, linkopts, linkstatic, local, local_defines, malloc, nocopts, restricted_to, shard_count, size, stamp, tags, target_compatible_with, testonly, timeout, toolchains, visibility, win_def_file)
```

### [cc_toolchain](https://bazel.build/reference/be/c-cpp?hl=en#cc_toolchain)

```
cc_toolchain(name, all_files, ar_files, as_files, compatible_with, compiler, compiler_files, compiler_files_without_includes, coverage_files, cpu, deprecation, distribs, dwp_files, dynamic_runtime_lib, exec_transition_for_inputs, features, libc_top, licenses, linker_files, module_map, objcopy_files, restricted_to, static_runtime_lib, strip_files, supports_header_parsing, supports_param_files, tags, target_compatible_with, testonly, toolchain_config, toolchain_identifier, visibility)
```

### [cc_toolchain_suite](https://bazel.build/reference/be/c-cpp?hl=en#cc_toolchain_suite)

```
cc_toolchain_suite(name, compatible_with, deprecation, distribs, features, licenses, restricted_to, tags, target_compatible_with, testonly, toolchains, visibility)
```



# [Best Practices (C++)](https://bazel.build/configure/best-practices?hl=en)

The overall goals are:

* To use fine-grained dependencies to allow parallelism and incrementality.
* To keep dependencies well-encapsulated.
* To make code well-structured and testable.
* To create a build configuration that is easy to understand and maintain.

In addition to [general Bazel best practices](https://bazel.build/configure/best-practices?hl=zh-cn), below are best practices specific to C++ projects.

## BUILD files

Follow the guidelines below when creating your `BUILD` files:

* Each `BUILD` file should contain one `cc_library` rule target per compilation unit in the directory.

* You should granularize(粒度化) your C++ libraries as much as possible to maximize incrementality and parallelize the build.

* If there is a single source file in `srcs`, name the library the same as that C++ file's name. This library should contain C++ file(s), any matching header file(s), and the library's direct dependencies. For example:

```
cc_library(
    name = "mylib",
    srcs = ["mylib.cc"],
    hdrs = ["mylib.h"],
    deps = [":lower-level-lib"]
)
```

* Use one `cc_test` rule target per `cc_library` target in the file. Name the target `[library-name]_test` and the source file `[library-name]_test.cc`. For example, a test target for the mylib library target shown above would look like this:

```
cc_test(
    name = "mylib_test",
    srcs = ["mylib_test.cc"],
    deps = [":mylib"]
)
```

## Include paths

Follow these guidelines for include paths:

* Make all include paths relative to the workspace directory.

* Use quoted includes (`#include "foo/bar/baz.h"`) for non-system headers, not angle-brackets (`#include <foo/bar/baz.h>`).

* Avoid using UNIX directory shortcuts, such as `.` (current directory) or `..` (parent directory).

* For legacy or third_party code that requires includes pointing outside the project repository, such as external repository includes requiring a prefix, use the [include_prefix](https://bazel.build/reference/be/c-cpp#cc_library.include_prefix) and [strip_include_prefix](https://bazel.build/reference/be/c-cpp#cc_library.strip_include_prefix) arguments on the cc_library rule target.


# [General Rules](https://bazel.build/reference/be/general?hl=en)

* [alias](https://bazel.build/reference/be/general?hl=en#alias)
* [config_setting](https://bazel.build/reference/be/general?hl=en#config_setting)
* [filegroup](https://bazel.build/reference/be/general?hl=en#filegroup)
* [genquery](https://bazel.build/reference/be/general?hl=en#genquery)
* [genrule](https://bazel.build/reference/be/general?hl=en#genrule)
* [test_suite](https://bazel.build/reference/be/general?hl=en#test_suite)


# [External dependencies overview](https://bazel.build/external/overview?hl=en)

Bazel supports external dependencies, source files (both text and binary) used in your build that are not from your workspace. For example, they could be a ruleset hosted in a GitHub repo, a Maven artifact, or a directory on your local machine outside your current workspace.

As of Bazel 6.0, there are two ways to manage external dependencies with Bazel: the traditional, repository-focused [WORKSPACE](https://bazel.build/external/overview?hl=en#workspace-system) system, and the newer module-focused [MODULE.bazel](https://bazel.build/external/overview?hl=en#bzlmod) system (codenamed Bzlmod, and enabled with the flag `--enable_bzlmod`). The two systems can be used together, but `Bzlmod` is replacing the `WORKSPACE` system in future Bazel releases.


# [Bazel Tutorial: Configure C++ Toolchains](https://bazel.build/tutorials/ccp-toolchain-config?hl=en)

This tutorial uses an example scenario to describe how to configure C++ toolchains for a project. It's based on an [example C++ project](https://github.com/bazelbuild/examples/tree/master/cpp-tutorial/stage1) that builds error-free using clang.


# [Workspace Rules](https://bazel.build/reference/be/workspace?hl=en)

Workspace rules are used to pull in [external dependencies](https://bazel.build/docs/external), typically source code located outside the main repository.

> Note: besides the native workspace rules, Bazel also embeds various [Starlark workspace rules](https://bazel.build/rules/lib/repo), in particular those to deal with git repositories or archives hosted on the web.

Rules:

* [bind](https://bazel.build/reference/be/workspace?hl=en#bind)
* [local_repository](https://bazel.build/reference/be/workspace?hl=en#local_repository)
* [new_local_repository](https://bazel.build/reference/be/workspace?hl=en#new_local_repository)

## local_repository

Allows targets from a local directory to be bound. This means that the current repository can use targets defined in this other directory. See the [bind section](https://bazel.build/reference/be/workspace#bind_examples) for more details.

Examples:

Suppose the current repository is a chat client, rooted at the directory `~/chat-app`. It would like to use an SSL library which is defined in a different repository: `~/ssl`. The SSL library has a target `//src:openssl-lib`.

The user can add a dependency on this target by adding the following lines to `~/chat-app/WORKSPACE`:

```
local_repository(
    name = "my-ssl",
    path = "/home/user/ssl",
)
```

Targets would specify `@my-ssl//src:openssl-lib` as a dependency to depend on this library.


## new_local_repository

Allows a local directory to be turned into a Bazel repository (允许一个本地目录转换为一个 Bazel repository). This means that the current repository can define and use targets from anywhere on the filesystem.

This rule creates a Bazel repository by creating a `WORKSPACE` file and subdirectory containing symlinks to the `BUILD` file and path given. The build file should create targets relative to the path. For directories that already contain a `WORKSPACE` file and a `BUILD` file, the `local_repository` rule can be used.

Examples:

Suppose the current repository is a chat client, rooted at the directory `~/chat-app`. It would like to use an SSL library which is defined in a different directory: `~/ssl`.

The user can add a dependency by creating a `BUILD` file for the SSL library (`~/chat-app/BUILD.my-ssl`) containing:

```
java_library(
    name = "openssl",
    srcs = glob(['*.java'])
    visibility = ["//visibility:public"],
)
```

Then they can add the following lines to `~/chat-app/WORKSPACE`:

```
new_local_repository(
    name = "my-ssl",
    path = "/home/user/ssl",
    build_file = "BUILD.my-ssl",
)
```

This will create a `@my-ssl` repository that symlinks to `/home/user/ssl`. Targets can depend on this library by adding `@my-ssl//:openssl` to a target's dependencies.

You can also use `new_local_repository` to include single files, not just directories. For example, suppose you had a jar file at `/home/username/Downloads/piano.jar`. You could add just that file to your build by adding the following to your `WORKSPACE` file:

```
new_local_repository(
    name = "piano",
    path = "/home/username/Downloads/piano.jar",
    build_file = "BUILD.piano",
)
```

And creating the following `BUILD.piano` file:

```
java_import(
    name = "play-music",
    jars = ["piano.jar"],
    visibility = ["//visibility:public"],
)
```

Then targets can depend on `@piano//:play-music` to use `piano.jar`.


# [Remote Caching - RC](https://bazel.build/remote/caching?hl=en)

> 之前构建过，即使本地 clean 了，也可以命中 RC 的缓存。RC 是用 IO 开销和存储开销，换取计算开销。bazel 会请求 RC 看某一个 action 是否存在缓存，存在则不再执行，通过 bazel print_action 可以查看具体某个 binary 存在哪些 action。

This page covers remote caching, setting up a server to host the cache, and running builds using the remote cache.

A remote cache is used by a team of developers and/or a continuous integration (CI) system to share build outputs. If your build is reproducible, the outputs from one machine can be safely reused on another machine, which can make builds significantly faster.


`--remote_cache` 是 Bazel 的一个命令行选项，允许您使用远程缓存服务来提高构建速度。当 Bazel 构建时，它将会将动作执行的结果缓存远程。在后续构建中，如果这些缓存结果仍然有效，那么它们将从远程缓存中直接提取，从而避免对已经构建过的结果进行重复计算。这意味着如果远程缓存中已经有某个操作的缓存结果，那么 Bazel 构建过程中就不需要在本地执行这个操作。

为了在 Bazel 中使用远程缓存，请按照以下步骤操作：

* 首先，设置一个远程缓存服务。Bazel 可以与很多 HTTP 缓存系统兼容，例如 Bazel Remote Cache、Google Cloud Storage、S3、nginx 等。根据您需要使用的缓存系统部署远程缓存，确保远程缓存可从您的构建机器进行访问。

* 然后，在命令行中使用 `--remote_cache` 选项，指定远程缓存服务器的 URL。例如：

```
bazel build //:my_target --remote_cache=http://remote.cache.server.com:port
```

这里 `http://remote.cache.server.com:port` 是部署的远程缓存服务器地址。在构建目标（例如 `//:my_target`）时，Bazel 将使用指定的远程缓存服务器。

还可以将这个选项加入到项目根目录下的 `.bazelrc` 文件中，这样就不需要在每次构建时手动指定选项：

```
build --remote_cache=http://remote.cache.server.com:port
```

当使用远程缓存时，请注意以下方面：

* 为了在不同构建之间共享缓存，远程缓存需要与项目中的所有构建机器共享。
* 在共享库或以构建链的方式进行构建时，多个项目可以充分利用远程缓存的好处。

缓存可以提高跨越多个构建的构建速度，但请注意远程缓存可能会延迟单个构建的开始时间，因为 Bazel 需要在网络上完成远程缓存查询。不过，通常情况下，大分析缓存命中率可以弥补这些延迟，总体上实现更快的构建速度。


# [Remote Execution Overview - RE](https://bazel.build/remote/rbe?hl=en)

This page covers the benefits, requirements, and options for running Bazel with remote execution.

By default, Bazel executes builds and tests on your local machine. Remote execution of a Bazel build allows you to distribute build and test actions across multiple machines, such as a datacenter.

Remote execution provides the following benefits:

* Faster build and test execution through scaling of nodes available for parallel actions
* A consistent execution environment for a development team
* Reuse of build outputs across a development team

Bazel uses an open-source [gRPC protocol](https://github.com/bazelbuild/remote-apis) to allow for remote execution and remote caching.


Requirements:

Remote execution of Bazel builds imposes a set of mandatory configuration constraints on the build. For more information, see [Adapting Bazel Rules for Remote Execution](https://bazel.build/remote/rules?hl=en).





# 如何分析构建耗时

bazel 构建分为三个阶段：Loading, Analysising and Executing

![bazel_build6](/assets/images/202306/bazel_build6.png)


分析构建任务耗时，要关注完整的构建过程，不要仅关注第三阶段。通过参数 `--profile` 可以查看三个阶段各自的耗时：

第一步：构建时生成 profile 数据

执行 bazel 时增加 `--profile=bazel_profile.log` 相关参数，可将构建阶段耗时信息详细记录到 bazel_profile.log 文件中

第二步：查看 profile 数据

执行 analyze-profile 命令，查看 profile 数据

```
$bazel analyze-profile bazel_profile.log
WARNING: This information is intended for consumption by Bazel developers only, and may change at any time. Script against it at your own risk
INFO: Profile created on 2023-06-30T03:16:36.524737Z, build ID: 56b5d6fc-f100-43c6-8564-b5f3819ed9de, output base: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6

=== PHASE SUMMARY INFORMATION ===

Total launch phase time                              0.013 s    0.08%
Total init phase time                                0.601 s    3.79%
Total target pattern evaluation phase time           0.203 s    1.28%
Total interleaved loading-and-analysis phase time    1.421 s    8.97%
Total preparation phase time                         0.013 s    0.09%
Total execution phase time                          13.575 s   85.65%
Total finish phase time                              0.022 s    0.14%
---------------------------------------------------------------------
Total run time                                      15.850 s  100.00%

Critical path (13.428 s):
       Time Percentage   Description
    76.6 ms    0.57%   action 'Writing script frame/common/common_lib.cppmap'
    9.996 s   74.44%   action 'Compiling src/unittestsvr/UnittestCtrl.cpp'
    3.355 s   24.99%   action 'Linking src/unittestsvr/unittestsvr'
    0.18 ms    0.00%   runfiles for //src/unittestsvr unittestsvr
```

通过分析上述 profiling 结果数据，可以看到 bazel 各阶段的耗时，以及 action 具体的编译耗时，根据这些信息可以大致了解构建耗时情况。


# Tips

## .bazelignore 忽略配置

在 WORKSPACE 目录下通过 `.bazelignore` 配置指定忽略的 BUILD。


## [Sharing Variables](https://bazel.build/build/share-variables?hl=en)

If it is useful to share values (for example, if values must be kept in sync), you can introduce a variable:

```
COPTS = ["-DVERSION=5"]

cc_library(
  name = "foo",
  copts = COPTS,
  srcs = ["foo.cc"],
)

cc_library(
  name = "bar",
  copts = COPTS,
  srcs = ["bar.cc"],
  deps = [":foo"],
)
```

Multiple declarations now use the value `COPTS`. By convention, use uppercase letters to name global constants.

## Sharing variables across multiple BUILD files

If you need to share a value across multiple `BUILD` files, you have to put it in a `.bzl` file. `.bzl` files contain definitions (variables and functions) that can be used in `BUILD` files.

In path/to/variables.bzl, write:

```
COPTS = ["-DVERSION=5"]
```

Then, you can update your `BUILD` files to access the variable:

```
load("//path/to:variables.bzl", "COPTS")

cc_library(
  name = "foo",
  copts = COPTS,
  srcs = ["foo.cc"],
)

cc_library(
  name = "bar",
  copts = COPTS,
  srcs = ["bar.cc"],
  deps = [":foo"],
)
```

## [Recommended Rules](https://bazel.build/community/recommended-rules?hl=en)

In the documentation, we provide a list of [recommended rules](https://bazel.build/rules).

This is a set of high quality rules, which will provide a good experience to our users. We make a distinction between the supported rules, and the hundreds of rules you can find on the Internet.



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

## bazel analyze-profile

通过 `--profile=file` 进行性能分析。

```
$bazel analyze-profile bazel_profile_output.log
WARNING: This information is intended for consumption by Bazel developers only, and may change at any time. Script against it at your own risk
INFO: Profile created on 2023-06-21T03:22:35.541621Z, build ID: c980b6c0-af1a-46ef-a5ca-cacc3d3f7264, output base: /data/home/gerryyang/.cache/bazel/_bazel_gerryyang/31b5c5a4697c67885c83a7460c9628d6

=== PHASE SUMMARY INFORMATION ===

Total launch phase time                              0.010 s    0.02%
Total init phase time                                0.102 s    0.23%
Total target pattern evaluation phase time           0.043 s    0.10%
Total interleaved loading-and-analysis phase time    0.237 s    0.53%
Total preparation phase time                         0.001 s    0.00%
Total execution phase time                          44.646 s   99.12%
Total finish phase time                              0.002 s    0.01%
---------------------------------------------------------------------
Total run time                                      45.043 s  100.00%

Critical path (17.842 s):
       Time Percentage   Description
    5.12 ms    0.03%   action 'Writing script external/bazel_tools/tools/cpp/malloc.cppmap'
   14.314 s   80.23%   action 'Compiling src/unittestsvr1/UnittestCtrl.cpp'
    3.523 s   19.74%   action 'Linking src/unittestsvr1/unittestsvr1'
    0.09 ms    0.00%   runfiles for //src/unittestsvr1 unittestsvr1
```

## bazel print_action (查看有哪些 action)

```
bazel print_action //src/unittestsvr:unittestsvr
```





# Tools

## [bazelisk](https://github.com/bazelbuild/bazelisk)

`Bazelisk` is **a wrapper for Bazel** written in `Go`. It automatically picks a good version of Bazel given your current working directory, downloads it from the official server (if required) and then transparently passes through all command-line arguments to the real Bazel binary. You can call it just like you would call `Bazel`.

> Bazelisk 原理

Bazel 版本更新的非常快，从 1.x 到 4.x 只用了 1 年多的时间。如果按照传统的安装方法，安装特定版本的话，将会面临如下的两个问题：

1. 如何快速升级 Bazel，跟上 Bazel 新版本的功能将是一个比较挑战的问题。
2. 不同仓库可能需要不同的 Bazel 版本，比如版本库 A 使用了 Bazel1.x，而版本库 B 使用了 Bazel4.x。那么这样会导致在同一台机难易同时支持构建这两个版本库。

非常幸运的是，Bazel 官方提供的 `Bazelisk` 模块，能够解决我们这个问题。Bazelisk 是基于 GO 语言写的，能自动管理 Bazel 版本。其原理如下：

通过安装 Bazelisk 模块，将 bazel 命令指向 bazelisk 指令集。这样，当用户在任何地方运行 Bazel 的时候，bazelisk 会检查用户指定的 Bazel 版本号。

1. 如果用户有指定版本号，则 bazelisk 会判断当前机器是否已经安装了该 Bazel 版本。如果有的话，则直接启动该版本；如果不存在，则 bazelisk 会像 bazel 官方 github 网站自动下载该版本的 bazel，然后启动。
2. 如果用户没有指定版本号，则使用当前官方最新的版本。相当于 `lastest`。

bazelisk 通过以下的方式查找指定的 Bazel 版本号。

1. 查找环境变量 `USE_BAZEL_VERSION`。如果该环境变量有指定版本号，则使用之
2. 查找当前 `WORKSPACE` 是否包含 `.bazeliskrc` 文件，如果有的话取里面定义的 `USE_BAZEL_VERSION` 变量所指向的版本号
3. 从当前目录查找 `.bazelversion` 文件，如果当前目录没有，则一级一级往其父目录查找 `.bazelversion`，直到 `WORKSPACE` 的根目录。如果存在 `.bazelversion` 文件，则直接取该文件里面指定的版本号
4. 以上都没有的话，默认为 `lastest`，相当于官方最新的版本。

> `bazelisk` 下载每个版本之后，统一存放在本地的一个目录中，根据操作系统不同，其默认的下载位置不同。其中，Unix/Linux 上的位置是 `~/.cache/bazelisk/downloads/bazelbuild`






## [bazel-gazelle](https://github.com/bazelbuild/bazel-gazelle)

`Gazelle` is a build file generator for Bazel projects. It can create new BUILD.bazel files for a project that follows language conventions, and it can update existing build files to include new sources, dependencies, and options. Gazelle natively supports Go and protobuf, and it may be extended to support new languages and custom rule sets.

* [How use bazel-gazelle in cpp project?](https://github.com/bazelbuild/bazel-gazelle/issues/910)


## [bazel_rules_install](https://github.com/google/bazel_rules_install)

Bazel rules for installing build results. Similar to `make install`.


## [bazel-skylib](https://github.com/bazelbuild/bazel-skylib)

`Skylib` is a library of `Starlark` functions for manipulating collections, file paths, and various other data types in the domain of Bazel build rules.

Each of the `.bzl` files in the `lib` directory defines a "module"—a `struct` that contains a set of related functions and/or other symbols that can be loaded as a single unit, for convenience.

Skylib also provides build rules under the `rules` directory.



## [Aspect's Bazel helpers library](https://github.com/aspect-build/bazel-lib/tree/main)

Common useful functions for writing BUILD files and Starlark macros/rules.





# 最佳实践

## 增加 bazel 并发度，提升构建效率 (--jobs)

在执行 bazel 命令时设置 `--jobs` 参数，不设置时 bazel 默认为当前服务器的 CPU 核数，即 `HOST_CPUS`。

下图为某测试项目在不同并发度 `HOST_CPUS * {1, 2, 3, 4}` 下的构建耗时情况：

![bazel_build7](/assets/images/202306/bazel_build7.png)

注意：并发度并不是越高越好，要基于服务器配置和项目实际情况进行调整，确定最佳值。



## [How do I install a project built with bazel?](https://stackoverflow.com/questions/43549923/how-do-i-install-a-project-built-with-bazel)

* https://github.com/bazelbuild/bazel-skylib (可用)
* https://github.com/google/bazel_rules_install
* https://github.com/aspect-build/bazel-lib/blob/main/docs/copy_directory.md






# Bazel 源码

https://cs.opensource.google/bazel/bazel;l=1336;drc=b56a2aa709dcb681cfc3faa148a702015ec631d5

# [Bazel Release Model](https://bazel.build/release?hl=en) (版本发布说明)

As announced in [the original blog post](https://blog.bazel.build/2020/11/10/long-term-support-release.html), Bazel 4.0 and higher versions provides support for two release tracks: **rolling releases** and **long term support (LTS) releases**.

Bazel uses a `major.minor.patch` Semantic Versioning scheme.

* A `major` release contains features that are not backward compatible with the previous release. Each major Bazel version is an LTS release.
* A `minor` release contains backward-compatible bug fixes and features back-ported from the main branch.
* A `patch` release contains critical bug fixes.



# Books


* [Beginning Bazel: Building and Testing for Java, Go, and More](https://github.com/Apress/beginning-bazel/blob/master/README.md)
* 下载地址：
  + https://drive.weixin.qq.com/s?k=AJEAIQdfAAoXSWclXBAAQAmwaCACc
  + file:///Users/gerry/Downloads/beginning-bazel-building-and-testing-for-java-go-and-more_compress.pdf
* 对应代码：https://github.com/Apress/beginning-bazel

# Q&A

* [Link archive to shared library with Bazel](https://stackoverflow.com/questions/61487115/link-archive-to-shared-library-with-bazel)



# Refer

* https://bazel.build/start
* https://bazel.build/tutorials/cpp
* https://bazel.build/tutorials/cpp-use-cases
* https://bazel.build/reference?hl=zh-cn
* [Bazel学习笔记](https://blog.gmem.cc/bazel-study-note)



* Remote Cache
  * [Using Remote Cache Service for Bazel - pdf](https://dl.acm.org/doi/pdf/10.1145/3267120)
  * [Using Remote Cache Service for Bazel](https://cacm.acm.org/magazines/2019/1/233524-using-remote-cache-service-for-bazel/fulltext)










