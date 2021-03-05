

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
0x7f7ee3825018(0x7f7ee3624516) puts
0x7f7ee3824fe0((nil)) _ITM_deregisterTMCloneTable
0x7f7ee3824fe8((nil)) __gmon_start__
0x7f7ee3824ff0((nil)) _ITM_registerTMCloneTable
0x7f7ee3824ff8(0x7f7ee2e6e520) __cxa_finalize
say_hello
puts_hook, hooked
hello

test demo5
say_world_hook ...
Foo::say_world
call orig func

test end...
```


Refer: 

* https://github.com/kubo/plthook
