---
layout: post
title:  "Redis 5.0.0 in Action"
date:   2018-10-23 13:00:00 +0800
categories: redis
---

* Do not remove this line (it will not be displayed)
{:toc}

一提起数据库，大多数人可能想到的是`Oracle`，`MySQL`以及`Microsoft SQL Server`这三大巨头。但除此外，很多新兴的数据库也开始被开发者使用。例如，基于海量日志查询的[Elasticsearch]，在高并发场景使用[Redis]做缓存。[DB-engines]上可以看到近期数据库的使用排名情况。可以看到Redis作为一款NoSQL数据库（非关系型数据库）其排名正处在一个上升的阶段。Redis使用ANSI C编写，目前是一位意大利的开发者[Salvatore Sanfilippo]来维护，他在twitter上非常活跃，可以帮助开发者了解更多最新的Redis信息，也可订阅官方的[Redis邮件组]。可以在GitHub上找到最新的[Redis源码]，也有一个[Redis在线测试]工具可以尝试Redis的一些基本用法。

![db_rank](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/db_rank.jpg)

# 数据库对比

| 名称 | 类型 | 数据存储选项 | 查询类型 | 附加功能
| -- | -- | -- | -- | --
| Redis | 使用内存存储(in-memory)的非关系数据库 | 字符串、列表、集合、散列表、有序集合 | 每种数据类型都有自己的专属命令，另外还有批量操作(bulk operation)和不完全(partial)的事务支持 | 发布与订阅，主从复制，持久化，存储过程
| memcached | 使用内存存储的键值缓存 | 键值之间的映射 | 创建命令、读取命令、更新命令、删除命令以及其他几个命令 | 为提升性能而设的多线程服务器
| MySQL | 关系数据库 | 每个数据库可以包含多个表，每个表可以包含多个行，可以处理多个表的视图(view)，支持空间(spatial)和第三方扩展 | SELECT, INSERT, UPDATE, DELETE, 函数, 存储过程 | 支持ACID性质(需要使用InnoDB)，主从复制，主主复制
| PostgreSQL | 关系数据库 | 基本同MySQL | 基本同MySQL | 支持ACID性质，主从复制，由第三方支持的多主复制
| MongoDB| 使用硬盘存储(on-disk)的非关系文档存储 | 每个数据库可以包含多个表，每个表可以包含多个schema-less的BSON文档 | 创建命令，读取命令，更新命令，删除命令，条件查询命令等 | 支持map-reduce操作，主从复制，分片，空间索引(spatial index) 

# Redis初步

Redis is an open source (BSD licensed), `in-memory` data structure store, used as a `database`, `cache` and `message broker`. It supports data structures such as `strings`, `hashes`, `lists`, `sets`, `sorted sets` with range queries, `bitmaps`, `hyperloglogs` and `geospatial indexes` with radius queries. Redis has built-in `replication`, `Lua scripting`, `LRU eviction`, `transactions` and `different levels of on-disk persistence`, and provides high availability via `Redis Sentinel` and `automatic partitioning` with Redis Cluster.

可以将Redis视为一个复杂的[Memcached]，因为Redis支持更多的数据类型操作。

* Redis可以存储key与5种不同类型的value之间的映射
* Redis可以将存储在内存的键值对数据持久化到硬盘
* Redis可以使用复制特性来扩展读性能
* Redis可以使用客户端分片来扩展写性能

> 分片是一种将数据划分为多个部分的方法，对数据的划分可以基于键包含的ID、基于键的散列值、或者基于以上两者的某种组合。通过对数据进行分片，用户可以将数据存储到多台机器里面，也可以从多台机器里面获取数据，这种方法在解决某些问题时可以获得线性级别的性能提升。

