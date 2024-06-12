
# heap profile

编译动态连接

```
$ g++ -g -std=c++11 test.cc -L../lib -ltcmalloc_and_profiler
$ HEAPPROFILE=heapprofile.out ./a.out
```

非侵入式使用

```
$ g++ -g -std=c++11 test.cc
$ HEAPPROFILE=heapprofile.out LD_PRELOAD=../lib/libtcmalloc_and_profiler.so ./a.out

# 每分配 1MB 生成一个文件
# Dump heap profiling information each time the specified number of bytes has been allocated by the program.
$ HEAPPROFILE=heapprofile.out HEAP_PROFILE_ALLOCATION_INTERVAL=1048576 LD_PRELOAD=../lib/libtcmalloc_and_profiler.so ./a.out
```

dlopen 的方式参考 test2.cc

```
$ g++ -g -std=c++11 test2.cc -ldl
$ LD_PRELOAD=../lib/libtcmalloc.so ./a.out
```

fork 的方式

```
$ g++ -g -std=c++11 test3.cc
HEAPPROFILE=heapprofile.out LD_PRELOAD=../lib/libtcmalloc_and_profiler.so ./a.out
```

dlopen 的方式参考 test4.cc

```
$ g++ -g -std=c++11 test4.cc -ldl
$ LD_PRELOAD=../lib/libtcmalloc.so ./a.out
```

生成 svg 文件

```
../bin/pprof --svg a.out heapprofile.out.0001.heap > heap.svg

# 比较差异
../bin/pprof --svg a.out --base=heapprofile.out.0001.heap heapprofile.out.0002.heap > heap.diff.svg
```

# cpu profile

fork 的方式

```
$ g++ -g -std=c++11 test3.cc
$ CPUPROFILE=cpuprofile.out LD_PRELOAD=../lib/libtcmalloc_and_profiler.so ./a.out
```

生成 svg 文件

```
$ pprof --svg a.out cpuprofile.out > cpu.svg
```

# MALLOCSTATS (内存使用统计)

