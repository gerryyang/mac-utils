---
layout: post
title:  "Make in Action"
date:   2025-06-02 20:00:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}



**make** 工具解决的问题：它会在必要时重新编译所有受改动影响的源文件。

> 注意：make 命令不仅仅用于编译程序，无论何时，当需要通过多个输入文件来生成输出文件时，你都可以利用它来完成任务。它的其他用法还包括文档处理。

虽然 make 命令内置了很多智能机制，但光凭其自身是无法了解应该如何建立应用程序的。你必须为其提供一个文件，告诉它应用程序应该如何构造，这个文件称为 **makefile**。makefile 文件一般都会和项目的其他源文件放在同一个目录下。你的机器上可以同时存在许多不同的 makefile 文件。事实上，如果管理的是一个大项目，你可以用多个不同的 makefile 文件来分别管理项目的不同部分。

**make** 命令和 **makefile** 文件的结合提供了一个在项目管理领域十分强大的工具。它不仅常被用于控制源代码的编译，而且还用于手册页的编写以及将应用程序安装到目标目录。


# makefile 的语法

makefile 文件由**一组依赖关系和规则**构成。每个**依赖关系**由一个目标（即将要创建的文件）和一组该目标所依赖的源文件组成。make 命令会去读取 makefile 文件的内容，它先确定目标文件或要创建的文件，然后比较该目标所依赖的源文件的日期和时间以决定该采用哪条规则来构造目标。通常在创建最终的目标文件之前，它需要先创建一些中间目标。make 命令会根据 makefile 文件来确定目标文件的创建顺序以及正确的规则调用顺序。


# make 命令的选项和参数

make 程序本身有许多选项，其中最常用的3个选项为：

* `-k`

作用是让 make 命令在发现错误时仍然继续执行，而不是在检测到第一个错误时就停下来。可以利用这个选项在一次操作中发现所有未编译成功的源文件。

* `-n`

作用是让 make 命令输出将要执行的操作步骤，而不真正执行这些操作。

* `-f <filename>`

作用是告诉 make 命令将哪个文件作为 `makefile` 文件。如果未使用这个选项，标准版本的 make 命令将首先在当前目录下查找名为 `makefile` 的文件，如果该文件不存在，它就会查找名为 `Makefile` 的文件。但如果你是在 Linux 系统中，你使用的可能是 GNU Make，这个版本的 make 命令将在搜索 `makefile` 文件和 `Makefile` 文件之前，首先查找名为 `GNUmakefile` 的文件。

> 建议：使用文件名 `Makefile`，因为如果一个目录下都是以小写字母为名称的文件，则 `Makefile` 文件将在目录的文件列表中第一个出现。建议不要使用文件名 `GNUmakefile`，因为它是特定于 make 命令的 GNU 实现的。

为了指示 make 命令创建一个特定的目标（通常是一个可执行文件），可以把该目标的名字作为 make 命令的一个参数。如果不这么做，make 命令将试图创建列在 makefile 文件中的第一个目标。**许多程序员都会在自己的 makefile 文件中将第一个目标定义为 `all`，然后再列出其他从属目标。这个约定可以明确地告诉 make 命令，在未指定特定目标时，默认情况下应该创建哪个目标**。


# 依赖关系

在 makefile 文件中，规则的写法是：先写目标的名称，然后紧跟着一个冒号，接着是空格或制表符 tab，最后是用空格或制表符 tab 隔开的文件列表（这些文件用于创建目标文件）。

例子：

``` makefile
myapp: main.o 2.o 3.o
main.o: main.c a.h
2.o: 2.c a.h b.h
3.o: 3.c b.h c.h
```

它表示目标 `myapp` 依赖于 `main.o`、`2.o` 和 `3.o`，而 `main.o` 依赖于 `main.c` 和 `a.h`，等等。这组依赖关系形成一个层次结构，它显示了源文件之间的关系。可以很容易看出，如果文件 `b.h` 发生改变，你就需重新编译 `2.o` 和 `3.o`，而由于 `2.o` 和 `3.o` 发生了改变，你还需要重新创建目标 `myapp`。

