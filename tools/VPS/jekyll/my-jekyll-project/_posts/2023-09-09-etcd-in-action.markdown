---
layout: post
title:  "ETCD in Action"
date:   2023-09-09 09:00:00 +0800
categories: GoLang
---

* Do not remove this line (it will not be displayed)
{:toc}


# 思考问题

* **原理**
  + 如何判断 etcd 是否适合你的业务场景？
  + 为什么 etcd 适合读多写少？
  + 线性读和串行读各自适用什么业务场景，分别是如何实现的？
  + etcd Watch 机制能保证事件不丢吗？
  + 为什么 Follower 节点的 Raft 日志条目可能会与 Leader 冲突？冲突时 Follower 节点的 WAL 日志如何删除已持久化但冲突的日志条目？
  + 不小心删除一个 key 后，可以马上找回来吗？

* **稳定性**
  + 哪些因素会导致 etcd 集群 Leader 发生切换？
  + 为什么 etcd 社区压测结果显示读能达到10几万每秒，但在自己的业务集群中 QPS 可能未过百就出现超时错误？
  + etcd 能跨地域部署吗？
  + 如何优化提升 etcd 性能和稳定性？

* **一致性**
  + 为什么基于 Raft 实现的 etcd 还可能会出现数据不一致？

* **延迟**
  + 为什么集群各节点磁盘 I/O 延迟很低，写请求也会超时？

* **内存**
  + 为什么只存储一个几百 KB 的 KV，etcd 进程却可能消耗数 G 内存？

* **DB 大小**
  + 为什么删除大量的数据，DB 大小不减少？
  + 为何 etcd 社区建议 DB 大小不要超过 8G？
  + 哪些因素会导致 DB 大小增加？

* **最佳实践**
  + 当在一个 namespace 下创建了数万个 Pod/CRD 资源时，同时频繁通过标签去查询指定 Pod/CRD 资源时， APIServer 和 etcd 为什么扛不住？
  + 快速增长的业务应如何避免单 etcd 集群出现性能瓶颈？
  + 如何构建安全，高可靠的 etcd 集群运维体系？

* **Kubernetes**
  + Kubernetes 创建 Pod 背后 etcd 是如何工作的？
  + etcd 如何为 Kubernetes 控制器编程模型提供支撑？
  + APIServer 的 "too old resource version" 错误跟 etcd 有什么关系？



# 基本介绍

* `etcd` 是一个分布式键值 KV 存储系统，应用于服务发现，分布式锁，配置存储，分布式协调等。
* `etcd` 使用 `Raft` 共识算法来确保数据在集群中的一致性。
* 支持 `gRPC` 和 `Http` 协议调用。
* Kubernetes 使用 etcd 作为底层存储。Google 的 Spanner 和微信的 PaxosStore 都是基于 Paxos 协议实现的 KV 存储。Paxos 和 Raft 各有优劣，一般 Raft 工程实现简单，但是有租约不可用的问题；基于无租约 Paxos 协议没有 Leader 节点，可以做到无缝切换。



![etcd_frame](/assets/images/202501/etcd_frame.png)

1. client 层：包括 client V2 和 V3 这两个客户端 API 库，同时支持负载均衡和节点故障转移。
2. API 层：负责对外提供 HTTP 和 gRPC 两种类型的访问接口。
3. Raft 层：实现了包括 Leader 选举和日志复制等 Raft 算法核心功能，并且通过 wal 机制持久化日志条目，保障 etcd 多节点的一致性和可用性，同时使用 Read Index 机制实现读请求的强一致性。
4. 通信层：集群多节点通信实现，使用 gRPC 在服务器 peer 间和 client 间通信。
5. 存储层：包括 KeyIndex 和 BoltDB，KeyIndex 基于 B-Tree，存储每个 KV 对的历史版本，在内存中 BoltDB 基于 B+Tree 存储在磁盘中。结合 KeyIndex 和 BoltDB 实现 MVCC 机制。



# Raft 协议论文 [In Search of an Understandable Consensus Algorithm](https://raft.github.io/raft.pdf)

`Raft` is a consensus algorithm for **managing a replicated log**. It produces a result equivalent to **(multi-)Paxos**, and it is as efficient as `Paxos`, but its structure is different from `Paxos`; this makes `Raft` more understandable than `Paxos` and also provides a better foundation for building practical systems.

In order to enhance **understandability (易懂)**, `Raft` separates the key elements of consensus, such as **leader election**, **log replication**, and **safety**, and it enforces a stronger degree of coherency to reduce the number of states that must be considered.

Results from a user study demonstrate that `Raft` is easier for students to learn than `Paxos`. `Raft` also includes a new mechanism for changing the cluster membership, which uses overlapping majorities to guarantee safety.

Raft is similar in many ways to existing consensus algorithms, but it has several novel features:

