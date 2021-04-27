

# Performance 

* 测试工具
  + 在线工具：https://quick-bench.com/q/fe070fSjnso_C7Hwe7ElI7tEuiw
  + 离线工具：https://github.com/DigitalInBlue/Celero
* 测试代码
  + https://github.com/gerryyang/mac-utils/blob/master/programing/protocol-buffers/tutorial/src/press.cc
  + https://github.com/gerryyang/mac-utils/blob/master/programing/protocol-buffers/tutorial/src/celero_benchmark.cc

```
$ perf stat -B ./press
pid(2008)
pb repeated
find it(999999) cnt(999999)
elapse(0.280948s)

 Performance counter stats for './press':

        606.010051      task-clock (msec)         #    0.858 CPUs utilized
               730      context-switches          #    0.001 M/sec
                 0      cpu-migrations            #    0.000 K/sec
           123,296      page-faults               #    0.203 M/sec
   <not supported>      cycles
   <not supported>      instructions
   <not supported>      branches
   <not supported>      branch-misses

       0.706142993 seconds time elapsed


$ perf stat -B ./press
pid(2277)
map
find it(999999) cnt(999999)
elapse(0.0260171s)

 Performance counter stats for './press':

        457.361877      task-clock (msec)         #    0.914 CPUs utilized
               572      context-switches          #    0.001 M/sec
                 0      cpu-migrations            #    0.000 K/sec
            19,711      page-faults               #    0.043 M/sec
   <not supported>      cycles
   <not supported>      instructions
   <not supported>      branches
   <not supported>      branch-misses

       0.500588136 seconds time elapsed
```

使用[Celero](https://github.com/DigitalInBlue/Celero)测试结果：(`-O2`编译)

```
 $ ./celero_benchmark 
Celero
Timer resolution: 0.001000 us
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |   RAM (bytes)   | 
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|find            | vector          |            Null |               1 |               1 |         1.00000 |       172.00000 |         5813.95 |        51277824 | 
|find            | pb_repeated     |            Null |               1 |               1 |        73.59302 |     12658.00000 |           79.00 |        51777536 | 
|find            | set             |            Null |              10 |              20 |         0.00058 |         0.10000 |     10000000.00 |        51777536 | 
|find            | unordered_set   |            Null |              10 |              20 |         0.00029 |         0.05000 |     20000000.00 |        51777536 | 
|find            | flat_set        |            Null |              10 |              20 |         0.00058 |         0.10000 |     10000000.00 |        51777536 | 
Completed in 00:00:00.028138
```

