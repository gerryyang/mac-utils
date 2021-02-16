


```
$ time ./a.out 1
This machine does not support rdtscp.
tsc=4030837879, aux=0x0, cpu speed 2397 mhz
getspeed_010:2410
getspeed_100:2395
getspeed_500:-280931417
max diff=5626, ave diff=0, diff1 count=336682, diff10 count=1562.

real    0m5.788s
user    0m2.004s
sys     0m3.025s

$ time ./a.out 1
This machine does not support rdtscp.
tsc=3545218234, aux=0x0, cpu speed 2397 mhz
getspeed_010:2410
getspeed_100:2396
getspeed_500:-280931417
max diff=4927, ave diff=0, diff1 count=288140, diff10 count=1096.

real    0m5.686s
user    0m1.862s
sys     0m3.038s

$ time ./a.out 2

real    0m3.448s
user    0m1.114s
sys     0m2.260s

$ time ./a.out 3

real    0m0.191s
user    0m0.100s
sys     0m0.040s
```
