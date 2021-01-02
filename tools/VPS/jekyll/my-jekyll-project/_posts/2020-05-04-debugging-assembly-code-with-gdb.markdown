---
layout: post
title:  "Debugging Assembly Code with GDB"
date:   2020-05-04 08:00:00 +0800
categories: [GDB,]
---

* Do not remove this line (it will not be displayed)
{:toc}


# What is GDB

The purpose of a debugger such as GDB is to allow you to see what is going on “inside” another program while it executes—or what another program was doing at the moment it crashed. GDB can **do four main kinds of things** (plus other things in support of these) to help you catch bugs in the act:

1. Start your program, specifying anything that might affect its behavior.
2. Make your program stop on specified conditions.
3. Examine what has happened, when your program has stopped.
4. Change things in your program, so you can experiment with correcting the effects of one bug and go on to learn about another.


You can use GDB to debug programs written in C and C++. For more information, see [Supported Languages](https://sourceware.org/gdb/current/onlinedocs/gdb/Supported-Languages.html#Supported-Languages). For more information, see [C and C++](https://sourceware.org/gdb/current/onlinedocs/gdb/C.html#C).


#  How to Use

## Compiling for Debugging

In order to debug a program effectively, you need to generate debugging information when you compile it. This debugging information is stored in the object file; it describes the data type of each variable or function and the correspondence between source line numbers and addresses in the executable code.

To request debugging information, specify the `-g` option when you run the compiler.

* Programs that are to be shipped to your customers are compiled with optimizations, using the `-O` compiler option. However, some compilers are unable to handle the `-g` and `-O` options together. Using those compilers, you cannot generate optimized executables containing debugging information. ( -g 与 -O 冲突的情况)
* GCC, the GNU C/C++ compiler, supports `-g` with or without `-O`, **making it possible to debug optimized code**. We recommend that you always use `-g` whenever you compile a program. You may think your program is correct, but there is no sense in pushing your luck. For more information, see [Optimized Code](https://sourceware.org/gdb/current/onlinedocs/gdb/Optimized-Code.html#Optimized-Code).
* GDB knows about preprocessor macros and can show you their expansion (see [Macros](https://sourceware.org/gdb/current/onlinedocs/gdb/Macros.html#Macros)). Most compilers do not include information about preprocessor macros in the debugging information if you specify the `-g` flag alone. **Version 3.1 and later of GCC, the GNU C compiler, provides macro information if you are using the DWARF debugging format, and specify the option `-g3`.**


Options for Debugging Your Program: http://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html#Debugging-Options



## Invoking or Quitting GDB

```
# debug a program
gdb program  

# debug a core file
gdb program core

# debug a running process
gdb program 1234
gdb -p 1234

# This will cause gdb to debug gcc, and to set gcc’s command-line arguments to ‘-O2 -c foo.c’
gdb --args gcc -O2 -c foo.c

# Quit
quit / q / Ctrl-d
```

Your Program’s Arguments：https://sourceware.org/gdb/current/onlinedocs/gdb/Arguments.html#Arguments

```
$ gdb --silent a.out
Reading symbols from a.out...done.
(gdb) l
1       #include <stdio.h>
2
3       int main(int argc, char* argv[])
4       {
5               printf("hello world\n");
6
7               return 0;
8       }
(gdb) 
```

Starting your Program: https://sourceware.org/gdb/current/onlinedocs/gdb/Starting.html#Starting

## What GDB Does During Startup

https://sourceware.org/gdb/current/onlinedocs/gdb/Startup.html#Startup


## Shell Commands

If you need to execute occasional shell commands during your debugging session, there is no need to leave or suspend GDB; you can just use the shell command.

```
shell command-string
!command-string
```

## GDB Commands

You can **abbreviate** a GDB command to **the first few letters of the command name, if that abbreviation is unambiguous(明确的)**; and you can repeat certain GDB commands by typing just `RET`. You can also use the `TAB` key to get GDB to fill out the rest of a word in a command (or to show you the alternatives available, if there is more than one possibility).


## Program’s Environment

The environment consists of a set of environment variables and their values. Environment variables conventionally record such things as `your user name`, `your home directory`, `your terminal type`, and `your search path` for programs to run. Usually you set up environment variables with the shell and they are inherited by all the other programs you run. When debugging, it can be useful to try running your program with a modified environment without having to start GDB over again.

```
# Add directory to the front of the PATH environment variable (the search path for executables) that will be passed to your program
path directory

# Display the list of search paths for executables (the PATH environment variable).
show paths
```

https://sourceware.org/gdb/current/onlinedocs/gdb/Environment.html#Environment

## Program’s Working Directory

```
# Print the GDB working directory
pwd

# Set the GDB working directory to directory. If not given, directory uses '~'
cd [directory]
```

https://sourceware.org/gdb/current/onlinedocs/gdb/Working-Directory.html#Working-Directory

## Program’s Input and Output

You can redirect your program’s input and/or output using shell redirection with the run command. For example,

```
run > outfile
```

https://sourceware.org/gdb/current/onlinedocs/gdb/Input_002fOutput.html#Input_002fOutput


## Debugging Programs with Multiple Threads

In some operating systems, such as GNU/Linux and Solaris, a single program may have more than one thread of execution. The precise semantics of threads differ from one operating system to another, but in general the threads of a single program are akin to multiple processes—except that they share one address space (that is, they can all examine and modify the same variables). On the other hand, each thread has its own registers and execution stack, and perhaps private memory.


https://sourceware.org/gdb/current/onlinedocs/gdb/Threads.html#Threads

## Debugging Forks

If you want to follow the child process instead of the parent process, use the command `set follow-fork-mode`.

```
# Set the debugger response to a program call of fork or vfork. A call to fork or vfork creates a new process.
set follow-fork-mode child

# Display the current debugger response to a fork or vfork call.
show follow-fork-mode

# On Linux, if you want to debug both the parent and child processes, use the command set detach-on-fork.
set detach-on-fork mode

# Show whether detach-on-fork mode is on/off.
show detach-on-fork
```

https://sourceware.org/gdb/current/onlinedocs/gdb/Forks.html#Forks

https://stackoverflow.com/questions/15126925/debugging-child-process-after-fork-follow-fork-mode-child-configured


## Breakpoints

A `breakpoint` makes your program stop whenever a certain point in the program is reached. For each breakpoint, you can add conditions to control in finer detail whether your program stops. You can set breakpoints with the break command and its variants (see [Setting Breakpoints](https://sourceware.org/gdb/current/onlinedocs/gdb/Set-Breaks.html#Set-Breaks)), to specify the place where your program should stop **by line number, function name or exact address in the program**.

On some systems, **you can set breakpoints in shared libraries before the executable is run**.

https://sourceware.org/gdb/current/onlinedocs/gdb/Breakpoints.html#Breakpoints

## Watchpoints


A `watchpoint` is **a special breakpoint** that stops your program **when the value of an expression changes**. The expression may be a value of a variable, or it could involve values of one or more variables combined by operators, such as ‘a + b’. **This is sometimes called data breakpoints**. You must use a different command to set watchpoints (see [Setting Watchpoints](https://sourceware.org/gdb/current/onlinedocs/gdb/Set-Watchpoints.html#Set-Watchpoints)), but aside from that, you can manage a watchpoint like any other breakpoint: you enable, disable, and delete both breakpoints and watchpoints using the same commands.


You can arrange to have values from your program displayed automatically whenever GDB stops at a breakpoint. See [Automatic Display](https://sourceware.org/gdb/current/onlinedocs/gdb/Auto-Display.html#Auto-Display).

## Catchpoints

A `catchpoint` is another special breakpoint that stops your program **when a certain kind of event occurs**, such as the throwing of a C++ exception or the loading of a library. As with watchpoints, you use a different command to set a catchpoint (see [Setting Catchpoints](https://sourceware.org/gdb/current/onlinedocs/gdb/Set-Catchpoints.html#Set-Catchpoints)), but aside from that, you can manage a catchpoint like any other breakpoint. (To stop when your program receives a signal, use the `handle` command; see [Signals](https://sourceware.org/gdb/current/onlinedocs/gdb/Signals.html#Signals).)

## Disassemble


Disassembles a specified function or a function fragment.

```
disassemble
disassemble [Function]
disassemble [Address]
disassemble [Start],[End]
disassemble [Function],+[Length]
disassemble [Address],+[Length]
disassemble /m [...]
disassemble /r [...]
```

https://visualgdb.com/gdbreference/commands/disassemble


## Add index files to speed up GDB

对于比较大的二进制文件，为了缩短gdb的加载时间可以对程序文件事先创建符号索引。

```
gdb-add-index filename
```

https://man7.org/linux/man-pages/man1/gdb-add-index.1.html


## Other


`gdb` is the GNU source-level debugger that is standard on linux (and many other unix) systems. It can be used both for programs written in high-level languages like C and C++ and for assembly code programs; this document concentrates on the latter.

For detailed information on the use of gdb, consult the documentation. Unfortunately, this is not in form of a man page, and even the info page does not seem
to be installed on the linuxlab machines; the best thing is to read it on the web at [http://sourceware.org/gdb/current/onlinedocs/gdb/](http://sourceware.org/gdb/current/onlinedocs/gdb/).

gdb will work in an ordinary terminal window, and this is fine for debugging assembly code. For use with higher-level source code, it is more convenient to use gdb from within the emacs editor (a good one to learn!) or using a graphical front-end like ddd. The basic commands remain the same.

To use gdb with high-level language programs, you should compile with the `-g` option. This will include information in the object file to relate it back to the source file. When assembling `.s` files to be debugged, the `-g` option is not necessary, but it is harmless.



```
gcc -m64 -g -o foo fooDriver.c fooRoutine.s
```


To invoke the debugger on `foo`, type

```
gdb foo
```

This loads program `foo` and brings up the gdb command line interpreter, which then waits for you to type commands. Program execution doesn’t begin until you say so.
Here are some useful commands. Many can be **abbreviated**, as shown. Hitting return generally repeats the last command, sometimes advancing the current location.


* `h[elp] [keyword]`

Displays help information.

* `r[un] [args]`

Begin program execution. If the program normally takes command-line arguments (e.g., foo hi 3), you should specify them here (e.g., run hi 3).


* `b[reak] [address]`

Set a breakpoint at the specified address (or at the current address if none specified). Addresses can be given symbolically (e.g., foo) or numerically (e.g. 0x10a38). When execution reaches a breakpoint, you are thrown back into the gdb command line interpreter.


* `c[ontinue]`

Continue execution after stopping at a breakpoint.

* `i[nfo] b[reak]`

Display numbered list of all breakpoints currently set.


* `d[elete] b[reakpoints] number`

Delete specified breakpoint number.

* `p[rint][/format] expr`

Print the value of an expression using the specified format (decimal if unspecified). Expressions can involve program variables or registers, which are specified
using a `$` rather than a `%` sign. Useful formats include:
	+ d decimal
	+ x hex
	+ t binary
	+ f floating point
	+ i instruction
	+ c character

For example, to display the value of register `%rdi` in decimal, type `p/x $rdi`. Note that you need to use the **64-bit (%r) forms of register names**. To see the
value of the current program counter, type `p/x $rip`.


* `i[nfo] r[egisters] register`

An alternative way to print the value of a register (or, if none is specified, of all registers) in hex and decimal. Specify the register without a leading %, e.g., `i r rdi`.


* `x/[count][format] [address]`

Examine the contents of a specified memory address, or the current address if none specified. If count is specified, displays specified number of words. Addresses can be symbolic (e.g., main) or numeric (e.g., 0x10a44). Formats are as for print. Particularly useful for printing the program text, e.g., `x/100i foo` disassembles and prints 100 instructions starting at foo.n

* `disas[semble] address [,address]`

Another way to print the assembly program text surrounding an address, or between two addresses.

* `set var = expr`

Set specified register or memory location to value of expression. Examples: set `$rdi=0x456789AB` or `set myVar=myVar*2`.


* `s[tep]i`

Execute a single instruction and then return to the command line interpreter.

* `n[ext]i`

Like `stepi`, except that if the instruction is a subroutine call, the entire subroutine is executed before control returns to the interpreter.

* `whe[re]` \ `backtrace (bt)`

Show current activation stack.

* `q[uit]` \ `Ctrl-d`

To exit GDB, use the `quit` command (abbreviated `q`), or type an end-of-file character (usually `Ctrl-d`). An interrupt (often `Ctrl-c`) does not exit from GDB, but rather terminates the action of any GDB command that is in progress and returns to GDB command level.

# Common Use

* gdb中简写命令配合tab键使用

| 命令 | 作用
| -- | --
| dir path | 切换目录
| gdb -p PID; b xxx; c | PID方式调试（或 gdb program pid）
| gdb bin; b xxx; r 参数 | bin方式调试
| break filename:lineno | 在文件某行设置断点
| list filename:lineno | 选择显示文件的哪一行
| list | 显示当前文件的默认行数代码信息
| run para | 调试带参数的程序
| print VarName | 显示变量当前的值
| ptype VarName | 显示变量的类型
| frame 0 | 显示栈顶的信息
| frame 1 | 显示堆栈栈顶下一层信息
| up n | 表示向栈的上面移动n层，可以不打n，表示向上移动一层
| down n | 表示向栈的下面移动n层，可以不打n，表示向下移动一层                              
| info break | 查看所有断点的信息  
| info threads | 查看所有线程的信息
| info registers | 查看所有寄存器的信息
| shell date | 在gdb中调用shell，使用完后exit重新返回到gdb中
| set args | 可指定运行时参数（如：set args 10 20 30 40 50）
| show args | 命令可以查看设置好的运行参数
| finish/fin | 跳出当前函数栈


# Refer


* [Debugging with GDB - Tenth Edition](http://sourceware.org/gdb/current/onlinedocs/gdb/)
	+ [A Sample GDB Session](https://sourceware.org/gdb/current/onlinedocs/gdb/Sample-Session.html#Sample-Session)
* [Debugging Assembly Code with gdb](http://web.cecs.pdx.edu/~apt/cs491/gdb.pdf)
* [100个gdb小技巧](https://github.com/hellogcc/100-gdb-tips)
* [GDB中应该知道的几个调试方法 - CoolShell](http://coolshell.cn/articles/3643.html)
* [Introduction to GDB - Posted by adrian.ancona on February 9, 2018](https://ncona.com/2018/02/introduction-to-gdb/)
* [Debugging assembly with GDB - Posted by adrian.ancona on December 11, 2019](https://ncona.com/2019/12/debugging-assembly-with-gdb/)

