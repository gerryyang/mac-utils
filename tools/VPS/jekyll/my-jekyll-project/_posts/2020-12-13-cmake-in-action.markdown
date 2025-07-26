---
layout: post
title:  "CMake in Action"
date:   2020-12-13 11:30:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}

`CMake` is an open-source, cross-platform family of tools designed to build, test and package software. CMake is used to control the software compilation process using simple platform and compiler independent configuration files, and generate native `makefiles` and workspaces that can be used in the compiler environment of your choice. The suite of CMake tools were created by Kitware in response to the need for a powerful, cross-platform build environment for open-source projects such as ITK and VTK.


# Install CMake

## release 版本

```
#!/bin/bash

## for Linux

export CMAKE_INSTALL_DIR=$HOME/cmake
export PATH=$CMAKE_INSTALL_DIR:$PATH
mkdir -p $CMAKE_INSTALL_DIR
cd $CMAKE_INSTALL_DIR

wget -q -O cmake-linux.sh https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0-Linux-x86_64.sh
sh cmake-linux.sh -- --skip-license --prefix=$CMAKE_INSTALL_DIR
rm cmake-linux.sh
$CMAKE_INSTALL_DIR/bin/cmake --version
```

完整的安装脚本：先使用本地的，如果没有则下载。

``` bash
#!/bin/bash

CUR_DIR=$(dirname $(readlink -f $0))

CMAKE_DATA_DIR=cmake-3.17.0-Linux-x86_64
CMAKE_INSTALL_DIR_PREFIX=/usr/local/bin
export CMAKE_INSTALL_DIR="$CMAKE_INSTALL_DIR_PREFIX/$CMAKE_DATA_DIR"
export PATH="$CMAKE_INSTALL_DIR/bin":$PATH

# Specify the cmake version to install
CMAKE_VERSION="3.17.0"
CMAKE_DOWNLOAD_URL="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-3.17.0-Linux-x86_64.sh"
LOCAL_CMAKE_DIR="$CUR_DIR/$CMAKE_DATA_DIR"

function InstallCMake()
{
    echo "Installing cmake version $CMAKE_VERSION..."

    if [[ -d $LOCAL_CMAKE_DIR ]]; then
        echo "Using local cmake dir: $LOCAL_CMAKE_DIR"
        cp -r $LOCAL_CMAKE_DIR "$CMAKE_INSTALL_DIR_PREFIX/$CMAKE_DATA_DIR"

    elif wget -q -O "cmake-linux.sh" --tries=1 "$CMAKE_DOWNLOAD_URL"; then
        echo "Downloaded cmake from $CMAKE_DOWNLOAD_URL"

        sh cmake-linux.sh -- --skip-license --prefix=$CMAKE_INSTALL_DIR_PREFIX
        #rm cmake-linux.sh

    else
        echo "Error: Could not download cmake"
        exit 1
    fi

    echo "cmake installation completed, version:"
    echo "-----------------------------------------"
    echo "$(cmake --version)"
    echo "-----------------------------------------"
}

# Check if cmake is already installed
if command -v bazel &> /dev/null; then
    echo "cmake is already installed, $(cmake --version | head -n1)"

    CUR_CMAKE_VERSION=`cmake --version | head -n1 | awk '{print $3}' | awk -F'.' '{printf $1$2}'`
    if [[ "$CUR_CMAKE_VERSION" -lt 314 ]]; then
        echo "cmake3.14 is required, please use $CUR_DIR/tools/cmake-build/cmake-install/install_cmake.sh to upgrade cmake firstly"

        InstallCMake
    fi
else
    InstallCMake
fi
```

## 源码编译

```
yum info cmake
yum remove cmake -y

wget https://cmake.org/files/v3.17/cmake-3.17.5.tar.gz
tar zxvf cmake-3.17.5.tar.gz
cd cmake-3.17.5

./bootstrap --prefix=/usr/local
make -j16
make install

# 备份老版本，软链新版本
mv /usr/bin/cmake /usr/bin/cmake2.8.11 && ln -s /usr/local/bin/cmake /usr/bin/

/usr/bin/cmake --version
```


# CMake Introduction

The cmake executable is the command-line interface of the cross-platform buildsystem generator CMake.

官方 manual：

