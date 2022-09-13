---
layout: post
title:  "Clang in Action"
date:   2022-09-08 18:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Clang 3.5.2 编译环境下载安装

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






