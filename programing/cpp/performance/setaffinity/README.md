
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

```
$ make
$ time -p ./test 0 0 10000000
real 4.93
user 2.00
sys 2.85
```
