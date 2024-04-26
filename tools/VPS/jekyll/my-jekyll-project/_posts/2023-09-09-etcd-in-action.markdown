---
layout: post
title:  "ETCD in Action"
date:   2023-09-09 09:00:00 +0800
categories: GoLang
---

* Do not remove this line (it will not be displayed)
{:toc}

`etcd` 是一个分布式键值存储系统，用于存储和管理配置数据。`etcd` 使用一种称为 `Raft` 的共识算法来确保数据在集群中的一致性。


# 读取模型

在 `etcd` 中，有两种读取请求：**线性一致性**（`linearizable`）读取和**可串行化**（`serializable`）读取。这两种读取请求之间的主要区别在于它们**如何确保数据的一致性和最新性**。

**线性一致性读取**和**可串行化读取**在**性能和数据一致性之间进行权衡**。**线性一致性读取**提供了最新的数据，但代价是更高的延迟和开销。**可串行化读取**具有较低的延迟和开销，但可能会返回过时的数据。根据应用程序对一致性和性能的需求，可以选择使用适当的读取请求类型。


## 线性一致性读取（Linearizable read）

**线性一致性读取**会通过集群成员的法定人数（`quorum`）进行共识以获取最新的数据。这意味着，当一个线性一致性读取请求被发出时，它需要集群中的大多数成员（即法定人数）同意返回的数据是最新的。这确保了您读取的数据是最新的，但代价是更高的延迟和开销，因为需要在集群成员之间进行通信以达成共识。

## 可串行化读取（Serializable read）

与线性一致性读取相比，**可串行化读取**的成本更低，因为它们可以由单个 `etcd` 成员提供，而**无需在集群成员之间达成共识**。这意味着，当一个可串行化读取请求被发出时，它可以直接从任何一个 `etcd` 成员获取数据，而无需等待其他成员的同意。这可以减少延迟和开销，但可能会返回过时的数据，因为数据可能尚未在集群中完全同步。





# Performance

## Understanding performance: `latency` & `throughput`

`etcd` provides stable, sustained high performance. **Two factors** define performance: `latency` and `throughput`.

* `Latency` is the time taken to complete an operation.
* `Throughput` is the total operations completed within some time period.

Usually average latency increases as the overall throughput increases when `etcd` accepts concurrent client requests.

In common cloud environments, like a standard `n-4` on Google Compute Engine (GCE) or a comparable machine type on AWS, a three member etcd cluster finishes a request in less than one millisecond(毫秒) under light load, and can complete more than 30,000 requests per second under heavy load.

`etcd` uses the `Raft` consensus algorithm to replicate requests among members and reach agreement. Consensus performance, especially commit latency, is limited by **two physical constraints**: `network IO latency` and `disk IO latency`.

The minimum time to finish an etcd request is the network **Round Trip Time** (`RTT`) between members, **plus** the time fdatasync requires to commit the data to permanent storage.

The RTT within a datacenter may be as long as several hundred microseconds(几百微妙). A typical RTT within the United States is around `50ms`, and can be as slow as `400ms` between continents(大洲).

The typical fdatasync latency for a spinning disk is about `10ms`. For `SSDs`, the latency is often lower than `1ms`. To increase throughput, `etcd` **batches multiple requests** together and submits them to `Raft`. This batching policy lets `etcd` attain high throughput despite heavy load.

There are other sub-systems which impact(影响) the overall performance of `etcd`. Each serialized etcd request must run through **etcd’s boltdb-backed MVCC storage engine**, which usually takes tens of microseconds(几十微妙) to finish. Periodically etcd incrementally snapshots its recently applied requests, merging them back with the previous on-disk snapshot. This process may lead to a latency spike(急升). Although this is usually not a problem on `SSDs`, it may double the observed latency on `HDD`. Likewise, inflight compactions(压缩) can impact etcd’s performance. Fortunately, the impact is often insignificant since the compaction is staggered(交错) so it does not compete for resources with regular requests. The RPC system, gRPC, gives etcd a well-defined, extensible API, but it also introduces additional latency, especially for local reads.


## Benchmarks

