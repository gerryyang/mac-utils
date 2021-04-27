---
layout: post
title:  "Protocol Buffers in Action"
date:   2021-04-05 20:00:00 +0800
categories: [Protocol Buffers,]
---

* Do not remove this line (it will not be displayed)
{:toc}

# 最佳实践

> 参考官方 [Style Guide](https://developers.google.com/protocol-buffers/docs/style)
> 
> Note that protocol buffer style has evolved over time, so it is likely that you will see `.proto` files written in different conventions or styles. **Please respect the existing style when you modify these files. Consistency is key.** However, it is best to adopt the current best style when you are creating a new `.proto` file.


## 工具

* Protobuf 编译器。尽量采用高版本的编译器，高版本的编译器向下兼容，即使是`3.x`版的编译器也依然兼容`proto2`。
* 由于不同版本的`protoc`生成的代码不兼容，因此在代码库中应该只放`.proto`文件，不应当放`protoc`编译生成的代码，这些文件应当由构建系统来处理。

## 组织结构

* 目录结构。目录结构建议和`package`名保持一致。比如`a.b`表示在`a/b`目录中。
* 文件名。`.proto`源文件名统一用**小蛇形（例如：`lower_snake_case.proto`）**命名。
* 避免庞大的 proto 定义。避免在一个 proto 文件中定义过多的内容。不但造成不必要的耦合，还会降低编译速度。一个 proto 文件里只放密切相关的一组定义，依赖的定义应当放在其他 proto 文件中，通过`import`的方式导入。

## 文件格式（File structure）

> All files should be ordered in the following manner:
1. License header (if applicable)
2. File overview
3. Syntax
4. Package
5. Imports (sorted)
6. File options
7. Everything else


* 文件编码。统一采用`UTF-8`编码。
* 缩进。采用`2空格`缩进，和Google官方规范保持一致。
* 注释。不仅可以在阅读代码时提供帮助信息，还能在程序中通过[反射机制获取注释信息](https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.descriptor#SourceLocation)。
* `.proto`文件整体结构。按照`版权信息`，`文件头注释`，`syntax语句`，`package语句`，`import语句`，`文件option`，其他内容的顺序组织定义，每个部分和其他部分之间空一行。
	+ 2014年Google在2.6.1发布后，版本号突然升到3.0，并引入了一种新的`proto3`语法，简化了语法，但是和原来的`proto2`并不完全兼容。新版本的编译器对于不带`syntax语句`的`.proto`文件，默认为` proto2`，但是会输出警告。`.proto`文件都带上`syntax语句`显式指定版本不但可以消除这个警告，同时也更清晰。
	+ `.proto`文件必须有`package`声明。`package`名统一小写，用点分割。可以按业务、模块需要引入子`package`。
	+ `import`用于导入其他`.proto`文件，按照字母顺序排列，不同来源的`import`之间可以用空行分组，不要导入未用到的文件。
	+ Protobuf支持设置一些文件级别的选项，用于控制代码生成等目的。例如：`option optimize_for = CODE_SIZE`

* 命名
	+ **消息名**，为名词短语，采用**大驼峰（例如：`CamelCase`）**命名法。
	+ **枚举类型名**，同消息名，采用**大驼峰（例如：`CamelCase`）**命名法。
  + **枚举值名**，采用**大蛇形（例如：`SNAKE_CASE`）**命名法。不要受C++影响以`k`开头。因为protobuf是跨语言的，在Java、Python等大部分语言，包括C++里传统上都允许用全大写表示常量。
	+ **字段名**，采用**小蛇形（例如：`snake_case`）**命名法。
		- `protoc`生成不同类型的目标语言的代码时，会把字段名自动转换为符合其代码规范的命名格式
		- `repeated`字段名用单词的复数
		- 每个消息的字段从`1`开始，依次递增。如果需要预留某些字段，请使用`reserved`语句进行标记并添加注释进行解释；如果需要废弃现存字段，请使用`deprecated`选项进行标记并添加注释进行解释。对于废弃的字段，建议不要删除，这样可以更好的说明编号不连续的原因，提高可维护性
		- 不要使用`required`字段。对于proto2，`required`字段会对未来的兼容带来潜在的风险。proto3中已经取消了`required`字段。详情参见[文档](https://developers.google.com/protocol-buffers/docs/proto#specifying_field_rules)
		- `default`值等同于类型的默认值时，不需要写出
	+ **服务名**，采用**大驼峰（例如：`CamelCase`）**命名，以`Service`结尾，例如：`QueryService`
	+ **方法名**，采用**大驼峰（例如：`CamelCase`）**命名，应当是动词或者动词短语，例如：`Query`

下面是一个简单的`.proto`模版。

``` proto
// Copyright (C) 2021 $company Inc.  All rights reserved.
//
// This is a sample protobuf source code.

syntax = "proto3";

package $company.protobuf.style.example;

import "google/protobuf/nested_extension.proto";
import "google/protobuf/nested_extension_lite.proto";

option optimize_for = LITE_RUNTIME;

message MyNestedExtension {
  optional bool is_extension = 1;
}

message MyNestedExtensionLite {
  extend MessageLiteToBeExtended {
    optional MessageLiteToBeExtended recursiveExtensionLite = 3;
  }
}

message Person {
  reserved 5, 7, 9 to 11;      // 因为 xxx 原因预留
  reserved "blood_type";       // 下一版本使用
  optional string name = 1;
  optional int32 age = 2;
  optional bool married = 3 [deprecated=true]; // 隐私条例更新, 不再记录婚姻状态
  optional int32 gender = 4;
}
```

# proto2 和 proto3的版本差异

* proto3 去掉了`required`字段，所有的singular字段都是`optional`的，因此也去掉了这两个关键字
* proto3 去掉了字段的默认值
* proto3 去掉了扩展，但是创建选项依然需要用 proto2 的扩展语法
* Proto3 的早期版本去掉了自动保留未知字段功能，直到[3.5 版才恢复](https://github.com/protocolbuffers/protobuf/issues/272)

版本兼容性：可以导入 proto3 消息类型并在 proto2 消息中使用它们，反之亦然。 但是，不能在 proto3 语法中使用 proto2 枚举。

# 选择正确的数据类型

Protobuf支持多种数据类型，这些数据类型都有不同的设计用意，用得好可以获得较好的性能，节省存储空间，保证良好的兼容性，反之则可能会带来麻烦。因此需要掌握规律，慎重选择。

## 整数类型

* 对于大部分场景，优先选用`int32`，如果不够考虑用`int64`。
* `int64`编译生成的消息会多占存储空间，但是序列化后并不比`int32`多费空间，因此，如果数值将来可能很大，且在目标语言环境下也没有性能和空间差异的问题，可以大胆采用。需要注意的是，在接口需要提供`Json`类型返回值时, `int64`在序列化时会自动转为`string`。
* `int32`和`int64`对绝对值小的负数编码很低效，`sint32`和`sint64`适合于经常会出现这种负数的情况，必要时可以谨慎选择。但是需要注意他们和`int32`及`int64`的编码方式并不兼容。
* `fixed32`、`fixed64`、`sfixed32`、`sfixed64`适合于数字通常很大（阈值分别是`28位`和`56位`）的场合，比如 IP 地址、hash 值等，编码更高效。但是 fixed 类型和常规的`int32`、`int64`不兼容，也不兼容其他整数类型。

## 浮点数类型

* `float`和`double`都是按定长方式保存的，因此互不兼容。
* 虽然用`float`会节省一些空间，但要考虑未来的精度和值域的兼容性，因此建议通常还是优先用`double`。
* 大量的浮点数，比如需要大量创建的消息对象，个数很多的`repeated`的字段等，用`float`保存时可以节省一些空间，在精度和未来的扩展都不是问题的情况下，可以考虑采用。

## string 和 bytes 类型

* 用`string`类型来表示`UTF-8`（包括`ASCII`）能表示的文本信息，比如文字消息，URL，ASCII化编码后的二进制数据（base64编码，十六进制文本表示的MD5等）。
* 用`bytes`类型来表示二进制字节流（通常不可读），比如序列化后的内容，二进制表示的MD5，非UTF8编码能表示的文本（比如GBK）。
* 如果用`string`类型保存非UTF-8兼容的内容，在C++生成的代码中，也会导致运行期警告。

# 服务设计

* 服务最好放在单独的`.proto`文件中。
* 避免在服务定义文件中定义除了请求和回应消息之外的消息类型，非RPC类型定义在其他的`.proto`文件中，通过`import`的方式导入。
* 避免单个服务的方法个数太多。
* 对于已经上线的服务，不得更改服务名、方法名、请求和返回值的类型。

# 协议更新

对于尚未发布上线的消息定义，可以自由更改。但是任何`.proto`文件发布后，再修改都会涉及到兼容性问题，因此除非特别必要，尽量不要破坏对先前版本的兼容性。

> 以下是官方的协议更新说明：

* 请勿更改任何现有字段的字段编号。
* `int32`，`uint32`，`int64`，`uint64`和 `bool` 都是兼容的。这意味着可以将字段从这些类型中的一种更改为另一种，而不会破坏向前或向后的兼容性。但是要注意的是，如果解析数值时用的类型和实际编码时用的类型不一致，则等效于在 C++ 中执行强制类型转换，如果数值存在溢出就会导致丢失（例如，如果将 64 位数值按 int32 来读取，它将被截断为 32 位，如果高 32 位非 0 则会丢失）。
* `sint32` 和 `sint64` 彼此兼容，但与其他整数类型不兼容。
* `string` 和 `bytes` 兼容，只要 bytes 的内容是有效的 `UTF-8`。
* 如果 `bytes` 包含消息的编码后的内容，则嵌入式消息与 `bytes` 兼容。
* `fixed32` 与 `sfixed32` 兼容，而 `fixed64` 与 `sfixed64` 兼容，信息不会丢失，但是可能会获得不一样的值的解释方式（比如以 `sfixed32` 表示的 `-1` 如果用 `fixed32` 来读取会得到 `0xFFFFFFFF`）。
* 就传输格式而言，`枚举`与 `int32`，`uint32`，`int64` 和 `uint64` 兼容（请注意，如果值不合适，将会被截断），但是请注意，在反序列化消息时，客户端代码可能会以不同的方式对待它们。
* 对于 `string`，`bytes` 和 `message` 字段，`optional` 与 `repeated` 兼容。当读取方以 `optional` 的定义方式解析以 `repeated` 的方式序列化的数据时，则如果是原始类型的字段，则将采用最后一个输入值；如果是消息类型的字段，则将合并所有的输入元素。请注意，这对于数值类型（包括布尔值和枚举）通常不安全，`repeated` 的数值类型的字段可以以 `packed` 的格式序列化（`proto3` 中默认开启），当读取方的定义是 `optional` 字段时，该格式将无法正确解析。
* 将单个可选值更改为新的 `oneof` 的成员是安全且二进制兼容的。如果您确定没有代码一次设置多个可选字段，则将它们移动到一个新建的 `oneof` 字段中可能是安全的。将任何字段移动到现有的 `oneof` 字段中都是不安全的。
* 只要在更新后的消息类型中不再使用字段号，就可以删除字段。可以重命名该字段，或者添加前缀 `OBSOLETE_`，或者保留该字段编号，以使 `.proto` 的将来的修改不会意外重用该编号。


# Defining A Message Type (Step by Step)

``` proto
syntax = "proto3";

/* SearchRequest represents a search query, with pagination options to
 * indicate which results to include in the response. */

message SearchRequest {
  string query = 1;
  int32 page_number = 2;       // Which page number do we want
  int32 result_per_page = 3;   // Number of results to return per page

  enum Corpus {
    UNIVERSAL = 0;
    WEB = 1;
    IMAGES = 2;
    LOCAL = 3;
    NEWS = 4;
    PRODUCTS = 5;
    VIDEO = 6;
  }
  Corpus corpus = 4;
}

message SearchResponse {
 ...
}
```


## Specifying Field Types

* [Scalar Value Types](https://developers.google.com/protocol-buffers/docs/proto3#scalar)
* [Enumerations](https://developers.google.com/protocol-buffers/docs/proto3#enum)


| .proto Type | C++ Type | Notes
| -- | -- | --
| double | double |
| float | float |
| int32 | int32 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint32 instead.
| int64 | int64 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint64 instead.
| uint32 | uint32 | Uses variable-length encoding.
| uint64 | uint64 | Uses variable-length encoding.
| sint32 | int32 | Uses variable-length encoding. **Signed int value. These more efficiently encode negative numbers than regular int32s**.
| sint64 | int64 | Uses variable-length encoding. **Signed int value. These more efficiently encode negative numbers than regular int64s**.
| fixed32 | uint32 | Always four bytes. **More efficient than uint32 if values are often greater than 2^28**.
| fixed64 | uint64 | Always eight bytes. **More efficient than uint64 if values are often greater than 2^56.**
| sfixed32 | int32 | Always four bytes.
| sfixed64 | int64 | Always eight bytes.
| bool | bool |
| string | string | **A string must always contain UTF-8 encoded or 7-bit ASCII text, and cannot be longer than 2^32.**
| bytes | string | **May contain any arbitrary sequence of bytes no longer than 2^32.**



## Assigning Field Numbers
	
* Note that field numbers in the range `1` through `15` **take one byte to encode**, including **the field number and the field's type**. So reserve the numbers `1` through `15` for very frequently occurring message elements. **Remember to leave some room for frequently occurring elements that might be added in the future**.
* **The smallest field number you can specify is `1`, and the largest is `2^29 - 1`, or `536,870,911`**. You also cannot use the numbers `19000` through `19999`, as they are reserved for the Protocol Buffers implementation - the protocol buffer compiler will complain if you use one of these reserved numbers in your `.proto`. Similarly, you cannot use any previously reserved field numbers.

## Specifying Field Rules

Message fields can be one of the following:

* `singular`: a well-formed message can have zero or one of this field (but not more than one). And this is the default field rule for proto3 syntax.
* `repeated`: this field can be repeated any number of times (including zero) in a well-formed message. The order of the repeated values will be preserved.

In proto3, `repeated` fields of scalar numeric types use `packed` encoding by default.

## Adding More Message Types

Multiple message types can be defined in a single `.proto` file. This is useful if you are defining multiple related messages – so, for example, if you wanted to define the reply message format that corresponds to your `SearchResponse` message type, you could add it to the same `.proto`.

## Adding Comments

To add comments to your `.proto` files, use C/C++-style `//` and `/* ... */` syntax.

## Reserved Fields

If you update a message type by entirely removing a field, or commenting it out, future users can reuse the field number when making their own updates to the type. This can cause severe issues if they later load old versions of the same `.proto`, including data corruption, privacy bugs, and so on. One way to make sure this doesn't happen is to specify that the field numbers (and/or names, which can also cause issues for JSON serialization) of your deleted fields are `reserved`. The protocol buffer compiler will complain if any future users try to use these field identifiers.

``` proto
message Foo {
  reserved 2, 15, 9 to 11;
  reserved "foo", "bar";
}
```

Note that you can't mix field names and field numbers in the same reserved statement.

## What's Generated From Your `.proto`?

When you run **the protocol buffer compiler** on a `.proto`, the compiler generates the code in your chosen language you'll need to work with the message types you've described in the file, including getting and setting field values, serializing your messages to an output stream, and parsing your messages from an input stream.

* For `C++`, the compiler generates a `.h` and `.cc` file from each `.proto`, with a `class` for each message type described in your file.
* For `Java`, the compiler generates a `.java` file with a class for each message type, as well as a special Builder classes for creating message class instances.
* For `Go`, the compiler generates a `.pb.go` file with a type for each message type in your file.
* ...

## Default Values

When a message is parsed, if the encoded message does not contain a particular singular element, the corresponding field in the parsed object is set to the default value for that field. These defaults are type-specific.

The default value for repeated fields is empty (generally an empty list in the appropriate language).


## Using Other Message Types

You can use other message types as field types. For example, let's say you wanted to include `Result` messages in each `SearchResponse` message – to do this, you can define a `Result` message type in the same `.proto` and then specify a field of type `Result` in `SearchResponse`:


``` proto
message SearchResponse {
  repeated Result results = 1;
}

message Result {
  string url = 1;
  string title = 2;
  repeated string snippets = 3;
}
```


## Importing Definitions

In the above example, the `Result` message type is defined in the same file as `SearchResponse` – what if the message type you want to use as a field type is already defined in another `.proto` file?

You can use definitions from other `.proto` files by importing them. To import another .proto's definitions, you add an import statement to the top of your file:

``` proto
import "myproject/other_protos.proto";
```

## Nested Types

You can define and use message types inside other message types, as in the following example – here the `Result` message is defined inside the `SearchResponse` message:

``` proto
message SearchResponse {
  message Result {
    string url = 1;
    string title = 2;
    repeated string snippets = 3;
  }
  repeated Result results = 1;
}
```

If you want to reuse this message type outside its parent message type, you refer to it as `_Parent_._Type_`:

``` proto
message SomeOtherMessage {
  SearchResponse.Result result = 1;
}
```

You can nest messages as deeply as you like:

``` proto
message Outer {                  // Level 0
  message MiddleAA {             // Level 1
    message Inner {              // Level 2
      int64 ival = 1;
      bool  booly = 2;
    }
  }

  message MiddleBB {             // Level 1
    message Inner {              // Level 2
      int32 ival = 1;
      bool  booly = 2;
    }
  }
}
```

## Updating A Message Type

It's very simple to update message types without breaking any of your existing code. Just remember the following rules:

* Don't change the field numbers for any existing fields.
* If you add new fields, any messages serialized by code using your "old" message format can still be parsed by your new generated code. You should keep in mind the default values for these elements so that new code can properly interact with messages generated by old code. Similarly, messages created by your new code can be parsed by your old code: old binaries simply ignore the new field when parsing. See the **Unknown Fields** section for details.
* Fields can be removed, as long as the field number is not used again in your updated message type. You may want to rename the field instead, perhaps adding the prefix `"OBSOLETE_"`, or make the field number `reserved`, so that future users of your `.proto` can't accidentally reuse the number.
* `int32`, `uint32`, `int64`, `uint64`, and `bool` are all compatible – **this means you can change a field from one of these types to another without breaking forwards- or backwards-compatibility**. If a number is parsed from the wire which doesn't fit in the corresponding type, you will get the same effect as if you had cast the number to that type in C++ (e.g. if a 64-bit number is read as an int32, it will be truncated to 32 bits).
* `sint32` and `sint64` are compatible with each other but are not compatible with the other integer types.
* `string` and `bytes` are compatible as long as the bytes are valid UTF-8.
* Embedded messages are compatible with `bytes` if the bytes contain an encoded version of the message.
* `fixed32` is compatible with `sfixed32`, and `fixed64` with `sfixed64`.
* For `string`, `bytes`, and `message fields`, `optional` is compatible with `repeated`.
* `enum` is compatible with `int32`, `uint32`, `int64`, and `uint64` in terms of wire format (note that values will be truncated if they don't fit). 
* Changing a single value into a member of a new `oneof` is safe and binary compatible. Moving multiple fields into a new `oneof` may be safe if you are sure that no code sets more than one at a time. Moving any fields into an existing `oneof` is not safe.


## Unknown Fields

`Unknown fields` are well-formed protocol buffer serialized data representing fields that the parser does not recognize. For example, when an old binary parses data sent by a new binary with new fields, those new fields become unknown fields in the old binary.

Originally, `proto3` messages always discarded unknown fields during parsing, but in version `3.5` we reintroduced the preservation of unknown fields to match the proto2 behavior. In versions `3.5` and later, unknown fields are retained during parsing and included in the serialized output.


## Any

The `Any` message type lets you use messages as embedded types without having their `.proto` definition. An `Any` contains an arbitrary serialized message as bytes, along with a URL that acts as a globally unique identifier for and resolves to that message's type. To use the `Any` type, you need to import `google/protobuf/any.proto`.

``` proto
import "google/protobuf/any.proto";

message ErrorStatus {
  string message = 1;
  repeated google.protobuf.Any details = 2;
}
```

## Oneof

If you have a message with many fields and where at most one field will be set at the same time, you can enforce this behavior and save memory by using the `oneof` feature.

To define a `oneof` in your `.proto` you use the `oneof` keyword followed by your oneof name, in this case `test_oneof`:

``` proto
message SampleMessage {
  oneof test_oneof {
    string name = 4;
    SubMessage sub_message = 9;
  }
}
```

You then add your oneof fields to the oneof definition. You can add fields of any type, except `map` fields and `repeated` fields.

In your generated code, oneof fields have the same getters and setters as regular fields. You also get a special method for checking which value (if any) in the oneof is set. 

## Maps

If you want to create an associative map as part of your data definition, protocol buffers provides a handy shortcut syntax:

``` proto
map<key_type, value_type> map_field = N;
```

* The `key_type` can be any integral or string type (so, any scalar type except for floating point types and bytes). Note that `enum` is not a valid `key_type`. 
* The `value_type` can be any type except another `map`.

So, for example, if you wanted to create a map of projects where each Project message is associated with a string key, you could define it like this:

``` proto
map<string, Project> projects = 3;
```

* Map fields cannot be `repeated`.
* Wire format ordering and map iteration ordering of map values is undefined, so you cannot rely on your map items being in a particular order.
* When generating text format for a `.proto`, maps are sorted by key. Numeric keys are sorted numerically.
* When parsing from the wire or when merging, if there are duplicate map keys the last key seen is used. When parsing a map from text format, parsing may fail if there are duplicate keys.
* If you provide a key but no value for a map field, the behavior when the field is serialized is language-dependent. In C++, Java, and Python the default value for the type is serialized, while in other languages nothing is serialized.

## Packages

You can add an optional `package` specifier to a `.proto` file to prevent name clashes between protocol message types.

``` proto
package foo.bar;
message Open { ... }
```

You can then use the package specifier when defining fields of your message type:

``` proto
message Foo {
  ...
  foo.bar.Open open = 1;
  ...
}
```

The way a package specifier affects the generated code depends on your chosen language:

* In **C++** the generated classes are wrapped inside a C++ namespace. For example, `Open` would be in the namespace `foo::bar`.
* In **Java**, the package is used as the Java package, unless you explicitly provide an option `java_package` in your `.proto` file.
* In **Go**, the package is used as the Go package name, unless you explicitly provide an option `go_package` in your `.proto` file.
* ...

## Defining Services

If you want to use your message types with an **RPC (Remote Procedure Call)** system, you can define an RPC service interface in a `.proto` file and the protocol buffer compiler will generate **service interface code and stubs** in your chosen language. So, for example, if you want to define an RPC service with a method that takes your `SearchRequest` and returns a `SearchResponse`, you can define it in your `.proto` file as follows:

``` proto
service SearchService {
  rpc Search(SearchRequest) returns (SearchResponse);
}
```

The most straightforward RPC system to use with protocol buffers is [gRPC](https://grpc.io/): **a language- and platform-neutral open source RPC system developed at Google**. gRPC works particularly well with protocol buffers and lets you generate the relevant RPC code directly from your `.proto` files using a special protocol buffer compiler plugin.

By default, the protocol compiler will then generate **an abstract interface called `SearchService`** and **a corresponding "stub" implementation**. The stub forwards all calls to an `RpcChannel`, which in turn is an abstract interface that you must define yourself in terms of your own RPC system. For example, you might implement an `RpcChannel` which serializes the message and sends it to a server via HTTP. In other words, the generated stub provides a type-safe interface for making protocol-buffer-based RPC calls, without locking you into any particular RPC implementation. So, in C++, you might end up with code like this:

``` cpp
using google::protobuf;

protobuf::RpcChannel* channel;
protobuf::RpcController* controller;
SearchService* service;
SearchRequest request;
SearchResponse response;

void DoSearch() {
  // You provide classes MyRpcChannel and MyRpcController, which implement
  // the abstract interfaces protobuf::RpcChannel and protobuf::RpcController.
  channel = new MyRpcChannel("somehost.example.com:1234");
  controller = new MyRpcController;

  // The protocol compiler generates the SearchService class based on the
  // definition given above.
  service = new SearchService::Stub(channel);

  // Set up the request.
  request.set_query("protocol buffers");

  // Execute the RPC.
  service->Search(controller, request, response, protobuf::NewCallback(&Done));
}

void Done() {
  delete service;
  delete channel;
  delete controller;
}
```

All service classes also implement the `Service` interface, which provides a way to call specific methods without knowing the method name or its input and output types at compile time. On the server side, this can be used to implement an RPC server with which you could register services.

``` cpp
using google::protobuf;

class ExampleSearchService : public SearchService {
 public:
  void Search(protobuf::RpcController* controller,
              const SearchRequest* request,
              SearchResponse* response,
              protobuf::Closure* done) {
    if (request->query() == "google") {
      response->add_result()->set_url("http://www.google.com");
    } else if (request->query() == "protocol buffers") {
      response->add_result()->set_url("http://protobuf.googlecode.com");
    }
    done->Run();
  }
};

int main() {
  // You provide class MyRpcServer.  It does not have to implement any
  // particular interface; this is just an example.
  MyRpcServer server;

  protobuf::Service* service = new ExampleSearchService;
  server.ExportOnPort(1234, service);
  server.Run();

  delete service;
  return 0;
}
```

There are also a number of ongoing third-party projects to develop RPC implementations for Protocol Buffers. For a list of links to projects we know about, see the [third-party add-ons wiki page](https://github.com/protocolbuffers/protobuf/blob/master/docs/third_party.md).

## JSON 

Proto3 supports a canonical encoding in JSON, making it easier to share data between systems.

https://developers.google.com/protocol-buffers/docs/proto3#json

## Options

Individual declarations in a `.proto` file can be annotated with a number of options. Options do not change the overall meaning of a declaration, but may affect the way it is handled in a particular context. The complete list of available options is defined in `google/protobuf/descriptor.proto`.

Here are a few of the most commonly used options:

* `optimize_for` (file option): Can be set to `SPEED`, `CODE_SIZE`, or `LITE_RUNTIME`. This affects the C++ and Java code generators (and possibly third-party generators) in the following ways:
	+ `SPEED (default)`: The protocol buffer compiler will generate code for serializing, parsing, and performing other common operations on your message types. This code is highly optimized.
	+ `CODE_SIZE`: The protocol buffer compiler will generate minimal classes and will rely on shared, reflection-based code to implement serialialization, parsing, and various other operations. The generated code will thus be much smaller than with `SPEED`, but operations will be slower. Classes will still implement exactly the same public API as they do in `SPEED` mode. This mode is most useful in apps that contain a very large number `.proto` files and do not need all of them to be blindingly fast.
	+ `LITE_RUNTIME`: The protocol buffer compiler will generate classes that depend only on the "lite" runtime library (`libprotobuf-lite` instead of `libprotobuf`). The lite runtime is much smaller than the full library (around an order of magnitude smaller) but omits certain features like descriptors and reflection. This is particularly useful for apps running on constrained platforms like mobile phones. The compiler will still generate fast implementations of all methods as it does in SPEED mode. Generated classes will only implement the `MessageLite` interface in each language, which provides only a subset of the methods of the full `Message` interface.


``` proto
option optimize_for = CODE_SIZE;
```

https://developers.google.com/protocol-buffers/docs/proto3#options

## Generating Your Classes

To generate the Java, Python, C++, Go, Ruby, Objective-C, or C# code you need to work with the message types defined in a `.proto` file, you need to run the protocol buffer compiler `protoc` on the `.proto`. 

The Protocol Compiler is invoked as follows:

```
protoc --proto_path=IMPORT_PATH --cpp_out=DST_DIR --java_out=DST_DIR --python_out=DST_DIR --go_out=DST_DIR --ruby_out=DST_DIR --objc_out=DST_DIR --csharp_out=DST_DIR path/to/file.proto
```

* `IMPORT_PATH` specifies a directory in which to look for `.proto` files when resolving import directives. If omitted, the current directory is used. Multiple import directories can be specified by passing the `--proto_path` option multiple times; they will be searched in order. `-I=_IMPORT_PATH_` can be used as a short form of `--proto_path`.
* You can provide one or more output directives:
	+ `--cpp_out` generates C++ code in `DST_DIR`. See the [C++ generated code reference](https://developers.google.com/protocol-buffers/docs/reference/cpp-generated) for more.
	+ `--java_out` generates Java code in `DST_DIR`. See the [Java generated code reference](https://developers.google.com/protocol-buffers/docs/reference/java-generated) for more.
	+ `--go_out` generates Go code in `DST_DIR`. See the [Go generated code reference](https://developers.google.com/protocol-buffers/docs/reference/go-generated) for more.
	+ ...
* You must provide one or more `.proto` files as input. Multiple `.proto` files can be specified at once. Although the files are named relative to the current directory, each file must reside in one of the `IMPORT_PATHs` so that the compiler can determine its canonical name.

# Encoding

## A Simple Message

Let's say you have the following very simple message definition:

``` proto
message Test1 {
  optional int32 a = 1;
}
```

In an application, you create a Test1 message and set a to 150. You then serialize the message to an output stream. If you were able to examine the encoded message, you'd see three bytes:

```
08 96 01
```

So far, so small and numeric – but what does it mean? Read on...

## Base 128 Varints

To understand your simple protocol buffer encoding, you first need to understand `varints`. **Varints are a method of serializing integers using one or more bytes. Smaller numbers take a smaller number of bytes**.

When a message is encoded, the keys and values are concatenated into a byte stream. When the message is being decoded, the parser needs to be able to skip fields that it doesn't recognize. This way, new fields can be added to a message without breaking old programs that do not know about them. 

https://developers.google.com/protocol-buffers/docs/encoding#varints


# Other

* [Protobuffs import from another directory](https://stackoverflow.com/questions/21159451/protobuffs-import-from-another-directory)

```
--proto_path
```

# [Protocol Buffer Basics: C++](https://developers.google.com/protocol-buffers/docs/cpptutorial)

## Defining Your Protocol Format

``` proto3
// Copyright (C) 2021 $company Inc.  All rights reserved.
//
// This is a sample protobuf source code.

// addressbook.proto

syntax = "proto3";

package demo;

message Person {
  string name = 1;
  int32 id = 2;
  string email = 3;

  enum PhoneType {
    MOBILE = 0;
    HOME = 1;
    WORK = 2;
  }

  message PhoneNumber {
    string number = 1;
    PhoneType type = 2;
  }

  repeated PhoneNumber phones = 4;
}

message AddressBook {
  repeated Person people = 1;
}
```

* The `.proto` file starts with a `package` declaration, which helps to prevent naming conflicts between different projects. In C++, your generated classes will be placed in a `namespace` matching the package name.

* Next, you have your message definitions. A message is just an aggregate containing a set of typed fields. Many standard simple data types are available as field types, including `bool`, `int32`, `float`, `double`, and `string`. You can also add further structure to your messages by using other message types as field types – in the above example the `Person` message contains `PhoneNumber` messages, while the `AddressBook` message contains `Person` messages. You can even define message types nested inside other messages – as you can see, the `PhoneNumber` type is defined inside `Person`. You can also define `enum` types if you want one of your fields to have one of a predefined list of values – here you want to specify that a phone number can be one of `MOBILE`, `HOME`, or `WORK`.

* The " = 1", " = 2" markers on each element identify the unique "tag" that field uses in the binary encoding. Tag numbers 1-15 require one less byte to encode than higher numbers, so as an optimization you can decide to use those tags for the commonly used or repeated elements, leaving tags 16 and higher for less-commonly used optional elements. **Each element in a repeated field requires re-encoding the tag number, so repeated fields are particularly good candidates for this optimization**.

* Each field must be annotated with one of the following modifiers:
  + `optional`: the field may or may not be set. If an optional field value isn't set, a default value is used. **For simple types, you can specify your own default value, as we've done for the phone number type in the example. Otherwise, a system default is used: `zero` for numeric types, `the empty string` for strings, `false` for bools. For embedded messages, the default value is always the "default instance" or "prototype" of the message, which has none of its fields set.** Calling the accessor to get the value of an optional (or required) field which has not been explicitly set always returns that field's default value.
  + `repeated`: the field may be repeated any number of times (including zero). The order of the repeated values will be preserved in the protocol buffer. **Think of repeated fields as dynamically sized arrays**.
  + `required`: a value for the field must be provided, otherwise the message will be considered "uninitialized". If `libprotobuf` is compiled in `debug` mode, serializing an uninitialized message will cause an assertion failure. In optimized builds, the check is skipped and the message will be written anyway. However, parsing an uninitialized message will always fail (by returning false from the parse method). Other than this, a required field behaves exactly like an `optional` field.

> Note: **Required Is Forever You should be very careful about marking fields as required**. If at some point you wish to stop writing or sending a `required` field, it will be problematic to change the field to an optional field – old readers will consider messages without this field to be incomplete and may reject or drop them unintentionally. You should consider writing application-specific custom validation routines for your buffers instead. **Within Google, required fields are strongly disfavored; most messages defined in proto2 syntax use `optional` and `repeated` only. (Proto3 does not support required fields at all.)**


## Compiling Your Protocol Buffers

* Now that you have a `.proto`, the next thing you need to do is generate the classes you'll need to read and write `AddressBook` (and hence `Person` and `PhoneNumber`) messages. To do this, you need to run the protocol buffer compiler `protoc` on your `.proto`:

* If you haven't installed the compiler, [download the package](https://developers.google.com/protocol-buffers/docs/downloads) and follow the instructions in the README.

```
$ protoc --version
libprotoc 3.15.8
```

* Now run the compiler, specifying the destination directory (where you want the generated code to go), and the path to your `.proto`. This generates the following files in your specified destination directory:
  + `addressbook.pb.h`, the header which declares your generated classes.
  + `addressbook.pb.cc`, which contains the implementation of your classes.


``` bash
#!/bin/bash

PROTOCOL_DIR=./
PROTOCOL_SRC_DIR=../src_protocol

PROTOCOL_FILES="addressbook.proto"

function Proc {
        for file in $1
        do
                protoc --cpp_out=$PROTOCOL_SRC_DIR --proto_path=$PROTOCOL_DIR $file
                if [ $? -ne 0 ]; then
                        echo "protoc $file failed"
                        exit 1
                fi
        done
}

Proc "$PROTOCOL_FILES"

echo "ok"
```

## The Protocol Buffer API

Let's look at some of the generated code and see what classes and functions the compiler has created for you. If you look in `addressbook.pb.h`, you can see that you have a class for each message you specified in `addressbook.proto`. Looking closer at the Person class, you can see that the compiler has generated accessors for each field. For example, for the `name`, `id`, `email`, and `phones` fields, you have these methods:

``` cpp
// string name = 1;
void clear_name();
const std::string& name() const;
void set_name(const std::string& value);
void set_name(std::string&& value);
void set_name(const char* value);
void set_name(const char* value, size_t size);
std::string* mutable_name();
std::string* release_name();
void set_allocated_name(std::string* name);
private:
const std::string& _internal_name() const;
void _internal_set_name(const std::string& value);
std::string* _internal_mutable_name();

// int32 id = 2;
void clear_id();
::PROTOBUF_NAMESPACE_ID::int32 id() const;
void set_id(::PROTOBUF_NAMESPACE_ID::int32 value);

// repeated .demo.Person.PhoneNumber phones = 4;
inline int Person::_internal_phones_size() const {
  return phones_.size();
}
inline int Person::phones_size() const {
  return _internal_phones_size();
}
inline void Person::clear_phones() {
  phones_.Clear();
}
inline ::demo::Person_PhoneNumber* Person::mutable_phones(int index) {
  // @@protoc_insertion_point(field_mutable:demo.Person.phones)
  return phones_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::demo::Person_PhoneNumber >*
Person::mutable_phones() {
  // @@protoc_insertion_point(field_mutable_list:demo.Person.phones)
  return &phones_;
}
inline const ::demo::Person_PhoneNumber& Person::_internal_phones(int index) const {
  return phones_.Get(index);
}
inline const ::demo::Person_PhoneNumber& Person::phones(int index) const {
  // @@protoc_insertion_point(field_get:demo.Person.phones)
  return _internal_phones(index);
}
inline ::demo::Person_PhoneNumber* Person::_internal_add_phones() {
  return phones_.Add();
}
inline ::demo::Person_PhoneNumber* Person::add_phones() {
  // @@protoc_insertion_point(field_add:demo.Person.phones)
  return _internal_add_phones();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::demo::Person_PhoneNumber >&
Person::phones() const {
  // @@protoc_insertion_point(field_list:demo.Person.phones)
  return phones_;
}

// oneof para
enum : int {
    kAFieldNumber = 1, 
    kBFieldNumber = 2, 
  };
  // int32 a = 1;
  bool has_a() const;
  void clear_a();
  ::PROTOBUF_NAMESPACE_ID::int32 a() const;
  void set_a(::PROTOBUF_NAMESPACE_ID::int32 value);

  // string b = 2;
  bool has_b() const;
  void clear_b();
  const std::string& b() const;
  void set_b(const std::string& value);
  void set_b(std::string&& value);
  void set_b(const char* value);
  void set_b(const char* value, size_t size);
  std::string* mutable_b();
  std::string* release_b();
  void set_allocated_b(std::string* b);

// map<string, string> meta = 5;
inline int Person::meta_size() const {
  return _internal_meta_size();
}
inline void Person::clear_meta() {
  meta_.Clear();
}
inline const ::PROTOBUF_NAMESPACE_ID::Map< std::string, std::string >&
Person::meta() const {
  // @@protoc_insertion_point(field_map:tutorial.Person.meta)
  return _internal_meta();
}
inline ::PROTOBUF_NAMESPACE_ID::Map< std::string, std::string >*
Person::mutable_meta() {
  // @@protoc_insertion_point(field_mutable_map:tutorial.Person.meta)
  return _internal_mutable_meta();
}
```

* the `getters` have exactly the name as the field in lowercase
* the `setter` methods begin with **set_**
* There are also **has_** methods for each singular (required or optional) field which return true if that field has been set
* Finally, each field has a **clear_** method that un-sets the field back to its empty state.
* The `name` and `email` fields have a couple of extra methods because they're strings – a **mutable_** getter that lets you get a direct pointer to the string, and an extra setter. 
* Repeated fields also have some special methods
  + check the repeated field's **_size** (in other words, how many phone numbers are associated with this Person).
  + get a specified phone number using its index.
  + update an existing phone number at the specified index.
  + add another phone number to the message which you can then edit (repeated scalar types have an **add_** that just lets you pass in the new value).

More: [C++ Generated Code](https://developers.google.com/protocol-buffers/docs/reference/cpp-generated)


## Standard Message Methods

Each message class also contains a number of other methods that let you check or manipulate the entire message, including:

* `bool IsInitialized() const;`: checks if all the required fields have been set.
* `string DebugString() const;`: returns a human-readable representation of the message, particularly useful for debugging.
* `void CopyFrom(const Person& from);`: overwrites the message with the given message's values.
* `void Clear();`: clears all the elements back to the empty state.

These and the I/O methods described in the following section implement the `Message` interface shared by all C++ protocol buffer classes. For more info, see [the complete API documentation for Message](https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.message#Message).

## Parsing and Serialization

Finally, each protocol buffer class has methods for writing and reading messages of your chosen type using the protocol buffer [binary format](https://developers.google.com/protocol-buffers/docs/encoding). These include:

* `bool SerializeToString(string* output) const;`: serializes the message and stores the bytes in the given string. Note that the bytes are binary, not text; we only use the string class as a convenient container.
* `bool ParseFromString(const string& data);`: parses a message from the given string.
* `bool SerializeToOstream(ostream* output) const;`: writes the message to the given C++ ostream.
* `bool ParseFromIstream(istream* input);`: parses a message from the given C++ istream.

These are just a couple of the options provided for parsing and serialization. Again, see the [Message API reference](https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.message#Message) for a complete list.

> Note: **Protocol Buffers and Object Oriented Design Protocol buffer classes are basically dumb data holders (like structs in C); they don't make good first class citizens in an object model.** If you want to add richer behavior to a generated class, the best way to do this is to wrap the generated protocol buffer class in an application-specific class. Wrapping protocol buffers is also a good idea if you don't have control over the design of the .proto file (if, say, you're reusing one from another project). In that case, you can use the wrapper class to craft an interface better suited to the unique environment of your application: hiding some data and methods, exposing convenience functions, etc. **You should never add behaviour to the generated classes by inheriting from them. This will break internal mechanisms and is not good object-oriented practice anyway**.

## Writing A Message

Now let's try using your protocol buffer classes. The first thing you want your address book application to be able to do is write personal details to your address book file. To do this, you need to create and populate instances of your protocol buffer classes and then write them to an output stream.

Here is a program which reads an `AddressBook` from a file, adds one new `Person` to it based on user input, and writes the new `AddressBook` back out to the file again. The parts which directly call or reference code generated by the protocol compiler are highlighted.


* Notice the `GOOGLE_PROTOBUF_VERIFY_VERSION` macro. It is good practice – though not strictly necessary – to execute this macro before using the C++ Protocol Buffer library. It verifies that you have not accidentally linked against a version of the library which is incompatible with the version of the headers you compiled with. If a version mismatch is detected, the program will abort. Note that every `.pb.cc` file automatically invokes this macro on startup.
* Also notice the call to `ShutdownProtobufLibrary()` at the end of the program. All this does is delete any global objects that were allocated by the Protocol Buffer library. This is unnecessary for most programs, since the process is just going to exit anyway and the OS will take care of reclaiming all of its memory. However, if you use a memory leak checker that requires that every last object be freed, or if you are writing a library which may be loaded and unloaded multiple times by a single process, then you may want to force Protocol Buffers to clean up everything.

``` cc
// ...
```


## Reading A Message

Of course, an address book wouldn't be much use if you couldn't get any information out of it! This example reads the file created by the above example and prints all the information in it.

``` cc
// ...
```

## Extending a Protocol Buffer

https://developers.google.com/protocol-buffers/docs/cpptutorial#extending-a-protocol-buffer

## Optimization Tips

The C++ Protocol Buffers library is extremely heavily optimized. However, proper usage can improve performance even more. Here are some tips for squeezing every last drop of speed out of the library:

* Reuse message objects when possible. Messages try to keep around any memory they allocate for reuse, even when they are cleared. Thus, if you are handling many messages with the same type and similar structure in succession, it is a good idea to reuse the same message object each time to take load off the memory allocator. However, objects can become bloated over time, especially if your messages vary in "shape" or if you occasionally construct a message that is much larger than usual. You should monitor the sizes of your message objects by calling the SpaceUsed method and delete them once they get too big.

* Your system's memory allocator may not be well-optimized for allocating lots of small objects from multiple threads. Try using [Google's tcmalloc](https://github.com/gperftools/gperftools) instead.

## Advanced Usage

Protocol buffers have uses that go beyond simple accessors and serialization. Be sure to explore the [C++ API reference](https://developers.google.com/protocol-buffers/docs/reference/cpp) to see what else you can do with them.

One key feature provided by protocol message classes is `reflection`. You can iterate over the fields of a message and manipulate their values without writing your code against any specific message type. **One very useful way to use reflection is for converting protocol messages to and from other encodings, such as XML or JSON.** A more advanced use of reflection might be to find differences between two messages of the same type, or to develop a sort of "regular expressions for protocol messages" in which you can write expressions that match certain message contents. If you use your imagination, it's possible to apply Protocol Buffers to a much wider range of problems than you might initially expect!

Reflection is provided by the [Message::Reflection](https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.message#Message.Reflection) interface.

# C++ API

https://developers.google.com/protocol-buffers/docs/reference/cpp

# 关键结构

## RepeatedField / RepeatedPtrField 

https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.repeated_field

> **RRepeatedField** is used to represent repeated fields of a primitive type (in other words, everything except strings and nested Messages).
> **RepeatedPtrField** is like RepeatedField, but used for repeated strings or Messages.

* `RepeatedField`和`RepeatedPtrField`是用来操作管理`repeated`类型字段的`class`。它们的功能和`STL vector`非常相似，不同的是针对`Protocol Buffers`做了很多相关的优化。
* `RepeatedPtrField`与`STL vector`特别不一样的地方在于，它对指针所有权的管理。
* 通常来说，客户端不应该直接操作`RepeatedField`对象，而是应该通过`protoc`生成的[accessor functions](https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.repeated_field#RepeatedField)来操作。

``` 
#include <google/protobuf/repeated_field.h>
namespace google::protobuf

template <typename Element>
class RepeatedField

RepeatedField()
explicit RepeatedField(Arena * arena)
RepeatedField(const RepeatedField & other)
template RepeatedField(Iter begin, const Iter & end)


typedef Element * iterator  // STL-like iterator support.
typedef Element value_type

typedef value_type & reference
typedef value_type * pointer

typedef int size_type
typedef ptrdiff_t difference_type

const typedef Element * const_iterator
const typedef value_type & const_reference
const typedef value_type * const_pointer

typedef std::reverse_iterator< const_iterator >  // Reverse iterator support.
typedef std::reverse_iterator< iterator > reverse_iterator
```

# 性能测试

## 在线工具 [Quick C++ Benchmark](https://quick-bench.com/)

![quick_cpp_benchmark](/assets/images/202104/quick_cpp_benchmark.png)

``` cpp
static void VectorFind(benchmark::State& state) {

  int max = 10000;
  std::string last_v = std::to_string(max - 1);

  std::vector<std::string> vec;
  for (int i = 0; i != max; ++i) {
    vec.push_back(std::to_string(i));
  }

  for (auto _ : state) {
    
  for (int i = 0; i != max; ++i) {
    if (vec[i] == last_v) {
      break;
    }
  }

  //std::find(vec.begin(), vec.end(), last_v);

  }
}
BENCHMARK(VectorFind);

static void SetFind(benchmark::State& state) {

  int max = 10000;
  std::string last_v = std::to_string(max - 1);

  std::set<std::string> set;
  for (int i = 0; i != max; ++i) {
    set.insert(std::to_string(i));
  }

  for (auto _ : state) {
    set.find(last_v);

  }
}

BENCHMARK(SetFind);
```
## [Celero](https://github.com/DigitalInBlue/Celero)

* [测试代码](https://github.com/gerryyang/mac-utils/blob/master/programing/protocol-buffers/tutorial/src/celero_benchmark.cc)：

```
 $ ./celero_benchmark 
Celero
Timer resolution: 0.001000 us
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |   RAM (bytes)   |   
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|find            | vector          |            Null |               1 |               1 |         1.00000 |       172.00000 |         5813.95 |        51277824 | 
|find            | pb_repeated     |            Null |               1 |               1 |        73.59302 |     12658.00000 |           79.00 |        51777536 | 
|find            | set             |            Null |              10 |              20 |         0.00058 |         0.10000 |     10000000.00 |        51777536 | 
|find            | unordered_set   |            Null |              10 |              20 |         0.00029 |         0.05000 |     20000000.00 |        51777536 | 
|find            | flat_set        |            Null |              10 |              20 |         0.00058 |         0.10000 |     10000000.00 |        51777536 | 
Completed in 00:00:00.028138
```

## vector/map/unordered_map/pb repeated

* [测试代码](https://github.com/gerryyang/mac-utils/blob/master/programing/protocol-buffers/tutorial/src/press.cc)：
 

```
$ perf stat -B ./press
pid(2008)
pb repeated
find it(999999) cnt(999999)
elapse(0.280948s)

 Performance counter stats for './press':

        606.010051      task-clock (msec)         #    0.858 CPUs utilized          
               730      context-switches          #    0.001 M/sec                  
                 0      cpu-migrations            #    0.000 K/sec                  
           123,296      page-faults               #    0.203 M/sec                  
   <not supported>      cycles                                                      
   <not supported>      instructions                                                
   <not supported>      branches                                                    
   <not supported>      branch-misses                                               

       0.706142993 seconds time elapsed


$ perf stat -B ./press
pid(2277)
map
find it(999999) cnt(999999)
elapse(0.0260171s)

 Performance counter stats for './press':

        457.361877      task-clock (msec)         #    0.914 CPUs utilized          
               572      context-switches          #    0.001 M/sec                  
                 0      cpu-migrations            #    0.000 K/sec                  
            19,711      page-faults               #    0.043 M/sec                  
   <not supported>      cycles                                                      
   <not supported>      instructions                                                
   <not supported>      branches                                                    
   <not supported>      branch-misses                                               

       0.500588136 seconds time elapsed
```



# Refer

* [Style Guide](https://developers.google.com/protocol-buffers/docs/style)
* [Language Guide (proto3)](https://developers.google.com/protocol-buffers/docs/proto3)
* [Language Guide (proto2)](https://developers.google.com/protocol-buffers/docs/proto)
* [Tutorials](https://developers.google.com/protocol-buffers/docs/tutorials)
* [Tutorials - Protocol Buffer Basics: C++](https://developers.google.com/protocol-buffers/docs/cpptutorial)
* [Protocol Buffer Encoding](https://developers.google.com/protocol-buffers/docs/encoding)
* [API Reference](https://developers.google.com/protocol-buffers/docs/reference/overview)


