---
layout: post
title:  "ETCD in Action"
date:   2023-09-09 09:00:00 +0800
categories: GoLang
---

* Do not remove this line (it will not be displayed)
{:toc}

`etcd` 是一个分布式键值存储系统，用于存储和管理配置数据。`etcd` 使用一种称为 `Raft` 的共识算法来确保数据在集群中的一致性。


# Install

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




# Quickstart [v3.5](https://etcd.io/docs/v3.5/) ()

These docs cover everything from setting up and running an etcd cluster to using etcd in applications.

Follow these instructions to locally install, run, and test a single-member cluster of etcd:

* Install etcd from pre-built binaries or from source. For details, see [Install](https://etcd.io/docs/v3.5/install/).

> **Important**: Ensure that you perform the last step of the installation instructions to verify that `etcd` is in your path.


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

### 安装和测试 etcd 功能脚本

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

### etcd 的运行脚本

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

### etcd 用例测试

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

## 'data-dir' was empty; using default","data-dir":"default.etcd


## etcd 默认启动参数

```
starting an etcd server","etcd-version":"3.5.17","git-sha":"507c0de","go-version":"go1.22.9","go-os":"linux","go-arch":"amd64","max-cpu-set":48,"max-cpu-available":48,"member-initialized":false,"name":"default","data-dir":"default.etcd","wal-dir":"","wal-dir-dedicated":"","member-dir":"default.etcd/member","force-new-cluster":false,"heartbeat-interval":"100ms","election-timeout":"1s","initial-election-tick-advance":true,"snapshot-count":100000,"max-wals":5,"max-snapshots":5,"snapshot-catchup-entries":5000,"initial-advertise-peer-urls":["http://localhost:2380"],"listen-peer-urls":["http://localhost:2380"],"advertise-client-urls":["http://localhost:2379"],"listen-client-urls":["http://localhost:2379"],"listen-metrics-urls":[],"cors":["*"],"host-whitelist":["*"],"initial-cluster":"default=http://localhost:2380","initial-cluster-state":"new","initial-cluster-token":"etcd-cluster","quota-backend-bytes":2147483648,"max-request-bytes":1572864,"max-concurrent-streams":4294967295,"pre-vote":true,"initial-corrupt-check":false,"corrupt-check-time-interval":"0s","compact-check-time-enabled":false,"compact-check-time-interval":"1m0s","auto-compaction-mode":"periodic","auto-compaction-retention":"0s","auto-compaction-interval":"0s","discovery-url":"","discovery-proxy":"","downgrade-check-interval":"5s"
```

## opened backend db","path":"default.etcd/member/snap/db","took":"4.283251ms





# Refer

* https://etcd.io/docs/v3.5/
* https://etcd.io/docs/v3.4/op-guide/performance/











