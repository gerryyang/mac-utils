---
layout: post
title:  "ETCD in Action"
date:   2023-09-09 09:00:00 +0800
categories: GoLang
---

* Do not remove this line (it will not be displayed)
{:toc}


# 抽屉原理

**桌上有十个苹果，要把这十个苹果放到九个抽屉里，无论怎样放，我们会发现至少会有一个抽屉里面放不少于两个苹果。这一现象就是我们所说的“抽屉原理”**。

**抽屉原理的一般含义**为：“如果每个抽屉代表一个集合，每一个苹果就可以代表一个元素，假如有 `n+1` 个元素放到 `n` 个集合中去，其中必定有一个集合里至少有两个元素。”

抽屉原理有时也被称为**鸽巢原理**。它是**组合数学中一个重要的原理**。





# 历史背景

* etcd 名字的由来？它源于两个方面，Unix 的 **/etc** 文件夹和分布式系统 (**D**istribute system) 的 **D**，组合在一起表示 **etcd** 是用于存储分布式配置的存储服务。
* CoreOS 团队在 **2013 年 8 月**对外发布了**第一个测试版本 v0.1**，API v1 版本，命名为 etcd。
* v0.1 版本实现了简单的 HTTP Get/Set/Delete/Watch API，但读数据一致性无法保证。
* v0.2 版本，支持通过指定 consistent 模式，从 Leader 读取数据，并将 Test And Set 机制修正为 CAS(Compare And Swap)，解决原子更新的问题，同时发布了新的 API 版本 v2。
* 在 **2015 年 1 月**，CoreOS 发布了 etcd **第一个稳定版本 2.0**，支持了 **quorum read**，提供了严格的线性一致性读能力。7 月，基于 etcd 2.0 的 Kubernetes 第一个生产环境可用版本 v1.0.1 发布了，Kubernetes 开始了新的里程碑的发展。

![etcd6](/assets/images/202501/etcd6.png)

![etcd7](/assets/images/202501/etcd7.png)

* 随着 Kubernetes 项目不断发展，v2 版本的瓶颈和缺陷逐渐暴露，遇到了若干性能和稳定性问题，Kubernetes 社区呼吁支持新的存储、批评 etcd 不可靠的声音开始不断出现。具体问题如下：
  + **功能局限性问题。**
    - etcd v2 不支持范围查询和分页。分页对于数据较多的场景是必不可少的。在 Kubernetes 中，在集群规模增大后，Pod、Event 等资源可能会出现数千个以上，但是 etcd v2 不支持分页，不支持范围查询，大包等 expensive request 会导致严重的性能乃至雪崩问题。
    - etcd v2 不支持多 key 事务。在实际转账等业务场景中，往往我们需要在一个事务中同时更新多个 key。
  + **Watch 机制可靠性问题。**
    - Kubernetes 项目严重依赖 etcd Watch 机制，然而 etcd v2 是内存型、不支持保存 key 历史版本的数据库，只在内存中使用滑动窗口保存了最近的 1000 条变更事件，当 etcd server 写请求较多、网络波动时等场景，很容易出现事件丢失问题，进而又触发 client 数据全量拉取，产生大量 expensive request，甚至导致 etcd 雪崩。
  + **性能瓶颈问题。**
    - etcd v2 早期使用了简单、易调试的 HTTP/1.x API，但是随着 Kubernetes 支撑的集群规模越来越大，HTTP/1.x 协议的瓶颈逐渐暴露出来。比如集群规模大时，由于 HTTP/1.x 协议没有压缩机制，批量拉取较多 Pod 时容易导致 APIServer 和 etcd 出现 CPU 高负载、OOM、丢包等问题。
    - 另外，etcd v2 client 会通过 HTTP 长连接轮询 Watch 事件，当 watcher 较多的时候，因 HTTP/1.x 不支持多路复用，会创建大量的连接，消耗 server 端过多的 socket 和内存资源。
    - 同时 etcd v2 支持为每个 key 设置 TTL 过期时间，client 为了防止 key 的 TTL 过期后被删除，需要周期性刷新 key 的 TTL。实际业务中很有可能若干 key 拥有相同的 TTL，可是在 etcd v2 中，即使大量 key TTL 一样，你也需要分别为每个 key 发起续期操作，当 key 较多的时候，这会显著增加集群负载、导致集群性能显著下降。
  + **内存开销问题。**
    - etcd v2 在内存维护了一颗树来保存所有节点 key 及 value。在数据量略大的场景，如配置项较多、存储了大量 Kubernetes Events，它会导致较大的内存开销，同时 etcd 需要定时把全量内存树持久化到磁盘。这会消耗大量的 CPU 和磁盘 I/O 资源，对系统的稳定性造成一定影响。

![etcd8](/assets/images/202501/etcd8.png)


> 为什么 etcd v2 有以上若干问题，Consul 等其他竞品依然没有被 Kubernetes 支持呢？

1. 一方面当时包括 Consul 在内，没有一个开源项目是十全十美完全满足 Kubernetes 需求。而 CoreOS 团队一直在聆听社区的声音并积极改进，解决社区的痛点。用户吐槽 etcd 不稳定，他们就设计实现自动化的测试方案，模拟、注入各类故障场景，及时发现修复 Bug，以提升 etcd 稳定性。
2. 另一方面，用户吐槽性能问题，针对 etcd v2 各种先天性缺陷问题，他们从 2015 年就开始设计、实现新一代 etcd v3 方案去解决以上痛点，并积极参与 Kubernetes 项目，负责 etcd v2 到 v3 的存储引擎切换，推动 Kubernetes 项目的前进。同时，设计开发通用压测工具、输出 Consul、ZooKeeper、etcd 性能测试报告，证明 etcd 的优越性。

> etcd v3 就是为了解决以上稳定性、扩展性、性能问题而诞生的。

1. 在内存开销、Watch 事件可靠性、功能局限上，它通过引入 B-tree、boltdb 实现一个 MVCC 数据库，数据模型从层次型目录结构改成扁平的 key-value，提供稳定可靠的事件通知，实现了事务，支持多 key 原子更新，同时基于 boltdb 的持久化存储，显著降低了 etcd 的内存占用、避免了 etcd v2 定期生成快照时的昂贵的资源开销。
2. 性能上，首先 etcd v3 使用了 gRPC API，使用 protobuf 定义消息，消息编解码性能相比 JSON 超过 2 倍以上，并通过 HTTP/2.0 多路复用机制，减少了大量 watcher 等场景下的连接数。
3. 其次使用 Lease 优化 TTL 机制，每个 Lease 具有一个 TTL，相同的 TTL 的 key 关联一个 Lease，Lease 过期的时候自动删除相关联的所有 key，不再需要为每个 key 单独续期。
4. 最后是 etcd v3 支持范围、分页查询，可避免大包等 expensive request。

> **2016 年 6 月，etcd 3.0 诞生**，随后 Kubernetes 1.6 发布，默认启用 etcd v3，助力 **Kubernetes 支撑 5000 节点集群规模**。

下面的时间轴图总结了 etcd3 重要特性及版本发布时间。从图中可以看出，从 3.0 到未来的 3.5，更稳、更快是 etcd 的追求目标。

![etcd9](/assets/images/202501/etcd9.png)

> 从 2013 年发布第一个版本 v0.1 到今天的 3.5.0-pre，从 v2 到 v3，etcd 走过了 7 年的历程，etcd 的稳定性、扩展性、性能不断提升。在 Kubernetes 的业务场景磨炼下它不断成长，走向稳定和成熟，成为技术圈众所周知的开源产品，而 v3 方案的发布，也标志着 etcd 进入了技术成熟期，成为云原生时代的首选元数据存储产品。







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


# 方案选型

方案需要具备的能力：

1. 高可用（方案：多节点 -> 问题：数据一致性问题）
2. 数据一致性（方案：Raft 协议）
3. 存储容量（方案：仅需保存控制面信息，存储上不需要考虑分片）
4. 增删改查，监听数据变化（方案：Watch 机制）
5. 可维护性（方案：提供 API 等易便的操作接口，降低运维复杂度）

可选方案：

* ZooKeeper
  + 维护成本相对较高
  + Java 程序部署繁琐且资源开销较高
  + 私有 TCP 协议通信，不支持 HTTP 协议调用，调试不方便

* etcd
  + Kubernetes 使用 etcd 作为底层存储
  + Raft 共识算法来确保数据在集群中的一致性，相比 Paxos 算法工程化更简单
  + 支持 gRPC 和 HTTP 协议调用
  + Watch 机制
  + Key TTL 特性
  + 服务发现
  + 分布式锁
  + 主备选举
  + 事务
  + etcd v3 支持范围、分页查询，可避免大包等 expensive request
  + Kubernetes 支撑 5000 节点集群规模
  + 低容量的关键元数据存储，DB大小一般不超过8G
  + etcd 集群异地容灾，建议使用 raft learner 特性，建议在 >= 3.5 版本使用


* Consul
  + 在大规模集群中，etcd 的性能会优于 Consul
  + etcd v3 在稳定性、扩展性、性能方面做了大量优化
  + 支持健康检查功能，可以自动剔除不健康的服务。而 etcd 则没有这个功能，需要额外的工具来实现

* Redis
  + 主备异步复制，可能会丢数据
  + 存储的一般是用户数据，可以承载上T数据
  + 存储引擎和 API 上 Redis 内存实现了各种丰富数据结构


# 基本介绍

* `etcd` 是一个分布式键值 KV 存储系统，应用于服务发现，分布式锁，配置存储，分布式协调等。
* `etcd` 使用 `Raft` 共识算法来确保数据在集群中的一致性。
* 支持 `gRPC` 和 `HTTP` 协议调用。
* Kubernetes 使用 etcd 作为底层存储。Google 的 Spanner 和微信的 PaxosStore 都是基于 Paxos 协议实现的 KV 存储。Paxos 和 Raft 各有优劣，一般 Raft 工程实现简单，但是有租约不可用的问题；基于无租约 Paxos 协议没有 Leader 节点，可以做到无缝切换。


# 基础架构


![etcd_frame](/assets/images/202501/etcd_frame.png)

按照分层模型，etcd 可分为 **Client 层**、**API 网络层**、**Raft 算法层**、**逻辑层**和**存储层**。这些层的功能如下：