**如果想一次创建多个文件，可以利用伪目标 `all`**。假设应用程序由二进制文件 `myapp` 和使用手册 `myapp.1` 组成，可以用下面这行语句进行定义：

``` makefile
all: myapp myapp.1
```

> 注意：如果未指定一个 `all` 目标，则 `make` 命令将只创建它在文件 `makefile` 中找到的第一个目标。


# 规则

makefile 文件的第二部分内容是规则，它们定义了目标的创建方式。

> 注意：**makefile 文件中有一个非常奇怪而又令人遗憾的语法现象 —— 空格和制表符 tab 是有区别的**。规则所在的行必须以制表符 tab 开头，用空格是不行的。此外，如果 makefile 文件中的某行以空格结尾，它也可能会导致 make 命令执行失败。但这些都是历史遗留问题，而且因为已有太多的 makefile 文件存在，企图将其全部改正是不现实的，所以请小心编写 makefile 文件。

![make](/assets/images/202506/make.png)


一个简单的 makefile 文件：

``` makefile
myapp: main.o 2.o 3.o
    gcc -o myapp main.o 2.o 3.o            # 注意：开头的为 tab，不能是空格
main.o: main.c a.h
    gcc -c main.c
2.o: 2.c a.h b.h
    gcc -c 2.c
3.o: 3.c b.h c.h
    gcc -c 3.c
```


make 命令处理 makefile 文件中定义的依赖关系，确定需要创建的文件以及创建顺序。虽然把如何创建目标 `myapp` 列在最前面，但 make 命令能够自行判断出创建文件的正确顺序。它调用你在规则部分给出的命令来创建相应的文件，同时会在执行时在屏幕上将命令显示出来。

> 注意：改变需要编译的某一文件，重新 make 编译，make 命令读取 makefile 文件，确定重建 `myapp` 所需的最少命令，并以正确的顺序执行它们。（因为可以根据文件的修改时间来决定是否是最新文件，并决定是否要重新编译）


# makefile 文件中的注释

makefile 文件中的注释以 `#` 号开头，一直延续到这一行的结束。

![make2](/assets/images/202506/make2.png)


# makefile 文件中的宏

makefile 文件允许你使用宏，以一种更通用的格式来书写它们。

在 makefile 文件中定义宏：

``` makefile
MACRONAME=value
```

引用宏的方法：

``` makefile
$(MACRONAME)
# or
${MACRONAME}    # make 的有些版本还允许 $MACRONAME
```

把一个宏的值设置为空的方法：

``` makefile
MACRONAME=                  # 将等号后面留空
```

> 注意：

1. makefile 文件中的**宏常被用于设置编译器的选项**。
2. makefile 文件的另一个问题：它假设编译器的名字是 `gcc`，而在其他 UNIX 系统中，编译器的名字可能是 `cc` 或 `c89`。如果想将 makefile 文件移植到另一版本的 UNIX 系统中，或在现有系统中使用另一个编译器，为了使其工作，你将不得不修改 makefile 文件中许多行的内容。—— **宏是用来收集所有这些与系统相关内容的好方法，通过使用宏定义，你可以方便地修改这些内容**。
3. 宏通常都是在 makefile 文件中定义的，但你也可以在调用 make 命令时在命令行上给出宏定义。例如，命令 `make CC=c89`。
4. 命令行上的宏将覆盖在 makefile 文件中的宏定义。
5. **当在 makefile 文件之外使用宏定义时，要注意宏定义必须以单个参数的形式传递**，所以应避免在宏定义中使用空格或应给宏定义加上引号，例如，`make "CC = c89"`。
6. make 命令将`$(CC)`、`$(CFLAGS)` 和 `$(INCLUDE)` 替换为相应的宏定义，这与 C 语言编译器对 `#define` 语句的处理方式很相似。现在，如果想改变编译器命令，只需要修改 makefile 文件中的一行即可。