Benchmarking `etcd` performance can be done with the [benchmark CLI tool](https://github.com/etcd-io/etcd/tree/v3.4.16/tools/benchmark) included with etcd.

For some baseline performance numbers, we consider **a three member etcd cluster** with the following **hardware configuration**:

* Google Cloud Compute Engine
* 3 machines of 8 vCPUs + 16GB Memory + 50GB SSD
* 1 machine(client) of 16 vCPUs + 30GB Memory + 50GB SSD
* Ubuntu 17.04
* etcd 3.2.0, go 1.8.3


With this configuration, etcd can approximately write:

![etcd_benchmark](/assets/images/202309/etcd_benchmark.png)

Sample commands are:

``` bash
# write to leader
benchmark --endpoints=${HOST_1} --target-leader --conns=1 --clients=1 \
    put --key-size=8 --sequential-keys --total=10000 --val-size=256
benchmark --endpoints=${HOST_1} --target-leader  --conns=100 --clients=1000 \
    put --key-size=8 --sequential-keys --total=100000 --val-size=256

# write to all members
benchmark --endpoints=${HOST_1},${HOST_2},${HOST_3} --conns=100 --clients=1000 \
    put --key-size=8 --sequential-keys --total=100000 --val-size=256
```

**Linearizable read requests** (线性一致性读取) go through a `quorum`(法定人数) of cluster members for consensus to fetch the most recent data. **Serializable read requests** (可串行化读取) are cheaper than linearizable reads since they are served by any single etcd member, instead of a quorum of members, in exchange for possibly serving stale data. etcd can read:

![etcd_benchmark2](/assets/images/202309/etcd_benchmark2.png)

Sample commands are:

```
# Single connection read requests
benchmark --endpoints=${HOST_1},${HOST_2},${HOST_3} --conns=1 --clients=1 \
    range YOUR_KEY --consistency=l --total=10000
benchmark --endpoints=${HOST_1},${HOST_2},${HOST_3} --conns=1 --clients=1 \
    range YOUR_KEY --consistency=s --total=10000

# Many concurrent read requests
benchmark --endpoints=${HOST_1},${HOST_2},${HOST_3} --conns=100 --clients=1000 \
    range YOUR_KEY --consistency=l --total=100000
benchmark --endpoints=${HOST_1},${HOST_2},${HOST_3} --conns=100 --clients=1000 \
    range YOUR_KEY --consistency=s --total=100000
```

We encourage running the benchmark test when setting up an etcd cluster for the first time in **a new environment** to ensure the cluster achieves adequate performance; cluster latency and throughput can be sensitive to minor environment differences.

在新环境中首次设置 etcd 集群时，建议运行基准测试，以确保集群具有足够的性能。这是因为集群的延迟和吞吐量可能对环境差异非常敏感，即使是很小的差异也可能导致性能的显著变化。

基准测试是一种性能测试方法，用于评估系统在特定工作负载下的性能。通过运行基准测试，可以确定 etcd 集群在新环境中的实际延迟和吞吐量，并检查它们是否满足应用程序需求。

在新环境中设置 etcd 集群时运行基准测试的好处如下：

1. 识别性能瓶颈：基准测试可以帮助发现可能影响集群性能的问题，例如硬件限制、网络延迟或配置问题。这使可以在投入生产环境之前解决这些问题。
2. 优化配置：基准测试可以帮助确定最佳的 etcd 集群配置，以实现最佳性能。例如，可能需要调整 etcd 的各种参数，如心跳间隔、选举超时或快照阈值，以适应新环境。
3. 评估环境差异的影响：由于 etcd 集群的性能可能受到环境差异的影响，因此在新环境中运行基准测试可以帮助您了解这些差异对性能的实际影响。这可以提供有关如何优化环境以提高性能的见解。

总之，当在新环境中首次设置 etcd 集群时，运行基准测试可以帮助您确保集群具有足够的性能。这将能够在投入生产环境之前发现和解决潜在的性能问题，从而确保应用程序能够正常运行。



# Refer

* https://etcd.io/docs/v3.4/op-guide/performance/











