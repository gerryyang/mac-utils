

# Usage

```
$make
$export LD_LIBRARY_PATH=.:$

$ ./demo1
Hello, World! hooked
Hello, World!
ubuntu@VM-0-16-ubuntu:~/code/got_plt$ ./demo2
Hello, World! hooked ...
Hello, World!
call orig func
ubuntu@VM-0-16-ubuntu:~/code/got_plt$ ./demo3
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
Hello, World! hooked ...
Hello, World!
call orig func
```


Refer: 

* https://github.com/kubo/plthook
