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
    + Lua 语言中有 **8 种基本类型**，使用函数 `type` 可获得一个值对应的类型名称
    + `userdata` 类型允许把任意的 C 语言数据保存在 Lua 语言变量中。在 Lua 语言中，用户数据类型除了赋值和相等性测试外，没有其他预定义的操作。用户数据被用来表示由应用或 C 语言编写的库所创建的新类型。
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
* 在 Lua 语言中，全局变量无须声明即可使用，使用未经初始化的全局变量也不会导致错误，当使用未经初始化的全局变量时，得到的结果是 nil。当把 nil 赋值给全局变量时，Lua 会回收该全局变量。
* nil 是一种只有一个 nil 值的类型，它的主要作用是与其他所有值进行区分
    + 使用 nil 来表示无效值
    + Lua 语言不区分**未初始化变量**和**被赋值为 nil 的变量**

* 将除 Boolean 值 `false` 和 `nil` **外的**所有其他值视为**真**。注意：**零**和**空字符串**都是为真。
* 逻辑运算符：and, or, not
    + 当 x 未被初始化时，将其默认值设为 v: if not x then x = v end
    + (x>y) and x or y 与 C 语言的三目运算符 a ? b : c 含义等价
    + not 运算符永远返回 Boolean 类型的值

```
> print(not nil)
true
> print(not false)
true
> print(not 0)
false
> print(not not 1)
true
> print(not not nil)
false
```





```
> print(4 and 5)
5
> print(nil and 13)
nil
> print(false and 13)
false
> print(0 or 5)
0
> print(false or "hi")
hi
> print(nil or false)
```


# 相关用法

## [lua_call](https://www.lua.org/manual/5.3/manual.html#lua_call)

``` cpp
void lua_call (lua_State *L, int nargs, int nresults);
```

Calls a function.


To call a function you must use the following protocol:

* first, the function to be called is pushed onto the stack;
* then, the arguments to the function are pushed in direct order; that is, the first argument is pushed first.
* Finally you call lua_call;
* nargs is the number of arguments that you pushed onto the stack.
* All arguments and the function value are popped from the stack when the function is called.
* The function results are pushed onto the stack when the function returns.
* The number of results is adjusted to nresults, unless nresults is LUA_MULTRET. In this case, all results from the function are pushed;
* Lua takes care that the returned values fit into the stack space, but it does not ensure any extra space in the stack. The function results are pushed onto the stack in direct order (the first result is pushed first), so that after the call the last result is on the top of the stack.

Any error inside the called function is propagated upwards (with a `longjmp`).

The following example shows how the host program can do the equivalent to this Lua code:

``` lua
a = f("how", t.x, 14)
```

Here it is in C:

``` c
lua_getglobal(L, "f");                  /* function to be called */
lua_pushliteral(L, "how");                       /* 1st argument */
lua_getglobal(L, "t");                    /* table to be indexed */
lua_getfield(L, -1, "x");        /* push result of t.x (2nd arg) */
lua_remove(L, -2);                  /* remove 't' from the stack */
lua_pushinteger(L, 14);                          /* 3rd argument */
lua_call(L, 3, 1);     /* call 'f' with 3 arguments and 1 result */
lua_setglobal(L, "a");                         /* set global 'a' */
```

Note that the code above is balanced: at its end, the stack is back to its original configuration. This is considered good programming practice.



## [lua_pcall](https://www.lua.org/manual/5.3/manual.html#lua_pcall)

``` cpp
int lua_pcall (lua_State *L, int nargs, int nresults, int msgh);
```

Calls a function in protected mode.

Both `nargs` and `nresults` have the same meaning as in `lua_call`. If there are no errors during the call, `lua_pcall` behaves exactly like `lua_call`. However, if there is any error, `lua_pcall` catches it, pushes a single value on the stack (the error object), and returns an error code. Like `lua_call`, `lua_pcall` always removes the function and its arguments from the stack.

If `msgh` is 0, then the error object returned on the stack is exactly the original error object. Otherwise, `msgh` is the stack index of a message handler. (This index cannot be a pseudo-index.) In case of runtime errors, this function will be called with the error object and its return value will be the object returned on the stack by `lua_pcall`.

