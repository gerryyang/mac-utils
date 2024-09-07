# [stack-inspector](https://github.com/sharkdp/stack-inspector)

A `gdb` command to inspect the size of objects on the stack.

![](https://i.imgur.com/uiDEJab.png)

## how to

Use `gdb` to navigate to a certain stack frame (run until your stack overflows or set a breakpoint somewhere). Then, simply run:
```gdb
source stack-inspector.py
stack-inspector
```

## test

``` bash
$ ./build.sh
```

``` bash
$ ./gdb.sh
Reading symbols from ./test...
Breakpoint 1 at 0x401181: file test.cc, line 6.
Starting program: /data/home/gerryyang/jlib_proj/JLib/tools/gdb/stack-inspector/test
main
func3
func2

Breakpoint 1, func1 () at test.cc:6
6           std::cout << __func__ << std::endl;
#0  func1 () at test.cc:6
#1  0x00000000004011cc in func2 () at test.cc:14
#2  0x00000000004011fb in func3 () at test.cc:21
#3  0x0000000000401223 in main () at test.cc:27

  #0   func1() @ test.cc:6

               400   arr1 (std::array<int, 100>)

  #1   func2() @ test.cc:14

               800   arr2 (std::array<int, 200>)

  #2   func3() @ test.cc:21

             1,200   arr3 (std::array<int, 300>)

  #3   main() @ test.cc:27


Total size: 2,400
Missing separate debuginfos, use: dnf debuginfo-install bash-4.4.20-1.tl3.2.x86_64 glibc-2.28-164.tl3.x86_64 libgcc-8.4.1-1.tl3.x86_64 libstdc++-8.4.1-1.tl3.x86_64
(gdb)
```

