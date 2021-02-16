---
layout: post
title:  "ELF PLT Hook"
date:   2021-02-13 13:00:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}


# 动态链接

* 在动态链接方式实现以前，普遍采用**静态链接**的方式来生成可执行文件。如果一个程序使用了外部的库函数，那么整个库都会被直接编译到可执行文件中。
* ELF支持**动态链接**，当一个程序被加载进内存时，动态链接器会把需要的共享库加载并绑定到该进程的地址空间中。随后在调用某个函数时，对该函数地址进行解析，以达到对该函数调用的目的。

# -fPIC作用

* `-fPIC`参数作用于**编译阶段**，是告诉编译器生成与位置无关（Position Independent Code）的代码。
* 对于共享库来说，如果不加`-fPIC`，则`.so`文件的代码段在被加载时，代码段引用的数据对象需要重新定位，重新定位会修改代码段的内容，这就造成每个使用这个`.so`文件代码段的进程在内核中都需要生成这个`.so`文件代码段的副本，每个副本都不一样，具体取决于这个`.so`文件代码和数据段内存映射的位置。
* 当添加`-fPIC`参数后，则产生的代码中，没有绝对地址，全部使用相对地址，故而代码可以被加载器加载到内存的任意位置，都可以正确执行。

因此，对于动态库来说，一般产生位置无关的代码。


# ELF (Executable and Linkable Format)

> In computing, the Executable and Linkable Format (ELF, formerly named Extensible Linking Format), is a common standard file format for executable files, object code, shared libraries, and core dumps. First published in the specification for the application binary interface (ABI) of the Unix operating system version named System V Release 4 (SVR4), and later in the Tool Interface Standard, it was quickly accepted among different vendors of Unix systems. In 1999, it was chosen as the standard binary file format for Unix and Unix-like systems on x86 processors by the 86open project.


每个`ELF文件`由`一个ELF头`和`文件数据`组成。这些数据包括：

* 程序头表，描述0或多个内存段
* 节头表，描述0或多个段
* 由程序头或节头表引用的数据

ELF文件参与程序的**链接**和程序的**执行**，因此通常可以分别从**可链接文件角度**和**可执行文件角度**来看待ELF文件的格式。

* 对于**编译链接**，则编译器和链接器把ELF文件看作是**节头表描述的节集合，而程序头表可选**
* 对于**加载执行**，则加载器把ELF文件看作是**程序头表描述的段的集合，而节头表可选**
* 如果是共享文件，则两者都包含

动态链接和动态加载：

* 为了解决静态链接空间浪费和程序更新困难的问题，通常会将程序模块互相分割，形成独立的文件（动态库），而不是将它们静态链接在一起。
* 动态链接过程
  + 操作系统在加载一个可执行的ELF文件，内核首先将ELF映像加载到用户空间虚拟内存，在加载完动态链接器后（lib/ld-linux.so），系统会读取`.dynamic`段中的`DT_NEEDED`条目，该条目中列出该可执行文件所依赖的共享库，之后动态链接器就会去依次加载这些共享库，共享库本身依赖了其他库，这些库也会被加载。
  + 当所有的共享库都被加载完毕后，就开始执行**重定位的流程**。重定位是通过**过程链接表（PLT）和 全局偏移表（GOT）**的间接机制来处理的。这些表提供了外部函数和数据的地址，动态链接器需要根据全局符号表重定位每一个符号的地址，即修正GOT和PLT。


![ELF_Executable_and_Linkable_Format_diagram_by_Ange_Albertini](/assets/images/202102/ELF_Executable_and_Linkable_Format_diagram_by_Ange_Albertini.png)

refer: 

* [Executable and Linkable Format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)
* [linux/include/uapi/linux/elf.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/elf.h)


# GOT/PLT

* GOT (The Global Offset Table)
  + 在程序中以`.got.plt`表示，该表处于**数据段**，每一个表项存储的都是一个地址，每个表项长度是当前程序的对应需要寻址长度（32位程序：4字节，64位程序：8字节）。
  + 问题：如果一个引用的函数是在共享库中，而共享库在加载时没有固定地址，所以在GOT表中无法直接保存该符号的地址，此时就需要引入PLT表。

* PLT (The Procedure Linkage Table)
  + 在程序中以`.plt`节表示，该表处于**代码段**，每一个表项表示了一个与要重定位的函数相关的若干条指令，每个表项长度为`16`个字节，存储的是用于做延迟绑定的代码。


# 动态链接程序的执行过程

测试代码：

```
// testa.h
#include <cstdio>

void say_hello();

// testa.c
#include "testa.h"

void say_hello()
{
  printf("Hello, World!\n");
}

// main.c
#include <cstdio>
#include "testa.h"

void local_func()
{
        printf("local_func\n");
}

int main()
{
        local_func();
        say_hello();
        return 0;
}
```

