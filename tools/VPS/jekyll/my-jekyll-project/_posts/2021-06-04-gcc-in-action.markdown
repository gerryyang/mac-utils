---
layout: post
title:  "GCC in Action"
date:   2021-06-04 08:00:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}

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

## std::unordered_map在不同gcc版本的ABI兼容性问题

In the GCC 5.1 release libstdc++ introduced a new library ABI that includes new implementations of std::string and std::list. These changes were necessary to conform to the 2011 C++ standard which forbids Copy-On-Write strings and requires lists to keep track of their size.

_GLIBCXX_USE_CXX11_ABI

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

* https://developers.redhat.com/blog/2015/02/05/gcc5-and-the-c11-abi
* https://stackoverflow.com/questions/45417707/glibcxx-use-cxx11-abi-gcc-4-8-and-abi-compatibility