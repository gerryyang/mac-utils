---
layout: post
title:  "ELF PLT Hook"
date:   2021-02-13 13:00:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Tips

## 安装和使用动态库

Once you've created a shared library, you'll want to install it. The simple approach is simply to copy the library into one of the standard directories (e.g., `/usr/lib`) and run `ldconfig`.

First, you'll need to create the shared libraries somewhere. Then, you'll need to set up the necessary symbolic links, in particular a link from a `soname` to the real name (as well as from a versionless soname, that is, a soname that ends in `.so` for users who don't specify a version at all). The simplest approach is to run:

```
ldconfig -n directory_with_shared_libraries
```

Finally, when you compile your programs, you'll need to tell the linker about any static and shared libraries that you're using. Use the `-l` and `-L` options for this.

If you can't or don't want to install a library in a standard place (e.g., you don't have the right to modify `/usr/lib`), then you'll need to change your approach. In that case, you'll need to install it somewhere, and then give your program enough information so the program can find the library and there are several ways to do that. You can use gcc's `-L` flag in simple cases. You can use the `rpath` approach, particularly if you only have a specific program to use the library being placed in a `non-standard` place. You can also use environment variables to control things. In particular, you can set `LD_LIBRARY_PATH`, which is a colon-separated list of directories in which to search for shared libraries before the usual places. If you're using bash, you could invoke my_program this way using:

```
LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH  ./my_program
```

## 动态加载共享库（插件场景）

`Dynamically loaded` (DL) libraries are libraries that are loaded at times other than during the startup of a program. They're particularly useful for implementing plugins or modules, because they permit waiting to load the plugin until it's needed.

In Linux, DL libraries aren't actually special from the point-of-view of their format; they are built as standard object files or standard shared libraries as discussed above. The main difference is that the libraries aren't automatically loaded at program link time or start-up; instead, there is an API for opening a library, looking up symbols, handling errors, and closing the library. C users will need to include the header file `<dlfcn.h>` to use this API.

[ld(1) - Linux man page](https://linux.die.net/man/1/ld)

### 关于`-rdynamic`的用途

* [What exactly does `-rdynamic` do and when exactly is it needed?](https://stackoverflow.com/questions/36692315/what-exactly-does-rdynamic-do-and-when-exactly-is-it-needed) 
* [Why do we need -rdynamic option in gcc? ](https://stackoverflow.com/questions/50418941/why-do-we-need-rdynamic-option-in-gcc)

`-rdynamic` exports the symbols of an executable, this mainly addresses scenarios as described in Mike Kinghan's answer, but also it helps e.g. Glibc's `backtrace_symbols()` symbolizing the backtrace.

Symbols are only exported by default from shared libraries. `-rdynamic` tells linker to do the same for `executables`. Normally that's a bad idea but sometimes you want to provide APIs for dynamically loaded plugins and then this comes handy (even though one much better off using [explicit visibility annotations](http://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html), [version script](http://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html) or [dynamic export file](https://www.cs.kent.ac.uk/people/staff/srk21/blog/2011/12/01/) ).

From The Linux Programming Interface:

![rdynamic](/assets/images/202111/rdynamic.png)


#### 示例1: 导出可执行文件中的符号

bar.c

``` cpp
extern void foo();

void bar()
{
    foo();
}
```

main.c

``` cpp
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void foo()
{
        puts("Hello world");
}

int main()
{
        void* dlh = dlopen("./libbar.so", RTLD_NOW);
        if (!dlh) {
                fprintf(stderr, "%s\n", dlerror());
                exit(EXIT_FAILURE);
        }

        void (*bar)(void) = dlsym(dlh, "bar");
        if (!bar) {
                fprintf(stderr, "%s\n", dlerror());
                exit(EXIT_FAILURE);
        }

        bar();
}
```

Makefile

```
.PHONY: all clean test

LDEXTRAFLAGS ?=

all: prog

bar.o: bar.c
        gcc -c -Wall -fpic -o $@ $<

libbar.so: bar.o
        gcc -shared -o $@ $<

main.o: main.c
        gcc -c -Wall -o $@ $<

prog: main.o | libbar.so
        #gcc $(LDEXTRAFLAGS) -o $@ $< -L. -lbar -ldl
        gcc $(LDEXTRAFLAGS) -o $@ $< -ldl

clean:
        rm -f *.o *.so prog

test: prog
        ./$<
```

Here, `bar.c` becomes a shared library `libbar.so` and `main.c` becomes a program that dlopens `libbar` and calls `bar()` from that library. `bar()` calls `foo()`, which is external in `bar.c` and defined in `main.c`.

So, without `-rdynamic`:

```
$ make test
gcc -c -Wall -o main.o main.c
gcc -c -Wall -fpic -o bar.o bar.c
gcc -shared -o libbar.so bar.o
gcc  -o prog main.o -L. -lbar -ldl
./prog
./libbar.so: undefined symbol: foo
Makefile:23: recipe for target 'test' failed
make: *** [test] Error 1
```

And with `-rdynamic`:

```
$ make clean
rm -f *.o *.so prog
$ make test LDEXTRAFLAGS=-rdynamic
gcc -c -Wall -o main.o main.c
gcc -c -Wall -fpic -o bar.o bar.c
gcc -shared -o libbar.so bar.o
gcc -rdynamic -o prog main.o -L. -lbar -ldl
./prog
Hello world
```

#### 示例2: backtrace系统调用

refer: https://www.gnu.org/software/libc/manual/html_node/Backtraces.html

``` cpp
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

/* Obtain a backtrace and print it to stdout. */
void
print_trace (void)
{
  void *array[10];
  char **strings;
  int size, i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);
  if (strings != NULL)
  {

    printf ("Obtained %d stack frames.\n", size);
    for (i = 0; i < size; i++)
      printf ("%s\n", strings[i]);
  }

  free (strings);
}

/* A dummy function to make the backtrace more interesting. */
void
dummy_function (void)
{
  print_trace ();
}

int
main (void)
{
  dummy_function ();
  return 0;
}
```

编译输出，没有使用`-rdynamic`：

```
$ gcc backtrace.c 
$ ./a.out 
Obtained 5 stack frames.
./a.out(+0x7dd) [0x557ae8da77dd]
./a.out(+0x879) [0x557ae8da7879]
./a.out(+0x885) [0x557ae8da7885]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xe7) [0x7fd6a8405bf7]
./a.out(+0x6fa) [0x557ae8da76fa]
```

without -rdynamic:

```
$ readelf --dyn-syms a.out 

Symbol table '.dynsym' contains 12 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND free@GLIBC_2.2.5 (2)
     2: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.2.5 (2)
     4: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND backtrace_symbols@GLIBC_2.2.5 (2)
     5: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND backtrace@GLIBC_2.2.5 (2)
     6: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __stack_chk_fail@GLIBC_2.4 (3)
     7: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND printf@GLIBC_2.2.5 (2)
     8: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __libc_start_main@GLIBC_2.2.5 (2)
     9: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
    10: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
    11: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (2)
```

对比使用`-rdynamic`以后，可以看到符号了。

```
$ gcc -rdynamic backtrace.c 
$ ./a.out 
Obtained 5 stack frames.
./a.out(print_trace+0x28) [0x556f3eb0f9fd]
./a.out(dummy_function+0x9) [0x556f3eb0fa99]
./a.out(main+0x9) [0x556f3eb0faa5]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xe7) [0x7f70cd629bf7]
./a.out(_start+0x2a) [0x556f3eb0f91a]
```

with `-rdynamic`, we have more symbols, including the executable's:

```
$ readelf --dyn-syms a.out 

Symbol table '.dynsym' contains 26 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND free@GLIBC_2.2.5 (2)
     2: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.2.5 (2)
     4: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND backtrace_symbols@GLIBC_2.2.5 (2)
     5: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND backtrace@GLIBC_2.2.5 (2)
     6: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __stack_chk_fail@GLIBC_2.4 (3)
     7: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND printf@GLIBC_2.2.5 (2)
     8: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __libc_start_main@GLIBC_2.2.5 (2)
     9: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
    10: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
    11: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (2)
    12: 0000000000201010     0 NOTYPE  GLOBAL DEFAULT   23 _edata
    13: 0000000000201000     0 NOTYPE  GLOBAL DEFAULT   23 __data_start
    14: 0000000000201018     0 NOTYPE  GLOBAL DEFAULT   24 _end
    15: 0000000000000a90    12 FUNC    GLOBAL DEFAULT   14 dummy_function
    16: 0000000000201000     0 NOTYPE  WEAK   DEFAULT   23 data_start
    17: 0000000000000b30     4 OBJECT  GLOBAL DEFAULT   16 _IO_stdin_used
    18: 0000000000000ab0   101 FUNC    GLOBAL DEFAULT   14 __libc_csu_init
    19: 00000000000008f0    43 FUNC    GLOBAL DEFAULT   14 _start
    20: 0000000000201010     0 NOTYPE  GLOBAL DEFAULT   24 __bss_start
    21: 0000000000000a9c    16 FUNC    GLOBAL DEFAULT   14 main
    22: 0000000000000858     0 FUNC    GLOBAL DEFAULT   11 _init
    23: 0000000000000b20     2 FUNC    GLOBAL DEFAULT   14 __libc_csu_fini
    24: 0000000000000b24     0 FUNC    GLOBAL DEFAULT   15 _fini
    25: 00000000000009d5   187 FUNC    GLOBAL DEFAULT   14 print_trace
```

### Using `ld' linker version script (控制符号导出)

This version file tells the linker, that all symbols `(*)` should be considered as `local symbols` (that is: `hidden`), and all symbols that match the wildcard `foo*` should be considered as `global` (so, `visible`).

**The problem with this approach is that it can't handle some more complicated scenarios, like filtering only some symbols that are using C++ templates.** Some of the template-based symbols in C++ can easily grow up to few hundred characters, but you probably know what I mean. Once you start using functions from `std::`, you'll know.

Please do some reading about the linker's version scripts, because it allows you to perform some really cool things, like symbol versioning!

symbol.version

```
{
    global: foo*;
    local: *;
};
```

或者`C++`的导出函数：例如，只导出`lua`开头的函数。

```
{
global:
    extern "C++" {
        lua*;
    };
local:
    *;
};
```

Makefile

```
.PHONY: all clean test

LDEXTRAFLAGS ?= -rdynamic -Wl,--version-script=symbol.txt

all: prog

bar.o: bar.c
        gcc -c -Wall -fpic -o $@ $<

libbar.so: bar.o
        gcc -shared -o $@ $<

main.o: main.c
        gcc -c -Wall -o $@ $<

prog: main.o | libbar.so
        gcc $(LDEXTRAFLAGS) -o $@ $< -ldl

clean:
        rm -f *.o *.so prog

test: prog
        ./$<
```

refer: https://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html/

## 使用`LD_DEBUG`环境变量查看某程序加载so的过程

```
# 查看帮助
LD_DEBUG=help ./bin

# display library search paths
LD_DEBUG=libs ./bin

# 将信息输出到log中
LD_DEBUG=libs LD_DEBUG_OUTPUT=log ./bin
```

## 使用`rpath`编译时指定动态库搜索路径

During development, there's the potential problem of modifying a library that's also used by many other programs -- and **you don't want the other programs to use the developmental library**, only a particular application that you're testing against it. One link option you might use is ld's `rpath` option, which specifies the runtime library search path of that particular program being compiled. **From gcc, you can invoke the rpath option by specifying it this way**:

```
-Wl,-rpath,$(DEFAULT_LIB_INSTALL_PATH)
```

If you use this option when building the library client program, you don't need to bother with `LD_LIBRARY_PATH` other than to ensure it's not conflicting, or using other techniques to hide the library.

## LD_PRELOAD

在GNU C Library的主要开发成员Ulrich Drepper写的[How to Write Shared Libraries, 2011](http://www.akkadia.org/drepper/dsohowto.pdf)中有如下描述：

> scope中包含两个以上的同名标识符的定义也没有关系。标识符查找的算法，只需采用最先发现的来定义就好了，这种概念非常有用。使用`LD_PRELOAD`的功能就是其中的一例。

这样一来，无论是静态链接库，还是共享库，即时定义了同名标识符，程序也可以正常的进行链接和运行。在进行这个操作时可能会发生**“莫名其妙地调用非预期的函数“**这样的bug，所以必须引起注意。**解决方法是：使用命名空间来避免冲突。**

### 用`LD_PRELOAD`更换共享库

通过将共享对象指定为环境变量`LD_PRELOAD`并运行，先链接`LD_PRELOAD`的共享对象。

If you want to **override just a few selected functions, you can do this by creating an overriding object file and setting `LD_PRELOAD`; the functions in this object file will override just those functions (leaving others as they were)**.


例子：

```
$ hostname
VM-0-16-ubuntu

$ ltrace hostname
rindex("hostname", '/')                                                                             = nil
strcmp("hostname", "domainname")                                                                    = 4
strcmp("hostname", "ypdomainname")                                                                  = -17
strcmp("hostname", "nisdomainname")                                                                 = -6
getopt_long(1, 0x7ffc89e23d28, "aAdfbF:h?iIsVy", 0x55fc0d41faa0, nil)                               = -1
__errno_location()                                                                                  = 0x7fae1faed480
malloc(128)                                                                                         = 0x55fc0d831260
gethostname("VM-0-16-ubuntu", 128)                                                                  = 0
memchr("VM-0-16-ubuntu", '\0', 128)                                                                 = 0x55fc0d83126e
puts("VM-0-16-ubuntu"VM-0-16-ubuntu
)                                                                              = 15
+++ exited (status 0) +++
```

```
$ nm -D /bin/hostname |grep gethostname
                 U gethostname

$ ldd /bin/hostname 
        linux-vdso.so.1 (0x00007fff071f5000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fc62282b000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fc622e21000)

$ nm -D /lib/x86_64-linux-gnu/libc.so.6 | grep hostname
0000000000116e30 W gethostname
...
```

测试代码：

``` c
// gethostname.c

#include <stdlib.h>
#include <string.h>

int gethostname(char *name, size_t len)
{
        char *p = getenv("FAKE_HOSTNAME");
        if (p == NULL) {
                p = "localhost";
        }
        strncpy(name, p, len - 1);
        name[len - 1] = '\0';

        return 0;
}
```

输出：

``` bash
#!/bin/bash

# origin
hostname

# hook
FAKE_HOSTNAME=gerryyang.com LD_PRELOAD=./gethostname.so hostname
```

### 用`LD_PRELOAD`来lap既存的函数

使用handle `RTLD_NEXT`，用dlsym调出原始的调用函数。handle是`RTLD_NEXT`扩展的特殊代名，在共享对象的下一个共享对象以后取得寻找符号值。`RTLD_NEXT`是GNU的扩展，在包含`dlfcn.h`之前有必要先定义`GNU_SOURCE`。

``` c
#define _GNU_SOURCE
#include <dlfcn.h>

static int (*bind0)(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);

...

bind0 = dlsym(RTLD_NEXT, "bind");
```

使用方法：

``` sh
$ LD_PRELOAD=./bindwrap.so BIND_ADDR=127.0.0.1 daemon-program
```


## 使用`ldd`查看共享库依赖

* 使用`objdump -p`和`readelf -d`可以查询共享库的依赖关系（通过动态节的NEEDED），但是要查看和动态库相关的全部依赖关系，就比较麻烦。
* 在GUN/Linux里，`ldd`实际上仅是shell脚本，若将环境变量`LD_TRACE_LOADED_OBJECTS`设置为1后执行程序，解释器（ld-linux-x86-64.so.2）将在执行实际的程序之前查看程序必要的共享库，将其载入内存并把它的信息显示出来。因此，不用ldd，只用环境变量`LD_TRACE_LOADED_OBJECTS`也可以得到同样的结果。

```
# /usr/bin/ldd

# This is the `ldd' command, which lists what shared libraries are
# used by given dynamically-linked executables.  It works by invoking the
# run-time dynamic linker as a command and setting the environment
# variable LD_TRACE_LOADED_OBJECTS to a non-empty value.  
```

```
$ ldd demo1
        linux-vdso.so.1 (0x00007fff6e92c000)
        libtesta.so => ./libtesta.so (0x00007ff426f1e000)
        libplthook.so => ./libplthook.so (0x00007ff426d1a000)
        libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007ff426b16000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007ff426725000)
        /lib64/ld-linux-x86-64.so.2 (0x00007ff427323000)

$ LD_TRACE_LOADED_OBJECTS=1 demo1
        linux-vdso.so.1 (0x00007ffc55f6d000)
        libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f419dffc000)
        libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f419ddf8000)
        libutil.so.1 => /lib/x86_64-linux-gnu/libutil.so.1 (0x00007f419dbf5000)
        libexpat.so.1 => /lib/x86_64-linux-gnu/libexpat.so.1 (0x00007f419d9c3000)
        libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f419d7a6000)
        libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f419d408000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f419d017000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f419e21b000)
```


You can see the list of the shared libraries used by a program using `ldd`. So, for example, you can see the shared libraries used by ls by typing:

```
ldd /bin/ls
```

Generally you'll see a list of the sonames being depended on, along with the directory that those names resolve to. In practically all cases you'll have at least two dependencies:

```
/lib/ld-linux.so.N (where N is 1 or more, usually at least 2). This is the library that loads all other libraries.

libc.so.N (where N is 6 or more). This is the C library. Even other languages tend to use the C library (at least to implement their own libraries), so most programs at least include this one.
```

Beware: do not run `ldd` on a program you don't trust. As is clearly stated in the `ldd` manual, ldd works by (in certain cases) by setting a special environment variable (for ELF objects, LD_TRACE_LOADED_OBJECTS) and then executing the program. It may be possible for an untrusted program to force the ldd user to run arbitrary code (instead of simply showing the ldd information). So, for safety's sake, don't use ldd on programs you don't trust to execute.

## C/C++共享库不兼容的情况

When a new version of a library is binary-incompatible with the old one the soname needs to change. In C, there are four basic reasons that a library would cease to be binary compatible:

1. The behavior of a function changes so that it no longer meets its original specification,
2. Exported data items change (exception: adding optional items to the ends of structures is okay, as long as those structures are only allocated within the library).
3. An exported function is removed.
4. The interface of an exported function changes.

If you can avoid these reasons, you can keep your libraries binary-compatible. Said another way, you can keep your `Application Binary Interface (ABI)` compatible if you avoid such changes. For example, you might want to add new functions but not delete the old ones. You can add items to structures but only if you can make sure that old programs won't be sensitive to such changes by adding items only to the end of the structure, only allowing the library (and not the application) to allocate the structure, making the extra items optional (or having the library fill them in), and so on. Watch out - you probably can't expand structures if users are using them in arrays.

For C++ (and other languages supporting compiled-in templates and/or compiled dispatched methods), the situation is trickier. All of the above issues apply, plus many more issues. The reason is that some information is implemented **under the covers** in the compiled code, resulting in dependencies that may not be obvious if you don't know how C++ is typically implemented. Strictly speaking, they aren't **new issues**, it's just that compiled C++ code invokes them in ways that may be surprising to you. The following is a (probably incomplete) list of things that you can and can't do in C++ and retain binary compatibility (these were originally reported by **Troll Tech's Technical FAQ; a more up-to-date list is in KDE's Policies/Binary Compatibility Issues With C++**):

1. add reimplementations of virtual functions (unless it it safe for older binaries to call the original implementation), because the compiler evaluates SuperClass::virtualFunction() calls at compile-time (not link-time).
2. add or remove virtual member functions, because this would change the size and layout of the vtbl of every subclass.
3. change the type of any data members or move any data members that can be accessed via inline member functions.
4. change the class hierarchy, except to add new leaves.
5. add or remove private data members, because this would change the size and layout of every subclass.
6. remove public or protected member functions unless they are inline.
7. make a public or protected member function inline.
8. change what an inline function does, unless the old version continues working.
9. change the access rights (i.e. public, protected or private) of a member function in a portable program, because some compilers mangle the access rights into the function name.

Given this lengthy list, developers of C++ libraries in particular must plan for more than occasional updates that break binary compatibility. Fortunately, on Unix-like systems (including Linux) you can have multiple versions of a library loaded at the same time, so while there is some disk space loss, users can still run old programs needing old libraries.


## 使用`nm`查找符号 

The `nm` command can report the list of symbols in a given library. It works on both static and shared libraries. For a given library nm can list the symbol names defined, each symbol's value, and the symbol's type. It can also identify where the symbol was defined in the source code (by filename and line number), if that information is available in the library (see the -l option).

The symbol type requires a little more explanation. The type is displayed as a letter; lowercase means that the symbol is local, while uppercase means that the symbol is global (external). Typical symbol types include `T` (a normal definition in the code section), `D` (initialized data section), `B` (uninitialized data section), `U` (undefined; the symbol is used by the library but not defined by the library), and `W` (weak; if another library also defines this symbol, that definition overrides this one).

If you know the name of a function, but you truly can't remember what library it was defined in, you can use nm's `-o` option (which prefixes the filename in each line) along with grep to find the library name. From a Bourne shell, you can search all the libraries in /lib, /usr/lib, direct subdirectories of /usr/lib, and /usr/local/lib for `cos` as follows:

```
nm -o /lib/* /usr/lib/* /usr/lib/*/* \
      /usr/local/lib/* 2> /dev/null | grep 'cos$' 
```

## 生成大的共享库

What if you want to first create smaller libraries, then later merge them into larger libraries? In this case, you may find ld's `--whole-archive` option useful, which can be used to forcibly bring `.a` files and link them into an `.so` file.

Here's an example of how to use `--whole-archive`:

```
gcc -shared -Wl,-soname,libmylib.$(VER) -o libmylib.so $(OBJECTS) \
            -Wl,--whole-archive $(LIBS_TO_LINK) -Wl,--no-whole-archive $(REGULAR_LIBS)
```

As the `ld` documentation notes, be sure to use `--no-whole-archive` option at the end, or gcc will try to merge in the standard libraries as well. 


## 共享库为什么要用PIC编译

``` 
# 不使用PIC
gcc -o fpic-no-pic.s -S fpic.c

# 使用PIC
gcc -fPIC -o fpic-pic.s -S fpic.c
```

建立共享库：

```
gcc -shared -o fpic-no-pic.so fpic.c

gcc -shared -fPIC -o fpic-pic.so fpic.c
```

* 用PIC编译必须把`-fpic`或`-fPIC`传递给gcc。`-fpic`可以生成小而高效的代码，但是不同的处理器中`-fpic`生成的GOT（Global Offset Table，全局偏移表）的大小有限制。
* 通过生成的汇编代码，可知道PIC版通过`PLT（Procedure Linkage Table）`调用`printf`。 

* `-fPIC`参数作用于**编译阶段**，是告诉编译器生成与位置无关（Position Independent Code）的代码。
* 对于共享库来说，如果不加`-fPIC`，则`.so`文件的代码段在被加载时，代码段引用的数据对象需要重新定位，重新定位会修改代码段的内容，这就造成每个使用这个`.so`文件代码段的进程在内核中都需要生成这个`.so`文件代码段的副本，每个副本都不一样，具体取决于这个`.so`文件代码和数据段内存映射的位置。
* 当添加`-fPIC`参数后，则产生的代码中，没有绝对地址，全部使用相对地址，故而代码可以被加载器加载到内存的任意位置，都可以正确执行。

**结论：虽然也能建立非PIC的共享库，但运行时的再配置不但花费时间，还存在不能和其他路径代码（.text）共享这一大缺点。因此，建立共享库请用PIC编译c文件。**



## 查看是否使用了PIC

检查目标文件在编译时是否使用了`–fPIC`选项，即检查目标文件符号表中是否存在名称`_GLOBAL_OFFSET_TABLE_`。

```
# 没有使用
$nm -s foo.o |grep "_GLOBAL" 
                 U _GLOBAL_OFFSET_TABLE_

$readelf -s foo.o | grep "_GLOBAL"
   219: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND _GLOBAL_OFFSET_TABLE_

# 有使用
$ nm -s libhello.so | grep GLOBAL
0000000000201000 d _GLOBAL_OFFSET_TABLE_

$ readelf -s libhello.so | grep GLOBAL_
    46: 0000000000201000     0 OBJECT  LOCAL  DEFAULT   21 _GLOBAL_OFFSET_TABLE_
```

 

# 静态链接

* 静态链接库，是包含了各种程序中所使用的函数等模块的各个目标文件，这样一个集合。即，静态库是多个目标文件的存档。
* `ar`命令
  + 可以把多个目标文件归纳整理成一个文件：`ar rcus libhoge.a foo.o bar.o baz.o`
  + 可以查看库的内容：`ar tv libhoge.a`
  + 可以展开存档：`ar xv libhoge.a`
* 静态链接过程：链接器，先从其他目标文件中查找未定义的符号，再从指定的静态链接库中读取定义符号的目标文件的副本，加入到可执行文件中，完成链接。
* 使用静态链接库，执行生成的可执行文件时，即时没有静态链接库也可以正常执行，因为必要的代码副本包含在可执行二进制文件里了。
* 选项`s`可与`ranlib`进行相同处理并建立索引。若不建立索引，链接的速度就会降低，在不同的环境中可能会产生不同的错误。这种索引可用`nm -s`浏览。
* 注意：操作是以库中的目标文件为单位进行的。

静态链接库的编写如下：

```
# 生成静态库
cc -c -o foo.o foo.c
cc -c -o bar.o bar.c
ar ruv libfoo.a foo.o bar.o

# 查看静态库
$ ar tv libfoo.a 
rw-r--r-- 0/0  12504 Jan  1 08:00 1970 foo.o
```

选项解释：


```
r   Insert the files member... into archive (with replacement).

c   Create the archive.

s   Add an index to the archive, or update it if it already exists.

u   Normally, ar r... inserts all files listed into the archive.  If you would like to insert only those of the files you list that are newer than existing members of the same names, use this modifier.  The u modifier is allowed only for the operation r (replace).  In particular, the combination qu is not allowed, since checking the timestamps would lose any speed advantage from the operation q.

v   This modifier requests the verbose version of an operation.  Many operations display additional information, such as filenames processed, when the modifier v is appended.

x   Extract members (named member) from the archive.
```


# 动态链接

* 静态库是多个目标文件的存档，而动态库则把多个目标文件复制成一个巨大的目标文件中进行共享。
* ELF支持**动态链接**，当一个程序被加载进内存时，动态链接器会把需要的共享库加载并绑定到该进程的地址空间中。随后在调用某个函数时，对该函数地址进行解析，以达到对该函数调用的目的。

静态链接库的编写如下：

```
cc -fPIC -c -o foo.o foo.c
cc -fPIC -c -o bar.o bar.c
cc -shared -Wl,-soname,libfoo.so.0 -o libfoo.so.0.0 foo.o bar.o

# equal to `ln -sf libhello.so.0.0 libhello.so.0` but let's let ldconfig figure it out
/sbin/ldconfig -n .
ln -sf libhello.so.0 libhello.so
```

* 加入`-shared`选项，生成共享目标。
* 通过`-Wl,-soname`选项，指定该共享目标的`SONAME`
* 注意，操作是以共享库为单位进行的。链接时，只要把需要的共享库`SONAME`作为`NEEDED`登记到可执行文件里就可以了。
* 执行链接里共享库的可执行文件时，动态链接加载器使用`NEEDED`的信息找到必要的共享库，执行的时候，操作该进程中的内存映射，可以在同样的空间里使用共享库和可执行二进制文件。因为实际的库代码没有包含在可执行文件中，必须保证在系统中已经存在共享库。


# ELF (Executable and Linkable Format)

![ELF_Executable_and_Linkable_Format_diagram_by_Ange_Albertini](/assets/images/202102/ELF_Executable_and_Linkable_Format_diagram_by_Ange_Albertini.png)

> An executable file using the ELF file format consists of `an ELF header`, followed by `a program header table` or `a section header table`, or both. 
>
> `The ELF header` is always at offset zero of the file. `The program header table` and `the section header table`'s offset in the file are defined in the ELF header. The two tables describe the rest of the particularities of the file.

* **ELF header (Ehdr)**
  + The ELF header is described by the type `Elf32_Ehdr` or `Elf64_Ehdr`
* **Program header (Phdr)**
  + An executable or shared object file's program header table is an array of structures, each describing a segment or other information the system needs to prepare the program for execution. An object file segment contains one or more sections. Program headers are meaningful only for executable and shared object files. A file specifies its own program header size with the ELF header's e_phentsize and e_phnum members. The ELF program header is described by the type `Elf32_Phdr` or `Elf64_Phdr` depending on the architecture
* **Section header (Shdr)**
  + A file's section header table lets one locate all the file's sections. The section header table is an array of `Elf32_Shdr` or `Elf64_Shdr` structures. The ELF header's `e_shoff` member gives the byte offset from the beginning of the file to the section header table.  `e_shnum` holds the number of entries the section header table contains. `e_shentsize` holds the size in bytes of each entry.


ELF文件参与程序的**链接**和程序的**执行**，因此通常可以分别从**可链接文件角度**和**可执行文件角度**来看待ELF文件的格式。

* 对于**编译链接**，则编译器和链接器把ELF文件看作是**节头表描述的节集合，而程序头表可选**
* 对于**加载执行**，则加载器把ELF文件看作是**程序头表描述的段的集合，而节头表可选**
* 如果是共享文件，则两者都包含

![elf_view](/assets/images/202102/elf_view.png)

动态链接和动态加载：

* 为了解决静态链接空间浪费和程序更新困难的问题，通常会将程序模块互相分割，形成独立的文件（动态库），而不是将它们静态链接在一起。
* 动态链接过程
  + 操作系统在加载一个可执行的ELF文件，内核首先将ELF映像加载到用户空间虚拟内存，在加载完动态链接器后（lib/ld-linux.so），系统会读取`.dynamic`段中的`DT_NEEDED`条目，该条目中列出该可执行文件所依赖的共享库，之后动态链接器就会去依次加载这些共享库，共享库本身依赖了其他库，这些库也会被加载。
  + 当所有的共享库都被加载完毕后，就开始执行**重定位的流程**。重定位是通过**过程链接表（PLT）和 全局偏移表（GOT）**的间接机制来处理的。这些表提供了外部函数和数据的地址，动态链接器需要根据全局符号表重定位每一个符号的地址，即修正GOT和PLT。


## 用readelf表示ELF文件的信息

* 读出ELF头

| 需要的头 | 选项 | long option
| --- | --- | ---
| ELF文件头 | -h | --file-header
| 程序头 | -l | --program-headers, --segments
| 节头 | -S | --section-headers, --sections
| 以上3个头 | -e | --headers

* 读出ELF信息

| 需要的信息 | 选项 | long option
| --- | --- | ---
| 符号表 | -s | --syms, -symbols
| 再配置信息 | -r | --relocs
| 动态段 | -d | --dynamic
| 版本信息 | -V | --version-info
| 依赖设计 | -A | --arch-specific
| 长的直方图 | -I | --histogram
| 所有的头和以上全部 | -a | --all
| 核标记 | -n | --notes
| unwind信息 | -u | --unwind


## Various sections hold program and control information

* `.bss`
  + This section holds uninitialized data that contributes to the program's memory image.

* `.data`
  + This section holds initialized data that contribute to the program's memory image.

* `.debug`
  + This section holds information for symbolic debugging. The contents are unspecified. 

* `.dynamic`
  + This section holds dynamic linking information.

* `.dynstr`
  + This section holds strings needed for dynamic linking, most commonly the strings that represent the names associated with symbol table entries.  This section is of type `SHT_STRTAB`.

* `.dynsym`
  + This section holds the dynamic linking symbol table. This section is of type `SHT_DYNSYM`. 

* `.got `
  + This section holds the global offset table. This section is of type `SHT_PROGBITS`. The attributes are processor-specific.

* `.plt`
  + This section holds the procedure linkage table. This section is of type `SHT_PROGBITS`. The attributes are processor-specific.

* `.symtab`
  + This section holds a symbol table.

* `.text`
  + This section holds the "text", or executable instructions, of a program.  This section is of type `SHT_PROGBITS`.

...


## Dynamic tags (Dyn)

The `.dynamic` section contains a series of structures that hold relevant dynamic linking information. The `d_tag` member controls the interpretation of `d_un`.

``` c
typedef struct {
   Elf32_Sword    d_tag;
   union {
       Elf32_Word d_val;
       Elf32_Addr d_ptr;
   } d_un;
} Elf32_Dyn;
extern Elf32_Dyn _DYNAMIC[];

typedef struct {
   Elf64_Sxword    d_tag;
   union {
       Elf64_Xword d_val;
       Elf64_Addr  d_ptr;
   } d_un;
} Elf64_Dyn;
extern Elf64_Dyn _DYNAMIC[];
```

```
DT_NEEDED

This element holds the string table offset of a null-terminated string, giving the name of
a needed library. The offset is an index into the table recorded in the DT_STRTAB
entry. See ‘‘Shared Object Dependencies’’ for more information about these names.
The dynamic array may contain multiple entries with this type. These entries’ relative
order is significant, though their relation to entries of other types is not.


DT_JMPREL

If present, this entries’s d_ptr member holds the address of relocation entries associated solely with the procedure linkage table. 
Separating these relocation entries lets the
dynamic linker ignore them during process initialization, if lazy binding is enabled. If
this entry is present, the related entries of types DT_PLTRELSZ and DT_PLTREL must
also be present.

...
```

例如：

`DT_NEEDED`表示一个列表，列表里面以（NEEDED）为标志的项，就是当前库加载时要依赖的其它库，可以使用`ldd`或`readelf`查看。

```
$ ldd demo1
        linux-vdso.so.1 (0x00007ffdab7fa000)
        libtesta.so => ./libtesta.so (0x00007f5a2fd44000)
        libplthook.so => ./libplthook.so (0x00007f5a2fb40000)
        libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f5a2f93c000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f5a2f54b000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f5a30149000)

$ readelf -d demo1

Dynamic section at offset 0x1d58 contains 30 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libtesta.so]
 0x0000000000000001 (NEEDED)             Shared library: [libplthook.so]
 0x0000000000000001 (NEEDED)             Shared library: [libdl.so.2]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x000000000000000c (INIT)               0x828
 0x000000000000000d (FINI)               0xb94
 0x0000000000000019 (INIT_ARRAY)         0x201d48
 0x000000000000001b (INIT_ARRAYSZ)       8 (bytes)
 0x000000000000001a (FINI_ARRAY)         0x201d50
 0x000000000000001c (FINI_ARRAYSZ)       8 (bytes)
 0x000000006ffffef5 (GNU_HASH)           0x298
 0x0000000000000005 (STRTAB)             0x4b0
 0x0000000000000006 (SYMTAB)             0x2d0
 0x000000000000000a (STRSZ)              339 (bytes)
 0x000000000000000b (SYMENT)             24 (bytes)
 0x0000000000000015 (DEBUG)              0x0
 0x0000000000000003 (PLTGOT)             0x201f78
 0x0000000000000002 (PLTRELSZ)           216 (bytes)
 0x0000000000000014 (PLTREL)             RELA
 0x0000000000000017 (JMPREL)             0x750
 0x0000000000000007 (RELA)               0x690
 0x0000000000000008 (RELASZ)             192 (bytes)
 0x0000000000000009 (RELAENT)            24 (bytes)
 0x000000000000001e (FLAGS)              BIND_NOW
 0x000000006ffffffb (FLAGS_1)            Flags: NOW PIE
 0x000000006ffffffe (VERNEED)            0x630
 0x000000006fffffff (VERNEEDNUM)         2
 0x000000006ffffff0 (VERSYM)             0x604
 0x000000006ffffff9 (RELACOUNT)          3
 0x0000000000000000 (NULL)               0x0
```

Refer: 

* [Executable and Linkable Format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)
* [linux/include/uapi/linux/elf.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/elf.h)
* [Executable and Linkable Format (ELF)](http://www.skyfree.org/linux/references/ELF_Format.pdf)
* [elf(5) — Linux manual page](https://man7.org/linux/man-pages/man5/elf.5.html)

# GOT/PLT

* 全局偏移表GOT (`The Global Offset Table`)
  + 实现位置无关代码PIC（Position Independent Code）的必要数据，在PIC中，使用GOT间接引用对全局数据进行存取。
  + 在程序中以`.got.plt`表示，该表处于**数据段**，每一个表项存储的都是一个地址，每个表项长度是当前程序的对应需要寻址长度（32位程序：4字节，64位程序：8字节）。
  + 问题：如果一个引用的函数是在共享库中，而共享库在加载时没有固定地址，所以在GOT表中无法直接保存该符号的地址，此时就需要引入PLT表。

* 程序链接表PLT (`The Procedure Linkage Table`)
  + 实现动态链接的必要数据，与GOT同时使用，间接调用动态链接的共享库的函数。
  + 在程序中以`.plt`节表示，该表处于**代码段**，每一个表项表示了一个与要重定位的函数相关的若干条指令，每个表项长度为`16`个字节，存储的是用于做延迟绑定的代码。


# 动态链接程序的执行过程

测试代码：

``` c
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

[https://github.com/gerryyang/mac-utils/tree/master/programing/cpp/hook/got_plt](https://github.com/gerryyang/mac-utils/tree/master/programing/cpp/hook/got_plt)


## link_map

`/usr/include/link.h`

* l_addr: Difference between the address in the ELF file and the addresses in memory
* l_ld: Dynamic section of the shared object


``` cpp
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

``` cpp
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

# Refer

* [Linux下ELF共享库使用摘记](http://blog.csdn.net/delphiwcdj/article/details/43647435)
* [Controlling Symbol Visibility 在C/C++中控制符号的可见性](http://blog.csdn.net/delphiwcdj/article/details/45225889)
* [How to Write Shared Libraries by Ulrich Drepper, 2011](http://www.akkadia.org/drepper/dsohowto.pdf)
* [A Whirlwind Tutorial on Creating Really Teensy ELF Executables for Linux](http://www.muppetlabs.com/~breadbox/software/tiny/teensy.html)


* [Controlling Symbol Visibility](https://developer.apple.com/library/archive/documentation/DeveloperTools/Conceptual/CppRuntimeEnv/Articles/SymbolVisibility.html) (推荐)
* [Dynamic Library Programming Topics](https://developer.apple.com/library/archive/documentation/DeveloperTools/Conceptual/DynamicLibraries/000-Introduction/Introduction.html#//apple_ref/doc/uid/TP40001869)
* [Control over symbol exports in GCC](https://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html/)
* [Weak dynamic symbols](https://www.humprog.org/~stephen//blog/2011/12/01/)