1. **Client 层**：Client 层包括 client v2 和 v3 两个大版本 API 客户端库，提供了简洁易用的 API，同时支持负载均衡、节点间故障自动转移，可极大降低业务使用 etcd 复杂度，提升开发效率、服务可用性。
2. **API 网络层**：API 网络层主要包括 client 访问 server 和 server 节点之间的通信协议。一方面，client 访问 etcd server 的 API 分为 v2 和 v3 两个大版本。v2 API 使用 `HTTP/1.x` 协议，v3 API 使用 `gRPC` 协议。同时 v3 通过 etcd grpc-gateway 组件也支持 `HTTP/1.x` 协议，便于各种语言的服务调用。另一方面，server 之间通信协议，是指节点间通过 Raft 算法实现数据复制和 Leader 选举等功能时使用的 HTTP 协议。
3. **Raft 层**：Raft 算法层实现了 Leader 选举、日志复制、ReadIndex 等核心算法特性，用于保障 etcd 多个节点间的数据一致性、提升服务可用性等，是 etcd 的基石和亮点。
4. **功能逻辑层**：etcd 核心特性实现层，如典型的 KVServer 模块、MVCC 模块、Auth 鉴权模块、Lease 租约模块、Compactor 压缩模块等，其中 MVCC 模块主要由 treeIndex 模块和 boltdb 模块组成。
5. **存储层**：存储层包含预写日志 (WAL) 模块、快照 (Snapshot) 模块、boltdb 模块。其中 WAL 可保障 etcd crash 后数据不丢失，boltdb 则保存了集群元数据和用户写入的数据。



# 数据模型

etcd 使用的是简单内存树，它的节点数据结构精简后如下，含节点路径、值、孩子节点信息。这是一个典型的低容量设计，数据全放在内存，无需考虑数据分片，只能保存 key 的最新版本，简单易实现。

``` go
type node struct {
   Path string                    // 节点路径
   Parent *node                   // 关联父亲节点
   Value string                   // key 的 value 值
   ExpireTime time.Time           // 过期时间
   Children map[string]*node      // 此节点的孩子节点
}
```

![etcd5](/assets/images/202501/etcd5.png)




# 基本原理


## etcd 一个读请求是如何执行的？

在下面这张架构图中，用序号标识了 etcd 默认读模式（**线性读**）的执行流程。(以 etcd v3.4.9 版本作为解释示例)

![etcd10](/assets/images/202501/etcd10.png)

> client

启动完 etcd 集群后，用 etcd 的客户端工具 `etcdctl` 执行一个 `get hello` 命令时，对应到图中流程1，etcdctl 是如何工作的呢？

``` bash
$ etcdctl put hello world --endpoints http://127.0.0.1:2379
OK
$ etcdctl get hello --endpoints http://127.0.0.1:2379
hello
world
```


* 首先，etcdctl 会对命令中的参数进行解析。其中，参数 get 是请求的方法，它是 KVServer 模块的 API；hello 是查询的 key 名；endpoints 是后端的 etcd 地址，通常，生产环境下中需要配置多个 endpoints，这样在 etcd 节点出现故障后，client 就可以自动重连到其它正常的节点，从而保证请求的正常执行。

* 在 etcd v3.4.9 版本中，etcdctl 是通过 clientv3 库来访问 etcd server 的，clientv3 库基于 gRPC client API 封装了操作 etcd KVServer、Cluster、Auth、Lease、Watch 等模块的 API，同时还包含了负载均衡、健康探测和故障切换等特性。

* 在解析完请求中的参数后，etcdctl 会创建一个 clientv3 库对象，使用 KVServer 模块的 API 来访问 etcd server。

* 接下来，就需要为这个 get hello 请求选择一个合适的 etcd server 节点了，这里得用到负载均衡算法。在 etcd 3.4 中，clientv3 库采用的负载均衡算法为 Round-robin。针对每一个请求，Round-robin 算法通过轮询的方式依次从 endpoint 列表中选择一个 endpoint 访问 (长连接)，使 etcd server 负载尽量均衡。

* 为请求选择好 etcd server 节点，client 就可调用 etcd server 的 KVServer 模块的 Range RPC 方法，把请求发送给 etcd server。

* client 和 server 之间的通信，使用的是基于 HTTP/2 的 gRPC 协议。相比 etcd v2 的 HTTP/1.x，HTTP/2 是基于二进制而不是文本、支持多路复用而不再有序且阻塞、支持数据压缩以减少包大小、支持 server push 等特性。因此，基于 HTTP/2 的 gRPC 协议具有低延迟、高性能的特点，有效解决了 etcd v2 中 HTTP/1.x 性能问题。

> KVServer

* client 发送 Range RPC 请求到了 server 后，就开始进入架构图中的流程2，也就是 KVServer 模块了。


> 拦截器

* etcd 提供了丰富的 metrics、日志、请求行为检查等机制，可记录所有请求的执行耗时及错误码、来源 IP 等，也可控制请求是否允许通过，比如 etcd Learner 节点只允许指定接口和参数的访问，帮助大家定位问题、提高服务可观测性等，而这些特性是怎么非侵入式的实现呢？答案就是拦截器。

* etcd server 定义了如下的 Service KV 和 Range 方法，启动的时候它会将实现 KV 各方法的对象注册到 gRPC Server，并在其上注册对应的拦截器。下面的代码中的 Range 接口就是负责读取 etcd key-value 的的 RPC 接口。

``` golang
service KV {
  // Range gets the keys in the range from the key-value store.
  rpc Range(RangeRequest) returns (RangeResponse) {
      option (google.api.http) = {
        post: "/v3/kv/range"
        body: "*"
      };
  }
  ....
}
```

* 拦截器提供了在执行一个请求前后的 hook 能力，除了上面提到的 debug 日志、metrics 统计、对 etcd Learner 节点请求接口和参数限制等能力，etcd 还基于它实现了以下特性:
  + 要求执行一个操作前集群必须有 Leader
  + 请求延时超过指定阈值的，打印包含来源 IP 的慢查询日志 (3.5 版本)

* server 收到 client 的 Range RPC 请求后，根据 ServiceName 和 RPC Method 将请求转发到对应的 handler 实现，handler 首先会将上面描述的一系列拦截器串联成一个执行，在拦截器逻辑中，通过调用 KVServer 模块的 Range 接口获取数据。

> 串行读与线性读

* 进入 KVServer 模块后，就进入核心的读流程了，对应架构图中的流程3和4。知道 etcd 为了保证服务高可用，生产环境一般部署多个节点，那各个节点数据在任意时间点读出来都是一致的吗？什么情况下会读到旧数据呢？

* 这里为了更好的理解读流程，先简单提下写流程。如下图所示，当 client 发起一个更新 hello 为 world 请求后，若 Leader 收到写请求，它会将此请求持久化到 WAL 日志，并广播给各个节点，若一半以上节点持久化成功，则该请求对应的日志条目被标识为已提交，etcdserver 模块异步从 Raft 模块获取已提交的日志条目，应用到状态机 (boltdb 等)。

![etcd12](/assets/images/202501/etcd12.png)

* 此时若 client 发起一个读取 hello 的请求，假设此请求直接从状态机中读取， 如果连接到的是 C 节点，若 C 节点磁盘 I/O 出现波动，可能导致它应用已提交的日志条目很慢，则会出现更新 hello 为 world 的写命令，在 client 读 hello 的时候还未被提交到状态机，因此就可能读取到旧数据，如上图查询 hello 流程所示。

* 从以上介绍可以看出，在多节点 etcd 集群中，各个节点的状态机数据一致性存在差异。而不同业务场景的读请求对数据是否最新的容忍度是不一样的，有的场景它可以容忍数据落后几秒甚至几分钟，有的场景要求必须读到反映集群共识的最新数据。
  + 首先来看一个对数据敏感度较低的场景。假如老板让你做一个旁路数据统计服务，希望你每分钟统计下 etcd 里的服务、配置信息等，这种场景其实对数据时效性要求并不高，读请求可直接从节点的状态机获取数据。即便数据落后一点，也不影响业务，毕竟这是一个定时统计的旁路服务而已。**这种直接读状态机数据返回、无需通过 Raft 协议与集群进行交互的模式，在 etcd 里叫做串行 (Serializable) 读，它具有低延时、高吞吐量的特点，适合对数据一致性要求不高的场景**。
  + 再看一个对数据敏感性高的场景。比如说一个转账场景，Alice 给 Bob 转账成功，钱被正常扣出，一刷新页面发现钱又回来了，这也是令人不可接受的。以上的业务场景就对数据准确性要求极高了，**在 etcd 里面，提供了一种线性读模式来解决对数据一致性要求高的场景**。

> 什么是线性读呢?

* **可以理解一旦一个值更新成功，随后任何通过线性读的 client 都能及时访问到**。虽然集群中有多个节点，但 client 通过线性读就如访问一个节点一样。etcd 默认读模式是线性读，因为它需要经过 Raft 协议模块，反应的是集群共识，因此在延时和吞吐量上相比串行读略差一点，适用于对数据一致性要求高的场景。

* 如果 etcd 读请求显示指定了是串行读，就不会经过架构图流程中的流程3、4。默认是线性读，因此接下来看看读请求进入线性读模块，它是如何工作的。

> 线性读之 ReadIndex

串行读之所以能读到旧数据，主要原因是 Follower 节点收到 Leader 节点同步的写请求后，**应用日志条目到状态机是个异步过程**，那么能否有一种机制在读取的时候，确保最新的数据已经应用到状态机中？

![etcd13](/assets/images/202501/etcd13.png)

* 其实这个机制就是叫 ReadIndex，它是在 etcd 3.1 中引入的，把简化后的原理图放在了上面。当收到一个线性读请求时，它首先会从 Leader 获取集群最新的已提交的日志索引 (committed index)，如上图中的流程2所示。

* Leader 收到 ReadIndex 请求时，为防止脑裂等异常场景，会向 Follower 节点发送心跳确认，一半以上节点确认 Leader 身份后才能将已提交的索引 (committed index) 返回给节点 C(上图中的流程3)。

* C 节点则会等待，直到状态机已应用索引 (applied index) 大于等于 Leader 的已提交索引时 (committed Index)(上图中的流程四)，然后去通知读请求，数据已赶上 Leader，你可以去状态机中访问数据了 (上图中的流程5)。

* 以上就是线性读通过 ReadIndex 机制保证数据一致性原理， 当然还有其它机制也能实现线性读，如在早期 etcd 3.0 中读请求通过走一遍 Raft 协议保证一致性，这种 Raft log read 机制依赖磁盘IO，性能相比 ReadIndex 较差。

