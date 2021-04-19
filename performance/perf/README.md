
# Usage

```
$ uname -r
4.15.0-29-generic
$ cat "/boot/config-`uname -r`" | grep "PERF_EVENT"
CONFIG_HAVE_PERF_EVENTS=y
CONFIG_PERF_EVENTS=y
CONFIG_HAVE_PERF_EVENTS_NMI=y
CONFIG_PERF_EVENTS_INTEL_UNCORE=y
CONFIG_PERF_EVENTS_INTEL_RAPL=m
CONFIG_PERF_EVENTS_INTEL_CSTATE=m
# CONFIG_PERF_EVENTS_AMD_POWER is not set
CONFIG_SECURITY_PERF_EVENTS_RESTRICT=y
```

```
perf top -p $pid

perf stat -p $pid
perf stat --repeat 5 -e cache-misses,cache-references,instructions,cycles ./perf_stat_example

perf record ./target
perf report

```


# Refer

* http://wiki.csie.ncku.edu.tw/embedded/perf-tutorial
