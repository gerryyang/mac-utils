

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

# Usage

```
$ ./atomic_test 
test_error_inc:
dec3: val=3428565
dec8: val=4921243
dec2: val=4850203
dec1: val=6334533
dec6: val=4428565
dec4: val=2000000
dec7: val=4749238
dec5: val=2714206
dec9: val=5921243
dec0: val=4850203
test_inc:
inc7: val=7201050
inc4: val=7836520
inc5: val=8252683
inc1: val=10000000
inc2: val=8963049
inc8: val=9323011
inc3: val=9530750
inc6: val=9744199
inc9: val=9769534
inc0: val=10000000
test_add:
add5: val=6877407
add8: val=7832532
add6: val=8304950
add1: val=8849561
add3: val=9174690
add9: val=9443580
add4: val=9677397
add7: val=9848655
add2: val=10000000
add0: val=10000000
test_dec:
dec3: val=1327290
dec9: val=1006510
dec7: val=812365
dec8: val=484468
dec5: val=366002
dec2: val=247463
dec1: val=64841
dec6: val=45496
dec4: val=0
dec0: val=0
```