* 总体而言，KVServer 模块收到线性读请求后，通过架构图中流程3向 Raft 模块发起 ReadIndex 请求，Raft 模块将 Leader 最新的已提交日志索引封装在流程4的 ReadState 结构体，通过 channel 层层返回给线性读模块，线性读模块等待本节点状态机追赶上 Leader 进度，追赶完成后，就通知 KVServer 模块，进行架构图中流程5，与状态机中的 MVCC 模块进行进行交互了。

> MVCC

流程5中的多版本并发控制 (**Multiversion concurrency control**) 模块是为了解决 etcd v2 不支持保存 key 的历史版本、不支持多 key 事务等问题而产生的。

* 它核心由内存树形索引模块 (`treeIndex`) 和嵌入式的 KV 持久化存储库 `boltdb` 组成。

* 首先需要简单了解下 `boltdb`，它是个基于 B+ tree 实现的 key-value 键值库，支持事务，提供 Get/Put 等简易 API 给 etcd 操作。

* 那么 etcd 如何基于 boltdb 保存一个 key 的多个历史版本呢?
  + 比如现在有以下方案：方案 1 是一个 key 保存多个历史版本的值；方案 2 每次修改操作，生成一个新的版本号 (revision)，以版本号为 key，value 为用户 key-value 等信息组成的结构体。
  + 很显然方案 1 会导致 value 较大，存在明显读写放大、并发冲突等问题，而方案 2 正是 etcd 所采用的。**boltdb 的 key 是全局递增的版本号 (revision)，value 是用户 key、value 等字段组合成的结构体，然后通过 treeIndex 模块来保存用户 key 和版本号的映射关系**。

* `treeIndex` 与 `boltdb` 关系如下面的读事务流程图所示，从 `treeIndex` 中获取 key hello 的**版本号**，再以**版本号**作为 `boltdb` 的 key，从 `boltdb` 中获取其 `value` 信息。


![etcd14](/assets/images/202501/etcd14.png)

```
* B树更适合于需要频繁更新数据的场景，因为它可以在每个节点中存储更多的键值对，从而减少树的高度和磁盘I/O操作。然而，其搜索路径可能较长，不适合顺序访问。
* B+树则更适合于顺序访问和范围查询的场景，如文件系统和数据库索引。它通过将所有数据存储在叶子节点并链接成链表，使得范围查询更加高效。

因此，如果主要关注的是数据的插入和删除效率，可以选择B树；如果更关注顺序访问和范围查询的性能，则应优先考虑B+树。

https://dezeming.top/wp-content/uploads/2023/04/B-Tree%E5%92%8CBTree.pdf
```

> treeIndex

* treeIndex 模块是基于 Google 开源的内存版 btree 库实现的，为什么 etcd 选择上图中的 B-tree 数据结构保存用户 key 与版本号之间的映射关系，而不是哈希表、二叉树呢？(后面介绍)

* treeIndex 模块只会保存用户的 key 和相关版本号信息，**用户 key 的 value 数据存储在 boltdb 里面，相比 ZooKeeper 和 etcd v2 全内存存储，etcd v3 对内存要求更低**。

* 简单介绍了 etcd 如何保存 key 的历史版本后，架构图中流程6也就非常容易理解了，它需要从 treeIndex 模块中获取 hello 这个 key 对应的版本号信息。treeIndex 模块基于 B-tree 快速查找此 key，返回此 key 对应的索引项 `keyIndex` 即可。索引项中包含版本号等信息。

> buffer

* 在获取到版本号信息后，就可从 boltdb 模块中获取用户的 key-value 数据了。不过有一点要注意，并不是所有请求都一定要从 boltdb 获取数据。

* etcd 出于数据一致性、性能等考虑，在访问 boltdb 前，首先会从一个内存读事务 buffer 中，二分查找要访问 key 是否在 buffer 里面，若命中则直接返回。

> boltdb

* 若 buffer 未命中，此时就真正需要向 boltdb 模块查询数据了，进入了流程7。

* 我们知道 MySQL 通过 table 实现不同数据逻辑隔离，那么在 boltdb 是如何隔离集群元数据与用户数据的呢？答案是 `bucket`。boltdb 里每个 `bucket` 类似对应 **MySQL 一个表**，用户的 key 数据存放的 bucket 名字的是 key，etcd MVCC 元数据存放的 bucket 是 meta。

* 因 boltdb 使用 B+ tree 来组织用户的 key-value 数据，获取 bucket key 对象后，通过 boltdb 的游标 Cursor 可快速在 B+ tree 找到 key hello 对应的 value 数据，返回给 client。

* 到这里，一个读请求之路执行完成。


### Q&A

#### etcd 在执行读请求过程中涉及磁盘 IO 吗？如果涉及，是什么模块在什么场景下会触发呢？如果不涉及，又是什么原因呢？

有同学说 buffer 没读到，从 boltdb 读时会产生磁盘 I/O，这是一个常见误区。

实际上，etcd 在启动的时候会通过 mmap 机制将 etcd db 文件映射到 etcd 进程地址空间，并设置了 mmap 的 MAP_POPULATE flag，它会告诉 Linux 内核预读文件，Linux 内核会将文件内容拷贝到物理内存中，此时会产生磁盘 I/O。节点内存足够的请求下，后续处理读请求过程中就不会产生磁盘 I/IO 了。

若 etcd 节点内存不足，可能会导致 db 文件对应的内存页被换出，当读请求命中的页未在内存中时，就会产生缺页异常，导致读过程中产生磁盘 IO，可以通过观察 etcd 进程的 majflt 字段来判断 etcd 是否产生了主缺页中断。


#### readIndex 需要请求 leader，为什么不直接让 leader 返回读请求的结果，而要等待 follower 的进度赶上 leader？

非常好的问题，我个人认为主要还是性能因素，我记得 etcd v2 早期的时候如果你指定线性读/共识读，它就是直接转发给 leader 的。后来在 etcd v3.0 中实现了 raft log read 但是要走一遍 raft log，读涉及到磁盘IO，v3.1 中引入了 readIndex 机制，它是非常轻量级的，开销较小，相比各个 follower 都转发给 leader 会导致 leader 负载较高，特别是 expensive request 场景，性能会急剧下降，leader 的内存、CPU、网络带宽资源都很容易耗尽，readIndex 机制的引入，使得每个 follower 节点都可以处理读请求，极大扩展提升了写性能。


#### 当 Readindex 结束并等待本节点的状态机apply的时候，key又被最新的更新请求给更新了怎么办，这个时候读取到的value是不是又是旧值了？

线性读，读出来的值实际上是你发出读请求时间点的集群最新共识数据，在你读请求发出后，若耗时一定时间还未完成，在这过程中leader又收到了写请求更新了它，的确你原来读出来的值相比最新的集群共识就是旧的，在实际应用中，我们一般会通过增加版本号检测识别此类问题，后面事务篇会详细介绍。


## etcd一个写请求是如何执行的？

那么 etcd 一个写请求执行流程又是怎样的呢？在执行写请求过程中，如果进程 crash 了，如何保证数据不丢、命令不重复执行呢？

![etcd15](/assets/images/202501/etcd15.png)

在如上的架构图中，用序号标识了下面的一个 put hello 为 world 的写请求的简要执行流程，帮助从整体上快速了解一个写请求的全貌。

``` bash
$ etcdctl put hello world --endpoints http://127.0.0.1:2379
OK
```

* 首先 client 端通过负载均衡算法选择一个 etcd 节点，发起 gRPC 调用。然后 etcd 节点收到请求后经过 gRPC 拦截器、Quota 模块后，进入 KVServer 模块，KVServer 模块向 Raft 模块提交一个提案，提案内容为“大家好，请使用 put 方法执行一个 key 为 hello，value 为 world 的命令”。

* 随后此提案通过 RaftHTTP 网络模块转发、经过集群多数节点持久化后，状态会变成已提交，etcdserver 从 Raft 模块获取已提交的日志条目，传递给 Apply 模块，Apply 模块通过 MVCC 模块执行提案内容，更新状态机。

* 与读流程不一样的是写流程还涉及 Quota、WAL、Apply 三个模块。**crash-safe 及幂等性也正是基于 WAL 和 Apply 流程的 consistent index 等实现的**，因此重点介绍这三个模块。

> Quota (限额) 模块

* 首先是流程1 client 端发起 gRPC 调用到 etcd 节点，和读请求不一样的是，写请求需要经过流程二 db 配额（Quota）模块，它有什么功能呢？

* 先从此模块的一个常见错误说起，在使用 etcd 过程中是否遇到过 "etcdserver: mvcc: database space exceeded" 错误呢？只要使用过 etcd 或者 Kubernetes，大概率见过这个错误。它是指当前 etcd db 文件大小超过了配额，当出现此错误后，整个集群将不可写入，只读，对业务的影响非常大。
  + 哪些情况会触发这个错误呢？**一方面默认 db 配额仅为 2G**，当业务数据、写入 QPS、Kubernetes 集群规模增大后，etcd db 大小就可能会超过 2G。**另一方面 etcd v3 是个 MVCC 数据库，保存了 key 的历史版本，当未配置压缩策略的时候，随着数据不断写入，db 大小会不断增大，导致超限**。

* 了解完触发 Quota 限制的原因后，再详细了解下 Quota 模块它是如何工作的。
  + 当 etcd server 收到 put/txn 等写请求的时候，会首先检查下当前 etcd db 大小加上请求的 key-value 大小之和是否超过了配额（quota-backend-bytes）。
  + 如果超过了配额，它会产生一个告警（Alarm）请求，告警类型是 NO SPACE，并通过 Raft 日志同步给其它节点，告知 db 无空间了，并将告警持久化存储到 db 中。
  + **最终，无论是 API 层 gRPC 模块还是负责将 Raft 侧已提交的日志条目应用到状态机的 Apply 模块，都拒绝写入，集群只读**。

