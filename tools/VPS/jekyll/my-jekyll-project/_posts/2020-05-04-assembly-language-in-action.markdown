---
layout: post
title:  "Assembly Language in Action"
date:   2020-05-04 08:00:00 +0800
categories: [Assembly Language,]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Common Knowledge

* 汇编指令是机器指令的助记符，同**机器指令**一一对应。
* 每一种 CPU 都有自己的**汇编指令集**。
* CPU 可以直接使用的信息在**存储器**中存放。
* 在**存储器**中`指令`和`数据`没有任何区别，都是二进制信息。
* 存储单元从零开始顺序编号。
* 一个存储单元可以存储**8 个 bit**，即 8 位二进制数。
* 1B=8b 1KB=1024B 1MB=1024KB 1GB=1024MB
* 每一个 CPU 芯片都有许多**管脚**，这些管脚和**总线**相连。也可以说，这些管脚引出总线。一个 CPU 可以引出**三种总线**的宽度标志了这个 CPU 的不同方面的性能：
	+ **地址总线** 的宽度决定了 CPU 的**寻址能力**；
	+ **数据总线** 的宽度决定了 CPU 与其他器件进行数据传送时的一次数据传送量；
	+ **控制总线** 的宽度决定了 CPU 对系统中其他器件的控制能力。



* 内存地址空间

什么是内存地址空间呢？举例来讲，一个 CPU 的地址线宽度为 10 ，那么可以寻址 1024 个内存单元，这 1024 个可寻到的内存单元就构成这个 CPU 的内存地址空间。

> 注意：最终运行程序的是 CPU ，我们用汇编编程的时候，必须要从 CPU 角度考虑问题。对 CPU 来讲，系统中的所有存储器中的存储单元都处于一个统一的逻辑存储器中，它的容量受 CPU 寻址能力的限制。这个逻辑存储器即是我们所说的内存地址空间。



* 主板

在每一台 PC 机中，都有一个主板，主板上有核心器件和一些主要器件，这些器件通过总线 ( 地址总线、数据总线、控制总线 ) 相连。这些器件有： CPU 、存储器、外围芯片组、扩展插槽等。扩展插槽上一般插有 RAM 内存条和各类接口卡。


* 接口卡

计算机系统中，所有可用程序控制其工作的设备，必须受到 CPU 的控制。 CPU 对外部设备都不能直接控制，如显示器、音响、打印机等。直接控制这些设备进行工作的是插在扩展插槽上的接口卡。扩展插槽通过总线和 CPU 相连，所以接口卡也通过总线同 CPU 相连。 CPU 可以直接控制这些接口卡，从而实现 CPU 对外设的间接控制。简单地讲，就是 CPU 通过总线向接口卡发送命令，接口卡根据 CPU 的命令控制外设进行工作 。


* 各类存储器芯片

一台 PC 机中，装有多个存储器芯片，这些存储器芯片从物理连接上看是独立的、不同的器件。从读写属性上 看分为两类：**随机存储器 (RAM)** 和 **只读存储器 (ROM)** 。随机存储器可读可写，但必须带电存储，关机后存储的内容丢失；只读存储器只能读取不能写入，关机后其中的内容不丢失。这些存储器从功能和连接上 又分为以下几类：


随机存储器：用于存放供 CPU 使用的绝大部分程序和数据 ，主随机存储器一般由两个位置上的 RAM 组成，装在主板上的 RAM 和插在扩展插槽上的 RAM 。

装有 BIOS (Basic Input/Output System ， 基本输入输出系统 ) 的 ROM：BIOS 是由主板和各类接口卡 ( 如 ： 显卡、网卡等 ) 厂商提供的软件系统 ， 可以通过它利用该硬件设备进行最基本的输入输出。在主板和某些接口卡上插有存储相应 BIOS 的 ROM 。例如：主板上的 ROM 中存储着主板的 BIOS( 通常称为系统 BIOS) ；显卡上的 ROM 中存储着显卡的 BIOS ；如果网卡上装有 ROM ，那其中就可以存储网卡的 BIOS 。