Typically, the message handler is used to add more debug information to the error object, such as a stack traceback. Such information cannot be gathered after the return of `lua_pcall`, since by then the stack has unwound.

The `lua_pcall` function returns one of the following constants (defined in `lua.h`):

* LUA_OK (0): success.
* LUA_ERRRUN: a runtime error.
* LUA_ERRMEM: memory allocation error. For such errors, Lua does not call the message handler.
* LUA_ERRERR: error while running the message handler.
* LUA_ERRGCMM: error while running a __gc metamethod. For such errors, Lua does not call the message handler (as this kind of error typically has no relation with the function being called).


测试代码：

``` lua
function printmsg()
        -- ok
        --print("hello world")

        -- mock err
        print_not_exist("hello world")
end

function errorhandle(str)
        return string.upper(str)
end
```

``` cpp
#include<iostream>
#include<string>

extern "C" {
#include<lua.h>
#include<lualib.h>
#include<lauxlib.h>
}

int main()
{
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);

        if (luaL_loadfile(L, "test.lua"))
        {
                std::cout << "open file error" << std::endl;
                return 1;
        }

        // run test.lua
        int ret = lua_pcall(L, 0, 0, 0);
        if (ret)
        {
                std::cout << "1 function call error: " << ret << std::endl;
        }

        // function to be called
        lua_getglobal(L, "errorhandle");
        lua_getglobal(L, "printmsg");

        // run test.lua and call printmsg but no errfunc
        ret = lua_pcall(L, 0, 0, 0);
        if (ret)
        {
                std::cout << "2 function call error: " << ret << std::endl;
                std::cout << lua_tostring(L, -1) << std::endl;
        }

        // run test.lua and call printmsg and set errfunc
        ret = lua_pcall(L, 0, 0, -2);
        if (ret)
        {
                std::cout << "3 function call error: " << ret << std::endl;
                std::cout << lua_tostring(L, -1) << std::endl;
        }

        lua_close(L);

        return 0;
}

/*
$ ./a.out
2 function call error: 2
test.lua:7: attempt to call a nil value (global 'print_not_exist')
3 function call error: 2
ATTEMPT TO CALL A STRING VALUE
*/
```

``` bash
#!/bin/bash

# lua 5.3.5
g++ -g -I../lua_5.3.5/include -L../lua_5.3.5/lib lua_pcall.cc -llua  -ldl
echo "done"
```



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


## [Using lua_checkstack?](https://stackoverflow.com/questions/63272970/using-lua-checkstack)


``` c
void luaL_checkstack (lua_State *L, int sz, const char *msg);

// Grows the stack size to top + sz elements, raising an error if the stack cannot grow to that size. msg is an additional text to go into the error message (or NULL for no additional text).
```

* https://github.com/starwing/lua-protobuf/commit/d5f144b0f2a1930f700b5a7cce106374c0e64adf


# Lua 5.3 升级注意事项

``` bash
sudo yum install readline-devel
```

* [Lua 5.3 升级注意](https://blog.codingnow.com/2015/01/lua_53_update.html)


# Lua 工具

## [LuaPanda](https://github.com/Tencent/LuaPanda)

LuaPanda 是一个基于 VS Code 的 lua 代码工具，设计目标是简单易用。它支持多种开发框架，主要提供以下功能：

* 代码补全（code completion）
* 代码片段（snippet completion）
* 定义跳转（definition）
* 生成注释（comment generation）
* 类型推断（limited type inference）
* 代码格式化（formatting）: 依赖 lua-fmt
* 代码诊断（linting）：依赖 luacheck
* 调试器（debugger）

功能详情：https://github.com/Tencent/LuaPanda/blob/master/Docs/Manual/feature-introduction.md

LuaPanda 支持 lua5.1- 5.4，**调试器运行环境需要包含 luasocket**。


# Refer

* https://lua.org/manual/5.3/manual.html
* https://www.lua.org/manual/5.3/
* [Lua 5.3 参考手册](https://www.runoob.com/manual/lua53doc/contents.html)
