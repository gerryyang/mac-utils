---
layout: post
title:  "GCC/Clang Compilation Optimize"
date:   2021-02-13 14:00:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}

# [GNU Binutils](https://en.wikipedia.org/wiki/GNU_Binutils)

The **GNU Binary Utilities**, or **binutils**, are a set of programming tools for creating and managing binary programs, object files, libraries, profile data, and assembly source code.

The GNU Binutils are typically used in conjunction with compilers such as the GNU Compiler Collection (`gcc`), build tools like `make`, and the GNU Debugger (`gdb`).

The binutils include the following commands:

| name | usage |
| -- | --
| [as](https://en.wikipedia.org/wiki/GNU_Assembler) | [assembler](https://en.wikipedia.org/wiki/Assembly_language#Assembler) popularly known as GAS (GNU Assembler)
| [ld](https://en.wikipedia.org/wiki/GNU_linker) | [linker](https://en.wikipedia.org/wiki/Linker_(computing))
| [gprof](https://en.wikipedia.org/wiki/Gprof) | [profiler](https://en.wikipedia.org/wiki/Profiling_(computer_programming))
| [addr2line](https://en.wikipedia.org/w/index.php?title=Addr2line&action=edit&redlink=1) | convert address to file and line
| [ar](https://en.wikipedia.org/wiki/Ar_(Unix)) | create, modify, and extract from [archives](https://en.wikipedia.org/wiki/Archive_file)
| c++filt | [demangling](https://en.wikipedia.org/wiki/Name_mangling#Name_mangling_in_C++) filter for C++ symbols
| dlltool | creation of Windows [dynamic-link libraries](https://en.wikipedia.org/wiki/Dynamic-link_library)
| [gold](https://en.wikipedia.org/wiki/Gold_(linker)) | alternative linker for [ELF](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format) files
| nlmconv | object file conversion to a NetWare Loadable Module
| [nm](https://en.wikipedia.org/wiki/Nm_(Unix)) | list symbols exported by object file
| objcopy | copy object files, possibly making changes
| [objdump](https://en.wikipedia.org/wiki/Objdump) | dump information about object files
| [ranlib](https://en.wikipedia.org/wiki/Ranlib) | generate indices for archives (for compatibility; same as `ar -s`)
| [readelf](https://en.wikipedia.org/wiki/Readelf) | display content of ELF files
| size | list total and section sizes
| [strings](https://en.wikipedia.org/wiki/Strings_(Unix)) | list printable strings
| [strip](https://en.wikipedia.org/wiki/Strip_(Unix)) | remove symbols from an object file
| windmc | generates Windows message resources
| windres | compiler for Windows resource files



# [DWARF](https://en.wikipedia.org/wiki/DWARF)

`DWARF` is a widely used, standardized [debugging data format](https://en.wikipedia.org/wiki/Debugging_data_format). `DWARF` was originally designed along with [Executable and Linkable Format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format) (`ELF`), although it is independent of object file formats. The name is a medieval fantasy complement to "ELF" that had no official meaning, although the backronym "Debugging With Arbitrary Record Formats" has since been proposed.

DWARF 是一种用于表示源代码调试信息的标准格式。调试信息通常包括变量名、类型信息、行号等，用于在调试过程中帮助开发人员了解程序的运行状态。DWARF 的不同版本提供了不同的特性和优化，其中 DWARF version 5 是最新的版本，它引入了许多改进，包括更紧凑的表示形式和更高效的数据访问方式。

GCC 11 将 DWARF version 5 作为默认的 debug info 版本，这意味着当使用 GCC 11 编译项目时，生成的二进制文件将包含 DWARF version 5 格式的调试信息。由于 DWARF version 5 的优化，这使得生成的二进制文件尺寸显著缩小，同时仍保留了丰富的调试信息。

在实际项目中，这种尺寸缩小可以带来诸多好处，如节省磁盘空间、加快传输速度和提高加载速度等。因此，升级到 GCC 11 可以帮助开发人员更高效地处理大型项目和二进制文件。


More:

* https://en.wikipedia.org/wiki/DWARF
* [DWARF, 调试信息存储格式](https://zhuanlan.zhihu.com/p/419908664)






# [Precompiled Headers](https://clang.llvm.org/docs/PCHInternals.html)

This document describes the design and implementation of Clang's precompiled headers (PCH). If you are interested in the end-user view, please see the [User's Manual](https://releases.llvm.org/3.1/tools/clang/docs/UsersManual.html#precompiledheaders).

更多参考：

* [Clang precompiled headers and improving C++ compile times, conclusion](http://llunak.blogspot.com/2021/04/clang-precompiled-headers-and-improving.html)
* [Why precompiled headers do (not) improve C++ compile times](http://llunak.blogspot.com/2019/05/why-precompiled-headers-do-not-improve.html)
* [How do I generate and use precompiled headers with Clang++?](https://stackoverflow.com/questions/55885920/how-do-i-generate-and-use-precompiled-headers-with-clang)


## Using Precompiled Headers with clang

The Clang compiler frontend, `clang -cc1`, supports two command line options for generating and using `PCH` files.

To generate `PCH` files using `clang -cc1`, use the option `-emit-pch`:

```
$ clang -cc1 test.h -emit-pch -o test.h.pch
```

This option is transparently used by clang when generating `PCH` files. The resulting `PCH` file contains the serialized form of the compiler's internal representation after it has completed parsing and semantic analysis. The `PCH` file can then be used as a prefix header with the `-include-pch` option:

```
$ clang -cc1 -include-pch test.h.pch test.c -o test.s
```

> 说明：上面的 `clang -cc1` 在实际使用中应替换为 `clang++`。例如，要生成预编译头文件 `my_header.pch`，可以使用命令 `clang++ -x c++-header -std=c++11 -o my_header.pch my_header.hpp`


## Design Philosophy

Precompiled headers are meant to improve overall compile times for projects, so the design of precompiled headers is entirely driven by performance concerns. The use case for precompiled headers is relatively simple: when there is a common set of headers that is included in nearly every source file in the project, we **precompile** that bundle of headers into a single precompiled header (`PCH` file). Then, when compiling the source files in the project, we load the `PCH` file first (as a prefix header), which acts as a stand-in for that bundle of headers.

A precompiled header implementation improves performance when:

* Loading the PCH file is significantly faster than re-parsing the bundle of headers stored within the PCH file. Thus, a precompiled header design attempts to minimize the cost of reading the PCH file. Ideally, this cost should not vary with the size of the precompiled header file.

* The cost of generating the PCH file initially is not so large that it counters the per-source-file performance improvement due to eliminating the need to parse the bundled headers in the first place. This is particularly important on multi-core systems, because PCH file generation serializes the build when all compilations require the PCH file to be up-to-date.

> 预编译头文件的实现主要在以下两个方面改善性能：
>
> 1. 加载 PCH 文件的速度明显快于重新解析 PCH 文件中存储的头文件集合。因此，预编译头文件设计试图最小化读取 PCH 文件的成本。理想情况下，这个成本不应随预编译头文件的大小而变化。
> 这意味着，通过使用预编译头文件，编译器可以快速地加载已经解析过的头文件内容，而不需要重新解析这些头文件。这将减少每个源文件的编译时间，从而提高整个项目的编译速度。
>
> 2. 生成 PCH 文件的初始成本不应过大，以免抵消由于消除解析捆绑头文件的需要而带来的每个源文件的性能改进。这在多核系统上尤为重要，因为 PCH 文件生成会在所有编译都需要最新的 PCH 文件时序列化构建。
> 这意味着，尽管生成预编译头文件会带来一定的开销，但这个开销不应过大，以免影响预编译头文件带来的性能提升。在多核系统上，这一点尤为重要，因为生成预编译头文件可能会导致编译过程中的其他任务等待，从而降低并行编译的效果。
>
> 总之，预编译头文件实现通过加快加载 PCH 文件的速度和控制生成 PCH 文件的成本来提高编译性能。这使得编译器能够更快地处理头文件，从而提高整个项目的编译速度。

Clang's precompiled headers are designed with a compact on-disk representation, which minimizes both PCH creation time and the time required to initially load the PCH file. The PCH file itself contains a serialized representation of Clang's abstract syntax trees and supporting data structures, stored using the same compressed bitstream as [LLVM's bitcode file format](https://llvm.org/docs/BitCodeFormat.html).

Clang's precompiled headers are loaded "lazily" from disk. When a PCH file is initially loaded, Clang reads only a small amount of data from the PCH file to establish where certain important data structures are stored. The amount of data read in this initial load is independent of the size of the PCH file, such that a larger PCH file does not lead to longer PCH load times. The actual header data in the PCH file--macros, functions, variables, types, etc.--is loaded only when it is referenced from the user's code, at which point only that entity (and those entities it depends on) are deserialized from the PCH file. With this approach, the cost of using a precompiled header for a translation unit is proportional to the amount of code actually used from the header, rather than being proportional to the size of the header itself.

> Clang 编译器如何以“懒加载”（lazy loading）的方式从磁盘加载预编译头文件（PCH）。懒加载意味着只有在实际需要时才加载数据，这有助于提高性能和降低内存使用。
>
> 当 PCH 文件最初被加载时，Clang 只从 PCH 文件中读取少量数据以确定某些重要数据结构的存储位置。这个初始加载阶段读取的数据量与 PCH 文件的大小无关，因此较大的 PCH 文件不会导致更长的加载时间。
>
> PCH 文件中的实际头文件数据（如宏、函数、变量、类型等）只有在用户代码中引用时才会被加载。此时，只有该实体（以及它所依赖的实体）会从 PCH 文件中被反序列化。通过这种方法，使用预编译头文件的成本与实际从头文件中使用的代码量成正比，而不是与头文件的大小成正比。
>
> 总之，Clang 编译器通过懒加载的方式从磁盘加载预编译头文件，从而提高了性能。这种方法使得使用预编译头文件的成本与实际使用的代码量成正比，而不是与头文件的大小成正比。这有助于在保持编译速度的同时，降低内存使用。

When given the `-print-stats` option, Clang produces statistics describing how much of the precompiled header was actually loaded from disk. For a simple "Hello, World!" program that includes the Apple `Cocoa.h` header (which is built as a precompiled header), this option illustrates how little of the actual precompiled header is required:

```
*** PCH Statistics:
  933 stat cache hits
  4 stat cache misses
  895/39981 source location entries read (2.238563%)
  19/15315 types read (0.124061%)
  20/82685 declarations read (0.024188%)
  154/58070 identifiers read (0.265197%)
  0/7260 selectors read (0.000000%)
  0/30842 statements read (0.000000%)
  4/8400 macros read (0.047619%)
  1/4995 lexical declcontexts read (0.020020%)
  0/4413 visible declcontexts read (0.000000%)
  0/7230 method pool entries read (0.000000%)
  0 method pool misses
```

For this small program, only a tiny fraction of the source locations, types, declarations, identifiers, and macros were actually deserialized from the precompiled header. These statistics can be useful to determine whether the precompiled header implementation can be improved by making more of the implementation lazy.

Precompiled headers can be chained. When you create a PCH while including an existing PCH, Clang can create the new PCH by referencing the original file and only writing the new data to the new file. For example, you could create a PCH out of all the headers that are very commonly used throughout your project, and then create a PCH for every single source file in the project that includes the code that is specific to that file, so that recompiling the file itself is very fast, without duplicating the data from the common headers for every file.

> 预编译头文件（PCH）可以被链接在一起。当你在创建一个新的 PCH 时包含了一个已有的 PCH，Clang 可以通过引用原始文件并只将新数据写入新文件来创建新的 PCH。这种方法允许在不重复公共头文件数据的情况下，更高效地为每个源文件创建 PCH。
>
> 举个例子，你可以为项目中经常使用的所有头文件创建一个 PCH，然后为项目中的每个源文件创建一个 PCH，该 PCH 包含特定于该文件的代码。这样，在重新编译文件本身时，速度会非常快，同时避免了为每个文件重复公共头文件的数据。
>
> 通过链接预编译头文件，可以在保持编译速度的同时，减少生成的 PCH 文件的大小。这种方法在处理大型项目时尤为有用，因为它可以有效地减少编译时间和磁盘空间占用。


## Precompiled Header Contents

Clang's precompiled headers are organized into several different blocks, each of which contains the serialized representation of a part of Clang's internal representation. Each of the blocks corresponds to either a block or a record within [LLVM's bitstream format](http://llvm.org/docs/BitCodeFormat.html). The contents of each of these logical blocks are described below.

For a given precompiled header, the [llvm-bcanalyzer](http://llvm.org/cmds/llvm-bcanalyzer.html) utility can be used to examine the actual structure of the bitstream for the precompiled header. This information can be used both to help understand the structure of the precompiled header and to isolate areas where precompiled headers can still be optimized, e.g., through the introduction of abbreviations.

* Metadata Block
* Source Manager Block
* Preprocessor Block
* Types Block
* Declarations Block
* Statements and Expressions
* Identifier Table Block
* Method Pool Block


## Precompiled Header Integration Points

The "lazy" deserialization behavior of precompiled headers requires their integration into several completely different submodules of Clang. For example, lazily deserializing the declarations during name lookup requires that the name-lookup routines be able to query the precompiled header to find entities within the PCH file.




# 编译加速 (统一编译)

```
bUseUnityBuild
Whether to unify C++ code into larger files for faster compilation.

bForceUnityBuild
Whether to force C++ source files to be combined into larger files for faster compilation.
```

* https://stackoverflow.com/questions/45110783/when-do-i-need-to-include-cpp-files
* https://accu.org/journals/overload/25/138/thomason_2360/
* https://leegoonz.blog/2020/04/26/disable-to-buseunitybuild-even-building-to-ue4-source-code/
* https://docs.unrealengine.com/5.1/en-US/build-configuration-for-unreal-engine/


# 编译优化级别

## gcc

* 编译环境：x86-64 gcc 4.8.5
* 在线编译工具：https://gcc.godbolt.org/
* GCC优化选项说明：[Options That Control Optimization](https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/Optimize-Options.html#Optimize-Options)

gcc默认使用`-O0`优化级别，可以通过`gcc -Q --help=optimizers -O<number>`查看每个优化级别的差异。

```
$gcc -Q --help=optimizers -O0 | head -n20
The following options control optimizations:
  -O<number>
  -Ofast
  -Og
  -Os
  -faggressive-loop-optimizations       [enabled]
  -falign-functions                     [disabled]
  -falign-jumps                         [disabled]
  -falign-labels                        [disabled]
  -falign-loops                         [disabled]
  -fasynchronous-unwind-tables          [enabled]
  -fbranch-count-reg                    [enabled]
  -fbranch-probabilities                [disabled]
  -fbranch-target-load-optimize         [disabled]
  -fbranch-target-load-optimize2        [disabled]
  -fbtr-bb-exclusive                    [disabled]
  -fcaller-saves                        [disabled]
  -fcombine-stack-adjustments           [disabled]
  -fcommon                              [enabled]
  -fcompare-elim                        [disabled]
```


测试代码：

```
#include <cstdio>

int func()
{
        int x = 3;
        return x;
}

int func2()
{
        int y = 4;
        return y;
}

// 1M
//char str[1024 * 1024] = {0, 1};

int main()
{
        int a = 1;
        int b = 2;

        int c = func();

        int d = a + b + c;

        printf("d(%d)\n", d);

        return 0;
}
```

Makefile:

```
# Compare gcc and clang, default is gcc

#CC = /root/compile/llvm_install/bin/clang
#CXX = /root/compile/llvm_install/bin/clang++

CFLAGS = -Werror -Wall -g -pipe

CFLAGS += -O0
#CFLAGS += -O1
#CFLAGS += -O2
#CFLAGS += -O3

# https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/Optimize-Options.html#Optimize-Options
# https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/Link-Options.html#Link-Options
CFLAGS += -fdata-sections -ffunction-sections

INCLUDE = -I./
LIBPATH = -L./
LIBS =

BIN = demo

OBJS = demo.o

MAP_FILE = mapfile

.PHONY: clean

all: $(BIN)

$(BIN): $(OBJS)
        #$(CXX) -o $@ $^ $(LIBPATH) $(LIBS) -Wl,-Map=$(MAP_FILE)
        $(CXX) -o $@ $^ $(LIBPATH) $(LIBS) -Wl,-Map=$(MAP_FILE) -Wl,--gc-sections
        @echo "build $(BIN) ok"

install:
        @echo "nothing to install"

clean:
        rm -f $(OBJS) $(BIN) $(MAP_FILE)


%.o: %.cpp
        $(CXX) $(CFLAGS) $(INCLUDE) -c $<
%.o: %.c
        $(CC) $(CFLAGS) $(INCLUDE) -c $<
```

使用gcc编译，优化选项（默认级别，不做优化）：`-O0`

```
(gdb) b main
Breakpoint 1 at 0x400605: file demo.cpp, line 20.
(gdb) r
Starting program: /root/test/cpp/cpp_strip/demo

Breakpoint 1, main () at demo.cpp:20
20              int a = 1;
Missing separate debuginfos, use: debuginfo-install glibc-2.17-196.tl2.3.x86_64 libgcc-4.8.5-4.el7.x86_64 libstdc++-4.8.5-4.el7.x86_64
(gdb) disassemble /m main
Dump of assembler code for function main():
19      {
   0x00000000004005fd <+0>:     push   %rbp
   0x00000000004005fe <+1>:     mov    %rsp,%rbp
   0x0000000000400601 <+4>:     sub    $0x10,%rsp

20              int a = 1;
=> 0x0000000000400605 <+8>:     movl   $0x1,-0x4(%rbp)

21              int b = 2;
   0x000000000040060c <+15>:    movl   $0x2,-0x8(%rbp)

22
23              int c = func();
   0x0000000000400613 <+22>:    callq  0x4005ed <func()>
   0x0000000000400618 <+27>:    mov    %eax,-0xc(%rbp)

24
25              int d = a + b + c;
   0x000000000040061b <+30>:    mov    -0x8(%rbp),%eax
   0x000000000040061e <+33>:    mov    -0x4(%rbp),%edx
   0x0000000000400621 <+36>:    add    %eax,%edx
   0x0000000000400623 <+38>:    mov    -0xc(%rbp),%eax
   0x0000000000400626 <+41>:    add    %edx,%eax
   0x0000000000400628 <+43>:    mov    %eax,-0x10(%rbp)

26
27              printf("d(%d)\n", d);
   0x000000000040062b <+46>:    mov    -0x10(%rbp),%eax
   0x000000000040062e <+49>:    mov    %eax,%esi
   0x0000000000400630 <+51>:    mov    $0x4006cd,%edi
   0x0000000000400635 <+56>:    mov    $0x0,%eax
   0x000000000040063a <+61>:    callq  0x4004d0 <printf@plt>

28
29              return 0;
   0x000000000040063f <+66>:    mov    $0x0,%eax

30      }
   0x0000000000400644 <+71>:    leaveq
   0x0000000000400645 <+72>:    retq

End of assembler dump.
```

使用clang编译，优化选项（默认级别，不做优化）：`-O0`

```
(gdb) b main
Breakpoint 1 at 0x40115f: file demo.cpp, line 20.
(gdb) r
Starting program: /root/test/cpp/cpp_strip/demo

Breakpoint 1, main () at demo.cpp:20
20              int a = 1;
Missing separate debuginfos, use: debuginfo-install glibc-2.17-196.tl2.3.x86_64 libgcc-4.8.5-4.el7.x86_64 libstdc++-4.8.5-4.el7.x86_64
(gdb) disas /m main
Dump of assembler code for function main:
19      {
   0x0000000000401150 <+0>:     push   %rbp
   0x0000000000401151 <+1>:     mov    %rsp,%rbp
   0x0000000000401154 <+4>:     sub    $0x20,%rsp
   0x0000000000401158 <+8>:     movl   $0x0,-0x4(%rbp)

20              int a = 1;
=> 0x000000000040115f <+15>:    movl   $0x1,-0x8(%rbp)

21              int b = 2;
   0x0000000000401166 <+22>:    movl   $0x2,-0xc(%rbp)

22
23              int c = func();
   0x000000000040116d <+29>:    callq  0x401140 <func()>
   0x0000000000401172 <+34>:    mov    %eax,-0x10(%rbp)

24
25              int d = a + b + c;
   0x0000000000401175 <+37>:    mov    -0x8(%rbp),%eax
   0x0000000000401178 <+40>:    add    -0xc(%rbp),%eax
   0x000000000040117b <+43>:    add    -0x10(%rbp),%eax
   0x000000000040117e <+46>:    mov    %eax,-0x14(%rbp)

26
27              printf("d(%d)\n", d);
   0x0000000000401181 <+49>:    mov    -0x14(%rbp),%esi
   0x0000000000401184 <+52>:    movabs $0x402000,%rdi
   0x000000000040118e <+62>:    mov    $0x0,%al
   0x0000000000401190 <+64>:    callq  0x401030 <printf@plt>
   0x0000000000401195 <+69>:    xor    %eax,%eax

28
29              return 0;
   0x0000000000401197 <+71>:    add    $0x20,%rsp
   0x000000000040119b <+75>:    pop    %rbp
   0x000000000040119c <+76>:    retq

End of assembler dump.
```


也可通过[在线编译工具](https://gcc.godbolt.org/)反汇编。

![gcc_compile](/assets/images/202101/gcc_compile.png)

## clang

* [Clang optimization levels](https://stackoverflow.com/questions/15548023/clang-optimization-levels)
* [CLang optimizations on Mac OSX](https://gist.github.com/lolo32/fd8ce29b218ac2d93a9e)
* [Compiling With Clang Optimization Flags](https://www.incredibuild.com/blog/compiling-with-clang-optimization-flags)
* [In clang, how do you use per-function optimization attributes?](https://stackoverflow.com/questions/26266820/in-clang-how-do-you-use-per-function-optimization-attributes)
* [How to change optimization level of one function?](https://stackoverflow.com/questions/31373885/how-to-change-optimization-level-of-one-function)
* https://clang.llvm.org/docs/LanguageExtensions.html#extensions-for-selectively-disabling-optimization
* [clang ignoring attribute noinline](https://stackoverflow.com/questions/54481855/clang-ignoring-attribute-noinline)
* https://clang.llvm.org/docs/AttributeReference.html
* [Does LLVM/clang have flags to control code padding?](https://stackoverflow.com/questions/27712716/does-llvm-clang-have-flags-to-control-code-padding)


To sum it up, to find out about compiler optimization passes:

`llvm-as < /dev/null | opt -O3 -disable-output -debug-pass=Arguments`

clang additionally runs some higher level optimizations, which we can retrieve with:

`echo 'int;' | clang -xc -O3 - -o /dev/null -\#\#\#`

Documentation of individual passes is available [here](http://llvm.org/docs/Passes.html).

You can compare the effect of changing high-level flags such as -O like this:

``` bash
diff -wy --suppress-common-lines  \
  <(echo 'int;' | clang -xc -Os   - -o /dev/null -\#\#\# 2>&1 | tr " " "\n" | grep -v /tmp) \
  <(echo 'int;' | clang -xc -O2 - -o /dev/null -\#\#\# 2>&1 | tr " " "\n" | grep -v /tmp)
# will tell you that -O0 is indeed the default.
```

-O0 优化选项：

``` bash
$echo 'int;' | clang -xc -O0 - -o /dev/null -\#\#\#
clang version 3.5.2 (tags/RELEASE_352/final)
Target: x86_64-unknown-linux-gnu
Thread model: posix
 "/usr/local/bin/clang-3.5" "-cc1" "-triple" "x86_64-unknown-linux-gnu" "-emit-obj" "-mrelax-all" "-disable-free" "-disable-llvm-verifier" "-main-file-name" "-" "-mrelocation-model" "static" "-mdisable-fp-elim" "-fmath-errno" "-masm-verbose" "-mconstructor-aliases" "-munwind-tables" "-fuse-init-array" "-target-cpu" "x86-64" "-dwarf-column-info" "-resource-dir" "/usr/local/bin/../lib/clang/3.5.2" "-internal-isystem" "/usr/local/include" "-internal-isystem" "/usr/local/bin/../lib/clang/3.5.2/include" "-internal-externc-isystem" "/include" "-internal-externc-isystem" "/usr/include" "-O0" "-fdebug-compilation-dir" "/data/home/gerryyang/pracing/build/release/src/gamesvr/CMakeFiles/gamesvr.dir" "-ferror-limit" "19" "-fmessage-length" "198" "-mstackrealign" "-fobjc-runtime=gcc" "-fdiagnostics-show-option" "-fcolor-diagnostics" "-o" "/tmp/--e22b2f.o" "-x" "c" "-"
 "/bin/ld" "--eh-frame-hdr" "-m" "elf_x86_64" "-dynamic-linker" "/lib64/ld-linux-x86-64.so.2" "-o" "/dev/null" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64/crt1.o" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64/crti.o" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/crtbegin.o" "-L/usr/lib/gcc/x86_64-redhat-linux/4.8.5" "-L/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64" "-L/usr/local/bin/../lib64" "-L/lib/../lib64" "-L/usr/lib/../lib64" "-L/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../.." "-L/usr/local/bin/../lib" "-L/lib" "-L/usr/lib" "/tmp/--e22b2f.o" "-lgcc" "--as-needed" "-lgcc_s" "--no-as-needed" "-lc" "-lgcc" "--as-needed" "-lgcc_s" "--no-as-needed" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/crtend.o" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64/crtn.o"
```

-O2 优化选项：

``` bash
$echo 'int;' | clang -xc -O2 - -o /dev/null -\#\#\#
clang version 3.5.2 (tags/RELEASE_352/final)
Target: x86_64-unknown-linux-gnu
Thread model: posix
 "/usr/local/bin/clang-3.5" "-cc1" "-triple" "x86_64-unknown-linux-gnu" "-emit-obj" "-disable-free" "-disable-llvm-verifier" "-main-file-name" "-" "-mrelocation-model" "static" "-fmath-errno" "-masm-verbose" "-mconstructor-aliases" "-munwind-tables" "-fuse-init-array" "-target-cpu" "x86-64" "-momit-leaf-frame-pointer" "-dwarf-column-info" "-resource-dir" "/usr/local/bin/../lib/clang/3.5.2" "-internal-isystem" "/usr/local/include" "-internal-isystem" "/usr/local/bin/../lib/clang/3.5.2/include" "-internal-externc-isystem" "/include" "-internal-externc-isystem" "/usr/include" "-O2" "-fdebug-compilation-dir" "/data/home/gerryyang/pracing/build/release/src/gamesvr/CMakeFiles/gamesvr.dir" "-ferror-limit" "19" "-fmessage-length" "198" "-mstackrealign" "-fobjc-runtime=gcc" "-fdiagnostics-show-option" "-fcolor-diagnostics" "-vectorize-loops" "-vectorize-slp" "-o" "/tmp/--ebb79a.o" "-x" "c" "-"
 "/bin/ld" "--eh-frame-hdr" "-m" "elf_x86_64" "-dynamic-linker" "/lib64/ld-linux-x86-64.so.2" "-o" "/dev/null" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64/crt1.o" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64/crti.o" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/crtbegin.o" "-L/usr/lib/gcc/x86_64-redhat-linux/4.8.5" "-L/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64" "-L/usr/local/bin/../lib64" "-L/lib/../lib64" "-L/usr/lib/../lib64" "-L/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../.." "-L/usr/local/bin/../lib" "-L/lib" "-L/usr/lib" "/tmp/--ebb79a.o" "-lgcc" "--as-needed" "-lgcc_s" "--no-as-needed" "-lc" "-lgcc" "--as-needed" "-lgcc_s" "--no-as-needed" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/crtend.o" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64/crtn.o"
```

-O3 优化选项：

``` bash
$echo 'int;' | clang -xc -O3 - -o /dev/null -\#\#\#
clang version 3.5.2 (tags/RELEASE_352/final)
Target: x86_64-unknown-linux-gnu
Thread model: posix
 "/usr/local/bin/clang-3.5" "-cc1" "-triple" "x86_64-unknown-linux-gnu" "-emit-obj" "-disable-free" "-disable-llvm-verifier" "-main-file-name" "-" "-mrelocation-model" "static" "-fmath-errno" "-masm-verbose" "-mconstructor-aliases" "-munwind-tables" "-fuse-init-array" "-target-cpu" "x86-64" "-momit-leaf-frame-pointer" "-dwarf-column-info" "-resource-dir" "/usr/local/bin/../lib/clang/3.5.2" "-internal-isystem" "/usr/local/include" "-internal-isystem" "/usr/local/bin/../lib/clang/3.5.2/include" "-internal-externc-isystem" "/include" "-internal-externc-isystem" "/usr/include" "-O3" "-fdebug-compilation-dir" "/data/home/gerryyang/pracing/build/release/src/gamesvr/CMakeFiles/gamesvr.dir" "-ferror-limit" "19" "-fmessage-length" "198" "-mstackrealign" "-fobjc-runtime=gcc" "-fdiagnostics-show-option" "-fcolor-diagnostics" "-vectorize-loops" "-vectorize-slp" "-o" "/tmp/--1ea6a8.o" "-x" "c" "-"
 "/bin/ld" "--eh-frame-hdr" "-m" "elf_x86_64" "-dynamic-linker" "/lib64/ld-linux-x86-64.so.2" "-o" "/dev/null" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64/crt1.o" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64/crti.o" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/crtbegin.o" "-L/usr/lib/gcc/x86_64-redhat-linux/4.8.5" "-L/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64" "-L/usr/local/bin/../lib64" "-L/lib/../lib64" "-L/usr/lib/../lib64" "-L/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../.." "-L/usr/local/bin/../lib" "-L/lib" "-L/usr/lib" "/tmp/--1ea6a8.o" "-lgcc" "--as-needed" "-lgcc_s" "--no-as-needed" "-lc" "-lgcc" "--as-needed" "-lgcc_s" "--no-as-needed" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/crtend.o" "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../lib64/crtn.o"
```


```
diff -wy --suppress-common-lines    <(echo 'int;' | clang -xc -O0   - -o /dev/null -\#\#\# 2>&1 | tr " " "\n" | grep -v /tmp)   <(echo 'int;' | clang -xc -O2 - -o /dev/null -\#\#\# 2>&1 | tr " " "\n" | grep -v /tmp)
"-mrelax-all"                                                 <
"-mdisable-fp-elim"                                           <
                                                              > "-momit-leaf-frame-pointer"
"-O0"                                                         | "-O2"
                                                              > "-vectorize-loops"
                                                              > "-vectorize-slp"
```

```
diff -wy --suppress-common-lines  \
>   <(echo 'int;' | clang -xc -O2   - -o /dev/null -\#\#\# 2>&1 | tr " " "\n" | grep -v /tmp) \
>   <(echo 'int;' | clang -xc -O3 - -o /dev/null -\#\#\# 2>&1 | tr " " "\n" | grep -v /tmp)
"-O2"                                                         | "-O3"
```

With `version 3.5` the passes are as follow (parsed output of the command above):

* default (-O0): -targetlibinfo -verify -verify-di
* -O1 is based on -O0
   + adds: -correlated-propagation -basiccg -simplifycfg -no-aa -jump-threading -sroa -loop-unswitch -ipsccp -instcombine -memdep -memcpyopt -barrier -block-freq -loop-simplify -loop-vectorize -inline-cost -branch-prob -early-cse -lazy-value-info -loop-rotate -strip-dead-prototypes -loop-deletion -tbaa -prune-eh -indvars -loop-unroll -reassociate -loops -sccp -always-inline -basicaa -dse -globalopt -tailcallelim -functionattrs -deadargelim -notti -scalar-evolution -lower-expect -licm -loop-idiom -adce -domtree -lcssa
* -O2 is based on -01
   + adds: -gvn -constmerge -globaldce -slp-vectorizer -mldst-motion -inline
   + removes: -always-inline
* -O3 is based on -O2
   + adds: -argpromotion
* -Os is identical to -O2
* -Oz is based on -Os
   + removes: -slp-vectorizer


[Extensions for selectively disabling optimization](https://clang.llvm.org/docs/LanguageExtensions.html#extensions-for-selectively-disabling-optimization)

Clang provides a mechanism for selectively disabling optimizations in functions and methods.

To disable optimizations in a single function definition, the GNU-style or C++11 non-standard attribute `optnone` can be used.

``` cpp
// The following functions will not be optimized.
// GNU-style attribute
__attribute__((optnone)) int foo() {
  // ... code
}

// C++11 attribute
[[clang::optnone]] int bar() {
  // ... code
}
```

To facilitate disabling optimization for a range of function definitions, a range-based pragma is provided. Its syntax is `#pragma clang optimize` followed by `off` or `on`.

All function definitions in the region between an `off` and the following `on` will be decorated with the `optnone` attribute unless doing so would conflict with explicit attributes already present on the function (e.g. the ones that control inlining).

``` cpp
#pragma clang optimize off
// This function will be decorated with optnone.
int foo() {
  // ... code
}

// optnone conflicts with always_inline, so bar() will not be decorated.
__attribute__((always_inline)) int bar() {
  // ... code
}
#pragma clang optimize on
```

If no `on` is found to close an `off` region, the end of the region is the end of the compilation unit.

Note that a stray `#pragma clang optimize on` does not selectively enable additional optimizations when compiling at low optimization levels. This feature can only be used to selectively disable optimizations.

[clang ignoring attribute noinline](https://stackoverflow.com/questions/54481855/clang-ignoring-attribute-noinline)

I expected `__attribute__((noinline))`, when added to a function, to make sure that that function gets emitted. This works with gcc, but clang still seems to inline it.

Here is an example, which you can also [open on Godbolt](https://godbolt.org/z/QMTL8f):

``` cpp
namespace {

__attribute__((noinline))
int inner_noinline() {
    return 3;
}

int inner_inline() {
    return 4;
}

int outer() {
    return inner_noinline() + inner_inline();
}

}

int main() {
    return outer();
}
```

When build with `-O3`, `gcc` emits `inner_noinline`, but not `inner_inline`:

``` cpp
(anonymous namespace)::inner_noinline():
        mov     eax, 3
        ret
main:
        call    (anonymous namespace)::inner_noinline()
        add     eax, 4
        ret
```

Clang insists on inlining it:

``` cpp
main: # @main
  mov eax, 7
  ret
```

If adding a parameter to the functions and letting them perform some trivial work, clang respects the noinline attribute: https://godbolt.org/z/NNSVab

Shouldn't noinline be independent of how complex the function is? What am I missing?

Answers:

Does clang support noinline attribute?

It doesn't have its own category in the [list of attributes](https://clang.llvm.org/docs/AttributeReference.html), but if you search for noinline there, you will find it mentioned several times.

Also, looking at the version with parameters, if I remove it there, both functions are inlined. So clang seems to at least know it.

related: [noinline attribute is not respected in -O1 and above #3409](github.com/emscripten-core/emscripten/issues/3409)

`__attribute__((noinline))` prevents the compiler from inlining the function. It doesn't prevent it from doing constant folding. In this case, the compiler was able to recognize that there was no need to call `inner_noinline`, either as an inline insertion or an out-of-line call. It could just replace the function call with the constant `3`.

It sounds like you want to use the `optnone` attribute instead, to prevent the compiler from applying even the most obvious of optimizations (as this one is).



# 去除Dead Codes (删除未使用的函数)

参考[Compilation options](https://gcc.gnu.org/onlinedocs/gnat_ugn/Compilation-options.html)通过下面两步，去除代码没有使用的函数：

1. 添加编译选项`CFLAGS += -fdata-sections -ffunction-sections`
2. 添加链接选项`-Wl,--gc-sections`

通过上面两步，会将函数代码生成为独立的section，并在链接的时候去除不用的Dead Codes。

> 注意：此选项对gcc和clang都生效。

```
$readelf -t demo.o
There are 26 section headers, starting at offset 0x10b0:

Section Headers:
  [Nr] Name
       Type              Address          Offset            Link
       Size              EntSize          Info              Align
       Flags
  [ 0]
       NULL                   NULL             0000000000000000  0000000000000000  0
       0000000000000000 0000000000000000  0                 0
       [0000000000000000]:
  [ 1] .text
       PROGBITS               PROGBITS         0000000000000000  0000000000000040  0
       0000000000000000 0000000000000000  0                 4
       [0000000000000006]: ALLOC, EXEC
  [ 2] .data
       PROGBITS               PROGBITS         0000000000000000  0000000000000040  0
       0000000000000000 0000000000000000  0                 4
       [0000000000000003]: WRITE, ALLOC
  [ 3] .bss
       NOBITS                 NOBITS           0000000000000000  0000000000000040  0
       0000000000000000 0000000000000000  0                 4
       [0000000000000003]: WRITE, ALLOC
  [ 4] .text._Z4funcv
       PROGBITS               PROGBITS         0000000000000000  0000000000000040  0
       0000000000000010 0000000000000000  0                 1
       [0000000000000006]: ALLOC, EXEC
  [ 5] .text._Z5func2v
       PROGBITS               PROGBITS         0000000000000000  0000000000000050  0
       0000000000000010 0000000000000000  0                 1
       [0000000000000006]: ALLOC, EXEC
  [ 6] .rodata
       PROGBITS               PROGBITS         0000000000000000  0000000000000060  0
       0000000000000007 0000000000000000  0                 1
       [0000000000000002]: ALLOC
  [ 7] .text.main
       PROGBITS               PROGBITS         0000000000000000  0000000000000067  0
       0000000000000049 0000000000000000  0                 1
       [0000000000000006]: ALLOC, EXEC
  [ 8] .rela.text.main
       RELA                   RELA             0000000000000000  0000000000001970  24
       0000000000000048 0000000000000018  7                 8
       [0000000000000000]:
  [ 9] .debug_info
       PROGBITS               PROGBITS         0000000000000000  00000000000000b0  0
       000000000000076f 0000000000000000  0                 1
       [0000000000000000]:

...
```

并且最终的链接代码中，不会存在未使用函数的代码：

```
$nm -C demo | grep func
00000000004005f0 T func()
```

查看mapfile，可以看到func2被discard了：

```
Discarded input sections
...
 .text          0x0000000000000000        0x0 demo.o
 .data          0x0000000000000000        0x0 demo.o
 .bss           0x0000000000000000        0x0 demo.o
 .text._Z5func2v
                0x0000000000000000       0x10 demo.o
...
```

``` bash
$objdump -s -j .text._Z5func2v demo.o

demo.o:     file format elf64-x86-64

Contents of section .text._Z5func2v:
 0000 554889e5 c745fc04 0000008b 45fc5dc3  UH...E......E.].
```

refer:

* https://stackoverflow.com/questions/6687630/how-to-remove-unused-c-c-symbols-with-gcc-and-ld
* https://stackoverflow.com/questions/54996229/is-ffunction-sections-fdata-sections-and-gc-sections-not-working
* https://stackoverflow.com/questions/17710024/clang-removing-dead-code-during-static-linking-gcc-equivalent-of-wl-gc-sect



# strip

* `strip`用于删除目标文件中的符号（Discard symbols from object files），通常用于删除已生成的可执行文件和库中不需要的符号。
* 在想要减少文件的大小，并保留对调试有用的信息时，使用`-d`选项，可以删除不使用的信息（文件名和行号等），并可以保留函数名等一般的符号，用gdb进行调试时，只要保留了函数名，即便不知道文件名和行号，也可以进行调试。
* 使用`-R`选项，是可删除其他任意信息的选项，在执行`strip -R .text demo1`后，程序的text部分（代码部分）会被完全删除，从而导致程序的无法运行。
* 实际上，对`.o`文件以及`.a`文件使用strip后，就不能进行和其他目标文件的链接操作。这是由于文件对链接器符号有依赖性，所以最好不要从`.o`和`.a`文件中删除符号。
* 对release的版本strip，当用户环境产生coredump后，可以通过包含调试信息的开发版本在开发环境进行调试。
* 虽然在磁盘容量足够大的PC中，可能不会出现想要将可执行文件变小的情况。但在容量有限的环境，或想要通过网络复制并运行程序时，`strip`却是一个方便的工具。


``` bash
# 去除目标文件中的符号
$ strip objfile
$ nm objfile
nm: objfile: no symbols

# 删除代码段
$ strip -R .text demo1
$ ./demo1
Segmentation fault (core dumped)
```


# objcopy

* `objcopy` - copy and translate object file
* 实际上，在`objcopy`上使用`-strip-*`选项后也能进行与`strip`同样的处理。

例如：

``` bash
# 拷贝出一个符号表文件
$ objcopy --only-keep-debug mainO3 mainO3.symbol       

# 拷贝出一个不包含调试信息的执行文件
$ objcopy --strip-debug mainO3 mainO3.bin

$ objcopy --add-gnu-debuglink=mainO3.symbol mainO3
```


* 用`objcopy`嵌入可执行文件的数据，`objcopy`可以将任意文件转换为可以链接的目标文件。

例如：可以将`foo.jpg`转换为x86用的ELF32形式的目标文件`foo.o`

``` bash
$ objcopy -I binary -O elf32-i386 -B i386 foo.jpg foo.o
```

脚本工具：

``` bash
#!/bin/bash

scriptdir=`dirname ${0}`
scriptdir=`(cd ${scriptdir}; pwd)`
scriptname=`basename ${0}`

set -e

function errorexit()
{
  errorcode=${1}
  shift
  echo $@
  exit ${errorcode}
}

function usage()
{
  echo "USAGE ${scriptname} <tostrip>"
}

tostripdir=`dirname "$1"`
tostripfile=`basename "$1"`


if [ -z ${tostripfile} ] ; then
  usage
  errorexit 0 "tostrip must be specified"
fi

cd "${tostripdir}"

debugdir=.debug
debugfile="${tostripfile}.debug"

if [ ! -d "${debugdir}" ] ; then
  echo "creating dir ${tostripdir}/${debugdir}"
  mkdir -p "${debugdir}"
fi
echo "stripping ${tostripfile}, putting debug info into ${debugfile}"
objcopy --only-keep-debug "${tostripfile}" "${debugdir}/${debugfile}"
strip --strip-debug --strip-unneeded "${tostripfile}"
objcopy --add-gnu-debuglink="${debugdir}/${debugfile}" "${tostripfile}"
chmod -x "${debugdir}/${debugfile}"
```

refer: [How to generate gcc debug symbol outside the build target?](https://stackoverflow.com/questions/866721/how-to-generate-gcc-debug-symbol-outside-the-build-target)

# -fno-rtti / -frtti


* https://desk.zoho.com.cn/portal/sylixos/zh/kb/articles/c-%E7%BC%96%E8%AF%91%E9%80%89%E9%A1%B9-fno-rtti-%E5%92%8C-frtti%E6%B5%85%E6%9E%90
* https://stackoverflow.com/questions/23912955/disable-rtti-for-some-classes
* https://stackoverflow.com/questions/36261573/gcc-c-override-frtti-for-single-class

# -Wl,--start-group / -Wl,--end-group

[What are the --start-group and --end-group command line options?](https://stackoverflow.com/questions/5651869/what-are-the-start-group-and-end-group-command-line-options)

What is the purpose of those command line options? Please help to decipher the meaning of the following command line:

```
-Wl,--start-group -lmy_lib -lyour_lib -lhis_lib -Wl,--end-group -ltheir_lib
```

Apparently it has something to do with linking, but the GNU manual is quiet what exactly grouping means.

Answers:

It is for resolving circular dependences between several libraries (listed between `-(` and `-)`).

Citing [Why does the order in which libraries are linked sometimes cause errors in GCC?](https://stackoverflow.com/questions/45135/linker-order-gcc/409470#409470) or man ld http://linux.die.net/man/1/ld

> `-(` archives `-)` or `--start-group` archives `--end-group`
>
> The archives should be a list of archive files. They may be either explicit file names, or -l options.
>
> The specified archives are searched repeatedly until no new undefined references are created. Normally, an archive is searched only once in the order that it is specified on the command line. If a symbol in that archive is needed to resolve an undefined symbol referred to by an object in an archive that appears later on the command line, the linker would not be able to resolve that reference. By grouping the archives, they all be searched repeatedly until all possible references are resolved.
>
> Using this option has a significant performance cost. It is best to use it only when there are unavoidable circular references between two or more archives.

So, libraries inside the group can be searched for new symbols several time, and you need no ugly constructs like `-llib1 -llib2 -llib1`

PS archive means basically a static library (`*.a` files)

# [Options for Debugging Your Program - GCC](https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html)

To tell `GCC` to emit extra information for use by a debugger, in almost all cases you need only to add `-g` to your other options. Some debug formats can co-exist (like DWARF with CTF) when each of them is enabled explicitly by adding the respective command line option to your other options.

GCC allows you to use `-g` with `-O`. **The shortcuts taken by optimized code may occasionally be surprising**: some variables you declared may not exist at all; flow of control may briefly move where you did not expect it; some statements may not be executed because they compute constant results or their values are already at hand; some statements may execute in different places because they have been moved out of loops. Nevertheless it is possible to debug optimized output. This makes it reasonable to use the optimizer for programs that might have bugs.

If you are not using some other optimization option, consider using `-Og` (see [Options That Control Optimization](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)) with `-g`. With no `-O` option at all, some compiler passes that collect information useful for debugging do not run at all, so that `-Og` may result in a better debugging experience.


## -g

Produce debugging information in the operating system’s native format (stabs, COFF, XCOFF, or DWARF). GDB can work with this debugging information.

On most systems that use stabs format, `-g` enables use of extra debugging information that only GDB can use; this extra information makes debugging work better in GDB but probably makes other debuggers crash or refuse to read the program. If you want to control for certain whether to generate the extra information, use `-gvms` (see below).

## -ggdb

Produce debugging information for use by GDB. This means to use the most expressive format available (DWARF, stabs, or the native format if neither of those are supported), including GDB extensions if at all possible.

## -gdwarf / -gdwarf-version

**Produce debugging information in DWARF format** (if that is supported). The value of version may be either 2, 3, 4 or 5; the default version for most targets is 5 (with the exception of VxWorks, TPF and Darwin/Mac OS X, which default to version 2, and AIX, which defaults to version 4).

Note that with DWARF Version 2, some ports require and always use some non-conflicting DWARF 3 extensions in the unwind tables.

Version 4 may require GDB 7.0 and -fvar-tracking-assignments for maximum benefit. **Version 5 requires GDB 8.0 or higher**.








# 编译二进制大小优化



## [LLVM Link Time Optimization: Design and Implementation](https://llvm.org/docs/LinkTimeOptimization.html) - LTO

> 使用 LTO 通过牺牲更多的编译时间，通过跨模块的上下文信息，实现编译优化。

由于编译器一次只编译优化一个编译单元，所以只是在做局部优化，而利用 LTO，利用链接时的全局视角进行操作，从而得到能够进行更加极致的优化。

跨模块优化的效果，也即开启 LTO 主要有这几点好处：

1. 将一些函数內联化
2. 去除了一些无用代码
3. 对程序有全局的优化作用

比较体验不好的是，LTO 会导致编译和链接变慢，以及会使用更多的内存，所以即使到现在，也没有看到 LTO 被广泛地使用。

* [代码优化利器 LTO 介绍](https://zhuanlan.zhihu.com/p/384160632)
* [开启Link Time Optimization(LTO)后到底有什么优化？](https://www.jianshu.com/p/58fef052291a)
* [Link Time Optimizations: New Way to Do Compiler Optimizations](https://johnysswlab.com/link-time-optimizations-new-way-to-do-compiler-optimizations/)
* [ThinLTO: Scalable and Incremental LTO](http://blog.llvm.org/2016/06/thinlto-scalable-and-incremental-lto.html)
* [LTO with LLVM and CMake](https://stackoverflow.com/questions/35922966/lto-with-llvm-and-cmake)
* [Clang: How to check if LTO was performed](https://stackoverflow.com/questions/51048414/clang-how-to-check-if-lto-was-performed)


### Description

LLVM features powerful intermodular optimizations which can be used at link time. **Link Time Optimization (LTO)** is another name for **intermodular optimization** when performed during the link stage. This document describes the interface and design between the LTO optimizer and the linker.

### Design Philosophy

**The LLVM Link Time Optimizer provides complete transparency, while doing intermodular optimization, in the compiler tool chain. Its main goal is to let the developer take advantage of intermodular optimizations without making any significant changes to the developer’s makefiles or build system**. This is achieved through tight integration with the linker. In this model, the linker treats LLVM bitcode files like native object files and allows mixing and matching among them. The linker uses [libLTO](https://llvm.org/docs/LinkTimeOptimization.html#liblto), a shared object, to handle LLVM bitcode files. This tight integration between the linker and LLVM optimizer helps to do optimizations that are not possible in other models. The linker input allows the optimizer to avoid relying on conservative escape analysis.

### Example of link time optimization

The following example illustrates the advantages of LTO’s integrated approach and clean interface. This example requires a system linker which supports LTO through the interface described in this document. Here, clang transparently invokes system linker.

* Input source file `a.c` is compiled into LLVM bitcode form.
* Input source file `main.c` is compiled into native object code.

``` c
--- a.h ---
extern int foo1(void);
extern void foo2(void);
extern void foo4(void);

--- a.c ---
#include "a.h"

static signed int i = 0;

void foo2(void) {
  i = -1;
}

static int foo3() {
  foo4();
  return 10;
}

int foo1(void) {
  int data = 0;

  if (i < 0)
    data = foo3();

  data = data + 42;
  return data;
}

--- main.c ---
#include <stdio.h>
#include "a.h"

void foo4(void) {
  printf("Hi\n");
}

int main() {
  return foo1();
}
```

To compile, run:

```
% clang -flto -c a.c -o a.o        # <-- a.o is LLVM bitcode file
% clang -c main.c -o main.o        # <-- main.o is native object file
% clang -flto a.o main.o -o main   # <-- standard link command with -flto
```

* In this example, the linker recognizes that `foo2()` is an externally visible symbol defined in LLVM bitcode file. The linker completes its usual symbol resolution pass and finds that `foo2()` is not used anywhere. This information is used by the LLVM optimizer and it removes `foo2()`.
* As soon as `foo2()` is removed, the optimizer recognizes that condition `i < 0` is always `false`, which means `foo3()` is never used. Hence, the optimizer also removes `foo3()`.
* And this in turn, enables linker to remove `foo4()`.

This example illustrates the advantage of tight integration with the linker. Here, the optimizer can not remove `foo3()` without the linker’s input.

### Alternative Approaches

* Compiler driver invokes link time optimizer separately.

In this model the link time optimizer is not able to take advantage of information collected during the linker’s normal symbol resolution phase. In the above example, the optimizer can not remove `foo2()` without the linker’s input because it is externally visible. This in turn prohibits the optimizer from removing `foo3()`.

* Use separate tool to collect symbol information from all object files.

In this model, a new, separate, tool or library replicates the linker’s capability to collect information for link time optimization. Not only is this code duplication difficult to justify, but it also has several other disadvantages. For example, the linking semantics and the features provided by the linker on various platform are not unique. This means, this new tool needs to support all such features and platforms in one super tool or a separate tool per platform is required. This increases maintenance cost for link time optimizer significantly, which is not necessary. This approach also requires staying synchronized with linker developments on various platforms, which is not the main focus of the link time optimizer. Finally, this approach increases end user’s build time due to the duplication of work done by this separate tool and the linker itself.

### Multi-phase communication between libLTO and linker

The linker collects information about symbol definitions and uses in various link objects which is more accurate than any information collected by other tools during typical build cycles. The linker collects this information by looking at the definitions and uses of symbols in native .o files and using symbol visibility information. The linker also uses user-supplied information, such as a list of exported symbols. LLVM optimizer collects control flow information, data flow information and knows much more about program structure from the optimizer’s point of view. Our goal is to take advantage of tight integration between the linker and the optimizer by sharing this information during various linking phases.


### 问题

* [-Wl,--wrap not supported with LTO](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88643)


## [-fdebug-types-section](https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html)

When using DWARF Version 4 or higher, type DIEs can be put into their own `.debug_types` section instead of making them part of the `.debug_info` section. It is more efficient to put them in a separate comdat section since the linker can then remove duplicates. But not all DWARF consumers support `.debug_types` sections yet and on some objects `.debug_types` produces larger instead of smaller debugging information.

`-fdebug-types-section` 选项用于在生成 DWARF 调试信息时将类型定义（type DIEs）放入单独的 .debug_types 节中，而不是将它们作为 .debug_info 节的一部分。这个选项适用于 DWARF 版本4及更高版本。

将类型定义放入单独的 .debug_types 节有以下优势：

链接器效率：链接器可以通过合并重复的类型定义来减小生成的调试信息的大小。将类型定义放入单独的 .debug_types 节（通常是comdat节）可以让链接器更容易地识别和删除重复的类型定义。

然而，使用 -fdebug-types-section 选项也存在一些限制和问题：

DWARF 消费者的兼容性：并非所有处理 DWARF 调试信息的工具（如调试器和分析器）都支持 .debug_types 节。在这种情况下，使用 -fdebug-types-section 可能会导致兼容性问题。

调试信息大小：在某些情况下，使用 .debug_types 节可能会导致生成的调试信息更大，而不是更小。这取决于具体的对象文件和类型定义。

总之，-fdebug-types-section 选项用于将类型定义放入单独的 .debug_types 节，以提高链接器效率。然而，在使用此选项时，请注意兼容性和调试信息大小的潜在问题。在选择是否使用此选项时，请根据您的项目需求和目标平台进行权衡。

## [--compress-debug-sections=zlib](https://sourceware.org/binutils/docs/ld/Options.html#index-compress-debug-sections-1)

```
--compress-debug-sections=none
--compress-debug-sections=zlib
--compress-debug-sections=zlib-gnu
--compress-debug-sections=zlib-gabi
--compress-debug-sections=zstd
```

On ELF platforms, these options control how DWARF debug sections are compressed using zlib.


使用 -Wl,--compress-debug-sections=zlib 可以压缩调试信息，从而减小生成的二进制文件大小。然而，在使用此选项时，有一些注意事项和可能的问题：

调试器兼容性：并非所有调试器都支持压缩后的调试信息。在使用压缩调试信息的二进制文件进行调试时，请确保您的调试器（如GDB）支持处理压缩后的调试节。较新版本的GDB通常支持这一点。

解压缩开销：虽然压缩调试信息可以减小文件大小，但在调试过程中，调试器需要解压缩这些信息。这可能会导致调试过程稍微变慢。对于大型项目，解压缩时间可能会有所增加。

链接器支持：并非所有链接器都支持 --compress-debug-sections 选项。在使用此选项时，请确保您的链接器支持它。通常，较新版本的GNU ld链接器支持此功能。

二进制文件可移植性：如果您需要将二进制文件分发给其他用户，他们可能使用不同的调试器或操作系统。在这种情况下，使用压缩调试信息可能会导致兼容性问题。在将二进制文件分发给其他用户之前，请确保他们的环境支持处理压缩后的调试信息。

总之，在使用 -Wl,--compress-debug-sections=zlib 选项时，请确保您的工具链和调试器支持处理压缩后的调试信息。同时，请注意，在某些情况下，这可能会影响调试过程的性能。


# 优化调试

## [time-trace: timeline / flame chart profiler for Clang](https://aras-p.info/blog/2019/01/16/time-trace-timeline-flame-chart-profiler-for-Clang/)