
* https://github.com/brendangregg/perf-tools
* https://github.com/brendangregg/perf-tools/blob/master/examples/tpoint_example.txt


1. 通过 `./tpoint.sh signal:signal_generate 'sig == 5 && pid==3073758` 可以捕获到哪个 pid 向 gamesvr 进程发送的 SIGTRAP 信号。
2. 通过 `./functrace.sh '*ptrace' -p xxx` 抓取 `ptrace` 系统调用，或者通过 `perf trace -a -e 'ptrace'` 抓取。


```
$ ./tpoint.sh -h
USAGE: tpoint [-hHsv] [-d secs] [-p PID] [-L TID] tracepoint [filter]
       tpoint -l
                 -d seconds      # trace duration, and use buffers
                 -p PID          # PID to match on event
                 -L TID          # thread id to match on event
                 -v              # view format file (don't trace)
                 -H              # include column headers
                 -l              # list all tracepoints
                 -s              # show kernel stack traces
                 -h              # this usage message
   eg,
       tpoint -l | grep open
                                 # find tracepoints containing "open"
       tpoint syscalls:sys_enter_open
                                 # trace open() syscall entry
       tpoint block:block_rq_issue
                                 # trace block I/O issue
       tpoint -s block:block_rq_issue
                                 # show kernel stacks

See the man page and example file for more info.
```

```
$ ./tpoint.sh -l | grep open
syscalls:sys_enter_fsopen
syscalls:sys_enter_mq_open
syscalls:sys_enter_open
syscalls:sys_enter_openat
syscalls:sys_enter_open_by_handle_at
syscalls:sys_enter_open_tree
syscalls:sys_enter_perf_event_open
syscalls:sys_enter_pidfd_open
syscalls:sys_exit_fsopen
syscalls:sys_exit_mq_open
syscalls:sys_exit_open
syscalls:sys_exit_openat
syscalls:sys_exit_open_by_handle_at
syscalls:sys_exit_open_tree
syscalls:sys_exit_perf_event_open
syscalls:sys_exit_pidfd_open
```

```
$ ls -rtlh /sys/kernel/debug/tracing/
total 0
-rw-r--r--  1 root root 0 Jun 13 09:00 set_event_pid
-rw-r--r--  1 root root 0 Jun 13 09:00 set_event
drwxr-xr-x 88 root root 0 Jun 13 09:00 events
-r--r--r--  1 root root 0 Jun 13 09:00 available_events
-rw-r--r--  1 root root 0 Jun 13 09:00 tracing_on
-rw-r--r--  1 root root 0 Jun 13 09:00 tracing_max_latency
-rw-r--r--  1 root root 0 Jun 13 09:00 tracing_cpumask
-r--r--r--  1 root root 0 Jun 13 09:00 trace_pipe
-rw-r--r--  1 root root 0 Jun 13 09:00 trace_options
--w--w----  1 root root 0 Jun 13 09:00 trace_marker_raw
--w--w----  1 root root 0 Jun 13 09:00 trace_marker
-rw-r--r--  1 root root 0 Jun 13 09:00 trace_clock
-rw-r--r--  1 root root 0 Jun 13 09:00 trace
-r--r--r--  1 root root 0 Jun 13 09:00 timestamp_mode
-rw-r--r--  1 root root 0 Jun 13 09:00 snapshot
-rw-r--r--  1 root root 0 Jun 13 09:00 set_graph_function
-rw-r--r--  1 root root 0 Jun 13 09:00 set_ftrace_pid
-rw-r--r--  1 root root 0 Jun 13 09:00 set_ftrace_notrace
-rw-r--r--  1 root root 0 Jun 13 09:00 set_ftrace_filter
drwxr-xr-x 50 root root 0 Jun 13 09:00 per_cpu
drwxr-xr-x  2 root root 0 Jun 13 09:00 options
--w-------  1 root root 0 Jun 13 09:00 free_buffer
-rw-r--r--  1 root root 0 Jun 13 09:00 error_log
-r--r--r--  1 root root 0 Jun 13 09:00 enabled_functions
-rw-r--r--  1 root root 0 Jun 13 09:00 current_tracer
-r--r--r--  1 root root 0 Jun 13 09:00 buffer_total_size_kb
-rw-r--r--  1 root root 0 Jun 13 09:00 buffer_size_kb
-r--r--r--  1 root root 0 Jun 13 09:00 buffer_percent
-r--r--r--  1 root root 0 Jun 13 09:00 available_tracers
-r--r--r--  1 root root 0 Jun 13 09:00 available_filter_functions
-rw-r--r--  1 root root 0 Jun 13 09:00 tracing_thresh
drwxr-xr-x  2 root root 0 Jun 13 09:00 trace_stat
-rw-r--r--  1 root root 0 Jun 13 09:00 set_graph_notrace
-r--r--r--  1 root root 0 Jun 13 09:00 saved_tgids
-rw-r--r--  1 root root 0 Jun 13 09:00 saved_cmdlines_size
-r--r--r--  1 root root 0 Jun 13 09:00 saved_cmdlines
-r--r--r--  1 root root 0 Jun 13 09:00 README
-rw-r--r--  1 root root 0 Jun 13 09:00 function_profile_enabled
-r--r--r--  1 root root 0 Jun 13 09:00 uprobe_profile
-rw-r--r--  1 root root 0 Jun 13 09:00 uprobe_events
-r--r--r--  1 root root 0 Jun 13 09:00 printk_formats
-rw-r--r--  1 root root 0 Jun 13 09:00 max_graph_depth
-r--r--r--  1 root root 0 Jun 13 09:00 kprobe_profile
-rw-r--r--  1 root root 0 Jun 13 09:00 kprobe_events
drwxr-xr-x  2 root root 0 Jun 13 09:00 instances
-r--r--r--  1 root root 0 Jun 13 09:00 dyn_ftrace_total_info
-rw-r--r--  1 root root 0 Jun 13 09:00 dynamic_events
-rw-r--r--  1 root root 0 Jun 13 09:00 stack_trace_filter
-r--r--r--  1 root root 0 Jun 13 09:00 stack_trace
-rw-r--r--  1 root root 0 Jun 13 09:00 stack_max_size
```

```
$ ./tpoint.sh signal:signal_generate 'sig == 5 && pid==3073758'
Tracing signal:signal_generate. Ctrl-C to end.
           <...>-2908857 [006] d... 1391957.290037: signal_generate: sig=5 errno=0 code=0 comm=gamesvr pid=3073758 grp=1 res=0
		   ^C
		   Ending tracing...
```


```
$ ps ux|grep gamesvr
gerryya+ 3073758 23.5  1.2 5517940 1695872 ?     Sl   11:31   1:50 /data/home/gerryyang/speedgame/bin/gamesvr/gamesvr --id=72.120.100.1 --bus-key=3072 --svr-id-mask=9.7.7.9
$ kill -5 3073758
$ ps
    PID TTY          TIME CMD
	2908857 pts/23   00:00:00 bash
	3121156 pts/23   00:00:00 ps
```