* 接口卡上的 RAM

某些接口卡需要对大批量输入、输出数据进行暂时存储，在其上装有 RAM 。最典型的是显示卡上的 RAM ，一般称为显存。显示卡随时将显存中的数据向显示器上输出。换句话说，我们将需要显示的内容写入显存，就会出现在显示器上。


# CPU Registers

`Registers` are a space in the CPU that can be used to hold data. In an x64 CPU, each register can hold 64 bits. 

The most common registers are the **general-purpose registers**. They are called general-purpose because they can be used to store any kind of data. x64 defines `16 `of these registers: **rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14 and r15**

There is another kind of registers called **special-purpose registers**. These registers have a specific pupose. To give an example, `rip` is called the instruction pointer; it always points to the next instruction to be executed by the program. Another example is `rflags`; this register contains various flags that change depending on the result of an operation; the flags tell you things like if the result was zero, there was a carry or an overflow, etc. There are more special purpose registers, but I won’t explore them in this article.


# Intel vs AT&T

There are two ways to write assembly; `Intel syntax`, which in the beginning was used mostly in the windows world and `AT&T syntax`, which was used everywhere else. Which syntax you use can also depend on the assembler.

Here is an example instruction in Intel:

```
mov rax, 1
```

And the same instruction in AT&T:

```
mov $1, %rax
```

**Both instructions set the registry rax to the value 1**. We can see in the AT&T case that the value 1 is prefixed with `$` and the registry name is prefixed with `%`. The order of the parameters is also different.

In the rest of the article I will use only `Intel syntax` because it’s the one supported by `NASM`.


# Installing an assembler

Assembly is a low level language where we tell the computer exactly which instructions to execute, but the code we write in a text editor has to be transformed into a binary file that the OS and processor can execute. An assembler takes care of this step.

`Nasm` is one of the most popular assemblers out there. It has great support for x64 and works in multiple platforms. To install nasm in Ubuntu, you can do:

```
sudo apt-get install nasm
```

You can verify it installed correctly:

```
$ nasm -v
NASM version 2.13.02
```

# Assembling a program

The general format for assembling a program is:

```
nasm -f <format> -o <output file> <source file>
```

The `format` is the platform for which the program will be assembled (windows, linux, etc). To see the list of supported formats you can use:

```
# For a list of valid output formats, use -hf
$ nasm -hf

valid output formats for -f are (`*' denotes default):
  * bin       flat-form binary files (e.g. DOS .COM, .SYS)
    ith       Intel hex
    srec      Motorola S-records
    aout      Linux a.out object files
    aoutb     NetBSD/FreeBSD a.out object files
    coff      COFF (i386) object files (e.g. DJGPP for DOS)
    elf32     ELF32 (i386) object files (e.g. Linux)
    elf64     ELF64 (x86_64) object files (e.g. Linux)
    elfx32    ELFX32 (x86_64) object files (e.g. Linux)
    as86      Linux as86 (bin86 version 0.3) object files
    obj       MS-DOS 16-bit/32-bit OMF object files
    win32     Microsoft Win32 (i386) object files
    win64     Microsoft Win64 (x86-64) object files
    rdf       Relocatable Dynamic Object File Format v2.0
    ieee      IEEE-695 (LADsoft variant) object file format
    macho32   NeXTstep/OpenStep/Rhapsody/Darwin/MacOS X (i386) object files
    macho64   NeXTstep/OpenStep/Rhapsody/Darwin/MacOS X (x86_64) object files
    dbg       Trace of all info passed to output stage
    elf       ELF (short name for ELF32)
    macho     MACHO (short name for MACHO32)
    win       WIN (short name for WIN32)