* Redis有两种不同的`持久化方法`。一种是时间点转储(point-in-time dump)，第二种是将所有修改了数据库的命令都写入一个只追加(append-only)文件里面
* Redis通过主从复制，来提供故障转移(failover)支持。执行复制的从服务器会连接上主服务器，接收主服务器发送的整个数据库的初始副本，然后主服务器执行的`写命令`，都会发送给所有连着的从服务器去执行，从而实时地更新从服务器的数据集。因此，客户端可以读任意一台Redis集群的机器获取数据。(即，强同步的方式)
* 使用Redis一般是基于性能(通过内存操作，避免对磁盘的随机读或者随机写)或者功能(比如，使用Redis可以将聚合数据放到有序集合`ZSET`里面，构建出一个实时的排行榜)的考虑，如果因为费用原因而没办法将大量数据存储到内存里面，则会选择关系数据库

* 当前最新稳定的版本是[Redis 5.0.0(Released Wed Oct 17 13:28:26 CEST 2018)]
* 通过[README]可以快速了解其基本用法
* 支持的平台包括，Linux, OSX, OpenBSD, NetBSD, FreeBSD
* Redis的安装
	+ 从 http://redis.io/download下载最新的stable版本的源码
	+ 编译(如果要查看详细的编译过程，可以指定`make V=1`)，安装和启动Redis
	+ 下载并安装Python语言的Redis客户端库

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

* 对Redis实例的起停

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

### 主流程

Redis server的入口`main()`函数在此文件定义，可以了解到Redis内部是如何启动的。

``` cpp
int main(int argc, char **argv) {
	// ...

	// setups the default values of the server structure
	initServerConfig();

	// allocates the data structures needed to operate, setup the listening socket, and so forth
	initServer();

	// starts the event loop which listens for new connections
	aeMain(server.el);
}
```

### 周期性任务

有两个特殊的函数，在event loop中被周期地调用。

1. `serverCron()` is called periodically (according to `server.hz` frequency), and performs tasks that must be performed from time to time, like checking for timedout clients.
2. `beforeSleep()` is called every time the event loop fired, Redis served a few requests, and is returning back into the event loop.

``` cpp
/* This is our timer interrupt, called server.hz times per second.
 * Here is where we do a number of things that need to be done asynchronously.
 * For instance:
 *
 * - Active expired keys collection (it is also performed in a lazy way on
 *   lookup).
 * - Software watchdog.
 * - Update some statistic.
 * - Incremental rehashing of the DBs hash tables.
 * - Triggering BGSAVE / AOF rewrite, and handling of terminated children.
 * - Clients timeout of different kinds.
 * - Replication reconnection.
 * - Many more...
 *
 * Everything directly called here will be called server.hz times per second,
 * so in order to throttle execution of things we want to do less frequently
 * a macro is used: run_with_period(milliseconds) { .... }
 */

int serverCron(struct aeEventLoop *eventLoop, long long id, void *clientData)
```

``` cpp
/* This function gets called every time Redis is entering the
 * main loop of the event driven library, that is, before to sleep
 * for ready file descriptors. */
void beforeSleep(struct aeEventLoop *eventLoop);
```

### 其他重要功能

* `call()` is used in order to call a given command in the context of a given client.
* `activeExpireCycle()` handles eviciton of keys with a time to live set via the `EXPIRE` command.
* `freeMemoryIfNeeded()` is called when a new write command should be performed but Redis is out of memory according to the `maxmemory` directive.
* The global variable `redisCommandTable` defines all the Redis commands, specifying the name of the command, the function implementing the command, the number of arguments required, and other properties of each command.