编译链接：

```
#!/bin/bash

# create testa.so
g++ -O2 testa.c -fPIC -shared -o libtesta.so

# create main
g++ main.c -L. -ltesta -o main
```

执行和反编译：

```
export LD_LIBRARY_PATH=./:$PATH
objdump -M intel -S main
```

objdump用法说明：


```
objdump

-d
--disassemble
   Display the assembler mnemonics for the machine instructions from objfile. This option only disassembles those sections which are expected to contain instructions.

-M options
--disassembler-options=options
   Pass target specific information to the disassembler. Only supported on some targets. If it is necessary to specify more than one disassembler option then multiple -M options can be used or can be placed together into a comma separated list.

-S
--source
Display source code intermixed with disassembly, if possible. Implies -d.
```

反编译结果：

```
$ objdump -M intel -S main

main:     file format elf64-x86-64


Disassembly of section .init:

00000000000005f8 <_init>:
 5f8:   48 83 ec 08             sub    rsp,0x8
 5fc:   48 8b 05 e5 09 20 00    mov    rax,QWORD PTR [rip+0x2009e5]        # 200fe8 <__gmon_start__>
 603:   48 85 c0                test   rax,rax
 606:   74 02                   je     60a <_init+0x12>
 608:   ff d0                   call   rax
 60a:   48 83 c4 08             add    rsp,0x8
 60e:   c3                      ret    

Disassembly of section .plt:

0000000000000610 <.plt>:
 610:   ff 35 a2 09 20 00       push   QWORD PTR [rip+0x2009a2]        # 200fb8 <_GLOBAL_OFFSET_TABLE_+0x8>
 616:   ff 25 a4 09 20 00       jmp    QWORD PTR [rip+0x2009a4]        # 200fc0 <_GLOBAL_OFFSET_TABLE_+0x10>
 61c:   0f 1f 40 00             nop    DWORD PTR [rax+0x0]

0000000000000620 <puts@plt>:
 620:   ff 25 a2 09 20 00       jmp    QWORD PTR [rip+0x2009a2]        # 200fc8 <puts@GLIBC_2.2.5>
 626:   68 00 00 00 00          push   0x0
 62b:   e9 e0 ff ff ff          jmp    610 <.plt>

0000000000000630 <_Z9say_hellov@plt>:
 630:   ff 25 9a 09 20 00       jmp    QWORD PTR [rip+0x20099a]        # 200fd0 <_Z9say_hellov>
 636:   68 01 00 00 00          push   0x1
 63b:   e9 d0 ff ff ff          jmp    610 <.plt>

Disassembly of section .plt.got:

0000000000000640 <__cxa_finalize@plt>:
 640:   ff 25 b2 09 20 00       jmp    QWORD PTR [rip+0x2009b2]        # 200ff8 <__cxa_finalize@GLIBC_2.2.5>
 646:   66 90                   xchg   ax,ax

Disassembly of section .text:

0000000000000650 <_start>:
 650:   31 ed                   xor    ebp,ebp
 652:   49 89 d1                mov    r9,rdx
 655:   5e                      pop    rsi
 656:   48 89 e2                mov    rdx,rsp
 659:   48 83 e4 f0             and    rsp,0xfffffffffffffff0
 65d:   50                      push   rax
 65e:   54                      push   rsp
 65f:   4c 8d 05 9a 01 00 00    lea    r8,[rip+0x19a]        # 800 <__libc_csu_fini>
 666:   48 8d 0d 23 01 00 00    lea    rcx,[rip+0x123]        # 790 <__libc_csu_init>
 66d:   48 8d 3d f9 00 00 00    lea    rdi,[rip+0xf9]        # 76d <main>
 674:   ff 15 66 09 20 00       call   QWORD PTR [rip+0x200966]        # 200fe0 <__libc_start_main@GLIBC_2.2.5>
 67a:   f4                      hlt    
 67b:   0f 1f 44 00 00          nop    DWORD PTR [rax+rax*1+0x0]

...

000000000000075a <_Z10local_funcv>:
 75a:   55                      push   rbp
 75b:   48 89 e5                mov    rbp,rsp
 75e:   48 8d 3d af 00 00 00    lea    rdi,[rip+0xaf]        # 814 <_IO_stdin_used+0x4>
 765:   e8 b6 fe ff ff          call   620 <puts@plt>
 76a:   90                      nop
 76b:   5d                      pop    rbp
 76c:   c3                      ret    

000000000000076d <main>:
 76d:   55                      push   rbp
 76e:   48 89 e5                mov    rbp,rsp
 771:   e8 e4 ff ff ff          call   75a <_Z10local_funcv>
 776:   e8 b5 fe ff ff          call   630 <_Z9say_hellov@plt>
 77b:   b8 00 00 00 00          mov    eax,0x0
 780:   5d                      pop    rbp
 781:   c3                      ret    
 782:   66 2e 0f 1f 84 00 00    nop    WORD PTR cs:[rax+rax*1+0x0]
 789:   00 00 00 
 78c:   0f 1f 40 00             nop    DWORD PTR [rax+0x0]

...
```