```

Since I’m using Linux, I’ll use something like this to assemble my programs:

```
nasm -f elf64 -o example.o example.asm
```

**There is one more step before our program is ready to run. We need to link it**. Linking a program is helpful to combine many object files together and is necessary to create the executable we need. For linking a program, I’ll use GNU linker (`ld`):

```
ld -o <executable name> <object file>
```

We can try these steps with an empty file and see what happens:

```
touch example.asm
nasm -f elf64 -o example.o example.asm
ld -o example example.o
```

If you run those commands, you will notice that the assembly step finishes successfully, but there is an error in the linking step:


```
$ touch example.asm
$ nasm -f elf64 -o example.o example.asm
$ hexdump example.o
0000000 457f 464c 0102 0001 0000 0000 0000 0000
0000010 0001 003e 0001 0000 0000 0000 0000 0000
0000020 0000 0000 0000 0000 0040 0000 0000 0000
0000030 0000 0000 0040 0000 0000 0040 0004 0001
0000040 0000 0000 0000 0000 0000 0000 0000 0000
*
0000080 0001 0000 0003 0000 0000 0000 0000 0000
0000090 0000 0000 0000 0000 0140 0000 0000 0000
00000a0 001b 0000 0000 0000 0000 0000 0000 0000
00000b0 0001 0000 0000 0000 0000 0000 0000 0000
00000c0 000b 0000 0002 0000 0000 0000 0000 0000
00000d0 0000 0000 0000 0000 0160 0000 0000 0000
00000e0 0030 0000 0000 0000 0003 0000 0002 0000
00000f0 0008 0000 0000 0000 0018 0000 0000 0000
0000100 0013 0000 0003 0000 0000 0000 0000 0000
0000110 0000 0000 0000 0000 0190 0000 0000 0000
0000120 000d 0000 0000 0000 0000 0000 0000 0000
0000130 0001 0000 0000 0000 0000 0000 0000 0000
0000140 2e00 6873 7473 7472 6261 2e00 7973 746d
0000150 6261 2e00 7473 7472 6261 0000 0000 0000
0000160 0000 0000 0000 0000 0000 0000 0000 0000
0000170 0000 0000 0000 0000 0001 0000 0004 fff1
0000180 0000 0000 0000 0000 0000 0000 0000 0000
0000190 6500 6178 706d 656c 612e 6d73 0000 0000
00001a0

$ ld -o example example.o
ld: warning: cannot find entry symbol _start; not setting start address
```

An assembly program needs a`_start` entry point. Let’s modify our example so it works:

```
section .text
  global _start
_start:
```

**This is the tiniest program that can be linked successfully, but it does nothing**. Not only, it does nothing, but it fails to execute:

```
$ nasm -f elf64 -o example.o example.asm
$ hexdump example.o
0000000 457f 464c 0102 0001 0000 0000 0000 0000
0000010 0001 003e 0001 0000 0000 0000 0000 0000
0000020 0000 0000 0000 0000 0040 0000 0000 0000
0000030 0000 0000 0040 0000 0000 0040 0005 0002
0000040 0000 0000 0000 0000 0000 0000 0000 0000
*
0000080 0001 0000 0001 0000 0006 0000 0000 0000
0000090 0000 0000 0000 0000 0180 0000 0000 0000
00000a0 0000 0000 0000 0000 0000 0000 0000 0000
00000b0 0010 0000 0000 0000 0000 0000 0000 0000
00000c0 0007 0000 0003 0000 0000 0000 0000 0000
00000d0 0000 0000 0000 0000 0180 0000 0000 0000
00000e0 0021 0000 0000 0000 0000 0000 0000 0000
00000f0 0001 0000 0000 0000 0000 0000 0000 0000
0000100 0011 0000 0002 0000 0000 0000 0000 0000
0000110 0000 0000 0000 0000 01b0 0000 0000 0000
0000120 0060 0000 0000 0000 0004 0000 0003 0000
0000130 0008 0000 0000 0000 0018 0000 0000 0000
0000140 0019 0000 0003 0000 0000 0000 0000 0000
0000150 0000 0000 0000 0000 0210 0000 0000 0000
0000160 0014 0000 0000 0000 0000 0000 0000 0000
0000170 0001 0000 0000 0000 0000 0000 0000 0000
0000180 2e00 6574 7478 2e00 6873 7473 7472 6261
0000190 2e00 7973 746d 6261 2e00 7473 7472 6261
00001a0 0000 0000 0000 0000 0000 0000 0000 0000
*
00001c0 0000 0000 0000 0000 0001 0000 0004 fff1
00001d0 0000 0000 0000 0000 0000 0000 0000 0000
00001e0 0000 0000 0003 0001 0000 0000 0000 0000
00001f0 0000 0000 0000 0000 000d 0000 0010 0001
0000200 0000 0000 0000 0000 0000 0000 0000 0000
0000210 6500 6178 706d 656c 612e 6d73 5f00 7473
0000220 7261 0074 0000 0000 0000 0000 0000 0000
0000230
$ ld -o example example.o
$ ./example 
-bash: ./example: cannot execute binary file: Exec format error
```

**Adding an instruction** to our program fixes this problem:

```
section .text
  global _start
