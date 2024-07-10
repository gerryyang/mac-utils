#!/bin/bash

# @brief 循环记录某个进程的 cpu 使用率符合某个条件时，输出这个进程所有线程的堆栈信息
# @note 需要 root 执行权限

process_name="gamesvr"
cpu_usage=60

# 循环执行任务，直到程序停止
while :; do
    # 通过 top 命令获取当前正在运行的进程，并通过 pgrep 命令过滤出 process_name 进程
    for pid in $(top -p $(pgrep -f $process_name) -d 1 -bn2 | tail -1 | awk '$9 < $cpu_usage { print $1 }'); do
        # 获取进程的启动时间
        echo "${pid} `date`"
        
        # 遍历该进程的子进程
        for dir in /proc/$pid/task/*; do
            # 获取进程的详细信息，包括进程名称和进程 ID
            echo "${dir##*/} ($(cat $dir/comm))"
            
            # 打印进程的栈信息
            cat $dir/stack
            
            # 打印当前进程的详细信息
            echo
        done
    done
    
    # 等待一段时间再继续检测
    sleep 10
done
