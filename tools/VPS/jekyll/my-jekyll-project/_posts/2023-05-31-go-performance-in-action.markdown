---
layout: post
title:  "Go Performance in Action"
date:   2023-05-31 20:00:00 +0800
categories: [GoLang]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Go 内存泄漏

## 垃圾回收

自动内存管理减轻了开发人员管理内存的复杂性，不需要像 C/C++ 那样显式 malloc/free，或者 new/delete。垃圾回收借助于一些垃圾回收算法完成对无用内存的清理，垃圾回收算法有很多，比如：引用计数、标记清除、拷贝、分代等等。

Go 中垃圾回收器采用的是**并发三色标记清除**算法。参考：

* [Garbage Collection In Go : Part I - Semantics](https://www.ardanlabs.com/blog/2018/12/garbage-collection-in-go-part1-semantics.html)
* [Garbage Collection In Go : Part II - GC Traces](https://www.ardanlabs.com/blog/2019/05/garbage-collection-in-go-part2-gctraces.html)
* [Garbage Collection In Go : Part III - GC Pacing](https://www.ardanlabs.com/blog/2019/07/garbage-collection-in-go-part3-gcpacing.html)

Go 语言支持自动内存管理，那还存在内存泄漏问题吗？

理论上，垃圾回收（gc）算法能够对堆内存进行有效的清理，这个是没什么可质疑的。但是要理解，垃圾回收能够正常运行的前提是，程序中必须解除对内存的引用，这样垃圾回收才会将其判定为可回收内存并回收。


## 内存泄漏场景

实际情况是，编码中确实存在一些场景，会造成“临时性”或者“永久性”内存泄露，是需要开发人员加深对编程语言设计实现、编译器特性的理解之后才能优化掉的，参考：[Memory Leaking Scenarios](https://go101.org/article/memory-leaking.html)。

**临时性泄露**，指的是该释放的内存资源没有及时释放，对应的内存资源仍然有机会在更晚些时候被释放，即便如此在内存资源紧张情况下，也会是个问题。这类主要是 string、slice 底层 buffer 的错误共享，导致无用数据对象无法及时释放，或者 defer 函数导致的资源没有及时释放。

**永久性泄露**，指的是在进程后续生命周期内，泄露的内存都没有机会回收，如 goroutine 内部预期之外的 for-loop 或者 chan select-case 导致的无法退出的情况，导致协程栈及引用内存永久泄露问题。





# [Profiling a Go program](https://golang.org/pkg/runtime/pprof/)

[pprof](https://github.com/google/pprof/blob/main/doc/README.md) is a tool for visualization and analysis of profiling data.

在 Go 语言中，可以通过在程序中导入 `net/http/pprof` 包来启用性能分析。这个包提供了一个 HTTP 服务器，可以通过浏览器访问并查看程序的性能分析结果。在程序中导入 `net/http/pprof` 包后，可以在程序中添加一些代码来启动 HTTP 服务器，例如：

``` golang
import (
    "net/http"
    _ "net/http/pprof"
)

func main() {
    // 启动 HTTP 服务器
    go func() {
        http.ListenAndServe("localhost:6060", nil)
    }()

    // 程序的其他代码
    // ...
}
```

在程序中添加这些代码后，可以通过访问 `http://localhost:6060/debug/pprof/` 来查看程序的性能分析结果。


## 使用 HTTP 服务实时性能分析

测试代码：

``` go
package main

import (
	"fmt"
	"net/http"
	_ "net/http/pprof"
)

func main() {

	// 启动 HTTP 服务器
	go func() {
		// http://localhost:8080/debug/pprof/
		http.ListenAndServe("localhost:8080", nil)
	}()

	fmt.Println("Hello World")
}
```

通过 `http://localhost:8080/debug/pprof/` 查看：

![pprof1](/assets/images/202305/pprof1.png)



## 使用 pprof 文件分析性能

测试代码：

```go
package main

import (
	"fmt"
	"net/http"
	"os"
	"runtime/pprof"
	"time"
)

func perfStat() {

	if err := os.MkdirAll("./pprof", 0755); err != nil {
		fmt.Printf("MkdirAll err: %+v\n", err)
		return
	}

	go func() {
		fmt.Printf("Start Monitor\n")

		for {
			fmt.Printf("Update CPU Info\n")
			cpuFile := fmt.Sprintf("./pprof/pprof_cpu_%v", time.Now().Minute())
			fcpu, err := os.Create(cpuFile)
			if err != nil {
				fmt.Printf("Create err: %+v\n", err)
				break
			}

			if err = pprof.StartCPUProfile(fcpu); err != nil {
				fmt.Printf("StartCPUProfile err: %+v\n", err)
				fcpu.Close()
				break
			}

			// sleep 1 min
			time.Sleep(1 * time.Minute)

			pprof.StopCPUProfile()
			fcpu.Close()

			fmt.Printf("Update MEM Info\n")
			memFile := fmt.Sprintf("./pprof/pprof_mem_%v", time.Now().Minute())
			fmem, err := os.Create(memFile)
			if err != nil {
				fmt.Printf("Create err: %+v\n", err)
				break
			}
			if err = pprof.WriteHeapProfile(fmem); err != nil {
				fmt.Printf("WriteHeapProfile err: %+v\n", err)
				fmem.Close()
				break
			}
			fmem.Close()

			fmt.Printf("Update BLOCK Info\n")
			blockFile := fmt.Sprintf("./pprof/pprof_block_%v", time.Now().Minute())
			fblock, err := os.Create(blockFile)
			if err != nil {
				fmt.Printf("Create err: %+v\n", err)
				break
			}
			if err = pprof.Lookup("block").WriteTo(fblock, 0); err != nil {
				fmt.Printf("Block WriteTo err: %+v\n", err)
				fblock.Close()
				break
			}
			fblock.Close()
		}
	}()
}

func main() {

	// 性能监控
	perfStat()

	time.Sleep(10 * time.Minute)
}
```

上面的代码每分钟会生成一个 pprof 文件，结果如下：

```
-rw-r--r--  1 gerry  staff   108B  5 31 22:48 pprof_cpu_47
-rw-r--r--  1 gerry  staff   807B  5 31 22:48 pprof_mem_48
-rw-r--r--  1 gerry  staff   104B  5 31 22:48 pprof_block_48
-rw-r--r--  1 gerry  staff   108B  5 31 22:49 pprof_cpu_48
-rw-r--r--  1 gerry  staff   1.3K  5 31 22:49 pprof_mem_49
-rw-r--r--  1 gerry  staff   104B  5 31 22:49 pprof_block_49
```

进行性能分析：

``` bash
# 自动弹出浏览器窗口查看
go tool pprof -http=":8080" [binary] [profile]
```

查看内存分配：

``` bash
go tool pprof -http=":8080" ../test2 pprof_mem_48
Serving web UI on http://localhost:8080
```

![pprof2](/assets/images/202305/pprof2.png)





# Q&A

## Could not execute dot; may need to install graphviz

安装 [Graphviz](https://www.graphviz.org/)，下载安装可参考 https://www.graphviz.org/download/

> What is Graphviz?
>
> Graphviz is open source graph visualization software. Graph visualization is a way of representing structural information as diagrams of abstract graphs and networks. It has important applications in networking, bioinformatics, software engineering, database and web design, machine learning, and in visual interfaces for other technical domains.



``` bash
# Linux
sudo yum install graphviz

# MacOS
brew install graphviz
```

# Refer

* [How does the OOM killer decide which process to kill first?](https://unix.stackexchange.com/questions/153585/how-does-the-oom-killer-decide-which-process-to-kill-first)
* [golang性能诊断看这篇就够了](https://zhuanlan.zhihu.com/p/345413502)
* [Go程序内存泄露问题快速定位](https://www.hitzhangjie.pro/blog/2021-04-14-go%E7%A8%8B%E5%BA%8F%E5%86%85%E5%AD%98%E6%B3%84%E9%9C%B2%E9%97%AE%E9%A2%98%E5%BF%AB%E9%80%9F%E5%AE%9A%E4%BD%8D/)

* [Go语言调试器开发](https://www.hitzhangjie.pro/debugger101.io/)


