



# Usage

```
$ ./atomic_test
test_dec:
dec7: val=622179
dec5: val=369450
dec6: val=283327
dec9: val=213840
dec4: val=129543
dec1: val=0
dec2: val=50793
dec3: val=34450
dec8: val=539851
dec0: val=0
test_inc:
inc7: val1=8019509
inc3: val1=9928810
inc4: val1=8328563
inc9: val1=8544382
inc2: val1=8921042
inc0: val1=8923165
inc8: val1=9064555
inc6: val1=9359599
inc5: val1=9509903
inc1: val1=10913493
inc0: val2=18305900
inc8: val2=18963099
inc9: val2=19492273
inc6: val2=19507456
inc7: val2=19088475
inc5: val2=19419810
inc3: val2=19723118
inc4: val2=19589799
inc2: val2=19895276
inc1: val2=20001000
test_add:
add0: val=5047012
add9: val=7553208
add8: val=7989479
add5: val=8519825
add7: val=8720512
add4: val=9123185
add6: val=9288219
add3: val=9507431
add2: val=9780925
add1: val=10000000

```

# 原子操作

何时需要原子操作？

例如，在并发计数场景，`ADD`操作虽然只有一个指令，但CPU在执行时会分两次去访问内存，将内存的值读出来，执行加操作，然后再写内存。多个CPU同时执行ADD操作时就可能出现相互覆盖的情况，从而导致统计不准确。因此，需要一种原子操作的方式来保证。


* 内存变量的类型一定是`volatile`，目的是使得编译器不对其进行优化，每次访问该变量时都从内存读取数据。
* 写内存的操作有两种
	+ 直接写入（set），对这种情况，无需特殊处理，CPU可以在一个总线周期完成操作，保证操作被原子执行。
	+ 先读后写（add/sub/inc/dec/xchg/xor），这种情况一般原子操作的步骤是：`lock系统总线 --> read --> op --> write --> unlock系统总线`

> 对于锁住系统总线，Intel系列机器提供两种方式的支持：
> 
> 1. xor/xchg的操作自动就是锁住系统总线后操作，用户无需自己触发
> 2. 其它指令用户需要指定lock前缀


在gcc的环境下，可以使用嵌入汇编：

``` c
__asm__ __volatile__( "lock ; addl %1,%0" :"+m" (pstCounter->ptr->dwCounter) :"ir" (val));
```

gcc自4.1以后提供了内置的函数：

``` c
// 返回更新前的值
type __sync_fetch_and_add (type *ptr, type value, ...)
type __sync_fetch_and_sub (type *ptr, type value, ...)
type __sync_fetch_and_or (type *ptr, type value, ...)
type __sync_fetch_and_and (type *ptr, type value, ...)
type __sync_fetch_and_xor (type *ptr, type value, ...)
type __sync_fetch_and_nand (type *ptr, type value, ...)

// 返回更新后的值
type __sync_add_and_fetch (type *ptr, type value, ...)
type __sync_sub_and_fetch (type *ptr, type value, ...)
type __sync_or_and_fetch (type *ptr, type value, ...)
type __sync_and_and_fetch (type *ptr, type value, ...)
type __sync_xor_and_fetch (type *ptr, type value, ...)
type __sync_nand_and_fetch (type *ptr, type value, ...)
```