例子：带宏定义的 makefile 文件

``` c
/* main.c */
#include <stdlib.h>
#include "a.h"

extern void function_two();
extern void function_three();

int main()
{
	function_two();
	function_three();
	exit(EXIT_SUCCESS);
}
```

``` c
/* 2.c */
#include "a.h"
#include "b.h"
#include <stdio.h>

void function_two()
{
	printf("call function_two/n");
}
```

``` c
/* 3.c */
#include "b.h"
#include "c.h"
#include <stdio.h>

void function_three()
{
	printf("call function_three/n");
}
```

3个头文件 a.h、b.h、c.h 都为空

makefile 文件：

``` makefile
all: myapp

# Which compiler
CC = gcc

# Where are include files kept
INCLUDE = .

# Options for development
CFLAGS = -O -Wall -ansi

# Options for release
# CFLAGS = -O -Wall -ansi

myapp: main.o 2.o 3.o
	$(CC) -o myapp main.o 2.o 3.o

main.o: main.c a.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c main.c

2.o: 2.c a.h b.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c 2.c

3.o: 3.c b.h c.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c 3.c
```

事实上，make 命令内置了一些特殊的宏定义，通过使用它们，可以让 makefile 文件变得更加简洁。这些宏在使用前才展开，所以它们的含义会随着 makefile 文件的处理进展而发生变化 —— 这才是特殊宏用法的关键。

**几个较常用的宏：**

* `$?`            当前目标所依赖的文件列表中比当前目标文件还要新的文件
* `$@`            当前目标的文字
* `$<`            当前依赖文件的名字
* `$*`            不包括后缀名的当前依赖文件的名字

在 makefile 文件中，可能还会看到下面两个有用的特殊字符，**它们出现在命令之前**：

* `-`     告诉 make 命令忽略所有错误。
* `@`     告诉 make 命令在执行某条命令前不要将该命令显示在标准输出上。如果想用 echo 命令给出一些说明信息，这个字符将非常有用。


# 多个目标

通常制作不止一个目标文件或将多组命令集中到一个位置来执行是很有用的。

例子：在 makefile 文件中增加一个 `clean` 选项来删除不需要的目标文件，增加一个 `install` 选项来将编译成功的应用程序安装到另一个目录下。

main.c、2.c、3.c、a.h、b.h、c.h 文件内容与上例相同。

makefile 文件：

``` makefile
all: myapp

# Which compiler
CC = gcc

# Where to install
#INSTDIR = /usr/local/bin
INSTDIR = /home/colinux/bin

# Where are include files kept
INCLUDE = .

# Options for development
CFLAGS = -O -Wall -ansi

# Options for release
# CFLAGS = -O -Wall -ansi

myapp: main.o 2.o 3.o
	$(CC) -o myapp main.o 2.o 3.o

main.o: main.c a.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c main.c

2.o: 2.c a.h b.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c 2.c

3.o: 3.c b.h c.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c 3.c

clean:
	-rm main.o 2.o 3.o

install: myapp
	@if [ -d $(INSTDIR) ];/
		then/
		cp myapp $(INSTDIR);/
		chmod a+x $(INSTDIR)/myapp;/
		chmod og-w $(INSTDIR)/myapp;/
		echo "Installed in $(INSTDIR)";/
	else/
		echo "Sorry, $(INSTDIR) does not exist";/
	fi
```

> 注意：