```
[gerryyang ~/mytest/gperftools-2.10 17:13:31]$ MALLOCSTATS=1 LD_PRELOAD=../lib/libtcmalloc.so ./a.out
Child process (pid: 3763949): shared_var = 42
Child process (pid: 3763949): shared_var changed to 13
Starting tracking the heap
PROFILE: interrupts/evictions/bytes = 52/0/2152
Dumping heap profile to heap.prof.0001.heap (over)
------------------------------------------------
MALLOC:       16798072 (   16.0 MiB) Bytes in use by application
MALLOC: +     11403264 (   10.9 MiB) Bytes in page heap freelist
MALLOC: +       107856 (    0.1 MiB) Bytes in central cache freelist
MALLOC: +            0 (    0.0 MiB) Bytes in transfer cache freelist
MALLOC: +         2360 (    0.0 MiB) Bytes in thread cache freelists
MALLOC: +      2621440 (    2.5 MiB) Bytes in malloc metadata
MALLOC:   ------------
MALLOC: =     30932992 (   29.5 MiB) Actual memory used (physical + swap)
MALLOC: +            0 (    0.0 MiB) Bytes released to OS (aka unmapped)
MALLOC:   ------------
MALLOC: =     30932992 (   29.5 MiB) Virtual address space used
MALLOC:
MALLOC:           2065              Spans in use
MALLOC:              2              Thread heaps in use
MALLOC:           8192              Tcmalloc page size
------------------------------------------------
Call ReleaseFreeMemory() to release freelist memory to the OS (via madvise()).
Bytes released to the OS take up virtual address space but no physical memory.
Parent process (pid: 3763948): shared_var = 42
------------------------------------------------
MALLOC:          20632 (    0.0 MiB) Bytes in use by application
MALLOC: +       974848 (    0.9 MiB) Bytes in page heap freelist
MALLOC: +        52888 (    0.1 MiB) Bytes in central cache freelist
MALLOC: +            0 (    0.0 MiB) Bytes in transfer cache freelist
MALLOC: +          208 (    0.0 MiB) Bytes in thread cache freelists
MALLOC: +      2490368 (    2.4 MiB) Bytes in malloc metadata
MALLOC:   ------------
MALLOC: =      3538944 (    3.4 MiB) Actual memory used (physical + swap)
MALLOC: +            0 (    0.0 MiB) Bytes released to OS (aka unmapped)
MALLOC:   ------------
MALLOC: =      3538944 (    3.4 MiB) Virtual address space used
MALLOC:
MALLOC:             10              Spans in use
MALLOC:              2              Thread heaps in use
MALLOC:           8192              Tcmalloc page size
------------------------------------------------
Call ReleaseFreeMemory() to release freelist memory to the OS (via madvise()).
Bytes released to the OS take up virtual address space but no physical memory.



[gerryyang ~/mytest/gperftools-2.10 17:15:32]$ MALLOCSTATS=2 LD_PRELOAD=lib/libtcmalloc.so ./a.out
Child process (pid: 3766654): shared_var = 42
Child process (pid: 3766654): shared_var changed to 13
Starting tracking the heap
PROFILE: interrupts/evictions/bytes = 53/0/1728
Dumping heap profile to heap.prof.0001.heap (over)
------------------------------------------------
MALLOC:       16798072 (   16.0 MiB) Bytes in use by application
MALLOC: +     11403264 (   10.9 MiB) Bytes in page heap freelist
MALLOC: +       107856 (    0.1 MiB) Bytes in central cache freelist
MALLOC: +            0 (    0.0 MiB) Bytes in transfer cache freelist
MALLOC: +         2360 (    0.0 MiB) Bytes in thread cache freelists
MALLOC: +      2621440 (    2.5 MiB) Bytes in malloc metadata
MALLOC:   ------------
MALLOC: =     30932992 (   29.5 MiB) Actual memory used (physical + swap)
MALLOC: +            0 (    0.0 MiB) Bytes released to OS (aka unmapped)
MALLOC:   ------------
MALLOC: =     30932992 (   29.5 MiB) Virtual address space used
MALLOC:
MALLOC:           2065              Spans in use
MALLOC:              2              Thread heaps in use
MALLOC:           8192              Tcmalloc page size
------------------------------------------------
Call ReleaseFreeMemory() to release freelist memory to the OS (via madvise()).
Bytes released to the OS take up virtual address space but no physical memory.
------------------------------------------------
Total size of freelists for per-thread caches,
transfer cache, and central cache, by size class
------------------------------------------------
class   1 [        8 bytes ] :     1023 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
class   2 [       16 bytes ] :      511 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
class   3 [       32 bytes ] :      251 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
class   4 [       48 bytes ] :      170 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
class   5 [       64 bytes ] :      127 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
class   6 [       80 bytes ] :      102 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
class   8 [      112 bytes ] :       73 objs;   0.0 MiB;   0.1 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
class   9 [      128 bytes ] :       63 objs;   0.0 MiB;   0.1 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
class  11 [      160 bytes ] :       51 objs;   0.0 MiB;   0.1 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
class  16 [      240 bytes ] :       34 objs;   0.0 MiB;   0.1 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
class  33 [     1280 bytes ] :       12 objs;   0.0 MiB;   0.1 cum MiB;    0.001 overhead MiB;    0.001 cum overhead MiB
class  43 [     4096 bytes ] :        3 objs;   0.0 MiB;   0.1 cum MiB;    0.000 overhead MiB;    0.001 cum overhead MiB
------------------------------------------------
PageHeap: 1 sizes;   10.9 MiB free;    0.0 MiB unmapped
------------------------------------------------
   112 pages *      1 spans ~    0.9 MiB;    0.9 MiB cum; unmapped:    0.0 MiB;    0.0 MiB cum
   >128   large *      1 spans ~   10.0 MiB;   10.9 MiB cum; unmapped:    0.0 MiB;    0.0 MiB cum
   Parent process (pid: 3766653): shared_var = 42
   ------------------------------------------------
   MALLOC:          20632 (    0.0 MiB) Bytes in use by application
   MALLOC: +       974848 (    0.9 MiB) Bytes in page heap freelist
   MALLOC: +        52888 (    0.1 MiB) Bytes in central cache freelist
   MALLOC: +            0 (    0.0 MiB) Bytes in transfer cache freelist
   MALLOC: +          208 (    0.0 MiB) Bytes in thread cache freelists
   MALLOC: +      2490368 (    2.4 MiB) Bytes in malloc metadata
   MALLOC:   ------------
   MALLOC: =      3538944 (    3.4 MiB) Actual memory used (physical + swap)
   MALLOC: +            0 (    0.0 MiB) Bytes released to OS (aka unmapped)
   MALLOC:   ------------
   MALLOC: =      3538944 (    3.4 MiB) Virtual address space used
   MALLOC:
   MALLOC:             10              Spans in use
   MALLOC:              2              Thread heaps in use
   MALLOC:           8192              Tcmalloc page size
   ------------------------------------------------
   Call ReleaseFreeMemory() to release freelist memory to the OS (via madvise()).
   Bytes released to the OS take up virtual address space but no physical memory.
   ------------------------------------------------
   Total size of freelists for per-thread caches,
   transfer cache, and central cache, by size class
   ------------------------------------------------
   class   1 [        8 bytes ] :     1023 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
   class   2 [       16 bytes ] :      511 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
   class   3 [       32 bytes ] :      252 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
   class   4 [       48 bytes ] :      170 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
   class   6 [       80 bytes ] :      102 objs;   0.0 MiB;   0.0 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
   class  43 [     4096 bytes ] :        3 objs;   0.0 MiB;   0.1 cum MiB;    0.000 overhead MiB;    0.000 cum overhead MiB
   ------------------------------------------------
   PageHeap: 1 sizes;    0.9 MiB free;    0.0 MiB unmapped
   ------------------------------------------------
      119 pages *      1 spans ~    0.9 MiB;    0.9 MiB cum; unmapped:    0.0 MiB;    0.0 MiB cum
	  >128   large *      0 spans ~    0.0 MiB;    0.9 MiB cum; unmapped:    0.0 MiB;    0.0 MiB cum
```

