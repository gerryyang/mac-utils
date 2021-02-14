

# Usage

```
$make
$export LD_LIBRARY_PATH=.:$

$ ./demo1
say_hello, hooked
say_hello
$ ./demo2
say_hello, hooked ...
say_hello
call orig func
$ ./demo3
page_size(4096)
name(free)
name(__errno_location)
name(strncmp)
name(fclose)
name(strlen)
name(__stack_chk_fail)
name(strchr)
name(fgets)
name(malloc)
name(sscanf)
name(__vsnprintf_chk)
name(__printf_chk)
name(mprotect)
name(fopen)
name(say_hello)
say_hello, hooked ...
say_hello
call orig func
$ ./demo4
0x7f8c613b4018(0x7f8c611b3516) puts
0x7f8c613b3fe0((nil)) _ITM_deregisterTMCloneTable
0x7f8c613b3fe8((nil)) __gmon_start__
0x7f8c613b3ff0((nil)) _ITM_registerTMCloneTable
0x7f8c613b3ff8(0x7f8c609fd520) __cxa_finalize
say_hello
puts_hook, hooked
hello
```


Refer: 

* https://github.com/kubo/plthook
