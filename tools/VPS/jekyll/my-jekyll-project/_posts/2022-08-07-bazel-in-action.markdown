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

# Bazel 构建优化方案

1. 使用远程缓存：Bazel 支持远程缓存，这可以显著提高构建速度
2. 使用远程执行：Bazel 支持将构建和测试任务分发到远程执行器上
3. 优化构建规则：优化 Bazel 的构建规则，确保没有不必要的依赖
4. 使用增量构建：Bazel 支持增量构建，只构建发生变化的部分
5. 使用工作空间缓存：Bazel 支持将构建结果缓存到本地磁盘
6. 调整并行度：Bazel 支持配置并行构建的线程数
7. 采用分层构建：将项目分解为多个模块，每个模块只依赖于其下游模块
8. 使用 Bazel 的分析器：Bazel 提供了分析工具，可以帮助找到构建过程中的瓶颈


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

> 参考 [How does Bazelisk know which Bazel version to run?](https://github.com/bazelbuild/bazelisk#how-does-bazelisk-know-which-bazel-version-to-run) 控制使用的 bazel 版本。


# Bazel 版本升级

## [Bzlmod Migration Guide](https://bazel.build/external/migration)

Due to the [shortcomings of WORKSPACE](https://bazel.build/external/overview#workspace-shortcomings), `Bzlmod` is replacing the legacy `WORKSPACE` system. The `WORKSPACE` file is already disabled in **Bazel 8** (late 2024) and will be removed in **Bazel 9** (late 2025). This guide helps you migrate your project to `Bzlmod` and drop `WORKSPACE` for managing external dependencies.

### Shortcomings of the WORKSPACE system

In the years since the `WORKSPACE` system was introduced, users have reported many pain points, including:

* Bazel does not evaluate the `WORKSPACE` files of any dependencies, so all transitive dependencies must be defined in the `WORKSPACE` file of the main repo, in addition to direct dependencies.

* To work around this, projects have adopted the "deps.bzl" pattern, in which they define a macro which in turn defines multiple repos, and ask users to call this macro in their `WORKSPACE` files.
  + This has its own problems: macros cannot `load` other `.bzl` files, so these projects have to define their transitive dependencies in this "deps" macro, or work around this issue by having the user call multiple layered "deps" macros.
  + Bazel evaluates the `WORKSPACE` file sequentially. Additionally, dependencies are specified using `http_archive` with URLs, without any version information. This means that there is no reliable way to perform version resolution in the case of diamond dependencies (A depends on B and C; B and C both depend on different versions of D).

Due to the shortcomings of `WORKSPACE`, Bzlmod is going to replace the legacy `WORKSPACE` system in future Bazel releases. Please read the [Bzlmod migration guide](https://bazel.build/external/migration) on how to migrate to Bzlmod.


### Why migrate to Bzlmod?

* There are many [advantages](https://bazel.build/external/migration#benefits-of-bzlmod) compared to the legacy `WORKSPACE` system, which helps to ensure a healthy growth of the Bazel ecosystem.

* If your project is a dependency of other projects, migrating to `Bzlmod` will unblock their migration and make it easier for them to depend on your project.

* Migration to `Bzlmod` is a necessary step in order to use future Bazel versions (mandatory(强制性的) in Bazel 9).



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


# [Output Directory Layout](https://bazel.build/remote/output-directories?hl=en) (输出目录布局)

## Current layout (当前布局)

The solution that's currently implemented:

* Bazel must be invoked from a directory containing a `WORKSPACE` file (the "workspace directory"), or a subdirectory thereof. It reports an error if it is not.

* The `outputRoot` directory defaults to `~/.cache/bazel` on **Linux**, `/private/var/tmp` on macOS, and on Windows it defaults to `%HOME%` if set, else `%USERPROFILE%` if set, else the result of calling `SHGetKnownFolderPath()` with the `FOLDERID_Profile` flag set. If the environment variable `$TEST_TMPDIR` is set, as in a test of Bazel itself, then that value overrides the default.

* The Bazel user's build state is located beneath `outputRoot/_bazel_$USER`. This is called the **outputUserRoot** directory.

* Beneath the **outputUserRoot** directory there is an `install` directory, and in it is an `installBase` directory whose name is the MD5 hash of the Bazel installation manifest.

* Beneath the **outputUserRoot** directory, an **outputBase** directory is also created whose name is the MD5 hash of the path name of the workspace directory. So, for example, if Bazel is running in the workspace directory `/home/user/src/my-project` (or in a directory symlinked to that one), then an output base directory is created called: `/home/user/.cache/bazel/_bazel_user/7ffd56a6e4cb724ea575aba15733d113`. You can also run `echo -n $(pwd) | md5sum` in a Bazel workspace to get the MD5.

* You can use Bazel's `--output_base` startup option to override the default output base directory. For example, `bazel --output_base=/tmp/bazel/output build x/y:z`.

* You can also use Bazel's `--output_user_root` startup option to override the default install base and output base directories. For example: `bazel --output_user_root=/tmp/bazel build x/y:z`.

The symlinks for "`bazel-<workspace-name>`", "`bazel-out`", "`bazel-testlogs`", and "`bazel-bin`" are put in the workspace directory; these symlinks point to some directories inside a target-specific directory inside the output directory. These symlinks are only for the user's convenience, as Bazel itself does not use them. Also, this is done only if the workspace directory is writable.

```
--output_base 和 --output_user_root 都是 Bazel 配置选项，用于设置 Bazel 缓存和输出文件的存储位置。它们之间的主要区别在于它们影响的目录结构层次。

--output_base：此选项设置 Bazel 的输出基目录。Bazel 会在此目录下创建一个名为 execroot 的子目录，用于存储构建过程中的所有文件，包括缓存、构建输出、日志等。此选项允许您为所有 Bazel 项目设置一个统一的输出基目录。例如，如果您设置 --output_base=/data/cache/bazel，那么所有项目的构建输出和缓存将存储在 /data/cache/bazel/execroot 目录下。

--output_user_root：此选项设置 Bazel 的用户根目录。Bazel 会在此目录下为每个工作区创建一个子目录，用于存储与特定工作区相关的缓存和输出文件。例如，如果您设置 --output_user_root=/dev/shm/bazel_cache，那么每个工作区的构建输出和缓存将存储在 /dev/shm/bazel_cache/<workspace_name> 目录下。

总之，--output_base 设置了一个全局的输出基目录，适用于所有 Bazel 项目，而 --output_user_root 设置了一个用户根目录，允许为每个工作区创建单独的缓存和输出目录。在大多数情况下，设置 --output_user_root 更具灵活性，因为它允许您为不同的工作区分配不同的缓存和输出目录。然而，如果您希望为所有项目设置一个统一的缓存和输出位置，可以选择使用 --output_base。
```

## Layout diagram (布局示意图)

The directories are laid out as follows:

```
<workspace-name>/                         <== The workspace directory
  bazel-my-project => <...my-project>     <== Symlink to execRoot
  bazel-out => <...bin>                   <== Convenience symlink to outputPath
  bazel-bin => <...bin>                   <== Convenience symlink to most recent written bin dir $(BINDIR)
  bazel-testlogs => <...testlogs>         <== Convenience symlink to the test logs directory

/home/user/.cache/bazel/                  <== Root for all Bazel output on a machine: outputRoot
  _bazel_$USER/                           <== Top level directory for a given user depends on the user name:
                                              outputUserRoot
    install/
      fba9a2c87ee9589d72889caf082f1029/   <== Hash of the Bazel install manifest: installBase
        _embedded_binaries/               <== Contains binaries and scripts unpacked from the data section of
                                              the bazel executable on first run (such as helper scripts and the
                                              main Java file BazelServer_deploy.jar)
    7ffd56a6e4cb724ea575aba15733d113/     <== Hash of the client's workspace directory (such as
                                              /home/user/src/my-project): outputBase
      action_cache/                       <== Action cache directory hierarchy
                                              This contains the persistent record of the file
                                              metadata (timestamps, and perhaps eventually also MD5
                                              sums) used by the FilesystemValueChecker.
      command.log                         <== A copy of the stdout/stderr output from the most
                                              recent bazel command.
      external/                           <== The directory that remote repositories are
                                              downloaded/symlinked into.
      server/                             <== The Bazel server puts all server-related files (such
                                              as socket file, logs, etc) here.
        jvm.out                           <== The debugging output for the server.
      execroot/                           <== The working directory for all actions. For special
                                              cases such as sandboxing and remote execution, the
                                              actions run in a directory that mimics execroot.
                                              Implementation details, such as where the directories
                                              are created, are intentionally hidden from the action.
                                              Every action can access its inputs and outputs relative
                                              to the execroot directory.
        <workspace-name>/                 <== Working tree for the Bazel build & root of symlink forest: execRoot
          _bin/                           <== Helper tools are linked from or copied to here.

          bazel-out/                      <== All actual output of the build is under here: outputPath
            _tmp/actions/                 <== Action output directory. This contains a file with the
                                              stdout/stderr for every action from the most recent
                                              bazel run that produced output.
            local_linux-fastbuild/        <== one subdirectory per unique target BuildConfiguration instance;
                                              this is currently encoded
              bin/                        <== Bazel outputs binaries for target configuration here: $(BINDIR)
                foo/bar/_objs/baz/        <== Object files for a cc_* rule named //foo/bar:baz
                  foo/bar/baz1.o          <== Object files from source //foo/bar:baz1.cc
                  other_package/other.o   <== Object files from source //other_package:other.cc
                foo/bar/baz               <== foo/bar/baz might be the artifact generated by a cc_binary named
                                              //foo/bar:baz
                foo/bar/baz.runfiles/     <== The runfiles symlink farm for the //foo/bar:baz executable.
                  MANIFEST
                  <workspace-name>/
                    ...
              genfiles/                   <== Bazel puts generated source for the target configuration here:
                                              $(GENDIR)
                foo/bar.h                     such as foo/bar.h might be a headerfile generated by //foo:bargen
              testlogs/                   <== Bazel internal test runner puts test log files here
                foo/bartest.log               such as foo/bar.log might be an output of the //foo:bartest test with
                foo/bartest.status            foo/bartest.status containing exit status of the test (such as
                                              PASSED or FAILED (Exit 1), etc)
              include/                    <== a tree with include symlinks, generated as needed. The
                                              bazel-include symlinks point to here. This is used for
                                              linkstamp stuff, etc.
            host/                         <== BuildConfiguration for build host (user's workstation), for
                                              building prerequisite tools, that will be used in later stages
                                              of the build (ex: Protocol Compiler)
        <packages>/                       <== Packages referenced in the build appear as if under a regular workspace
```

The layout of the `*.runfiles` directories is documented in more detail in the places pointed to by RunfilesSupport.

> bazel clean
>
> bazel clean does an rm -rf on the outputPath and the action_cache directory. It also removes the workspace symlinks. The --expunge option will clean the entire outputBase. (bazel clean 对 outputPath 和 action_cache 目录执行 rm -rf。还会移除工作区符号链接。--expunge 选项将清理整个 outputBase。)


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

A package is defined as a directory containing a `BUILD` file named either `BUILD` or `BUILD.bazel`. A package includes all files in its directory, plus all subdirectories beneath it, except those which themselves contain a `BUILD` file. **From this definition, no file or directory may be a part of two different packages.**

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

Whether the input to a rule is a source file or a generated file is in most cases immaterial (不重要的); what matters is only the contents of that file. This fact makes it easy to replace a complex source file with a generated file produced by a rule, such as happens when the burden of manually maintaining a highly structured file becomes too tiresome, and someone writes a program to derive it. No change is required to the consumers of that file. Conversely, a generated file may easily be replaced by a source file with only local changes.

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

More: https://google.github.io/googletest/quickstart-bazel.html

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


More: [What are Bazel's strategies?](https://jmmv.dev/2019/12/bazel-strategies.html)


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

cc_proto_library 从 `.proto` 文件生成 C++ 代码。

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


# [Starlark Language](https://bazel.build/rules/language?hl=en)

For more information about the language, see [Starlark's GitHub repo](https://github.com/bazelbuild/starlark/).

For the authoritative specification of the Starlark syntax and behavior, see the [Starlark Language Specification](https://github.com/bazelbuild/starlark/blob/master/spec.md).


## Syntax (语法)

Starlark's syntax is inspired by `Python3`. This is valid syntax in Starlark:

``` python
def fizz_buzz(n):
  """Print Fizz Buzz numbers from 1 to n."""
  for i in range(1, n + 1):
    s = ""
    if i % 3 == 0:
      s += "Fizz"
    if i % 5 == 0:
      s += "Buzz"
    print(s if s else i)

fizz_buzz(20)
```

Starlark's semantics can differ from Python, but behavioral differences are rare, except for cases where Starlark raises an error. The following Python types are supported:

* [None](https://bazel.build/rules/lib/globals#None)
* [bool](https://bazel.build/rules/lib/bool)
* [dict](https://bazel.build/rules/lib/dict)
* [tuple](https://bazel.build/rules/lib/tuple)
* [function](https://bazel.build/rules/lib/function)
* [int](https://bazel.build/rules/lib/int)
* [list](https://bazel.build/rules/lib/list)
* [string](https://bazel.build/rules/lib/string)

## Mutability (可变性)

Starlark favors immutability. Two mutable data structures are available: [lists](https://bazel.build/rules/lib/list) and [dicts](https://bazel.build/rules/lib/dict). Changes to mutable data-structures, such as appending a value to a list or deleting an entry in a dictionary are valid only for objects created in the current context. After a context finishes, its values become immutable.

This is because Bazel builds use parallel execution. During a build, each `.bzl` file and each `BUILD` file get their own execution context. Each rule is also analyzed in its own context.

## Differences between BUILD and .bzl files

`BUILD` files register targets via making calls to rules. `.bzl` files provide definitions for constants, rules, macros, and functions.

[Native functions](https://bazel.build/reference/be/functions) and [native rules](https://bazel.build/reference/be/overview#language-specific-native-rules) are global symbols in `BUILD` files. `bzl` files need to load them using the [native module](https://bazel.build/rules/lib/toplevel/native).

There are two syntactic restrictions in BUILD files:

1. declaring functions is illegal
2. `*args` and `**kwargs` arguments are not allowed.

## Differences with Python

More: https://bazel.build/rules/language?hl=en#differences_with_python


# [Sharing Variables](https://bazel.build/build/share-variables?hl=en)

## 在同一个 BUILD 文件里共享变量

`BUILD` files are intended to be simple and declarative. They will typically consist of a series of a target declarations. As your code base and your `BUILD` files get larger, you will probably notice some duplication, such as:

```
cc_library(
  name = "foo",
  copts = ["-DVERSION=5"],
  srcs = ["foo.cc"],
)

cc_library(
  name = "bar",
  copts = ["-DVERSION=5"],
  srcs = ["bar.cc"],
  deps = [":foo"],
)
```

Code duplication in `BUILD` files is usually fine. This can make the file more readable: each declaration can be read and understood without any context. This is important, not only for humans, but also for external tools. For example, a tool might be able to read and update `BUILD` files to add missing dependencies. Code refactoring and code reuse might prevent this kind of automated modification.

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


## 在多个不同 BUILD 文件里共享变量

If you need to share a value across multiple `BUILD` files, you have to put it in a `.bzl` file. `.bzl` files contain definitions (variables and functions) that can be used in `BUILD` files.

In `path/to/variables.bzl`, write:

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


# [自定义扩展](https://bazel.build/extending/concepts?hl=en)

Bazel extensions are files ending in `.bzl`. Use a [load statement](https://bazel.build/concepts/build-files#load) to import a symbol from an extension.

Before learning the more advanced concepts, first:

1. Read about the [Starlark language](https://bazel.build/rules/language), used in both the `BUILD` and `.bzl` files.
2. Learn how you can [share variables](https://bazel.build/build/share-variables) between two BUILD files.

## Macros and rules (宏和规则)

A [macro](https://bazel.build/extending/macros) is a function that instantiates rules. It is useful when a `BUILD` file is getting too repetitive or too complex, as it allows you to reuse some code. The function is evaluated as soon as the `BUILD` file is read. After the evaluation of the `BUILD` file, Bazel has little information about macros: if your macro generates a `genrule`, Bazel will behave as if you wrote the `genrule`. As a result, bazel query will only list the generated `genrule`.

宏是用于对规则进行实例化的函数。当 BUILD 文件过于重复或过于复杂时，这会很有用，因为它允许您重复使用一些代码。系统会在读取 BUILD 文件后立即评估该函数。在对 BUILD 文件求值后，Bazel 几乎没有关于宏的信息：如果您的宏生成了 genrule，Bazel 会像您编写了 genrule 一样。因此，bazel query 将仅列出生成的 genrule。

A [rule](https://bazel.build/extending/rules) is more powerful than a macro. It can access Bazel internals and have full control over what is going on. It may for example pass information to other rules.

规则比宏更强大。它可以访问 Bazel 内部，并完全掌控发生的情况。例如，它可以将信息传递给其他规则。

If you want to reuse simple logic, start with a macro. If a macro becomes complex, it is often a good idea to make it a rule. Support for a new language is typically done with a rule. Rules are for advanced users, and most users will never have to write one; they will only load and call existing rules.

如果您想重复使用简单的逻辑，请从宏着手。如果宏变得复杂，通常最好将其设为规则。通常可以使用规则来实现对新语言的支持。规则适用于高级用户，大多数用户永远无需编写规则；他们只需加载并调用现有规则。

## Evaluation model (评估模型)

A build consists of three phases.

* **Loading phase**. First, load and evaluate all extensions and all `BUILD` files that are needed for the build. The execution of the `BUILD` files simply instantiates rules (each time a rule is called, it gets added to a graph). This is where macros are evaluated.

加载阶段。首先，加载并评估 build 所需的所有扩展程序和所有 BUILD 文件。执行 BUILD 文件只是实例化规则（每次调用规则时，系统都会将其添加到图表中）。系统会对宏进行评估。

* **Analysis phase**. The code of the rules is executed (their `implementation` function), and actions are instantiated. An action describes how to generate a set of outputs from a set of inputs, such as "run gcc on hello.c and get hello.o". You must list explicitly which files will be generated before executing the actual commands. In other words, the analysis phase takes the graph generated by the loading phase and generates an action graph.

分析阶段。执行规则的代码（其 implementation 函数），并实例化操作。操作描述了如何根据一组输入（例如，“在 hello.c 和 hello.o 上运行 gcc”）生成一组输出。在执行实际命令之前，您必须明确列出将生成哪些文件。换言之，分析阶段会采用加载阶段生成的图并生成操作图。

* **Execution phase**. Actions are executed, when at least one of their outputs is required. If a file is missing or if a command fails to generate one output, the build fails. Tests are also run during this phase.

执行阶段。当需要至少一项输出时，执行操作。如果文件缺失或某个命令未能生成一项输出，构建就会失败。在此阶段还会运行测试。

Bazel uses parallelism to read, parse and evaluate the `.bzl` files and `BUILD` files. A file is read at most once per build and the result of the evaluation is cached and reused. A file is evaluated only once all its dependencies (`load()` statements) have been resolved. By design, loading a `.bzl` file has no visible side-effect, it only defines values and functions.

Bazel 使用并行处理来读取、解析和评估 .bzl 文件和 BUILD 文件。每个构建最多只能读取一个文件，并且会缓存和重复使用评估结果。文件只有在所有依赖项（load() 语句）都经过解析后才会进行求值。根据设计，加载 .bzl 文件不会产生明显的副作用，而是仅定义值和函数。

Bazel tries to be clever: it uses dependency analysis to know which files must be loaded, which rules must be analyzed, and which actions must be executed. For example, if a rule generates actions that you don't need for the current build, they will not be executed.

Bazel 非常聪明：它使用依赖项分析功能来确定必须加载哪些文件、必须分析哪些规则以及必须执行哪些操作。例如，如果规则生成当前构建不需要的操作，则不会执行这些规则。

## Creating extensions (创建扩展程序)

* [Create your first macro](https://bazel.build/rules/macro-tutorial) in order to reuse some code. Then [learn more about macros](https://bazel.build/extending/macros) and [using them to create "custom verbs"](https://bazel.build/rules/verbs-tutorial).

* [Follow the rules tutorial](https://bazel.build/rules/rules-tutorial) to get started with rules. Next, you can read more about the [rules concepts](https://bazel.build/extending/rules).

The two links below will be very useful when writing your own extensions. Keep them within reach:

* The [API reference](https://bazel.build/rules/lib)

* [Examples](https://github.com/bazelbuild/examples/tree/master/rules)


## Going further

https://bazel.build/extending/concepts?hl=en#going_further


# [Creating a Macro](https://bazel.build/rules/macro-tutorial?hl=en)

Imagine that you need to run a tool as part of your build. For example, you may want to generate or preprocess a source file, or compress a binary. In this tutorial, you are going to create a macro that resizes an image.

假设您需要在构建过程中运行工具。例如，您可能希望生成或预处理源文件，或者压缩二进制文件。在本教程中，您将创建一个用于调整图片大小的宏。

Macros are suitable for simple tasks. If you want to do anything more complicated, for example add support for a new programming language, consider creating a [rule](https://bazel.build/extending/rules). Rules give you more control and flexibility.

宏适用于简单的任务。如果您想执行更复杂的操作，例如添加对新编程语言的支持，请考虑创建规则。规则可让您拥有更大的控制权和更大的灵活性。

The easiest way to create a macro that resizes an image is to use a `genrule`:

```
genrule(
    name = "logo_miniature",
    srcs = ["logo.png"],
    outs = ["small_logo.png"],
    cmd = "convert $< -resize 100x100 $@",
)

cc_binary(
    name = "my_app",
    srcs = ["my_app.cc"],
    data = [":logo_miniature"],
)
```

If you need to resize more images, you may want to reuse the code. To do that, define a function in a separate `.bzl` file, and call the file `miniature.bzl`:

```
def miniature(name, src, size="100x100", **kwargs):
  """Create a miniature of the src image.

  The generated file is prefixed with 'small_'.
  """
  native.genrule(
    name = name,
    srcs = [src],
    outs = ["small_" + src],
    cmd = "convert $< -resize " + size + " $@",
    **kwargs
  )
```

注意事项：

* 按照惯例，宏具有 name 参数，就像规则一样。
* 如需记录宏的行为，请使用 Python 中的 docstring。
* 如需调用 genrule 或任何其他原生规则，请使用 native.。
* 使用 **kwargs 将额外参数转发到底层 genrule（其工作原理与在 Python 中类似）。这很有用，因为用户可以使用 visibility 或 tags 等标准特性。

Now, use the macro from the `BUILD` file:

```
load("//path/to:miniature.bzl", "miniature")

miniature(
    name = "logo_miniature",
    src = "image.png",
)

cc_binary(
    name = "my_app",
    srcs = ["my_app.cc"],
    data = [":logo_miniature"],
)
```

# [Rules Tutorial](https://bazel.build/rules/rules-tutorial?hl=en)

[Starlark](https://github.com/bazelbuild/starlark) is a Python-like configuration language originally developed for use in Bazel and since adopted by other tools. Bazel's `BUILD` and `.bzl` files are written in a dialect of Starlark properly known as the "Build Language", though it is often simply referred to as "Starlark", especially when emphasizing that a feature is expressed in the Build Language as opposed to being a built-in or "native" part of Bazel. Bazel augments the core language with numerous build-related functions such as `glob`, `genrule`, `java_binary`, and so on.

Starlark 是一种类似于 Python 的配置语言，最初是在 Bazel 中使用，之后被其他工具采用。Bazel 的 BUILD 和 .bzl 文件是用 Starlark 的方言（通常称为“build 语言”）编写的，但它通常简称为“Starlark”，尤其是在强调某项功能用 Build 语言表示而不是作为 Bazel 的内置或“原生”部分时。Bazel 使用众多与构建相关的函数（例如 glob、genrule、java_binary 等）来增强核心语言。

## The empty rule (空规则)

To create your first rule, create the file `foo.bzl`:

```
def _foo_binary_impl(ctx):
    pass

foo_binary = rule(
    implementation = _foo_binary_impl,
)
```

When you call the `rule` function, you must define a callback function. The logic will go there, but you can leave the function empty for now. The `ctx` argument provides information about the target.

当您调用 rule 函数时，必须定义一个回调函数。逻辑将转到那里，但现在您可以将函数留空。ctx 参数提供有关目标的信息。

You can load the rule and use it from a `BUILD` file.

Create a `BUILD` file in the same directory:

```
load(":foo.bzl", "foo_binary")

foo_binary(name = "bin")
```

Now, the target can be built:

```
$ bazel build bin
INFO: Analyzed target //:bin (2 packages loaded, 17 targets configured).
INFO: Found 1 target...
Target //:bin up-to-date (nothing to build)
```

Even though the rule does nothing, it already behaves like other rules: it has a mandatory name, it supports common attributes like visibility, testonly, and tags.

虽然该规则不执行任何操作，但它的行为与其他规则一样：它具有强制性名称，因此支持 visibility、testonly 和 tags 等常见特性。

## Evaluation model (评估模型)

Before going further, it's important to understand how the code is evaluated.

Update `foo.bzl` with some print statements:

```
def _foo_binary_impl(ctx):
    print("analyzing", ctx.label)

foo_binary = rule(
    implementation = _foo_binary_impl,
)

print("bzl file evaluation")
```

and `BUILD`:

```
load(":foo.bzl", "foo_binary")

print("BUILD file")
foo_binary(name = "bin1")
foo_binary(name = "bin2")
```

`ctx.label` corresponds to the label of the target being analyzed. The `ctx` object has many useful fields and methods; you can find an exhaustive list in the [API reference](https://bazel.build/rules/lib/ctx).

Query the code:

```
$ bazel query :all
DEBUG: /usr/home/bazel-codelab/foo.bzl:8:1: bzl file evaluation
DEBUG: /usr/home/bazel-codelab/BUILD:2:1: BUILD file
//:bin2
//:bin1
```

Make a few observations:

* "bzl file evaluation" is printed first. Before evaluating the `BUILD` file, Bazel evaluates all the files it loads. If multiple `BUILD` files are loading `foo.bzl`, you would see only one occurrence of "bzl file evaluation" because Bazel caches the result of the evaluation.

* The callback function `_foo_binary_impl` is not called. Bazel query loads `BUILD` files, but doesn't analyze targets.

To analyze the targets, use the [cquery](https://bazel.build/query/cquery) ("configured query") or the `build` command:

```
$ bazel build :all
DEBUG: /usr/home/bazel-codelab/foo.bzl:2:5: analyzing //:bin1
DEBUG: /usr/home/bazel-codelab/foo.bzl:2:5: analyzing //:bin2
INFO: Analyzed 2 targets (0 packages loaded, 0 targets configured).
INFO: Found 2 targets...
```

As you can see, `_foo_binary_impl` is now called twice - once for each target.

Notice that neither "bzl file evaluation" nor "BUILD file" are printed again, because the evaluation of `foo.bzl` is cached after the call to `bazel query`. Bazel only emits `print` statements when they are actually executed.

## Creating a file (创建文件)

To make your rule more useful, update it to generate a file. First, declare the file and give it a name. In this example, create a file with the same name as the target:

为了使规则更实用，请更新规则以生成文件。首先，声明文件并为其命名。在此示例中，请创建一个与目标同名的文件：

```
ctx.actions.declare_file(ctx.label.name)
```

If you run bazel build :all now, you will get an error:

```
The following files have no generating action:
bin2
```

Whenever you declare a file, you have to tell Bazel how to generate it by creating an action. Use [ctx.actions.write](https://bazel.build/rules/lib/actions#write), to create a file with the given content.

```
def _foo_binary_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.write(
        output = out,
        content = "Hello\n",
    )
```

The code is valid, but it won't do anything:

```
$ bazel build bin1
Target //:bin1 up-to-date (nothing to build)
```

The `ctx.actions.write` function registered an action, which taught Bazel how to generate the file. But Bazel won't create the file until it is actually requested. So the last thing to do is tell Bazel that the file is an output of the rule, and not a temporary file used within the rule implementation.

```
def _foo_binary_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.write(
        output = out,
        content = "Hello!\n",
    )
    return [DefaultInfo(files = depset([out]))]
```

Look at the `DefaultInfo` and `depset` functions later. For now, assume that the last line is the way to choose the outputs of a rule.

Now, run Bazel:

```
$ bazel build bin1
INFO: Found 1 target...
Target //:bin1 up-to-date:
  bazel-bin/bin1

$ cat bazel-bin/bin1
Hello!
```

You have successfully generated a file!

## Attributes (属性)

To make the rule more useful, add new attributes using [the attr module](https://bazel.build/rules/lib/attr) and update the rule definition.

Add a string attribute called `username`:

```
foo_binary = rule(
    implementation = _foo_binary_impl,
    attrs = {
        "username": attr.string(),
    },
)
```

Next, set it in the `BUILD` file:

```
foo_binary(
    name = "bin",
    username = "Alice",
)
```

To access the value in the callback function, use `ctx.attr.username`. For example:

```
def _foo_binary_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.write(
        output = out,
        content = "Hello {}!\n".format(ctx.attr.username),
    )
    return [DefaultInfo(files = depset([out]))]
```

Note that you can make the attribute mandatory or set a default value. Look at the documentation of [attr.string](https://bazel.build/rules/lib/attr#string). You may also use other types of attributes, such as [boolean](https://bazel.build/rules/lib/attr#bool) or [list of integers](https://bazel.build/rules/lib/attr#int_list).


## Dependencies (依赖项)

Dependency attributes, such as [attr.label](https://bazel.build/rules/lib/attr#label) and [attr.label_list](https://bazel.build/rules/lib/attr#label_list), declare a dependency from the target that owns the attribute to the target whose label appears in the attribute's value. This kind of attribute forms the basis of the target graph.

依赖项属性（例如 attr.label 和 attr.label_list）会声明从拥有该属性的目标到目标在标签值中显示目标的依赖项。这种特性构成了目标图的基础。

In the `BUILD` file, the target label appears as a string object, such as `//pkg:name`. In the implementation function, the target will be accessible as a [Target](https://bazel.build/rules/lib/Target) object. For example, view the files returned by the target using [Target.files](https://bazel.build/rules/lib/Target#modules.Target.files).


## Create a file with a template (使用模板创建文件)

You can create a rule that generates a `.cc` file based on a **template**. Also, you can use `ctx.actions.write` to output a string constructed in the rule implementation function, but this has two problems. First, as the template gets bigger, it becomes more memory efficient to put it in a separate file and avoid constructing large strings during the analysis phase. Second, using a separate file is more convenient for the user. Instead, use [ctx.actions.expand_template](https://bazel.build/rules/lib/actions#expand_template), which performs substitutions on a template file.

您可以创建基于模板生成 `.cc` 文件的规则。此外，您还可以使用 `ctx.actions.write` 输出在规则实现函数中构建的字符串，但存在两个问题。首先，随着模板越来越庞大，将内存放在一个单独的文件中，从而避免在分析阶段构建大型字符串，从而节省更多内存。其次，使用单独的文件更方便用户。而是改用 `ctx.actions.expand_template`，以替换模板文件。

Create a **template** attribute to declare a dependency on the template file:

```
def _hello_world_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name + ".cc")
    ctx.actions.expand_template(
        output = out,
        template = ctx.file.template,
        substitutions = {"{NAME}": ctx.attr.username},
    )
    return [DefaultInfo(files = depset([out]))]

hello_world = rule(
    implementation = _hello_world_impl,
    attrs = {
        "username": attr.string(default = "unknown person"),
        "template": attr.label(
            allow_single_file = [".cc.tpl"],
            mandatory = True,
        ),
    },
)
```

Users can use the rule like this:

```
hello_world(
    name = "hello",
    username = "Alice",
    template = "file.cc.tpl",
)

cc_binary(
    name = "hello_bin",
    srcs = [":hello"],
)
```

If you don't want to expose the template to the end-user and always use the same one, you can set a default value and make the attribute private:

```
    "_template": attr.label(
        allow_single_file = True,
        default = "file.cc.tpl",
    ),
```

Attributes that start with an **underscore** are **private** and cannot be set in a `BUILD` file. The template is now an implicit dependency: Every `hello_world` target has a dependency on this file. Don't forget to make this file visible to other packages by updating the `BUILD` file and using [exports_files](https://bazel.build/reference/be/functions#exports_files):

```
exports_files(["file.cc.tpl"])
```

## Going further

* Take a look at the [reference documentation for rules](https://bazel.build/extending/rules#contents).
* Get familiar with [depsets](https://bazel.build/extending/depsets).
* Check out the [examples repository](https://github.com/bazelbuild/examples/tree/master/rules) which includes additional examples of rules.





# 最佳实践

## 修改 bazel 输出目录使用 SSD 磁盘存储

``` bash
# .bazelrc
startup --output_user_root=/data_ssd/home/gerryyang/bazel

# 命令行
bazel --output_user_root=/data1/home/bazel_build/.bazel build //...
```

## 在二进制中注入版本信息

### 方案1：自定义规则

在工程根目录的 BUILD 中：

```python
load("//:version_info.bzl", "generate_version_info")

# 自定义规则
generate_version_info(
    name = "version_info",
    visibility = ["//visibility:public"],
)

config_setting(
    name = "enable_version_info",
    values = {"define": "enable_version_info=true"},
)
```

version_info.bzl 内容如下：

``` python
"""
测试命令：
ACTION_ENV="--action_env=COMPILER_NAME=clang --action_env=OS=$(uname -r)"
bazel build //:version_info $ACTION_ENV
"""

def _generate_version_info_impl(ctx):
    template_path = ctx.file._template.path
    #print("Template path:", template_path)

    # create version_info.h
    out = ctx.actions.declare_file(ctx.label.name + ".h")

    ctx.actions.expand_template(
        output = out,
        template = ctx.file._template,

        # 通过 rule 参数
        #substitutions = {"%COMPILER_NAME%": ctx.attr.compiler},

        # 通过环境变量 --action_env
        substitutions = {
            "%HOST%": ctx.configuration.default_shell_env["HOST"],
            "%COMPILE_PATH%": ctx.configuration.default_shell_env["COMPILE_PATH"],
            "%COMPILER_NAME%": ctx.configuration.default_shell_env["COMPILER_NAME"],
            "%COMPILER_VER%": ctx.configuration.default_shell_env["COMPILER_VER"],
            "%STL%": ctx.configuration.default_shell_env["STL"],
            "%OS%": ctx.configuration.default_shell_env["OS"],
            "%GIT%": ctx.configuration.default_shell_env["GIT"],
            "%BRANCH%": ctx.configuration.default_shell_env["BRANCH"],
            "%COMMIT%": ctx.configuration.default_shell_env["COMMIT"],
        },

    )
    # 创建一个包含生成的头文件的 CcInfo 提供器
    cc_info = CcInfo(compilation_context = cc_common.create_compilation_context(headers = depset([out])))
    return [DefaultInfo(files = depset([out])), cc_info]


generate_version_info = rule(
    implementation = _generate_version_info_impl,
    attrs = {
        "_compiler": attr.string(),
        "_version": attr.string(),
        "_template": attr.label(
            allow_single_file = True,
            default = "//build/template:version_info.h.template",
        ),
    },
)
```

构建使用时：

``` bash
function SetActionEnvInfo()
{
    # 获取环境信息
    ACTION_ENV="--action_env=HOST=$(/sbin/ifconfig | grep -A1 '^eth' | grep inet | awk '{print $2}' | grep -v '^172\\.' | grep -v '^127\\.') \
--action_env=COMPILE_PATH=$(pwd -P) \
--action_env=COMPILER_NAME=clang \
--action_env=COMPILER_VER=$(clang --version | grep version | sed "s/.*version \([0-9\.]*\).*/\1/") \
--action_env=STL=$(ls /lib64/libstdc++.so.6 -l | awk '{print $NF}') \
--action_env=OS=$(uname -r) \
--action_env=GIT=$(git remote -v | grep git | head -1 | awk '{print $2}') \
--action_env=BRANCH=$(git rev-parse --abbrev-ref HEAD) \
--action_env=COMMIT=$(git rev-parse --short HEAD)"
}

function SetVersionInfoTarget()
{
    EnableVersionInfoDefault=$(GetProjectOption $ProjectCfg VersionInfo.Enable)
    EnableVersionInfo="${EnableVersionInfoVar:=${EnableVersionInfoDefault}}"

    if [[ $EnableVersionInfo == "1" ]]; then

        SetActionEnvInfo
        ALL_TARGET+=" //:version_info"

        CONDITION_OPTION+=" --define enable_version_info=true"
        COMMON_OPTION+=" --copt=-D_ENABLE_VERSION_INFO_"

        #USE_RBS="" # 如果注入二进制版本信息带有时间戳会导致远程缓存失效，建议不使用 RC
    fi
}
```

在构建依赖的 BUILD 中根据条件编译选择是否依赖 version_info 目标：

```python
select({
          "//:enable_version_info": ["//:version_info"], # 版本信息
          "//conditions:default": [],
})
```

### 方案2：通过 genrule

https://github.com/envoyproxy/envoy/blob/release/v1.22/source/common/version/BUILD

示例：

``` python
genrule(
    name = "generate_version_number",
    srcs = ["//:VERSION.txt"],
    outs = ["version_number.h"],
    cmd = """echo "#define BUILD_VERSION_NUMBER \\"$$(cat $<)\\"" >$@""",
    visibility = ["//visibility:private"],
)
```



## [.bazelrc flags you should enable](https://blog.aspect.dev/bazelrc-flags)


* `build --incompatible_strict_action_env`: don't let environment variables like $PATH sneak into the build, which can cause massive cache misses when they change.

* `build --modify_execution_info=PackageTar=+no-remote`: Some actions are always IO-intensive but require little compute. It's wasteful to put the output in the remote cache, it just saturates the network and fills the cache storage causing earlier evictions. It's also not worth sending them for remote execution. For actions like PackageTar it's faster to just re-run the work locally every time. You'll have to look at an execution log to figure out which action mnemonics you care about.
* `--bes_upload_mode=fully_async`: Don't make the user wait for uploads, instead allow the bazel command to complete and exit.





refer:

* https://docs.aspect.build/guides/bazelrc/
* https://github.com/aspect-build/bazel-examples/blob/main/bazelrc/.bazelrc



## [Fixing Bazel out-of-memory problems](https://blog.aspect.dev/bazel-oom)

There are two potential problems:

* The Bazel server runs in a JVM, and it internally tries to allocate more objects than the max heap size its allowed.
* Bazel spawns subprocesses (called "actions", including test actions) and they collectively exhaust the memory in the machine or VM that Bazel runs in.

```
$ bazel info | grep heap
committed-heap-size: 826MB
max-heap-size: 32178MB
used-heap-size: 193MB
```


## 生成 compile_commands.json 文件

> The [JSON compilation database](http://clang.llvm.org/docs/JSONCompilationDatabase.html) is used in the clang project to provide information on how a single compilation unit is processed. With this, it is easy to re-run the compilation with alternate programs.

### 生成方案

#### bazel-compile-commands-extractor

clangd 官方建议的方案：https://github.com/hedronvision/bazel-compile-commands-extractor

#### bazel-compilation-database

已停止维护，经测试使用发现功能存在问题，会生成多条相同的头文件且其中一条头文件使用的 include 路径不对，导致 vscode-clangd 扩展符号代码跳转功能异常，不建议使用。https://github.com/grailbio/bazel-compilation-database

#### Bear

Bear is a tool that generates a compilation database for clang tooling.

Some build system natively supports the generation of JSON compilation database. For projects which does not use such build tool, Bear generates the JSON file during the build process.

> Troubleshooting

* https://github.com/rizsotto/Bear/wiki/Troubleshooting
* https://github.com/rizsotto/Bear/issues

> How it works?

To build the compilation database, Bear split this work into two major steps:

1. It execute the build and intercepts the executed commands (most likely the compiler calls).
2. It read the command execution log, and build the final output (deduce the semantic of the commands).

> The output is empty

The most common cause for empty outputs is that the build command did not execute any commands. The reason for that could be, because incremental builds not running the compilers if everything is up-to-date. Remember, Bear does not understand the build file (eg.: makefile), but intercepts the executed commands.

> TODO:

参考[gRPC call failed: Socket closed #636](https://github.com/rizsotto/Bear/issues/636)代码示例，使用 bear 进行测试出现：“recognition failed: No tools recognize this execution.” 错误。





### 标准说明 [JSON Compilation Database Format Specification](https://clang.llvm.org/docs/JSONCompilationDatabase.html) (编译数据库)

Tools based on the **C++ Abstract Syntax Tree** need full information how to parse **a translation unit**. Usually this information is **implicitly** available in the **build system**, but running tools as part of the build system is not necessarily the best solution:

* Build systems are inherently change driven, so running multiple tools over the same code base without changing the code does not fit into the architecture of many build systems.

* Figuring out whether things have changed is often an IO bound process; this makes it hard to build low latency end user tools based on the build system.

* Build systems are inherently sequential in the build graph, for example due to generated source code. While tools that run independently of the build still need the generated source code to exist, running tools multiple times over unchanging source does not require serialization of the runs according to the build dependency graph.


JSON 编译数据库格式规范（JSON Compilation Database Format）是一种用于记录 C/C++ 项目编译信息的标准化格式，其核心目的是让代码分析工具（如静态分析器、代码索引工具等）能够在不依赖原始构建系统的前提下，独立获取并复现单个编译单元的完整编译信息。

具体优势如下：

* 独立于构建系统运行
  + 工具直接读取 JSON 文件中的编译命令，无需与构建系统交互，避免了变更驱动机制的限制。
  + 示例：clang-tidy 静态分析工具可通过该文件直接获取编译参数，无需触发 CMake 的构建流程。

* 减少 IO 开销
  + 编译信息被持久化存储，工具无需反复检查文件系统状态，显著降低延迟。
  + 示例：IDE 可以快速加载 JSON 文件中的编译命令，立即提供代码导航功能。

* 支持并行化处理
  + 工具可并行处理多个编译单元，无需遵循构建系统的串行依赖。
  + 示例：代码索引工具可同时分析多个 .cpp 文件，而无需等待生成代码的任务完成。


### Supported Systems

* Currently `CMake` (since 2.8.5) supports generation of compilation databases for Unix Makefile builds (Ninja builds in the works) with the option `CMAKE_EXPORT_COMPILE_COMMANDS`.

* `Bazel` can export a compilation database via [this extractor extension](https://github.com/hedronvision/bazel-compile-commands-extractor).

* Clang’s tooling interface supports reading compilation databases; see the [LibTooling documentation](https://clang.llvm.org/docs/LibTooling.html). libclang and its python bindings also support this (since clang 3.2); see [CXCompilationDatabase.h](https://clang.llvm.org/doxygen/group__COMPILATIONDB.html).


### Format

A compilation database is a `JSON` file, which consist of an array of “command objects”, where each command object specifies one way a translation unit is compiled in the project.

Each command object contains the translation unit’s main file, the working directory of the compile run and the actual compile command.

Example:

``` json
[
  { "directory": "/home/user/llvm/build",
    "arguments": ["/usr/bin/clang++", "-Irelative", "-DSOMEDEF=With spaces, quotes and \\-es.", "-c", "-o", "file.o", "file.cc"],
    "file": "file.cc" },

  { "directory": "/home/user/llvm/build",
    "command": "/usr/bin/clang++ -Irelative -DSOMEDEF=\"With spaces, quotes and \\-es.\" -c -o file.o file.cc",
    "file": "file2.cc" },

  ...
]
```

The contracts for each field in the command object are:

* **directory**: The working directory of the compilation. All paths specified in the command or file fields must be either absolute or relative to this directory.

* **file**: The main translation unit source processed by this compilation step. This is used by tools as the key into the compilation database. There can be multiple command objects for the same file, for example if the same source file is compiled with different configurations.

* **arguments**: The compile command argv as list of strings. This should run the compilation step for the translation unit file. `arguments[0]` should be the executable name, such as clang++. Arguments should not be escaped, but ready to pass to `execvp()`.

* **command**: The compile command as a single shell-escaped string. Arguments may be shell quoted and escaped following platform conventions, with ‘"’ and ‘\’ being the only special characters. Shell expansion is not supported.

> Either **arguments** or **command** is required. **arguments** is preferred, as shell (un)escaping is a possible source of errors.

* **output**: The name of the output created by this compilation step. **This field is optional**. It can be used to distinguish different processing modes of the same input file.


### Build System Integration

The convention is to name the file **compile_commands.json** and **put it at the top of the build directory**. Clang tools are pointed to the top of the build directory to detect the file and use the compilation database to parse C++ code in the source tree.


### Alternatives

For simple projects, Clang tools also recognize a **compile_flags.txt** file. This should contain one argument per line. The same flags will be used to compile any file.

Example:

```
-xc++
-I
libwidget/include/
```

Here `-I libwidget/include` is two arguments, and so becomes two lines. Paths are relative to the directory containing **compile_flags.txt**.



## 对 cc_binary 禁止使用远程缓存

bazel 是一个构建和测试工具，用于管理项目的源代码、依赖和构建过程。在 bazel 的构建文件（如 BUILD 或 BUILD.bazel 文件）中，可以为目标（如 cc_binary、cc_library、py_binary 等）指定一组标签（tags），用于对目标进行分类和筛选。

tags 是一个字符串列表，其中的每个元素都是一个标签。no-cache 是一个预定义的标签，它的作用是告诉 bazel 不要在远程缓存中存储或查找该目标的构建结果。当你为一个目标添加 `tags = ["no-cache"]` 时，bazel 将在每次构建时重新构建该目标，而不是尝试从远程缓存中获取已构建的结果。这对于那些不适合缓存或具有不确定性的构建目标（如生成时间戳或随机数的目标）特别有用。

以下是一个简单的 BUILD.bazel 文件示例，其中定义了一个带有 no-cache 标签的 cc_binary 目标：

```python
cc_binary(
    name = "example_binary",
    srcs = ["example.cc"],
    tags = ["no-cache"],
)
```

在这个例子中，每次使用 `bazel build //:example_binary` 构建 example_binary 目标时，bazel 都会重新构建它，而不是尝试从远程缓存中查找已构建的结果。

> 建议改成 `--modify_execution_info=^(CppLink|CppArchive)=+no-remote-cache` 的方式，对 CppLink 和 CppArchive 禁止上传 rc。

``` bash
# 控制只有 CppCompile 上传 rc
build --modify_execution_info=^(CppLink|CppArchive)=+no-remote-cache
```


## 如何对使用远程缓存文件上传进行优化

bazel 使用 remote cache 远程缓存编译后，编译完成的目标文件需要上传到 remote cache 服务，对于大文件可能会比较慢，如何提高上传的效率

使用 Bazel 远程缓存时，确实可能会遇到大文件上传速度较慢的问题。以下是一些建议，可以帮助提高上传效率：

1. 使用高速网络连接：确保构建机器和远程缓存服务之间的网络连接具有高带宽和低延迟。如果可能的话，将构建机器和远程缓存服务部署在相同的局域网或云服务提供商内。
2. 选择合适的远程缓存服务：选择一款性能良好的远程缓存服务，如 Google Cloud Storage、Amazon S3 或其他支持 HTTP/REST 或 gRPC 协议的缓存服务。这些服务通常具有大规模数据传输的优化和高可用性。
3. 并发上传：Bazel 支持并发上传。可以通过设置 --remote_max_connections 选项来调整并发上传的连接数，如 `--remote_max_connections=50`。
4. 限制需要缓存的目标：对于一些不适合缓存的目标（如生成时间戳或随机数的目标），可以使用 `tags = ["no-cache"]` 来避免将它们上传到远程缓存。这有助于减少不必要的上传开销。
5. 优化构建规则：尽量减小构建产物的大小，例如，避免将不必要的大型文件包含在目标中。这将减少上传到远程缓存所需的时间。

结合这些建议，可以通过调整 Bazel 配置和优化构建规则来提高上传到远程缓存的效率。



## [How to avoid 'No space left' on Bazel build?](https://stackoverflow.com/questions/54986853/how-to-avoid-no-space-left-on-bazel-build)

You can use the startup option [--output_base](https://docs.bazel.build/versions/master/command-line-reference.html#flag--output_base) to point to a location where there's more available storage. This will tell Bazel where to write all its outputs.

```
$ bazel --output_base=/path/to/more/space build ...
```

To avoid specifying this for every command, add it to your project `<project>/.bazelrc` or user `~/.bazelrc`:

```
startup --output_base=/path/to/more/space
```


## 条件编译

[config_setting](https://bazel.build/reference/be/general?hl=zh-cn#config_setting) 和 [select](https://bazel.build/reference/be/functions?hl=zh-cn#select) 可以用于在 Bazel 构建中实现条件编译，根据不同的配置选项选择性地包含不同的文件或编译选项。下面是一个使用 config_setting 和 select 的简单示例：

* 在项目根目录下创建一个 BUILD 文件（如果尚未创建）。

* 添加一个 config_setting 规则，用于定义一个名为 use_custom_lib 的配置标志：

```
config_setting(
    name = "use_custom_lib",
    values = {"define": "custom_lib=true"},
)
```

这将创建一个名为 use_custom_lib 的配置设置，它在 `--define custom_lib=true` 传递给 Bazel 时为真。

* 在 BUILD 文件中添加一个 cc_library 规则，并使用 select 语句根据 use_custom_lib 的值选择不同的源文件：

```
cc_library(
    name = "conditional_lib",
    srcs = select({
        ":use_custom_lib": ["custom_lib.cc"],
        "//conditions:default": ["default_lib.cc"],
    }),
    hdrs = ["lib.h"],
)
```

在这个示例中，当 custom_lib 定义为 true 时，conditional_lib 会使用 custom_lib.cc 作为源文件，否则会使用 default_lib.cc。


* 构建目标

```
# 使用默认库构建
bazel build //:conditional_lib

# 使用自定义库构建
bazel build //:conditional_lib --define custom_lib=true
```

这个简单的示例展示了如何使用 config_setting 和 select 在 Bazel 构建中实现条件编译。你可以根据项目需求扩展这个示例，例如在不同的平台上使用不同的源文件或依赖项。


## 增加 bazel 并发度，提升构建效率 (--jobs)

在执行 bazel 命令时设置 `--jobs` 参数，不设置时 bazel 默认为当前服务器的 CPU 核数，即 `HOST_CPUS`。

下图为某测试项目在不同并发度 `HOST_CPUS * {1, 2, 3, 4}` 下的构建耗时情况：

![bazel_build7](/assets/images/202306/bazel_build7.png)

注意：并发度并不是越高越好，要基于服务器配置和项目实际情况进行调整，确定最佳值。



## [How do I install a project built with bazel?](https://stackoverflow.com/questions/43549923/how-do-i-install-a-project-built-with-bazel)

* https://github.com/bazelbuild/bazel-skylib (可用)
* https://github.com/google/bazel_rules_install
* https://github.com/aspect-build/bazel-lib/blob/main/docs/copy_directory.md



## [.bazelignore 忽略配置](https://bazel.build/run/bazelrc?hl=zh-cn#bazelignore)

在 `WORKSPACE` 目录下通过 `.bazelignore` 配置指定忽略的 BUILD。

可以在工作区中指定想让 Bazel 忽略的目录，如使用其他构建系统的相关项目。将名为 `.bazelignore` 的文件放在工作区的根目录，然后添加您希望 Bazel 忽略的目录（每行一个）。相应条目是相对于工作区根目录而言的。



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



# 构建性能

## [提取构建性能指标](https://bazel.build/advanced/performance/build-performance-metrics?hl=zh-cn)


提高构建性能的重要步骤是了解资源的使用位置。[细分构建性能](https://bazel.build/configure/build-performance-breakdown?hl=zh-cn)一文说明了如何使用这些指标检测和修复构建性能问题。

从 Bazel 构建中提取指标的主要方法有以下几种：

### Build Event Protocol (BEP)

Bazel outputs a variety of protocol buffers `build_event_stream.proto` through the [Build Event Protocol (BEP)](https://bazel.build/remote/bep), which can be aggregated by a backend specified by you. Depending on your use cases, you might decide to aggregate the metrics in various ways, but here we will go over some concepts and proto fields that would be useful in general to consider.


### Bazel’s query / cquery / aquery commands

Bazel provides 3 different query modes ([query](https://bazel.build/query/quickstart), [cquery](https://bazel.build/query/cquery) and [aquery](https://bazel.build/query/aquery)) that allow users to query the target graph（目标图）, configured target graph（配置的目标图） and action graph（操作图） respectively. The query language provides a [suite of functions](https://bazel.build/query/language#functions) usable across the different query modes, that allows you to customize your queries according to your needs.


### JSON Trace Profiles

For every build-like Bazel invocation, Bazel writes a trace profile in JSON format. The [JSON trace profile](https://bazel.build/advanced/performance/json-trace-profile) can be very useful to quickly understand what Bazel spent time on during the invocation.


### Execution Log

The [execution log](https://bazel.build/remote/cache-remote) can help you to troubleshoot and fix missing remote cache hits due to machine and environment differences or non-deterministic actions. If you pass the flag `--experimental_execution_log_spawn_metrics` (available from Bazel 5.2) it will also contain detailed spawn metrics, both for locally and remotely executed actions. You can use these metrics for example to make comparisons between local and remote machine performance or to find out which part of the spawn execution is consistently slower than expected (for example due to queuing).


### Execution Graph Log

While the JSON trace profile contains the critical path information, sometimes you need additional information on the dependency graph of the executed actions. Starting with Bazel 6.0, you can pass the flags `--experimental_execution_graph_log` and `--experimental_execution_graph_log_dep_type=all` to write out a log about the executed actions and their inter-dependencies.

This information can be used to understand the drag that is added by a node on the critical path. The drag is the amount of time that can potentially be saved by removing a particular node from the execution graph.

The data helps you predict the impact of changes to the build and action graph before you actually do them.


### Benchmarking with bazel-bench

[Bazel bench](https://github.com/bazelbuild/bazel-bench) is a benchmarking tool for Git projects to benchmark build performance in the following cases:

* **Project benchmark**: Benchmarking two git commits against each other at a single Bazel version. Used to detect regressions in your build (often through the addition of dependencies).

* **Bazel benchmark**: Benchmarking two versions of Bazel against each other at a single git commit. Used to detect regressions within Bazel itself (if you happen to maintain / fork Bazel).

Benchmarks monitor wall time, CPU time and system time and Bazel’s retained heap size.

It is also recommended to run Bazel bench on dedicated, physical machines that are not running other processes so as to reduce sources of variability.


## [细分构建性能](https://bazel.build/advanced/performance/build-performance-breakdown?hl=zh-cn)

Bazel is complex and does a lot of different things over the course of a build, some of which can have an impact on build performance. This page attempts to map some of these Bazel concepts to their implications on build performance. While not extensive, we have included some examples of how to detect build performance issues through [extracting metrics](https://bazel.build/configure/build-performance-metrics) and what you can do to fix them. With this, we hope you can apply these concepts when investigating build performance regressions.


### Clean vs Incremental builds（干净构建与增量构建）

A clean build is one that builds everything from scratch, while an incremental build reuses some already completed work.（干净构建是指从头开始构建所有内容，而增量构建会重复使用一些已完成的工作。）

We suggest looking at clean and incremental builds separately, especially when you are collecting / aggregating metrics that are dependent on the state of Bazel’s caches (for example [build request size metrics](https://bazel.build/advanced/performance/build-performance-breakdown?hl=en#deterministic-build-metrics-as-a-proxy-for-build-performance) ). **They also represent two different user experiences. As compared to starting a clean build from scratch (which takes longer due to a cold cache), incremental builds happen far more frequently as developers iterate on code (typically faster since the cache is usually already warm)**.（与从头开始启动整洁构建（由于冷缓存而需要更长的时间）相比，随着开发者迭代代码（增量通常更快，因为缓存通常已经很热），增量构建的频率会高得多。）

### Deterministic build metrics as a proxy for build performance（确定性构建指标，以提升构建性能）

Measuring build performance can be difficult due to the non-deterministic nature of certain metrics (for example Bazel’s CPU time or queue times on a remote cluster). As such, it can be useful to use deterministic metrics as a proxy for the amount of work done by Bazel, which in turn affects its performance.（由于某些指标（例如 Bazel 的 CPU 时间或远程集群上的队列时间）具有不确定性，因此衡量构建性能可能很困难。因此，使用确定性指标作为 Bazel 完成的工作量的代理会很有帮助，后者会影响其性能。）

The size of a build request can have a significant implication on build performance. A larger build could represent more work in analyzing and constructing the build graphs. Organic growth of builds comes naturally with development, as more dependencies are added/created, and thus grow in complexity and become more expensive to build.（构建请求的大小可能会对构建性能产生重大影响。大型 build 可能意味着分析和构建 build 图方面的工作量更大。随着 build 的自然增多，自然而然地会伴随着开发，因为添加/创建的依赖项会更多，因此复杂性也会增加，构建成本也会更高。）

We can slice this problem into the various build phases, and use the following metrics as proxy metrics for work done at each phase:（我们可以将此问题划分到各个构建阶段，并使用以下指标作为每个阶段完成的指标：）

* `PackageMetrics.packages_loaded`: the number of packages successfully loaded. A regression here represents more work that needs to be done to read and parse each additional BUILD file in the loading phase.
  + This is often due to the addition of dependencies and having to load their transitive closure.
  + Use [query](https://bazel.build/query/quickstart) / [cquery](https://bazel.build/query/cquery) to find where new dependencies might have been added.

* `TargetMetrics.targets_configured`: representing the number of targets and aspects configured in the build. A regression represents more work in constructing and traversing the configured target graph.
  + This is often due to the addition of dependencies and having to construct the graph of their transitive closure.
  + Use [cquery](https://bazel.build/query/cquery) to find where new dependencies might have been added.

* `ActionSummary.actions_created`: represents the actions created in the build, and a regression represents more work in constructing the action graph. Note that this also includes unused actions that might not have been executed.
  + Use [aquery](https://bazel.build/query/aquery) for debugging regressions; we suggest starting with [--output=summary](https://bazel.build/reference/command-line-reference#flag--output) before further drilling down with [--skyframe_state](https://bazel.build/reference/command-line-reference#flag--skyframe_state).

* `ActionSummary.actions_executed`: the number of actions executed, a regression directly represents more work in executing these actions.
  + The [BEP](https://bazel.build/remote/bep) writes out the action statistics `ActionData` that shows the most executed action types. By default, it collects the top 20 action types, but you can pass in the `--experimental_record_metrics_for_all_mnemonics` to collect this data for all action types that were executed.
  + This should help you to figure out what kind of actions were executed (additionally).

* `BuildGraphSummary.outputArtifactCount`: the number of artifacts created by the executed actions.
  + If the number of actions executed did not increase, then it is likely that a rule implementation was changed.


These metrics are all affected by the state of the local cache, hence you will want to ensure that the builds you extract these metrics from are clean builds.（这些指标都受本地缓存状态的影响，因此需要确保从中提取指标的 build 是干净 build）

We have noted that a regression in any of these metrics can be accompanied by regressions in wall time, cpu time and memory usage.


### Usage of local resources（使用本地资源）

Bazel consumes a variety of resources on your local machine (both for analyzing the build graph and driving the execution, and for running local actions), this can affect the performance / availability of your machine in performing the build, and also other tasks.（Bazel 会消耗本地机器上的各种资源（包括用于分析构建图和驱动执行情况以及运行本地操作），这可能会影响机器在执行构建时的性能 / 可用性以及其他任务。）


#### Time spent（所用时间）

Perhaps the metrics most susceptible to noise (and can vary greatly from build to build) is time; in particular - wall time, cpu time and system time. You can use [bazel-bench](https://github.com/bazelbuild/bazel-bench) to get a benchmark for these metrics, and with a sufficient number of `--runs`, you can increase the statistical significance of your measurement.（或许，最容易受噪声影响的指标（可能因构建而异）是时间；尤其是实际用时、CPU 时间和系统时间。可以使用 bazel-bench 来获取这些指标的基准，只要有足够的 --runs，就可以提高衡量的统计显著性。）

* **Wall time** is the real world time elapsed.
  + If only wall time regresses, we suggest collecting a JSON trace profile and looking for differences. Otherwise, it would likely be more efficient to investigate other regressed metrics as they could have affected the wall time.

* **CPU time** is the time spent by the CPU executing user code.
  + If the CPU time regresses across two project commits, we suggest collecting a Starlark CPU profile. You should probably also use --nobuild to restrict the build to the analysis phase since that is where most of the CPU heavy work is done.

* **System time** is the time spent by the CPU in the kernel.
  + If system time regresses, it is mostly correlated with I/O when Bazel reads files from your file system.

#### System-wide load profiling（系统级负载分析）

Using the `--experimental_collect_load_average_in_profiler` flag introduced in Bazel 6.0, the [JSON trace profiler](https://bazel.build/advanced/performance/json-trace-profile) collects the system load average during the invocation.

A high load during a Bazel invocation can be an indication that Bazel schedules too many local actions in parallel for your machine. You might want to look into adjusting `--local_cpu_resources` and `--local_ram_resources`, especially in container environments (at least until `#16512` is merged).


#### Monitoring Bazel memory usage（监控 Bazel 内存用量）

There are two main sources to get Bazel’s memory usage, Bazel `info` and the [BEP](https://bazel.build/remote/bep).

* `bazel info used-heap-size-after-gc`: The amount of used memory in bytes after a call to `System.gc()`.
  + [Bazel bench](https://github.com/bazelbuild/bazel-bench) provides benchmarks for this metric as well.
  + Additionally, there are `peak-heap-size`, `max-heap-size`, `used-heap-size` and `committed-heap-size` (see [documentation](https://bazel.build/docs/user-manual#configuration-independent-data)), but are less relevant.

* BEP’s `MemoryMetrics.peak_post_gc_heap_size`: Size of the peak JVM heap size in bytes post GC (requires setting [--memory_profile](https://bazel.build/reference/command-line-reference#flag--memory_profile) that attempts to force a full GC).

A regression in memory usage is usually a result of a regression in [build request size metrics](https://bazel.build/advanced/performance/build-performance-breakdown?hl=en#deterministic_build_metrics_as_a_proxy_for_build_performance), which are often due to addition of dependencies or a change in the rule implementation.（内存用量的下降通常是由构建请求大小指标回归引起的，这通常是由于添加了依赖项或规则实现发生了变化）

To analyze Bazel’s memory footprint on a more granular level, we recommend using the [built-in memory profiler](https://bazel.build/rules/performance#memory-profiling) for rules.（如需更精细地分析 Bazel 的内存占用情况，我们建议您使用内置内存分析器进行规则分析）

#### Memory profiling of persistent workers（持久性工作器的内存性能分析）

While [persistent workers](https://bazel.build/remote/persistent) can help to speed up builds significantly (especially for interpreted languages) their memory footprint can be problematic. Bazel collects metrics on its workers, in particular, the `WorkerMetrics.WorkerStats.worker_memory_in_kb` field tells how much memory workers use (by mnemonic).（虽然持久性工作器有助于显著加快构建速度（尤其是对于解释型语言），但其内存占用量可能有问题。Bazel 会收集有关其工作器的指标，尤其是 WorkerMetrics.WorkerStats.worker_memory_in_kb 字段，这表明工作器使用的内存量）

### Monitoring network traffic for remote builds（监控远程构建的网络流量）

In remote execution, Bazel downloads artifacts that were built as a result of executing actions. As such, your network bandwidth can affect the performance of your build.（在远程执行中，Bazel 会下载因执行操作而构建的工件。因此，网络带宽可能会影响构建的性能）

If you are using remote execution for your builds, you might want to consider monitoring the network traffic during the invocation using the `NetworkMetrics.SystemNetworkStats` proto from the [BEP](https://bazel.build/remote/bep) (requires passing `--experimental_collect_system_network_usage`).

Furthermore, [JSON trace profiles](https://bazel.build/advanced/performance/json-trace-profile) allow you to view system-wide network usage throughout the course of the build by passing the `--experimental_collect_system_network_usage` flag (new in Bazel 6.0).

A high but rather flat network usage when using remote execution might indicate that network is the bottleneck in your build; if you are not using it already, consider turning on Build without the Bytes by passing [--remote_download_minimal](https://bazel.build/reference/command-line-reference#flag--remote_download_minimal). This will speed up your builds by avoiding the download of unnecessary intermediate artifacts.

Another option is to configure a local [disk cache](https://bazel.build/reference/command-line-reference#flag--disk_cache) to save on download bandwidth.

## [JSON 跟踪配置文件](https://bazel.build/advanced/performance/json-trace-profile?hl=zh-cn)

The JSON trace profile can be very useful to quickly understand what Bazel spent time on during the invocation.（JSON 跟踪记录配置文件非常有助于快速了解 Bazel 在调用期间花费的时间）

By default, for all build-like commands and query Bazel writes such a profile to `command.profile.gz`. You can configure whether a profile is written with the [--generate_json_trace_profile](https://bazel.build/reference/command-line-reference?hl=zh-cn#flag--generate_json_trace_profile) flag, and the location it is written to with the [--profile](https://bazel.build/docs/user-manual?hl=zh-cn#profile) flag. Locations ending with `.gz` are compressed with GZIP. Use the flag [--experimental_announce_profile_path](https://bazel.build/reference/command-line-reference?hl=zh-cn#flag--experimental_announce_profile_path) to print the path to this file to the log.


### Tools

You can load this profile into `chrome://tracing` or analyze and post-process it with other tools.

#### chrome://tracing

To visualize the profile, open `chrome://tracing` in a Chrome browser tab, click "Load" and pick the (potentially compressed) profile file. For more detailed results, click the boxes in the lower left corner.


#### bazel analyze-profile

The Bazel subcommand [analyze-profile](https://bazel.build/docs/user-manual#analyze-profile) consumes a profile format and prints cumulative statistics for different task types for each build phase and an analysis of the critical path.

For example, the commands

```
$ bazel build --profile=/tmp/profile.gz //path/to:target
...
$ bazel analyze-profile /tmp/profile.gz
```

may yield output of this form:

```
INFO: Profile created on Tue Jun 16 08:59:40 CEST 2020, build ID: 0589419c-738b-4676-a374-18f7bbc7ac23, output base: /home/johndoe/.cache/bazel/_bazel_johndoe/d8eb7a85967b22409442664d380222c0

=== PHASE SUMMARY INFORMATION ===

Total launch phase time         1.070 s   12.95%
Total init phase time           0.299 s    3.62%
Total loading phase time        0.878 s   10.64%
Total analysis phase time       1.319 s   15.98%
Total preparation phase time    0.047 s    0.57%
Total execution phase time      4.629 s   56.05%
Total finish phase time         0.014 s    0.18%
------------------------------------------------
Total run time                  8.260 s  100.00%

Critical path (4.245 s):
       Time Percentage   Description
    8.85 ms    0.21%   _Ccompiler_Udeps for @local_config_cc// compiler_deps
    3.839 s   90.44%   action 'Compiling external/com_google_protobuf/src/google/protobuf/compiler/php/php_generator.cc [for host]'
     270 ms    6.36%   action 'Linking external/com_google_protobuf/protoc [for host]'
    0.25 ms    0.01%   runfiles for @com_google_protobuf// protoc
     126 ms    2.97%   action 'ProtoCompile external/com_google_protobuf/python/google/protobuf/compiler/plugin_pb2.py'
    0.96 ms    0.02%   runfiles for //tools/aquery_differ aquery_differ
```

#### Bazel Invocation Analyzer

The open-source [Bazel Invocation Analyzer](https://github.com/EngFlow/bazel_invocation_analyzer) consumes a profile format and prints suggestions on how to improve the build’s performance. This analysis can be performed using its CLI or on https://analyzer.engflow.com.

#### jq

`jq` is like `sed` for JSON data. An example usage of `jq` to extract all durations of the sandbox creation step in local action execution:（使用 jq 提取本地操作执行中沙盒创建步骤的所有时长的示例）

```
$ zcat $(../bazel-6.0.0rc1-linux-x86_64 info output_base)/command.profile.gz | jq '.traceEvents | .[] | select(.name == "sandbox.createFileSystem") | .dur'
6378
7247
11850
13756
6555
7445
8487
15520
[...]
```

### Profile information

The profile contains multiple rows. Usually the bulk of rows represent Bazel threads and their corresponding events, but some special rows are also included.


### Common performance issues

When analyzing performance profiles, look for:

* Slower than expected analysis phase (`runAnalysisPhase`), especially on incremental builds. This can be a sign of a poor rule implementation, for example one that flattens depsets. Package loading can be slow by an excessive amount of targets, complex macros or recursive globs.

分析阶段 (runAnalysisPhase) 低于预期，尤其是在增量构建中。这可能表明规则实现不合理，例如，会使实现扁平化的依赖项。软件包加载可能会因目标数量、复杂的宏或递归 glob 而导致速度缓慢

* Individual slow actions, especially those on the critical path. It might be possible to split large actions into multiple smaller actions or reduce the set of (transitive) dependencies to speed them up. Also check for an unusual high non-PROCESS_TIME (such as `REMOTE_SETUP` or `FETCH`).

单个缓慢操作，尤其是关键路径上的操作。可以将大型操作拆分为多个较小的操作，也可以减少（传递）依赖项集以提高速度。此外，还要检查是否存在异常的非 PROCESS_TIME（例如 REMOTE_SETUP 或 FETCH）

* Bottlenecks, that is a small number of threads is busy while all others are idling / waiting for the result (see around 22s and 29s in Figure 1). Optimizing this will most likely require touching the rule implementations or Bazel itself to introduce more parallelism. This can also happen when there is an unusual amount of GC.

瓶颈（即少量线程处于忙碌状态），而其他所有线程都处于空闲 / 等待结果（参见图 1 中的大约 22 秒和 29 秒）。对此进行优化很可能需要您触摸规则实现或 Bazel 本身，以引入更多并行性。如果 GC 数量异常，也可能发生这种情况

## [优化内存](https://bazel.build/advanced/performance/memory?hl=zh-cn)

### Running Bazel with Limited RAM（使用有限 RAM 运行 Bazel）

In certain situations, you may want Bazel to use minimal memory. You can set the maximum heap via the startup flag [--host_jvm_args](https://bazel.build/docs/user-manual#host-jvm-args), like `--host_jvm_args=-Xmx2g`.

However, if your builds are big enough, Bazel may throw an `OutOfMemoryError` (OOM) when it doesn't have enough memory. You can make Bazel use less memory, at the cost of slower incremental builds, by passing the following command flags: [--discard_analysis_cache](https://bazel.build/docs/user-manual#discard-analysis-cache), [--nokeep_state_after_build](https://bazel.build/reference/command-line-reference#flag--keep_state_after_build), and [--notrack_incremental_state](https://bazel.build/reference/command-line-reference#flag--track_incremental_state).

These flags will minimize the memory that Bazel uses in a build, at the cost of making future builds slower than a standard incremental build would be.（这些标志可最大限度减少 Bazel 在构建中使用的内存，代价是未来的构建速度会低于标准增量构建）


## [优化迭代速度](https://bazel.build/advanced/performance/iteration-speed?hl=zh-cn)

### Bazel's Runtime State（Bazel 的运行时状态）

A Bazel invocation involves several interacting parts.

* The `bazel` command line interface (CLI) is the user-facing front-end tool and receives commands from the user.（bazel 命令行界面 (CLI) 是面向用户的前端工具，用于接收来自用户的命令）

* The CLI tool starts a [Bazel server](https://bazel.build/run/client-server) for each distinct [output base](https://bazel.build/remote/output-directories). The Bazel server is generally persistent, but will shut down after some idle time so as to not waste resources.（CLI 工具会为每个不同的输出基准启动一个 Bazel 服务器。Bazel 服务器通常是永久性的，但会在闲置一段时间后关闭，以免浪费资源）

* The Bazel server performs the loading and analysis steps for a given command (build, run, cquery, etc.), in which it constructs the necessary parts of the build graph in memory. The resulting data structures are retained in the Bazel server as part of the analysis cache.（Bazel 服务器会针对给定命令（build、run、cquery 等）执行加载和分析步骤，在此过程中，它会在内存中构造构建图的必要部分。生成的数据结构将作为分析缓存的一部分保留在 Bazel 服务器中）

* The Bazel server can also perform the action execution, or it can send actions off for remote execution if it is set up to do so. The results of action executions are also cached, namely in the action cache (or execution cache, which may be either local or remote, and it may be shared among Bazel servers).（Bazel 服务器也可以执行操作，或者发送相关操作以便远程执行（如果已设置）。操作执行的结果也会缓存，即缓存在操作缓存（或执行缓存，可以是本地或远程的，并且可在 Bazel 服务器之间共享））

* The result of the Bazel invocation is made available in the output tree.（Bazel 调用的结果将在输出树中提供）

### Running Bazel Iteratively（以迭代方式运行 Bazel）

In a typical developer workflow, it is common to build (or run) a piece of code repeatedly, often at a very high frequency (e.g. to resolve some compilation error or investigate a failing test). In this situation, it is important that repeated invocations of `bazel` have as little overhead as possible relative to the underlying, repeated action (e.g. invoking a compiler, or executing a test).（在典型的开发者工作流中，开发者常常会重复构建（或运行）一段代码，而且频率通常非常高（例如，为了解决某些编译错误或调查失败的测试）。在这种情况下，对于 bazel 的重复调用，相对于底层的重复操作（例如调用编译器或执行测试）而言，其开销必须尽可能低）

With this in mind, we take another look at Bazel's runtime state:

The analysis cache is a critical piece of data. A significant amount of time can be spent just on the loading and analysis phases of a cold run (i.e. a run just after the Bazel server was started or when the analysis cache was discarded). For a single, successful cold build (e.g. for a production release) this cost is bearable, but for repeatedly building the same target it is important that this cost be amortized and not repeated on each invocation.

分析缓存是关键数据。大量的时间只会花在冷运行的加载和分析阶段（即 Bazel 服务器启动之后的运行或分析缓存被舍弃时）。对于单次成功的冷构建（例如，生产版本），可承担这笔费用，但对于重复构建同一目标，必须摊销这笔费用，而不是在每次调用时重复

The analysis cache is rather volatile. First off, it is part of the in-process state of the Bazel server, so losing the server loses the cache. But the cache is also invalidated very easily: for example, many bazel command line flags cause the cache to be discarded. This is because many flags affect the build graph (e.g. because of [configurable attributes](https://bazel.build/configure/attributes)). Some flag changes can also cause the Bazel server to be restarted (e.g. changing [startup options](https://bazel.build/docs/user-manual#startup-options)).

分析缓存相当易失。首先，它是 Bazel 服务器的进程中状态的一部分，因此丢失服务器会使缓存丢失。不过，缓存也很容易失效：例如，许多 bazel 命令行标志都会导致缓存被舍弃。这是因为很多标志都会影响构建图（例如，由于可配置属性）。某些标志更改还可能会导致 Bazel 服务器重启（例如，更改启动选项）

A good execution cache is also valuable for build performance. An execution cache can be kept locally [on disk](https://bazel.build/remote/caching#disk-cache), or [remotely](https://bazel.build/remote/caching). The cache can be shared among Bazel servers, and indeed among developers.

良好的执行缓存对构建性能也很重要。执行缓存可以保存在本地磁盘上，也可以远程保留。缓存可以共享给 Bazel 服务器，甚至可以在开发者之间共享。

### Avoid discarding the analysis cache（避免舍弃分析缓存）

Bazel will print a warning if either the analysis cache was discarded or the server was restarted. Either of these should be avoided during iterative use:（如果分析缓存被舍弃或服务器重启，Bazel 会输出一条警告。在迭代使用过程中应避免以下两种情况）

* Be mindful of changing `bazel` flags in the middle of an iterative workflow. For example, mixing a `bazel build -c opt` with a `bazel cquery` causes each command to discard the analysis cache of the other. In general, try to use a fixed set of flags for the duration of a particular workflow.

请注意在迭代工作流过程中更改 bazel 标志。例如，将 bazel build -c opt 与 bazel cquery 混用会导致每个命令舍弃彼此的分析缓存。通常，应尝试在特定工作流期间使用一组固定的标志


* Losing the Bazel server loses the analysis cache. The Bazel server has a [configurable](https://bazel.build/docs/user-manual#max-idle-secs) idle time, after which it shuts down. You can configure this time via your bazelrc file to suit your needs. The server also restarted when startup flags change, so, again, avoid changing those flags if possible.

丢失 Bazel 服务器会导致分析缓存丢失。Bazel 服务器具有可配置的空闲时间，超过此时间后就会关闭。您可以通过 bazelrc 文件来配置此时间，以满足自己的需求。启动标志更改时，服务器也会重启，因此同样，请尽可能避免更改这些标志

* Beware that the Bazel server is killed if you press Ctrl-C repeatedly while Bazel is running. It is tempting to try to save time by interrupting a running build that is no longer needed, but only press Ctrl-C once to request a graceful end of the current invocation.

请注意，如果在 Bazel 运行期间反复按 Ctrl-C，Bazel 服务器会终止。人们往往想尝试通过中断不再需要的正在运行的构建来节省时间，但只需按一次 Ctrl-C 即可请求顺利结束当前调用

* If you want to use multiple sets of flags from the same workspace, you can use multiple, distinct output bases, switched with the `--output_base` flag. Each output base gets its own Bazel server.

如果要使用同一工作区中的多组标志，您可以使用通过 --output_base 标志切换的多个不同输出基准。每个输出库都有自己的 Bazel 服务器

To make this condition an error rather than a warning, you can use the `--noallow_analysis_cache_discard` flag (introduced in Bazel 6.4.0)












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


## VS CODE

* Bazel
* bazel-stack-vscode



# [Protocol Buffers in Bazel](https://blog.bazel.build/2017/02/27/protocol-buffers.html)

## TL;DR - Usage example

* https://github.com/cgrushko/proto_library contains a buildable example.
* https://github.com/cgrushko/proto_library/blob/master/src/BUILD


## Benefits

In comparison with a macro that's responsible for compiling all `.proto` files in a project.

1. Caching + incrementality: changing a single `.proto` only causes the rebuilding of dependant `.proto` files. This includes not only regenerating code, but also recompiling it. For large proto graphs this could be significant.

2. Depend on pieces of a proto graph from multiple places: in the example above, one can add a `cc_proto_library` that deps on `zip_code_proto`, and including it together with `//src:person_cc_proto` in the same project. Though they both transitively depend on `zip_code_proto`, there won't be a linking error.


## Recommended Code Organization

1. One `proto_library` rule per `.proto` file.
2. A file named `foo.proto` will be in a rule named `foo_proto`, which is located in the same package.
3. A `<lang>_proto_library` that wraps a `proto_library` named `foo_proto` should be called `foo_<lang>_proto`, and be located in the same package.





# Bazel 源码

源码搜索：https://cs.opensource.google/bazel


# [Bazel Release Model](https://bazel.build/release?hl=en) (版本发布说明)

As announced in [the original blog post](https://blog.bazel.build/2020/11/10/long-term-support-release.html), Bazel 4.0 and higher versions provides support for two release tracks: **rolling releases** and **long term support (LTS) releases**.

Bazel uses a `major.minor.patch` Semantic Versioning scheme.

* A `major` release contains features that are not backward compatible with the previous release. Each major Bazel version is an LTS release.
* A `minor` release contains backward-compatible bug fixes and features back-ported from the main branch.
* A `patch` release contains critical bug fixes.



# Tutorial


* [Bazel Training](https://docs.aspect.build/tutorial/)
* [Beginning Bazel: Building and Testing for Java, Go, and More](https://github.com/Apress/beginning-bazel/blob/master/README.md)
* 下载地址：
  + https://drive.weixin.qq.com/s?k=AJEAIQdfAAoXSWclXBAAQAmwaCACc
  + file:///Users/gerry/Downloads/beginning-bazel-building-and-testing-for-java-go-and-more_compress.pdf
* 对应代码：https://github.com/Apress/beginning-bazel

# Q&A

## 使用 bazel 构建出的二进制程序功能缺失 (cc_library.alwayslink)

问题描述：使用下面的构建规则出现生成的二进制文件中缺失功能。

``` python
package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "gamesvr",
    deps = ["//src/gamesvr:gamesvr_inner"],
    tags = ["no-cache"],
)

cc_library(
    name = "gamesvr_inner",
    srcs = glob(
        [
            "**/*.cpp",
            "**/*.cc",
            "**/*.c",
            "**/*.h",
        ],
    ),
    includes = ["."],
    copts = ["-Ilibs"],
    deps = [
        "//libs/common:common_lib",
        "//libs/business:business_hdrs",
    ],
    linkstatic = True,
)
```

解决方法：参考 https://bazel.build/reference/be/c-cpp?hl=zh-cn#cc_library.alwayslink，需要在 cc_library 中添加一个 `alwayslink = True` 保证 gamesvr_inner 所有符号链接到 gamesvr 这个目标。




## bazel 使用动态库

* [bazel c++ create and link with shared library](https://stackoverflow.com/questions/53514630/bazel-c-create-and-link-with-shared-library?rq=3)
* [Link archive to shared library with Bazel](https://stackoverflow.com/questions/61487115/link-archive-to-shared-library-with-bazel)


## -c opt and -fPIC (release 版本构建仍然会生成 non PIC 的目标文件)

问题描述：

业务构建有指定 `build --copt=-fPIC` ，但是在使用 `opt` 模式编译时，会分别生成 pic 和 non-pic 两个版本的目标文件；但是在 `dbg` 模式下只会生成 pic 版本的目标文件。

参考 https://groups.google.com/g/bazel-discuss/c/agycffdH0R0 发现：

bazel 在使用 `-c opt and -fPIC` 构建时，会同时构建 pic 和 non-pic 两个版本。然而 release 版本链接可执行文件时只会用到非 pic 版本，链接动态库时会使用 pic 版本。这就解释了为什么构建 release 版本会构建两次，分别有 pic 和 non-pic 两个版本的构建。


> We are building ffmpeg inside of bazel, and it needs to be compiled with -fPIC. We've discovered two potential bugs:
>
> 1. Even when specifying '-fPIC' in the copts, bazel tries to do a non PIC build. So we end up building each source file twice, but they are in fact the same. Once bazel includes it's own '-fPIC', and once it doesn't, but since it's included as part of the copts, it is always passed to the compiler.
>
> ...

相关的解释：

> Building non-PIC code for binaries is intentional, although I'm somewhat surprised that binaries are built *twice*. Are you by any chance building dynamic libraries and binaries on the same command line?
>
> This behavior is indeed surprising. Marcel opined that it was trading startup time for faster code, which makes sense, at least on the surface.
>
> You can use the --force_pic command line option to build binaries in PIC mode, too. We should give you more control over picness, but in the meantime, I hope this'll do.


> This behavior is indeed surprising. Marcel opined that it was trading startup time for faster code, which makes sense, at least on the surface.
>
> Yes, it's an expensive trade-off, but it would be worth it at Google scale probably.

解决方法：

在构建 release 版本时添加 `--force_pic` 选项强制使用 pic 版本且在链接可执行文件时指定 `build --linkopt="-no-pie"` 不生成 pie 的可执行程序。这样可以保证 release 版本构建时只会构建一次 pic 的目标文件，而不会构建两次。

https://bazel.build/reference/command-line-reference?hl=zh-cn#flag--force_pic


## [bazel: why do targets keep increasing if build graph is known](https://stackoverflow.com/questions/51484140/bazel-why-do-targets-keep-increasing-if-build-graph-is-known)

Q:

Per [this doc](https://docs.bazel.build/versions/master/user-manual.html#phases), the analysis and execution phases handle building out the dependency tree (among other things) and going and doing the work if needed, respectively. If that's true, I'm curious why the total number of targets keeps increasing as the build progresses (i.e., when I start a large build, bazel may report that it's built 5 out of 100 targets, but later will say it's built 20 out of 300 targets, and so forth, with the denominator increasing for a while until it levels off).

A:

The number you're seeing in the progress bar refers to **actions** and not **targets** (e.g. //my:target). [I wrote a blog post](https://jin.crypt.sg/articles/bazel-action-graph.html) about the action graph, and here's the relevant description about it:

> The action graph contains a different set of information: file-level dependencies, full command lines, and other information Bazel needs to execute the build. If you are familiar with Bazel's build phases, the action graph is the output of the loading and analysis phase and used during the execution phase.
>
> However, Bazel does not necessarily execute every action in the graph. It only executes if it has to, that is, the action graph is the super set of what is actually executed.

As to why the denominator is ever-increasing, it's because the actions-to-execute discovery within the action graph is lazy. Here's a better explanation from the Bazel TL, Ulf Adams:

> The problem is that Skyframe does not eagerly walk the action graph, but it does it lazily. The reason for that is performance, since the action graph can be rather large and this was previously a blocking operation (where Bazel would just hang for some time). The downside is that all threads that walk the action graph block on actions that they execute, which delays discovery of remaining actions. That's why the number keeps going up during the build.

Source: https://github.com/bazelbuild/bazel/issues/3582#issuecomment-329405311

refer: [Grok Your Bazel Build: The Action Graph](https://jin.crypt.sg/articles/bazel-action-graph.html)

## 单测加载外部文件

* [Access runtime files in application built with Bazel](https://stackoverflow.com/questions/49875481/access-runtime-files-in-application-built-with-bazel)

Looks like Bazel 0.15 [added](https://github.com/bazelbuild/bazel/issues/4460#issuecomment-402165299) support for what is called [Rlocation](https://github.com/bazelbuild/bazel/blob/master/tools/cpp/runfiles/runfiles_src.h), which allows looking up the runtime files in the application code:

1. Depend on this runfiles library from your build rule:

```
cc_binary(
  name = "my_binary",
  ...
  deps = ["@bazel_tools//tools/cpp/runfiles"],
)
```

2. Include the runfiles library.

``` cpp
#include "tools/cpp/runfiles/runfiles.h"
using bazel::tools::cpp::runfiles::Runfiles;
```

3. Create a `Runfiles` object and use `Rlocation` to look up runfile paths:

``` cpp
int main(int argc, char** argv) {
  std::string error;
  std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0], &error));

  // Important:
  //   If this is a test, use Runfiles::CreateForTest(&error).
  //   Otherwise, if you don't have the value for argv[0] for whatever
  //   reason, then use Runfiles::Create(&error).

  if (runfiles == nullptr) {
    // error handling
  }

  std::string path = runfiles->Rlocation("my_workspace/path/to/my/data.txt");

  // ...
}
```

* [Why can't my programs find resource files when using bazel run //package](https://stackoverflow.com/questions/71826978/why-cant-my-programs-find-resource-files-when-using-bazel-run-package)

* [C++ Bazel project with a Data repository](https://stackoverflow.com/questions/46100906/c-bazel-project-with-a-data-repository)

https://github.com/vincent-picaud/Bazel_with_Data





# Examples

* [5 minute guide to Bazel, Part 1: C & C++](https://jin.crypt.sg/articles/bazel-in-5-minutes-c.html)
* [5 minute guide to Bazel, Part 2: Command lines and tools](https://jin.crypt.sg/articles/bazel-in-5-minutes-genrule.html)
* [Grok Your Bazel Build: The Action Graph](https://jin.crypt.sg/articles/bazel-action-graph.html)
* [Questions to Ask Before Writing A Bazel Rule](https://jin.crypt.sg/articles/bazel-rules-questions.html)
* [Generating pretty-printed sources with Bazel](https://jin.crypt.sg/articles/bazel-pretty-print.html)
* https://github.com/bazelbuild/examples
* https://github.com/abseil/abseil-cpp/blob/master/absl/base/BUILD.bazel




# Refer

* https://bazel.build/start
* https://bazel.build/tutorials/cpp
* https://bazel.build/tutorials/cpp-use-cases
* https://bazel.build/reference?hl=zh-cn
* [Bazel学习笔记](https://blog.gmem.cc/bazel-study-note)
* https://github.com/abseil/abseil-cpp/blob/master/absl/base/BUILD.bazel
* https://google.github.io/googletest/quickstart-bazel.html



* Remote Cache
  * [Using Remote Cache Service for Bazel - pdf](https://dl.acm.org/doi/pdf/10.1145/3267120)
  * [Using Remote Cache Service for Bazel](https://cacm.acm.org/magazines/2019/1/233524-using-remote-cache-service-for-bazel/fulltext)