* 那遇到这个错误时应该如何解决呢？
  + 首先当然是调大配额。具体多大合适呢？**etcd 社区建议不超过 8G**。遇到过这个错误的你是否还记得，为什么当你把配额（quota-backend-bytes）调大后，集群依然拒绝写入呢?
  + 原因就是前面提到的 NO SPACE 告警。Apply 模块在执行每个命令的时候，都会去检查当前是否存在 NO SPACE 告警，如果有则拒绝写入。所以还需要你额外发送一个**取消告警（etcdctl alarm disarm）的命令**，以消除所有告警。
  + 其次需要检查 etcd 的压缩（compact）配置是否开启、配置是否合理。**etcd 保存了一个 key 所有变更历史版本，如果没有一个机制去回收旧的版本，那么内存和 db 大小就会一直膨胀，在 etcd 里面，压缩模块负责回收旧版本的工作**。
  + 压缩模块支持按多种方式回收旧版本，比如保留最近一段时间内的历史版本。不过要注意，**它仅仅是将旧版本占用的空间打个空闲（Free）标记，后续新的数据写入的时候可复用这块空间，而无需申请新的空间**。
  + 如果需要回收空间，减少 db 大小，得使用碎片整理（defrag），它会遍历旧的 db 文件数据，写入到一个新的 db 文件。但是它对服务性能有较大影响，不建议在生产集群频繁使用。
  + 最后需要注意配额（quota-backend-bytes）的行为，默认0 就是使用 etcd 默认的 2GB 大小，需要根据业务场景适当调优。如果填的是个小于 0 的数，就会禁用配额功能，这可能会让 db 大小处于失控，导致性能下降，不建议禁用配额。

> KVServer 模块

* 通过流程2的配额检查后，请求就从 API 层转发到了流程3的 KVServer 模块的 put 方法，我们知道 etcd 是基于 Raft 算法实现节点间数据复制的，因此它需要将 put 写请求内容打包成一个提案消息，提交给 Raft 模块。不过 KVServer 模块在提交提案前，还有如下的一系列检查和限速。

> Preflight Check

* **为了保证集群稳定性，避免雪崩，任何提交到 Raft 模块的请求，都会做一些简单的限速判断**。如下面的流程图所示，首先，如果 Raft 模块已提交的日志索引（committed index）比已应用到状态机的日志索引（applied index）超过了 5000，那么它就返回一个 "etcdserver: too many requests" 错误给 client。

* 然后它会尝试去获取请求中的鉴权信息，若使用了密码鉴权、请求中携带了 token，如果 token 无效，则返回 "auth: invalid auth token" 错误给 client。

* 其次它会检查你写入的包大小是否超过默认的 1.5MB， 如果超过了会返回 "etcdserver: request is too large" 错误给给 client。

![etcd16](/assets/images/202501/etcd16.png)

> Propose

* **最后通过一系列检查之后，会生成一个唯一的 ID，将此请求关联到一个对应的消息通知 channel，然后向 Raft 模块发起（Propose）一个提案（Proposal）**，提案内容为 “大家好，请使用 put 方法执行一个 key 为 hello，value 为 world 的命令”，也就是整体架构图里的流程4。

* 向 Raft 模块发起提案后，KVServer 模块会等待此 put 请求，等待写入结果通过消息通知 channel 返回或者超时。**etcd 默认超时时间是 7 秒**（5 秒磁盘 IO 延时 + 2*1 秒竞选超时时间），如果一个请求超时未返回结果，则可能会出现你熟悉的 "etcdserver: request timed out" 错误。

> WAL 模块

* **Raft 模块收到提案后，如果当前节点是 Follower，它会转发给 Leader，只有 Leader 才能处理写请求**。Leader 收到提案后，通过 Raft 模块输出待转发给 Follower 节点的消息和待持久化的日志条目，日志条目则封装了上面所说的 put hello 提案内容。

* etcdserver 从 Raft 模块获取到以上消息和日志条目后，作为 Leader，它会将 put 提案消息广播给集群各个节点，同时需要把集群 Leader 任期号、投票信息、已提交索引、提案内容持久化到一个 WAL（Write Ahead Log）日志文件中，用于保证集群的一致性、可恢复性，也就是图中的流程5模块。

* WAL 日志结构是怎样的呢？

![etcd17](/assets/images/202501/etcd17.png)

* 上图是 WAL 结构，它由多种类型的 WAL 记录顺序追加写入组成，每个记录由类型、数据、循环冗余校验码组成。不同类型的记录通过 Type 字段区分，Data 为对应记录内容，CRC 为循环校验码信息。

* WAL 记录类型目前支持 5 种，分别是**文件元数据记录**、**日志条目记录**、**状态信息记录**、**CRC 记录**、**快照记录**：
  + 文件元数据记录包含节点 ID、集群 ID 信息，它在 WAL 文件创建的时候写入；
  + 日志条目记录包含 Raft 日志信息，如 put 提案内容；
  + 状态信息记录，包含集群的任期号、节点投票信息等，一个日志文件中会有多条，以最后的记录为准；
  + CRC 记录包含上一个 WAL 文件的最后的 CRC（循环冗余校验码）信息， 在创建、切割 WAL 文件时，作为第一条记录写入到新的 WAL 文件， 用于校验数据文件的完整性、准确性等；
  + 快照记录包含快照的任期号、日志索引信息，用于检查快照文件的准确性。


* WAL 模块又是如何持久化一个 put 提案的日志条目类型记录呢? 首先来看看 put 写请求如何封装在 Raft 日志条目里面。下面是 Raft 日志条目的数据结构信息，它由以下字段组成：
  + Term 是 Leader 任期号，随着 Leader 选举增加；
  + Index 是日志条目的索引，单调递增增加；
  + Type 是日志类型，比如是普通的命令日志（EntryNormal）还是集群配置变更日志（EntryConfChange）；
  + Data 保存上面描述的 put 提案内容。

``` golang
type Entry struct {
   Term             uint64    `protobuf:"varint，2，opt，name=Term" json:"Term"`
   Index            uint64    `protobuf:"varint，3，opt，name=Index" json:"Index"`
   Type             EntryType `protobuf:"varint，1，opt，name=Type，enum=Raftpb.EntryType" json:"Type"`
   Data             []byte    `protobuf:"bytes，4，opt，name=Data" json:"Data，omitempty"`
}
```

* 了解完 Raft 日志条目数据结构后，再看 WAL 模块如何持久化 Raft 日志条目。它首先先将 Raft 日志条目内容（含任期号、索引、提案内容）序列化后保存到 WAL 记录的 Data 字段， 然后计算 Data 的 CRC 值，设置 Type 为 Entry Type， 以上信息就组成了一个完整的 WAL 记录。

* 最后计算 WAL 记录的长度，顺序先写入 WAL 长度（Len Field），然后写入记录内容，调用 `fsync` 持久化到磁盘，完成将日志条目保存到持久化存储中。

* 当一半以上节点持久化此日志条目后，Raft 模块就会通过 channel 告知 etcdserver 模块，put 提案已经被集群多数节点确认，提案状态为已提交，可以执行此提案内容了。

* 于是进入流程6，etcdserver 模块从 channel 取出提案内容，添加到先进先出（FIFO）调度队列，随后通过 Apply 模块按入队顺序，异步、依次执行提案内容。

> Apply 模块

* 执行 put 提案内容对应架构图中的流程7，其细节图如下。那么 Apply 模块是如何执行 put 请求的呢？若 put 请求提案在执行流程7的时候 etcd 突然 crash 了，重启恢复的时候，etcd 是如何找回异常提案，再次执行的呢？

![etcd18](/assets/images/202501/etcd18.png)

* **核心就是上面介绍的 WAL 日志，因为提交给 Apply 模块执行的提案已获得多数节点确认、持久化，etcd 重启时，会从 WAL 中解析出 Raft 日志条目内容，追加到 Raft 日志的存储中，并重放已提交的日志提案给 Apply 模块执行**。

* 然而这又引发了另外一个问题，**如何确保幂等性，防止提案重复执行导致数据混乱呢?**

* **etcd 是个 MVCC 数据库，每次更新都会生成新的版本号**。如果没有幂等性保护，同样的命令，一部分节点执行一次，一部分节点遭遇异常故障后执行多次，则系统的各节点一致性状态无法得到保证，导致数据混乱，这是严重故障。因此 etcd 必须要确保幂等性。怎么做呢？**Apply 模块从 Raft 模块获得的日志条目信息里，是否有唯一的字段能标识这个提案？**

* 答案就是上面介绍 Raft 日志条目中的索引（index）字段。**日志条目索引是全局单调递增的，每个日志条目索引对应一个提案**，如果一个命令执行后，在 db 里面也记录下当前已经执行过的日志条目索引，是不是就可以解决幂等性问题呢？是的。但是这还不够安全，**如果执行命令的请求更新成功了，更新 index 的请求却失败了，是不是一样会导致异常**？**因此在实现上，还需要将两个操作作为原子性事务提交，才能实现幂等**。

* 正如上面的讨论的这样，**etcd 通过引入一个 consistent index 的字段，来存储系统当前已经执行过的日志条目索引，实现幂等性**。

* Apply 模块在执行提案内容前，首先会判断当前提案是否已经执行过了，如果执行了则直接返回，若未执行同时无 db 配额满告警，则进入到 MVCC 模块，开始与持久化存储模块打交道。

> MVCC

* Apply 模块判断此提案未执行后，就会调用 MVCC 模块来执行提案内容。**MVCC 主要由两部分组成，一个是内存索引模块 treeIndex，保存 key 的历史版本号信息，另一个是 boltdb 模块，用来持久化存储 key-value 数据**。那么 MVCC 模块执行 put hello 为 world 命令时，它是如何构建内存索引和保存哪些数据到 db 呢？

> treeIndex

* 首先来看 MVCC 的索引模块 treeIndex，当收到更新 key hello 为 world 的时候，此 key 的索引版本号信息是怎么生成的呢？需要维护、持久化存储一个全局版本号吗？

* 版本号（revision）在 etcd 里面发挥着重大作用，它是 etcd 的逻辑时钟。**etcd 启动的时候默认版本号是 1，随着对 key 的增、删、改操作而全局单调递增**。

* 因为 boltdb 中的 key 就包含此信息，所以 etcd 并不需要再去持久化一个全局版本号。**只需要在启动的时候，从最小值 1 开始枚举到最大值，未读到数据的时候则结束，最后读出来的版本号即是当前 etcd 的最大版本号 currentRevision**。

* MVCC 写事务在执行 put hello 为 world 的请求时，会基于 **currentRevision** 自增生成新的 revision 如{2,0}，然后从 treeIndex 模块中查询 key 的创建版本号、修改次数信息。这些信息将填充到 boltdb 的 value 中，同时将用户的 hello key 和 revision 等信息存储到 B-tree，也就是下面简易写事务图的流程1，整体架构图中的流程8。