1. 特殊目标 `all` 仍然只指定了 `myapp` 这一个目标。因此，如果在执行 make 命令时未指定目标，它的默认行为就是创建目标 myapp。
2. 两个新增的目标：`clean` 和 `install`。
3. 目标 `clean` 用 `rm` `命令来删除目标文件，rm` 以减号（`-`）开头，含义是让 make 命令忽略 `rm` 命令的执行结果，即，即使由于目标文件不存在而导致 `rm` 命令返回错误，命令 `make clean` 也会成功。
4. 用于制作目标 `clean` 的规则并未给目标 `clean` 定义任何依赖关系，**行 `clean:` 的后面是空的，因此该目标总被认为是过时的**，所以在执行 make 命令时，如果指定目标 `clean`，则该目标所对应的规则将总被执行。
5. 目标 `install` 依赖于 `myapp`，所以 make 命令知道它必须首先创建 `myapp`，然后才能执行制作该目标所需的其他命令。
6. 用于制作 `install` 目标的规则由几个 shell 脚本命令组成。
7. 由于 make 命令在执行规则时会调用一个 shell，并且会针对每个规则使用一个新 shell，所以必须在上面每行代码的结尾加上一个反斜杠 `/`，让所有 shell 脚本命令在逻辑上处于同一行，并作为一个整体传递给一个 shell 执行。
8. 这个命令以符号 `@` 开头，表示 make 在执行这些规则之前不会在标准输出上显示命令本身。
9. 目标 `install` 按顺序执行多个命令，将应用程序安装到其最终位置。它并没有在执行下一个命令前检查前一个命令的执行是否成功。可以改为，将这些命令用符号 `&&` 连接起来。对 shell 来说，它是“与”的意思，即每个后续命令只在前面的命令都执行成功的前提下才会被执行。
10. 可能不能以普通用户的身份将命令安装到目录 `/usr/local/bin` 下。


将这些命令用符号 `&&` 连接起来，在执行下一个命令前检查前一个命令的执行是否成功：

``` makefile
install: myapp
	@if [ -d $(INSTDIR) ];/
		then/
		cp myapp $(INSTDIR) &&/
		chmod a+x $(INSTDIR)/myapp &&/
		chmod og-w $(INSTDIR)/myapp &&/
		echo "Installed in $(INSTDIR)";/
	else/
		echo "Sorry, $(INSTDIR) does not exist";/
	fi
```

测试输入：

![make3](/assets/images/202506/make3.png)


# 内置规则

目前为止，你在 makefile 文件中对每个操作步骤的执行都做了精确的说明。事实上，make 命令本身带有大量的内置规则，它们可以极大地简化 makefile 文件的内容，尤其在拥有许多源文件时更是如此。

``` c
/* foo.c */
#include <stdlib.h>
#include <stdio.h>

int main()
{
	printf("Hello World/n");
	exit(EXIT_SUCCESS);
}
```

在不指定 makefile 文件时，尝试用 make 命令来编译它：

``` bash
$ make foo
cc          foo.c          -o   foo
```

可以看到，make 命令知道如何调用编译器，虽然此例中，它选择的是 `cc` 而不是 `gcc`（在 Linux 系统中，这没有问题，因为 `cc` 通常是 `gcc` 的一个连接文件）。

![make4](/assets/images/202506/make4.png)


这些**内置规则**又被称为 —— **推导规则**，由于它们都会使用宏定义，因此可以通过给宏赋予新值来改变其默认行为。

![make5](/assets/images/202506/make5.png)

可以通过下面命令打印出make命令所有内置规则：

``` bash
$ make -p | more
```

内置规则较多。

![make6](/assets/images/202506/make6.png)

**考虑到存在这些内置规则，可以将文件 makefile 中用于制作目标的规则去掉，而只需要指定依赖关系，从而达到简化 makefile 文件的目的**。

``` makefile
main.o: main.c a.h
2.o: 2.c a.h b.h
3.o: 3.c b.h c.h
```


# Tips

## [What is the purpose of `.PHONY` in a Makefile?](https://stackoverflow.com/questions/2145590/what-is-the-purpose-of-phony-in-a-makefile)

What does `.PHONY` mean in a Makefile? I have gone through [this](https://www.gnu.org/software/make/manual/make.html#Phony-Targets), but it is too complicated.

Can somebody explain it to me in simple terms?

------

![make7](/assets/images/202506/make7.png)

![make8](/assets/images/202506/make8.png)

See also: [GNU make manual: Phony Targets](https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html)




# Refer

* [GNU make](https://www.gnu.org/software/make/manual/make.html#Introduction)



