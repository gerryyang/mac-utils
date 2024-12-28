---
layout: post
title:  "Prometheus in Action"
date:   2023-1-30 20:00:00 +0800
categories: 云原生
---

* Do not remove this line (it will not be displayed)
{:toc}

# Prometheus 简介

> Prometheus, a [Cloud Native Computing Foundation](https://cncf.io/) project, is a service monitoring system. It collects **metrics** from configured targets at given intervals, evaluates rule expressions, displays the results, and can trigger alerts when specified conditions are observed.


`Prometheus` 是一个开源监控系统。与 `Kubernetes` 相似，Prometheus 受启发于 Google 的 `Borgman` 监控系统，而 `Kubernetes` 也是从 Google 的 `Borg` 演变而来的。`Prometheus` 始于 2012 年，并由 SoundCloud 内部工程师开发，于 2015 年 1 月发布。2016 年 5 月，其成为继 `Kubernetes` 之后第二个正式加入 **Cloud Native Computing Foundation**（`CNCF`） 基金会的项目。现最常见的 `Kubernetes` 容器管理系统中，通常会搭配 `Prometheus` 进行监控。


![prometheus_arch](/assets/images/202304/prometheus_arch.svg)

# 安装测试

There are various ways of [installing Prometheus](https://github.com/prometheus/prometheus#install).

1. Precompiled binaries
2. Docker images
3. Building from source



# Prometheus 主要功能

The features that distinguish Prometheus from other metrics and monitoring systems are:

* A **multi-dimensional data model** (time series defined by metric name and set of key/value dimensions)

> 自定义多维数据模型（时序列数据由 `Metric` 和一组 `Key/Value Label` 组成）

* PromQL, **a powerful and flexible query language** to leverage this dimensionality

> 灵活而强大的查询语言 `PromQL`，可利用多维数据完成复杂的监控查询

* No dependency on distributed storage; **single server nodes are autonomous**

> 这句话的意思是，系统不依赖于分布式存储，单个服务器节点是自治的 (不依赖分布式存储，支持单主节点工作)。
> 这意味着系统中的每个服务器节点都可以独立地运行，而不需要依赖其他节点或外部存储系统。每个节点都包含自己的存储和计算资源，可以独立地处理请求和存储数据。
> 相比于分布式存储系统，这种单节点自治的系统具有以下优点：
>
> 1. 简单易用：由于不需要配置和管理分布式存储系统，因此这种系统更容易部署和维护。
>
> 2. 可靠性高：由于每个节点都是自治的，因此系统的可靠性更高。即使一个节点出现故障，其他节点仍然可以继续工作。
>
> 3. 性能高：由于不需要在多个节点之间传输数据，因此这种系统的性能更高。每个节点都可以直接访问本地存储，从而提高了数据访问速度。
>
> 总之，单节点自治的系统适用于一些小规模的应用场景，例如个人博客、小型网站等。但是，对于大规模的应用场景，分布式存储系统仍然是更好的选择，因为它可以提供更高的可扩展性和容错性。

* An HTTP **pull model** for time series collection

> 通过基于 `HTTP` 的 `Pull` 方式采集时序数据

* **Pushing time series** is supported via an intermediary gateway for batch jobs

> 可通过 `PushGateway` 的方式来实现数据 `Push` 模式

* Targets are discovered via **service discovery** or **static configuration**

> 可通过动态的服务发现或者静态配置去获取要采集的目标服务器

* Multiple modes of **graphing and dashboarding support**

> 结合 `Grafana` 可方便的支持多种可视化图表及仪表盘

* Support for hierarchical and horizontal **federation**

> 支持水平扩展，联邦机制


# 监控场景

根据监控分层，Prometheus 监控服务覆盖了**业务监控**、**应用层监控**、**中间件监控**、**系统层监控**。

* 系统层监控：例如 CPU、Memory、Disk 和 Network 等。
* 中间组件层监控：例如 Kafka、MySQL 和 Redis 等。
* 应用层监控：例如 JVM、HTTP 和 RPC 等。
* 业务监控：例如登录数和订单量等。




# 基本概念

* `Exporter`: 是一个采集监控数据并通过 Prometheus 监控规范对外提供数据的组件。目前有上百个官方或者三方 `Exporter` 可供使用，请参见 [Exporter 详情](https://prometheus.io/docs/instrumenting/exporters/)。
* `PromQL`: Prometheus 监控服务的查询语言。支持瞬时查询和时间跨度查询，内置多种函数和操作符。可以对原始数据进行聚合、切片、预测和联合。
* `Metric`: 采集目标暴露的、可以完整反映监控对象运行或者业务状态的一系列标签化数据。
* `Label`: 描述指标的一组 Key-Value 值。
* `Series`: 时间序列（时间线），由指标名（Metric）和标签（Label）组成。相同的指标名和标签在时间序列中构成唯一的一条时间线。
* Series 上限: 指标个数上限，Series 上限= (单个指标 × 该指标的维度组合) × 指标个数。
* `Remote Write`: 支持作为远程数据库存储 Prometheus 监控服务的数据。可以使用 Remote Write 地址，将自建 Prometheus 的监控数据存储到 Prometheus 监控服务的实例中，实现远程存储，并可视化展示在同一 Grafana。
* 预聚合: 对一些常用的指标或者计算相对复杂的指标进行提前计算，然后将这些数据存储到新的数据指标中，提前计算好的指标查询速度更快，可以解决用户配置以及查询慢的问题。

> Note:
>
> 标签（Label）的作用：Prometheus 中存储的数据为时间序列，是由指标名和一系列的标签（键值对）唯一标识的，不同的标签代表不同的时间序列，即通过指定标签查询指定数据。添加的标签越多，查询的维度越细。


# 存储模型

Prometheus 内置的时序数据库 TSDB。

Prometheus 读写的是**时序数据**，与一般的数据对象相比，时序数据有其特殊性，TSDB 对此进行了大量针对性的设计与优化。因此理解时序数据是理解 Prometheus 存储模型的第一步。通常它由如下所示的**标识**和**采样数据**两部组成：

```
标识 -> {(t0, v0), (t1, v1), (t2, v2), ...}
```

**标识**，用于**区分各个不同的监控指标**。在 Prometheus 中通常用**指标名 + 一系列的 label** 唯一地标识**一个时间序列**。如下为 Prometheus 抓取的一条时间序列，其中 `http_request_tota` l为**指标名**，表示 HTTP 请求的总数，它有 `path` 和 `method` 两个 **label**，用于表示各种请求的路径和方法。

```
http_request_total{path="/", method="GET"} -> {(t0, v1), (t1, v1), ...}
```

事实上，**指标名**最后也是作为一个特殊的 **label** 被存储的，它的 key 为 `__name__`，如下所示。最终 Prometheus 存储在数据库中的时间序列标识就是一堆 **label**。我们将这堆 **label** 称为 `series`。

```
{__name__="http_request_total", path="/", method="GET"}
```

采样数据则由诸多的**采样点**（Prometheus 中称为 `sample`）构成。`t0, t1, t2, ...` 表示**样本采集的时间**，`v0, v1, v2, ...` 则表示**指标在采集时刻的值**。**采样时间**一般是单调递增的并且相邻 sample 的时间间隔往往相同，Prometheus 中默认为 15s。而且一般相邻 sample 的指标值 v 并不会相差太多。基于采样数据的上述特性，对它进行高效地压缩存储是完全可能的。Prometheus 对于采样数据压缩算法的实现，参考了 Facebook 的时序数据库 `Gorilla` 中的做法，**通过该算法，16 字节的 sample 平均只需要 1.37 个字节的存储空间**。


**监控数据**是一种**时效性非常强的数据类型**，它被查询的热度会随着时间的流逝而不断降低，而且对于监控指标的访问通常会指定一个时间段，例如，最近十五分钟，最近一小时，最近一天等等。**一般来说，最近一个小时采集到的数据被访问地是最为频繁的，过去一天的数据也经常会被访问用来了解某个指标整体的波动情况，而一个月乃至一年之前的数据被访问的意义就不是很大了**。

基于监控数据的上述特性，TSDB 的设计就非常容易理解了，其整体架构如下：

对于最新采集到的数据，Prometheus 会直接将它们存放在**内存**中，从而加快数据的读写。但是内存的空间是有限的，而且随着时间的推移，内存中较老的那部分数据被访问的概率也逐渐降低。因此，默认情况下，每隔两小时 Prometheus 就会将部分“老”数据持久化到磁盘，每一次持久化的数据都独立存放在磁盘的一个 Block 中。例如上图中的 block0 就存放了 `[t0, t1]` 时间段内 Prometheus 采集的所有监控数据。这样做的好处很明显，如果我们想要访问某个指标在 `[t0, t2]` 范围内的数据，那么只需要加载 block0 和 block1 中的数据并进行查找即可，这样一来大大缩小了查找的范围，从而提高了查询的速度。

虽然最近采集的数据存放在内存中能够提高读写效率，但是由于内存的易失性，一旦 Prometheus 崩溃（如果系统内存不足，Prometheus 被 OOM 的概率并不算低）那么这部分数据就彻底丢失了。因此 Prometheus 在将采集到的数据真正写入内存之前，会首先存入 `WAL`（`Write Ahead Log`）中。因为 `WAL` 是存放在磁盘中的，相当于对内存中的监控数据做了一个完全的备份，即使 Prometheus 崩溃这部分的数据也不至于丢失。当 Prometheus 重启之后，它首先会将WAL的内容加载到内存中，从而完美恢复到崩溃之前的状态，接着再开始新数据的抓取。

![tsdb](/assets/images/202409/tsdb.png)

refer:

* https://www.cnblogs.com/YaoDD/p/11391335.html




# 存储容灾

Prometheus 2.x 采用**自定义的存储格式**将样本数据保存在**本地磁盘**当中。按照**两个小时**（最少时间）为一个时间窗口，将两小时内产生的数据存储在一个块 (Block) 中，每一个块中包含该时间窗口内的所有样本数据 (chunks)，元数据文件 (meta.json) 以及索引文件 (index)。

```
$ tree
.
├── 01E2MA5GDWMP69GVBVY1W5AF1X
│   ├── chunks               # 保存压缩后的时序数据，每个 chunks 大小为 512M，超过会生成新的 chunks
│   │   └── 000001
│   ├── index                # chunks 中的偏移位置
│   ├── meta.json            # 记录 block 块元信息，比如 样本的起始时间、chunks 数量和数据量大小等
│   └── tombstones           # 通过 API 方式对数据进行软删除，将删除记录存储在此处（API 的删除方式，并不是立即将数据从 chunks 文件中移除）
├── 01E2MH175FV0JFB7EGCRZCX8NF
│   ├── chunks
│   │   └── 000001
│   ├── index
│   ├── meta.json
│   └── tombstones
├── 01E2MQWYDFQAXXPB3M1HK6T20A
│   ├── chunks
│   │   └── 000001
│   ├── index
│   ├── meta.json
│   └── tombstones
├── lock
├── queries.active
└── wal                      # 防止数据丢失(数据收集上来暂时是存放在内存中，WAL 记录了这些信息)
    ├── 00000366             # 每个数据段最大为 128M，默认存储两个小时的数据量
    ├── 00000367
    ├── 00000368
    ├── 00000369
    └── checkpoint.000365
        └── 00000000
```

## Block

TSDB 将存储的监控数据按照时间分成多个 block 存储，默认最小的 block 保存时间为 2h。后台程序还会将小块合并成大块，减少内存中 block 的数量，便于索引查找数据，可以通过 meta.json 查看，可以看到01E2MA5GDWMP69GVBVY1W5AF1X 被压缩 1 次，source 有 3 个 block，那么 2*3=6 小时的数据量。


## WAL (Write-ahead logging, 预写日志)

Prometheus 为了防止丢失暂存在内存中的还未被写入磁盘的监控数据，引入了 WAL 机制。WAL 被分割成默认大小为 128M 的文件段（segment），之前版本默认大小是 256M，文件段以数字命名，长度为 8 位的整型。WAL 的写入单位是页（page），每页的大小为 32KB，所以每个段大小必须是页的大小的整数倍。如果 WAL 一次性写入的页数超过一个段的空闲页数，就会创建一个新的文件段来保存这些页，从而确保一次性写入的页不会跨段存储。

> Prometheus 将周期性采集到的数据通过 Add 接口添加到 head block，但是这些数据暂时没有持久化，TSDB 通过 WAL 将数据保存到磁盘上(保存的数据没有压缩，占用内存较大)，当出现宕机，启动多协程读取WAL，恢复数据。



# Querying Prometheus

Prometheus provides a functional query language called `PromQL` (**Prometheus Query Language**) that lets the user select and aggregate time series data in real time. The result of an expression can either be shown as a graph, viewed as tabular data in Prometheus's expression browser, or consumed by external systems via the [HTTP API](https://prometheus.io/docs/prometheus/latest/querying/api/).

## Expression language data types

In Prometheus's expression language, an expression or sub-expression can evaluate to one of four types:

* **Instant vector** - a set of time series containing a single sample for each time series, all sharing the same timestamp
* **Range vector** - a set of time series containing a range of data points over time for each time series
* **Scalar** - a simple numeric floating point value
* **String** - a simple string value; currently unused

## Literals

### String literals

Strings may be specified as literals in **single quotes**, **double quotes** or **backticks**.

PromQL follows the same [escaping rules as Go](https://go.dev/ref/spec#String_literals). In single or double quotes a backslash begins an escape sequence, which may be followed by `a`, `b`, `f`, `n`, `r`, `t`, `v` or `\`. Specific characters can be provided using octal (`\nnn`) or hexadecimal (`\xnn`, `\unnnn` and `\Unnnnnnnn`).

> No escaping is processed inside backticks. Unlike Go, Prometheus does not discard newlines inside backticks.

Example:

```
"this is a string"
'these are unescaped: \n \\ \t'
`these are not unescaped: \n ' " \t`
```

### Float literals

Scalar float values can be written as literal integer or floating-point numbers in the format (whitespace only included for better readability):

```
[-+]?(
      [0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?
    | 0[xX][0-9a-fA-F]+
    | [nN][aA][nN]
    | [iI][nN][fF]
)
```

Examples:

```
23
-2.43
3.4e-9
0x8f
-Inf
NaN
```

## Time series Selectors

### Instant vector selectors

Instant vector selectors allow the selection of a set of time series and a single sample value for each at a given timestamp (instant): in the simplest form, only a metric name is specified. This results in an instant vector containing elements for all time series that have this metric name.

This example selects all time series that have the `http_requests_total` metric name:

```
http_requests_total
```

> It is possible to filter these time series further by appending a comma separated list of label matchers in curly braces (`{}`).

This example selects only those time series with the `http_requests_total` metric name that also have the `job` label set to `prometheus` and their `group` label set to `canary`:

```
http_requests_total{job="prometheus",group="canary"}
```

> It is also possible to negatively match a label value, or to match label values against regular expressions. The following label matching operators exist:

* `=`: Select labels that are exactly equal to the provided string.
* `!=`: Select labels that are not equal to the provided string.
* `=~`: Select labels that regex-match the provided string.
* `!~`: Select labels that do not regex-match the provided string.

> Regex matches are fully anchored. A match of `env=~"foo"` is treated as `env=~"^foo$"`.

For example, this selects all `http_requests_total` time series for `staging`, `testing`, and `development` environments and HTTP methods other than `GET`.

```
http_requests_total{environment=~"staging|testing|development",method!="GET"}
```

> Label matchers that match empty label values also select all time series that do not have the specific label set at all. It is possible to have multiple matchers for the same label name.

Vector selectors must either specify a name or at least one label matcher that does not match the empty string. The following expression is illegal:

```
{job=~".*"} # Bad!
```

In contrast, these expressions are valid as they both have a selector that does not match empty label values.

```
{job=~".+"}              # Good!
{job=~".*",method="get"} # Good!
```

Label matchers can also be applied to metric names by matching against the internal `__name__` label. For example, the expression `http_requests_total` is equivalent to {`__name__="http_requests_total"`}. Matchers other than `=` (`!=`, `=~`, `!~`) may also be used. The following expression selects all metrics that have a name starting with `job:`:

```
{__name__=~"job:.*"}
```

### Range Vector Selectors

Range vector literals work like instant vector literals, except that they select a range of samples back from the current instant. Syntactically, a [time duration](https://prometheus.io/docs/prometheus/latest/querying/basics/#time-durations) is appended in square brackets (`[]`) at the end of a vector selector to specify how far back in time values should be fetched for each resulting range vector element.

In this example, we select all the values we have recorded within the last 5 minutes for all time series that have the metric name `http_requests_total` and a `job` label set to `prometheus`:

```
http_requests_total{job="prometheus"}[5m]
```

### Time Durations

Time durations are specified as a number, followed immediately by one of the following units:

* `ms` - milliseconds
* `s` - seconds
* `m` - minutes
* `h` - hours
* `d` - days - assuming a day has always 24h
* `w` - weeks - assuming a week has always 7d
* `y` - years - assuming a year has always 365d

> Time durations can be combined, by concatenation. Units must be ordered from the longest to the shortest. A given unit must only appear once in a time duration.

Here are some examples of valid time durations:

```
5h
1h30m
5m
10s
```

### Offset modifier

The `offset` modifier allows changing the time offset for individual instant and range vectors in a query.

For example, the following expression returns the value of `http_requests_total` 5 minutes in the past relative to the current query evaluation time:

```
http_requests_total offset 5m
```

Note that the offset modifier always needs to follow the selector immediately, i.e. the following would be correct:

```
sum(http_requests_total{method="GET"} offset 5m) // GOOD.
```

While the following would be incorrect:

```
sum(http_requests_total{method="GET"}) offset 5m // INVALID.
```

The same works for range vectors. This returns the 5-minute rate that http_requests_total had a week ago:

```
rate(http_requests_total[5m] offset 1w)
```

For comparisons with temporal shifts forward in time, a negative offset can be specified:

```
rate(http_requests_total[5m] offset -1w)
```

> Note that this allows a query to look ahead of its evaluation time.


### @ modifier

The `@` modifier allows changing the evaluation time for individual instant and range vectors in a query. The time supplied to the `@` modifier is a unix timestamp and described with a float literal.

For example, the following expression returns the value of `http_requests_total` at `2021-01-04T07:40:00+00:00`:

```
http_requests_total @ 1609746000
```

Note that the `@` modifier always needs to follow the selector immediately, i.e. the following would be correct:

```
sum(http_requests_total{method="GET"} @ 1609746000) // GOOD.
```

While the following would be incorrect:

```
sum(http_requests_total{method="GET"}) @ 1609746000 // INVALID.
```

The same works for range vectors. This returns the 5-minute rate that `http_requests_total` had at `2021-01-04T07:40:00+00:00`:

```
rate(http_requests_total[5m] @ 1609746000)
```

## Subquery

Subquery allows you to run an instant query for a given range and resolution. The result of a subquery is a range vector.

Syntax: `<instant_query> '[' <range> ':' [<resolution>] ']' [ @ <float_literal> ] [ offset <duration> ]`

* `<resolution>` is optional. Default is the global evaluation interval.


## Operators

Prometheus supports many binary and aggregation operators. These are described in detail in the [expression language operators](https://prometheus.io/docs/prometheus/latest/querying/operators/) page.


## Functions

Prometheus supports several functions to operate on data. These are described in detail in the [expression language functions](https://prometheus.io/docs/prometheus/latest/querying/functions/) page.


## Comments

PromQL supports line comments that start with `#`. Example:

```
    # This is a comment
```

## Gotchas

### Avoiding slow queries and overloads

If a query needs to operate on a very large amount of data, graphing it might time out or overload the server or browser. Thus, when constructing queries over unknown data, always start building the query in the tabular view of Prometheus's expression browser until the result set seems reasonable (hundreds, not thousands, of time series at most). Only when you have filtered or aggregated your data sufficiently, switch to graph mode. If the expression still takes too long to graph ad-hoc, pre-record it via a [recording rule](https://prometheus.io/docs/prometheus/latest/configuration/recording_rules/#recording-rules).

如果一个查询需要处理大量的数据，将其绘制成图表可能会导致超时或者服务器或浏览器负载过高。因此，在构建对未知数据的查询时，应该始终从 Prometheus 表达式浏览器的表格视图开始构建查询，直到结果集看起来合理（最多只有数百个时间序列，而不是数千个）。只有在对数据进行足够的过滤或聚合后，才切换到图形模式。如果表达式仍然需要太长时间才能进行 ad-hoc 绘图，则可以通过记录规则预先记录它。

This is especially relevant for Prometheus's query language, where a bare metric name selector like `api_http_requests_total` could expand to thousands of time series with different labels. Also keep in mind that expressions which aggregate over many time series will generate load on the server even if the output is only a small number of time series. This is similar to how it would be slow to sum all values of a column in a relational database, even if the output value is only a single number.

这一点在 Prometheus 的查询语言中尤为重要，因为一个裸的指标名称选择器（如 api_http_requests_total）可能会扩展成数千个带有不同标签的时间序列。此外，需要注意的是，即使输出的时间序列数量很少，对许多时间序列进行聚合的表达式也会在服务器上产生负载。这类似于在关系型数据库中对一列的所有值求和会很慢，即使输出值只有一个数字。

为了避免这些问题，选择指标和标签时需要具有选择性和精确性。应该尝试使用标签匹配表达式来缩小感兴趣的时间序列集合，并避免使用裸的指标名称选择器，除非你真的需要查询与该指标匹配的所有时间序列。

此外，应该尽量少使用聚合函数，只有在真正需要对许多时间序列进行聚合时才使用。如果可能的话，应该尝试使用过滤器和选择器来减少需要聚合的时间序列数量，并使用更具体的聚合函数，如 sum by() 或 avg by() 来对特定的标签进行聚合。

通过遵循这些最佳实践，可以减少 Prometheus 服务器的负载，提高监控系统的性能和可扩展性。



## Examples

## Simple time series selection

Return all time series with the metric `http_requests_total`:

```
http_requests_total
```

Return all time series with the metric `http_requests_total` and the given `job` and `handler` labels:

```
http_requests_total{job="apiserver", handler="/api/comments"}
```

Return a whole range of time (in this case 5 minutes up to the query time) for the same vector, making it a [range vector](https://prometheus.io/docs/prometheus/latest/querying/basics/#range-vector-selectors):

```
http_requests_total{job="apiserver", handler="/api/comments"}[5m]
```

> Note that an expression resulting in a range vector cannot be graphed directly, but viewed in the tabular ("Console") view of the expression browser.

Using regular expressions, you could select time series only for jobs whose name match a certain pattern, in this case, all jobs that end with `server`:

```
http_requests_total{job=~".*server"}
```

> All regular expressions in Prometheus use [RE2 syntax](https://github.com/google/re2/wiki/Syntax).

To select all HTTP status codes except 4xx ones, you could run:

```
http_requests_total{status!~"4.."}
```

## Subquery

Return the 5-minute rate of the `http_requests_total` metric for the past 30 minutes, with a resolution of 1 minute.

```
rate(http_requests_total[5m])[30m:1m]
```

This is an example of a nested subquery. The subquery for the `deriv` function uses the default resolution. Note that using subqueries unnecessarily is unwise.

```
max_over_time(deriv(rate(distance_covered_total[5s])[30s:5s])[10m:])
```

## Using functions, operators, etc.

Return the per-second rate for all time series with the `http_requests_total` metric name, as measured over the last 5 minutes:

```
rate(http_requests_total[5m])
```

Assuming that the `http_requests_total` time series all have the labels `job` (fanout by job name) and `instance` (fanout by instance of the job), we might want to sum over the rate of all instances, so we get fewer output time series, but still preserve the `job` dimension:

```
sum by (job) (
  rate(http_requests_total[5m])
)
```

If we have two different metrics with the same dimensional labels, we can apply binary operators to them and elements on both sides with the same label set will get matched and propagated to the output. For example, this expression returns the unused memory in MiB for every instance (on a fictional cluster scheduler exposing these metrics about the instances it runs):


```
(instance_memory_limit_bytes - instance_memory_usage_bytes) / 1024 / 1024
```

The same expression, but summed by application, could be written like this:

```
sum by (app, proc) (
  instance_memory_limit_bytes - instance_memory_usage_bytes
) / 1024 / 1024
```

If the same fictional cluster scheduler exposed CPU usage metrics like the following for every instance:

```
instance_cpu_time_ns{app="lion", proc="web", rev="34d0f99", env="prod", job="cluster-manager"}
instance_cpu_time_ns{app="elephant", proc="worker", rev="34d0f99", env="prod", job="cluster-manager"}
instance_cpu_time_ns{app="turtle", proc="api", rev="4d3a513", env="prod", job="cluster-manager"}
instance_cpu_time_ns{app="fox", proc="widget", rev="4d3a513", env="prod", job="cluster-manager"}
...
```

We could get the top 3 CPU users grouped by application (`app`) and process type (`proc`) like this:

```
topk(3, sum by (app, proc) (rate(instance_cpu_time_ns[5m])))
```

Assuming this metric contains one time series per running instance, you could count the number of running instances per application like this:

```
count by (app) (instance_cpu_time_ns)
```


# [Exposition Formats](https://prometheus.io/docs/instrumenting/exposition_formats/)

Metrics can be exposed to Prometheus using a simple text-based exposition format. There are various [client libraries](https://prometheus.io/docs/instrumenting/clientlibs/) that implement this format for you.

## Text-based format

As of Prometheus version 2.0, all processes that expose metrics to Prometheus need to use a text-based format. In this section you can find some [basic information](https://prometheus.io/docs/instrumenting/exposition_formats/#basic-info) about this format as well as a more [detailed breakdown](https://prometheus.io/docs/instrumenting/exposition_formats/#text-format-details) of the format.

Prometheus' text-based format is line oriented. Lines are separated by a line feed character (`\n`). The last line must end with a line feed character. Empty lines are ignored.

Lines with a `#` as the first non-whitespace character are **comments**. They are ignored unless the first token after `#` is either **HELP** or **TYPE**. Those lines are treated as follows:

If the token is **HELP**, at least one more token is expected, which is the metric name. All remaining tokens are considered the docstring for that metric name. HELP lines may contain any sequence of UTF-8 characters (after the metric name), but the backslash and the line feed characters have to be escaped as \ and \n, respectively. Only one HELP line may exist for any given metric name.

If the token is **TYPE**, exactly two more tokens are expected. The first is the metric name, and the second is either **counter**, **gauge**, **histogram**, **summary**, or **untyped**, defining the type for the metric of that name. Only one TYPE line may exist for a given metric name. The TYPE line for a metric name must appear before the first sample is reported for that metric name. If there is no TYPE line for a metric name, the type is set to untyped.

## Text format example

以下是一个完整的Prometheus指标输出示例，包括注释、HELP和TYPE表达式、直方图、摘要、字符转义示例等：

```
# HELP http_requests_total The total number of HTTP requests.
# TYPE http_requests_total counter
http_requests_total{method="GET", handler="/api/v1/users"} 100
http_requests_total{method="POST", handler="/api/v1/users"} 50

# HELP http_request_duration_seconds The duration of HTTP requests in seconds.
# TYPE http_request_duration_seconds histogram
http_request_duration_seconds_bucket{method="GET", handler="/api/v1/users", le="0.1"} 10
http_request_duration_seconds_bucket{method="GET", handler="/api/v1/users", le="0.5"} 20
http_request_duration_seconds_bucket{method="GET", handler="/api/v1/users", le="1"} 30
http_request_duration_seconds_bucket{method="GET", handler="/api/v1/users", le="+Inf"} 40
http_request_duration_seconds_sum{method="GET", handler="/api/v1/users"} 123.45
http_request_duration_seconds_count{method="GET", handler="/api/v1/users"} 100

# HELP http_request_size_bytes The size of HTTP requests in bytes.
# TYPE http_request_size_bytes summary
http_request_size_bytes{method="GET", handler="/api/v1/users", quantile="0.5"} 100
http_request_size_bytes{method="GET", handler="/api/v1/users", quantile="0.9"} 200
http_request_size_bytes{method="GET", handler="/api/v1/users", quantile="0.99"} 300
http_request_size_bytes_sum{method="GET", handler="/api/v1/users"} 12345
http_request_size_bytes_count{method="GET", handler="/api/v1/users"} 100

# HELP escaped_characters_examples Examples of escaped characters in HELP lines.
# TYPE escaped_characters_examples untyped
escaped_characters_examples{example="backslash"} 1 # This is a backslash: \\
escaped_characters_examples{example="newline"} 2 # This is a newline: \n
escaped_characters_examples{example="quote"} 3 # This is a quote: \"

# HELP unicode_characters_examples Examples of Unicode characters in HELP lines.
# TYPE unicode_characters_examples untyped
unicode_characters_examples{example="smiley_face"} 1 # This is a smiley face: 😊
unicode_characters_examples{example="heart"} 2 # This is a heart: ❤️
```

在这个示例中，我们定义了三个指标：`http_requests_total`、`http_request_duration_seconds`和`http_request_size_bytes`。每个指标都有一个`HELP`行和一个`TYPE`行，用于指定**指标的帮助文档**和**类型**。`http_requests_total`是一个计数器指标，`http_request_duration_seconds`是一个直方图指标，`http_request_size_bytes`是一个摘要指标。

**每个指标都有一些样本数据，用于报告指标的值**。每个样本都包含一个或多个**标签**，用于**标识指标的不同维度**。在`http_request_duration_seconds`和`http_request_size_bytes`指标中，我们还使用了**桶**和**分位数**，用于报告**指标的分布情况**。

在`HELP`行中，我们使用了转义字符和Unicode字符，以演示如何在HELP行中包含**特殊字符**。在这个示例中，我们使用了反斜杠来转义反斜杠、换行符和引号。我们还使用了Unicode字符来显示笑脸和心形符号。

总的来说，这个示例展示了如何使用Prometheus的注释和指标输出格式，以便更好地理解和使用指标数据。




# [Storage](https://prometheus.io/docs/prometheus/latest/storage/#overview)

Prometheus includes a local on-disk time series database, but also optionally integrates with remote storage systems.

## Local storage

Prometheus's local time series database stores data in a custom, highly efficient format on local storage.

## Remote storage integrations

Prometheus's local storage is limited to a single node's scalability and durability. Instead of trying to solve clustered storage in Prometheus itself, Prometheus offers a set of interfaces that allow integrating with remote storage systems.

Prometheus integrates with remote storage systems in three ways:

1. Prometheus can write samples that it ingests to a remote URL in a standardized format.
2. Prometheus can receive samples from other Prometheus servers in a standardized format.
3. Prometheus can read (back) sample data from a remote URL in a standardized format.


# Q&A

## the query hit the max number of series limit (limit: 100000 series)

这个错误表示查询返回的时间序列数量超过了 Prometheus 的默认限制，即 100000 个时间序列。这个限制是为了防止查询过于耗费资源，导致 Prometheus 的性能下降。

要解决这个问题，可以尝试以下几种方法：

1. 优化查询：尝试缩小查询的时间范围、减少查询的标签数量等，以减少返回的时间序列数量。
2. 增加限制：可以通过修改 Prometheus 的配置文件，增加 --storage.tsdb.max-block-duration 和 --storage.tsdb.min-block-duration 参数的值，以增加 Prometheus 存储时间序列的块大小，从而提高查询的限制。
3. 水平扩展：如果以上方法无法解决问题，可以考虑使用水平扩展的方式，即增加 Prometheus 实例的数量，将查询分散到多个实例上，从而提高查询的并发能力和容量。

```
topk(1, count ({__name__=~"msgame_P.*"}) by(__name__))
```


# Manual

* https://github.com/prometheus/prometheus
* https://prometheus.io/docs/prometheus/latest/querying/functions/
* https://prometheus.io/docs/prometheus/latest/querying/examples/
* https://github.com/google/re2/wiki/Syntax

# Refer

* [Prometheus 实战](https://songjiayang.gitbooks.io/prometheus/content/promql/summary.html)
* [腾讯云 Prometheus 介绍](https://cloud.tencent.com/document/product/1416/55770)
* [PromQL Cheat Sheet: Must-Know PromQL Queries](https://last9.io/blog/promql-cheat-sheet/)
* [初识 PromQL](https://prometheus.fuckcloudnative.io/di-san-zhang-prometheus/di-4-jie-cha-xun/basics)





