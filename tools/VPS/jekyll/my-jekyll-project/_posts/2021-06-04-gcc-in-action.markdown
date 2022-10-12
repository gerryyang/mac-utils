---
layout: post
title:  "GCC in Action"
date:   2021-06-04 08:00:00 +0800
categories: [GCC/Clang]
---

# GCC (GNU project C and C++ compiler)

* Do not remove this line (it will not be displayed)
{:toc}

When you invoke `GCC`, it normally does preprocessing, compilation, assembly and linking. The "overall options" allow you to stop this process at an intermediate stage. For example, the `-c` option says not to run the linker. Then the output consists of object files output by the assembler.

Other options are passed on to one stage of processing. Some options control the preprocessor and others the compiler itself. Yet other options control the assembler and linker; most of these are not documented here, since you rarely need to use any of them.

Most of the command-line options that you can use with GCC are useful for C programs; when an option is only useful with another language (usually C++), the explanation says so explicitly. If the description for a particular option does not mention a source language, you can use that option with all supported languages.

The gcc program accepts options and file names as operands. Many options have multi-letter names; therefore multiple single-letter options may not be grouped: -dv is very different from -d -v.

You can mix options and other arguments. For the most part, the order you use doesn't matter. Order does matter when you use several options of the same kind; for example, if you specify `-L` more than once, the directories are searched in the order specified. Also, the placement of the `-l` option is significant.

Many options have long names starting with `-f` or with `-W---for` example, `-fmove-loop-invariants`, `-Wformat` and so on. Most of these have both positive and negative forms; the negative form of `-ffoo` is `-fno-foo`. This manual documents only one of these two forms, whichever one is not the default.

More: man gcc


# Update GCC

查看当前环境C++版本：

```
$ ls -l /lib64/libstdc++.so.6
lrwxrwxrwx 1 root root 19 Aug 18  2020 /lib64/libstdc++.so.6 -> libstdc++.so.6.0.25
$ rpm -qf /lib64/libstdc++.so.6
libstdc++-8.3.1-5.el8.0.2.x86_64
```

## CentOS