可以看到，在`000000000000076d <main>`中，调用了`call 630 <_Z9say_hellov@plt>`函数，而`630`的地址是PLT表的一个代码段 (PLT表存储的是代码段)

```
Disassembly of section .plt:

0000000000000610 <.plt>:
 610:   ff 35 a2 09 20 00       push   QWORD PTR [rip+0x2009a2]        # 200fb8 <_GLOBAL_OFFSET_TABLE_+0x8>
 616:   ff 25 a4 09 20 00       jmp    QWORD PTR [rip+0x2009a4]        # 200fc0 <_GLOBAL_OFFSET_TABLE_+0x10>
 61c:   0f 1f 40 00             nop    DWORD PTR [rax+0x0]

0000000000000620 <puts@plt>:
 620:   ff 25 a2 09 20 00       jmp    QWORD PTR [rip+0x2009a2]        # 200fc8 <puts@GLIBC_2.2.5>
 626:   68 00 00 00 00          push   0x0
 62b:   e9 e0 ff ff ff          jmp    610 <.plt>

0000000000000630 <_Z9say_hellov@plt>:
 630:   ff 25 9a 09 20 00       jmp    QWORD PTR [rip+0x20099a]        # 200fd0 <_Z9say_hellov>
 636:   68 01 00 00 00          push   0x1
 63b:   e9 d0 ff ff ff          jmp    610 <.plt>
```

在`630`的代码段可以看到，`jmp QWORD PTR [rip+0x20099a]`汇编指令的注释，该地址为GOT表中的地址，这里又进行了一次跳转。原因是，对于外部共享库，虽然共享库的代码部分的物理内存是共享的，但是数据部分是各个动态链接它的应用程序里各加载一份。因此，所有需要引用共享库外部地址的指令，都会查询GOT表，来找到该函数在当前运行程序的虚拟内存的对应位置。（GOT表存储的是数据段）

GOT表位于数据段，当外部函数第一次被调用时，GOT表保存的并不是该函数实际被加载的内存地址，由于Linux系统使用了延迟绑定技术，因此在首次调用时，该地址需要由动态链接库的`dl_runtime_resolve`函数解析后才能得到。

```
 636:   68 01 00 00 00          push   0x1
 63b:   e9 d0 ff ff ff          jmp    610 <.plt>
```

`610`对应是PLT表的第一项，PLT[0]是一条特殊的记录，其内容为跳转到GOT表中保存了`dl_runtime_resolve`地址的位置。当执行`dl_runtime_resolve`解析出动态库函数的地址后，会将真实的地址写回到GOT表中。


参考：一个共享库函数调用在`PLT+GOT`机制下工作的流程：

![PLT_GOT](/assets/images/202102/PLT_GOT.jpg)


# PLT Hook


## 参考代码

