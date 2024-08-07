---
layout: post
title:  "Linux Performance in Action"
date:   2024-07-01 12:00:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}

# 程序员为什么要关心代码性能？

代码性能表现在很多方面和指标，比较常见的几个指标有**吞吐量**（Throughput）、**服务延迟**（Service latency）、**扩展性**（Scalability）和**资源使用效率**（Resource Utilization）。

* 吞吐量：单位时间处理请求的数量。
* 服务延迟：客户请求的处理时间。
* 扩展性：系统在高压的情况下能不能正常处理请求。
* 资源使用效率：单位请求处理所需要的资源量（比如 CPU，内存等）。

> 注意，除了这几个指标之外，根据场景，还可以有其他性能指标，比如**可靠性**（Reliability）。可靠性注重的是在极端情况下能不能持续处理正常的服务请求

性能好的代码，可以用四个字来概括："**多快好省**"

![performance](/assets/images/202407/performance.png)

## 优化性能示例 1：`google::dense_hash_map` 的性能可以比 `std::unordered_map` 快好几倍

* [Benchmark of major hash maps implementations](https://tessil.github.io/2016/08/29/benchmark-hopscotch-map.html)
* [Boost.Unordered](https://www.boost.org/doc/libs/develop/libs/unordered/doc/html/unordered.html)

## 优化性能示例 2：通过使用 GCC 的 `__builtin_prefetch` 指令来预先提取关键指令，从而降低缓存的缺失比例，提高 CPU 的使用效率

![performance2](/assets/images/202407/performance2.png)

假设有一个处理大量数据的函数：

``` cpp
#include <stdio.h>

void process_data(int *data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        data[i] = data[i] * 2;
    }
}

int main() {
    int data[10000];
    for (int i = 0; i < 10000; ++i) {
        data[i] = i;
    }

    process_data(data, 10000);
    printf("result: %d\n", data[9999]);
    return 0;
}
```

可以使用 `__builtin_prefetch` 对 `process_data` 函数进行优化，以预先提取关键指令并降低缓存的缺失比例：

``` cpp
#include <stdio.h>

void process_data(int *data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        // 预先提取下一个数据元素，提前 16 个元素进行预取
        if (i + 16 < size) {
            __builtin_prefetch(&data[i + 16], 1, 1);
        }
        data[i] = data[i] * 2;
    }
}

int main() {
    int data[10000];
    for (int i = 0; i < 10000; ++i) {
        data[i] = i;
    }

    process_data(data, 10000);
    printf("result: %d\n", data[9999]);
    return 0;
}
```

在 `process_data` 函数中，添加了 `__builtin_prefetch` 指令来预先提取数组中的下一个元素。这样，当 CPU 处理当前元素时，下一个元素已经被预取到缓存中，从而减少了缓存缺失和等待指令获取的时间。请注意，这个优化可能在某些情况下对性能产生负面影响，因为预取操作可能会消耗内存带宽。在实际应用中，需要根据具体情况调整预取距离（在本例中为 16）并进行性能测试，以确保优化达到预期效果。




# Refer