_start:
  mov rax, 1
```

But we get a segmentation fault:

```
$ nasm -f elf64 -o example.o example.asm
$ ld -o example example.o
$ ./example 
Segmentation fault (core dumped)
```

**The reason we get a segmentation fault is that the program doesn’t end correctly**. In higher level programming languages, the runtime (the compiler) takes care of this. In assembly, this needs to be done by the programmer. To do this, we need to use **syscall 60** (`sys_exit`). The interface for `sys_exit` is:

```
rdi int error_code
```

What this means is that is takes a single int argument in the `rdi` register. This argument is the exit code for the program. A successful program should finish with code `0`.

Let’s make our program end successfully:

``` asm
section .text
  global _start
_start:
  mov rax, 60
  mov rdi, 0
  syscall
```

Looking at the program, you’ll notice that we first have to move the value `60` (The id of `sys_exit`) to the `rax` register. **This is necessary to execute any system call**. The next step is to populate the correct registers with the arguments that system call needs. In this case, it only needs the exit code in `rdi`. Finally, execute the system call.

This program can be executed, and although it doesn’t do anything, it will end successfully:

```
$ nasm -f elf64 -o example.o example.asm
$ ld -o example example.o
$ ./example 
$ echo $?
0
$ ls -rtlh
total 12K
-rw-rw-r-- 1 ubuntu ubuntu  75 May  5 12:10 example.asm
-rw-rw-r-- 1 ubuntu ubuntu 576 May  5 12:10 example.o
-rwxrwxr-x 1 ubuntu ubuntu 704 May  5 12:10 example
$ strip example
$ ls -rtlh
total 12K
-rw-rw-r-- 1 ubuntu ubuntu  75 May  5 12:10 example.asm
-rw-rw-r-- 1 ubuntu ubuntu 576 May  5 12:10 example.o
-rwxrwxr-x 1 ubuntu ubuntu 352 May  5 12:10 example
```

After strip, you may find that the size of tiniest elf is about 350 bytes.


# Variables

The simplest way do declare variables is by initializing them in the `.data` segment of a program. The format to define initialized data is:

```
[variable-name] define-directive initial-value [,initial-value] ...
```

An example use:

``` asm
section .data
  exit_code dq 0
  sys_call dq 60

section .text
  global _start

_start:
  mov rax, [sys_call]
  mov rdi, [exit_code]
  syscall
