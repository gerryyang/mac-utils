

# Usage

```
$ bash ./test.sh
test begin...

test demo1
say_hello, hooked
say_hello

test demo2
say_hello, hooked ...
say_hello
call orig func

test demo3
page_size(4096)
name(free)
name(__errno_location)
name(strncmp)
name(fclose)
name(strlen)
name(__stack_chk_fail)
name(strchr)
name(fgets)
name(dlopen)
name(malloc)
name(sscanf)
name(__vsnprintf_chk)
name(dlclose)
name(__printf_chk)
name(mprotect)
name(fopen)
name(say_hello)
say_hello, hooked ...
say_hello
call orig func

test demo4
say_hello
hello
malloc_hook
12
34

test demo5
say_world_hook ...
Foo::say_world
call orig func

test end...
```

On Linux, you can use `readelf --dyn-sym YOUR_PROGRAM_NAME` command to check your module(executable or library) which dynamic functions used.

```
$ readelf --dyn-sym demo4

Symbol table '.dynsym' contains 26 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __printf_chk@GLIBC_2.3.4 (2)
     2: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND say_hello
     3: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (3)
     4: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND plthook_error
     5: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND plthook_open
     6: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND plthook_open_by_address
     7: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND _Znwm@GLIBCXX_3.4 (4)
     8: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND _ZdlPvm@CXXABI_1.3.9 (5)
     9: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __stack_chk_fail@GLIBC_2.4 (6)
    10: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND free@GLIBC_2.2.5 (3)
    11: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND malloc@GLIBC_2.2.5 (3)
    12: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND plthook_close
    13: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND plthook_replace
    14: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.2.5 (3)
    15: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
    16: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND dlsym@GLIBC_2.2.5 (7)
    17: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __libc_start_main@GLIBC_2.2.5 (3)
    18: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
    19: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
    20: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND plthook_enum
    21: 0000000000202010     0 NOTYPE  GLOBAL DEFAULT   23 _edata
    22: 0000000000202020     0 NOTYPE  GLOBAL DEFAULT   24 _end
    23: 00000000000009e0     0 FUNC    GLOBAL DEFAULT   11 _init
    24: 0000000000202010     0 NOTYPE  GLOBAL DEFAULT   24 __bss_start
    25: 0000000000000f24     0 FUNC    GLOBAL DEFAULT   15 _fini
```


Refer: 

* https://github.com/kubo/plthook
* https://github.com/iqiyi/xHook