Often people want the most recent version of gcc, and [devtoolset](https://www.softwarecollections.org/en/scls/rhscl/devtoolset-6/) is being kept up-to-date, so maybe you want devtoolset-N where `N={4,5,6,7...}`, check yum for the latest available on your system. Updated the cmds below for N=7.

> Developer Toolset 6
>
> devtoolset-6 - Developer Toolset is designed for developers working on CentOS or Red Hat Enterprise Linux platform. It provides current versions of the GNU Compiler Collection, GNU Debugger, and other development, debugging, and performance monitoring tools.

* [List of Software Collections available in SCLo SIG](https://wiki.centos.org/SpecialInterestGroup/SCLo/CollectionsList)
* [Red Hat Software Collections Product Life Cycle](https://access.redhat.com/support/policy/updates/rhscl/)

``` bash
# 1. Install a package with repository for your system:
# On CentOS, install package centos-release-scl available in CentOS repository:
$ sudo yum install centos-release-scl

# 2. Install the collection:
$ sudo yum install devtoolset-6

# 3. Start using software collections:
$ scl enable devtoolset-6 bash
```

At this point you should be able to use `gcc` and other tools just as a normal application. See examples bellow:

```
$ gcc hello.c
$ sudo yum install devtoolset-6-valgrind
$ valgrind ./a.out
$ gdb ./a.out
```

In order to view the individual components included in this collection, including additional development tools, you can run:

```
$ sudo yum list devtoolset-6\*
```

Developer Toolset Software Collections as Docker Formatted Containers

On CentOS 7 and RHEL 7 you can pull the images with the following commands:

```
$ docker pull registry.access.redhat.com/rhscl/devtoolset-6-perftools-rhel7
$ docker pull registry.access.redhat.com/rhscl/devtoolset-6-toolchain-rhel7
$ docker pull centos/devtoolset-6-perftools-centos7
$ docker pull centos/devtoolset-6-toolchain-centos7
```

For more on the docker images follow the link to public source repository: https://github.com/sclorg/devtoolset-container

设置使用`gcc7`的步骤：

> 注意： When I open a new terminal, it is still v4.8.5. You need to `scl enable devtoolset-7 bash` every time you open a new shell, or add it to your `*rc`.

```
# enable gcc7

sudo yum install centos-release-scl
sudo yum install devtoolset-7-gcc*

# sudo yum list devtoolset-7\*
已加载插件：fastestmirror, ovl
Loading mirror speeds from cached hostfile
已安装的软件包
devtoolset-7-binutils.x86_64                                                                                                2.28-11.el7                                                                                       @centos-sclo-rh
devtoolset-7-gcc.x86_64                                                                                                     7.3.1-5.16.el7                                                                                    @centos-sclo-rh
devtoolset-7-gcc-c++.x86_64                                                                                                 7.3.1-5.16.el7                                                                                    @centos-sclo-rh
devtoolset-7-gcc-gdb-plugin.x86_64                                                                                          7.3.1-5.16.el7                                                                                    @centos-sclo-rh
devtoolset-7-gcc-gfortran.x86_64                                                                                            7.3.1-5.16.el7                                                                                    @centos-sclo-rh
devtoolset-7-gcc-plugin-devel.x86_64                                                                                        7.3.1-5.16.el7                                                                                    @centos-sclo-rh
devtoolset-7-libquadmath-devel.x86_64                                                                                       7.3.1-5.16.el7                                                                                    @centos-sclo-rh
devtoolset-7-libstdc++-devel.x86_64                                                                                         7.3.1-5.16.el7                                                                                    @centos-sclo-rh
devtoolset-7-runtime.x86_64                                                                                                 7.1-4.el7                                                                                         @centos-sclo-rh

// 切换不同版本
$ scl enable devtoolset-7 bash

$ which gcc
$ gcc --version
```

详细的输出：

```
~$gcc -v
使用内建 specs。
COLLECT_GCC=/bin/gcc
COLLECT_LTO_WRAPPER=/usr/libexec/gcc/x86_64-redhat-linux/4.8.5/lto-wrapper
目标：x86_64-redhat-linux
配置为：../configure --prefix=/usr --mandir=/usr/share/man --infodir=/usr/share/info --with-bugurl=http://bugzilla.redhat.com/bugzilla --enable-bootstrap --enable-shared --enable-threads=posix --enable-checking=release --with-system-zlib --enable-__cxa_atexit --disable-libunwind-exceptions --enable-gnu-unique-object --enable-linker-build-id --with-linker-hash-style=gnu --enable-languages=c,c++,objc,obj-c++,java,fortran,ada,go,lto --enable-plugin --enable-initfini-array --disable-libgcj --with-isl=/builddir/build/BUILD/gcc-4.8.5-20150702/obj-x86_64-redhat-linux/isl-install --with-cloog=/builddir/build/BUILD/gcc-4.8.5-20150702/obj-x86_64-redhat-linux/cloog-install --enable-gnu-indirect-function --with-tune=generic --with-arch_32=x86-64 --build=x86_64-redhat-linux
线程模型：posix
gcc 版本 4.8.5 20150623 (Red Hat 4.8.5-39) (GCC)
$which gcc
/usr/lib64/ccache/gcc
$ll -lh `which gcc`
lrwxrwxrwx 1 root root 16 3月   5 2021 /usr/lib64/ccache/gcc -> ../../bin/ccache
$ll -lh /usr/bin/ccache
-rwxr-xr-x 1 root root 135K 2月  19 2020 /usr/bin/ccache

~$scl enable devtoolset-7 bash
~$which gcc
/opt/rh/devtoolset-7/root/usr/bin/gcc
~$gcc -v
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/opt/rh/devtoolset-7/root/usr/libexec/gcc/x86_64-redhat-linux/7/lto-wrapper
Target: x86_64-redhat-linux
Configured with: ../configure --enable-bootstrap --enable-languages=c,c++,fortran,lto --prefix=/opt/rh/devtoolset-7/root/usr --mandir=/opt/rh/devtoolset-7/root/usr/share/man --infodir=/opt/rh/devtoolset-7/root/usr/share/info --with-bugurl=http://bugzilla.redhat.com/bugzilla --enable-shared --enable-threads=posix --enable-checking=release --enable-multilib --with-system-zlib --enable-__cxa_atexit --disable-libunwind-exceptions --enable-gnu-unique-object --enable-linker-build-id --with-gcc-major-version-only --enable-plugin --with-linker-hash-style=gnu --enable-initfini-array --with-default-libstdcxx-abi=gcc4-compatible --with-isl=/builddir/build/BUILD/gcc-7.3.1-20180303/obj-x86_64-redhat-linux/isl-install --enable-libmpx --enable-gnu-indirect-function --with-tune=generic --with-arch_32=i686 --build=x86_64-redhat-linux
Thread model: posix
gcc version 7.3.1 20180303 (Red Hat 7.3.1-5) (GCC)
```

## Ubuntu

```
update-alternatives --query gcc

sudo update-alternatives --config gcc

sudo apt-get install gcc-4.8 g++-4.8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50 --slave /usr/bin/g++ g++ /usr/bin/g++-4.8 --slave /usr/bin/gcov gcov /usr/bin/gcov-4.8

sudo apt install gcc-8 g++-8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 80 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gcov gcov /usr/bin/gcov-8
```

切换不同的版本：

```
# sudo update-alternatives --config gcc
There are 6 choices for the alternative gcc (providing /usr/bin/gcc).

  Selection    Path              Priority   Status
------------------------------------------------------------
  0            /usr/bin/gcc-10    90        auto mode
  1            /usr/bin/gcc-10    90        manual mode
* 2            /usr/bin/gcc-11    89        manual mode
  3            /usr/bin/gcc-4.8   50        manual mode
  4            /usr/bin/gcc-5     50        manual mode
  5            /usr/bin/gcc-8     80        manual mode
  6            /usr/bin/gcc-9     60        manual mode
```

refer:

* https://stackoverflow.com/questions/36327805/how-to-install-gcc-5-3-with-yum-on-centos-7-2
* https://linuxize.com/post/how-to-install-gcc-on-ubuntu-20-04/


# Basic

## gcc的基本用法

使用gcc编译器时，必须给出一系列必要的调用参数和文件名称。不同参数的先后顺序对执行结果没有影响，只有在使用同类参数时的先后顺序才需要考虑。如果使用了多个 `-L` 的参数来定义库目录，gcc会根据多个 `-L` 参数的先后顺序来执行相应的库目录。因为很多gcc参数都由多个字母组成，所以gcc参数不支持单字母的组合，Linux中常被叫短参数（short options），如 `-dr` 与 `-d -r` 的含义不一样。gcc编译器的调用参数大约有100多个，其中多数参数我们可能根本就用不到，这里只介绍其中最基本、最常用的参数。

gcc最基本的用法是：`gcc [options] [filenames]`

其中，`options`就是编译器所需要的参数，`filenames`给出相关的文件名称，最常用的有以下参数：

* `-c`

只编译，不链接成为可执行文件。编译器只是由输入的 `.c` 等**源代码文件**生成 `.o` 为后缀的**目标文件**，通常用于编译不包含主程序的子程序文件。

* `-o output_filename` 

确定输出文件的名称为`output_filename`。同时这个名称不能和源文件同名。如果不给出这个选项，gcc就给出默认的可执行文件 `a.out`。

* `-g`

产生符号调试工具（GNU的 `gdb`）所必要的符号信息。想要对源代码进行调试，就必须加入这个选项。

* `-O`

对程序进行优化编译、链接。采用这个选项，整个源代码会在编译、链接过程中进行优化处理，这样产生的可执行文件的执行效率可以提高，但是编译、链接的速度就相应地要慢一些，而且对执行文件的调试会产生一定的影响，造成一些执行效果与对应源文件代码不一致等一些令人“困惑”的情况。因此，一般在编译输出软件发行版时使用此选项。

* `-O2`

比 `-O` 更好的优化编译、链接。当然整个编译链接过程会更慢。

* `-Idirname`

将 `dirname` 所指出的目录加入到程序头文件目录列表中，是在预编译过程中使用的参数。

> 说明：C程序中的头文件包含两种情况：

``` cpp
#include <stdio.h>
#include "stdio.h"
```

其中，使用尖括号（`<>`），预处理程序 `cpp` 在系统默认包含文件目录（如`/usr/include`）中搜索相应的文件；使用双引号，预处理程序 `cpp` 首先在当前目录中搜寻头文件，如果没有找到，就到指定的 `dirname` 目录中去寻找。在程序设计中，如果需要的这种包含文件分别分布在不同的目录中，就需要逐个使用 `-I` 选项给出搜索路径。

* `-Ldirname`

将`dirname`所指出的目录加入到程序函数库文件的目录列表中，是在链接过程中使用的参数。在默认状态下，链接程序 `ld` 在系统默认路径中（如 `/usr/lib`）寻找所需要的库文件。这个选项告诉链接程序，首先到 `-L` 指定的目录中去寻找，然后到系统默认路径中寻找；如果函数库存放在多个目录下，就需要依次使用这个选项，给出相应的存放目录。

* `-lname`

链接时装载名为 `libname.a` 的函数库。该函数库位于系统默认的目录或者由 `-L` 选项确定的目录下。例如，`-lm` 表示链接名为 `libm.a` 的数学函数库。

例子：假定有一个程序名为 `test.c` 的C语言源代码文件，要生成一个可执行文件。

``` c
#include <stdio.h>
int main(void)
{
    printf("Hello world/n");
    return 0;
}
```

最简单的办法：`gcc test.c -o test`

首先，gcc需要调用预处理程序 `cpp`，由它负责展开在源文件中定义的宏，并向其中插入`#include`语句所包含的内容；接着，gcc调用 `ccl` 和 `as`，将处理后的源代码编译成目标代码；最后，gcc调用链接程序 `ld`，把生成的目标代码链接成一个可执行程序。因此，默认情况下，预编译、编译链接一次完成。

编译过程的分步执行：

为了更好地理解gcc的工作过程，我们可以让在gcc工作的**4个阶段**中的任何一个阶段中停止下来。相关的参数有：

* `-E`

预编译后停下来，生成后缀为 `.i` 的**预编译文件**。

* `-c`

编译后停下来，生成后缀为 `.o` 的**目标文件**。

* `-S`

汇编后停下来，生成后缀为 `.s` 的**汇编源文件**。

第一步：进行预编译，使用 `-E` 参数

```
gcc -E test.c -o test.i
```
查看 `test.i` 文件中的内容，会发现 `stdio.h` 的内容确实都插到文件里去了，而其他应当被预处理的宏定义也都做了相应的处理。

第二步：将 `test.i` 编译为目标代码，使用 `-c` 参数

```
gcc -c test.c -o test.o
```

第三步：生成汇编源文件

```
gcc -S test.c -o test.s
```

第四步：将生成的目标文件链接成可执行文件

```
gcc test.o - o test
```

对于稍微复杂的情况，比如有多个源代码文件、需要链接库或有其他比较特别的要求，就要给定适当的调用选项参数。

例子：整个源代码程序由两个文件 `testmain.c` 和 `testsub.c` 组成，程序中使用了系统提供的数学库（所有与浮点相关的数学运算都必须使用数学库）。

```
gcc testmain.c testsub.c -lm -o test
```

其中，`-lm` 表示链接系统的数学库 `libm.a` 。

> 说明：在编译一个包含许多源文件的工程时，若只用一条gcc命令来完成编译是非常浪费时间的。假如项目中有100个源文件需要编译，并且每个源文件中都包含一万行代码，如果像上面那样仅用一条gcc命令来完成编译工作，那么gcc需要将每个源文件都重新编译一遍，然后再全部链接起来。很显然，这样浪费的时间相当多，尤其是当用户只是修改了其中某个文件的时候，完全没有必要将每个文件都重新编译一遍，因为很多已经生成的目标文件是不会发生改变的。要解决这个问题，需要借助像`make`这样的工具。

## 警告提示功能选项

gcc包含完整的出错检查和警告提示功能，它们可以帮助Linux程序员写出更加专业的代码。

* `-pedantic` 选项

当gcc在编译不符合`ANSI/ISO C` 语言标准的源代码时，将产生相应的警告信息。

``` c
#include <stdio.h>
void main(void)
{
    long long int var = 1;
    printf("It is not standard C code!/n");
}
```

它有以下问题：
1. `main` 函数的返回值被声明为 `void`，但实际上应该是 `int`。
2. 使用了 GNU 语法扩展，即使用 `long long` 来声明64位整数，不符合 `ANSI/ISO C` 语言标准。
3. `main` 函数在终止前没有调用 `return` 语句。

* `-Wall` 选项

除了 `-pedantic` 之外，gcc 还有一些其他编译选项，也能够产生有用的警告信息。这些选项大多以 `-W` 开头。其中最有价值的当数 `-Wall` 了，使用它能够使 gcc 产生尽可能多的警告信息。

gcc 给出的警告信息虽然从严格意义上说不能算作错误，但却和可能成为错误来源。一个优秀的程序员应该尽量避免产生警告信息，使自己的代码始终保持简洁、优美和健壮的特性。

> 建议：gcc 给出的警告信息是很有价值的，它们不仅可以帮助程序员写出更加健壮的程序，而且还是跟踪和调试程序的有力工具。建议在用 gcc 编译源代码时始终带上 `-Wall` 选项，并把它逐渐培养成一种习惯，这对找出常见的隐式编程错误很有帮助。

* `-Werror` 选项

在处理警告方面，另一个常用的编译选项是 `-Werror`。它要求 gcc 将所有的警告当成错误进行处理，这在使用自动编译工具（如 `Make` 等）时非常有用。如果编译时带上 `-Werror` 选项，那么 gcc 会在所有产生警告的地方停止编译，迫使程序员对自己的代码进行修改。只有当相应的警告信息消除时，才可能将编译过程继续朝前推进。

* `-Wcast-align` 选项

当源程序中地址不需要对齐的指针指向一个地址需要对齐的变量地址时，则产生一个警告。例如，`char *` 指向一个 `int *` 地址，而通常在机器中 int 变量类型是需要地址能被2或4整除的对齐地址。

* 其他常用选项
    + `-v`                  输出 gcc 工作的详细过程
    + `--target-help`       显示目前所用的gcc支持CPU类型
    + `-Q`                  显示编译过程的统计数据和每一个函数名


## 库操作选项

在Linux下开发软件时，完全不使用第三方函数库的情况是比较少见的，通常来讲都需要借助一个或多个函数库的支持才能够完成相应的功能。从程序员的角度看，函数库实际上就是一些头文件（`.h`）和库文件（`.so` 或 `.a`）的集合。虽然Linux下的大多数函数都默认将头文件放到 `/usr/include/` 目录下，而库文件则放到 `/usr/lib/` 目录下，但并不是所有的情况都是这样。正因如此，gcc 在编译时必须有自己的办法来查找所需要的头文件和库文件。常用的方法有：

* `-I`

可以向 gcc 的头文件搜索路径中添加新的目录。

* `-L`

如果使用了不在标准位置的库文件，那么可以通过 `-L` 选项向 gcc 的库文件搜索路径中添加新的目录。

* `-l`

Linux下的库文件在命名时有一个约定，就是应该以 `lib` 这3个字母开头，由于所有的库文件都遵循了同样的规范，因此在用 `-l` 选项指定链接的库文件名时可以省去 lib 这3个字母。例如，gcc 在对 `-lfoo` 进行处理时，会自动去链接名为 `libfoo.so` 的文件。

* `-static`

Linux下的库文件分为两大类，分别是：动态链接库（通常以 `.so` 结尾）和静态链接库（通常以 `.a` 结尾）。两者的差别仅在程序执行时所需的代码是在运行时动态加载的，还是在编译时静态加载的。默认情况下，gcc 在链接时优先使用动态链接库，只有当动态链接库不存在时才考虑使用静态链接库。如果需要的话，可以在编译时加上 `-static` 选项，强制使用静态链接库。

* `-shared`

生成一个共享的目标文件，它能够与其他的目标一起链接生成一个可执行的文件。

## 调试选项

对于Linux程序员来讲，gdb（GNU Debugger）通过与 gcc 的配合使用，为基于Linux的软件开发提供了一个完善的调试环境。常用的有：

* `-g` 和 `-ggdb`

默认情况下，gcc 在编译时不会将调试符号插入到生成的二进制代码中，因为这样会增加可执行文件的大小。如果需要在编译时生成调试符号信息，可以使用 gcc 的 `-g` 或 `-ggdb` 选项。

gcc 在产生调试符号时，同样采用了分级的思路，开发人员可以通过在 `-g` 选项后附加数字1、2、3指定在代码中加入调试信息的多少。**默认的级别是2（`-g2`），此时产生的调试信息包括：扩展的符号表、行号、局部或外部变量信息**。级别3（`-g3`）包含级别2中的所有调试信息以及源代码中定义的宏。级别1（`-g1`）不包含局部变量和与行号有关的调试信息，因此只能够用于回溯跟踪和堆栈转储。

>回溯追踪：指的是监视程序在运行过程中函数调用历史。
>
>堆栈转储：则是一种以原始的十六进制格式保存程序执行环境的方法。
>
>注意：使用任何一个调试选项都会使最终生成的二进制文件的大小急剧增加，同时增加程序在执行时的开销，因此，调试选项通常仅在软件的开发和调试阶段使用。

* `-p` 和 `-pg`

会将剖析（Profiling）信息加入到最终生成的二进制代码中。剖析信息对于找出程序的性能瓶颈很有帮助，是协助Linux程序员开发出高性能程序的有力工具。

* `-save-temps`

保存编译过程中生成的一些列中间文件。

```
# gcc test.c -o test -save-temps
```

除了生成执行文件test之外，还保存了`test.i` 和 `test.s` 中间文件，供用户查询调试。

## 交叉编译选项

通常情况下使用 gcc 编译的目标代码都与使用的机器是一致的，但 gcc 也支持交叉编译的功能，能够编译其他不同CPU的目标代码。使用 gcc 开发嵌入式系统，我们几乎都是以通用的PC机（X86）平台来做宿主机，通过 gcc 的交叉编译功能对其他嵌入式CPU的开发任务。

## Specifying Attributes of Variables (__attribute__)

The keyword `__attribute__` allows you to specify special attributes of variables or structure fields. This keyword is followed by an attribute specification inside **double parentheses(双括号)**. Ten attributes are currently defined for variables: `aligned`, `mode`, `nocommon`, `packed`, `section`, `transparent_union`, `unused`, `deprecated`, `vector_size`, and `weak`.

section ("section-name")

Normally, the compiler places the objects it generates in sections like `data` and `bss`. Sometimes, however, you need additional sections, or you need certain particular variables to appear in special sections, for example to map to special hardware. The section attribute specifies that a variable (or function) lives in a particular section.

示例：

``` cpp
#include <iostream>
#include "mydefine.h"

int main()
{
  BIN_INFO(DESC_VER("gerry"));
}
```

``` cpp
// mydefine.h
#pragma once

#ifndef MY_VER
#define MY_VER ""
#endif

#define DESC_VER(info) MY_VER "(" info ")"

#ifndef MY_DATE
#define MY_DATE __DATE__ " " __TIME__
#endif

#define ___me(a, b) __me_ ##a ## b
#define __me(a, b) ___me(a, b)
#define __me_info(name, tag, info) \
	static const char __me(name, __LINE__)[] \
	__attribute_used__ \
	__attribute__((section(tag),unused)) = \
	"Ver_" info "-[" MY_DATE "][gcc_" __VERSION__ "]";

#define BIN_INFO_EX(name, info) __me_info(name, ".bininfo", info)
#define LIB_INFO_EX(name, info) __me_info(name, ".libinfo", info)
#define MOD_INFO_EX(name, info) __me_info(name, ".modinfo", info)

#define BIN_INFO(info) BIN_INFO_EX(bin, info)
#define LIB_INFO(info) LIB_INFO_EX(lib, info)
#define MOD_INFO(info) MOD_INFO_EX(mod, info)
```

用法解释：

* `#define __attribute_used__ __attribute__((__used__))` 表示该函数或变量可能不使用，这个属性可以避免编译器产生警告信息。



```
# gcc -E main.cc

 static const char __me_bin6[] __attribute__ ((__used__)) __attribute__((section(".bininfo"),unused)) = "Ver_" "" "(" "gerry" ")" "-[" "Jun 13 2022" " " "18:04:18" "][gcc_" "4.8.5 20150623 (Red Hat 4.8.5-39)" "]";
 ```

 ```
$objdump -s a.out | grep .bininfo -A10
Contents of section .bininfo:
 400820 5665725f 28676572 7279292d 5b4a756e  Ver_(gerry)-[Jun
 400830 20313320 32303232 2031363a 35313a33   13 2022 16:51:3
 400840 385d5b67 63635f34 2e382e35 20323031  8][gcc_4.8.5 201
 400850 35303632 33202852 65642048 61742034  50623 (Red Hat 4
 400860 2e382e35 2d333929 5d00               .8.5-39)].
Contents of section .eh_frame_hdr:
 40086c 011b033b 40000000 07000000 34fdffff  ...;@.......4...
 40087c 8c000000 a4fdffff 5c000000 91feffff  ........\.......
 40088c b4000000 abfeffff d4000000 e8feffff  ................
 40089c f4000000 04ffffff 14010000 74ffffff  ............t...
 ```


* https://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Variable-Attributes.html


# [ABI Policy and Guidelines](https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html)

## The C++ Interface

**The C++ Standard Library** has many **include files**, types defined in those include files, specific named functions, and other behavior. The text of these behaviors, as written in source include files, is called the `Application Programing Interface`, or `API`.

Furthermore, **C++ source** that is compiled into **object files** is transformed by the compiler: **it arranges objects with specific alignment and in a particular layout, mangling names according to a well-defined algorithm, has specific arrangements for the support of virtual functions**, etc. These details are defined as the compiler `Application Binary Interface`, or `ABI`. From GCC version 3 onwards the GNU C++ compiler uses an industry-standard C++ ABI, the [Itanium C++ ABI](https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html#biblio.cxxabi).

The GNU C++ compiler, g++, has a compiler command line option to switch between various different C++ ABIs. This explicit version switch is the flag `-fabi-version`. In addition, some g++ command line options may change the ABI as a side-effect of use. Such flags include `-fpack-struct` and `-fno-exceptions`, but include others: see the complete list in the GCC manual under the heading [Options for Code Generation Conventions](http://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html#Code%20Gen%20Options).

The configure options used when building a specific **libstdc++** version may also impact the resulting library ABI. The available configure options, and their impact on the library ABI, are documented [here](https://gcc.gnu.org/onlinedocs/libstdc++/manual/configure.html).

> Putting all of these ideas together results in the C++ Standard Library ABI, which is the compilation of a given library API by a given compiler ABI. In a nutshell: **library API + compiler ABI = library ABI**

**The library ABI** is mostly of interest for end-users who have unresolved symbols and are linking dynamically to the C++ Standard library, and who thus must be careful to compile their application with a compiler that is compatible with the available C++ Standard library binary. In this case, compatible is defined with the equation above: given an application compiled with a given compiler ABI and library API, it will work correctly with a Standard C++ Library created with the same constraints.

To use a specific version of the **C++ ABI**, one must use a corresponding **GNU C++ toolchain** (i.e., `g++` and `libstdc++`) that implements the **C++ ABI** in question.

## Versioning

The C++ interface has evolved throughout the history of the GNU C++ toolchain. With each release, various details have been changed so as to give distinct versions to the C++ interface.

> Goals

Extending existing, stable ABIs. Versioning gives subsequent releases of library binaries the ability to add new symbols and add functionality, all the while retaining compatibility with the previous releases in the series. Thus, program binaries linked with the initial release of a library binary will still run correctly if the library binary is replaced by carefully-managed subsequent library binaries. This is called **forward compatibility(向前兼容)**.

The reverse (**backwards compatibility(向后兼容)**) is **not true**. It is **not possible to** take program binaries linked with the latest version of a library binary in a release series (with additional symbols added), substitute in the initial release of the library binary, and remain link compatible.

**Allows multiple, incompatible ABIs to coexist at the same time**.

> History

**How can this complexity be managed?** What does C++ versioning mean? Because library and compiler changes often make binaries compiled with one version of the GNU tools **incompatible with** binaries compiled with other (either newer or older) versions of the same GNU tools, **specific techniques are used to make managing this complexity easier**.

The following techniques are used:

* Release versioning on the `libgcc_s.so` binary.

This is implemented via file names and the ELF `DT_SONAME` mechanism (at least on `ELF` systems). It is versioned as follows:

```
GCC 3.x: libgcc_s.so.1
GCC 4.x: libgcc_s.so.1
...
```

* Symbol versioning on the `libgcc_s.so` binary.

It is versioned with the following labels and version definitions, where the version definition is the maximum for a particular release. Labels are cumulative. If a particular release is not listed, it has the same version labels as the preceding release.

This corresponds to the mapfile: `gcc/libgcc-std.ver`

```
GCC 3.0.0: GCC_3.0
GCC 3.3.0: GCC_3.3
...
GCC 4.8.0: GCC_4.8.0
```

* Release versioning on the `libstdc++.so` binary, implemented in the same way as the `libgcc_s.so` binary above. Listed is the filename: `DT_SONAME` can be deduced from the filename by removing the last two period-delimited numbers. For example, filename `libstdc++.so.5.0.4` corresponds to a `DT_SONAME` of `libstdc++.so.5`. **Binaries with equivalent DT_SONAMEs are forward-compatibile**: in the table below, releases incompatible with the previous one are explicitly noted. If a particular release is not listed, its `libstdc++.so` binary has the same filename and `DT_SONAME` as the preceding release.

It is versioned as follows:

```
GCC 3.0.0: libstdc++.so.3.0.0
GCC 3.0.1: libstdc++.so.3.0.1
...
GCC 4.8.0: libstdc++.so.6.0.18
GCC 4.8.3: libstdc++.so.6.0.19
GCC 4.9.0: libstdc++.so.6.0.20
GCC 5.1.0: libstdc++.so.6.0.21
...
GCC 10.1.0: libstdc++.so.6.0.28
GCC 11.1.0: libstdc++.so.6.0.29
```

* Symbol versioning on the `libstdc++.so` binary.

mapfile: `libstdc++-v3/config/abi/pre/gnu.ver`

```
GCC 4.8.0: GLIBCXX_3.4.18, CXXABI_1.3.7
GCC 4.8.3: GLIBCXX_3.4.19, CXXABI_1.3.7
GCC 4.9.0: GLIBCXX_3.4.20, CXXABI_1.3.8
GCC 5.1.0: GLIBCXX_3.4.21, CXXABI_1.3.9
...
GCC 10.1.0: GLIBCXX_3.4.28, CXXABI_1.3.12
GCC 11.1.0: GLIBCXX_3.4.29, CXXABI_1.3.13
```

* [See More](https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html)


## Testing

> Single ABI Testing

Testing for **GNU C++ ABI changes** is composed of two distinct areas: testing **the C++ compiler (g++) for compiler changes**, and testing **the C++ library (libstdc++) for library changes**.

> Multiple ABI Testing

A "C" application, dynamically linked to two shared libraries, `liba`, `libb`. The dependent library `liba` is a C++ shared library compiled with `GCC 3.3`, and uses io, exceptions, locale, etc. The dependent library `libb` is a C++ shared library compiled with `GCC 3.4`, and also uses io, exceptions, locale, etc.

As above, `libone` is constructed as follows:

```
gcc-3.4.0/bin/g++ -fPIC -DPIC -c a.cc
gcc-3.4.0/bin/g++ -shared -Wl,-soname -Wl,libone.so.1 -Wl,-O1 -Wl,-z,defs a.o -o libone.so.1.0.0

ln -s libone.so.1.0.0 libone.so

gcc-3.4.0/bin/g++ -c a.cc

ar cru libone.a a.o
```

And, `libtwo` is constructed as follows:

```
gcc-3.3.3/bin/g++ -fPIC -DPIC -c b.cc
gcc-3.3.3/bin/g++ -shared -Wl,-soname -Wl,libtwo.so.1 -Wl,-O1 -Wl,-z,defs b.o -o libtwo.so.1.0.0

ln -s libtwo.so.1.0.0 libtwo.so

gcc-3.3.3/bin/g++ -c b.cc
ar cru libtwo.a b.o
```

with the resulting libraries looking like:

```
%ldd libone.so.1.0.0
	libstdc++.so.6 => /usr/lib/libstdc++.so.6 (0x40016000)
	libm.so.6 => /lib/tls/libm.so.6 (0x400fa000)
	libgcc_s.so.1 => /mnt/hd/bld/gcc/gcc/libgcc_s.so.1 (0x4011c000)
	libc.so.6 => /lib/tls/libc.so.6 (0x40125000)
	/lib/ld-linux.so.2 => /lib/ld-linux.so.2 (0x00355000)

%ldd libtwo.so.1.0.0
	libstdc++.so.5 => /usr/lib/libstdc++.so.5 (0x40027000)
	libm.so.6 => /lib/tls/libm.so.6 (0x400e1000)
	libgcc_s.so.1 => /mnt/hd/bld/gcc/gcc/libgcc_s.so.1 (0x40103000)
	libc.so.6 => /lib/tls/libc.so.6 (0x4010c000)
	/lib/ld-linux.so.2 => /lib/ld-linux.so.2 (0x00355000)
```

Then, the "C" compiler is used to compile a source file that uses functions from each library.

```
gcc test.c -g -O2 -L. -lone -ltwo /usr/lib/libstdc++.so.5 /usr/lib/libstdc++.so.6
```

Which gives the expected:

```
%ldd a.out
	libstdc++.so.5 => /usr/lib/libstdc++.so.5 (0x00764000)
	libstdc++.so.6 => /usr/lib/libstdc++.so.6 (0x40015000)
	libc.so.6 => /lib/tls/libc.so.6 (0x0036d000)
	libm.so.6 => /lib/tls/libm.so.6 (0x004a8000)
	libgcc_s.so.1 => /mnt/hd/bld/gcc/gcc/libgcc_s.so.1 (0x400e5000)
	/lib/ld-linux.so.2 => /lib/ld-linux.so.2 (0x00355000)
```

This resulting binary, when executed, will be able to safely use code from both `liba`, and the dependent `libstdc++.so.6`, and `libb`, with the dependent `libstdc++.so.5`.


# Troubleshooting

## How to compile 32-bit program on 64-bit gcc in C and C++

Now in order to compile with 32-bit gcc, just add a flag `-m32` in the command line of compling the ‘C’ language program. For instance, to compile a file of `geek.c` through Linux terminal, you must write the following commnad with `-m32` flag.

```
gcc -m32 geek.c -o geek
```

How to check whether a program is compiled with 32-bit after adding a “-m32” flag?

``` c
// C program to demonstrate difference
// in output in 32-bit and 64-bit gcc
// File name: geek.c
#include<stdio.h>
int main()
{
    printf("Size = %lu", sizeof(size_t));
}
```

Input: gcc -m64 geek.c -o out
Output: ./out
Size = 8

Input: gcc -m32 geek.c -o out
Output: ./out
Size = 4

refer: https://www.geeksforgeeks.org/compile-32-bit-program-64-bit-gcc-c-c/

## How can I hide "defined but not used" warnings in GCC?

The GCC compiler flags that control [unused warnings](http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) include:

```
-Wunused-function
-Wunused-label
-Wunused-parameter
-Wunused-value
-Wunused-variable
-Wunused (=all of the above)
```

Each of these has a corresponding negative form with "no-" inserted after the W which turns off the warning (in case it was turned on by -Wall, for example). Thus, in your case you should use

```
-Wno-unused-function
```

refer: http://stackoverflow.com/questions/386220/how-can-i-hide-defined-but-not-used-warnings-in-gcc

## GCC和Clang混合编译兼容性问题

* The compilers are compatible, but their default standard libraries are not. (clang和gcc编译器是兼容的，另外还要看它们使用的c++库是否兼容，其中，`libstdc++`和`libc++`是不兼容的)
* On GNU/Linux you are probably using `clang + libstdc++`, so it is compatible with `GCC + libstdc++`, because it uses the same definition of `std::string` from `libstdc++`.
* On Mac OS X you are using `clang + libc++`, which is not compatible with `GCC + libstdc++`, they define `std::string` differently so you get different mangled names.
* One option is to use `-stdlib=libstdc++` when compiling with Clang on Mac OS X, to tell it to use `libstdc++`, however the version of `libstdc++` included with Mac OS X is ancient and doesn't support any of C++11. Doing that might also mean you can't link to other native Mac OS X libraries that use the C++ standard library, because they would probably not have been built with `-stdlib=libstdc++`

```
# MacOS
$otool -L demo
demo:
	/usr/lib/libc++.1.dylib (compatibility version 1.0.0, current version 902.1.0)
	/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1281.100.1)

# Linux
$ ldd demo
        linux-vdso.so.1 (0x00007ffc68be3000)
        libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f37a14ad000)
        libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f37a1295000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f37a0ea4000)
        libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f37a0b06000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f37a1a39000)
```

* [We can’t mix the objects (or libraries) compiled by g++ and clang++ ? at least on Mac?](https://stackoverflow.com/questions/25266493/we-can-t-mix-the-objects-or-libraries-compiled-by-g-and-clang-at-least-o)
* [Can Clang compile code with GCC compiled .a libs?](https://stackoverflow.com/questions/20875924/can-clang-compile-code-with-gcc-compiled-a-libs)
* [Use libc++ standard library implementation on GCC instead of libstdc++](https://stackoverflow.com/questions/71675796/use-libc-standard-library-implementation-on-gcc-instead-of-libstdc/71681158)

## std::unordered_map 在不同gcc版本的ABI兼容性问题 (_GLIBCXX_USE_CXX11_ABI)

In the GCC 5.1 release libstdc++ introduced a new library ABI that includes new implementations of `std::string` and `std::list`. These changes were necessary to conform to the 2011 C++ standard which forbids Copy-On-Write strings and requires lists to keep track of their size.

see: https://gcc.gnu.org/gcc-5/changes.html#libstdcxx

> _GLIBCXX_USE_CXX11_ABI

Defined to the value 1 by default. Configurable via --disable-libstdcxx-dual-abi and/or --with-default-libstdcxx-abi. ABI-changing. When defined to a non-zero value the library headers will use the new C++11-conforming ABI introduced in GCC 5, rather than the older ABI introduced in GCC 3.4. This changes the definition of several class templates, including std:string, std::list and some locale facets. For more details see [Dual ABI](https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html).

https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_macros.html

``` cpp
#include <iostream>
#include <list>
#include <string>
#include <map>
#include <unordered_map>

int main()
{
    printf("__GNUC__(%d)\n", __GNUC__);
    printf("sizeof(std::unordered_map<uint64_t, uint64_t>) = %lu\n", sizeof(std::unordered_map<uint64_t, uint64_t>) );
    printf("sizeof(std::map<uint64_t, uint64_t>) = %lu\n", sizeof(std::map<uint64_t, uint64_t>) );
    printf("sizeof(std::string) = %lu\n", sizeof(std::string) );
    printf("sizeof(std::list<uint64_t>) = %lu\n", sizeof(std::list<uint64_t>) );
}
```

gcc 8.3.0编译：

```
sizeof(std::unordered_map<uint64_t, uint64_t>) = 56
sizeof(std::map<uint64_t, uint64_t>) = 48
sizeof(std::string) = 32
sizeof(std::list<uint64_t>) = 24
```

gcc 8.3.0编译：带 `-D_GLIBCXX_USE_CXX11_ABI=0` 选项，可以保证`std::string`和`std::list` 与 gcc 4.8.5 ABI兼容。

```
sizeof(std::unordered_map<uint64_t, uint64_t>) = 56
sizeof(std::map<uint64_t, uint64_t>) = 48
sizeof(std::string) = 8
sizeof(std::list<uint64_t>) = 16
```

gcc 4.8.5编译：

```
sizeof(std::unordered_map<uint64_t, uint64_t>) = 48
sizeof(std::map<uint64_t, uint64_t>) = 48
sizeof(std::string) = 8
sizeof(std::list<uint64_t>) = 16
```

在CMake中检查是否存在_GLIBCXX_USE_CXX11_ABI宏定义的方法：

```
# @refer https://gitcode.net/mirrors/intel-isl/Open3D/-/blob/wei/rename-voxelhashing/CMakeLists.txt
# Check if the compiler defines the _GLIBCXX_USE_CXX11_ABI macro
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("#include <cxxabi.h>
int main() { return _GLIBCXX_USE_CXX11_ABI; }" HAS_GLIBCXX_USE_CXX11_ABI)
```

测试代码：

``` cpp
#include <iostream>
#include <cxxabi.h>
#include <string>

int main()
{
    std::cout << _GLIBCXX_USE_CXX11_ABI;
    std::__cxx11::basic_string<char,std::char_traits<char>,std::allocator<char>> s;
}
```


* https://developers.redhat.com/blog/2015/02/05/gcc5-and-the-c11-abi
* https://stackoverflow.com/questions/45417707/glibcxx-use-cxx11-abi-gcc-4-8-and-abi-compatibility

## [Is it safe to link C++17, C++14, and C++11 objects](https://stackoverflow.com/questions/46746878/is-it-safe-to-link-c17-c14-and-c11-objects)

For `GCC` it is safe to link together any combination of objects A, B, and C. If they are all built with the same version then they are ABI compatible, the standard version (i.e. the -std option) doesn't make any difference.

Any combination of the following objects will work (although see note below about `libstdc++.so` version):

For exmaple,

```
object D compiled with GCC 4.9 and -std=c++03
object E compiled with GCC 5 and -std=c++11
object F compiled with GCC 7 and -std=c++17
```

* because `C++03` support is stable in all three compiler versions used, and so the `C++03` components are compatible between all the objects.
* C++11 support is stable since GCC 5, but object D doesn't use any C++11 features, and objects E and F both use versions where C++11 support is stable.
* C++17 support is not stable in any of the used compiler versions, but only object F uses C++17 features and so there is no compatibility issue with the other two objects (the only features they share come from C++03 or C++11, and the versions used make those parts OK).
* If you later wanted to compile a fourth object, G, using GCC 8 and -std=c++17 then you would need to recompile F with the same version (or not link to F) because the C++17 symbols in F and G are incompatible.

**The only caveat for the compatibility described above between D, E and F is that your program must use the libstdc++.so shared library from GCC 7 (or later).** Because object F was compiled with GCC 7, you need to use the shared library from that release, because compiling any part of the program with GCC 7 might introduce dependencies on symbols that are not present in the libstdc++.so from GCC 4.9 or GCC 5. Similarly, if you linked to object G, built with GCC 8, you would need to use the libstdc++.so from GCC 8 to ensure all symbols needed by G are found. The simple rule is to ensure the shared library the program uses at run-time is at least as new as the version used to compile any of the objects.

Another caveat when using GCC, already mentioned in the comments on your question, is that **since GCC 5 there are two implementations of std::string available in libstdc++. The two implementations are not link-compatible (they have different mangled names, so can't be linked together) but can co-exist in the same binary (they have different mangled names, so don't conflict if one object uses std::string and the other uses std::__cxx11::string)**. If your objects use `std::string` then usually they should all be compiled with the same string implementation. Compile with `-D_GLIBCXX_USE_CXX11_ABI=0` to select the original gcc4-compatible implementation, or `-D_GLIBCXX_USE_CXX11_ABI=1` to select the new cxx11 implementation (don't be fooled by the name, it can be used in C++03 too, it's called cxx11 because it conforms to the C++11 requirements). Which implementation is the default depends on how GCC was configured, but the default can always be overridden at compile-time with the macro.


## [Glibc vs GCC vs binutils compatibility](https://stackoverflow.com/questions/35873558/glibc-vs-gcc-vs-binutils-compatibility)

Is there a sort of official documentation about version compatibility between `binutils`, `glibc` and `GCC`?

glibc documents the min required version of binutils & gcc in its [INSTALL file](https://sourceware.org/git/?p=glibc.git;a=blob;f=INSTALL;hb=glibc-2.23).

glibc-2.23 states:

```
Recommended Tools for Compilation
GCC 4.7 or newer
GNU 'binutils' 2.22 or later
```

typically if you want to go newer than those, glibc will generally work with the version of gcc that was in development at the time of the release. e.g. glibc-2.23 was released 18 Feb 2016 and gcc-6 was under development at that time, so glibc-2.23 will work with gcc-4.7 through gcc-6.

so find the [version of gcc](https://ftp.gnu.org/pub/gnu/gcc/) you want, then find its release date, then look at the [glibc releases](https://ftp.gnu.org/pub/gnu/glibc/) from around the same time.

all that said, using an old version of glibc is a terrible idea. it will be full of known security vulnerabilities (include remotely exploitable ones). the latest glibc-2.23 release for example fixed [CVE-2015-7547](https://sourceware.org/bugzilla/show_bug.cgi?id=18665) which affects any application doing DNS network resolution and affects versions starting with glibc-2.9. remember: this is not the only bug lurking.


## 升级 GLIBC

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






# GCC 不同版本变更说明

* https://gcc.gnu.org/gcc-5/changes.html#libstdcxx




