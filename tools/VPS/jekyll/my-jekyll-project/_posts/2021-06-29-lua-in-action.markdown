---
layout: post
title:  "Lua in Action"
date:   2021-06-29 20:00:00 +0800
categories: Lua
---

* Do not remove this line (it will not be displayed)
{:toc}


* Lua 语言从 1993 年诞生，当初，Lua 语言只是为了两个特定项目而开发的实验室项目。目前主要作为开源嵌入式脚本语言，在游戏领域广泛应用，用于实现热更新。
* 除了游戏领域外，包括 Redis, Nginx/OpenResty 等中也使用 Lua 作为其嵌入式脚本引擎，以供开发者进行功能扩展和二次开发。
* 第三方机构对 Lua 语言进行了多方面的改进和增强，例如 LuaJIT，其中，OpenResty LuaJIT 2.05 约等于 lua 5.1。
* 从语言的实现，原理，设计哲学等角度看，Lua 语言也是学习编译原理，虚拟机，脚本引擎等的重要参考和实现依据。
* Lua 语言从一开始就被设计为能与 C/C++ 及其常用语言开发的软件集成在一起使用的语言。这样的好处：
    + Lua 语言不需要在性能，与三方软件交互等 C 语言已经非常完善的方面重复“造轮子”，可以直接依赖 C 语言实现上述特性。
    + 通过引入`安全的运行时环境`，`自动内存管理`，`良好的字符串处理能力`和`可变长的多种数据类型`，Lua 语言弥补了 C 语言在非面向硬件的高级抽象能力，动态数据结构，鲁棒性，调试能力等方面的不足。

* Lua 的语言特性
    + **可扩展**。既支持使用 Lua 语言代码来扩展，也支持使用外部的 C 语言代码来扩展。例如：Lua 语言的大部分基础功能都是通过外部库实现的，可以很容易地将 Lua 与 C/C++, Java, C# 和 Python 等结合在一起使用。
    + **简明**。其包含所有标准库的 Linux 64 位版本仅有 220 KB。
    + **可移植**。Lua 语言可以运行在几乎所有平台上。Lua 遵循 ANSI (ISO) C 标准，并未使用条件编译来对不同平台进行代码的适配。因此，当需要适配新平台时，只要使用对应平台下的 ISO C 编译器重新编译 Lua 语言的源码就可以了。
* Lua 语言有三类典型用户
    + 在应用程序中嵌入式地使用 Lua 语言的用户
    + 单独使用 Lua 语言的用户
    + 和 C 语言一起使用 Lua 语言的用户

> 例如，Adobe Lightroom, 魔兽世界等在内的许多应用程序中嵌入式地使用了 Lua 语言。这些应用使用 Lua 语言的 C 语言 API 去注册新函数，创建新类型和改变部分运算符的行为，以最终达到将 Lua 语言用于特定领域的目的。

* Lua 语言的主要能力源于标准库。例如，标准库提供了模式匹配和其他字符串处理函数。
* Lua 中连续的连字符 `--` 表示单行注释。`--[[ ]]` 表示多行注释。
* 从 [Lua 官网](www.lua.org) 上下载解释器的源码，建议尝试从源码编译并安装。

``` bash
# 下载 lua-5.3.6.tar.gz
wget http://www.lua.org/ftp/lua-5.3.6.tar.gz  # released on 14 Sep 2020
tar zxvf lua-5.3.6.tar.gz
cd lua-5.3.6
make linux -j 16
make install  # 指定 INSTALL_TOP 变量设置安装目录
```

安装目录：

```
$tree
.
├── bin
│   ├── lua
│   └── luac
├── include
│   ├── lauxlib.h
│   ├── luaconf.h
│   ├── lua.h
│   ├── lua.hpp
│   └── lualib.h
├── lib
│   ├── liblua.a
│   └── lua
│       └── 5.3
├── man
│   └── man1
│       ├── lua.1
│       └── luac.1
└── share
    └── lua
        └── 5.3

10 directories, 10 files
```

# 基础

* Lua 语言是一种动态类型语言，在这种语言中没有类型定义，每个值都带有其自身的类型信息。
    + Lua 语言中有 8 种基本类型，使用函数 type 可获得一个值对应的类型名称
    + userdata 类型允许把任意的 C 语言数据保存在 Lua 语言变量中。在 Lua 语言中，用户数据类型除了赋值和相等性测试外，没有其他预定义的操作。用户数据被用来表示由应用或 C 语言编写的库所创建的新类型。
    + **变量没有预定义的类型，任何变量都可以包含任何类型的值**。一般情况下，将一个变量用作不同类型时，会导致代码的可读性不佳，但是，在某些情况下谨慎地使用这个特性，可能会带来一定程度的便利。例如，当代码发生异常时可以返回一个 nil 以区别于其他正常情况下的返回值