![etcd19](/assets/images/202501/etcd19.png)

> boltdb

* MVCC 写事务自增全局版本号后生成的 revision{2,0}，它就是 boltdb 的 key，通过它就可以往 boltdb 写数据了，进入了整体架构图中的流程9。

* boltdb 它是一个基于 B+tree 实现的 key-value 嵌入式 db，它通过提供桶（bucket）机制实现类似 MySQL 表的逻辑隔离。

* 在 etcd 里面通过 put/txn 等 KV API 操作的数据，全部保存在一个名为 key 的桶里面，这个 key 桶在启动 etcd 的时候会自动创建。

* 除了保存用户 KV 数据的 key 桶，etcd 本身及其它功能需要持久化存储的话，都会创建对应的桶。比如上面提到的 etcd 为了保证日志的幂等性，保存了一个名为 consistent index 的变量在 db 里面，它实际上就存储在元数据（meta）桶里面。

* 那么写入 boltdb 的 value 含有哪些信息呢？写入 boltdb 的 value，并不是简单的 "world"，如果只存一个用户 value，索引又是保存在易失的内存上，那重启 etcd 后，就丢失了用户的 key 名，无法构建 treeIndex 模块了。因此为了构建索引和支持 Lease 等特性，etcd 会持久化以下信息:
  + key 名称；
  + key 创建时的版本号（create_revision）、最后一次修改时的版本号（mod_revision）、key 自身修改的次数（version）；
  + value 值；
  + 租约信息（后面介绍）。

* boltdb value 的值就是将含以上信息的结构体序列化成的二进制数据，然后通过 boltdb 提供的 put 接口，etcd 就快速完成了将数据写入 boltdb，对应上面简易写事务图的流程2。

* 但是 put 调用成功，就能够代表数据已经持久化到 db 文件了吗？这里需要注意的是，在以上流程中，etcd 并未提交事务（commit），因此数据只更新在 boltdb 所管理的内存数据结构中。

* 事务提交的过程，包含 B+tree 的平衡、分裂，将 boltdb 的脏数据（dirty page）、元数据信息刷新到磁盘，因此事务提交的开销是昂贵的。**如果每次更新都提交事务，etcd 写性能就会较差**。那么解决的办法是什么呢？**etcd 的解决方案是合并再合并**。
  + 首先 boltdb key 是版本号，put/delete 操作时，都会基于当前版本号递增生成新的版本号，因此属于顺序写入，可以调整 boltdb 的 bucket.FillPercent 参数，使每个 page 填充更多数据，减少 page 的分裂次数并降低 db 空间。
  + 其次 etcd 通过合并多个写事务请求，通常情况下，是异步机制定时（默认每隔 100ms）将批量事务一次性提交（pending 事务过多才会触发同步提交），从而大大提高吞吐量，对应上面简易写事务图的流程3。
  + 但是这优化又引发了另外的一个问题，因为事务未提交，读请求可能无法从 boltdb 获取到最新数据。
  + 为了解决这个问题，etcd 引入了一个 bucket buffer 来保存暂未提交的事务数据。在更新 boltdb 的时候，etcd 也会同步数据到 bucket buffer。因此 etcd 处理读请求的时候会优先从 bucket buffer 里面读取，其次再从 boltdb 读，通过 bucket buffer 实现读写性能提升，同时保证数据一致性。


### Q&A

#### expensive read 请求（如 Kubernetes 场景中查询大量 pod）会影响写请求的性能吗？

在 etcd 3.0 中，线性读请求需要走一遍 Raft 协议持久化到 WAL 日志中，因此读性能非常差，写请求肯定也会被影响。

在 etcd 3.1 中，引入了 ReadIndex 机制提升读性能，读请求无需再持久化到 WAL 中。

在 etcd 3.2 中, 优化思路转移到了 MVCC/boltdb 模块，boltdb 的事务锁由粗粒度的互斥锁，优化成读写锁，实现 “N reads or 1 write” 的并行，同时引入了 buffer 来提升吞吐量。问题就出在这个 buffer，读事务会加读锁，写事务结束时要升级锁更新 buffer，但是 expensive request 导致读事务长时间持有锁，最终导致写请求超时。

在 etcd 3.4 中，实现了全并发读，创建读事务的时候会全量拷贝 buffer, 读写事务不再因为 buffer 阻塞，大大缓解了 expensive request 对 etcd 性能的影响。尤其是 Kubernetes List Pod 等资源场景来说，etcd 稳定性显著提升。


## Raft 协议：etcd 如何实现高可用、数据强一致的？

> 如何避免单点故障？

* 首先回想下，早期使用的数据存储服务，它们往往是部署在单节点上的。但是单节点存在单点故障，一宕机就整个服务不可用，对业务影响非常大。

* 随后，为了解决单点问题，软件系统工程师引入了**数据复制技术，实现多副本**。通过数据复制方案，一方面可以提高服务可用性，避免单点故障。另一方面，多副本可以提升读吞吐量、甚至就近部署在业务所在的地理位置，降低访问延迟。

> 多副本复制是如何实现的呢？

* 多副本常用的技术方案主要有**主从复制**和**去中心化复制**。

* **主从复制**，又分为全同步复制、异步复制、半同步复制，比如 MySQL/Redis 单机主备版就基于主从复制实现的。
  + **全同步复制**，是指主收到一个写请求后，必须等待全部从节点确认返回后，才能返回给客户端成功。因此如果一个从节点故障，整个系统就会不可用。这种方案**为了保证多副本的一致性，而牺牲了可用性，一般使用不多**。
  + **异步复制**，是指主收到一个写请求后，可及时返回给 client，异步将请求转发给各个副本，若还未将请求转发到副本前就故障了，则可能导致数据丢失，但是可用性是最高的。
  + **半同步复制**，介于全同步复制、异步复制之间，它是指主收到一个写请求后，至少有一个副本接收数据后，就可以返回给客户端成功，在数据一致性、可用性上实现了平衡和取舍。

* **去中心化复制**，是指在一个 n 副本节点集群中，任意节点都可接受写请求，但一个成功的写入需要 w 个节点确认，读取也必须查询至少 r 个节点。
  + 可以根据实际业务场景对数据一致性的敏感度，设置合适 w/r 参数。比如希望每次写入后，任意 client 都能读取到新值，如果 n 是 3 个副本，可以将 w 和 r 设置为 2，这样当读两个节点时候，必有一个节点含有最近写入的新值，这种读称之为**法定票数读**（quorum read）。
  + AWS 的 Dynamo 系统就是基于去中心化的复制算法实现的。它的优点是节点角色都是平等的，降低运维复杂度，可用性更高。但是缺陷是去中心化复制，势必会导致各种写入冲突，业务需要关注冲突处理。

> 如何解决以上复制算法的困境呢？

答案就是**共识算法**，它最早是**基于复制状态机背景下提出来的**。 下图是复制状态机的结构（引用自 Raft paper）， 它由共识模块、日志模块、状态机组成。通过共识模块保证各个节点日志的一致性，然后各个节点基于同样的日志、顺序执行指令，最终各个复制状态机的结果实现一致。

![raft1](/assets/images/202501/raft1.png)

* 共识算法的祖师爷是 Paxos， 但是由于它过于复杂，难于理解，工程实践上也较难落地，导致在工程界落地较慢。standford 大学的 Diego 提出的 Raft 算法正是为了可理解性、易实现而诞生的，它通过问题分解，**将复杂的共识问题拆分成三个子问题**，分别是：
  + **Leader 选举**，Leader 故障后集群能快速选出新 Leader
  + **日志复制**，集群只有 Leader 能写入日志，Leader 负责复制日志到 Follower 节点，并强制 Follower 节点与自己保持相同
  + **安全性**，一个任期内集群只能产生一个 Leader、已提交的日志条目在发生 Leader 选举时，一定会存在更高任期的新 Leader 日志中、各个节点的状态机应用的任意位置的日志条目内容应一样等。

> Leader 选举

* 当 etcd server 收到 client 发起的 put hello 写请求后，KV 模块会向 Raft 模块提交一个 put 提案，**只有集群 Leader 才能处理写提案，如果此时集群中无 Leader， 整个请求就会超时**。

* 那么 Leader 是怎么诞生的呢？Leader crash 之后其他节点如何竞选呢？**首先在 Raft 协议中它定义了集群中的如下节点状态，任何时刻，每个节点肯定处于其中一个状态**：
  + **Follower**，跟随者，同步从 Leader 收到的日志，**etcd 启动的时候默认为此状态**
  + **Candidate**，竞选者，可以发起 Leader 选举
  + **Leader**，集群领导者，**唯一性，拥有同步日志的特权，需定时广播心跳给 Follower 节点，以维持领导者身份**


![etcd20](/assets/images/202501/etcd20.png)

* 上图是节点状态变化关系图，当 Follower 节点接收 Leader 节点心跳消息超时后，它会转变成 Candidate 节点，并可发起竞选 Leader 投票，若获得集群多数节点的支持后，它就可转变成 Leader 节点。

* 下面以 Leader crash 场景为案例，详细介绍一下 etcd Leader 选举原理。

* 假设集群总共 3 个节点，A 节点为 Leader，B、C 节点为 Follower。

![etcd21](/assets/images/202501/etcd21.png)

* 如上 Leader 选举图左边部分所示， 正常情况下，Leader 节点会按照心跳间隔时间，定时广播心跳消息（MsgHeartbeat 消息）给 Follower 节点，以维持 Leader 身份。 Follower 收到后回复心跳应答包消息（MsgHeartbeatResp 消息）给 Leader。

* 你可能注意到上图中的 Leader 节点下方有一个任期号（term），它具有什么样的作用呢？**这是因为 Raft 将时间划分成一个个任期，任期用连续的整数表示，每个任期从一次选举开始，赢得选举的节点在该任期内充当 Leader 的职责，随着时间的消逝，集群可能会发生新的选举，任期号也会单调递增**。通过任期号，可以比较各个节点的数据新旧、识别过期的 Leader 等，它在 Raft 算法中充当逻辑时钟，发挥着重要作用。

* 了解完正常情况下 Leader 维持身份的原理后，再看异常情况下，也就 Leader crash 后，etcd 是如何自愈的呢？

* 如上 Leader 选举图右边部分所示，当 Leader 节点异常后，Follower 节点会接收 Leader 的心跳消息超时，当超时时间大于竞选超时时间后，它们会进入 Candidate 状态。

