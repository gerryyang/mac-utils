---
layout: post
title:  "Redis 5 in Action"
date:   2018-10-23 13:00:00 +0800
categories: redis
---

* Do not remove this line (it will not be displayed)
{:toc}



一提起数据库，大多数人可能想到的是`Oracle`，`MySQL`以及`Microsoft SQL Server`这三大巨头。但除此外，很多新兴的数据库也开始被开发者使用。例如，基于海量日志查询的[Elasticsearch]，在高并发场景使用[Redis]做缓存。[DB-engines]上可以看到近期数据库的使用排名情况。可以看到Redis作为一款NoSQL数据库（非关系型数据库）其排名正处在一个上升的阶段。Redis使用ANSI C编写，目前是一位意大利的开发者[Salvatore Sanfilippo]来维护，他在twitter上非常活跃，可以帮助开发者了解更多最新的Redis信息，也可订阅官方的[Redis邮件组]。可以在GitHub上找到最新的[Redis源码]，也有一个[Redis在线测试]工具可以尝试Redis的一些基本用法。

![db_rank](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/db_rank.jpg)


# Redis初步

Redis is an open source (BSD licensed), `in-memory` data structure store, used as a `database`, `cache` and `message broker`. It supports data structures such as `strings`, `hashes`, `lists`, `sets`, `sorted sets` with range queries, `bitmaps`, `hyperloglogs` and `geospatial indexes` with radius queries. Redis has built-in `replication`, `Lua scripting`, `LRU eviction`, `transactions` and `different levels of on-disk persistence`, and provides high availability via `Redis Sentinel` and `automatic partitioning` with Redis Cluster.

可以将Redis视为一个复杂的[Memcached]，因为Redis支持更多的数据类型操作。

* 当前最新稳定的版本是[Redis 5.0.0(Released Wed Oct 17 13:28:26 CEST 2018)]
* 通过[README]可以快速了解其基本用法
* 支持的平台包括，Linux, OSX, OpenBSD, NetBSD, FreeBSD
* 如果要查看详细的编译过程，可以指定`make V=1`
* Redis在`deps`目录下是对外部第三方库的依赖包括，jemalloc, lua, hiredis, linenoise，如果要重新编译第三方库需要先执行`make distclean`，默认不会编译
* Redis在Linux下默认使用jemalloc，而在其他环境下默认使用libc malloc。jemalloc相比libc malloc对内存碎片问题处理的更好。通过`MALLOC`环境变量指定使用哪种内存分配器。

```
make MALLOC=jemalloc
make MALLOC=libc
```

* 运行Redis的方式，所有redis.conf配置中的选项，也支持在命令行指定

```
cd src
./redis-server
./redis-server /path/to/redis.conf

./redis-server --port 9999 --replicaof 127.0.0.1 6379
./redis-server /etc/redis/6379.conf --loglevel debug
```

* 编译后安装Redis的bin文件，不包括`初始化脚本`和`配置文件`

```
make install    # /usr/local/bin
make PREFIX=/some/other/directory install
```

对于生产环境的部署，如果是Ubuntu或者Debian系统，可以参考以下脚本

```
cd utils
./install_server.sh
```

对Redis实例的起停

```
/etc/init.d/redis_<portnumber>

/etc/init.d/redis_6379
```

# Redis代码结构

| 目录 | 含义
| -- | --
| src | contains the Redis implementation, written in `C`
| tests | contains the unit tests, implemented in `Tcl`
| deps | contains libraries Redis uses
| utils | 
| some files | Makefile/README.md/redis.conf/sentinel.conf/...

关于deps目录的补充说明：

* Everything needed to compile Redis is inside this directory; your system just needs to provide `libc`, `a POSIX compatible interface` and `a C compiler`. 
* Notably **deps** contains a copy of `jemalloc`, which is the default allocator of Redis under Linux. 
* Note that under deps there are also things which started with the Redis project, but for which the main repository is not antirez/redis. An exception to this rule is deps/geohash-int which is the low level geocoding library used by Redis: it originated from a different project, but at this point it diverged so much that it is developed as a separated entity directly inside the Redis repository.

## src/server.h

Redis的所有配置和一些状态信息，全部记录在`struct redisServer`结构里。

![redisServer](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/redisServer.jpg)

``` cpp
struct redisServer {
    /* General */
    pid_t pid;                  /* Main process pid. */
    char *configfile;           /* Absolute config file path, or NULL */
    char *executable;           /* Absolute executable file path. */
    char **exec_argv;           /* Executable argv vector (copy). */
    int dynamic_hz;             /* Change hz value depending on # of clients. */
    int config_hz;              /* Configured HZ value. May be different than
                                   the actual 'hz' field value if dynamic-hz
                                   is enabled. */

    // ...
};
```