``` cpp
/* Call() is the core of Redis execution of a command.
 *
 * The following flags can be passed:
 * CMD_CALL_NONE        No flags.
 * CMD_CALL_SLOWLOG     Check command speed and log in the slow log if needed.
 * CMD_CALL_STATS       Populate command stats.
 * CMD_CALL_PROPAGATE_AOF   Append command to AOF if it modified the dataset
 *                          or if the client flags are forcing propagation.
 * CMD_CALL_PROPAGATE_REPL  Send command to salves if it modified the dataset
 *                          or if the client flags are forcing propagation.
 * CMD_CALL_PROPAGATE   Alias for PROPAGATE_AOF|PROPAGATE_REPL.
 * CMD_CALL_FULL        Alias for SLOWLOG|STATS|PROPAGATE.
 *
 * The exact propagation behavior depends on the client flags.
 * Specifically:
 *
 * 1. If the client flags CLIENT_FORCE_AOF or CLIENT_FORCE_REPL are set
 *    and assuming the corresponding CMD_CALL_PROPAGATE_AOF/REPL is set
 *    in the call flags, then the command is propagated even if the
 *    dataset was not affected by the command.
 * 2. If the client flags CLIENT_PREVENT_REPL_PROP or CLIENT_PREVENT_AOF_PROP
 *    are set, the propagation into AOF or to slaves is not performed even
 *    if the command modified the dataset.
 *
 * Note that regardless of the client flags, if CMD_CALL_PROPAGATE_AOF
 * or CMD_CALL_PROPAGATE_REPL are not set, then respectively AOF or
 * slaves propagation will never occur.
 *
 * Client flags are modified by the implementation of a given command
 * using the following API:
 *
 * forceCommandPropagation(client *c, int flags);
 * preventCommandPropagation(client *c);
 * preventCommandAOF(client *c);
 * preventCommandReplication(client *c);
 *
 */
void call(client *c, int flags);
```

``` cpp
/* Try to expire a few timed out keys. The algorithm used is adaptive and
 * will use few CPU cycles if there are few expiring keys, otherwise
 * it will get more aggressive to avoid that too much memory is used by
 * keys that can be removed from the keyspace.
 *
 * No more than CRON_DBS_PER_CALL databases are tested at every
 * iteration.
 *
 * This kind of call is used when Redis detects that timelimit_exit is
 * true, so there is more work to do, and we do it more incrementally from
 * the beforeSleep() function of the event loop.
 *
 * Expire cycle type:
 *
 * If type is ACTIVE_EXPIRE_CYCLE_FAST the function will try to run a
 * "fast" expire cycle that takes no longer than EXPIRE_FAST_CYCLE_DURATION
 * microseconds, and is not repeated again before the same amount of time.
 *
 * If type is ACTIVE_EXPIRE_CYCLE_SLOW, that normal expire cycle is
 * executed, where the time limit is a percentage of the REDIS_HZ period
 * as specified by the ACTIVE_EXPIRE_CYCLE_SLOW_TIME_PERC define. */

void activeExpireCycle(int type);
```

``` cpp
/* This function is periodically called to see if there is memory to free
 * according to the current "maxmemory" settings. In case we are over the
 * memory limit, the function will try to free some memory to return back
 * under the limit.
 *
 * The function returns C_OK if we are under the memory limit or if we
 * were over the limit, but the attempt to free memory was successful.
 * Otehrwise if we are over the memory limit, but not enough memory
 * was freed to return back under the limit, the function returns C_ERR. */
int freeMemoryIfNeeded(void);
```

## src/networking.c

此文件定义了clients, masters和replicas涉及的网络I/O函数。

* `createClient()` allocates and initializes a new client.
* the `addReply*()` family of functions are used by commands implementations in order to append data to the client structure, that will be transmitted to the client as a reply for a given command executed.
* `writeToClient()` transmits the data pending in the output buffers to the client and is called by the *writable event handler* `sendReplyToClient()`.
* `readQueryFromClient()` is the *readable event handler* and accumulates data from read from the client into the query buffer.
* `processInputBuffer()` is the entry point in order to parse the client query buffer according to the Redis protocol. Once commands are ready to be processed, it calls `processCommand()` which is defined inside `server.c` in order to actually execute the command.
* `freeClient()` deallocates, disconnects and removes a client.

## src/aof.c src/rdb.c

RDB和AOF的持久化(persistence)实现。