```

When a variable is defined, some space in memory will be set for it. The `dq` directive is used to **reserve 64 bits in memory (8 bytes)**.

Something new in this code snippet is the use of **square brackets** `[]`. If we didn’t use the brackets, we would be assigning the memory address of the variable instead of the value in that memory address.

If you take a look at the initialization template above, you will notice that you can **supply multiple initial values**. When this is done, the variable works like an array. i.e. it uses one name to refer to multiple contiguous memory locations:

```
some_array dq 1, 1, 2, 3, 5, 8
```

Something similar can be done for `strings`, but luckily they allow us to type the whole value instead of having to type one character at a time:

```
some_string db "Hello world"
```

In this case, we used `db` to **allocate one byte per character**.

To make large strings easier to type, they can be split into multiple lines like this:

```
some_string db "Hello world, I'm trying to learn assembly, but it's hard. Do "
            db "you know what is the fastest way to learn?", 0
```

The variable name only needs to be specified once, but the `define-directive needs` to be repeated.


# Printing a string

Now that we know how to create strings, let’s try a simple program that prints a string. Before we start, Let’s look at the interface for **syscall 1** (`sys_write`):

```
rdi   int               file_descriptor
rsi   memory_location   string_to_print
rdx   int               string_size
```

For `rdi` we will use `1` because that is **the file descriptor for stdout**. Let’s see how this works in a program:

``` asm
section .data
  some_string dq "Hello world"     
  some_string_size dq 11           ; "Hello world" contains 11 characters

section .text
  global _start

_start:
  ; Print the string
  mov rax, 1                       ; 1 means sys_write
  mov rdi, 1                       ; 1 means stdout
  mov rsi, some_string             ; The memory address to the beginning of the string
  mov rdx, [some_string_size]      ; Number of characters to print
  syscall

  ; Exit the program
  mov rax, 60
  mov rdi, 0
  syscall
```

Executing this code will print `Hello world` to the terminal.

```
$ nasm -f elf64 -o example.o example.asm
$ ld -o example example.o
$ ./example 
Hello world
$ ls -rtlh
total 12K
-rw-rw-r-- 1 ubuntu ubuntu 329 May  5 13:28 example.asm
-rw-rw-r-- 1 ubuntu ubuntu 944 May  5 13:28 example.o
-rwxrwxr-x 1 ubuntu ubuntu 984 May  5 13:29 example
$ strip example
$ ls -rtlh
total 12K
-rw-rw-r-- 1 ubuntu ubuntu 329 May  5 13:28 example.asm
-rw-rw-r-- 1 ubuntu ubuntu 944 May  5 13:28 example.o
-rwxrwxr-x 1 ubuntu ubuntu 528 May  5 13:29 example
```

We may compare this asm program to c program:

``` c
#include <stdio.h>
int main(int argc, char* argv[])
{
    printf("hello world\n");
    return 0;
}
```

then we can get asm code by using `gcc -S`:

```
$ gcc -S helloworld.s helloworld.c
$ gcc helloworld.s
$ ls -rtlh
total 20K
-rw-rw-r-- 1 ubuntu ubuntu   95 May  4 14:13 helloworld.c
-rw-rw-r-- 1 ubuntu ubuntu  521 May  5 13:59 helloworld.s
-rwxrwxr-x 1 ubuntu ubuntu 8.2K May  5 14:02 a.out
$ strip a.out
$ ls -rtlh
total 16K
-rw-rw-r-- 1 ubuntu ubuntu   95 May  4 14:13 helloworld.c
-rw-rw-r-- 1 ubuntu ubuntu  521 May  5 13:59 helloworld.s
-rwxrwxr-x 1 ubuntu ubuntu 6.0K May  5 14:02 a.out
``` 

``` asm
.file   "helloworld.c"
        .text
        .section        .rodata
.LC0:
        .string "hello world"
        .text
        .globl  main
        .type   main, @function
main:
.LFB0:
        .cfi_startproc
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register 6
        subq    $16, %rsp
        movl    %edi, -4(%rbp)
        movq    %rsi, -16(%rbp)
        leaq    .LC0(%rip), %rdi
        call    puts@PLT
        movl    $0, %eax
        leave
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
.LFE0:
        .size   main, .-main
        .ident  "GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
        .section        .note.GNU-stack,"",@progbits
