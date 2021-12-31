---
layout: post
title:  "CMake in Action"
date:   2020-12-13 11:30:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Install CMake

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
cmake --version
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
$ mkdir build ; cd build
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

* `cmake_minimum_required`：cmake 的最低版本要求
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

# Command

## cmake_minimum_required

Require a minimum version of cmake.

```
cmake_minimum_required(VERSION <min>[...<policy_max>] [FATAL_ERROR])
```

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

https://cmake.org/cmake/help/latest/command/install.html

## get_filename_component

Get a specific component of a full filename.

```
get_filename_component(<var> <FileName> <mode> [CACHE])
```


https://cmake.org/cmake/help/latest/command/get_filename_component.html

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

# Variable

常用的变量：

refer: 

* https://cmake.org/Wiki/CMake_Useful_Variables
* cmake-variables(7)

```
MESSAGE(STATUS "CMAKE_CURRENT_LIST_FILE = ${CMAKE_CURRENT_LIST_FILE}")

MESSAGE(STATUS "CMAKE_VERSION = ${CMAKE_VERSION}")
MESSAGE(STATUS "CMAKE_SYSTEM = ${CMAKE_SYSTEM}")
MESSAGE(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")

MESSAGE(STATUS "CMAKE_SYSTEM_VERSION = ${CMAKE_SYSTEM_VERSION}")
MESSAGE(STATUS "CMAKE_HOST_SYSTEM_NAME = ${CMAKE_HOST_SYSTEM_NAME}")
MESSAGE(STATUS "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")

MESSAGE(STATUS "CMAKE_CONFIGURATION_TYPES = ${CMAKE_CONFIGURATION_TYPES}")
MESSAGE(STATUS "CMAKE_BUIlD_TYPE = ${CMAKE_BUIlD_TYPE}")

MESSAGE(STATUS "CMAKE_C_FLAGS_DEBUG = ${CMAKE_C_FLAGS_DEBUG}")
MESSAGE(STATUS "CMAKE_C_FLAGS_RELEASE = ${CMAKE_C_FLAGS_RELEASE}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}")

MESSAGE(STATUS "CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
MESSAGE(STATUS "CMAKE_C_COMPILER_ID = ${CMAKE_C_COMPILER_ID}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
MESSAGE(STATUS "CMAKE_C_COMPILER_ABI = ${CMAKE_C_COMPILER_ABI}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER_ABI = ${CMAKE_CXX_COMPILER_ABI}")

MESSAGE(STATUS "CMAKE_AR = ${CMAKE_AR}")
MESSAGE(STATUS "CMAKE_LINKER = ${CMAKE_LINKER}")
MESSAGE(STATUS "CMAKE_NM = ${CMAKE_NM}")
MESSAGE(STATUS "CMAKE_OBJDUMP = ${CMAKE_OBJDUMP}")
MESSAGE(STATUS "CMAKE_RANLIB = ${CMAKE_RANLIB}")

# Clang Version
IF(CMAKE_C_COMPILER_ID STREQUAL Clang AND CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    MESSAGE(STATUS "CLANG_VERSION_MAJOR = ${CLANG_VERSION_MAJOR}")
    MESSAGE(STATUS "CLANG_VERSION_MINOR = ${CLANG_VERSION_MINOR}")
    MESSAGE(STATUS "CLANG_VERSION_PATCHLEVEL = ${CLANG_VERSION_PATCHLEVEL}")
    MESSAGE(STATUS "CLANG_VERSION_STRING = ${CLANG_VERSION_STRING}")

    # Under cmake v3.1.3 I had to check CMAKE_CXX_COMPILER_VERSION as there was no CLANG_VERSION_STRING so looks like a version is now defined for each compiler type giving more control.
    MESSAGE(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
ENDIF()
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

# Q&A

* [Most simple but complete CMake example](https://stackoverflow.com/questions/21163188/most-simple-but-complete-cmake-example)
* [list(REMOVE_ITEM) not working in cmake](https://stackoverflow.com/questions/36134129/listremove-item-not-working-in-cmake)

# Refer

* [Switching between GCC and Clang/LLVM using CMake](https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake)
* [How to print messages after make done with cmake?](https://stackoverflow.com/questions/25240105/how-to-print-messages-after-make-done-with-cmake/34953585)