* 这里要提醒下你，etcd **默认心跳间隔时间**（heartbeat-interval）是 `100ms`， **默认竞选超时时间**（election timeout）是 `1000ms`， 你需要根据实际部署环境、业务场景适当调优，否则就很可能会频繁发生 Leader 选举切换，导致服务稳定性下降。

* 进入 Candidate 状态的节点，会立即发起选举流程，自增任期号，投票给自己，并向其他节点发送竞选 Leader 投票消息（MsgVote）。

* C 节点收到 Follower B 节点竞选 Leader 消息后，这时候可能会出现如下**两种情况**：
  + 第一种情况是 C 节点判断 B 节点的数据至少和自己一样新、B 节点任期号大于 C 当前任期号、并且 C 未投票给其他候选者，就可投票给 B。这时 B 节点获得了集群多数节点支持，于是成为了新的 Leader。
  + 第二种情况是，恰好 C 也心跳超时超过竞选时间了，它也发起了选举，并投票给了自己，那么它将拒绝投票给 B，这时谁也无法获取集群多数派支持，只能等待竞选超时，开启新一轮选举。**Raft 为了优化选票被瓜分导致选举失败的问题，引入了随机数，每个节点等待发起选举的时间点不一致，优雅的解决了潜在的竞选活锁，同时易于理解**。

* Leader 选出来后，它什么时候又会变成 Follower 状态呢？从上面的状态转换关系图中可以看到，如果现有 Leader 发现了新的 Leader 任期号，那么它就需要转换到 Follower 节点。A 节点 crash 后，再次启动成为 Follower，假设因为网络问题无法连通 B、C 节点，这时候根据状态图，我们知道它将不停自增任期号，发起选举。**等 A 节点网络异常恢复后，那么现有 Leader 收到了新的任期号，就会触发新一轮 Leader 选举，影响服务的可用性**。

* 然而 A 节点的数据是远远落后 B、C 的，是无法获得集群 Leader 地位的，发起的选举无效且对集群稳定性有伤害。那如何避免以上场景中的无效的选举呢？

* 在 etcd 3.4 中，etcd 引入了一个 PreVote 参数（默认 false），可以用来启用 PreCandidate 状态解决此问题，如下图所示。Follower 在转换成 Candidate 状态前，先进入 PreCandidate 状态，不自增任期号，发起预投票。若获得集群多数节点认可，确定有概率成为 Leader 才能进入 Candidate 状态，发起选举流程。

![etcd22](/assets/images/202501/etcd22.png)

* **因 A 节点数据落后较多，预投票请求无法获得多数节点认可，因此它就不会进入 Candidate 状态，导致集群重新选举**。

* 这就是 Raft Leader 选举核心原理，使用心跳机制维持 Leader 身份、触发 Leader 选举，**etcd 基于它实现了高可用，只要集群一半以上节点存活、可相互通信，Leader 宕机后，就能快速选举出新的 Leader，继续对外提供服务**。

> 日志复制

* 假设在上面的 Leader 选举流程中，B 成为了新的 Leader，它收到 put 提案后，它是如何将日志同步给 Follower 节点的呢？ 什么时候它可以确定一个日志条目为已提交，通知 etcdserver 模块应用日志条目指令到状态机呢？

* 这就涉及到 Raft 日志复制原理，为了帮助理解日志复制的原理，下面画了一幅 Leader 收到 put 请求后，向 Follower 节点复制日志的整体流程图，简称流程图，在图中用序号给标识了核心流程。结合流程图、后面的 Raft 的日志图，来简要分析 Leader B 收到 put hello 为 world 的请求后，是如何将此请求同步给其他 Follower 节点的。

![etcd23](/assets/images/202501/etcd23.png)

* 首先 Leader 收到 client 的请求后，etcdserver 的 KV 模块会向 Raft 模块提交一个 put hello 为 world 提案消息（流程图中的序号 2 流程），它的消息类型是 MsgProp。

* Leader 的 Raft 模块获取到 MsgProp 提案消息后，为此提案生成一个日志条目，追加到未持久化、不稳定的 Raft 日志中，随后会遍历集群 Follower 列表和进度信息，为每个 Follower 生成追加（MsgApp）类型的 RPC 消息，此消息中包含待复制给 Follower 的日志条目。

* **这里就出现两个疑问了。第一，Leader 是如何知道从哪个索引位置发送日志条目给 Follower，以及 Follower 已复制的日志最大索引是多少呢？第二，日志条目什么时候才会追加到稳定的 Raft 日志中呢？Raft 模块负责持久化吗？**

* 首先介绍下什么是 Raft 日志。下图是 Raft 日志复制过程中的日志细节图，简称日志图 1。在日志图中，最上方的是日志条目序号 / 索引，日志由有序号标识的一个个条目组成，每个日志条目内容保存了 Leader 任期号和提案内容。最开始的时候，A 节点是 Leader，任期号为 1，A 节点 crash 后，B 节点通过选举成为新的 Leader， 任期号为 2。日志图 1 描述的是 hello 日志条目未提交前的各节点 Raft 日志状态。

![etcd24](/assets/images/202501/etcd24.png)

* 回答第一个疑问。**Leader 会维护两个核心字段来追踪各个 Follower 的进度信息**，一个字段是 `NextIndex`，它**表示 Leader 发送给 Follower 节点的下一个日志条目索引**。一个字段是 `MatchIndex`，它**表示 Follower 节点已复制的最大日志条目的索引**，比如上面的日志图 1 中 C 节点的已复制最大日志条目索引为 5，A 节点为 4。

* 第二个疑问。**etcd Raft 模块设计实现上抽象了网络、存储、日志等模块，它本身并不会进行网络、存储相关的操作**，上层应用需结合自己业务场景选择内置的模块或自定义实现网络、存储、日志等模块。

* 上层应用通过 Raft 模块的输出接口（如 Ready 结构），获取到待持久化的日志条目和待发送给 Peer 节点的消息后（如上面的 MsgApp 日志消息），需持久化日志条目到自定义的 WAL 模块，通过自定义的网络模块将消息发送给 Peer 节点。

* **日志条目持久化到稳定存储中后，这时候就可以将日志条目追加到稳定的 Raft 日志中**。即便这个日志是内存存储，节点重启时也不会丢失任何日志条目，因为 WAL 模块已持久化此日志条目，可通过它重建 Raft 日志。etcd Raft 模块提供了一个内置的内存存储（MemoryStorage）模块实现，etcd 使用的就是它，**Raft 日志条目保存在内存中**。网络模块并未提供内置的实现，etcd 基于 HTTP 协议实现了 peer 节点间的网络通信，并根据消息类型，支持选择 pipeline、stream 等模式发送，显著提高了网络吞吐量、降低了延时。

* 解答完以上两个疑问后，我们继续分析 etcd 是如何与 Raft 模块交互，获取待持久化的日志条目和发送给 peer 节点的消息。

* 正如刚刚讲到的，Raft 模块输入是 `Msg` 消息，输出是一个 `Ready` 结构，它包含待持久化的日志条目、发送给 peer 节点的消息、已提交的日志条目内容、线性查询结果等 Raft 输出核心信息。

* etcdserver 模块通过 channel 从 Raft 模块获取到 Ready 结构后（流程图中的序号 3 流程），因 B 节点是 Leader，它首先会通过基于 HTTP 协议的网络模块将追加日志条目消息（MsgApp）广播给 Follower，并同时将待持久化的日志条目持久化到 WAL 文件中（流程图中的序号 4 流程），最后将日志条目追加到稳定的 Raft 日志存储中（流程图中的序号 5 流程）。

* 各个 Follower 收到追加日志条目（MsgApp）消息，并通过安全检查后，它会持久化消息到 WAL 日志中，并将消息追加到 Raft 日志存储，随后会向 Leader 回复一个应答追加日志条目（MsgAppResp）的消息，告知 Leader 当前已复制的日志最大索引（流程图中的序号 6 流程）。

* Leader 收到应答追加日志条目（MsgAppResp）消息后，会将 Follower 回复的已复制日志最大索引更新到跟踪 Follower 进展的 `MatchIndex` 字段，如下面的日志图 2 中的 Follower C `MatchIndex` 为 6，Follower A 为 5，**日志图 2 描述的是 hello 日志条目提交后的各节点 Raft 日志状态**。

![etcd25](/assets/images/202501/etcd25.png)

* 最后 Leader 根据 Follower 的 `MatchIndex` 信息，计算出一个位置，如果这个位置已经被一半以上节点持久化，那么这个位置之前的日志条目都可以被标记为已提交。

* 在这个案例中日志图 2 里 6 号索引位置之前的日志条目已被多数节点复制，那么他们状态都可被设置为已提交。**Leader 可通过在发送心跳消息（MsgHeartbeat）给 Follower 节点时，告知它已经提交的日志索引位置**。

* 最后各个节点的 etcdserver 模块，可通过 channel 从 Raft 模块获取到已提交的日志条目（流程图中的序号 7 流程），应用日志条目内容到存储状态机（流程图中的序号 8 流程），返回结果给 client。

* **通过以上流程，Leader 就完成了同步日志条目给 Follower 的任务，一个日志条目被确定为已提交的前提是，它需要被 Leader 同步到一半以上节点上。以上就是 etcd Raft 日志复制的核心原理**。

> 安全性

* 介绍完 Leader 选举和日志复制后，最后再来看看 Raft 是如何保证安全性的。

* 如果在上面的日志图 2 中，Leader B 在应用日志指令 put hello 为 world 到状态机，并返回给 client 成功后，突然 crash 了，那么 Follower A 和 C 是否都有资格选举成为 Leader 呢？

* 从日志图 2 中我们可以看到，如果 A 成为了 Leader 那么就会导致数据丢失，因为它并未含有刚刚 client 已经写入成功的 put hello 为 world 指令。

* Raft 算法如何确保面对这类问题时不丢数据和各节点数据一致性呢？

* 这就是 Raft 的第三个子问题需要解决的。Raft 通过给选举和日志复制增加一系列规则，来实现 Raft 算法的安全性。


> 选举规则

* 当节点收到选举投票的时候，**需检查候选者的最后一条日志中的任期号，若小于自己则拒绝投票**。**如果任期号相同，日志却比自己短，也拒绝为其投票**。

* 比如在日志图 2 中，Folllower A 和 C 任期号相同，但是 Follower C 的数据比 Follower A 要长，那么在选举的时候，Follower C 将拒绝投票给 A， 因为它的数据不是最新的。