```
nil (空)
boolean (布尔)
number (数值)
string (字符串)
userdata (用户数据)
function (函数)
thread (线程)
table (表)
```

```
Lua 5.1.4  Copyright (C) 1994-2008 Lua.org, PUC-Rio
> type(nil)
> print(type(nil))
nil
> print(type(true))
boolean
> print(type(10.4 * 3))
number
> print(type("Hello"))
string
> print(type(io.stdin))
userdata
> print(type(print))
function
> print(type(type))
function
> print(type({}))
table
> print(type(type(X)))
string
```

* Lua 语言中的标识符是由任意字母，数字和下划线组成的字符串，注意不能以数字开头。其中，下划线 + 大写字母（例如：_VERSION）组成的标识符通常被用作特殊用途。

> Lua 语言的保留字：and break do else elseif end false goto for fucntiion if in local nil not or repeat return then true until while

* Lua 语言对大小写敏感。and 和 And 是两个不同的标识符。
* 在 Lua 语言中，全局变量无须声明即可使用，使用未经初始化的全局变量也不会导致错误，当使用未经初始化的全局变量时，得到的结果是 nil。
    + 当把 nil 赋值给全局变量时，Lua 会回收该全局变量
    + Lua 语言不区分**未初始化变量**和**被赋值为 nil 的变量**




# Q&A

## 大数计算

问题描述：[lua大整数运算的问题](https://groups.google.com/g/openresty/c/09eW8bVevHQ)

最近用lua处理某些大整数的时候发现不对劲

下面这条语句的结果是：10992432728506384
print(string.format("%d", 186312419127226*59 + 49))
但是这个算术得到的结果应该是：10992432728506383

luajit也有同样的问题

lua 版本：5.1.5
luajit版本：2.1.0-alpha

64位操作系统

问题解释：

在 LuaJIT 中，Lua number 的默认精度上限是双精度浮点数（double）。如果你想进行无精度损失的 64 位整数计算，须使用
FFI cdata 的包装整数类型。例如，对于你这里的例子：

local ffi = require "ffi"
local res = ffi.new("int64_t", 186312419127226)*59 + 49)
print(tostring(res)

使用 LuaJIT 运行这三行 Lua 代码得到的结果是

10992432728506383LL

正是你期望的值。

好吧，这可以进一步化简为

```
$ luajit -e 'print(186312419127226LL*59 + 49)'
10992432728506383LL
```

不用显式地使用 ffi 模块。貌似这已经足够简单了。

默认为 number 使用 64-bit 整型对性能损耗很大。比如在标准 Lua 5.3 解释器中，TValue 类型长达 128 位。而在 LuaJIT 2 中，TValue 只有 64 位。TValue 的长度对一般的 Lua 代码的执行性能影响很大。

相近的问题：[Does Lua make use of 64-bit integers?](https://stackoverflow.com/questions/3104722/does-lua-make-use-of-64-bit-integers)

Does Lua make use of 64-bit integers? How do I use it?

Compile it yourself. Lua uses double-precision floating point numbers by default. However, this can be changed in the source (`luaconf.h`, look for `LUA_NUMBER`).

Lua has a single numerical data type. By default this is a `double`, but can be changed in the header files to another type, such as `int64_t`.

`Lua 5.3` introduces the integer subtype, which uses `64-bit` integer by default.

From [Lua 5.3 reference manual](http://www.lua.org/manual/5.3/manual.html#2.1)

> The type number uses two internal representations, one called **integer** and the other called **float**. Lua has explicit rules about when each representation is used, but it also converts between them automatically as needed (see §3.4.3). Therefore, the programmer may choose to mostly ignore the difference between integers and floats or to assume complete control over the representation of each number. **Standard Lua uses 64-bit integers and double-precision (64-bit) floats, but you can also compile Lua so that it uses 32-bit integers and/or single-precision (32-bit) floats**. The option with 32 bits for both integers and floats is particularly attractive for small machines and embedded systems. (See macro `LUA_32BITS` in file `luaconf.h`.)

# 与 C++ 的交互

## LuaBridge

LuaBridge is a lightweight and dependency-free library for mapping data, functions, and classes back and forth between C++ and Lua (a powerful, fast, lightweight, embeddable scripting language). LuaBridge has been tested and works with Lua revisions starting from 5.1.5, although it should work in any version of Lua from 5.1.0 as well as LuaJit.

* https://github.com/vinniefalco/LuaBridge


## Luna

轻量化，易扩展。

* https://github.com/trumanzhao/luna


# 序列化

* https://github.com/starwing/lua-protobuf

# 代码静态检查

* https://github.com/mpeterv/luacheck

# Refer

* https://lua.org/manual/5.3/manual.html
* [Lua 5.3 参考手册](https://www.runoob.com/manual/lua53doc/contents.html)
