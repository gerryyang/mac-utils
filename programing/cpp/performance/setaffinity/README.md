
# Description

```
sched_setaffinity, sched_getaffinity - set and get a thread's CPU affinity mask

A thread's CPU affinity mask determines the set of CPUs on which
it is eligible to run.  On a multiprocessor system, setting the
CPU affinity mask can be used to obtain performance benefits.
For example, by dedicating one CPU to a particular thread (i.e.,
setting the affinity mask of that thread to specify a single CPU,
and setting the affinity mask of all other threads to exclude
that CPU), it is possible to ensure maximum execution speed for
that thread.  Restricting a thread to run on a single CPU also
avoids the performance cost caused by the cache invalidation that
occurs when a thread ceases to execute on one CPU and then
recommences execution on a different CPU.

A CPU affinity mask is represented by the cpu_set_t structure, a
"CPU set", pointed to by mask.  A set of macros for manipulating
CPU sets is described in CPU_SET(3).

```

# Test

First employ `lscpu` to determine that this (x86) system has one cores, each with sixteen CPUs

```
$lscpu | egrep -i 'core.*:|socket'
Thread(s) per core:    1
Core(s) per socket:    16

$lscpu
Architecture:          x86_64
CPU op-mode(s):        32-bit, 64-bit
Byte Order:            Little Endian
CPU(s):                16
On-line CPU(s) list:   0-15
Thread(s) per core:    1
Core(s) per socket:    16
座：                 1
NUMA 节点：         1
厂商 ID：           GenuineIntel
CPU 系列：          6
型号：              85
型号名称：        Intel(R) Xeon(R) Platinum 8255C CPU @ 2.50GHz
步进：              5
CPU MHz：             2494.140
BogoMIPS：            4988.28
超管理器厂商：  KVM
虚拟化类型：     完全
L1d 缓存：          32K
L1i 缓存：          32K
L2 缓存：           4096K
L3 缓存：           36608K
NUMA 节点0 CPU：    0-15
```

Then time the operation of the example program for three cases: 

* both processes running on the same CPU; 
* both processes running on different CPUs on the same core; 
* and both processes running on different CPUs on different cores.

```
$ make

# case 1
$ time -p ./test 0 0 100000000
real 10.05
user 3.91
sys 6.13

# case 2
$ time -p ./test 0 1 100000000
real 4.95
user 3.84
sys 6.01

# case 2
$ time -p ./test 0 2 100000000
real 4.95
user 3.85
sys 6.04
```

```
$pidstat -h
Linux 3.10.107-1-tlinux2_kvm_guest-0049 (qsm_cloud_dev-15)      2021年02月17日  _x86_64_        (16 CPU)

#      Time   UID       PID    %usr %system  %guest    %CPU   CPU  Command
 1613577124  1000       477    0.00    0.00    0.00    0.00     0  test
 1613577124  1000       478    0.00    0.00    0.00    0.00     0  test
```