[https://github.com/kubo/plthook](https://github.com/kubo/plthook)

* What is PLTHook.
* A utility library to hook library function calls issued by specified object files (executable and libraries). This modifies PLT (Procedure Linkage Table) entries in ELF format used on most Unixes or IAT (Import Address Table) entries in PE format used on Windows.
* Note that built-in functions cannot be hooked. For example the C compiler in macOS Sierra compiles ceil() as inline assembly code, not as function call of ceil in the system library.

* What is PLT (or IAT) ?
* Note: This isn't precise explanation. Some details are omitted.
* When a function calls another function in another file, it is called via PLT (on Unix using ELF) or IAT (on Windows).
* In order to call foo_func() in libfoo.so, the address of the callee must be known. When callers are in the same file, the relative address to the callee is known at compile time regardless of the absolute address at run time. So some_func() calls foo_func() using relative addressing.
* When callers are in other files, the address of the callee cannot be known at compile time. To resolve it, each file has a mapping from external function names to addresses. The callers directly look at the address in the PLT entry for foo_func() and jump to the address.
* The addresses in PLT entries are resolved (1) at process startup or (2) at first function call (lazy binding). It depends on OSes or on settings.

![figure1](/assets/images/202102/plthook/figure1.png)

* What plthook does.
* Plthook changes the address in PLT entries as above. When foo_func() is called from program, hook_foo_func() is called instead. It doesn't change function calls from libfoo.so and libbar.so.

![figure2](/assets/images/202102/plthook/figure2.png)

* How to call original functions from hook functions.
* When hook functions are outside of modified files
  + When the hook function hook_foo_func() is in libbar.so, just call the original function foo_func(). It looks the PLT entry in libbar.so and jumps to the original.

![figure3](/assets/images/202102/plthook/figure3.png)

* When hook functions are inside of modified files
  + When the hook function hook_foo_func() is in program, do not call the original function foo_func() because it jumps to hook_foo_func() repeatedly and crashes the process after memory for stack is exhausted. You need to get the address of the original function and set it to the function pointer variable foo_func_addr. Use the fourth argument of plthook_replace() to get the address on Windows. Use the return value of dlsym(RTLD_DEFAULT, "foo_func") on Unixes. The fourth argument of plthook_replace() isn't available on Unixes because it doesn't set the address of the original before the address in the PLT entry is resolved.

![figure4](/assets/images/202102/plthook/figure4.png)


## 测试代码

[https://github.com/gerryyang/mac-utils/tree/master/programing/cpp/got_plt](https://github.com/gerryyang/mac-utils/tree/master/programing/cpp/got_plt)


## link_map

`/usr/include/link.h`

* l_addr: Difference between the address in the ELF file and the addresses in memory
* l_ld: Dynamic section of the shared object


```
/* Rendezvous structure used by the run-time dynamic linker to communicate
   details of shared object loading to the debugger.  If the executable's
   dynamic section has a DT_DEBUG element, the run-time linker sets that
   element's value to the address where this structure can be found.  */

struct r_debug
  {
    int r_version;              /* Version number for this protocol.  */

    struct link_map *r_map;     /* Head of the chain of loaded objects.  */

    /* This is the address of a function internal to the run-time linker,
       that will always be called when the linker begins to map in a
       library or unmap it, and again when the mapping change is complete.
       The debugger can set a breakpoint at this address if it wants to
       notice shared object mapping changes.  */
    ElfW(Addr) r_brk;
    enum
      {
        /* This state value describes the mapping change taking place when
           the `r_brk' address is called.  */
        RT_CONSISTENT,          /* Mapping change is complete.  */
        RT_ADD,                 /* Beginning to add a new object.  */
        RT_DELETE               /* Beginning to remove an object mapping.  */
      } r_state;

    ElfW(Addr) r_ldbase;        /* Base address the linker is loaded at.  */
  };

/* This is the instance of that structure used by the dynamic linker.  */
extern struct r_debug _r_debug;

...

struct link_map
  {
    /* These first few members are part of the protocol with the debugger.
       This is the same format used in SVR4.  */

    ElfW(Addr) l_addr;   /* Difference between the address in the ELF
                            file and the addresses in memory.  */
    char *l_name;        /* Absolute file name object was found in.  */
    ElfW(Dyn) *l_ld;     /* Dynamic section of the shared object.  */
    struct link_map *l_next, *l_prev; /* Chain of loaded objects.  */
  };  
```

## PLT Replace

```
int plthook_replace(plthook_t *plthook, const char *funcname, void *funcaddr, void **oldfunc)
{
    size_t funcnamelen = strlen(funcname);
    unsigned int pos = 0;
    const char *name;
    void **addr;
    int rv;

    if (plthook == NULL) {
        set_errmsg("invalid argument: The first argument is null.");
        return PLTHOOK_INVALID_ARGUMENT;
    }
    while ((rv = plthook_enum(plthook, &pos, &name, &addr)) == 0) {
        if (strncmp(name, funcname, funcnamelen) == 0) {
            if (name[funcnamelen] == '\0' || name[funcnamelen] == '@') {
                int prot = get_memory_permission(addr);
                if (prot == 0) {
                    return PLTHOOK_INTERNAL_ERROR;
                }
                if (!(prot & PROT_WRITE)) {
                    if (mprotect(ALIGN_ADDR(addr), page_size, PROT_READ | PROT_WRITE) != 0) {
                        set_errmsg("Could not change the process memory permission at %p: %s",
                                   ALIGN_ADDR(addr), strerror(errno));
                        return PLTHOOK_INTERNAL_ERROR;
                    }
                }
                if (oldfunc) {
                    *oldfunc = *addr;
                }
                *addr = funcaddr;
                if (!(prot & PROT_WRITE)) {
                    mprotect(ALIGN_ADDR(addr), page_size, prot);
                }
                return 0;
            }
        }
    }
    if (rv == EOF) {
        set_errmsg("no such function: %s", funcname);
        rv = PLTHOOK_FUNCTION_NOT_FOUND;
    }
    return rv;
}
```



