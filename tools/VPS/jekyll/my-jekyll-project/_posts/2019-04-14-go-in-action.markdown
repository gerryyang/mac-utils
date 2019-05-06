---
layout: post
title:  "Go in Action"
date:   2019-04-14 10:00:00 +0800
categories: golang
---

* Do not remove this line (it will not be displayed)
{:toc}

本文主要是基于[The Go Programming Language]的读书摘要。之前写过几篇关于[Go的文章](https://blog.csdn.net/delphiwcdj/column/info/b-1-goinaction)和
一些实践[goinaction](https://github.com/gerryyang/goinaction)。

# 起源

![go_stem](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201904/go_stem.jpg)

* Go形成构想是在2007年9月，并于`2009年11月`发布。其发明人是Robert Griesemer, Rob Pike和Ken Thompson，这几位都任职于Google。
* Go是个开源项目，所以其编译器，库和工具的源代码都可免费获取。
* Go的运行环境包括，`类UNIX系统` (Linux, FreeBSD, OpenBSD和Mac OS X)，还有`Plan 9`和`Microsoft Windows`。即，**一份程序可以跨平台运行**。
* Go是`编译型`的语言，其`工具链`将程序的`源文件`转变成机器相关的原生`二进制指令`，这些工具可以通过单一的`go命令`配合其`子命令`进行使用。
	- `go run helloworld.go` 将一个或多个`.go`为后缀的源文件进行编译，链接，并运行生成的可执行文件。
	- `go build helloworld.go` 编译，链接，生成二进制程序。
* Go原生支持`Unicode`，所以它可以处理所有国家的语言。
* Go平衡了`表达力`和`安全性`。Go程序通常比动态语言程序运行更快，同时遭遇意料之外的类型错误而导致的崩溃更少。
* Go语言从`祖先语言`中汲取的优点。(*混血儿*)
	- Go从**C语言**中继承了，`表达式语法`，`控制流语句`，`基本数据类型`，`按值调用的形参传递`，`指针`。但比这些更重要的是，继承了C所强调的`程序要编译成高效的机器码`，并自然地与所处的操作系统提供的抽象机制相配合。
	- Go从**Pascal**为发端的一个语言支流中
		+ Modula-2启发了`包`的概念。
		+ Oberon消除了模块接口文件和模块实现文件的区隔。
		+ Object Oberon提供了方法声明语法。
	- Go借鉴了一些基于**CSP (Communicating Sequential Process, 通信顺序进程)**实现的语言
	- Go还有一些特性并非来自祖先的基因。例如，`iota`多多少少有点APL的影子；嵌套函数的词法作用域则来自Scheme。
	- Go还有自己一些**全新的特性**。例如，创新性的`切片`为动态数组提供了高效的随机访问的同时，也允许旧式链表的复杂共享机制；`defer`语句也是新引入的。

* **简单性才是好软件的不二法门，Go保持极端简单性的行为文化**。
	- 在高级语言中，Go出现的较晚，因而有一定后发优势。它的基础部分实现不错。
		+ 垃圾收集
		+ 包系统
		+ 一等函数
		+ 词法作用域
		+ 系统调用接口
		+ 默认用UTF-8编码的字符串
	- 但相对来说，语言特性不多，而且不会增加新特性了。(**Go成熟稳定，并且保证兼容更早版本**)
		+ 没有`隐式数值类型强制转换`
		+ 没有`构造或析构函数`
		+ 没有`运算符重载`
		+ 没有`形参默认值`
		+ 没有`继承`
		+ 没有`泛型`
		+ 没有`异常`
		+ 没有`宏`
		+ 没有`函数注记`
		+ 没有`线程局部存储`
	- Go的`类型系统`，足可以使程序员避免在动态语言中会无意犯下的绝大多数错误，但相对而言，它在带类型的语言中，又算是类型系统比较简单的。Go能为程序员提供，具备相当强类型的系统才能实现的安全性和运行时性能，而不让程序员承担这种系统的复杂性。
	- Go提倡充分利用当代计算机系统设计，尤其强调`局部性的重要`。
		+ 由于现代计算机都是并行工作的，Go有着基于CSP的并行特性。并提供了`变长栈`来运行其轻量级线程(`goroutine`)，这个栈初始化时非常小，所以创建一个goroutine成本极低，创建`100万`个也完全可以接受。
	- Go`标准库`常常被称为`语言自带电池`，它提供了清晰的构件，以及用于`I/O`，`文本处理`，`图像`，`加解密`，`网络`，`分布式应用`的API，而且对许多`标准文件格式和协议`都提供了支持。

| 类型语言 | 复杂性 | 性能和安全性 | 
| -- | -- | --
| 强 | 高 | 高
| 弱 | 低 | 低        

# 测试环境

根据[The Go Programming Language Example Programs](https://github.com/adonovan/gopl.io/)，构建测试环境。

## 安装

三种安装方式：

* 源码安装。
* 标准包安装。
* 第三方工具安装。例如，Ubuntu的`apt-get`，Mac的`homebrew`。

```
root@gerryyang:~/workspace/go# go version
go version go1.9.2 linux/amd64
```

## 环境变量

``` bash
# go的安装路径
export GOROOT=/root/LAMP/golang/go_1_9_2
export PATH=$PATH:$GOROOT/bin
# go的工作目录
export GOPATH=/root/workspace/go
```
* `GOPATH`允许多个目录(Linux下用冒号分割)，当GOPATH指定了**多个目录时**，默认将`go get`的内容放在**第一个目录**。
* GOPATH目录约定有3个子目录：
	- `src` (源代码，例如，.go, .c, .h, .s等)
	- `pkg` (编译后生成的文件，例如，.a)
	- `bin` (编译后生成的可执行文件)

## 编译和执行

通过`go get`命令获取源码，构建和安装。

```
root@gerryyang:~/workspace/go#go get gopl.io/ch1/helloworld      
root@gerryyang:~/workspace/go# ls
bin  src
root@gerryyang:~/workspace/go# ls bin
helloworld
root@gerryyang:~/workspace/go# ./bin/helloworld 
Hello, 世界
root@gerryyang:~/workspace/go# ls src/gopl.io/
ch1  ch10  ch11  ch12  ch13  ch2  ch3  ch4  ch5  ch6  ch7  ch8  ch9  README.md
```

源码在`$(GOPATH)/src/gopl.io/ch1/helloworld/main.go`。


``` go
// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 1.

// Helloworld is our first Go program.
//!+
package main

import "fmt"

func main() {
        fmt.Println("Hello, 世界")
}

//!-
```


# Go基础知识

* Go代码是使用`包`来组织的，包类似其他语言中的库和模块。一个包由一个或多个`.go`源文件组成，放在`一个文件夹`中，该`文件夹的名字`描述了包的作用。
* 每一个源文件的开始都用`package`声明。例如，`package main`，指明了这个文件属于哪个包。后面跟着它导入的`其他包的列表`，然后是存储在文件中的`程序声明`。
* Go的`标准库`中有100多个包用来完成输入，输出，排序，文本处理等常见任务。
* 名为`main`的包用来定义一个独立的可执行程序，而不是库。
* 必须精确地导入需要的包，在缺失导入，或存在不需要的包的情况下，编译会失败。(这种严格的要求可以防止程序演化中引用不需要的包)
	- `goimports`工具可以按需管理导入声明的插入和移除。`go get golang.org/x/tools/cmd/goimports`
* `import声明`必须跟在`package声明`之后。import导入声明后面，是组成程序的函数，变量，常量，类型声明。
* Go不需要在语句或声明后使用`分号结尾`，除非有多个语句或声明出现在同一行。事实上，跟在特定符号后面的换行符被转换为分号。在什么地方进行换行会影响对Go代码的解析。例如，`{`符号必须和关键字`func`在同一行，不能独自成行。
* Go对代码的`格式化`要求非常严格。`gofmt`工具将代码以标准格式重写。go工具的fmt子命令会用gofmt工具来格式化指定包里的所有文件，或者当前文件夹中的文件(默认情况下)。许多文本编辑器可以配置为在每次保存文件时自动运行gofmt，因此源文件总可以保持正确的形式。
* 命令行参数。`os包`提供了一些函数和变量，以平台无关的方式和操作系统打交道。命令行参数以os包中的`Args`名字的变量供程序访问。变量`os.Args`是一个字符串`slice`。

``` go
// go get gopl.io/ch1/echo1
package main

import (
	"fmt"
	"os"
)

func main() {
	var s, sep string
	for i := 1; i < len(os.Args); i++ {
		s += sep + os.Args[i]
		sep = " "
	}
	fmt.Println(s)
}
```
* 注释以`\\`开头。习惯上，在一个包声明前，使用注释对其进行描述。
* `var`关键字声明了两个`string`类型的变量。变量可以在声明的时候初始化，如果变量没有明确地初始化，它将隐式地初始化为这个类型的空值。例如，对于数字初始化结果是0，对于字符串是空字符串。
* 对于数字，Go提供常规的算术和逻辑操作符；对于字符串，`+`操作符表示追加操作。
* `:=`符号用于`短变量声明`，这种语句声明一个或多个变量，并且根据初始化的值给予合适的类型。
* 递增语句`i++`对i进行加1，它等价于`i += 1`，又等价于`i = i + 1`。**注意，这些是语句，而不像其他C族语言一样是表达式，所以`j = i ++`是不合法的。并且，仅支持后缀，`++i`也不合法。**
* `for`是Go里面的`唯一循环语句`，它有几种形式。
	- for循环的三个组成部分两边不用`小括号`，但`大括号`是必需的，左大括号必须和`post`语句在同一行。
	- 可选的`initialization`语句在循环开始之前执行。如果存在，它必须是一个简单的语句。
	- `condition`是一个布尔表达式，在循环的每一次迭代开始前推演。
	- 三部分都可以省略。

``` go
for initialization; condition; post {
	// ...
}

// while循环
for condition {
	// ...
}

// 无限循环
for {
	// ...
	// break;
	// return;
}
```

* 另一种形式的for循环在字符串或slice数据上迭代。

``` go
// Echo2 prints its command-line arguments.
package main

import (
	"fmt"
	"os"
)

func main() {
	s, sep := "", ""
	for _, arg := range os.Args[1:] {
		s += sep + arg
		sep = " "
	}
	fmt.Println(s)
}
```

* 每一次迭代，`range`产生一对值：`索引`和`这个索引处元素的值`。
* 在这个例子中，不需要索引，但是语法上range循环需要处理，因此也必须处理索引。一个主意是将索引赋予一个`临时变量`然后忽略它。但是，Go不允许存在无用的临时变量，不然会出现编译错误。解决方案是使用空标识符`_`，空标识符可以用在任何语法需要变量名但是程序逻辑不需要的地方。例如，丢弃每次迭代产生的无用的索引。
* 这个版本使用`短的变量声明`来声明和初始化。原则：使用`显式的初始化`来说明初始化变量的重要性，使用`隐式的初始化`来表明初始化变量不重要。

``` go
// 以下几种声明字符串变量的方式是等价的
s := ""              // 此方式，更加简洁，通常在一个函数内部使用，不适合包级别的变量 (推荐)
var s string         // 默认初始化为空字符串 (推荐)
var s = ""           // 很少用
var s string = ""    // 显式的变量类型，在类型一致的情况下是冗余的信息，在类型不一致时是必需的
```

* 上面程序的问题：每次循环，字符串`s`有了新的内容，`+=`语句通过追加旧的字符串，空格字符，和下一个参数，生成一个新的字符串，然后把新字符串赋给`s`。旧的内容不再需要使用，会被`例行垃圾回收`。如果有大量的数据需要处理，这样的代价会比较大。

* 一个高效的方式是使用strings包中的`Join`函数。

``` go
// Echo3 prints its command-line arguments.
package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
	fmt.Println(strings.Join(os.Args[1:], " "))
}
```
* 如果关心格式只是想看下值或调试，那么使用`Println`格式化结果就可以了。

``` go
// 任何slice都能以这样的方式输出
fmt.Println(os.Args[1:])
```

# Go程序组成

声明，变量，新类型，包和文件，以及作用域。

数值，布尔量，字符串，常量，Unicode

组合类型 (数组，键值对，结构体，切片)

函数，错误处理，崩溃和恢复，以及defer语句

方法，接口，并发，包，测试，反射

面向对象设计 (Go没有类继承，没有类，较复杂的对象行为是通过较简单的对象组合，而非继承完成的)

并发处理，基于CSP思想，采用goroutine和信道实现，共享变量

# Refer

1. [Go官网]
2. [Go博客]
3. [Go在线测试]
4. [Go在线交互式课程]
5. [The Go Programming Language]
6. [Dave Cheney的golang博客]
7. [High Performance Go Workshop - Dave Cheney]

[Go官网]: https://golang.org
[Go博客]: https://blog.golang.org
[Go在线测试]: https://play.golang.org
[Go在线交互式课程]: https://tour.golang.org
[The Go Programming Language]: http://www.gopl.io/
[The Go Programming Language Example Programs]: https://github.com/adonovan/gopl.io/
[Dave Cheney的golang博客]: dave.cheney.net
[High Performance Go Workshop - Dave Cheney]: https://dave.cheney.net/high-performance-go-workshop/gopherchina-2019.html?from=singlemessage&isappinstalled=0

