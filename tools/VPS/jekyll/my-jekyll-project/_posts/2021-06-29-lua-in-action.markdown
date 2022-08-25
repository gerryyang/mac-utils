---
layout: post
title:  "Lua in Action"
date:   2021-06-29 20:00:00 +0800
categories: Lua
---

* Do not remove this line (it will not be displayed)
{:toc}



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


# Tools

* https://github.com/trumanzhao/luna



# Refer

* https://lua.org/manual/5.3/manual.html

