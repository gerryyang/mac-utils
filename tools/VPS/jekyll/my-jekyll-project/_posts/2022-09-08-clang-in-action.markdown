---
layout: post
title:  "Clang in Action"
date:   2022-09-08 18:00:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}

# [LLVM](https://github.com/llvm/llvm-project)

https://llvm.org/docs/GettingStarted.html

## Overview

The LLVM project has multiple components. The core of the project is itself called "LLVM". This contains all of the tools, libraries, and header files needed to process intermediate representations and convert them into object files. Tools include an assembler, disassembler, bitcode analyzer, and bitcode optimizer. It also contains basic regression tests.

C-like languages use the [Clang](http://clang.llvm.org/) frontend. This component compiles C, C++, Objective-C, and Objective-C++ code into LLVM bitcode -- and from there into object files, using LLVM.

Other components include: the [libc++ C++ standard library](https://libcxx.llvm.org/), the [LLD linker](https://lld.llvm.org/), and more.

## Getting the Source Code and Building LLVM

This is an example work-flow and configuration to get and build the LLVM source:

* Checkout LLVM (including related sub-projects like Clang): `git clone https://github.com/llvm/llvm-project.git`
* Configure and build LLVM and Clang

```
cd llvm-project

cmake -S llvm -B build -G <generator> [options]

cmake --build build [-- [options] <target>]
```



# 安装 Clang


GCC 安装脚本：

https://github.com/owent-utils/bash-shell/tree/main/GCC%20Installer

Clang 安装脚本：

https://github.com/owent-utils/bash-shell/tree/main/LLVM&Clang%20Installer

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



## Clang 11.0.0

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

> LLVM_ENABLE_PROJECTS:STRING  用法参考：https://llvm.org/docs/CMake.html
>
> Semicolon-separated list of projects to build, or all for building all (clang, lldb, lld, polly, etc) projects. This flag assumes that projects are checked out side-by-side and not nested, i.e. clang needs to be in parallel of llvm instead of nested in llvm/tools. This feature allows to have one build for only LLVM and another for clang+llvm using the same source checkout. The full list is: clang;clang-tools-extra;cross-project-tests;libc;libclc;lld;lldb;openmp;polly;pstl


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

# 升级 GLIBC


参考 [glibc升级到2.29](https://www.jianshu.com/p/f4d603967e1d) 升级 glibc 可行。

例如，升级 glibc 2.18

``` bash
mkdir ~/glibc_install; cd ~/glibc_install
wget http://ftp.gnu.org/gnu/glibc/glibc-2.18.tar.gz
tar zxvf glibc-2.18.tar.gz
cd glibc-2.18
mkdir build
cd build

# 注意 --prefix=/usr
../configure --prefix=/usr --disable-profile --enable-add-ons --with-headers=/usr/include --with-binutils=/usr/bin

make -j4
sudo make install
```

验证：

```
$ ll /lib64/libc.so.6
lrwxrwxrwx 1 root root 12 10月 11 21:33 /lib64/libc.so.6 -> libc-2.18.so
$ strings /lib64/libc.so.6 | grep ^GLIBC
GLIBC_2.2.5
GLIBC_2.2.6
GLIBC_2.3
GLIBC_2.3.2
GLIBC_2.3.3
GLIBC_2.3.4
GLIBC_2.4
GLIBC_2.5
GLIBC_2.6
GLIBC_2.7
GLIBC_2.8
GLIBC_2.9
GLIBC_2.10
GLIBC_2.11
GLIBC_2.12
GLIBC_2.13
GLIBC_2.14
GLIBC_2.15
GLIBC_2.16
GLIBC_2.17
GLIBC_2.18
GLIBC_PRIVATE
GLIBC_2.8
GLIBC_2.5
GLIBC_2.9
GLIBC_2.7
GLIBC_2.6
GLIBC_2.18
GLIBC_2.11
GLIBC_2.16
GLIBC_2.10
GLIBC_2.17
GLIBC_2.13
GLIBC_2.2.6
```

refer:

* [Multiple glibc libraries on a single host](https://stackoverflow.com/questions/847179/multiple-glibc-libraries-on-a-single-host)
* [How to upgrade glibc from version 2.12 to 2.14 on CentOS?](https://stackoverflow.com/questions/35616650/how-to-upgrade-glibc-from-version-2-12-to-2-14-on-centos)


# Clangd (提升C++代码编写体验)

工欲善其事，必先利其器。LLVM 项目推出的 Clangd 拥有稳定的体验、完善的功能和活跃的社区，它作为一个稳定高效的 C++ 代码插件可以成倍地提升代码编写体验，是每个C++程序员提升代码效率的不二之选。

参考 [有谁可以介绍一下谷歌大牛Jeff Dean以及与他相关的事迹么?](https://www.zhihu.com/question/22081653)

> Compilers don’t warn Jeff Dean. Jeff Dean warns compilers.
> 编译器从来不给Jeff编译警告，而是Jeff警告编译器

而对于普通人还是需要借助工具（例如，clang-tidy）来提示或规避一些常见问题。

在业界，C++ 常用的代码检查工具有 cpplint，coverity，clang-tidy（clangd 背后也是通过 clang-tidy 检查，这里是指 clang-tidy 二进制本身独立使用）等，它们往往无法兼顾代码检查的完整性和实时性。如果要做到完整检查，就需要编译代码，通过代码的 IR 表示去分析逻辑流，比如 coverity，这种检查往往是 CI/CD 流水线的一部分，不能实时检查。而 cpplint 则是不编译代码，这是通过词法分析，检查代码格式不符合标准的地方或者局部的问题，没有办法检测代码的逻辑错误。如果要兼顾完整性和实时性，就需要一个编译器在后台实时地编译我们的代码，而 clangd 恰好就提供这种功能。

clangd 是 LLVM 项目推出的 C++ 语言服务器，通过 LSP(Language Server Protocal) 协议向编辑器如 vscode/vim/emacs 提供语法补全、错误检测、跳转、格式化等等功能。C++ 的 LSP 曾经是cquery, ccls, clangd 三足鼎立。但是 clangd 支持 clang-tidy 实时检查的功能是另外两者不具备的，而且 cquery 和 ccls 都是单个开发者主导的项目，clangd 背后则是有 LLVM 的背书。

## clangd 常用功能

* 代码跳转
* 代码补全
* 代码建议（推荐更好的现代用法）以及错误提示 (clang-tidy)
* 代码格式化 (clang-format)

## clangd 安装方式

建议下载最新的版本，可以使用最新的特性。

* 系统包管理
    + mac: brew install llvm
    + debian/ubuntu: sudo apt-get install clangd-14

* 手动下载最新的二进制
    + https://github.com/clangd/clangd/releases/tag/15.0.0

* 源码编译
    + https://github.com/llvm/llvm-project/tree/main/clang-tools-extra/clangd#building-and-testing-clangd

> 注意：
> 1. 在 vscode 扩展中，clangd 比 C/C++ 扩展执行效率更高且功能更丰富，但是 clangd 需要使用 GLIBC_2.18 版本。
> 2. clangd 可以单独下载然后在 vscode 配置中指定安装路径，也可以通过 vscode 的 clangd 扩展自动提示下载。

## clangd 作为 vscode 扩展的配置方法

vscode 上使用 clangd 的配置 (项目根目录 .vscode/settings.json )

```
"clangd.arguments": [
    "--clang-tidy",                 // 开启clang-tidy
    "--all-scopes-completion",      // 全代码库补全
    "--completion-style=detailed",  // 详细补全
    "--header-insertion=iwyu",
    "--pch-storage=disk",           // 如果内存够大可以关闭这个选项
    "--log=error",
    "--j=5",                        // 后台线程数，可根据机器配置自行调整
    "--background-index"
  ],
"clangd.path": "xxx", // 安装的clangd地址
"[cpp]": {
    "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd"
},
```

clangd 需要知道如何编译你的项目，因此需要一个“编译数据库”，通常情况下我们需要向 clangd 提供一个 `compile_commands.json` 文件，这个文件的生成需要依赖你的编译系统.

参考 https://clangd.llvm.org/installation.html Project setup 一节说明：

To understand your source code, clangd needs to know your build flags. (This is just a fact of life in C++, source files are not self-contained).

`compile_commands.json` - This file provides compile commands for every source file in a project. It is usually generated by tools.

clangd will look in the parent directories of the files you edit looking for it, and also in subdirectories named `build/`. For example, if editing `$SRC/gui/window.cpp`, we search in `$SRC/gui/`, `$SRC/gui/build/`, `$SRC/`, `$SRC/build/`, ...

CMake-based projects

If your project builds with CMake, it can generate this file. You should enable it with:

```
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1
```

`compile_commands.json` will be written to your build directory. If your build directory is `$SRC` or `$SRC/build`, clangd will find it. Otherwise, symlink or copy it to `$SRC`, the root of your source tree.

```
ln -s ~/myproject-build/compile_commands.json ~/myproject/
```

## clang-tidy

> clang-tidy is a clang-based C++ “linter” tool. Its purpose is to provide an extensible framework for diagnosing and fixing typical programming errors, like style violations, interface misuse, or bugs that can be deduced via static analysis. clang-tidy is modular and provides a convenient interface for writing new checks.

Clang 项目也提供了其他一些工具，包括代码的静态检查工具 [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/) 。这是一个比较全面的工具，它除了会提示你危险的用法，也会告诉你如何去现代化你的代码。默认情况下，Clang-Tidy 只做基本的分析。你也可以告诉它你想现代化你的代码和提高代码的可读性：

```
clang-tidy --checks='clang-analyzer-*,modernize-*,readability-*' test.cpp
```

### 配置方法

在 vscode 通过配置 clangd + clang-tidy + Error Lens 插件，实现代码检查和提示。在项目根目录创建 `.clang-tidy` 配置文件。

`.clang-tidy `配置内容如下，可根据[官方定义的规则](https://clang.llvm.org/extra/clang-tidy/checks/list.html)进行自定义，同时也可以参考其他开源项目的配置示例。

```
---
# @refer
# https://clang.llvm.org/extra/clang-tidy/checks/list.html
# https://github.com/googleapis/google-cloud-cpp/blob/main/.clang-tidy
# https://github.com/apache/arrow/blob/master/.clang-tidy

Checks: >
  -*,
  bugprone-*,
  google-*,
  misc-*,
  modernize-*,
  performance-*,
  portability-*,
  readability-*,
  -modernize-use-trailing-return-type,
  -bugprone-lambda-function-name,
  -google-runtime-references,
  -readability-magic-numbers,
  -bugprone-easily-swappable-parameters,
  -readability-identifier-length,
  -bugprone-narrowing-conversions,
# Turn all the warnings from the checks above into errors.
WarningsAsErrors: "performance-*, bugprone-*"

CheckOptions:
  - { key: readability-identifier-naming.NamespaceCase, value: lower_case }
  - { key: readability-identifier-naming.ClassCase, value: CamelCase }
  - { key: readability-identifier-naming.StructCase, value: CamelCase }
  - { key: readability-identifier-naming.TemplateParameterCase, value: aNy_CasE }
  - { key: readability-identifier-naming.FunctionCase, value: aNy_CasE }
  - { key: readability-identifier-naming.VariableCase, value: lower_case }
  - { key: readability-identifier-naming.ClassMemberCase, value: lower_case }
  - { key: readability-identifier-naming.ClassMemberSuffix, value: _ }
  - { key: readability-identifier-naming.PrivateMemberSuffix, value: _ }
  - { key: readability-identifier-naming.ProtectedMemberSuffix, value: _ }
  - { key: readability-identifier-naming.EnumConstantCase, value: CamelCase }
  - { key: readability-identifier-naming.EnumConstantPrefix, value: k }
  - { key: readability-identifier-naming.ConstexprVariableCase, value: CamelCase }
  - { key: readability-identifier-naming.ConstexprVariablePrefix, value: k }
  - { key: readability-identifier-naming.GlobalConstantCase, value: CamelCase }
  - { key: readability-identifier-naming.GlobalConstantPrefix, value: k }
  - { key: readability-identifier-naming.MemberConstantCase, value: CamelCase }
  - { key: readability-identifier-naming.MemberConstantPrefix, value: k }
  - { key: readability-identifier-naming.StaticConstantCase, value: CamelCase }
  - { key: readability-identifier-naming.StaticConstantPrefix, value: k }
  - { key: misc-non-private-member-variables-in-classes.IgnoreClassesWithAllMemberVariablesBeingPublic, value: 1 }
  - { key: readability-function-cognitive-complexity.Threshold, value: 100 }
```

* [Setting a sub-option to clang-tidy](https://stackoverflow.com/questions/53185985/setting-a-sub-option-to-clang-tidy)


### Suppressing Undesired Diagnostics

参考：https://clang.llvm.org/extra/clang-tidy/#suppressing-undesired-diagnostics

clang-tidy diagnostics are intended to call out code that does not adhere(遵守) to a coding standard, or is otherwise problematic in some way. However, if the code is known to be correct, it may be useful to silence the warning. Some clang-tidy checks provide a check-specific way to silence the diagnostics, e.g. [bugprone-use-after-move](https://clang.llvm.org/extra/clang-tidy/checks/bugprone-use-after-move.html) can be silenced by re-initializing the variable after it has been moved out, [bugprone-string-integer-assignment](https://clang.llvm.org/extra/clang-tidy/checks/bugprone-string-integer-assignment.html) can be suppressed by explicitly casting the integer to char, [readability-implicit-bool-conversion](https://clang.llvm.org/extra/clang-tidy/checks/readability-implicit-bool-conversion.html) can also be suppressed by using explicit casts, etc.

If a specific suppression mechanism is not available for a certain warning, or its use is not desired for some reason, clang-tidy has a generic mechanism to suppress diagnostics using `NOLINT`, `NOLINTNEXTLINE`, and `NOLINTBEGIN` ... `NOLINTEND` comments.

The `NOLINT` comment instructs clang-tidy to ignore warnings on the same line (it doesn’t apply to a function, a block of code or any other language construct; it applies to the line of code it is on). If introducing the comment on the same line would change the formatting in an undesired way, the `NOLINTNEXTLINE` comment allows suppressing clang-tidy warnings on the next line. The `NOLINTBEGIN` and `NOLINTEND` comments allow suppressing clang-tidy warnings on multiple lines (affecting all lines between the two comments).

All comments can be followed by an optional list of check names in parentheses (see below for the formal syntax). The list of check names supports globbing, with the same format and semantics as for enabling checks. Note: negative globs are ignored here, as they would effectively re-activate the warning.

For example:

``` cpp
class Foo {
  // Suppress all the diagnostics for the line
  Foo(int param); // NOLINT

  // Consider explaining the motivation to suppress the warning
  Foo(char param); // NOLINT: Allow implicit conversion from `char`, because <some valid reason>

  // Silence only the specified checks for the line
  Foo(double param); // NOLINT(google-explicit-constructor, google-runtime-int)

  // Silence all checks from the `google` module
  Foo(bool param); // NOLINT(google*)

  // Silence all checks ending with `-avoid-c-arrays`
  int array[10]; // NOLINT(*-avoid-c-arrays)

  // Silence only the specified diagnostics for the next line
  // NOLINTNEXTLINE(google-explicit-constructor, google-runtime-int)
  Foo(bool param);

  // Silence all checks from the `google` module for the next line
  // NOLINTNEXTLINE(google*)
  Foo(bool param);

  // Silence all checks ending with `-avoid-c-arrays` for the next line
  // NOLINTNEXTLINE(*-avoid-c-arrays)
  int array[10];

  // Silence only the specified checks for all lines between the BEGIN and END
  // NOLINTBEGIN(google-explicit-constructor, google-runtime-int)
  Foo(short param);
  Foo(long param);
  // NOLINTEND(google-explicit-constructor, google-runtime-int)

  // Silence all checks from the `google` module for all lines between the BEGIN and END
  // NOLINTBEGIN(google*)
  Foo(bool param);
  // NOLINTEND(google*)

  // Silence all checks ending with `-avoid-c-arrays` for all lines between the BEGIN and END
  // NOLINTBEGIN(*-avoid-c-arrays)
  int array[10];
  // NOLINTEND(*-avoid-c-arrays)
};
```

### 基于 clang-tidy AST 语法树代码自定义检查


* 场景：标准工具覆盖不到的场景

![clang-tidy-extend2](/assets/images/202306/clang-tidy-extend2.png)


* 生成规则代码

```
$ cd llvm-project-11.0.0/clang-tools-extra/clang-tidy
$ ./add_new_check.py performance JLibTest
$ cd performance
$ ls JlibtestCheck.*
JlibtestCheck.cpp  JlibtestCheck.h
```

![clang-tidy-extend](/assets/images/202306/clang-tidy-extend.png)


* 实现代码检查规则



``` cpp
// JlibtestCheck.h

//===--- JlibtestCheck.h - clang-tidy ---------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_PERFORMANCE_JLIBTESTCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_PERFORMANCE_JLIBTESTCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace performance {

/// FIXME: Write a short description.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/performance-JLibTest.html
class JlibtestCheck : public ClangTidyCheck {
public:
  JlibtestCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace performance
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_PERFORMANCE_JLIBTESTCHECK_H
```

``` cpp
// JlibtestCheck.cpp

//===--- JlibtestCheck.cpp - clang-tidy -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "JlibtestCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace performance {

void JlibtestCheck::registerMatchers(MatchFinder *Finder) {
  // FIXME: Add matchers.
  //Finder->addMatcher(functionDecl().bind("x"), this);

  // Matcher for for-loops having a call to strlen in the loop condition.
  //Finder->addMatcher(forStmt(hasCondition(hasDescendant(callExpr(callee(functionDecl(hasName("strlen"))))))).bind("myForLoop"), this);
  Finder->addMatcher(
      forStmt(hasCondition(hasDescendant(
          callExpr(callee(functionDecl(hasName("strlen")))))))
          .bind("myForLoop"),
      this);
}

void JlibtestCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.

  // Retrieve the matched for-loop node.
  const ForStmt *myForLoop = Result.Nodes.getNodeAs<ForStmt>("myForLoop");

  // If for some reason it's not retrieved, return early.
  if (!myForLoop) {
    return;
  }

  // Initialize a diagnostic message.
  diag(myForLoop->getBeginLoc(), "for-loop with strlen() called in loop condition found");

  // Apply possible fixes, if necessary.


/*
  const auto *MatchedDecl = Result.Nodes.getNodeAs<FunctionDecl>("x");
  if (MatchedDecl->getName().startswith("awesome_"))
    return;
  diag(MatchedDecl->getLocation(), "function %0 is insufficiently awesome")
      << MatchedDecl;
  diag(MatchedDecl->getLocation(), "insert 'awesome'", DiagnosticIDs::Note)
      << FixItHint::CreateInsertion(MatchedDecl->getLocation(), "awesome_");
*/
}

} // namespace performance
} // namespace tidy
} // namespace clang
```


* 构建编译 clang-tidy

```
$ cd llvm-project-11.0.0/build
$ cmake -DLLVM_ENABLE_PROJECTS="clang-tools-extra" -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../llvm
$ make -j16
$ cd bin
$ ls clang-tidy
clang-tidy
```

* 使用方法

```
$ cd llvm-project-11.0.0/build/bin
$ ./clang-tidy --checks="-*,performance-JLibTest" test.cc
```

![clang-tidy-extend3](/assets/images/202306/clang-tidy-extend3.png)


* 更多参考
  + [2022 全球 C++ 及系统软件技术大会 - C++ 性能、工具、库 - 陈硕](https://chenshuo.com/data/summit2022.pdf)
  + [How to build a C++ processing tool using the Clang libraries](https://static.linaro.org/connect/yvr18/presentations/yvr18-223.pdf)
  + [AST Matcher Reference](https://clang.llvm.org/docs/LibASTMatchersReference.html)
  + https://zhuanlan.zhihu.com/p/573342695


## clang-format 配置方法

同样，可以在项目根目录下添加`.clang-format`文件，实现代码的自动格式化。

可以参考：[CPP ClangFormat in Action](http://gerryyang.com/c/c++/2022/04/13/cpp-clangformat-in-action.html)


## Q&A

### 禁止自动插入包含头文件

[Any option to disable auto headers import? (clangd-9)](https://github.com/clangd/clangd/issues/55)

There's a flag: `-header-insertion=never`. You should be able to set you editor to pass that flag to clangd.

### clangd 内存开销过大问题

[Excessive memory consumption #251](https://github.com/clangd/clangd/issues/251)

[Clion clangd, how to limit memory usage](https://stackoverflow.com/questions/61346400/clion-clangd-how-to-limit-memory-usage)

* 设置 `--background-index` 选项 (Index project code in the background and persist index on disk)
* 设置 `--pch-storage=disk` 选项 (Storing PCHs in memory increases memory usages, but may improve performance)

Default limit for clangd is 8GB, but you can easily customize it. BTW, clangd works only with opened files. So as few files opened in editor as less memory clangd eats.

### 将 preamble-xxx.pch 默认的 root 输出目录改为用户目录

vscode 的 clangd 插件在 disk 模式下会在 /tmp 目录下产生大量的 preamble-xxx.pch 的文件，导致 / 挂载盘空间占满。
解决方案：(将 tmp 文件重定向到用户 data 目录下)
1. ~/.bash_profile 中加上 export TMPDIR=$HOME/tmp
2. kill 掉服务器上的 vscode 进程，重新连接进入



## 其他参考

* https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd
* https://github.com/llvm/llvm-project/tree/main/clang-tools-extra/clangd#building-and-testing-clangd
* https://github.com/clangd/clangd/releases/tag/15.0.1
* https://clangd.llvm.org/installation.html
* https://manpages.debian.org/experimental/clangd/clangd.1.en.html


# Diagnostic (诊断)

https://clang.llvm.org/docs/DiagnosticsReference.html#diagnostic-flags


# Manual

* http://developer.amd.com/wordpress/media/2013/12/AOCC-1.1-Clang-the-C-C-Compiler.pdf