Redis uses a persistence model based on the fork() system call in order to create a thread with the same (shared) memory content of the main Redis thread. This secondary thread dumps the content of the memory on disk. This is used by rdb.c to create the snapshots on disk and by aof.c in order to perform the AOF rewrite when the append only file gets too big.


## src/db.c

一些通用的命令实现。

* `lookupKeyRead()` and `lookupKeyWrite()` are used in order to get a pointer to the value associated to a given key, or `NULL` if the key does not exist.
* `dbAdd()` and its higher level counterpart `setKey()` create a new key in a Redis database.
* `dbDelete()` removes a key and its associated value.
* `emptyDb()` removes an entire single database or all the databases defined.

## src/object.c

对Redis对象`robj`的操作接口。

* `incrRefcount()` and `decrRefCount()` are used in order to increment or decrement an object reference count. When it drops to 0 the object is finally freed.
* `createObject()` allocates a new object. There are also specialized functions to allocate string objects having a specific content, like `createStringObjectFromLongLong()` and similar functions.

## src/replication.c

关于数据同步的实现。

One of the most important functions inside this file is `replicationFeedSlaves()` that writes commands to the clients representing replica instances connected
to our master, so that the replicas can get the writes performed by the clients:
this way their data set will remain synchronized with the one in the master.


## Other C files


* `t_hash.c`, `t_list.c`, `t_set.c`, `t_string.c` and `t_zset.c` contains the implementation of the Redis data types. They implement both an API to access a given data type, and the client commands implementations for these data types.
* `ae.c` implements the Redis event loop, it's a self contained library which is simple to read and understand.
* `sds.c` is the Redis string library, check http://github.com/antirez/sds for more information.
* `anet.c` is a library to use POSIX networking in a simpler way compared to the raw interface exposed by the kernel.
* `dict.c` is an implementation of a non-blocking hash table which rehashes incrementally.
* `scripting.c` implements Lua scripting. It is completely self contained from the rest of the Redis implementation and is simple enough to understand if you are familar with the Lua API.
* `cluster.c` implements the Redis Cluster. Probably a good read only after being very familiar with the rest of the Redis code base. If you want to read `cluster.c` make sure to read the [Redis Cluster specification][3].

[3]: http://redis.io/topics/cluster-spec


# 数据类型

首先需要了解[Redis Data Types]有哪些。

Redis提供的5种数据结构

| 结构类型 | 结构存储的值 | 结构的读写能力
| STRING | 可以是字符串，整数，浮点数 | 可以对整个字符串或者其中一部分执行操作；对整数和浮点数执行自增(increment)或者自减(decrement)
| LIST | 一个链表，链表上的每个节点都包含了一个字符串 | 从链表的两端推入或者弹出元素；根据偏移量对链表进行修剪(trim)；读取单个或者多个元素；根据值查找或者移除元素
| SET | 包含字符串的无序收集器(unordered collection)，并且被包含的每个字符串都是独一无二，各不相同 | 添加，获取，移除单个元素；检查一个元素是否存在于集合中；计算交集，并集，差集；从集合里面随机获取元素
| HASH | 包含键值对的无序散列表 | 添加，获取，删除单个键值对；获取所有键值对
| ZSET | 字符串成员与浮点数分值之间的有序映射，元素的排列顺序由分值的大小决定 | 添加，获取，删除单个元素；根据分值范围或者成员来获取元素

## STRING

## ...


# 执行命令

进一步了解[Redis Command]有哪些命令。

Redis的所有命令定义：

``` cpp
void foobarCommand(client *c) {
    printf("%s",c->argv[1]->ptr); /* Do something with the argument. */
    addReply(c,shared.ok); /* Reply something to the client. */
}
```

在`server.c`中定义了command table：

``` cpp
{"foobar",foobarCommand,2,"rtF",0,NULL,0,0,0,0,0},
```

含义：
* 2：表示此命令的参数个数
* rtF：表示命令的flag
* 命令执行结果通过`addReply()`函数返回，在`networking.c`定义

> 可以在Redis中扩展和定义自己的命令。



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