1. **Strong leader**: **Raft uses a stronger form of leadership than other consensus algorithms**. For example, log entries only flow from the leader to other servers. This simplifies the management of the replicated log and makes Raft easier to understand.
2. **Leader election**: **Raft uses randomized timers to elect leaders**. This adds only a small amount of mechanism to the heartbeats already required for any consensus algorithm, while resolving conflicts simply and rapidly.
3. **Membership changes**: Raft’s mechanism for changing the set of servers in the cluster uses a new joint consensus approach where the majorities of
two different configurations overlap during transitions. This allows the cluster to continue operating normally during configuration changes.

> Raft 协议的 "Membership changes" 特性是指 Raft 协议如何处理集群中服务器成员的变化。这是一个非常重要的特性，因为在一个分布式系统中，服务器可能会因为各种原因（比如故障、升级、扩容等）而加入或离开集群。
>
> 在 Raft 协议中，当集群需要改变配置（比如添加或删除服务器）时，会使用一种称为 "joint consensus" 的方法。这个方法的关键在于，它会在旧配置和新配置之间创建一个过渡状态，这个过渡状态的服务器集合是旧配置和新配置的并集。在这个过渡状态中，任何决策（比如选举领导者或者提交日志条目）都需要旧配置和新配置的大多数服务器的同意。这样，即使在配置变化的过程中，集群也能够继续正常运行。
>
> 这种方法的优点是，它可以在不中断服务的情况下进行配置变化。此外，它还可以防止在配置变化过程中出现分裂脑（split-brain）问题，这是一个在分布式系统中常见的问题，可能导致数据不一致。

We believe that `Raft` is superior to `Paxos` and other consensus algorithms, both for educational purposes and as a foundation for implementation. It is simpler and more understandable than other algorithms; it is described completely enough to meet the needs of a practical system; it has several open-source implementations and is used by several companies; its safety properties have been formally specified and proven; and its efficiency is comparable to other algorithms.


## Replicated state machines

Consensus algorithms typically arise in the context of **replicated state machines**. In this approach, state machines on a collection of servers compute identical copies of the same state and can continue operating even if some of the servers are down. Replicated state machines are used to solve a variety of fault tolerance problems in distributed systems.

**Replicated state machines are typically implemented using a replicated log**, as shown in **Figure 1**. **Each server stores a log containing a series of commands, which its state machine executes in order. Each log contains the same commands in the same order, so each state machine processes the same sequence of commands. Since the state machines are deterministic, each computes the same state and the same sequence of outputs**.

Figure 1:

![raft1](/assets/images/202501/raft1.png)

Keeping the replicated log consistent is the job of the consensus algorithm. **The consensus module on a server receives commands from clients and adds them to its log. It communicates with the consensus modules on other servers to ensure that every log eventually contains the same requests in the same order, even if some servers fail. Once commands are properly replicated, each server’s state machine processes them in log order, and the outputs are returned to clients. As a result, the servers appear to form a single, highly reliable state machine**.

Consensus algorithms for practical systems typically have the following properties:

1. **They ensure safety** (never returning an incorrect result) under **all non-Byzantine conditions** (非拜占庭问题), including network delays, partitions, and packet loss, duplication, and reordering.
2. **They are fully functional (available) as long as any majority of the servers are operational and can communicate with each other and with clients**. **Thus, a typical cluster of five servers can tolerate the failure of any two servers**. Servers are assumed to fail by stopping; they may later recover from state on stable
storage and rejoin the cluster.
3. **They do not depend on timing to ensure the consistency of the logs**: faulty clocks and extreme message delays can, at worst, cause availability problems.
4. **In the common case, a command can complete as soon as a majority of the cluster has responded to a single round of remote procedure calls**; a minority of
slow servers need not impact overall system performance.