* 同时，对于一个给定的任期号，最多只会有一个 leader 被选举出来，leader 的诞生需获得集群一半以上的节点支持。每个节点在同一个任期内只能为一个节点投票，节点需要将投票信息持久化，防止异常重启后再投票给其他节点。

* 通过以上规则就可防止日志图 2 中的 Follower A 节点成为 Leader。

> 日志复制规则

* 在日志图 2 中，Leader B 返回给 client 成功后若突然 crash 了，此时可能还并未将 6 号日志条目已提交的消息通知到 Follower A 和 C，那么如何确保 6 号日志条目不被新 Leader 删除呢？ 同时在 etcd 集群运行过程中，Leader 节点若频繁发生 crash 后，可能会导致 Follower 节点与 Leader 节点日志条目冲突，如何保证各个节点的同 Raft 日志位置含有同样的日志条目？

* **以上各类异常场景的安全性是通过 Raft 算法中的 Leader 完全特性和只附加原则、日志匹配等安全机制来保证的**。
  + **Leader 完全特性**，是指如果某个日志条目在某个任期号中已经被提交，那么这个条目必然出现在更大任期号的所有 Leader 中。
  + **Leader 只能追加日志条目，不能删除已持久化的日志条目**（**只附加原则**），因此 Follower C 成为新 Leader 后，会将前任的 6 号日志条目复制到 A 节点。
  + 为了保证各个节点日志一致性，Raft 算法在追加日志的时候，引入了一致性检查。Leader 在发送追加日志 RPC 消息时，会把新的日志条目紧接着之前的条目的索引位置和任期号包含在里面。
  + Follower 节点会检查相同索引位置的任期号是否与 Leader 一致，一致才能追加，这就是**日志匹配特性**。它本质上是一种归纳法，一开始日志空满足匹配特性，随后每增加一个日志条目时，都要求上一个日志条目信息与 Leader 一致，那么最终整个日志集肯定是一致的。

* 通过以上的 Leader 选举限制、Leader 完全特性、只附加原则、日志匹配等安全特性，Raft 就实现了一个可严格通过数学反证法、归纳法证明的高可用、一致性算法，为 etcd 的安全性保驾护航。

### Q&A

#### 哪些场景会出现 Follower 日志与 Leader 冲突，etcd WAL 模块只能持续追加日志条目，那冲突后 Follower 是如何删除无效的日志条目呢？

* 哪些场景会出现 Follower 日志与 Leader 冲突？

leader 崩溃的情况下可能 (如老的 leader 可能还没有完全复制所有的日志条目)，如果 leader 和 follower 出现持续崩溃会加剧这个现象。follower 可能会丢失一些在新的 leader 中有的日志条目，它也可能拥有一些 leader 没有的日志条目，或者两者都发生。

* follower 如何删除无效日志？

**leader 处理不一致是通过强制 follower 直接复制自己的日志来解决**。因此在 follower 中的冲突的日志条目会被 leader 的日志覆盖。leader 会记录 follower 的日志复制进度 nextIndex，如果 follower 在追加日志时一致性检查失败，就会拒绝请求，此时 leader 就会减小 nextIndex 值并进行重试，最终在某个位置让 follower 跟 leader 一致。

