---
layout: post
title:  "Clang in Action"
date:   2022-09-08 18:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# 安装

## Clang 3.5.2

下载 llvm 源码：

```
wget http://releases.llvm.org/3.5.2/llvm-3.5.2.src.tar.xz
tar xf llvm-3.5.2.src.tar.xz
mv llvm-3.5.2.src llvm
```

下载 clang 源码：

```
cd llvm/tools
wget http://releases.llvm.org/3.5.2/cfe-3.5.2.src.tar.xz
tar xf cfe-3.5.2.src.tar.xz
mv cfe-3.5.2.src clang
```

下载 compiler-rt 源码：

```
cd ../projects
wget http://releases.llvm.org/3.5.2/compiler-rt-3.5.2.src.tar.xz
tar xf compiler-rt-3.5.2.src.tar.xz
mv compiler-rt-3.5.2.src compiler-rt
```

配置编译选项：

```
cd ..
./configure --enable-optimized CC=gcc CXX=g++
```

编译 llvm：

```
make -j16
```

编译成功后的提示：

```
llvm[0]: ***** Completed Release+Asserts Build
```

安装编译好的 llvm：

``` bash
# 会安装在 /usr/local/bin 目录
make install
```

检查 clang 的版本：

```
clang --version
clang version 3.5.2 (tags/RELEASE_352/final)
```

Refer:

* http://releases.llvm.org/download.html#3.5.2
* https://www.cnblogs.com/dudu/p/4294374.html



# Clang 11.0.0

```
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/llvm-project-11.0.0.tar.xz

xz -d llvm-project-11.0.0.tar.xz
tar -xvf llvm-project-11.0.0.tar

cd llvm-project-11.0.0
mkdir build
cd build

# 编译 clang 和 asan 等工具
# Host GCC version must be at least 5.1
cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_RTTI=ON -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;libcxx;libcxxabi;libunwind;lldb;compiler-rt;lld;polly" -G "Unix Makefiles" ../llvm

#cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_RTTI=ON -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi" -G "Unix Makefiles" ../llvm

make -j16 && make install

clang --version
```

升级 gcc

```
yum install centos-release-scl -y
yum install devtoolset-7 -y --skip-broken

# 如果安装 devtoolset-7 依赖报错，可能需要安装 kernel-debug-devel
yum install kernel-debug-devel-3.10.0-327.el7.x86_64 -y

# 新创建一个 bash 切换到 devtoolset-7，退出当前 bash 则切换到原来版本
scl enable devtoolset-7 bash

# 如果希望固定到 gcc-7 环境则将配置写入文件
source /opt/rh/devtoolset-7/enable
echo "source /opt/rh/devtoolset-7/enable" >> ~/.bash_profile  source /opt/rh/devtoolset-7/enable
```

升级 ld 为 2.30

```
wget https://ftp.gnu.org/gnu/binutils/binutils-2.30.tar.gz --no-check-certificate
tar zxvf binutils-2.30.tar.gz
cd binutils-2.30

./configure --prefix=/usr/local/bin
make -j16 && make install

mv /bin/ld /bin/ld2.23
ln -s /usr/local/bin/bin/ld /bin/ld
```


