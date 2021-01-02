---
layout: post
title:  "CMake in Action"
date:   2020-12-13 11:30:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Update gcc

```
# enable gcc7
sudo yum install centos-release-scl
sudo yum install devtoolset-7-gcc*
scl enable devtoolset-7 bash
```

refer:

* https://stackoverflow.com/questions/36327805/how-to-install-gcc-5-3-with-yum-on-centos-7-2


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

# CMake Helloworld

* `cmake_minimum_required`：cmake 的最低版本要求
* `project`：指定项目的名称
* `set`：设置普通变量，缓存变量或环境变量
* `add_executable`：使用列出的源文件构建可执行文件
* `include_directories`：添加多个头文件搜索路径，路径之间用空格分隔；在 include 的时候就不需要使用相对路径了
* `aux_source_directory`：在目录中查找所有源文件，并将这些源文件存储在变量 SOURCE_DIR 中；需要注意这个指令不会递归包含子目录
* `link_directories`：指定静态库或动态库的搜索路径
* `target_link_libraries`：将指定的静态库连接到可执行文件上，singleton 和 libsingleton.a 两种形式等价
* `set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)`：命令将生成的二进制文件放到了 bin 目录下，注意这里的 bin 目录是使用 cmake 进行构建的目录（PROJECT_BINARY_DIR，即build/bin）
* `add_library`：来使用指定的源文件生成库文件，`add_library(calculator_static STATIC ${static_lib_source_file})` 第一个参数指定库名字。
* `target_link_libraries`：将生成的库文件添加到项目中

Note:

* cmake的指令是不区分大小写的，写作`CMAKE_MINIMUM_REQUIRED`或`cmake_minimum_required`，甚至是`cmAkE_mInImUm_rEquIrEd`（不建议）都是可以的
* 在使用 set 指令指定`CMAKE_CXX_FLAGS`的时候通过空格来分隔多个编译选项，生成的`CMAKE_CXX_FLAGS`字符串是 “-g;-Wall”，需要用字符串替换将分号替换为空格
* message 可以在构建的过程中向 stdout 输出一些信息
* 类似于 bash 脚本，在 CMakeLists.txt 中输出变量时要使用`${CMAKE_CXX_FLAGS}`的形式，而不能直接使用`CMAKE_CXX_FLAGS`
* 编辑好 CMakeLists.txt 之后，可以新建一个 build 目录，并在 build 目录下使用 cmake 来进行构建，构建成功的话再使用 make 来进行编译和链接，最终得到可执行文件
* 除了直接引用外部的静态库，cmake 还可以先将源文件编译成静态库之后在进行构建
	+ `target_link_libraries(Exp2 libcalculator.a)`
	+ `target_link_libraries(Exp2 calculator)`
* 使用`#[[ ... ]]`或`#`注释



# 编译效率对比

在8核CPU，16G内存机器，对比`gcc`, `clang`, `make`, `ninja`, `ld`, `lld`不同组合情况下的编译效率。

[测试代码](https://github.com/gerryyang/mac-utils/tree/master/tools/CMake/helloworld)

> 测试结果：

clang12 优于 gcc4.8/7/9，ninja 优于 make，lld 优于 ld。

| Case | Time |
| -- | -- |
| gcc7 + make + ld    | 25.7s
| clang12 + make + ld   | 5.2s
| gcc7 + ninja + ld   | 22s
| clang12 + ninja + ld  | 4.7s
| gcc7 + make + lld    | 17.8s
| clang12 + make + lld   | 4.82s
| gcc7 + ninja + lld   | 18.34s
| clang12 + ninja + lld  | 4.15s
| gcc9 + make + lld    | 10.03s
| gcc9 + ninja + lld   | 7.90s
| gcc4.8 + make + lld    | 8.93s
| gcc4.8 + ninja + lld   | 8.30s


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

# Refer

* https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake



