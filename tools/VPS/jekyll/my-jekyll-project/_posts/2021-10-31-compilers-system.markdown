---
layout: post
title:  "Compilers System"
date:   2021-10-31 11:30:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}


# 编译基础

## [Translation unit](https://en.wikipedia.org/wiki/Translation_unit_(programming))

In C and C++ programming language terminology, a **translation unit** (or more casually a compilation unit) is the ultimate input to a C or C++ compiler from which an object file is generated. **A translation unit** roughly consists of a source file after it has been processed by the C preprocessor, meaning that header files listed in `#include` directives are literally included, sections of code within `#ifndef` may be included, and macros have been expanded. A C++ module is also a translation unit.




## [Single compilation unit](https://en.wikipedia.org/wiki/Single_compilation_unit) (编译单元)

**Single compilation unit** (`SCU`) is a computer programming technique for the C and C++ languages, which reduces compilation time for programs spanning multiple files. Specifically, it allows the compiler to keep data from shared header files, definitions and templates, so that it need not recreate them for each file. It is an instance of program optimization. The technique can be applied to an entire program or to some subset of source files; when applied to an entire program, it is also known as a [unity build](https://en.wikipedia.org/wiki/Unity_build).

Example:

For example, if you have the source files `foo.cpp` and `bar.cpp`, they can be placed in a **Single Compilation Unit** as follows:

``` cpp
#include "foo.cpp"
#include "bar.cpp"
```

Suppose `foo.cpp` and `bar.cpp` are:

``` cpp
//foo.cpp
#include <iostream> // A large, standard header
#include "bar.h"    // Declaration of function 'bar'

int main()          // Definition of function 'main'
{
  bar();
}
```

``` cpp
//bar.cpp
#include <iostream> // The same large, standard header

void bar()          // Definition of function 'bar'
{
  ...
}
```

Now the standard header file (`iostream`) is compiled only once, and function `bar` may be inlined into function `main`, despite being from another module.


## [Precompiled header](https://en.wikipedia.org/wiki/Precompiled_header)

In computer programming, a **precompiled header** (`PCH`) is a (C or C++) header file that is compiled into an intermediate form that is faster to process for the compiler. Usage of precompiled headers may significantly reduce compilation time, especially when applied to large header files, header files that include many other header files, or header files that are included in many translation units.


## [C preprocessor](https://en.wikipedia.org/wiki/C_preprocessor)

The **C preprocessor** (`CPP`) is a text file processor that is used with C, C++ and other programming tools. The preprocessor provides for file inclusion (often header files), [macro](https://en.wikipedia.org/wiki/Macro_(computer_science)) expansion, [conditional compilation](https://en.wikipedia.org/wiki/Conditional_compilation), and line control. Although named in association with C and used with C, the preprocessor capabilities are not inherently tied to the C language. It can and is used to process other kinds of files.


## [Conditional compilation](https://en.wikipedia.org/wiki/Conditional_compilation)

In computer programming, **conditional compilation** is a compilation technique which results in differring executable programs depending on parameters specified. This technique is commonly used when these differences in the program are needed to run it on different platforms, or with different versions of required libraries or hardware.

Many programming languages support conditional compilation. Typically [compiler directives](https://en.wikipedia.org/wiki/Compiler_directives) define or "undefine" certain variables; other directives test these variables and modify compilation accordingly. For example, not using an actual language, the compiler may be set to define "Macintosh" and undefine "PC", and the code may contain:

``` c
(* System generic code *)
if mac != Null then
    (* macOS specific code *)
else if pc != Null
    (* Windows specific code *)
```

In C and some languages with a similar syntax, this is done using an ['#ifdef' directive](https://en.wikipedia.org/wiki/C_preprocessor#Conditional_compilation).



## [Interprocedural optimization](https://en.wikipedia.org/wiki/Interprocedural_optimization)

**Interprocedural optimization** (`IPO`) is a collection of compiler techniques used in computer programming to improve performance in programs containing many frequently used functions of small or medium length. IPO differs from other compiler optimizations by analyzing the entire program as opposed to a single function or block of code.

`IPO` seeks to reduce or eliminate duplicate calculations and inefficient use of memory and to simplify iterative sequences such as loops. If a call to another routine occurs within a loop, IPO analysis may determine that it is best to [inline](https://en.wikipedia.org/wiki/Inline_expansion) that routine. Additionally, IPO may re-order the routines for better memory layout and [locality](https://en.wikipedia.org/wiki/Memory_locality).

`IPO` may also include typical compiler optimizations applied on a whole-program level, for example, [dead code elimination](https://en.wikipedia.org/wiki/Dead_code_elimination) (`DCE`), which removes code that is never executed. IPO also tries to ensure better use of constants. Modern compilers offer IPO as an option at compile-time. The actual IPO process may occur at any step between the human-readable source code and producing a finished executable binary program.

For languages that compile on a file-by-file basis, effective IPO across [translation units](https://en.wikipedia.org/wiki/Translation_unit_(programming)) (module files) requires knowledge of the "entry points" of the program so that a **whole program optimization** (`WPO`) can be run. In many cases, this is implemented as a **link-time optimization** (`LTO`) pass, because the whole program is visible to the linker.


## [JSON Compilation Database Format Specification](https://clang.llvm.org/docs/JSONCompilationDatabase.html) (编译数据库)

> The [JSON compilation database](http://clang.llvm.org/docs/JSONCompilationDatabase.html) is used in the clang project to provide information on how a single compilation unit is processed. With this, it is easy to re-run the compilation with alternate programs.

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


> 注意：CMake 生成的 compile_commands.json 文件中只有 `.cpp` 的编译命令，而 Bazel 使用 [this extractor extension](https://github.com/hedronvision/bazel-compile-commands-extractor) 方案生成的 compile_commands.json 文件还会包含 `.h` 的编译命令，导致生成的 compile_commands.json 文件较大。此问题可参考这个 issue [Provide more specifics about headers in compile_commands.json #213](https://github.com/hedronvision/bazel-compile-commands-extractor/issues/213)

具体原因是：

参考 [this extractor extension](https://github.com/hedronvision/bazel-compile-commands-extractor) 工具的[解释](https://github.com/hedronvision/bazel-compile-commands-extractor/blob/1e08f8e0507b6b6b1f4416a9a22cf5c28beaba93/refresh_compile_commands.bzl#L34-L37)：

``` bash
    # Using ccls or another tool that doesn't want or need headers in compile_commands.json?
        # exclude_headers = "all", # By default, we include entries for headers to support clangd, working around https://github.com/clangd/clangd/issues/123
        # ^ excluding headers will speed up compile_commands.json generation *considerably* because we won't need to preprocess your code to figure out which headers you use.
        # However, if you use clangd and are looking for speed, we strongly recommend you follow the instructions below instead, since clangd is going to regularly infer the wrong commands for headers and give you lots of annoyingly unnecessary red squigglies.

    # Need things to run faster? [Either for compile_commands.json generation or clangd indexing.]
    # First: You might be able to refresh compile_commands.json slightly less often, making the current runtime okay.
        # If you're adding files, clangd should make pretty decent guesses at completions, using commands from nearby files. And if you're deleting files, there's not a problem. So you may not need to rerun refresh.py on every change to BUILD files. Instead, maybe refresh becomes something you run every so often when you can spare the time, making the current runtime okay.
        # If that's not enough, read on.
    # If you don't care about the implementations of external dependencies:
        # Then skip adding entries for compilation in external workspaces with
        # exclude_external_sources = True,
        # ^ Defaults to False, so the broadest set of features are supported out of the box, without prematurely optimizing.
    # If you don't care about browsing headers from external workspaces or system headers, except for a CTRL/CMD+click every now and then:
        # Then no need to add entries for their headers, because clangd will correctly infer from the CTRL/CMD+click (but not a quick open or reopen).
        # exclude_headers = "external",
    # Still not fast enough?
        # Make sure you're specifying just the targets you care about by setting `targets`, above.
```

再参考 [this extractor extension](https://github.com/hedronvision/bazel-compile-commands-extractor) 工具的[另一个解释](https://github.com/hedronvision/bazel-compile-commands-extractor/blob/1e08f8e0507b6b6b1f4416a9a22cf5c28beaba93/refresh.template.py#L665-L667)：由于 clangd 的 [Use parsed files to improve header compile commands #123](https://github.com/clangd/clangd/issues/123) 问题，**bazel-compile-commands-extractor 工具因此需要 apply commands to headers and sources**。

```
    # Note: We need to apply commands to headers and sources.
    # Why? clangd currently tries to infer commands for headers using files with similar paths. This often works really poorly for header-only libraries. The commands should instead have been inferred from the source files using those libraries... See https://github.com/clangd/clangd/issues/123 for more.
    # When that issue is resolved, we can stop looking for headers and just return the single source file.
```

其他相关的issue:

* [compile_commands.json contains entries for headers! And it takes a little while to generate! ~20s on the Hedron main repo at the time of writing. Could we speed it up? #5](https://github.com/hedronvision/bazel-compile-commands-extractor/issues/5) (此 issue 包含了更多讨论的信息)
* [compile_commands.json is large! Could we make it smaller? #6](https://github.com/hedronvision/bazel-compile-commands-extractor/issues/6)
* [Improve compile_commands refresh time. #24](https://github.com/hedronvision/bazel-compile-commands-extractor/issues/24)
* [Same compile command repeated many times. #106](https://github.com/hedronvision/bazel-compile-commands-extractor/issues/106)


如果不想对头文件生成编译命令，可以通过设置 `exclude_headers = "all"` 来实现：

```
Using ccls or another tool that, unlike clangd, doesn't want or need headers in compile_commands.json?
Similar to the above, we'll use refresh_compile_commands for configuration, but instead of setting targets, set exclude_headers = "all".
```



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




# Make (GNU make utility to maintain groups of programs)

The `make` utility will determine automatically which pieces of a large program need to be recompiled, and issue the commands to recompile them. The manual describes the GNU implementation of make, which was written by Richard Stallman and Roland McGrath, and is currently maintained by Paul Smith. Our examples show C programs, since they are very common, but you can use make with any programming language whose compiler can be run with a shell command. In fact, make is not limited to programs. You can use it to describe any task where some files must be updated automatically from others whenever the others change.

To prepare to use make, you must write a file called the `makefile` that describes the relationships among files in your program, and the states the commands for updating each file. In a program, typically the executable file is updated from object files, which are in turn made by compiling source files.

Once a suitable makefile exists, each time you change some source files, this simple shell command: `make`  suffices to perform all necessary recompilations. The make program uses the makefile description and the last-modification times of the files to decide which of the files need to be updated. For each of those files, it issues the commands recorded in the `makefile`.

`make` executes commands in the `makefile` to update one or more target names, where name is typically a program.  If no `-f` option is present, make will look for the makefiles `GNUmakefile`, `makefile`, and `Makefile`, **in that order**.

Normally you should call your makefile either **makefile** or **Makefile**.  (We recommend **Makefile** because it appears prominently near the beginning of a directory listing, right near other important files such as README.)  The first name checked, **GNUmakefile**, is not recommended for most makefiles. You should use this name if you have a makefile that is specific to GNU make, and will not be understood by other versions of make. If makefile is '-', the standard input is read.

`make` updates a target if it depends on **prerequisite files that have been modified since the target was last modified**, or **if the target does not exist**.


refer:

* https://man7.org/linux/man-pages/man1/make.1.html
* [GNU make](https://www.gnu.org/software/make/manual/make.html)
* [Automatic-Variables](https://www.gnu.org/software/make/manual/make.html#Automatic-Variables)


# Ninja (a small build system with a focus on speed)

[Ninja](https://ninja-build.org/) is a small build system with a focus on speed. It differs from other build systems in two major respects: it is designed to have its input files generated by a higher-level build system, and it is designed to run builds as fast as possible.

对比使用`ninja`和`make`编译[llvm-project-11.0.0.tar.xz](https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/llvm-project-11.0.0.tar.xz) ：

> 结论：

* `ninja`在高并发时表现出色，因此对于编译性能要求很高的场景，建议使用`ninja`构建系统。
* 在上文的llvm-project对比测试中，同样由cmake生成编译脚本（`-G Ninja`, `-G "Unix Makefiles"`）的情况下，`ninja`表现明显优于`Makefile`。


测试脚本：

```
#!/bin/bash

rm -rf build
mkdir -p build && cd build

export LLVM_INSTALL_DIR=$HOME/compile/test/install

COMPILE_MODE=gcc
#COMPILE_MODE=clang

## use `ccmake .` to use cmake gui
if [ $COMPILE_MODE == "gcc"  ]; then
        export CC=/opt/rh/devtoolset-7/root/usr/bin/cc
        export CXX=/opt/rh/devtoolset-7/root/usr/bin/c++

        ## ninja
        cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi;libunwind;lldb;compiler-rt;lld" -DCMAKE_INSTALL_PREFIX=$LLVM_INSTALL_DIR ../llvm-project-11.0.0/llvm

        ## makefile
        #cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi;libunwind;lldb;compiler-rt;lld" -DCMAKE_INSTALL_PREFIX=$LLVM_INSTALL_DIR ../llvm-project-11.0.0/llvm


elif [ $COMPILE_MODE == "clang" ]; then

        export CC=/root/compile/llvm_install/bin/clang
        export CXX=/root/compile/llvm_install/bin/clang++

        cmake -G "Ninja" -fuse-ld=lld  -DCMAKE_USER_MAKE_RULES_OVERRIDE=./ClangOverrides.txt  -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi;libunwind;lldb;compiler-rt;lld" -DCMAKE_INSTALL_PREFIX=$LLVM_INSTALL_DIR ../llvm-project-11.0.0/llvm
        #cmake -G "Ninja" -fuse-ld=lld  -DCMAKE_TOOLCHAIN_FILE=./LinuxToolchains.cmake  -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi;libunwind;lldb;compiler-rt;lld" -DCMAKE_INSTALL_PREFIX=$LLVM_INSTALL_DIR ../llvm-project-11.0.0/llvm

else
        echo "error: $COMPILE_MODE invalid"
        exit 1
fi

## 8 cpu, 16G mem
/usr/bin/time -f "real %e user %U sys %S" ninja -j8 -v
#/usr/bin/time -f "real %e user %U sys %S" ninja -j256

## LLD leaves its name and version number to a .comment section in an output
## readelf --string-dump .comment <output-file>

echo "have done"
```

## ninja + gcc

```
top - 21:08:49 up 181 days, 16 min,  4 users,  load average: 8.18, 8.43, 16.17
Tasks: 193 total,  10 running, 183 sleeping,   0 stopped,   0 zombie
%Cpu0  : 92.7 us,  7.3 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
%Cpu1  : 96.7 us,  3.3 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
%Cpu2  : 97.3 us,  2.7 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
%Cpu3  : 96.7 us,  3.3 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
%Cpu4  : 97.3 us,  2.7 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
%Cpu5  : 90.3 us,  9.7 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
%Cpu6  : 96.3 us,  3.7 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
%Cpu7  : 92.7 us,  7.3 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
KiB Mem : 16165976 total,  7329180 free,  4121088 used,  4715708 buff/cache
KiB Swap:        0 total,        0 free,        0 used. 11429800 avail Mem

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU %MEM     TIME+ COMMAND
19664 root      20   0  700252 652588   8600 R  99.7  4.0   0:07.02 cc1plus
19684 root      20   0  446644 397780   6464 R  99.7  2.5   0:03.94 cc1plus
19687 root      20   0  450332 402040   6400 R  99.7  2.5   0:03.84 cc1plus
19677 root      20   0  665556 615736   6476 R  99.3  3.8   0:05.49 cc1plus
19681 root      20   0  505488 457364   6452 R  98.7  2.8   0:04.45 cc1plus
19691 root      20   0  444044 394996   6404 R  97.0  2.4   0:03.17 cc1plus
19695 root      20   0  304324 254396   6028 R  47.2  1.6   0:01.42 cc1plus
19699 root      20   0  149372 102168   5920 R  20.3  0.6   0:00.61 cc1plus
```

results:

```
$time ninja -j8

real    40m26.115s
user    301m22.380s
sys     14m17.540s
```

增加`ninja`并发可以增加速度，但是需要有较大的内存。以下为256并发时内存空间已不足，编译时会报内部错误。


```
[71/6710] Building CXX object lib/Support/CMakeFiles/LLVMSupport.dir/Debug.cpp.o
FAILED: lib/Support/CMakeFiles/LLVMSupport.dir/Debug.cpp.o
/opt/rh/devtoolset-7/root/usr/bin/c++  -DGTEST_HAS_RTTI=0 -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -Ilib/Support -I/root/compile/test/llvm-project-11.0.0/llvm/lib/Support -Iinclude -I/root/compile/test/llvm-project-11.0.0/llvm/include -fPIC -fvisibility-inlines-hidden -Werror=date-time -Wall -Wextra -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wno-missing-field-initializers -pedantic -Wno-long-long -Wimplicit-fallthrough -Wno-maybe-uninitialized -Wno-noexcept-type -Wdelete-non-virtual-dtor -Wno-comment -fdiagnostics-color -ffunction-sections -fdata-sections -O3 -DNDEBUG   -std=c++14  -fno-exceptions -fno-rtti -MD -MT lib/Support/CMakeFiles/LLVMSupport.dir/Debug.cpp.o -MF lib/Support/CMakeFiles/LLVMSupport.dir/Debug.cpp.o.d -o lib/Support/CMakeFiles/LLVMSupport.dir/Debug.cpp.o -c /root/compile/test/llvm-project-11.0.0/llvm/lib/Support/Debug.cpp
c++: internal compiler error: Killed (program cc1plus)
Please submit a full bug report,
with preprocessed source if appropriate.
See <http://bugzilla.redhat.com/bugzilla> for instructions.
```

```
top - 23:38:58 up 181 days,  2:46,  4 users,  load average: 101.87, 24.81, 8.97
Tasks: 693 total, 105 running, 587 sleeping,   0 stopped,   1 zombie
%Cpu(s): 18.0 us, 17.8 sy,  0.0 ni,  0.2 id, 63.9 wa,  0.0 hi,  0.0 si,  0.0 st
KiB Mem : 16165976 total,   157708 free, 15388396 used,   619872 buff/cache
KiB Swap:        0 total,        0 free,        0 used.   183040 avail Mem

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU %MEM     TIME+ COMMAND
   64 root      20   0       0      0      0 R  27.8  0.0   3:15.14 kswapd0
    7 root      rt   0       0      0      0 S  15.2  0.0   1:25.26 migration/0
   22 root      rt   0       0      0      0 S  14.8  0.0   1:20.00 migration/3
12958 root      20   0       0      0      0 Z   5.1  0.0   4:09.43 base_agent_net
15647 root      20   0   84008  35320    896 D   3.3  0.2   0:00.31 cc1plus
26692 root      20   0  286276   4072      0 D   3.0  0.0  72:34.73 sap1012
15430 root      20   0  109000  60828   2428 D   2.9  0.4   0:00.48 cc1plus
15158 root      20   0  166464 116372   1908 R   2.7  0.7   0:00.86 cc1plus
15470 root      20   0   99312  49680    356 R   2.6  0.3   0:00.41 cc1plus
15532 root      20   0   99356  49384    360 R   2.6  0.3   0:00.37 cc1plus
```

## makefile + gcc

TODO


refer:

* [Replacing Make with Ninja](https://jpospisil.com/2014/03/16/replacing-make-with-ninja.html)
* [Ninja, a small build system with a focus on speed](https://martine.github.io/ninja)  (homepage)
* [The Performance of Open Source Software, Ninja](https://www.aosabook.org/en/posa/ninja.html)
* [Ninja, a new build system](http://neugierig.org/software/chromium/notes/2011/02/ninja.html)


# 链接器


## ld (The GNU linker)

> ld [options] objfile ...

`ld` combines a number of object and archive files, relocates their data and ties up symbol references. Usually the last step in compiling a program is to run `ld`.

refer: [ld(1) - Linux man page](https://linux.die.net/man/1/ld)

## ld.gold (an official GNU package)

The motivation for writing gold was to make a linker that is faster than the [GNU linker](https://en.wikipedia.org/wiki/GNU_linker), especially for large applications coded in C++.

```
# use ld.gold instead of ld for performance
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")

# get link stage stats
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--stats")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stats")
```

* [gold (linker)](https://en.wikipedia.org/wiki/Gold_%28linker%29)
* [Replacing ld with gold - any experience?](https://stackoverflow.com/questions/3476093/replacing-ld-with-gold-any-experience)


## lld (The LLVM Linker)

Replace `ld` to `lld`:

```
# orig: /opt/rh/devtoolset-7/root/etc/alternatives/ld -> /opt/rh/devtoolset-7/root/usr/bin/ld.bfd
# ln -s /opt/rh/devtoolset-7/root/usr/bin/ld.bfd /opt/rh/devtoolset-7/root/etc/alternatives/ld
#
rm /opt/rh/devtoolset-7/root/etc/alternatives/ld
ln -s /root/compile/llvm_install/bin/ld.lld /opt/rh/devtoolset-7/root/etc/alternatives/ld

ls -l /opt/rh/devtoolset-7/root/etc/alternatives/ld
lrwxrwxrwx 1 root root 37 Dec 13 16:47 /opt/rh/devtoolset-7/root/etc/alternatives/ld -> /root/compile/llvm_install/bin/ld.lld
```

LLD leaves its name and version number to a `.comment` section in an output. If you are in doubt whether you are successfully using LLD or not, run `readelf --string-dump .comment <output-file>` and examine the output. If the string “Linker: LLD” is included in the output, you are using LLD.

```
$readelf --string-dump .comment demo

String dump of section '.comment':
  [     0]  Linker: LLD 12.0.0 (/root/compile/llvm-project/lld f76b7f22f085fbf9f2585923f7a3a0558d75964b)
  [    5d]  clang version 12.0.0 (/root/compile/llvm-project/clang f76b7f22f085fbf9f2585923f7a3a0558d75964b)
  [    be]  GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-4)
  [    ea]  GCC: (GNU) 7.3.1 20180303 (Red Hat 7.3.1-5)
```

refer:

* [LLD - The LLVM Linker](https://lld.llvm.org/#lld-the-llvm-linker)
* [LLD and GNU linker incompatibilities](https://maskray.me/blog/2020-12-19-lld-and-gnu-linker-incompatibilities)



# CCache (a fast C/C++ compiler cache)

`ccache` is a compiler cache. It speeds up recompilation by caching the result of previous compilations and detecting when the same compilation is being done again. Supported languages are C, C++, Objective-C and Objective-C++.

`ccache` has been carefully written to always produce exactly the same compiler output that you would get without the cache. The only way you should be able to tell that you are using `ccache` is the speed. Currently known exceptions to this goal are listed under CAVEATS. If you ever discover an undocumented case where ccache changes the output of your compiler, please let us know.

There are two ways to use ccache. You can either **prefix your compilation commands with ccache** or you can **let ccache masquerade as the compiler by creating a symbolic link (named as the compiler) to ccache**. The first method is most convenient if you just want to try out ccache or wish to use it for some specific projects. The second method is most useful for when you wish to use ccache for all your compilations.

To use the first method, just make sure that `ccache` is in your `PATH`.

To use the symlinks method, do something like this:

```
cp ccache /usr/local/bin/
ln -s ccache /usr/local/bin/gcc
ln -s ccache /usr/local/bin/g++
ln -s ccache /usr/local/bin/cc
ln -s ccache /usr/local/bin/c++
```

And so forth. This will work as long as the directory with symlinks comes before the path to the compiler (which is usually in `/usr/bin`). After installing you may wish to run “which gcc” to make sure that the correct link is being used.

* Features
  + Keeps statistics on hits/misses.
  + Automatic cache size management.
  + Can cache compilations that generate warnings.
  + Easy installation.
  + Low overhead.
  + Optionally compresses files in the cache to reduce disk space.

* Limitations
  + Only knows how to cache the compilation of a single C/C++/Objective-C/Objective-C++ file. Other types of compilations (multi-file compilation, linking, etc) will silently fall back to running the real compiler.
  + Only works with GCC and compilers that behave similar enough.
  + Some compiler flags are not supported. If such a flag is detected, ccache will silently fall back to running the real compiler.

More: man ccache


```
$which gcc
/usr/lib64/ccache/gcc
$ll -lh `which gcc`
lrwxrwxrwx 1 root root 16 3月   5 2021 /usr/lib64/ccache/gcc -> ../../bin/ccache
$ll -lh /usr/bin/ccache
-rwxr-xr-x 1 root root 135K 2月  19 2020 /usr/bin/ccache
```

## 禁用 ccache

要禁用本地的 ccache，可以采用以下几种方法之一：

* 临时禁用 ccache：

在构建命令之前，将 CCACHE_DISABLE 环境变量设置为 1：

``` bash
export CCACHE_DISABLE=1
```

然后运行构建命令（例如：make）。这将在当前会话中禁用 ccache。

* 永久禁用 ccache：

编辑 ~/.bashrc 或 ~/.bash_profile 文件，将以下行添加到文件末尾：

``` bash
export CCACHE_DISABLE=1
```

保存文件并重新启动终端。这将在所有新的终端会话中禁用 ccache。


* 使用原始的编译器而不是 ccache 包装器

在构建命令中，直接指定原始编译器的路径，而不是使用 ccache 包装器。例如，如果您的原始 Clang 编译器位于 /usr/bin/clang，则可以在构建命令中使用此路径。

``` bash
make CC=/usr/bin/clang
```

这将确保在构建过程中不使用 ccache。


* [https://ccache.dev/](https://ccache.dev/)
* [ccache 4.8.2](https://ccache.dev/manual/4.8.2.html)
* https://github.com/ccache/ccache/blob/master/README.md
* https://github.com/ccache/ccache/blob/master/doc/INSTALL.md
* [ccache and clang, part 3](http://peter.eisentraut.org/blog/2014/12/01/ccache-and-clang-part-3/)
* [How to use ccache selectively?](https://stackoverflow.com/questions/2693805/how-to-use-ccache-selectively)



# 编译效率对比

在8核CPU，16G内存机器，对比`gcc`, `clang`, `make`, `ninja`, `ld`, `lld`不同组合情况下的编译效率。

[测试代码](https://github.com/gerryyang/mac-utils/tree/master/tools/CMake/helloworld)


* 使用`CMake`生成`Unix Makefiles`，分别指定不同的`gcc`或者`clang`版本编译构建：(make.sh)
* 使用`CMake`生成`Ninja`，分别指定不同的`gcc`或者`clang`版本编译构建：(ninja.sh)
* 对比不同链接器`ld`，`lld`的性能差异

make.sh

``` bash
#!/bin/bash

## https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake

rm -rf build
mkdir -p build && cd build

COMPILE_MODE=gcc
#COMPILE_MODE=clang

## use `ccmake .` to use cmake gui
if [ $COMPILE_MODE == "gcc"  ]; then
        export CC=/opt/rh/devtoolset-7/root/usr/bin/cc
        export CXX=/opt/rh/devtoolset-7/root/usr/bin/c++

        cmake -G "Unix Makefiles" -DCMAKE_USER_MAKE_RULES_OVERRIDE=./GccOverrides.txt  ..

elif [ $COMPILE_MODE == "clang" ]; then

        #export CC=/root/compile/llvm_install/bin/clang
        #export CXX=/root/compile/llvm_install/bin/clang++

        export CC=/usr/local/bin/clang
        export CXX=/usr/local/bin/clang++

        cmake -G "Unix Makefiles" -fuse-ld=lld  -DCMAKE_USER_MAKE_RULES_OVERRIDE=./ClangOverrides.txt  ..
        #cmake -G "Unix Makefiles" -fuse-ld=lld  -DCMAKE_TOOLCHAIN_FILE=./ClangToolchains.cmake  ..

else
        echo "error: $COMPILE_MODE invalid"
        exit 1
fi

/usr/bin/time -f "real %e user %U sys %S" make -j8 VERBOSE=1

## LLD leaves its name and version number to a .comment section in an output
## readelf --string-dump .comment <output-file>

echo "have done"
```

ninja.sh

``` bash
#!/bin/bash

## https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake

rm -rf build
mkdir -p build && cd build

COMPILE_MODE=gcc
#COMPILE_MODE=clang

## use `ccmake .` to use cmake gui
if [ $COMPILE_MODE == "gcc"  ]; then
        export CC=/opt/rh/devtoolset-7/root/usr/bin/cc
        export CXX=/opt/rh/devtoolset-7/root/usr/bin/c++

        cmake -G "Ninja" -DCMAKE_USER_MAKE_RULES_OVERRIDE=./GccOverrides.txt  ..

elif [ $COMPILE_MODE == "clang" ]; then

        #export CC=/root/compile/llvm_install/bin/clang
        #export CXX=/root/compile/llvm_install/bin/clang++

        export CC=/usr/local/bin/clang
        export CXX=/usr/local/bin/clang++

        cmake -G "Ninja" -fuse-ld=lld  -DCMAKE_USER_MAKE_RULES_OVERRIDE=./ClangOverrides.txt  ..
        #cmake -G "Ninja" -fuse-ld=lld  -DCMAKE_TOOLCHAIN_FILE=./ClangToolchains.cmake  ..

else
        echo "error: $COMPILE_MODE invalid"
        exit 1
fi

/usr/bin/time -f "real %e user %U sys %S" ninja -j8 -v

## LLD leaves its name and version number to a .comment section in an output
## readelf --string-dump .comment <output-file>

echo "have done"
```


测试结果：

clang12 优于 gcc4.8/7/9，ninja 优于 make，lld 优于 ld。

| Case                  | Time   |
| --------------------- | ------ |
| gcc7 + make + ld      | 25.7s  |
| clang12 + make + ld   | 5.2s   |
| gcc7 + ninja + ld     | 22s    |
| clang12 + ninja + ld  | 4.7s   |
| gcc7 + make + lld     | 17.8s  |
| clang12 + make + lld  | 4.82s  |
| gcc7 + ninja + lld    | 18.34s |
| clang12 + ninja + lld | 4.15s  |
| gcc9 + make + lld     | 10.03s |
| gcc9 + ninja + lld    | 7.90s  |
| gcc4.8 + make + lld   | 8.93s  |
| gcc4.8 + ninja + lld  | 8.30s  |

# Mixing Clang with GCC

## [Can Clang compile code with GCC compiled .a libs?](https://stackoverflow.com/questions/20875924/can-clang-compile-code-with-gcc-compiled-a-libs)

I have my project currently compiling under `gcc`. It uses Boost, ZeroMQ as static `.a` libraries and some `.so` libraries like SDL. I want to go `clang` all the way but not right now. I wonder if it is possible to compile code that uses `.a` and `.so` libraries that were compiled under gcc with clang?

Answers:

Yes, you usually can use `clang` with `GCC` compiled libraries (and vice versa, use `gcc` with `CLANG` compiled libraries), because in fact it is not **compilation**(编译) but **linking**(链接) which is relevant. You might be unlucky and get unpleasant suprises.

You could in principle have some dependencies on the version of `libstdc++` used to link the relevant libraries (if they are coded in C++). Actually, that usually does not matter much.

In C++, [name mangling](http://en.wikipedia.org/wiki/Name_mangling) might in theory be an issue (there might be some corner cases, even incompatibilities between two different versions of `g++`). Again, in practice it is usually not an issue.

So usually you can mix `CLANG` (even different but close versions of it) with `GCC` **but you may have unpleasant surprises**. **What should be expected from any C++ compiler (be it `CLANG` or `GCC`) is just to be able to compile and link an entire software (and all libraries) together using the same compiler and version (and that includes the same [C++ standard library](https://en.wikipedia.org/wiki/C%2B%2B_Standard_Library) implementation). This is why upgrading a compiler in a distribution is a lot of work**: the distribution makers have to ensure that all the packages compile well (and they do get surprises!).

Beware that the version of `libstdc++` does matter. Both `Clang` & `GCC` communities **work hard to make its ABI compatible for compiler upgrades, but there are subtle corner cases**. Read the documentation of your particular and specific C++ standard library implementation. These corner cases could explain mysterious crashes when using a good C++ library binary (compiled with `GCC 5`) in your code compiled with `GCC 8`. The bug is not in the library, **but the ABI evolved incompatibly**.

Another answers:

At least for [Crypto++ library](http://www.cryptopp.com/) this does not work (verified :-( ). So for c++ code it is less likely to work, while pure c code would probably link OK.

The solution appears to be: **if you need to compile C++ code with clang, and link it to a gcc-compiled library**, use `clang++ -stdlib=libstdc++`. The linking is successful, and the resulting binary runs correctly.

CAVEAT(注意): It does not seem to work the other way: even though you can build a library compiled with "clang++ -stdlib=libstdc++" and link gcc-compiled code with it, this code will crash with SEGV. So far I found the only way to link with a clang-compiled library is compiling your code with clang, not gcc.

## [gcc vs clang common library issue](https://stackoverflow.com/questions/49715462/gcc-vs-clang-common-library-issue?noredirect=1&lq=1)

I have two applications, one compiled with gcc(c++) and another compiled with clang++. I am to use common shared boost library for both the applications. My question is whether to compile boost shared library using clang compiler or gcc compiler. Can I use boost library compiled with gcc in my application that is being compiled using clang?

Answers:

`g++` and `clang++` are compatible as compilers (because they both follow the `Itanium ABI`), **but they may come with incompatible standard library implementations**.

`g++` comes with a standard library implementation called `libstdc++`. You can direct g++ to use a different implementation but this is not exactly trivial.

`clang++` sometimes comes without a standard library implementation of its own (and is configured to use implementation provided by `g++`), and sometimes comes with an implementation called `libc++`. One can easily switch `clang++` to use either `libc++` or `libstdc++` with a single command line option.

**So your question boils down to what standard library implementation(s) your applications use.** If they use the same implementation, you need to build Boost with that implementation (and either compiler). If they use different implementations, you need two separate builds of Boost.

Mixing components built against different standard library implementations in the same application can sometimes be done, but is not straightforward, entails a lot of restrictions, and with things like boost is either not feasible or downright impossible.


# Tools

## include-what-you-use (iwyu)

https://github.com/include-what-you-use/include-what-you-use



# Refer

* https://en.wikipedia.org/wiki/Single_compilation_unit (编译单元)
* [Switching between GCC and Clang/LLVM using CMake](https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake)