```

We can find the size of c program is bigger than the prior asm program. (**6 KB > 528 B**)


# Instructions

`Instructions` are how we tell the computer to do something. The exact number of instructions on the x64 architecture is hard to find, but it might be somewhere close to one thousand. An instruction consists of an `opcode` and optionally 1 or more `operands`. Let’s look at some common instructions.


## mov

We have already used the mov instruction before:

```
mov rax, 60
```

The `opcode` is mov and it receives 2 `operands` **rax** and **60**. What this instruction does is move the value 60 to the **rax** register.

## add, sub, imul

These are all **binary operations**. They take two operands and the result will be stored on the **first operand**:

```
mov rax, 60
sub rax, 50    ; rax is now 10
add rax, 5     ; rax is now 15
imul rax, 3    ; rax is now 45
```

## inc, dec

To increment an operand we can use `inc` and to decrement it, we can use `dec`:

```
mov rax, 60
inc rax      ; rax is 61
dec rax      ; rax is 60 again
```

## or, xor, and

These are binary bitwise operations:

```
mov rax, 5      ; 5 in binary is 101
and rax, 6      ; 6 in binary is 110. rax now holds 4 (100 in binary)
or rax, 8       ; 8 in binary is 1000. rax is now 12 (1100 in binary)
xor rax, 11     ; 11 in binary is 1011. rax is now 7 (111 in binary)
```

These are just some of the instructions available in an `x64` processor. There are many more that I’m not going to cover in this article.


# Addressing modes

One of the most fundamental things about assembly is understanding **addressing modes**. An addressing mode is a way to specify which values are going to be used as operands for an instruction. We already used addressing modes in the examples above. In this section, we are going to give them names and understand them a little more.

## Immediate mode (立刻的)

The immediate mode looks like this:

```
mov rax, 60
```

This mode is very simple because there is no indirection. The `rax` register will be set to `60`. The value `60` is called **an immediate constant (立即数)**. Immediate constants can be specified in decimal, binary, octal or hexadecimal. These instructions all do the same:

``` asm
mov rax, 60         ; decimal
mov rax, 0b111100   ; binary
mov rax, 0o74       ; octal
mov rax, 0x3C       ; hexadecimal
```

## Register mode

This mode is also very easy to understand. Information inside a register will be used:

```
mov rax, rbx
```

In this case, the value of `rax` will be set to whichever value is currently in `rbx`.

## Indirect mode

In this mode, the register contains a memory address, the value we care about, is the value in that memory address:

```
mov rdi, [rax]
```

In the example above, `rax` contains **a memory address**. `rdi` will be set to **the value in that memory address**. This is easier to understand with an example. Imagine registers and memory looked like this before executing the instruction above:


| Registers | 
| -- | --
| rax | 0x40
| rbx | 0x0
| ... | 
| rdi | 0x0

| Memory |
| -- | -- 
| 0x40 | 0xA
| 0x48 | 0x00
| 0x50 | 0x00
| 0x58 | 0x00


After the instruction is executed, `rdi` will contain `0xA` because `rax` contains the value `0x40`, which is a memory address. By looking at that memory address, we find the value `0xA`.

We can also use **indirect mode** for **variables**, as we did for some of the examples:

```
mov rdx, [some_string_size]      ; indirect mode for variable
```

With indirect mode, we can also do memory displacements, which is useful for arrays. Assumming we have this array:

```
some_array dq 1, 1, 2, 3, 5
```

We can access its elements like this:

```
mov rax, [some_array]         ; rax = 1 (first element)
mov rax, [some_array + 8]     ; rax = 1 (second element)
mov rax, [some_array + 16]    ; rax = 2 (third element)
mov rax, [some_array + 24]    ; rax = 3 (fourth element)
mov rax, [some_array + 32]    ; rax = 5 (fifth element)
```

To understand this a little better we have to remember that each memory address can hold **8 bytes**. The `dq` instruction used to **create the array**, reserves `64 bits` per value, so we need `8 addresses` to hold a single value (64 / 8 = 8. This is the number of memory addresses it takes to hold a value).

The array looks something like this in memory:

| Memory |
| -- | --
| 0xA0 | 0x1
| 0xAB | 0x1
| 0xB0 | 0x2
| 0xB8 | 0x3
| 0xC0 | 0x5

Notice that the address after `0xA0` is not `0xA1` but `0xA8`. This is because each number uses 8 memory addresses (64 bits). This way, every displacement(移位) on the example above, takes us to the next number in the array.


# Debugging assembly with GDB

We can use this command to assemble program:

```
nasm -f elf64 -o example.o example.asm
```

The `elf64` ([Executable and linkable format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)) parameter specifies **the format of the output file**. **This will generate a file with enough information so the Operating System can execute it, but it doesn’t contain any information to help debugging**. If we want our executable to contain debug information (information about the file and line number a program is executing) we need to say so when we assemble the program.

```
$ gdb example 
GNU gdb (Ubuntu 8.1-0ubuntu3.2) 8.1.0.20180409-git
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from example...(no debugging symbols found)...done.  # 没有调试信息
(gdb) 
```

To see what are the formats for debug information available in your version of `nasm`, you can use:

```
# For a list of debug formats, use -f <form> -y
$ nasm -f elf64 -y

