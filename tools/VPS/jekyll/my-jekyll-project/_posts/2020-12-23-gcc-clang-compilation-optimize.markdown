---
layout: post
title:  "GCC/Clang Compilation Optimize"
date:   2021-02-13 14:00:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}


# 编译优化 (gcc/clang)

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


# 去除Dead Codes (删除未使用的函数)

参考[Compilation options](https://gcc.gnu.org/onlinedocs/gnat_ugn/Compilation-options.html)通过下面两步，去除编译种没有使用的函数：

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

```
$objdump -s -j .text._Z5func2v demo.o

demo.o:     file format elf64-x86-64

Contents of section .text._Z5func2v:
 0000 554889e5 c745fc04 0000008b 45fc5dc3  UH...E......E.].
```

refer:

* https://stackoverflow.com/questions/6687630/how-to-remove-unused-c-c-symbols-with-gcc-and-ld
* https://stackoverflow.com/questions/54996229/is-ffunction-sections-fdata-sections-and-gc-sections-not-working
* https://stackoverflow.com/questions/17710024/clang-removing-dead-code-during-static-linking-gcc-equivalent-of-wl-gc-sect



# strip & objcopy (对可执行文件瘦身)

```
strip - Discard symbols from object files
```

```
objcopy - copy and translate object file

-g
--strip-debug
Do not copy debugging symbols or sections from the source file.

--only-keep-debug
Strip a file, removing contents of any sections that would not be stripped by --strip-debug and leaving the debugging sections intact. In ELF files, this preserves all note sections in the output.

Note - the section headers of the stripped sections are preserved, including their sizes, but the contents of the section are discarded. The section headers are preserved so that other tools can match up the debuginfo file with the real executable, even if that executable has been relocated to a different address space.

The intention is that this option will be used in conjunction with --add-gnu-debuglink to create a two part executable. One a stripped binary which will occupy less space in RAM and in a distribution and the second a debugging information file which is only needed if debugging abilities are required. The suggested procedure to create these files is as follows:

Link the executable as normal. Assuming that it is called foo then...
Run objcopy --only-keep-debug foo foo.dbg to create a file containing the debugging info.
Run objcopy --strip-debug foo to create a stripped executable.
Run objcopy --add-gnu-debuglink=foo.dbg foo to add a link to the debugging info into the stripped executable.
Note—the choice of .dbg as an extension for the debug info file is arbitrary. Also the --only-keep-debug step is optional. You could instead do this:

Link the executable as normal.
Copy foo to foo.full
Run objcopy --strip-debug foo
Run objcopy --add-gnu-debuglink=foo.full foo
i.e., the file pointed to by the --add-gnu-debuglink can be the full executable. It does not have to be a file created by the --only-keep-debug switch.

Note—this switch is only intended for use on fully linked files. It does not make sense to use it on object files where the debugging information may be incomplete. Besides the gnu_debuglink feature currently only supports the presence of one filename containing debugging information, not multiple filenames on a one-per-object-file basis.

--add-gnu-debuglink=path-to-file
Creates a .gnu_debuglink section which contains a reference to path-to-file and adds it to the output file. Note: the file at path-to-file must exist. Part of the process of adding the .gnu_debuglink section involves embedding a checksum of the contents of the debug info file into the section.

If the debug info file is built in one location but it is going to be installed at a later time into a different location then do not use the path to the installed location. The --add-gnu-debuglink option will fail because the installed file does not exist yet. Instead put the debug info file in the current directory and use the --add-gnu-debuglink option without any directory components, like this:

 objcopy --add-gnu-debuglink=foo.debug
At debug time the debugger will attempt to look for the separate debug info file in a set of known locations. The exact set of these locations varies depending upon the distribution being used, but it typically includes:

* The same directory as the executable.
* A sub-directory of the directory containing the executable
called .debug

* A global debug directory such as /usr/lib/debug.
As long as the debug info file has been installed into one of these locations before the debugger is run everything should work correctly.
```

例如：

```
# 去除目标文件中的符号
$strip objfile
$nm objfile
nm: objfile: no symbols

# 拷贝出一个符号表文件
$objcopy --only-keep-debug mainO3 mainO3.symbol       

# 拷贝出一个不包含调试信息的执行文件
$objcopy --strip-debug mainO3 mainO3.bin

$objcopy --add-gnu-debuglink=mainO3.symbol mainO3
```

refer:

* [linux中的strip命令简介------给文件脱衣服](https://blog.csdn.net/stpeace/article/details/47090255)