* [cmake(1)](https://cmake.org/cmake/help/latest/manual/cmake.1.html)
* [cmake-language(7)](https://cmake.org/cmake/help/latest/manual/cmake-language.7.html#manual:cmake-language(7))
* [cmake-buildsystem(7)](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#manual:cmake-buildsystem(7))
* [cmake-generators(7)](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#manual:cmake-generators(7))

```
Generate a Project Buildsystem
 cmake [<options>] <path-to-source>
 cmake [<options>] <path-to-existing-build>
 cmake [<options>] -S <path-to-source> -B <path-to-build>

Build a Project
 cmake --build <dir> [<options>] [-- <build-tool-options>]

Install a Project
 cmake --install <dir> [<options>]

Open a Project
 cmake --open <dir>

Run a Script
 cmake [{-D <var>=<value>}...] -P <cmake-script-file>

Run a Command-Line Tool
 cmake -E <command> [<options>]

Run the Find-Package Tool
 cmake --find-package [<options>]

View Help
 cmake --help[-<topic>]
```

## CMake Buildsystems

A buildsystem describes how to build a project's executables and libraries from its source code using a build tool to automate the process. For example, a buildsystem may be a `Makefile` for use with a command-line `make` tool or a project file for an Integrated Development Environment (IDE). In order to avoid maintaining multiple such buildsystems, a project may specify its buildsystem abstractly using files written in the `CMake language`. From these files CMake generates a preferred buildsystem locally for each user through a backend called a `generator`.

To generate a buildsystem with CMake, the following must be selected:

* Source Tree(源代码目录)
  + The top-level directory(最顶层的目录) containing source files provided by the project. The project specifies its buildsystem using files as described in the [cmake-language(7)](https://cmake.org/cmake/help/latest/manual/cmake-language.7.html#manual:cmake-language(7)) manual, starting with a top-level file named `CMakeLists.txt`. These files specify build targets and their dependencies as described in the [cmake-buildsystem(7)](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#manual:cmake-buildsystem(7)) manual.

* Build Tree(构建目录)
  + The top-level directory in which buildsystem files and build output artifacts (e.g. executables and libraries) are to be stored. CMake will write a `CMakeCache.txt` file to identify the directory as a build tree and store persistent information such as buildsystem configuration options.
  + To maintain a pristine(全新的) source tree, perform an **out-of-source build(外部构建，建议此方式)** by using a separate dedicated build tree. An **in-source build(内部构建，不建议此方式)** in which the build tree is placed in the same directory as the source tree is also supported, but discouraged.

* Generator(生成器，例如：Unix Makefiles, Ninja)
  + This chooses the kind of buildsystem to generate. See the [cmake-generators(7)](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#manual:cmake-generators(7)) manual for documentation of all generators. Run `cmake --help` to see a list of generators available locally. Optionally use the `-G` option below to specify a generator, or simply accept the default CMake chooses for the current platform.
  + When using one of [the Command-Line Build Tool Generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#command-line-build-tool-generators) CMake expects that the environment needed by the compiler toolchain is already configured in the shell. When using one of the IDE Build Tool Generators, no particular environment is needed.

```
$cmake --help

The following generators are available on this platform (* marks default):
* Unix Makefiles               = Generates standard UNIX makefiles.
  Green Hills MULTI            = Generates Green Hills MULTI files
                                 (experimental, work-in-progress).
  Ninja                        = Generates build.ninja files.
  Ninja Multi-Config           = Generates build-<Config>.ninja files.
  Watcom WMake                 = Generates Watcom WMake makefiles.
  CodeBlocks - Ninja           = Generates CodeBlocks project files.
  CodeBlocks - Unix Makefiles  = Generates CodeBlocks project files.
  CodeLite - Ninja             = Generates CodeLite project files.
  CodeLite - Unix Makefiles    = Generates CodeLite project files.
  Sublime Text 2 - Ninja       = Generates Sublime Text 2 project files.
  Sublime Text 2 - Unix Makefiles
                               = Generates Sublime Text 2 project files.
  Kate - Ninja                 = Generates Kate project files.
  Kate - Unix Makefiles        = Generates Kate project files.
  Eclipse CDT4 - Ninja         = Generates Eclipse CDT 4.0 project files.
  Eclipse CDT4 - Unix Makefiles= Generates Eclipse CDT 4.0 project files.
```

## Generate a Project Buildsystem

Run CMake with one of the following command signatures to specify the source and build trees and generate a buildsystem:

* `cmake [<options>] <path-to-source>`

Uses the current working directory as the **build tree**, and `<path-to-source>` as the **source tree**. The specified path may be absolute or relative to the current working directory. The source tree must contain a `CMakeLists.txt` file and must not contain a `CMakeCache.txt` file because the latter identifies an existing build tree. For example:

```
$ mkdir build; cd build
$ cmake ../src
```

* `cmake [<options>] <path-to-existing-build>`

Uses `<path-to-existing-build>` as the **build tree**, and loads the path to the source tree from its `CMakeCache.txt` file, which must have already been generated by a previous run of CMake. The specified path may be absolute or relative to the current working directory. For example:

```
$ cd build
$ cmake .
```

* `cmake [<options>] -S <path-to-source> -B <path-to-build>`

Uses `<path-to-build>` as the **build tree** and `<path-to-source>` as the **source tree**. The specified paths may be absolute or relative to the current working directory. The source tree must contain a `CMakeLists.txt` file. The build tree will be created automatically if it does not already exist. For example:

```
# -S <path-to-source> : Path to root directory of the CMake project to build.
# -B <path-to-build> : Path to directory which CMake will use as the root of build directory. If the directory doesn't already exist CMake will make it.

$ cmake -S src -B build
```

In all cases the `<options>` may be zero or more, refer [Options](https://cmake.org/cmake/help/latest/manual/cmake.1.html#options).

After generating a buildsystem one may use the corresponding native build tool to build the project. For example, after using the **Unix Makefiles** generator one may run **make** directly:

```
$ make
$ make install
```

Alternatively, one may use cmake to [Build a Project](https://cmake.org/cmake/help/latest/manual/cmake.1.html#build-a-project) by automatically choosing and invoking the appropriate native build tool.

## Run a Command-Line Tool

CMake provides builtin command-line tools through the signature. Run `cmake -E` or `cmake -E help` for a summary of commands.

```
cmake -E <command> [<options>]
```


# CMake Helloworld

* `cmake_minimum_required`：cmake 的最低版本要求，会根据指定的版本隐式调用 `cmake_policy(VERSION)`
* `project`：指定项目的名称
* `set`：设置普通变量，缓存变量或环境变量
* `add_subdirectory`：添加 build 的子目录，第一个参数 source_dir 指定的子目录包含了 CMakeLists.txt 和代码，通常为相对路径
* `add_executable`：使用列出的源文件构建可执行文件
* `include_directories`：添加多个头文件搜索路径，路径之间用空格分隔；在 include 的时候就不需要使用相对路径了
* `aux_source_directory`：在目录中查找所有源文件，并将这些源文件存储在变量 SOURCE_DIR 中；需要注意这个指令不会递归包含子目录
* `link_directories`：指定静态库或动态库的搜索路径
* `target_link_libraries`：将指定的静态库连接到可执行文件上，singleton 和 libsingleton.a 两种形式等价
* `set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)`：命令将生成的二进制文件放到了 bin 目录下，注意这里的 bin 目录是使用 cmake 进行构建的目录（PROJECT_BINARY_DIR，即build/bin）
* `add_library`：来使用指定的源文件生成库文件，`add_library(calculator_static STATIC ${static_lib_source_file})` 第一个参数指定库名字。

Note:

* cmake默认使用彩色精简的输出方式，若需要输出详细的编译过程有两种方法
  + 通过参数`make VERBOSE = 1`
  + 在CMakeLists.txt中设置`set(CMAKE_VERBOSE_MAKEFILE on)`
* cmake的指令是不区分大小写的，写作`CMAKE_MINIMUM_REQUIRED`或`cmake_minimum_required`，甚至是`cmAkE_mInImUm_rEquIrEd`（不建议）都是可以的
* 设置编译选项，例如，`SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")`
* 设置链接选项，例如，`SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")`

* 在使用`set`指令指定`CMAKE_CXX_FLAGS`的时候通过空格来分隔多个编译选项，生成的`CMAKE_CXX_FLAGS`字符串是 `-g;-Wall`，需要用字符串替换将分号替换为空格。如果不想替换，可以通过引号的方式一次设置多个选项。

```
# 方式1
set(CMAKE_CXX_FLAGS -g -Wall)

message(STATUS "CMAKE_CXX_FLAGS: " "${CMAKE_CXX_FLAGS}")
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: " "${CMAKE_CXX_FLAGS}")

# 方式2
set(CMAKE_CXX_FLAGS "-g -Wall")
message(STATUS "CMAKE_CXX_FLAGS: " "${CMAKE_CXX_FLAGS}")

# 方式3
set(CMAKE_CXX_FLAGS -g)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
message(STATUS "CMAKE_CXX_FLAGS: " "${CMAKE_CXX_FLAGS}")
```

* `message`可以在构建的过程中向 stdout 输出一些信息
* 类似于 bash 脚本，在 CMakeLists.txt 中输出变量时要使用`${CMAKE_CXX_FLAGS}`的形式，而不能直接使用`CMAKE_CXX_FLAGS`
* 编辑好 CMakeLists.txt 之后，可以新建一个 build 目录，并在 build 目录下使用 cmake 来进行构建，构建成功的话再使用 make 来进行编译和链接，最终得到可执行文件
* 除了直接引用外部的静态库，cmake 还可以先将源文件编译成静态库之后再进行构建
	+ `target_link_libraries(Exp2 libcalculator.a)`
	+ `target_link_libraries(Exp2 calculator)`
* 使用`#[[ ... ]]`或`#`注释
* 可通过命令行`cmake -j8 .. -DCMAKE_BUILD_TYPE=Debug`的方式编译的版本，cmake会在`CMAKE_CXX_FLAGS`之后添加对应编译版本额外的选项。注意：后面的选项优先级更高。也可以通过修改`CMAKE_CXX_FLAGS_DEBUG`或`CMAKE_CXX_FLAGS_RELEASE`变量，来修改默认的编译版本选项。
  + 在命令行指定`-DCMAKE_BUILD_TYPE=Release`或在CMakeFile文件中设置`set(CMAKE_BUILD_TYPE Release)`时，修改`set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Wall -O2")`才有效
  + `CMAKE_CXX_FLAGS_DEBUG`默认选项：`-O0 -g`
  + `CMAKE_CXX_FLAGS_RELEASE`默认选项：`-O3 -DNDEBUG`

* 在Release版本会添加`-DNDEBUG`编译选项，用于消除assert断言。refer: https://en.cppreference.com/w/cpp/error/assert

* Do not overwrite `CMAKE_C_COMPILER` or `CMAKE_CXX_COMPILER`, but export `CC` or `CXX` before calling cmake. [How to specify new GCC path for CMake](https://stackoverflow.com/questions/17275348/how-to-specify-new-gcc-path-for-cmake)

```
# 不建议
set(CMAKE_C_COMPILER /usr/bin/clang CACHE PATH "" FORCE)
set(CMAKE_CXX_COMPILER /usr/bin/clang++ CACHE PATH "" FORCE)

# 推荐
export CC=/usr/local/bin/gcc
export CXX=/usr/local/bin/g++
cmake /path/to/your/project
make
```

# Using Dependencies Guide

## Introduction

Projects will frequently depend on other projects, assets, and artifacts. CMake provides a number of ways to incorporate such things into the build. Projects and users have the flexibility to choose between methods that best suit their needs.

The primary methods of bringing dependencies into the build are the [find_package()](https://cmake.org/cmake/help/latest/command/find_package.html#command:find_package) command and the [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html#module:FetchContent) module.


## Using Pre-built Packages With find_package()

A package needed by the project may already be built and available at some location on the user's system. That package might have also been built by CMake, or it could have used a different build system entirely. It might even just be a collection of files that didn't need to be built at all. CMake provides the `find_package()` command for these scenarios. It searches well-known locations, along with additional hints and paths provided by the project or user. It also supports package components and packages being optional. Result variables are provided to allow the project to customize its own behavior according to whether the package or specific components were found.

In most cases, projects should generally use the [Basic Signature](https://cmake.org/cmake/help/latest/command/find_package.html#basic-signature). Most of the time, this will involve just the package name, maybe a version constraint, and the `REQUIRED` keyword if the dependency is not optional. A set of package components may also be specified.

Examples of `find_package()` basic signature

```
find_package(Catch2)
find_package(GTest REQUIRED)
find_package(Boost 1.79 COMPONENTS date_time)
```

### Config-file packages

The preferred way for a third party to provide executables, libraries, headers, and other files for use with CMake is to provide [config files](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages). These are text files shipped with the package, which define CMake targets, variables, commands, and so on. The config file is an ordinary CMake script, which is read in by the `find_package()` command.

The config files can usually be found in a directory whose name matches the pattern `lib/cmake/<PackageName>`, although they may be in other locations instead (see [Config Mode Search Procedure](https://cmake.org/cmake/help/latest/command/find_package.html#search-procedure)). The `<PackageName>` is usually the first argument to the `find_package()` command, and it may even be the only argument. Alternative names can also be specified with the `NAMES` option:

Providing alternative names when finding a package

```
find_package(SomeThing
  NAMES
    SameThingOtherName   # Another name for the package
    SomeThing            # Also still look for its canonical name
)
```

The config file must be named either `<PackageName>Config.cmake` or `<LowercasePackageName>-config.cmake` (the former is used for the remainder of this guide, but both are supported). This file is the entry point to the package for CMake.

If the `<PackageName>Config.cmake` file is found and any version constraint is satisfied, the `find_package()` command considers the package to be found, and the entire package is assumed to be complete as designed.

### Find Module Files

See https://cmake.org/cmake/help/latest/guide/using-dependencies/index.html#find-module-files


## Downloading And Building From Source With FetchContent

Dependencies do not necessarily have to be pre-built in order to use them with CMake. They can be built from sources as part of the main project. The [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html#module:FetchContent) module provides functionality to download content (typically sources, but can be anything) and add it to the main project if the dependency also uses CMake. The dependency's sources will be built along with the rest of the project, just as though the sources were part of the project's own sources.

The general pattern is that the project should first declare all the dependencies it wants to use, then ask for them to be made available. The following demonstrates the principle (see [Examples](https://cmake.org/cmake/help/latest/module/FetchContent.html#fetch-content-examples) for more):

```
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        703bd9caab50b139428cea1aaff9974ebee5742e # release-1.10.0
)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        de6fe184a9ac1a06895cdd1c9b437f0a0bdf14ad # v2.13.4
)
FetchContent_MakeAvailable(googletest Catch2)
```

Various download methods are supported, including downloading and extracting archives from a URL (a range of archive formats are supported), and a number of repository formats including Git, Subversion, and Mercurial. Custom download, update, and patch commands can also be used to support arbitrary use cases.

When a dependency is added to the project with `FetchContent`, the project links to the dependency's targets just like any other target from the project. If the dependency provides namespaced targets of the form `SomePrefix::ThingName`, the project should link to those rather than to any non-namespaced targets. See the next section for why this is recommended.

Not all dependencies can be brought into the project this way. Some dependencies define targets whose names clash with other targets from the project or other dependencies. Concrete executable and library targets created by `add_executable()` and `add_library()` are global, so each one must be unique across the whole build. If a dependency would add a clashing target name, it cannot be brought directly into the build with this method.


## FetchContent And find_package() Integration

```
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        703bd9caab50b139428cea1aaff9974ebee5742e # release-1.10.0
  FIND_PACKAGE_ARGS NAMES GTest
)
FetchContent_MakeAvailable(googletest)

add_executable(ThingUnitTest thing_ut.cpp)
target_link_libraries(ThingUnitTest GTest::gtest_main)
```

The above example calls `find_package(googletest NAMES GTest)` first. CMake provides a `FindGTest` module, so if that finds a `GTest` package installed somewhere, it will make it available, and the dependency will not be built from source. If no `GTest` package is found, it will be built from source. In either case, the `GTest::gtest_main` target is expected to be defined, so we link our unit test executable to that target.


* https://cmake.org/cmake/help/latest/guide/using-dependencies/index.html#guide:Using%20Dependencies%20Guide



# Command

## cmake_minimum_required

Require a minimum version of cmake.

```
cmake_minimum_required(VERSION <min>[...<policy_max>] [FATAL_ERROR])
```

The `cmake_minimum_required(VERSION)` command implicitly invokes the `cmake_policy(VERSION)` command to specify that the current project code is written for the given range of CMake versions.

https://cmake.org/cmake/help/latest/command/cmake_minimum_required.html


## project

Set the name of the project.

```
project(<PROJECT-NAME> [<language-name>...])
project(<PROJECT-NAME>
        [VERSION <major>[.<minor>[.<patch>[.<tweak>]]]]
        [DESCRIPTION <project-description-string>]
        [HOMEPAGE_URL <url-string>]
        [LANGUAGES <language-name>...])
```

https://cmake.org/cmake/help/latest/command/project.html

## include

Load and run CMake code from a file or module. If `OPTIONAL` is present, then no error is raised if the file does not exist.

```
include(<file|module> [OPTIONAL] [RESULT_VARIABLE <var>]
                      [NO_POLICY_SCOPE])
```

https://cmake.org/cmake/help/latest/command/include.html

## target_include_directories

Add include directories to a target.

```
target_include_directories(<target> [SYSTEM] [AFTER|BEFORE]
  <INTERFACE|PUBLIC|PRIVATE> [items1...]
  [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])
```

Specifies include directories to use when compiling a given target. The named `<target>` must have been created by a command such as add_executable() or add_library() and must not be an ALIAS target.

https://cmake.org/cmake/help/latest/command/target_include_directories.html

## set

Set a normal, cache, or environment variable to a given value.

```
set(<variable> <value>... [PARENT_SCOPE])
```

https://cmake.org/cmake/help/latest/command/set.html

## unset

Unset a variable, cache variable, or environment variable. Removes the specified variable causing it to become undefined.

```
unset(<variable> [CACHE | PARENT_SCOPE])
```

https://cmake.org/cmake/help/v3.0/command/unset.html

## message

Log a message.

```
# General messages
message([<mode>] "message text" ...)

# Reporting checks
message(<checkState> "message text" ...)
```

https://cmake.org/cmake/help/latest/command/message.html

## option

Provides an option for the user to select as `ON` or `OFF`. If no initial `<value>` is provided, `OFF` is used. If `<variable>` is already set as a normal or cache variable, then the command does nothing (see policy [CMP0077](https://cmake.org/cmake/help/latest/policy/CMP0077.html#policy:CMP0077)).

```
option(<variable> "<help_text>" [value])
```

```
option(WITH_GLOG "using glog" ON)

if (WITH_GLOG)
  add_compile_definitions(WITH_GLOG)
endif()
```

https://cmake.org/cmake/help/latest/command/option.html

## if

Conditionally execute a group of commands.

```
if(<condition>)
  <commands>
elseif(<condition>) # optional block, can be repeated
  <commands>
else()              # optional block
  <commands>
endif()
```

Comparisons:

```
if(<variable|string> MATCHES <regex>)             # True if the given string or variable's value matches the given regular expression.

if(<variable|string> LESS <variable|string>       #  True if the given string or variable's value is a valid number and less than that on the right.

if(<variable|string> GREATER <variable|string>)   # True if the given string or variable's value is a valid number and greater than that on the right.

if(<variable|string> EQUAL <variable|string>)     # True if the given string or variable's value is a valid number and equal to that on the right.

if(<variable|string> STREQUAL <variable|string>)  # True if the given string or variable's value is lexicographically equal to the string or variable on the right.
```

Logic Operators:

```
if(NOT <condition>)      # True if the condition is not true.

if(<cond1> AND <cond2>)  # True if both conditions would be considered true individually.

if(<cond1> OR <cond2>)   # True if either condition would be considered true individually.
```


https://cmake.org/cmake/help/latest/command/if.html

## file

File manipulation command. This command is dedicated to file and path manipulation requiring access to the filesystem.

```
# Filesystem

file({GLOB | GLOB_RECURSE} <out-var> [...] [<globbing-expr>...])

file(GLOB <variable>
     [LIST_DIRECTORIES true|false] [RELATIVE <path>] [CONFIGURE_DEPENDS]
     [<globbing-expressions>...])

file(GLOB_RECURSE <variable> [FOLLOW_SYMLINKS]
     [LIST_DIRECTORIES true|false] [RELATIVE <path>] [CONFIGURE_DEPENDS]
     [<globbing-expressions>...])
```

https://cmake.org/cmake/help/latest/command/file.html

## foreach

Evaluate a group of commands for each value in a list.

```
foreach(<loop_var> <items>)
  <commands>
endforeach()
```

https://cmake.org/cmake/help/latest/command/foreach.html

## aux_source_directory

Find all source files in a directory.

```
aux_source_directory(<dir> <variable>)
```

https://cmake.org/cmake/help/latest/command/aux_source_directory.html

## add_subdirectory

Add a subdirectory to the build.

```
add_subdirectory(source_dir [binary_dir] [EXCLUDE_FROM_ALL])
```

https://cmake.org/cmake/help/latest/command/add_subdirectory.html

## include_directories

Add include directories to the build. Add the given directories to those the compiler uses to search for include files. Relative paths are interpreted as relative to the current source directory.

```
include_directories([AFTER|BEFORE] [SYSTEM] dir1 [dir2 ...])
```

https://cmake.org/cmake/help/latest/command/include_directories.html

## link_directories

Add directories in which the linker will look for libraries.

```
link_directories([AFTER|BEFORE] directory1 [directory2 ...])
```

https://cmake.org/cmake/help/latest/command/link_directories.html


## add_definitions

Add -D define flags to the compilation of source files.

```
add_definitions(-DFOO -DBAR ...)
```

https://cmake.org/cmake/help/latest/command/add_definitions.html

##  add_custom_command

主要作用：

* 生成文件。添加定制命令来生成文件。
* 构建事件。为某个目标(如库或可执行程序)添加一个定制命令。
  + 这种定制命令可以设置在，构建这个目标过程中的某些时机。即，这种场景可以在目标构建的过程中，添加一些额外执行的命令。这些命令本身将会成为该目标的一部分。注意，仅在目标本身被构建过程才会执行。如果该目标已经构建，命令将不会执行。
* 如果使用 OUTPUT 参数，需要在目标的构建中指定依赖于该 OUTPUT
* 如果使用 TARGET 参数，直接指定目标就可以了

| 参数	| 含义
| -- | --
| PRE_BUILD	| 在目标中执行任何其他规则之前运行
| PRE_LINK	| 在编译源代码之后，链接二进制文件或库文件之前运行
| POST_BUILD	| 在目标内所有其他规则均已执行后运行

Add a custom build rule to the generated build system.

The first signature is for adding a custom command to produce an output:

```
add_custom_command(OUTPUT output1 [output2 ...]
                   COMMAND command1 [ARGS] [args1...]
                   [COMMAND command2 [ARGS] [args2...] ...]
                   [MAIN_DEPENDENCY depend]
                   [DEPENDS [depends...]]
                   [BYPRODUCTS [files...]]
                   [IMPLICIT_DEPENDS <lang1> depend1
                                    [<lang2> depend2] ...]
                   [WORKING_DIRECTORY dir]
                   [COMMENT comment]
                   [DEPFILE depfile]
                   [JOB_POOL job_pool]
                   [VERBATIM] [APPEND] [USES_TERMINAL]
                   [COMMAND_EXPAND_LISTS])

```

* OUTPUT：指定命令预期产生的输出文件。如果输出文件的名称是相对路径，即相对于当前的构建的源目录路径；
* COMMAND：指定要在构建时执行的命令行；
* DEPENDS：指定命令所依赖的文件；
* COMMENT：在构建时执行命令之前显示给定消息；
* WORKING_DIRECTORY：使用给定的当前工作目录执行命令。如果它是相对路径，它将相对于对应于当前源目录的构建树目录；
* DEPFILE：为生成器指定一个.d depfile .d文件保存通常由自定义命令本身发出的依赖关系；
* MAIN_DEPENDENCY：指定命令的主要输入源文件；
* BYPRODUCTS：指定命令预期产生的文件；

例子：

```
cmake_minimum_required(VERSION 3.10)

project(test)

# Usage: cmake -E <command> [arguments...]
# refer: https://cmake.org/cmake/help/latest/manual/cmake.1.html#run-a-command-line-tool
add_custom_command(OUTPUT SOME_RESULT
                  COMMAND ${CMAKE_COMMAND} -E echo "Do something"
)

add_custom_target(TaskA ALL
                 DEPENDS SOME_RESULT
)
```

The second signature adds a custom command to a target such as a library or executable. This is useful for performing an operation before or after building the target. The command becomes part of the target and will only execute when the target itself is built. If the target is already built, the command will not execute.

```
add_custom_command(TARGET <target>
                   PRE_BUILD | PRE_LINK | POST_BUILD
                   COMMAND command1 [ARGS] [args1...]
                   [COMMAND command2 [ARGS] [args2...] ...]
                   [BYPRODUCTS [files...]]
                   [WORKING_DIRECTORY dir]
                   [COMMENT comment]
                   [VERBATIM] [USES_TERMINAL]
                   [COMMAND_EXPAND_LISTS])
```

例子：

```
cmake_minimum_required(VERSION 3.10)

project(test)

# Note, should be before add_custom_command
add_custom_target(TaskA)

# Usage: cmake -E <command> [arguments...]
# refer: https://cmake.org/cmake/help/latest/manual/cmake.1.html#run-a-command-line-tool
add_custom_command(TARGET TaskA
                  PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E echo "Do something"
)
```

* https://cmake.org/cmake/help/latest/command/add_custom_command.html
* [cmake：添加自定义操作](https://zhuanlan.zhihu.com/p/95771200)

## add_custom_target

增加一个没有输出的目标，使得它总是被构建。

Add a target with no output so it will always be built. Adds a target with the given name that executes the given commands. **The target has no output file and is always considered out of date even if the commands try to create a file with the name of the target.** Use the add_custom_command() command to generate a file with dependencies.

```
add_custom_target(Name [ALL] [command1 [args1...]]
                  [COMMAND command2 [args2...] ...]
                  [DEPENDS depend depend depend ... ]
                  [BYPRODUCTS [files...]]
                  [WORKING_DIRECTORY dir]
                  [COMMENT comment]
                  [JOB_POOL job_pool]
                  [VERBATIM] [USES_TERMINAL]
                  [COMMAND_EXPAND_LISTS]
                  [SOURCES src1 [src2...]])
```

* ALL：表明该目标会被添加到默认的构建目标，使得它每次都被运行；
* COMMAND：指定要在构建时执行的命令行；
* DEPENDS：指定命令所依赖的文件；
* COMMENT：在构建时执行命令之前显示给定消息；
* WORKING_DIRECTORY：使用给定的当前工作目录执行命令。如果它是相对路径，它将相对于对应于当前源目录的构建树目录；
* BYPRODUCTS：指定命令预期产生的文件；

例子：

```
cmake_minimum_required(VERSION 3.10)

project(test)

# Usage: cmake -E <command> [arguments...]
# refer: https://cmake.org/cmake/help/latest/manual/cmake.1.html#run-a-command-line-tool
add_custom_target(TaskA
                  COMMAND ${CMAKE_COMMAND} -E echo "Do TaskA"
                  COMMAND ${CMAKE_COMMAND} -E echo "Do TaskA over"

)
```

https://cmake.org/cmake/help/latest/command/add_custom_target.html

## add_dependencies

Add a dependency between top-level targets.

Makes a top-level <target> depend on other top-level targets to ensure that they build before <target> does. A top-level target is one created by one of the add_executable(), add_library(), or add_custom_target() commands (but not targets generated by CMake like install).

```
add_dependencies(<target> [<target-dependency>]...)
```

https://cmake.org/cmake/help/latest/command/add_dependencies.html

## add_executable

Add an executable to the project using the specified source files.

```
add_executable(<name> [WIN32] [MACOSX_BUNDLE]
               [EXCLUDE_FROM_ALL]
               [source1] [source2 ...])
```

https://cmake.org/cmake/help/latest/command/add_executable.html

## add_library

Add a library to the project using the specified source files.

```
add_library(<name> [STATIC | SHARED | MODULE]
            [EXCLUDE_FROM_ALL]
            [<source>...])
```

https://cmake.org/cmake/help/latest/command/add_library.html

## target_link_libraries

Specify libraries or flags to use when linking a given target and/or its dependents.

```
target_link_libraries(<target> ... <item>... ...)
```

https://cmake.org/cmake/help/latest/command/target_link_libraries.html

## enable_language

Enable a language (CXX/C/OBJC/OBJCXX/Fortran/etc)

https://cmake.org/cmake/help/latest/command/enable_language.html


## macro

Start recording a macro for later invocation as a command

```
macro(<name> [<arg1> ...])
  <commands>
endmacro()
```

例子：

```
macro(CONFIG Key Value)
    IF(NOT DEFINED ${Key})
        SET(${Key} ${Value})
    ENDIF()
endmacro()

CONFIG(PROJ_BUILD_TYPE "debug")
```

https://cmake.org/cmake/help/latest/command/macro.html

## function

Start recording a function for later invocation as a command.

Defines a function named `<name>` that takes arguments named `<arg1>, ...` The `<commands>` in the function definition are recorded; they are not executed until the function is invoked.

```
function(<name> [<arg1> ...])
  <commands>
endfunction()
```

https://cmake.org/cmake/help/latest/command/function.html

## string

String operations.

```
# Search and Replace

string(REPLACE <match-string> <replace-string> <out-var> <input>...)
```

https://cmake.org/cmake/help/latest/command/string.html

## set_property

Set a named property in a given scope.

```
# Run command and display elapsed time for global
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
```

https://cmake.org/cmake/help/latest/command/set_property.html


## install

Specify rules to run at install time. This command generates installation rules for a project. Install rules specified by calls to the install() command within a source directory are executed in order during installation.

```
install(TARGETS <target>... [...])
install(IMPORTED_RUNTIME_ARTIFACTS <target>... [...])
install({FILES | PROGRAMS} <file>... [...])
install(DIRECTORY <dir>... [...])
install(SCRIPT <file> [...])
install(CODE <code> [...])
install(EXPORT <export-name> [...])
install(RUNTIME_DEPENDENCY_SET <set-name> [...])
```

单独 install 某个 target：

```
INSTALL(TARGETS ${TARGET_NAME} DESTINATION ${TARGET_INSTALL_PATH} COMPONENT ${TARGET_NAME})
```

``` bash
#make install

# install 支持单独 target
if [[ $COMPILE_TARGET = "all" ]]; then
    cmake --install .
else
    cmake --install . --component $COMPILE_TARGET
fi
```

https://cmake.org/cmake/help/latest/command/install.html

## get_filename_component

Get a specific component of a full filename.

```
get_filename_component(<var> <FileName> <mode> [CACHE])
```


https://cmake.org/cmake/help/latest/command/get_filename_component.html

## configure_file (代码生成)

```
configure_file(<input> <output>
               [NO_SOURCE_PERMISSIONS | USE_SOURCE_PERMISSIONS |
                FILE_PERMISSIONS <permissions>...]
               [COPYONLY] [ESCAPE_QUOTES] [@ONLY]
               [NEWLINE_STYLE [UNIX|DOS|WIN32|LF|CRLF] ])
```

Copies an `<input>` file to an `<output>` file and substitutes variable values referenced as `@VAR@` or `${VAR}` in the input file content. Each variable reference will be replaced with the current value of the variable, or the empty string if the variable is not defined.

[See Example](https://cmake.org/cmake/help/latest/command/configure_file.html#example)

https://cmake.org/cmake/help/latest/command/configure_file.html

## find_package



* https://cmake.org/cmake/help/latest/command/find_package.html
* https://zhuanlan.zhihu.com/p/97369704

## execute_process

Execute one or more child processes. Commands are executed concurrently as a pipeline, with the standard output of each process piped to the standard input of the next. A single standard error pipe is used for all processes.

* https://cmake.org/cmake/help/latest/command/execute_process.html

## add_compile_options

Adds options to the COMPILE_OPTIONS directory property. These options are used when compiling targets from the current directory and below.

Since different compilers support different options, a typical use of this command is in a compiler-specific conditional clause:

```
if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()
```

* https://cmake.org/cmake/help/latest/command/add_compile_options.html

## add_compile_definitions

The preprocessor definitions are added to the COMPILE_DEFINITIONS directory property for the current CMakeLists file. They are also added to the COMPILE_DEFINITIONS target property for each target in the current CMakeLists file.

* https://cmake.org/cmake/help/latest/command/add_compile_definitions.html


## find_library

```
find_library (<VAR> name1 [path1 path2 ...])
```

This command is used to find a library. A cache entry, or a normal variable if `NO_CACHE` is specified, named by `<VAR>` is created to store the result of this command. If the library is found the result is stored in the variable and the search will not be repeated unless the variable is cleared. If nothing is found, the result will be `<VAR>-NOTFOUND`.

* https://cmake.org/cmake/help/latest/command/find_library.html

## add_link_options

Add options to the link step for executable, shared library or module library targets in the current directory and below that are added after this command is invoked.

* https://cmake.org/cmake/help/latest/command/add_link_options.html

## set_target_properties

Targets can have properties that affect how they are built.

```
set_target_properties(target1 target2 ...
                      PROPERTIES prop1 value1
                      prop2 value2 ...)
```

Sets properties on targets. The syntax for the command is to list all the targets you want to change, and then provide the values you want to set next. You can use any prop value pair you want and extract it later with the `get_property()` or `get_target_property()` command.

* https://cmake.org/cmake/help/latest/command/set_target_properties.html


# Variable

常用的变量：

refer:

* https://cmake.org/Wiki/CMake_Useful_Variables
* cmake-variables(7)

```
SET(PRINT_HEAD_INFO 1)

IF(DEFINED PRINT_HEAD_INFO)

MESSAGE(STATUS "CMAKE_VERSION = ${CMAKE_VERSION}")
MESSAGE(STATUS "CMAKE_SYSTEM = ${CMAKE_SYSTEM}")
MESSAGE(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
MESSAGE(STATUS "CMAKE_SYSTEM_VERSION = ${CMAKE_SYSTEM_VERSION}")
MESSAGE(STATUS "CMAKE_HOST_SYSTEM_NAME = ${CMAKE_HOST_SYSTEM_NAME}")
MESSAGE(STATUS "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE(STATUS "CMAKE_CURRENT_LIST_FILE = ${CMAKE_CURRENT_LIST_FILE}")

MESSAGE(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")

# Compiler (toolchain)
MESSAGE(STATUS "CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
MESSAGE(STATUS "CMAKE_C_COMPILER_ID = ${CMAKE_C_COMPILER_ID}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
MESSAGE(STATUS "CMAKE_C_COMPILER_ABI = ${CMAKE_C_COMPILER_ABI}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER_ABI = ${CMAKE_CXX_COMPILER_ABI}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")

# CMAKE_BUILD_TYPE (Debug, Release, RelWithDebInfo and MinSizeRel)
MESSAGE(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

MESSAGE(STATUS "CMAKE_C_FLAGS_DEBUG = ${CMAKE_C_FLAGS_DEBUG}")
MESSAGE(STATUS "CMAKE_C_FLAGS_RELEASE = ${CMAKE_C_FLAGS_RELEASE}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}")

# ToolChains
MESSAGE(STATUS "CMAKE_AR = ${CMAKE_AR}")
MESSAGE(STATUS "CMAKE_LINKER = ${CMAKE_LINKER}")
MESSAGE(STATUS "CMAKE_NM = ${CMAKE_NM}")
MESSAGE(STATUS "CMAKE_OBJDUMP = ${CMAKE_OBJDUMP}")
MESSAGE(STATUS "CMAKE_RANLIB = ${CMAKE_RANLIB}")

ENDIF() # IF(DEFINED PRINT_HEAD_INFO)
```

## [CMAKE_COMPILER_IS_GNUCXX](https://cmake.org/cmake/help/latest/variable/CMAKE_COMPILER_IS_GNUCXX.html)

True if the C++ (CXX) compiler is GNU.

This variable is deprecated. Use [CMAKE_CXX_COMPILER_ID](https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html#variable:CMAKE_%3CLANG%3E_COMPILER_ID) instead.

## [CMAKE_CXX_COMPILER_ID](https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html#variable:CMAKE_%3CLANG%3E_COMPILER_ID)

```
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(CC_GCC 1)
  string(REGEX REPLACE "^([^\.]+)\..*$" \\1 GCC_MAJOR ${CMAKE_CXX_COMPILER_VERSION})
  message("GCC Major:" ${GCC_MAJOR} " # " ${CMAKE_CXX_COMPILER_VERSION})
else()
  set(CC_GCC 0)
  set(GCC_MAJOR 0)
endif()
```

## CMAKE_CXX_STANDARD

New in version 3.1. Default value for [CXX_STANDARD](https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html#prop_tgt:CXX_STANDARD) target property if set when a target is created.

The C++ standard whose features are requested to build this target.

This property specifies the C++ standard whose features are requested to build this target. For some compilers, this results in adding a flag such as `-std=gnu++11` to the compile line. For compilers that have no notion of a standard level, such as Microsoft Visual C++ before 2015 Update 3, this has no effect.

```
# set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

## CMAKE_CXX_STANDARD_REQUIRED

New in version 3.1. Default value for [CXX_STANDARD_REQUIRED](https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD_REQUIRED.html#prop_tgt:CXX_STANDARD_REQUIRED) target property if set when a target is created. Boolean describing whether the value of `CXX_STANDARD` is a requirement.

## CMAKE_CXX_EXTENSION

New in version 3.1. Default value for [CXX_EXTENSIONS](https://cmake.org/cmake/help/latest/prop_tgt/CXX_EXTENSIONS.html#prop_tgt:CXX_EXTENSIONS) target property if set when a target is created. This property specifies whether compiler specific extensions should be used. For some compilers, this results in adding a flag such as `-std=gnu++11` instead of `-std=c++11` to the compile line. This property is ON by default.


## CMAKE_CXX_COMPILER_VERSION

Compiler version string.

Compiler version in `major[.minor[.patch[.tweak]]]` format. This variable is not guaranteed to be defined for all compilers or languages.

For example `CMAKE_C_COMPILER_VERSION` and `CMAKE_CXX_COMPILER_VERSION` might indicate the respective C and C++ compiler version.

```
# Clang Version
SET(CLANG_EXPECTED_VERSION 3.5.2)
IF(CMAKE_CXX_COMPILER_VERSION VERSION_LESS CLANG_EXPECTED_VERSION)
    MESSAGE(FATAL_ERROR "[Clang Warning]: The project requires Clang version ${CLANG_EXPECTED_VERSION} to build but found ${CMAKE_CXX_COMPILER_VERSION}")
endif()
```

## CMAKE_COMMAND

The full path to the cmake(1) executable. This is the full path to the CMake executable cmake(1) which is useful from custom commands that want to use the cmake -E option for portable system commands. (e.g. /usr/local/bin/cmake)

```
cmake_minimum_required(VERSION 3.10)

project(test)

# Usage: cmake -E <command> [arguments...]
# refer: https://cmake.org/cmake/help/latest/manual/cmake.1.html#run-a-command-line-tool
add_custom_target(TaskA
                  COMMAND ${CMAKE_COMMAND} -E echo "Do TaskA"
                  COMMAND ${CMAKE_COMMAND} -E echo "Do TaskA over"

)
```

* https://cmake.org/cmake/help/v3.4/variable/CMAKE_COMMAND.html
* https://cmake.org/cmake/help/latest/manual/cmake.1.html#run-a-command-line-tool


## EXECUTABLE_OUTPUT_PATH

Old executable location variable. The target property RUNTIME_OUTPUT_DIRECTORY supersedes this variable for a target if it is set. Executable targets are otherwise placed in this directory.


https://cmake.org/cmake/help/latest/variable/EXECUTABLE_OUTPUT_PATH.html

## LIBRARY_OUTPUT_PATH

Old library location variable. The target properties ARCHIVE_OUTPUT_DIRECTORY, LIBRARY_OUTPUT_DIRECTORY, and RUNTIME_OUTPUT_DIRECTORY supersede this variable for a target if they are set. Library targets are otherwise placed in this directory.

https://cmake.org/cmake/help/latest/variable/LIBRARY_OUTPUT_PATH.html

## BUILD_SHARED_LIBS

Global flag to cause add_library() to create shared libraries if on.

If present and true, this will cause all libraries to be built shared unless the library was explicitly added as a static library. This variable is often added to projects as an option() so that each user of a project can decide if they want to build the project using shared or static libraries.

https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html

## CMAKE_BUILD_TYPE

Specifies the build type on single-configuration generators (e.g. **Makefile Generators** or **Ninja**). Typical values include `Debug`, `Release`, `RelWithDebInfo` and `MinSizeRel`, but custom build types can also be defined.

```
IF(PROJ_BUILD_TYPE STREQUAL release)
    #SET(CMAKE_BUILD_TYPE Release)        # -O3 -DNDEBUG
    SET(CMAKE_BUILD_TYPE RelWithDebInfo)  # -O2 -g -DNDEBUG
ELSEIF(PROJ_BUILD_TYPE STREQUAL debug)
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF()
```

https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html

## CMAKE_CXX_FLAGS_DEBUG, CMAKE_CXX_FLAGS_RELEASE / CMAKE_C_FLAGS_DEBUG, CMAKE_C_FLAGS_RELEASE

[how to use CMAKE_CXX_FLAGS_DEBUG and CMAKE_CXX_FLAGS_RELEASE ?](https://cmake.org/pipermail/cmake/2006-February/008371.html)



## CMAKE_BINARY_DIR

The path to the top level of the build tree. This is the full path to the top level of the current CMake build tree. For an in-source build, this would be the same as CMAKE_SOURCE_DIR.

https://cmake.org/cmake/help/latest/variable/CMAKE_BINARY_DIR.html

## CMAKE_SOURCE_DIR

The path to the top level of the source tree. This is the full path to the top level of the current CMake source tree. For an in-source build, this would be the same as CMAKE_BINARY_DIR.

https://cmake.org/cmake/help/latest/variable/CMAKE_SOURCE_DIR.html

## CMAKE_CURRENT_SOURCE_DIR

The path to the source directory currently being processed.

https://cmake.org/cmake/help/latest/variable/CMAKE_CURRENT_SOURCE_DIR.html

## CMAKE_CURRENT_LIST_FILE

Full path to the listfile currently being processed.

https://cmake.org/cmake/help/latest/variable/CMAKE_CURRENT_LIST_FILE.html

## CMAKE_INSTALL_PREFIX

Install directory used by `install()`.

https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html

## CMAKE_INSTALL_MESSAGE

```
SET(CMAKE_INSTALL_MESSAGE LAZY)
```

https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_MESSAGE.html

## CMAKE_SKIP_INSTALL_ALL_DEPENDENCY

Don't make the install target depend on the all target.

https://cmake.org/cmake/help/latest/variable/CMAKE_SKIP_INSTALL_ALL_DEPENDENCY.html

## CMAKE_EXPORT_COMPILE_COMMANDS (输出编译命令)

Enable/Disable output of compile commands during generation.

If enabled, generates a compile_commands.json file containing the exact compiler calls for all translation units of the project in machine-readable form. The format of the JSON file looks like:

``` json
[
  {
    "directory": "/home/user/development/project",
    "command": "/usr/bin/c++ ... -c ../foo/foo.cc",
    "file": "../foo/foo.cc"
  },

  ...

  {
    "directory": "/home/user/development/project",
    "command": "/usr/bin/c++ ... -c ../foo/bar.cc",
    "file": "../foo/bar.cc"
  }
]
```

> Note: This option is implemented only by Makefile Generators and the Ninja. It is ignored on other generators.

* https://cmake.org/cmake/help/latest/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html
* https://stackoverflow.com/questions/20059670/how-to-use-cmake-export-compile-commands

## CMAKE_FIND_LIBRARY_CUSTOM_LIB_SUFFIX

Specify a `<suffix>` to tell the `find_library()` command to search in a `lib<suffix>` directory before each lib directory that would normally be searched.

* https://cmake.org/cmake/help/v3.9/variable/CMAKE_FIND_LIBRARY_CUSTOM_LIB_SUFFIX.html

## CMAKE_MODULE_PATH

[Semicolon-separated list](https://cmake.org/cmake/help/latest/manual/cmake-language.7.html#cmake-language-lists) of directories specifying a search path for CMake modules to be loaded by the [include()](https://cmake.org/cmake/help/latest/command/include.html#command:include) or [find_package()](https://cmake.org/cmake/help/latest/command/find_package.html#command:find_package) commands before checking the default modules that come with CMake. By default it is empty, it is intended to be set by the project.

```
set(THIRDPARTY_PATH "${SLN_ROOT}/thirdparty")
set(CMAKE_MODULE_PATH "${THIRDPARTY_PATH}/cmake;${CMAKE_MODULE_PATH}")
```

* https://cmake.org/cmake/help/latest/variable/CMAKE_MODULE_PATH.html


# Module

## FetchContent

This module enables populating content at configure time via any method supported by the `ExternalProject` module. Whereas `ExternalProject_Add()` downloads at build time, the `FetchContent` module makes content available immediately, allowing the configure step to use the content in commands like `add_subdirectory()`, `include()` or `file()` operations.

* https://cmake.org/cmake/help/latest/module/FetchContent.html
* https://cmake.org/cmake/help/latest/module/FetchContent.html#fetch-content-examples
* https://cliutils.gitlab.io/modern-cmake/chapters/projects/fetch.html

## GoogleTest

This module defines functions to help use the Google Test infrastructure. Two mechanisms for adding tests are provided. `gtest_add_tests()` has been around for some time, originally via `find_package(GTest)`. `gtest_discover_tests()` was introduced in CMake 3.10.

https://cmake.org/cmake/help/git-stage/module/GoogleTest.html

## check_cxx_source_compiles

```
check_cxx_source_compiles(<code> <resultVar>
                          [FAIL_REGEX <regex1> [<regex2>...]])
```

Check that the source supplied in `<code>` can be compiled as a C++ source file and linked as an executable (so it must contain at least a `main()` function). The result will be stored in the internal cache variable specified by `<resultVar>`, with a boolean true value for success and boolean false for failure.


* https://cmake.org/cmake/help/latest/module/CheckCXXSourceCompiles.html
* https://stackoverflow.com/questions/40877744/cmake-why-doesnt-cmake-cxx-standard-seem-to-work-with-check-cxx-source-compile


# CTest

The [ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html#ctest-1) executable is the **CMake test driver program**. CMake-generated build trees created for projects that use the `enable_testing()` and `add_test()` commands have testing support. This program will run the tests and report results.

使用正则表达式过滤执行测试用例：

```
$ ctest -R .*Test
Test project /data/home/gerryyang/JLib_Build
    Start 1: MsgHelperTest,
1/2 Test #1: MsgHelperTest, ...................   Passed    0.01 sec
    Start 2: SampleCtrlTest,
2/2 Test #2: SampleCtrlTest, ..................   Passed    0.02 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   0.03 sec**
```

[Testing With CMake and CTest](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html):

1. **Smoke tests（冒烟测试*）**, such as one that simply verifies that the software compiles.
2. **Regression testing（回归测试）** verifies that the results of a test do not change over time or platform.
3. **White- and black-box testing（白盒和黑盒测试）** refer to tests written to exercise units of code (at various levels of integration), with and without knowledge of how those units are implemented respectively.
4. **Compliance testing** tries to determine if the code adheres to the coding standards of the software project. This could be a check to verify that all classes have implemented some key method, or that all functions have a common prefix.

## How Does CMake Facilitate Testing?

CMake facilitates testing your software through special testing commands and the `CTest` executable. First, we will discuss the key testing commands in CMake. To add testing to a CMake-based project, simply `include(CTest)` and use the `add_test` command. The `add_test` command has a simple syntax as follows:

```
add_test(NAME TestName COMMAND ExecutableToRun arg1 arg2 ...)
```

The first argument is simply a string name for the test. This is the name that will be displayed by testing programs. The second argument is the executable to run. The executable can be built as part of the project or it can be a standalone executable such as python, perl, etc. The remaining arguments will be passed to the running executable. A typical example of testing using the `add_test` command would look like this:

```
add_executable(TestInstantiator TestInstantiator.cxx)
target_link_libraries(TestInstantiator vtkCommon)
add_test(NAME TestInstantiator
         COMMAND TestInstantiator)
```

The `add_test` command is typically placed in the CMakeLists file for the directory that has the test in it. For large projects, there may be multiple CMakeLists files with `add_test` commands in them. Once the `add_test` commands are present in the project, the user can run the tests by invoking the “test” target of Makefile, or the RUN_TESTS target of Visual Studio or Xcode. An example of running tests on the CMake tests using the Makefile generator on Linux would be:

```
$ make test
Running tests...
Test project
     Start 2: kwsys.testEncode
 1/20 Test  #2: kwsys.testEncode ..........   Passed    0.02 sec
     Start 3: kwsys.testTerminal
 2/20 Test  #3: kwsys.testTerminal ........   Passed    0.02 sec
     Start 4: kwsys.testAutoPtr
 3/20 Test  #4: kwsys.testAutoPtr .........   Passed    0.02 sec
```

## Additional Test Properties

[refer](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html#additional-test-properties)

By default a test passes if all of the following conditions are true:

* The test executable was found
* The test ran without exception
* The test exited with return code 0

That said, these behaviors can be modified using the `set_property` command:

```
set_property(TEST test_name
             PROPERTY prop1 value1 value2 ...)
```

## Testing Using CTest

When you run the tests from your build environment, what really happens is that the build environment runs `CTest`. `CTest` is an executable that comes with `CMake`; it handles running the tests for the project. While `CTest` works well with `CMake`, you do not have to use `CMake` in order to use `CTest`. **The main input file for `CTest` is called CTestTestfile.cmake.** This file will be created in each directory that was processed by CMake (typically every directory with a CMakeLists file). The syntax of **CTestTestfile.cmake** is like the regular `CMake` syntax, with a subset of the commands available. If `CMake` is used to generate testing files, they will list any subdirectories that need to be processed as well as any `add_test` calls. The subdirectories are those that were added by the `add_subdirectory` commands. `CTest` can then parse these files to determine what tests to run. An example of such a file is shown below:

```
# CMake generated Testfile for
# Source directory: C:/CMake
# Build directory: C:/CMakeBin
#
# This file includes the relevent testing commands required
# for testing this directory and lists subdirectories to
# be tested as well.

add_test (SystemInformationNew ...)

add_subdirectory (Source/kwsys)
add_subdirectory (Utilities/cmzlib)
...
```

通过`strace -s0124 -tt -e trace=open ctest`可以跟踪`ctest`的执行过程：

```
...
20:19:39.346867 open("/data/home/gerryyang/JLib_Build/Testing/Temporary/LastTest.log.tmp", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
20:19:39.348271 open("CTestTestfile.cmake", O_RDONLY) = 4
20:19:39.353922 open("/data/home/gerryyang/JLib_Build/unittest/CTestTestfile.cmake", O_RDONLY) = 4
20:19:39.354770 open("/data/home/gerryyang/JLib_Build/unittest/libs/common/CTestTestfile.cmake", O_RDONLY) = 4
20:19:39.355874 open("/data/home/gerryyang/JLib_Build/unittest/src/samplesvr/CTestTestfile.cmake", O_RDONLY) = 4
20:19:39.357008 open("/etc/localtime", O_RDONLY|O_CLOEXEC) = 4
20:19:39.357999 open("/data/home/gerryyang/JLib_Build/Testing/Temporary/CTestCostData.txt", O_RDONLY) = 4
20:19:39.368874 open("/data/home/gerryyang/JLib_Build/Testing/Temporary/CTestCheckpoint.txt", O_WRONLY|O_CREAT|O_APPEND, 0666) = 4
...
```

When `CTest` parses the **CTestTestfile.cmake** files, it will extract the list of tests from them. These tests will be run, and for each test `CTest` will display the name of the test and its status. Consider the following sample output:

```
$ ctest
Test project C:/CMake-build26
        Start 1: SystemInformationNew
 1/21 Test  #1: SystemInformationNew ......   Passed    5.78 sec
        Start 2: kwsys.testEncode
 2/21 Test  #2: kwsys.testEncode ..........   Passed    0.02 sec
        Start 3: kwsys.testTerminal
 3/21 Test  #3: kwsys.testTerminal ........   Passed    0.00 sec
        Start 4: kwsys.testAutoPtr
 4/21 Test  #4: kwsys.testAutoPtr .........   Passed    0.02 sec
        Start 5: kwsys.testHashSTL
 5/21 Test  #5: kwsys.testHashSTL .........   Passed    0.02 sec
...
100% tests passed, 0 tests failed out of 21
Total Test time (real) =  59.22 sec
```

`CTest` is run from within your build tree. It will run all the tests found in the current directory as well as any subdirectories listed in the **CTestTestfile.cmake.** For each test that is run `CTest` will report if the test passed and how long it took to run the test.

The `CTest` executable includes some handy command line options to make testing a little easier. We will start by looking at the options you would typically use from the command line.

```
-R <regex>            Run tests matching regular expression
-E <regex>            Exclude tests matching regular expression
-L <regex>            Run tests with labels matching the regex
-LE <regex>           Run tests with labels not matching regexp
-C <config>           Choose the configuration to test
-V,--verbose          Enable verbose output from tests.
-N,--show-only        Disable actual execution of tests.
-I [Start,End,Stride,test#,test#|Test file]
                      Run specific tests by range and number.
-H                                        Display a help message
```

* The `-R` option is probably the most commonly used. It allows you to specify a regular expression; only the tests with names matching the regular expression will be run. Using the `-R` option with the name (or part of the name) of a test is a quick way to run a single test.

* The `-E` option is similar except that it excludes all tests matching the regular expression. The `-L` and `-LE` options are similar to `-R` and `-E`, except that they apply to test labels that were set using the `set_property` command described previously.

* The `-I` option allows you to flexibly specify a subset of the tests to run. For example, the following invocation of CTest will run every seventh test.

## Other

* [CMake how to install test files with unit tests](https://stackoverflow.com/questions/12313258/cmake-how-to-install-test-files-with-unit-tests)

## enable_testing

Enable testing for current directory and below. This command should be in the source directory root because [ctest](https://cmake.org/cmake/help/latest/module/CTest.html#module:CTest) expects to find a test file in the build directory root.

https://cmake.org/cmake/help/latest/command/enable_testing.html

## add_test

Add a test to the project to be run by [ctest(1)](https://cmake.org/cmake/help/latest/manual/ctest.1.html#manual:ctest(1)).

```
add_test(NAME <name> COMMAND <command> [<arg>...]
         [CONFIGURATIONS <config>...]
         [WORKING_DIRECTORY <dir>]
         [COMMAND_EXPAND_LISTS])
```

https://cmake.org/cmake/help/latest/command/add_test.html


# 编译效率对比

在8核CPU，16G内存机器，对比`gcc`, `clang`, `make`, `ninja`, `ld`, `lld`不同组合情况下的编译效率。

[测试代码](https://github.com/gerryyang/mac-utils/tree/master/tools/CMake/helloworld)

> 测试结果：

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


# Cross Compiling

[cmake-toolchains](https://cmake.org/cmake/help/v3.8/manual/cmake-toolchains.7.html)

 In normal builds, CMake automatically determines the toolchain for host builds based on system introspection and defaults. In cross-compiling scenarios, a toolchain file may be specified with information about compiler and utility paths.

If `cmake` is invoked with the command line parameter `-DCMAKE_TOOLCHAIN_FILE=path/to/file`, the file will be loaded early to set values for the compilers. The `CMAKE_CROSSCOMPILING` variable is set to true when CMake is cross-compiling.




# [Building LLVM with CMake](http://llvm.org/docs/CMake.html#building-llvm-with-cmake)

[CMake](http://www.cmake.org/) is a cross-platform build-generator tool. CMake does not build the project, it generates the files needed by your build tool (GNU make, Visual Studio, Xcode, etc.) for building LLVM.

make.sh


```
#!/bin/bash

export LLVM_DIR=$HOME/llvm
export LLVM_INSTALL_DIR=$HOME/llvm/install

mkdir -p $LLVM_INSTALL_DIR
cd $LLVM_DIR

git clone https://github.com/llvm/llvm-project llvm-project
mkdir build
cd build

## -DCMAKE_BUILD_TYPE=type — Valid options for type are Debug, Release, RelWithDebInfo, and MinSizeRel. Default is Debug
## -DCMAKE_INSTALL_PREFIX=directory — Specify for directory the full pathname of where you want the LLVM tools and libraries to be installed (default /usr/local)

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=On -DLLVM_TARGETS_TO_BUILD='X86' -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi;libunwind;lldb;compiler-rt;lld" -DCMAKE_INSTALL_PREFIX=$LLVM_INSTALL_DIR ../llvm-project/llvm

## The --build option tells cmake to invoke the underlying build tool (make, ninja, xcodebuild, msbuild, etc.)
cmake --build .

## After LLVM has finished building, install it from the build directory
cmake --build . --target install
```

CMakeLists.txt

```
cmake_minimum_required(VERSION 3.13.4)
project(SimpleProject)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Now build our tools
add_executable(simple-tool tool.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(simple-tool ${llvm_libs})

```


refer:

* [Embedding LLVM in your project](http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project)
* [LLVM Pass入门导引](https://zhuanlan.zhihu.com/p/122522485)


# LLD (The LLVM Linker)

Replace ld to lld:

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
* [gold (linker)](https://en.wikipedia.org/wiki/Gold_%28linker%29)


# [Ninja](https://ninja-build.org/)

Ninja is a small build system with a focus on speed. It differs from other build systems in two major respects: it is designed to have its input files generated by a higher-level build system, and it is designed to run builds as fast as possible.


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


# Bazel

* `bazel`默认会限制并发度到其估计的机器性能上限，实际使用需要通过--local_cpu_resources=9999999等参数绕过这一限制
* 已知（部分版本的）bazel在并发度过高（如`-j320`）下，bazel自身性能存在瓶颈。这具体表现为机器空闲但不会启动更多编译任务，同时bazel自身CPU（`400~500%`）、内存（几G）占用很高。
* 如果机器资源充足且对并发度有较高要求（几百并发），可以考虑使用其他构建系统构建。

refer:

* [C / C++ Rules](https://docs.bazel.build/versions/master/be/c-cpp.html)


# Building gRPC with CMake

```
#!/bin/bash

## refer: https://grpc.io/docs/languages/cpp/quickstart/
## Install the basic tools required to build gRPC

export CMAKE_INSTALL_DIR=$HOME/cmake
export GRPC_INSTALL_DIR=$HOME/grpc
mkdir -p $GRPC_INSTALL_DIR
cd $GRPC_INSTALL_DIR

#sudo apt install -y build-essential autoconf libtool pkg-config
sudo yum install -y build-essential autoconf libtool pkg-config

## Clone the grpc repo and its submodules
git clone --recurse-submodules -b v1.33.2 https://github.com/grpc/grpc
cd grpc

## Build and locally install gRPC and all requisite tools
mkdir -p cmake/build
pushd cmake/build

cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$CMAKE_INSTALL_DIR ../..

make -j
make install
popd

```

# CMake UNITY_BUILD (合并文件编译)

https://cmake.org/cmake/help/latest/prop_tgt/UNITY_BUILD.html#unity-build

一种编译优化技术，把多个源文件合并成一个大文件再进行编译和链接。

When this property is set to true, the target source files will be combined into batches for faster compilation. This is done by creating a (set of) unity sources which `#include` the original sources, then compiling these unity sources instead of the originals. This is known as a **Unity or Jumbo build**.

> ODR (One definition rule) errors

When multiple source files are included into one source file, as is done for unity builds, it can potentially lead to ODR errors. CMake provides a number of measures to help address such problems.


## 核心原理

将**多个源文件**合并为**单个编译单元**（Unity Source），**通过 `#include` 包含原始文件，减少编译次数和重复工作**。

> `UNITY_BUILD` 是“以空间换时间”的优化策略，适用于资源充足的构建环境，但需严格测试 `ODR` 问题。建议在开发与发布环境中差异化启用。

## 优点

1. **显著提升编译速度**
   * **减少头文件重复解析**：公共头文件仅被处理一次，避免多次加载（尤其对大型头文件效果显著）。
   * **降低编译器启动开销**：编译 100 个文件时，若合并为 10 个 Unity 文件，编译器仅启动 10 次（而非 100 次）。
   * **跨文件优化增强**：编译器可内联跨文件的函数，优化代码生成（传统编译因隔离翻译单元无法实现）。

2. **减少内存占用**
   * 合并编译单元减少重复符号处理，降低链接器内存消耗（对大型项目尤为重要）。

3. **简化构建流程**
   * 减少中间对象文件数量，加速链接阶段。


## 缺点

1. **ODR（单一定义规则）风险**
   * 根本原因：不同源文件的全局符号（函数、变量、类）被强制合并到同一编译单元。
   * 典型冲突场景：
      + 同名全局变量/函数出现在不同文件中（传统编译因隔离可共存）
      + 不同文件中的匿名命名空间可能被合并，引发符号冲突
   * CMake 缓解措施：
      + 自动为每个文件添加唯一后缀（如 `namespace { }` → `namespace <file_hash> { }`）
      + 通过 `set_source_files_properties(... SKIP_UNITY_BUILD_INCLUSION ON)` 排除问题文件
      + 使用 `UNITY_BUILD_BATCH_SIZE` 限制单批次文件数（**默认 8**），降低冲突概率

2. **增量构建效率下降**
  * 修改任意被合并的源文件，整个 Unity 文件需重新编译（传统编译仅重编单个文件）

3. **调试难度增加**
  * 调试器指向 Unity 合并后的文件（非原始源文件），影响代码定位
  * **解决方案：开发阶段禁用 `UNITY_BUILD`**

4. **编译内存峰值升高**
  * 超大 Unity 文件可能耗尽内存（需通过 `UNITY_BUILD_BATCH_SIZE` 控制规模）

5. **宏/静态变量冲突**
  * 不同文件中的同名静态变量或宏可能相互覆盖。

## 实践建议

1. 适用场景：大型项目 Release 构建（追求编译速度）
2. 规避 ODR 措施
   * 启用 `-Wodr` 等编译器警告
   * 测试阶段开启 `UNITY_BUILD` 主动暴露问题
   * 避免全局符号使用通用名称（如 `utils.h` 中的 `get()`）。

3. 参数调优

``` makefile
set_target_properties(my_target PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 5       # 控制合并规模
    UNITY_BUILD_UNIQUE_ID "MY_ID"  # 增强符号唯一性
)
```

4. 开发/发布分离配置

``` makefile
option(ENABLE_UNITY_BUILD "Enable faster builds" OFF)
if(ENABLE_UNITY_BUILD)
    set_target_properties(my_target PROPERTIES UNITY_BUILD ON)
endif()
```



# Tips

## 生成中间文件

```
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps")
```

参考：[保存临时文件](https://wizardforcel.gitbooks.io/100-gcc-tips/content/save-temps.html)

```
$ gcc -save-temps a/foo.c
$ ls foo.*
foo.c  foo.i  foo.o  foo.s

$ gcc -save-temps=obj a/foo.c -o a/foo
$ ls a
foo  foo.c  foo.i  foo.o  foo.s
```

* [gcc with parameters "-S -save-temps" puts intermediate files in current directory](https://stackoverflow.com/questions/2165079/gcc-with-parameters-s-save-temps-puts-intermediate-files-in-current-director)


## debug 编译去除 -g 选项

```
# disable -g
string(REPLACE "-g" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REPLACE "-g" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
```

## 检查 CXX 编译器是否支持某个编译选项 (CheckCXXCompilerFlag)

Check whether the CXX compiler supports a given flag.

```
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" SUPPORT_CXX11)
IF(NOT SUPPORT_CXX11)
    MESSAGE(FATAL_ERROR "compiler not support c++11")
ENDIF()
```

* https://cmake.org/cmake/help/latest/module/CheckCXXCompilerFlag.html


## (FindPackageHandleStandardArgs)

This module provides functions intended to be used in Find Modules implementing `find_package(<PackageName>`) calls.

```
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Protobuf
    REQUIRED_VARS Protobuf_PROTOC_EXECUTABLE Protobuf_LIBRARIES Protobuf_INCLUDE_DIRS
    VERSION_VAR Protobuf_VERSION
)
```

* https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.html
* https://stackoverflow.com/questions/52785157/what-does-findpackagehandlestandardargs-do-exactly


## 检查是否是 GCC 编译

```
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(CC_GCC 1)
  string(REGEX REPLACE "^([^\.]+)\..*$" \\1 GCC_MAJOR ${CMAKE_CXX_COMPILER_VERSION})
  message("GCC Major:" ${GCC_MAJOR} " # " ${CMAKE_CXX_COMPILER_VERSION})
else()
  set(CC_GCC 0)
  set(GCC_MAJOR 0)
endif()
```

## [CMake - remove a compile flag for a single translation unit](https://stackoverflow.com/questions/28344564/cmake-remove-a-compile-flag-for-a-single-translation-unit)

TODO





# 参考示例

* https://github.com/sogou/workflow/blob/master/CMakeLists.txt


# Issue

## [Makefiles: Multiple targets not built in parallel](https://gitlab.kitware.com/cmake/cmake/-/issues/23876)



# Q&A

## -rdynamic

`CMake 3.3` and below, for historical reasons, always linked executables on some platforms with flags like `-rdynamic` to export symbols from the executables for use by any plugins they may load via dlopen. `CMake 3.4` and above prefer to do this only for executables that are explicitly marked with the [ENABLE_EXPORTS](https://cmake.org/cmake/help/latest/prop_tgt/ENABLE_EXPORTS.html#prop_tgt:ENABLE_EXPORTS) target property.

`ENABLE_EXPORTS` 是一个 CMake 属性，**用于指定可执行文件或共享库是否导出符号**。通常，可执行文件不导出任何符号，因为它是最终程序。但是，可执行文件可以导出供可加载模块使用的符号。当此属性设置为 true 时，CMake 将允许其他目标使用 `target_link_libraries()` 命令链接到可执行文件。在所有平台上，链接到可执行文件的目标都会创建一个目标级依赖关系。可加载模块对可执行文件的链接处理因平台而异。

```
# cmake 3.17 需要显示打开 --rdynamic 链接选项，@refer https://cmake.org/cmake/help/latest/policy/CMP0065.html
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--export-dynamic")
```

以下是一个使用 `ENABLE_EXPORTS` 的 CMake 示例：


Step1: 创建以下目录结构

```
my_project/
|-- CMakeLists.txt
|-- main.cpp
|-- my_module/
    |-- CMakeLists.txt
    |-- my_module.cpp
```

Step2: 在 `my_project/CMakeLists.txt` 文件中添加以下内容

```
cmake_minimum_required(VERSION 3.0)
project(MyProject)

# 设置 CMAKE_EXECUTABLE_ENABLE_EXPORTS 变量
set(CMAKE_EXECUTABLE_ENABLE_EXPORTS ON)

# 添加 main 可执行文件
add_executable(main main.cpp)

# 添加子目录以便处理 my_module 的 CMakeLists.txt
add_subdirectory(my_module)

# 链接 main 可执行文件和 my_module
target_link_libraries(main my_module)
```

Step3: 在 `my_project/main.cpp` 文件中添加以下内容

``` cpp
#include <iostream>

// 声明要从 my_module 导入的函数
extern "C" void my_module_function();

int main() {
  std::cout << "Calling my_module_function from main:" << std::endl;
  my_module_function();
  return 0;
}
```

Step4: 在 `my_project/my_module/CMakeLists.txt` 文件中添加以下内容

```
# 添加 my_module 作为共享库
add_library(my_module SHARED my_module.cpp)
```

Step5: 在 `my_project/my_module/my_module.cpp` 文件中添加以下内容

``` cpp
#include <iostream>

extern "C" void my_module_function() {
  std::cout << "Hello from my_module_function!" << std::endl;
}
```

在这个示例中，创建了一个名为 `main` 的可执行文件，它导出一个符号 `my_module_function`。我们还创建了一个名为 `my_module` 的共享库，它使用 `main` 可执行文件中导出的 `my_module_function`。通过在 `my_project/CMakeLists.txt` 中设置 `CMAKE_EXECUTABLE_ENABLE_EXPORTS` 变量，我们启用了 `main` 可执行文件的符号导出功能。这使得 `my_module` 能够链接到 `main` 并使用它导出的符号。

> 注意：CMAKE_EXECUTABLE_ENABLE_EXPORTS 变量用于初始化目标属性 ENABLE_EXPORTS。当 ENABLE_EXPORTS 设置为 ON 时，CMake 会允许其他目标链接到可执行文件。然而，并不是所有平台都会自动添加 --export-dynamic 选项。要确保在链接时添加 --export-dynamic 选项，可以在 CMakeLists.txt 文件中显式设置此选项。



## [Most simple but complete CMake example](https://stackoverflow.com/questions/21163188/most-simple-but-complete-cmake-example)


## [list(REMOVE_ITEM) not working in cmake](https://stackoverflow.com/questions/36134129/listremove-item-not-working-in-cmake)


# Refer

* [Switching between GCC and Clang/LLVM using CMake](https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake)
* [How to print messages after make done with cmake?](https://stackoverflow.com/questions/25240105/how-to-print-messages-after-make-done-with-cmake/34953585)
* [CMake: Print out all accessible variables in a script](https://stackoverflow.com/questions/9298278/cmake-print-out-all-accessible-variables-in-a-script)
* https://github.com/BrightXiaoHan/CMakeTutorial

# Manual

* [An Introduction to Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
* [MAKEFILES TUTORIAL](https://devarea.com/makefiles-tutorial/#.ZAmnUuxBw0Q)