需要关心的几个重要结构包括：

### server.db

``` cpp
redisDb *db;
```

``` cpp
/* Redis database representation. There are multiple databases identified
 * by integers from 0 (the default database) up to the max configured
 * database. The database number is the 'id' field in the structure. */
typedef struct redisDb {
    dict *dict;                 /* The keyspace for this DB */
    dict *expires;              /* Timeout of keys with a timeout set */
    dict *blocking_keys;        /* Keys with clients waiting for data (BLPOP)*/
    dict *ready_keys;           /* Blocked keys that received a PUSH */
    dict *watched_keys;         /* WATCHED keys for MULTI/EXEC CAS */
    int id;                     /* Database ID */
    long long avg_ttl;          /* Average TTL, just for stats */
    list *defrag_later;         /* List of key names to attempt to defrag one by one, gradually. */
} redisDb;
```

### server.commands

``` cpp
 dict *commands;             /* Command table */
```

``` cpp
// src/dict.h
// Hash Tables Implementation

/* This is our hash table structure. Every dictionary has two of this as we
 * implement incremental rehashing, for the old to the new table. */
typedef struct dictht {
    dictEntry **table;
    unsigned long size;
    unsigned long sizemask;
    unsigned long used;
} dictht;

typedef struct dict {
    dictType *type;
    void *privdata;
    dictht ht[2];
    long rehashidx; /* rehashing not in progress if rehashidx == -1 */
    unsigned long iterators; /* number of iterators currently running */
} dict;
```

### server.clients

记录客户端的连接。(a linked list of clients connected to the server)

``` cpp
list *clients;              /* List of active clients */
```

### server.master 

a special client, the master, if the instance is a replica.

``` cpp
client *master;     /* Client that is master for this slave */
```

### client


``` cpp
// src/server.h

/* With multiplexing we need to take per-client state.
 * Clients are taken in a linked list. */
struct client {
    int fd;
    sds querybuf;
    int argc;
    robj **argv;
    redisDb *db;
    int flags;
    list *reply;
    char buf[PROTO_REPLY_CHUNK_BYTES];
    ... many other fields ...
}
```

* The `fd` field is the client socket file descriptor.
* `argc` and `argv` are populated with the command the client is executing, so that functions implementing a given Redis command can read the arguments.
* `querybuf` accumulates the requests from the client, which are parsed by the Redis server according to the Redis protocol and executed by calling the implementations of the commands the client is executing.
* `reply` and `buf` are dynamic and static buffers that accumulate the replies the server sends to the client. These buffers are incrementally written to the socket as soon as the file descriptor is writable.

### redisObject

* `struct redisObject`可以表示Redis的所有基本类型，例如，strings, lists, sets, sorted sets等
* 通过`type`字段，区分实际表示哪类对象
* 并通过`refcount`字段实现引用计数，防止对象的多次创建

``` cpp
#define LRU_BITS 24

typedef struct redisObject {
    unsigned type:4;
    unsigned encoding:4;
    unsigned lru:LRU_BITS; /* LRU time (relative to global lru_clock) or
                            * LFU data (least significant 8 bits frequency
                            * and most significant 16 bits access time). */
    int refcount;
    void *ptr;
} robj;
```

## src/server.c





# 数据类型

首先需要了解[Redis Data Types]有哪些。


# 执行命令

进一步了解[Redis Command]有哪些命令。




[Redis]: https://redis.io/
[Elasticsearch]: https://www.elastic.co/products/elasticsearch
[DB-engines]: https://db-engines.com/en/ranking

[Redis System Properties]: https://db-engines.com/en/system/Redis
[Salvatore Sanfilippo]: https://twitter.com/antirez
[Redis源码]: https://github.com/antirez/redis
[Redis在线测试]: http://try.redis.io/
[Redis邮件组]: https://groups.google.com/forum/?fromgroups#!forum/redis-db

[Redis 5.0.0]: http://download.redis.io/releases/redis-5.0.0.tar.gz
[README]: http://download.redis.io/redis-stable/README.md

[Redis Documentation]: https://redis.io/documentation


[Implement a Twitter Clone in Redis]: https://redis.io/topics/twitter-clone
[Redis Data Types]: https://redis.io/topics/data-types-intro

[Redis Command]: https://redis.io/commands

[Memcached]: https://memcached.org/