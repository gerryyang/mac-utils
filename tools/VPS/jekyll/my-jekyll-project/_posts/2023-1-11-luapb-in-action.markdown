---
layout: post
title:  "Luapb in Action"
date:   2023-1-11 20:00:00 +0800
categories: Lua
---

* Do not remove this line (it will not be displayed)
{:toc}

# Protocol Buffers


[Protocol Buffers](https://github.com/protocolbuffers/protobuf) (a.k.a., protobuf) are Google's language-neutral, platform-neutral, extensible mechanism for serializing structured data. You can find [protobuf's documentation on the Google Developers site](https://protobuf.dev/).

> Protocol buffers are Google’s language-neutral, platform-neutral, extensible mechanism for serializing structured data – think XML, but smaller, faster, and simpler. You define how you want your data to be structured once, then you can use special generated source code to easily write and read your structured data to and from a variety of data streams and using a variety of languages.

Protocol buffers currently support generated code in Java, Python, Objective-C, and C++. With our new proto3 language version, you can also work with Kotlin, Dart, Go, Ruby, PHP, and C#, with more languages to come.

More:

* https://protobuf.dev/overview/
* https://protobuf.dev/getting-started/
* https://github.com/protocolbuffers/protobuf/blob/main/docs/third_party.md


> 在 Lua 中操作 Google protobuf 格式数据。

[lua-protobuf](https://github.com/starwing/lua-protobuf) 提供在 Lua 全版本（5.1+、LuaJIT）下的 protobuf 2/3 版本支持。提供了高级的消息编码/解码接口以及底层的protobuf wireformat 二进制数据操作接口。

# Lua PB

Protocol Buffers 官方没有提供对 Lua 的支持，故参考其他第三方的实现方案。

## 方案：https://github.com/starwing/lua-protobuf

### 安装

> 注意：lua-prootbuf 毕竟是个纯 C 的 Lua 库，而 Lua 库的编译安装是有门槛的。如果遇到了问题，建议询问有 Lua 的 C 模块使用经验的人，或者参阅《Lua程序设计》里的相关内容，预先学习相关知识。

> Lua 的 C 模块是通用的，任何使用 Lua 的环境下都可以使用。

最简单的安装方法是使用 Lua 生态的包管理器 luarocks 进行安装。

```
pip install hererocks
hererocks -j 2.0 -rlatest .
```

```
git clone https://github.com/starwing/lua-protobuf
luarocks make rockspecs/lua-protobuf-scm-1.rockspec
```

生成静态库：

``` bash
#!/bin/bash

gcc -g -O2 -c pb.c -I ../lua
ar -scurv libpblua.a pb.o
rm pb.o
```

生成动态库：

``` bash
#!/bin/bash

gcc -O2 -shared -fPIC pb.c -o pb.so
```

## 单测

```
$lua test.lua
pb predefined types: 33
....................
Ran 20 tests in 0.006 seconds, 20 successes, 0 failures
OK
```

## 示例代码

``` lua
local pb = require "pb"
local protoc = require "protoc"

-- 直接载入schema (这么写只是方便, 生产环境推荐使用 protoc.new() 接口)
assert(protoc:load [[
   message Phone {
      optional string name        = 1;
      optional int64  phonenumber = 2;
   }
   message Person {
      optional string name     = 1;
      optional int32  age      = 2;
      optional string address  = 3;
      repeated Phone  contacts = 4;
   } ]])

-- lua 表数据
local data = {
   name = "ilse",
   age  = 18,
   contacts = {
      { name = "alice", phonenumber = 12312341234 },
      { name = "bob",   phonenumber = 45645674567 }
   }
}

-- 将Lua表编码为二进制数据
local bytes = assert(pb.encode("Person", data))
print(pb.tohex(bytes))

-- 再解码回Lua表
local data2 = assert(pb.decode("Person", bytes))
print(require "serpent".block(data2))
```

lua demo.lua 输出：

```
22 0D 10 F2 D5 FD EE 2D 0A 05 61 6C 69 63 65 22 0C 10 C7 F0 C6 85 AA 01 0A 03 62 6F 62 10 12 0A 04 69 6C 73 65
{
  age = 18,
  contacts = {
    {
      name = "alice",
      phonenumber = 12312341234
    } --[[table: 0x1d9d430]],
    {
      name = "bob",
      phonenumber = 45645674567
    } --[[table: 0x1d9d4c0]]
  } --[[table: 0x1d9d3f0]],
  name = "ilse"
} --[[table: 0x1d9d290]]
```


## 方案: https://github.com/cloudwu/sproto

Yet another protocol library like google protocol buffers , but simple and fast.

sproto 性能不错，云风强推第二版协议（第一版是pbc，兼容pb），已经不是 proto，而是一种新套接字，不能与 proto 兼容。






