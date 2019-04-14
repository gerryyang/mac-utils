---
layout: post
title:  "Go in Action"
date:   2019-04-14 10:00:00 +0800
categories: golang
---

* Do not remove this line (it will not be displayed)
{:toc}

本文主要是`The Go Programming Language`的读书摘要，并结合[goinaction](https://github.com/gerryyang/goinaction)的一些实践。

# 起源

![go_stem](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201904/go_stem.jpg)

* Go形成构想是在2007年9月，并于2009年11月发布。其发明人是Robert Griesemer, Rob Pike和Ken Thompson，这几位都任职于Google。
* Go是个开源项目，所以其编译器，库和工具的源代码都可免费获取。
* Go的运行环境包括，类UNIX系统 (Linux, FreeBSD, OpenBSD和Mac OS X)，还有Plan 9和Microsoft Windows。即，一份程序可以跨平台运行。
* Go平衡了表达力和安全性。Go程序通常比动态语言程序运行更快，同时遭遇意料之外的类型错误而导致的崩溃更少。
* Go语言从祖先语言中汲取的优点。(*混血儿*)
	- Go从**C语言**中继承了，`表达式语法`，`控制流语句`，`基本数据类型`，`按值调用的形参传递`，`指针`。但比这些更重要的是，继承了C所强调的`程序要编译成高效的机器码`，并自然地与所处的操作系统提供的抽象机制相配合。
	- Go从**Pascal**为发端的一个语言支流中
		+ Modula-2启发了`包`的概念。
		+ Oberon消除了模块接口文件和模块实现文件的区隔。
		+ Object Oberon提供了方法声明语法。
	- Go借鉴了一些基于**CSP (Communicating Sequential Process, 通信顺序进程)**实现的语言
	- Go还有一些特性并非来自祖先的基因。例如，`iota`多多少少有点APL的影子；嵌套函数的词法作用域则来自Scheme。
	- Go还有自己一些全新的特性。例如，创新性的`切片`为动态数组提供了高效的随机访问的同时，也允许旧式链表的复杂共享机制；`defer`语句也是新引入的。

* **简单性才是好软件的不二法门，Go保持极端简单性的行为文化**。
	- 在高级语言中，Go出现的较晚，因而有一定后发优势。它的基础部分实现不错。
		+ 垃圾收集
		+ 包系统
		+ 一等函数
		+ 词法作用域
		+ 系统调用接口
		+ 默认用UTF-8编码的字符串
	- 但相对来说，语言特性不多，而且不会增加新特性了。(Go成熟稳定，并且保证兼容更早版本)
		+ 没有隐式数值类型强制转换
		+ 没有构造或析构函数
		+ 没有运算符重载
		+ 没有形参默认值
		+ 没有继承
		+ 没有泛型
		+ 没有异常
		+ 没有宏
		+ 没有函数注记
		+ 没有线程局部存储
	- Go的`类型系统`，足可以使程序员避免在动态语言中会无意犯下的绝大多数错误，但相对而言，它在带类型的语言中，又算是类型系统比较简单的。Go能为程序员提供，具备相当强类型的系统才能实现的安全性和运行时性能，而不让程序员承担这种系统的复杂性。
	- Go提倡充分利用当担计算机系统设计，尤其强调`局部性的重要`。
		+ 由于现代计算机都是并行工作的，Go有着基于CSP的并行特性。并提供了`变长栈`来运行其轻量级线程(`goroutine`)，这个栈初始化时非常小，所以创建一个goroutine成本极低，创建`100万`个也完全可以接受。
	- Go`标准库`常常被称为`语言自带电池`，它提供了清晰的构件，以及用于`I/O`，`文本处理`，`图像`，`加解密`，`网络`，`分布式应用`的API，而且对许多`标准文件格式和协议`都提供了支持。

| 类型语言 | 复杂性 | 性能和安全性 | 
| -- | -- | --
| 强 | 高 | 高
| 弱 | 低 | 低              

# Go基础结构

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
5. [The Go Programming Language Sample Codes]

[Go官网]: https://golang.org
[Go博客]: blog.golang.org
[Go在线测试]: play.golang.org
[Go在线交互式课程]: tour.golang.org
[The Go Programming Language Sample Codes]: gopl.io
