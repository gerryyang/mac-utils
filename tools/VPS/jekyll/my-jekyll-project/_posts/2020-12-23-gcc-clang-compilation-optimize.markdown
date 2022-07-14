---
layout: post
title:  "GCC/Clang Compilation Optimize"
date:   2021-02-13 14:00:00 +0800
categories: [GCC/Clang]
---

* Do not remove this line (it will not be displayed)
{:toc}


# 编译优化

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