valid debug formats for 'elf64' output format are ('*' denotes default):
    dwarf     ELF64 (x86-64) dwarf debug format for Linux/Unix
    stabs     ELF64 (x86-64) stabs debug format for Linux/Unix
```

so we can use:

``` asm
$ nasm -f elf64 -g -F dwarf -o example.o example.asm
$ ld -o example example.o
$ ls -rtlh
total 12K
-rw-rw-r-- 1 ubuntu ubuntu  329 May  5 13:28 example.asm
-rw-rw-r-- 1 ubuntu ubuntu 2.3K May  5 15:43 example.o
-rwxrwxr-x 1 ubuntu ubuntu 2.0K May  5 15:44 example

$ gdb --quiet example 
Reading symbols from example...done.
(gdb) l
1
2       section .data
3         some_string dq "Hello world"
4         some_string_size dq 11   ; "Hello world" contains 11 characters
5
6       section .text
7         global _start
8
9       _start:
10        ; Print the string
(gdb) 
11        mov rax, 1        
12        mov rdi, 1
13        mov rsi, some_string
14        mov rdx, [some_string_size]
15        syscall
16
17        ; Exit the program
18        mov rax, 60
19        mov rdi, 0
20        syscall
(gdb) 
Line number 21 out of range; example.asm has 20 lines.
```

## Debugging with GDB

Now, we can debug this program with GDB:

```
$ gdb --quiet example 
Reading symbols from example...done.
(gdb) b _start
Breakpoint 1 at 0x4000b0: file example.asm, line 11.
(gdb) r
Starting program: /home/ubuntu/code/nasm/example 

Breakpoint 1, _start () at example.asm:11
11        mov rax, 1        
(gdb) l
6       section .text
7         global _start
8
9       _start:
10        ; Print the string
11        mov rax, 1        
12        mov rdi, 1
13        mov rsi, some_string
14        mov rdx, [some_string_size]
15        syscall
(gdb) p $rax
$1 = 0
(gdb) n
12        mov rdi, 1
(gdb) p $rax
$2 = 1
(gdb) i r rax
rax            0x1      1           # The first column is the hexadecimal value (0x1) and the second is decimal (1)
(gdb) n
13        mov rsi, some_string
(gdb) 
14        mov rdx, [some_string_size]
(gdb) 
15        syscall
(gdb) 
Hello world18     mov rax, 60
(gdb) 
19        mov rdi, 0
(gdb) 
20        syscall
(gdb) 
[Inferior 1 (process 26432) exited normally]
(gdb) 
```

Use `q` to quit gdb.


## Inspecting registers

Writing assembly code, you will find yourself moving things in and out of registers very often. It is then natural that debugging a program we might want to see their contents. To see the contents of all registers we can use `info registers` or the abbreviation `i r`. Using the same example program:


```
$ gdb --quiet example 
Reading symbols from example...done.
(gdb) b _start
Breakpoint 1 at 0x4000b0: file example.asm, line 11.
(gdb) r
Starting program: /home/ubuntu/code/nasm/example 