**这里补充下为什么 WAL 日志模块只通过追加，也能删除已持久化冲突的日志条目呢？** 其实这里 etcd 在实现上采用了一些比较有技巧的方法，在 WAL 日志中的确没删除废弃的日志条目，你可以在其中搜索到冲突的日志条目。只是 etcd 加载 WAL 日志时，发现一个 raft log index 位置上有多个日志条目的时候，会通过覆盖的方式，将最后写入的日志条目追加到 raft log 中，实现了删除冲突日志条目效果，你如果感兴趣可以参考下我和 Google ptabor [关于这个问题的讨论](https://github.com/etcd-io/etcd/issues/12589)。

#### Raft 日志和 WAL 日志的区别

WAL 日志是用来持久化 raft 日志条目和相关集群元数据信息的，可防止节点发生重启、crash 后，对应的已提交日志条目丢失等异常情况，一般情况下，它是将数据持久化到磁盘中。

Raft 日志记录了节点过去一段时间内收到的写请求，如 put/del/txn 操作等，一般是通过一个内存数组来存储最近一系列日志条目。当 Follower 节点落后Leader较小时，就可以通过 Leader 内存中维护的日志条目信息, 将落后的日志条目发送给它，最终各个节点应用一样的日志条目内容，来确保各个节点数据一致性。

etcd 节点重启后，可通过 WAL 日志来重建部分 raft 日志条目。


## 鉴权控制

当使用 etcd 存储业务敏感数据、多租户共享使用同 etcd 集群的时候，应该如何防止匿名用户访问 etcd 数据呢？多租户场景又如何最小化用户权限分配，防止越权访问的？那么 etcd 是如何实现多种鉴权机制和细粒度的权限控制的？在实现鉴权模块的过程中最核心的挑战是什么？又该如何确保鉴权的安全性以及提升鉴权性能呢？

### 整体架构

etcd 鉴权体系架构由**控制面**和**数据面**组成。

![etcd26](/assets/images/202501/etcd26.png)

* 上图是 etcd 鉴权体系**控制面**，可以通过客户端工具 etcdctl 和鉴权 API 动态调整认证、鉴权规则，AuthServer 收到请求后，为了确保各节点间鉴权元数据一致性，会通过 Raft 模块进行数据同步。

* 当对应的 Raft 日志条目被集群半数以上节点确认后，Apply 模块通过鉴权存储 (AuthStore) 模块，执行日志条目的内容，将规则存储到 boltdb 的一系列“鉴权表”里面。

* 下图是**数据面鉴权流程**，由**认证**和**授权**流程组成。**认证的目的**是检查 client 的身份是否合法、防止匿名用户访问等。目前 etcd 实现了两种认证机制，分别是**密码认证**和**证书认证**。

![etcd27](/assets/images/202501/etcd27.png)

* 认证通过后，为了提高密码认证性能，会分配一个 Token（类似我们生活中的门票、通信证）给 client，client 后续其他请求携带此 Token，server 就可快速完成 client 的身份校验工作。

* 实现分配 Token 的服务也有多种，这是 TokenProvider 所负责的，目前支持 SimpleToken 和 JWT 两种。

* 通过认证后，在访问 MVCC 模块之前，还需要通过**授权流程**。**授权的目的**是检查 client 是否有权限操作你请求的数据路径，etcd 实现了 **RBAC 机制**，支持为每个用户分配一个角色，为每个角色授予最小化的权限。

![etcd28](/assets/images/202501/etcd28.png)

下面就以 put hello 命令为例，深入分析以上鉴权体系是如何进行身份认证来防止匿名访问的，又是如何实现细粒度的权限控制以防止越权访问的。


### 认证

* 如何防止匿名用户访问你的 etcd 数据呢？解决方案当然是认证用户身份。那 etcd 提供了哪些机制来验证 client 身份呢？etcd 目前实现了两种机制，分别是**用户密码认证**和**证书认证**，下面分别介绍这两种机制在 etcd 中如何实现，以及这两种机制各自的优缺点。

* 密码认证
  + etcd 支持为每个用户分配一个账号名称、密码。密码认证在我们生活中无处不在，从银行卡取款到微信、微博 app 登录，再到核武器发射，密码认证应用及其广泛，是最基础的鉴权的方式。
  + 但密码认证存在两大难点，它们分别是如何**保障密码安全性**和**提升密码认证性能**。

#### etcd 的鉴权模块如何安全存储用户密码

* etcd 的用户密码存储正是融合了以上讨论的高安全性 hash 函数（Blowfish encryption algorithm）、随机的加盐 salt、可自定义的 hash 值计算迭代次数 cost。

* 下面通过几个简单 etcd 鉴权 API，介绍密码认证的原理。

* 首先可以通过如下的 auth enable 命令开启鉴权，注意 etcd 会先要求你创建一个 `root` 账号，**它拥有集群的最高读写权限**。

``` bash
$ etcdctl user add root:root
User root created
$ etcdctl auth enable
Authentication Enabled
```

* 启用鉴权后，这时 client 发起如下 put hello 操作时， etcd server 会返回 "user name is empty" 错误给 client，就初步达到了防止匿名用户访问你的 etcd 数据目的。那么 etcd server 是在哪里做的鉴权的呢?

``` bash
$ etcdctl put hello world
Error: etcdserver: user name is empty
```

* etcd server 收到 put hello 请求的时候，在提交到 Raft 模块前，它会从你请求的上下文中获取你的用户身份信息。如果你未通过认证，那么在状态机应用 put 命令的时候，检查身份权限的时候发现是空，就会返回此错误给 client。

* 下面通过鉴权模块的 user 命令，给 etcd 增加一个 alice 账号。一起来看看 etcd 鉴权模块是如何基于上面介绍的技术方案，来安全存储 alice 账号信息。

``` bash
$ etcdctl user add alice:alice --user root:root
User alice created
```

* 鉴权模块收到此命令后，它会使用 bcrpt 库的 blowfish 算法，基于明文密码、随机分配的 salt、自定义的 cost、迭代多次计算得到一个 hash 值，并将加密算法版本、salt 值、cost、hash 值组成一个字符串，作为加密后的密码。

* 最后，鉴权模块将用户名 alice 作为 key，用户名、加密后的密码作为 value，存储到 boltdb 的 authUsers bucket 里面，完成一个账号创建。

* 当使用 alice 账号访问 etcd 的时候，需要先调用鉴权模块的 Authenticate 接口，它会验证你的身份合法性。

* 那么 etcd 如何验证你密码正确性的呢？**鉴权模块首先会根据请求的用户名 alice，从 boltdb 获取加密后的密码，因此 hash 值包含了算法版本、salt、cost 等信息，因此可以根据请求中的明文密码，计算出最终的 hash 值，若计算结果与存储一致，那么身份校验通过**。

#### 如何提升密码认证性能

* 通过以上的鉴权安全性的深入分析，知道身份验证这个过程开销极其昂贵，那么问题来了，如何避免频繁、昂贵的密码计算匹配，提升密码认证的性能呢？这就是密码认证的第二个难点，如何保证性能。

* 想想我们办理港澳通行证的时候，流程特别复杂，需要各种身份证明、照片、指纹信息，办理成功后，下发通信证，每次过关你只需要刷下通信证即可，高效而便捷。

* 那么，在软件系统领域如果身份验证通过了后，我们是否也可以返回一个类似通信证的凭据给 client，后续请求携带通信证，只要通行证合法且在有效期内，就无需再次鉴权了呢？

* 是的，etcd 也有类似这样的凭据。当 etcd server 验证用户密码成功后，它就会返回一个 **Token** 字符串给 client，用于表示用户的身份。后续请求携带此 **Token**，就无需再次进行密码校验，实现了通信证的效果。

* etcd 目前支持两种 Token，分别为 **Simple Token** 和 **JWT Token**。



### 授权

* 当使用如上创建的 alice 账号执行 put hello 操作的时候，etcd 却会返回如下的 "etcdserver: permission denied" 无权限错误，这是为什么呢？

``` bash
$ etcdctl put hello world --user alice:alice
Error: etcdserver: permission denied
```

* 这是因为开启鉴权后，put 请求命令在应用到状态机前，etcd 还会对发出此请求的用户进行权限检查，判断其是否有权限操作请求的数据。常用的权限控制方法有 `ACL` (Access Control List)`、ABAC` (Attribute-based access control)、`RBAC` (Role-based access control)，etcd 实现的是 `RBAC` 机制。

#### RBAC 基于角色权限的控制系统

它由下图中的三部分组成，User、Role、Permission。

* User 表示用户，如 alice。
* Role 表示角色，它是权限的赋予对象。
* Permission 表示具体权限明细，比如赋予 Role 对 key 范围在 `[key，KeyEnd]` 数据拥有什么权限。目前支持三种权限，分别是 READ、WRITE、READWRITE。

![etcd29](/assets/images/202501/etcd29.png)

* 下面我们通过 etcd 的 RBAC 机制，给 alice 用户赋予一个可读写 `[hello,helly]` 数据范围的读写权限，如何操作呢？按照上面介绍的 RBAC 原理，首先需要创建一个 role，这里命名为 admin，然后新增了一个可读写 `[hello,helly]` 数据范围的权限给 admin 角色，并将 admin 的角色的权限授予了用户 alice。详细如下：

``` bash
$ #创建一个admin role
etcdctl role add admin  --user root:root
Role admin created
# #分配一个可读写[hello，helly]范围数据的权限给admin role
$ etcdctl role grant-permission admin readwrite hello helly --user root:root
Role admin updated
# 将用户alice和admin role关联起来，赋予admin权限给user
$ etcdctl user grant-role alice admin --user root:root
Role admin is granted to user alice
```

* 然后当你再次使用 etcdctl 执行 put hello 命令时，鉴权模块会从 boltdb 查询 alice 用户对应的权限列表。因为有可能一个用户拥有成百上千个权限列表，etcd 为了提升权限检查的性能，引入了**区间树**，检查用户操作的 key 是否在已授权的区间，时间复杂度仅为 `O(logN)`。

* 在这个案例中，很明显 hello 在 admin 角色可读写的 `[hello，helly)` 数据范围内，因此它有权限更新 key hello，执行成功。你也可以尝试更新 key hey，因为此 key 未在鉴权的数据区间内，因此 etcd server 会返回 "etcdserver: permission denied" 错误给 client，如下所示。

``` bash
$ etcdctl put hello world --user alice:alice
OK
$ etcdctl put hey hey --user alice:alice
Error: etcdserver: permission denied
```





### 测试

```
$ etcdctl user add root:root
User root created

$ etcdctl auth enable
{"level":"warn","ts":"2025-01-21T17:04:38.470215+0800","logger":"etcd-client","caller":"v3@v3.5.17/retry_interceptor.go:63","msg":"retrying of unary invoker failed","target":"etcd-endpoints://0xc0004483c0/127.0.0.1:2379","attempt":0,"error":"rpc error: code = FailedPrecondition desc = etcdserver: root user does not have root role"}
Authentication Enabled

 etcdctl put hello world --endpoints http://127.0.0.1:2379
{"level":"warn","ts":"2025-01-21T17:05:03.187828+0800","logger":"etcd-client","caller":"v3@v3.5.17/retry_interceptor.go:63","msg":"retrying of unary invoker failed","target":"etcd-endpoints://0xc000242000/127.0.0.1:2379","attempt":0,"error":"rpc error: code = InvalidArgument desc = etcdserver: user name is empty"}
Error: etcdserver: user name is empty

$ etcdctl user add gerry:gerry --user root:root
User gerry created

$ etcdctl put hello world --endpoints http://127.0.0.1:2379 --user gerry:gerry
{"level":"warn","ts":"2025-01-21T17:08:36.060296+0800","logger":"etcd-client","caller":"v3@v3.5.17/retry_interceptor.go:63","msg":"retrying of unary invoker failed","target":"etcd-endpoints://0xc0004b83c0/127.0.0.1:2379","attempt":0,"error":"rpc error: code = PermissionDenied desc = etcdserver: permission denied"}
Error: etcdserver: permission denied

$ etcdctl role add damin
{"level":"warn","ts":"2025-01-21T17:13:26.112285+0800","logger":"etcd-client","caller":"v3@v3.5.17/retry_interceptor.go:63","msg":"retrying of unary invoker failed","target":"etcd-endpoints://0xc0005a41e0/127.0.0.1:2379","attempt":0,"error":"rpc error: code = InvalidArgument desc = etcdserver: user name is empty"}
Error: etcdserver: user name is empty

$ etcdctl role add admin --user root:root
Role admin created

$ etcdctl role grant-permission admin readwrite hello world --user root:root
Role admin updated

$ etcdctl user grant-role gerry admin --user root:root
Role admin is granted to user gerry

$ etcdctl put hello world --endpoints http://127.0.0.1:2379 --user gerry:gerry
OK
```

重要业务不建议多租户模式哈，多租户场景不同租户可能会相互影响，导致各种稳定性问题，除非各个租户的行为是可控的，可信赖的。


# Tools

## 进程管理工具 [goreman](https://github.com/mattn/goreman)


``` bash
go install github.com/mattn/goreman@latest
```

goreman Procfile 文件：https://github.com/etcd-io/etcd/blob/v3.5.17/Procfile

* 它描述了 etcd 进程名、节点数、参数等信息。
* 最后通过 `goreman -f Procfile start` 命令就可以快速启动一个 3 节点的本地集群了。

``` bash
# Use goreman to run `go get github.com/mattn/goreman`

etcd1: /usr/local/bin/etcd --name infra1 --listen-client-urls http://127.0.0.1:2379 --advertise-client-urls http://127.0.0.1:2379 --listen-peer-urls http://127.0.0.1:12380 --initial-advertise-peer-urls http://127.0.0.1:12380 --initial-cluster-token etcd-cluster-1 --initial-cluster 'infra1=http://127.0.0.1:12380,infra2=http://127.0.0.1:22380,infra3=http://127.0.0.1:32380' --initial-cluster-state new --enable-pprof --logger=zap --log-outputs=stderr
etcd2: /usr/local/bin/etcd --name infra2 --listen-client-urls http://127.0.0.1:22379 --advertise-client-urls http://127.0.0.1:22379 --listen-peer-urls http://127.0.0.1:22380 --initial-advertise-peer-urls http://127.0.0.1:22380 --initial-cluster-token etcd-cluster-1 --initial-cluster 'infra1=http://127.0.0.1:12380,infra2=http://127.0.0.1:22380,infra3=http://127.0.0.1:32380' --initial-cluster-state new --enable-pprof --logger=zap --log-outputs=stderr
etcd3: /usr/local/bin/etcd --name infra3 --listen-client-urls http://127.0.0.1:32379 --advertise-client-urls http://127.0.0.1:32379 --listen-peer-urls http://127.0.0.1:32380 --initial-advertise-peer-urls http://127.0.0.1:32380 --initial-cluster-token etcd-cluster-1 --initial-cluster 'infra1=http://127.0.0.1:12380,infra2=http://127.0.0.1:22380,infra3=http://127.0.0.1:32380' --initial-cluster-state new --enable-pprof --logger=zap --log-outputs=stderr

#proxy: /usr/local/bin/etcd grpc-proxy start --endpoints=127.0.0.1:2379,127.0.0.1:22379,127.0.0.1:32379 --listen-addr=127.0.0.1:23790 --advertise-client-url=127.0.0.1:23790 --enable-pprof

# A learner node can be started using Procfile.learner
```

使用 goreman 启动 etcd 输出日志信息：

![etcd11](/assets/images/202501/etcd11.png)


查看进程信息：

``` bash
$ ps ux | grep etcd | grep -v grep
gerryya+ 1865815  0.9  0.0 11738844 30340 pts/0  Sl   14:57   0:00 /usr/local/bin/etcd --name infra1 --listen-client-urls http://127.0.0.1:2379 --advertise-client-urls http://127.0.0.1:2379 --listen-peer-urls http://127.0.0.1:12380 --initial-advertise-peer-urls http://127.0.0.1:12380 --initial-cluster-token etcd-cluster-1 --initial-cluster infra1=http://127.0.0.1:12380,infra2=http://127.0.0.1:22380,infra3=http://127.0.0.1:32380 --initial-cluster-state new --enable-pprof --logger=zap --log-outputs=stderr
gerryya+ 1865816  0.8  0.0 11738844 29556 pts/0  Sl   14:57   0:00 /usr/local/bin/etcd --name infra3 --listen-client-urls http://127.0.0.1:32379 --advertise-client-urls http://127.0.0.1:32379 --listen-peer-urls http://127.0.0.1:32380 --initial-advertise-peer-urls http://127.0.0.1:32380 --initial-cluster-token etcd-cluster-1 --initial-cluster infra1=http://127.0.0.1:12380,infra2=http://127.0.0.1:22380,infra3=http://127.0.0.1:32380 --initial-cluster-state new --enable-pprof --logger=zap --log-outputs=stderr
gerryya+ 1865817  0.7  0.0 11739100 31676 pts/0  Sl   14:57   0:00 /usr/local/bin/etcd --name infra2 --listen-client-urls http://127.0.0.1:22379 --advertise-client-urls http://127.0.0.1:22379 --listen-peer-urls http://127.0.0.1:22380 --initial-advertise-peer-urls http://127.0.0.1:22380 --initial-cluster-token etcd-cluster-1 --initial-cluster infra1=http://127.0.0.1:12380,infra2=http://127.0.0.1:22380,infra3=http://127.0.0.1:32380 --initial-cluster-state new --enable-pprof --logger=zap --log-outputs=stderr
```







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