> [Byzantine and non-Byzantine distributed systems](https://ravendb.net/articles/byzantine-and-non-byzantine-distributed-systems)
>
> The problem was originally posed by Lamport in the Byzantine Generals paper. You have a group of generals that needs to agree on a particular time to attack a city. They can only communicate by (unreliable) messenger, and one or more of them are traitors. The paper itself is interesting to read and the problem is pervasive enough that we now divide distributed systems to Byzantine and non-Byzantine systems.  We now have pervasive cryptography deployed, to the point where you read this post over an encrypted channel, verified using public key infrastructure to validate that it indeed came from me. You can solve the Byzantine generals problem easily now.
>
> Today, the terminology changed. We now refer to Byzantine networks as systems where some of the nodes are malicious and non-Byzantine as systems where we trust that other nodes will do their task. For example, Raft or Paxos are both distributed consensus algorithms that assumes a non-Byzantine system. Oh, the network communication gores through hostile environment, but that is why we have TLS for. Authentication and encryption over the wire are mostly a solved problem at this point. It isn’t a simple problem, but it is a solved one.



## Raft consensus algorithm

**Figure 2** summarizes the algorithm in condensed (概要) form for reference, and **Figure 3** lists key properties of the algorithm; the elements of these figures are discussed piecewise over the rest of this section.

Raft implements consensus by first electing a distinguished leader, then giving the leader complete responsibility for managing the replicated log. The leader accepts log entries from clients, replicates them on other servers, and tells servers when it is safe to apply log entries to their state machines.

Having a leader simplifies the management of the replicated log. For example, the leader can decide where to place new entries in the log without consulting other servers, and data flows in a simple fashion from the leader to other servers. A leader can fail or become disconnected from the other servers, in which case
a new leader is elected.

Given the leader approach, **`Raft` decomposes the consensus problem into three relatively independent subproblems**, which are discussed in the subsections that follow:

1. **Leader election**: a new leader must be chosen when an existing leader fails.
2. **Log replication**: the leader must accept log entries from clients and replicate them across the cluster, forcing the other logs to agree with its own.
3. **Safety**: the key safety property for Raft is the State Machine Safety Property in **Figure 3**: if any server has applied a particular log entry to its state machine, then no other server may apply a different command for the same log index.


Figure 2:

![raft2](/assets/images/202501/raft2.png)

Figure 3:

![raft3](/assets/images/202501/raft3.png)

## Raft basics

A Raft cluster contains several servers; **five is a typical number, which allows the system to tolerate two failures**. At any given time each server is in one of three states: `leader`, `follower`, or `candidate`. In normal operation there is exactly one leader and all of the other servers are followers. Followers are passive: they issue no requests on their own but simply respond to requests from leaders and candidates. The leader handles all client requests (if
a client contacts a follower, the follower redirects it to the leader). The third state, candidate, is used to elect a new leader as described in Section 5.2. **Figure 4** shows the states and their transitions; the transitions are discussed below.

Figure 4:

![raft4](/assets/images/202501/raft4.png)

Raft divides time into terms of arbitrary length, as shown in **Figure 5**. Terms are numbered with consecutive integers. Each term begins with an election, in which one or more candidates attempt to become leader as described in Section 5.2. If a candidate wins the election, then it serves as leader for the rest of the term. In some situations an election will result in a split vote. In this case the term will end with no leader; a new term (with a new election) will begin shortly. Raft ensures that there is at most one leader in a given term.

Figure 5:

![raft5](/assets/images/202501/raft5.png)

Different servers may observe the transitions between terms at different times, and in some situations a server may not observe an election or even entire terms. Terms act as **a logical clock** in Raft, and they allow servers to detect obsolete (过时的) information such as stale leaders. Each server stores a current term number, which increases monotonically over time. Current terms are exchanged whenever servers communicate; if one server’s current term is smaller than the other’s, then it updates its current term to the larger value. If a candidate or leader discovers that its term is out of date, it immediately reverts to follower state. If a server receives a request with a stale term number, it rejects the request.


在 Raft 协议中，"Term" 是一个非常重要的概念。每个 Term 代表一个选举周期，它可以被视为一个逻辑时钟，帮助服务器检测过时的信息，如陈旧的领导者。每个服务器都存储一个当前的 Term 数字，这个数字随着时间单调递增。每当服务器之间进行通信时，都会交换当前的 Term。如果一个服务器发现自己的当前 Term 小于另一个服务器的 Term，那么它会更新自己的当前 Term 为较大的值。

这种机制有几个重要的效果：

如果一个候选人或领导者发现自己的 Term 已经过时（即，存在一个更大的 Term），那么它会立即变回 Follower 状态。这是因为一个更大的 Term 意味着已经有一个新的选举开始，而这个服务器没有参与。为了保持一致性，它必须放弃当前的候选人或领导者角色，变回 Follower。如果一个服务器收到一个请求，但是这个请求的 Term 小于它自己的当前 Term，那么它会拒绝这个请求。这是因为一个较小的 Term 意味着这个请求是在一个过去的选举周期中产生的，因此是过时的。通过这种方式，Raft 协议能够确保集群中的所有服务器都能够在选举过程中保持一致性，即使在网络延迟或者服务器故障的情况下。


Raft servers communicate using remote procedure calls (RPCs), and **the basic consensus algorithm requires only two types of RPCs**. **RequestVote RPCs** are initiated by candidates during elections (Section 5.2), and **AppendEntries RPCs** are initiated by leaders to replicate log entries and to provide a form of heartbeat (Section 5.3). Section 7 adds **a third RPC for transferring snapshots between servers**. Servers retry RPCs if they do not receive a response in a timely manner, and they issue RPCs in parallel for best performance.

## Performance (举领导者和复制日志条目等关键操作上的性能)

Raft’s performance is similar to other consensus algorithms such as Paxos.

**The most important case for performance is when an established leader is replicating new log entries**. Raft achieves this using the minimal number of messages (a single round-trip from the leader to half the cluster). It is also possible to further improve Raft’s performance. For example, it easily supports batching and pipelining requests for higher throughput and lower latency. Various optimizations have been proposed in the literature for other algorithms; many of these could be applied to Raft, but we leave this to future work.

We used our Raft implementation to measure the performance of Raft’s leader election algorithm and answer two questions. First, does the election process converge quickly? Second, what is the minimum downtime that can be achieved after leader crashes?

To measure leader election, we repeatedly crashed the leader of a cluster of five servers and timed how long it took to detect the crash and elect a new leader (see **Figure 16**). **To generate a worst-case scenario**, the servers in each trial had different log lengths, so some candidates were not eligible to become leader. Furthermore, to encourage split votes, our test script triggered a synchronized broadcast of heartbeat RPCs from the leader before terminating its process (this approximates the behavior of the leader replicating a new log entry prior to crashing). The leader was crashed uniformly randomly within its heartbeat interval, which was half of the minimum election timeout for all tests. Thus, the smallest possible downtime was about half of the minimum election timeout.

**The top graph** in **Figure 16** shows that a small amount of randomization in the election timeout is enough to avoid split votes in elections. In the absence of randomness, leader election consistently took longer than 10 seconds in our tests due to many split votes. Adding just 5ms of randomness helps significantly, resulting in a median downtime of 287ms. Using more randomness improves worst-case behavior: with 50ms of randomness the worstcase completion time (over 1000 trials) was 513ms.

**The bottom graph** in **Figure 16** shows that downtime can be reduced by reducing the election timeout. With an election timeout of 12–24ms, it takes only 35ms on average to elect a leader (the longest trial took 152ms). However, lowering the timeouts beyond this point violates Raft’s timing requirement: leaders have difficulty broadcasting heartbeats before other servers start new elections. This can cause unnecessary leader changes and lower overall system availability. We recommend using a conservative election timeout such as 150–300ms; such timeouts are unlikely to cause unnecessary leader changes and will still provide good availability.

Figure 6:

![raft6](/assets/images/202501/raft6.png)

> Raft 协议在选举领导者和复制日志条目等关键操作上的性能表现，以及如何通过调整参数来优化性能。
>
> 首先，Raft 协议在复制新的日志条目时的性能与 Paxos 等其他一致性算法相当。这是因为 Raft 协议使用了最少的消息数量（从领导者到集群一半的服务器只需要一个往返的消息）来完成这个操作。此外，Raft 协议还支持批处理和流水线请求，以提高吞吐量和降低延迟。
>
> 然后，通过实验测量了 Raft 协议的领导者选举算法的性能。实验的目标是回答两个问题：选举过程是否能快速收敛？在领导者崩溃后，可以达到的最小停机时间是多少？
>
> 实验结果显示，通过在选举超时时间中引入一定的随机性，可以有效地避免选举过程中的分裂投票，从而加快选举速度。在没有随机性的情况下，由于分裂投票的问题，选举过程在测试中通常需要超过 10 秒。而添加了 5ms 的随机性后，中位数的停机时间就降低到了 287ms。增加更多的随机性可以进一步改善最坏情况的表现：在添加了 50ms 的随机性后，最坏情况下的完成时间（在 1000 次试验中）是 513ms。
>
> 此外，实验还发现，通过减小选举超时时间，可以进一步减少停机时间。当选举超时时间为 12-24ms 时，选举领导者的平均时间只需要 35ms（最长的试验需要 152ms）。然而，如果进一步降低超时时间，就可能违反 Raft 协议的时间要求：领导者可能在其他服务器开始新的选举之前，无法广播心跳消息。这可能导致不必要的领导者更换，降低整个系统的可用性。因此，建议使用较为保守的选举超时时间，如 150-300ms。这样的超时时间不太可能导致不必要的领导者更换，同时仍能提供良好的可用性。

Q: **在没有随机性的情况下，为什么会导致分裂投票的问题？**

A: 在 Raft 协议中，分裂投票（split vote）的问题通常发生在领导者崩溃或网络分区等情况下。当一个或多个 Follower 服务器在一段时间内没有收到领导者的心跳消息时，它们会认为领导者已经崩溃，然后开始新的选举。

如果没有随机性，那么所有的 Follower 服务器都会在相同的时间开始新的选举。这就可能导致分裂投票的问题，因为每个服务器都有可能成为候选人，并且它们都会在开始选举时给自己投票。如果集群中的服务器数量是偶数，那么投票就可能会被平分，导致没有任何一个候选人能够得到大多数的票数，从而无法选出新的领导者。这样，集群就会进入一个无领导者的状态，直到下一轮的选举开始。

为了解决这个问题，Raft 协议引入了随机性。每个 Follower 服务器在开始新的选举之前，都会等待一个随机的超时时间。这样，一些服务器会比其他服务器更早开始选举，从而有更大的机会成为新的领导者。这就大大降低了分裂投票问题的发生概率，从而提高了选举的效率和集群的稳定性。




# Install [v3.5](https://etcd.io/docs/v3.5/)

Instructions for installing `etcd` from pre-built binaries or from source.

## Requirements

* [Supported platforms](https://etcd.io/docs/v3.5/op-guide/supported-platform/)
  + etcd support for common architectures & operating systems

* [Hardware recommendations](https://etcd.io/docs/v3.5/op-guide/hardware/)
  + etcd usually runs well with limited resources for development or testing purposes; it’s common to develop with etcd on a laptop or a cheap cloud machine. However, when running etcd clusters in production, some hardware guidelines are useful for proper administration. These suggestions are not hard rules; they serve as a good starting point for a robust production deployment. As always, deployments should be tested with simulated workloads before running in production.

> CPUs >= 16 cores

Few etcd deployments require a lot of CPU capacity. Typical clusters need two to four cores to run smoothly. Heavily loaded etcd deployments, serving thousands of clients or tens of thousands of requests per second, tend to be CPU bound since etcd can serve requests from memory. Such heavy deployments usually need eight to sixteen dedicated cores.

> Memory >= 64GB

etcd has a relatively small memory footprint but its performance still depends on having enough memory. An etcd server will aggressively cache key-value data and spends most of the rest of its memory tracking watchers. Typically 8GB is enough. For heavy deployments with thousands of watchers and millions of keys, allocate 16GB to 64GB memory accordingly.

> Disks SSD

Fast disks are the most critical factor for etcd deployment performance and stability.

A slow disk will increase etcd request latency and potentially hurt cluster stability. **Since etcd’s consensus protocol depends on persistently storing metadata to a log, a majority of etcd cluster members must write every request down to disk. Additionally, etcd will also incrementally checkpoint its state to disk so it can truncate this log.** If these writes take too long, heartbeats may time out and trigger an election, undermining the stability of the cluster. In general, to tell whether a disk is fast enough for etcd, a benchmarking tool such as [fio](https://github.com/axboe/fio) can be used. Read [here](https://web.archive.org/web/20240726111518/https://prog.world/is-storage-speed-suitable-for-etcd-ask-fio/) for an example.

etcd is very sensitive to disk write latency. Typically 50 **sequential IOPS** (e.g., a 7200 RPM disk) is required. For heavily loaded clusters, 500 sequential IOPS (e.g., a typical local SSD or a high performance virtualized block device) is recommended. Note that most cloud providers publish concurrent IOPS rather than sequential IOPS; the published concurrent IOPS can be 10x greater than the sequential IOPS. To measure actual sequential IOPS, we suggest using a disk benchmarking tool such as [diskbench](https://github.com/ongardie/diskbenchmark) or [fio](https://github.com/axboe/fio).

etcd requires only modest **disk bandwidth **but more disk bandwidth buys faster recovery times when a failed member has to catch up with the cluster. Typically 10MB/s will recover 100MB data within 15 seconds. For large clusters, 100MB/s or higher is suggested for recovering 1GB data within 15 seconds.

When possible, back etcd’s storage with a SSD. A SSD usually provides lower write latencies and with less variance than a spinning disk, thus improving the stability and reliability of etcd. If using spinning disk, get the fastest disks possible (15,000 RPM). Using RAID 0 is also an effective way to increase disk speed, for both spinning disks and SSD. With at least three cluster members, mirroring and/or parity variants of RAID are unnecessary; etcd’s consistent replication already gets high availability.

> Network

Multi-member etcd deployments benefit from a fast and reliable network. In order for etcd to be both consistent and partition tolerant, an unreliable network with partitioning outages will lead to poor availability. Low latency ensures etcd members can communicate fast. High bandwidth can reduce the time to recover a failed etcd member. 1GbE is sufficient for common etcd deployments. For large etcd clusters, a 10GbE network will reduce mean time to recovery.

Deploy etcd members within a single data center when possible to avoid latency overheads and lessen the possibility of partitioning events. If a failure domain in another data center is required, choose a data center closer to the existing one. Please also read the [tuning](https://etcd.io/docs/v3.5/tuning/) documentation for more information on cross data center deployment.

* Example hardware configurations

Here are a few example hardware setups on `AWS` and `GCE` environments. As mentioned before, but must be stressed regardless, administrators should test an etcd deployment with a simulated workload before putting it into production.

> Small cluster

A small cluster serves fewer than 100 clients, fewer than 200 of requests per second, and stores no more than 100MB of data.

Example application workload: A 50-node Kubernetes cluster

![etcd1](/assets/images/202501/etcd1.png)

> Medium cluster

A medium cluster serves fewer than 500 clients, fewer than 1,000 of requests per second, and stores no more than 500MB of data.

Example application workload: A 250-node Kubernetes cluster

![etcd2](/assets/images/202501/etcd2.png)

> Large cluster

A large cluster serves fewer than 1,500 clients, fewer than 10,000 of requests per second, and stores no more than 1GB of data.

Example application workload: A 1,000-node Kubernetes cluster

![etcd3](/assets/images/202501/etcd3.png)

> xLarge cluster

An xLarge cluster serves more than 1,500 clients, more than 10,000 of requests per second, and stores more than 1GB data.

Example application workload: A 3,000 node Kubernetes cluster

![etcd4](/assets/images/202501/etcd4.png)


----------------

These docs cover everything from setting up and running an etcd cluster to using etcd in applications.

Follow these instructions to locally install, run, and test **a single-member cluster of etcd**:

Install etcd from pre-built binaries or from source. For details, see [Install](https://etcd.io/docs/v3.5/install/).


## Install pre-built binaries

The easiest way to install etcd is from pre-built binaries:

1. Download the compressed archive file for your platform from [Releases](https://github.com/etcd-io/etcd/releases/), choosing release [v3.5.17](https://github.com/etcd-io/etcd/releases/tag/v3.5.17) or later.
2. Unpack the archive file. This results in a directory containing the binaries.
3. Add the executable binaries to your path. For example, rename and/or move the binaries to a directory in your path (like `/usr/local/bin`), or add the directory created by the previous step to your path.
4. From a shell, test that `etcd` is in your path.

``` bash
$ etcd --version
etcd Version: 3.5.17
Git SHA: 507c0de
Go Version: go1.22.9
Go OS/Arch: linux/amd64
```

### etcd 简单安装和测试功能脚本

``` bash
#!/bin/bash

ETCD_VER=v3.5.17

# Check if the user has root privileges
if [ "$(id -u)" != "0" ]; then
    echo "This script must be run as root" 1>&2
        exit 1
fi

# choose either URL
GOOGLE_URL=https://storage.googleapis.com/etcd
GITHUB_URL=https://github.com/etcd-io/etcd/releases/download
DOWNLOAD_URL=${GOOGLE_URL}

echo "Preparing to download etcd version ${ETCD_VER}..."

# Clean up any previous downloads
rm -f /tmp/etcd-${ETCD_VER}-linux-amd64.tar.gz
rm -rf /tmp/etcd-download-test && mkdir -p /tmp/etcd-download-test

# Download etcd
echo "Downloading etcd from ${DOWNLOAD_URL}..."
curl -L ${DOWNLOAD_URL}/${ETCD_VER}/etcd-${ETCD_VER}-linux-amd64.tar.gz -o /tmp/etcd-${ETCD_VER}-linux-amd64.tar.gz
if [ $? -ne 0 ]; then
    echo "Download failed! Exiting."
    exit 1
fi

# Extract the tarball
echo "Extracting etcd tarball..."
tar xzvf /tmp/etcd-${ETCD_VER}-linux-amd64.tar.gz -C /tmp/etcd-download-test --strip-components=1
if [ $? -ne 0 ]; then
    echo "Extraction failed! Exiting."
    exit 1
fi

# Clean up the tarball
rm -f /tmp/etcd-${ETCD_VER}-linux-amd64.tar.gz

# Move etcd binaries to /usr/local/bin
echo "Copy etcd binaries to /usr/local/bin..."
sudo cp /tmp/etcd-download-test/etcd /usr/local/bin/
sudo cp /tmp/etcd-download-test/etcdctl /usr/local/bin/
sudo cp /tmp/etcd-download-test/etcdutl /usr/local/bin/

# Check versions
echo "Checking etcd version..."
etcd --version
etcdctl version
etcdutl version

# Start a local etcd server
echo "Starting local etcd server..."
/tmp/etcd-download-test/etcd &
ETCD_PID=$!

# Give etcd some time to start
sleep 10

# Write and read to etcd
echo "Writing and reading data to etcd..."
/tmp/etcd-download-test/etcdctl --endpoints=localhost:2379 put foo bar
/tmp/etcd-download-test/etcdctl --endpoints=localhost:2379 get foo

# Kill the etcd server
kill ${ETCD_PID}
```

### etcd 启动脚本

``` bash
#!/bin/bash

function CheckCmdExists()
{
  command -v "$1" >/dev/null 2>&1
}

##################

# etcd configuration
ETCD_NAME=${ETCD_NAME:-JLib-etcd-1}                                 # --name: The name of the etcd member
ETCD_DATA_DIR=${ETCD_DATA_DIR:-$HOME/tools/etcd/etcd-data}          # --data-dir: The directory to store etcd's data

LISTEN_CLIENT_URLS=${LISTEN_CLIENT_URLS:-http://0.0.0.0:2379}       # --listen-client-urls: List of URLs to listen on for client traffic
ADVERTISE_CLIENT_URLS=${ADVERTISE_CLIENT_URLS:-http://0.0.0.0:2379} # --advertise-client-urls: List of this member’s client URLs to advertise to the rest of the cluster

LISTEN_PEER_URLS=${LISTEN_PEER_URLS:-http://0.0.0.0:2380}           # --listen-peer-urls: List of URLs to listen on for peer traffic
INITIAL_ADVERTISE_PEER_URLS=${INITIAL_ADVERTISE_PEER_URLS:-http://0.0.0.0:2380} # --initial-advertise-peer-urls: List of this member’s peer URLs to advertise to the rest of the cluster

INITIAL_CLUSTER=${INITIAL_CLUSTER:-$ETCD_NAME=http://0.0.0.0:2380}  # --initial-cluster: Initial cluster configuration for bootstrapping
INITIAL_CLUSTER_TOKEN=${INITIAL_CLUSTER_TOKEN:-JLib-etcd-token}     # --initial-cluster-token: Initial cluster token for the etcd cluster during bootstrap
INITIAL_CLUSTER_STATE=${INITIAL_CLUSTER_STATE:-new}                 # --initial-cluster-state: Initial cluster state ("new" or "existing")

WAIT_TIME=${WAIT_TIME:-10}


# check etcd has been installed
if CheckCmdExists "etcd"; then
  echo "etcd version: $(etcd --version)"
else
  echo "etcd was not found"
  exit 1
fi

# start etcd
etcd \
  --name $ETCD_NAME \
  --data-dir $ETCD_DATA_DIR \
  --listen-client-urls $LISTEN_CLIENT_URLS \
  --advertise-client-urls $ADVERTISE_CLIENT_URLS \
  --listen-peer-urls $LISTEN_PEER_URLS \
  --initial-advertise-peer-urls $INITIAL_ADVERTISE_PEER_URLS \
  --initial-cluster $INITIAL_CLUSTER \
  --initial-cluster-token $INITIAL_CLUSTER_TOKEN \
  --initial-cluster-state $INITIAL_CLUSTER_STATE \
  > etcd.log 2>&1 &

ETCD_PID=$!

# wait for etcd to start
for i in $(seq 1 $WAIT_TIME); do
  if ps -p $ETCD_PID > /dev/null; then
    echo "etcd started successfully"
        exit 0
  fi
    echo "Waiting for etcd to start..."
    sleep 1
done

echo "etcd failed to start"
exit 1
```

启动参数：

``` bash
etcd --name JLib-etcd-1 --data-dir /data/home/gerryyang/tools/etcd/etcd-data --listen-client-urls http://0.0.0.0:2379 --advertise-client-urls http://0.0.0.0:2379 --listen-peer-urls http://0.0.0.0:2380 --initial-advertise-peer-urls http://0.0.0.0:2380 --initial-cluster JLib-etcd-1=http://0.0.0.0:2380 --initial-cluster-token JLib-etcd-token --initial-cluster-state new
```

etcd 默认启动参数：

```
starting an etcd server","etcd-version":"3.5.17","git-sha":"507c0de","go-version":"go1.22.9","go-os":"linux","go-arch":"amd64","max-cpu-set":48,"max-cpu-available":48,"member-initialized":false,"name":"default","data-dir":"default.etcd","wal-dir":"","wal-dir-dedicated":"","member-dir":"default.etcd/member","force-new-cluster":false,"heartbeat-interval":"100ms","election-timeout":"1s","initial-election-tick-advance":true,"snapshot-count":100000,"max-wals":5,"max-snapshots":5,"snapshot-catchup-entries":5000,"initial-advertise-peer-urls":["http://localhost:2380"],"listen-peer-urls":["http://localhost:2380"],"advertise-client-urls":["http://localhost:2379"],"listen-client-urls":["http://localhost:2379"],"listen-metrics-urls":[],"cors":["*"],"host-whitelist":["*"],"initial-cluster":"default=http://localhost:2380","initial-cluster-state":"new","initial-cluster-token":"etcd-cluster","quota-backend-bytes":2147483648,"max-request-bytes":1572864,"max-concurrent-streams":4294967295,"pre-vote":true,"initial-corrupt-check":false,"corrupt-check-time-interval":"0s","compact-check-time-enabled":false,"compact-check-time-interval":"1m0s","auto-compaction-mode":"periodic","auto-compaction-retention":"0s","auto-compaction-interval":"0s","discovery-url":"","discovery-proxy":"","downgrade-check-interval":"5s"
```


### etcd 用例测试脚本

``` bash
#!/bin/bash

ENDPOINT=localhost:2379

# test write
echo "Writing data to etcd..."
etcdctl --endpoints=$ENDPOINT put foo bar

# test read
echo "Reading data from etcd..."
etcdctl --endpoints=$ENDPOINT get foo

# test list
echo "Listing all keys in etcd..."
etcdctl --endpoints=$ENDPOINT get "" --prefix=true

# test delete
echo "Deleting data from etcd..."
etcdctl --endpoints=$ENDPOINT del foo

# test list after delete
echo "Listing all keys in etcd after deletion..."
etcdctl --endpoints=$ENDPOINT get "" --prefix=true
```

输出：

```
Writing data to etcd...
OK
Reading data from etcd...
foo
bar
Listing all keys in etcd...
foo
bar
Deleting data from etcd...
1
Listing all keys in etcd after deletion...
```



## Build from source

If you have [Go version 1.2+](https://golang.org/doc/install), you can build etcd from source by following these steps:

* [Download the etcd repo as a zip file](https://github.com/etcd-io/etcd/archive/v3.5.17.zip) and unzip it, or clone the repo using the following command.

``` bash
$ git clone -b v3.5.17 https://github.com/etcd-io/etcd.git
```

> To build from `main@HEAD`, omit the `-b v3.5.17` flag.

* Change directory:

``` bash
$ cd etcd
```

* Run the build script:

``` bash
$ ./build.sh
```

The binaries are under the `bin` directory.

* Add the full path to the `bin` directory to your path, for example:

``` bash
$ export PATH="$PATH:`pwd`/bin"
```

* Test that `etcd` is in your path:

``` bash
$ etcd --version
```

## Installation on Kubernetes, using a statefulset or helm chart

The etcd project does not currently maintain a helm chart, however you can follow the instructions provided by [Bitnami’s etcd Helm chart](https://bitnami.com/stack/etcd/helm).




# 读取模型

在 `etcd` 中，有两种读取请求：**线性一致性**（`linearizable`）读取和**可串行化**（`serializable`）读取。这两种读取请求之间的主要区别在于它们**如何确保数据的一致性和最新性**。

**线性一致性读取**和**可串行化读取**在**性能和数据一致性之间进行权衡**。**线性一致性读取**提供了最新的数据，但代价是更高的延迟和开销。**可串行化读取**具有较低的延迟和开销，但可能会返回过时的数据。根据应用程序对一致性和性能的需求，可以选择使用适当的读取请求类型。


## 线性一致性读取（Linearizable read）

**线性一致性读取**会通过集群成员的法定人数（`quorum`）进行共识以获取最新的数据。这意味着，当一个线性一致性读取请求被发出时，它需要集群中的大多数成员（即法定人数）同意返回的数据是最新的。这确保了您读取的数据是最新的，但代价是更高的延迟和开销，因为需要在集群成员之间进行通信以达成共识。

## 可串行化读取（Serializable read）

与线性一致性读取相比，**可串行化读取**的成本更低，因为它们可以由单个 `etcd` 成员提供，而**无需在集群成员之间达成共识**。这意味着，当一个可串行化读取请求被发出时，它可以直接从任何一个 `etcd` 成员获取数据，而无需等待其他成员的同意。这可以减少延迟和开销，但可能会返回过时的数据，因为数据可能尚未在集群中完全同步。


# Watch 机制 (高效获取数据变化)

![raft7](/assets/images/202501/raft7.png)


1. 当通过 etcd client 发起 watch 请求的时候，首先会通过 gRPC Proxy，这一部分会将多个 watch 请求合并减少请求处理负担。

2. 创建一个 serverWatchStream，当收到 watch key 请求会创建两个协程 recvLoop 和 sendLoop，recvLoop 负责接收 client create/cancel watcher 的请求，并将从管道中收到的请求转发给 sendLoop 最终发给 client。

3. serverWatchStream 收到 client 创建 watcher 的请求后会创建一个 WatchStream 并分配一个 watcherid，每个 watcher 对应唯一的 watcherid

4. 一旦 watcher 创建成功就会存储在 watchableStore 的 synced watcher 中，如果监听到版本号变化，则会将 watcher 放入 unsyned watcher 中。版本号变化通过 MVCC 得知，通过 watcher.ch 管道进行信息中转。同时 synced watcher 和 unsynced watcher 底层有 map 和 interval tree 两个数据结构，分别对应单个 key 的监控和区间 key 监控。

5. etcd 启动时会创建 syncWatchersLoop 和 syncVictimsLoop 协程，进行 watcher 的同步操作。

6. 用户通过 Raft 机制写入一致性数据，通过 MVCC 机制产生 Event 事件，并写入到管道中。


# 事务机制

事务，将应用程序的多个读写操作合并成一个逻辑操作单元，即事务中的操作要么成功提交，要么失败回滚，即使失败也可以在应用层进行重试。几乎所有的**关系型数据库** (例如，Mysql) 都支持事务处理，然而**当非关系型 (NoSQL) 数据库**兴起后，事务逐渐开始被放弃。非关系型数据库主要通过复制、分区的方式来提升系统的可扩展性和可用性，有更灵活的数据格式，而且一般是分布式设计。一方面，实现事务意味着降低性能，如果是跨 IDC 部署，那么就得用 `2PC` 等方式实现分布式事务，实现难度变得很大；另一方面，很多场景下事务又是不可或缺的，比如支付转账的场景，对异常极其敏感，不使用事务实现会需要在应用层做更多的兜底逻辑。

etcd 基于 MVCC 机制实现了事务，许多使用了 MVCC 的数据库都不会暴露底层 MVCC 信息，例如 Mysql 的事务操作基于 MVCC 实现，基于 `START TRANSACTION`、`COMMIT`、`ROLLBACK`就可以实现事务的创建、提交、回滚操作。

一个 etcd 事务的例子：

这个事务首先检查键 "key" 的值是否等于 "value"。如果条件满足，那么它会将 "key" 的值更新为 "new_value"；否则，它会获取 "key" 的当前值。这个事务是一个原子操作，它要么全部执行成功，要么全部执行失败，不会出现部分成功部分失败的情况。

``` go
_, err := cli.Txn(ctx).
  If(clientv3.Compare(clientv3.Value("key"), "=", "value")).
  Then(clientv3.OpPut("key", "new_value")).
  Else(clientv3.OpGet("key")).
  Commit()
```

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


# Q&A

## Running http and grpc server on single port. This is not recommended for production

etcd 服务同时在同一个端口上运行了 HTTP 和 gRPC 服务。在生产环境中，这种配置并不推荐，因为它可能会导致性能问题或者其他潜在的问题。etcd 默认使用 gRPC 进行通信，但是它也提供了一个 HTTP API 以便于向后兼容。在生产环境中，通常建议将 HTTP 和 gRPC 服务分别运行在不同的端口上，以便于管理和监控。









# Refer

* https://etcd.io/docs/v3.5/
* https://etcd.io/docs/v3.4/op-guide/performance/