Breakpoint 1, _start () at example.asm:11
11        mov rax, 1        
(gdb) i r
rax            0x0      0
rbx            0x0      0
rcx            0x0      0
rdx            0x0      0
rsi            0x0      0
rdi            0x0      0
rbp            0x0      0x0
rsp            0x7fffffffe420   0x7fffffffe420
r8             0x0      0
r9             0x0      0
r10            0x0      0
r11            0x0      0
r12            0x0      0
r13            0x0      0
r14            0x0      0
r15            0x0      0
rip            0x4000b0 0x4000b0 <_start>
eflags         0x202    [ IF ]
cs             0x33     51
ss             0x2b     43
ds             0x0      0
es             0x0      0
fs             0x0      0
gs             0x0      0
(gdb) n
12        mov rdi, 1
(gdb) i r
rax            0x1      1
rbx            0x0      0
rcx            0x0      0
rdx            0x0      0
rsi            0x0      0
rdi            0x0      0
rbp            0x0      0x0
rsp            0x7fffffffe420   0x7fffffffe420
r8             0x0      0
r9             0x0      0
r10            0x0      0
r11            0x0      0
r12            0x0      0
r13            0x0      0
r14            0x0      0
r15            0x0      0
rip            0x4000b5 0x4000b5 <_start+5>
eflags         0x202    [ IF ]
cs             0x33     51
ss             0x2b     43
ds             0x0      0
es             0x0      0
fs             0x0      0
gs             0x0      0
(gdb) 
```

By printing the registers we can see that the breakpoint takes effect before executing the line: `mov rax, 60`. In many cases we probably only want to see a specific register. To do this we just need to add the register name to the command: `i r <register>`:

## Inspecting memory

Note that we need to cast the variable to the correct type or we’ll get an error. Another thing we can do is get the memory address by `info address`, then We can also see the data at a memory address using an asterisk (`*`)

```
$ gdb --quiet example
Reading symbols from example...done.
(gdb) b _start
Breakpoint 1 at 0x4000b0: file example.asm, line 11.
(gdb) r
Starting program: /home/ubuntu/code/nasm/example 

Breakpoint 1, _start () at example.asm:11
11        mov rax, 1        
(gdb) l
6       section .text
7         global _start
8
9       _start:
10        ; Print the string
11        mov rax, 1        
12        mov rdi, 1
13        mov rsi, some_string
14        mov rdx, [some_string_size]
15        syscall
(gdb) p some_string_size
'some_string_size' has unknown type; cast it to its declared type
(gdb) p (int) some_string_size
$1 = 11
(gdb) info address some_string_size
Symbol "some_string_size" is at 0x6000ec in a file compiled without debugging.
(gdb) p (int) *0x6000ec
$2 = 11
```


# Refer

* 汇编语言（第2版），清华大学出版社，王爽
* [Introduction to assembly - Assembling a program - Posted by adrian.ancona on January 30, 2019](https://ncona.com/2019/01/introduction-to-assembly-assembling-a-program/)
* [Assembly - Variables, instructions and addressing modes - Posted by adrian.ancona on February 27, 2019](https://ncona.com/2019/02/assembly-variables-instructions-and-addressing-modes/)
* [Debugging assembly with GDB - Posted by adrian.ancona on December 11, 2019](https://ncona.com/2019/12/debugging-assembly-with-gdb/)
* [Introduction to x64 Assembly - Published on March 19, 2012](https://software.intel.com/en-us/articles/introduction-to-x64-assembly)




