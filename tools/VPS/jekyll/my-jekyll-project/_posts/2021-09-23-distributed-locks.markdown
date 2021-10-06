---
layout: post
title:  "Distributed Locks"
date:   2021-09-23 12:00:00 +0800
categories: 分布式
---

* Do not remove this line (it will not be displayed)
{:toc}


# 应用场景

应用开发时，如果需要在同进程内的不同线程并发访问某项资源，可以使用各种互斥锁、读写锁。

* 如果一台主机上的多个进程需要并发访问某项资源，则可以使用进程间同步的原语，例如信号量、管道、共享内存等。
* 如果多台主机需要同时访问某项资源，就需要使用一种在全局可见并具有互斥性的锁了。这种锁就是分布式锁，可以在分布式场景中对资源加锁，避免竞争资源引起的逻辑错误。

> 分布式锁，是控制分布式系统之间同步访问共享资源的一种方式。

![distributed-locks](/assets/images/202109/distributed-locks.png)

# 分布式锁的特性

* 互斥性。在任意时刻，只有一个客户端持有锁。
* 不死锁。分布式锁本质上是一个基于租约（Lease）的租借锁，如果客户端获得锁后自身出现异常，锁能够在一段时间后自动释放，资源不会被锁死。
* 一致性。硬件故障或网络异常等外部问题，以及慢查询、自身缺陷等内部因素都可能导致，提供分布式锁服务的系统发生高可用切换，replica提升为新的master。此时，如果业务对互斥性的要求非常高，锁需要在切换到新的master后保持原状态。

# 乐观锁与悲观锁

## Optimistic Lock

Instead of blocking something potentially dangerous happens, we continue anyway, in the hope that everything will be ok. To use optimistic lock we usually use **a version field on the database record** we have to handle, and when we update it we check if the data we read has the same version of the data we are writing.

![optimistic-lock-sequence-diagram](/assets/images/202109/optimistic-lock-sequence-diagram.png)


## Pessimistic Lock

Block access to the resource before operating on it, and we release the lock at the end. The pessimistic lock instead will rely on an external system that will hold the lock for our microservices.

![pessimistic-lock-sequence-diagram](/assets/images/202109/pessimistic-lock-sequence-diagram.png)

问题：悲观锁死锁 -> 引入锁超时时间 -> 可能多个人同时释放锁，如何保证正确性

Implementing a pessimistic lock we have a big issue, what happened if the lock owner doesn’t release it? If the lock owner dies? The lock will be held forever and we could be in a deadlock. To prevent this issue we will set an expiration time on the lock, so the lock will be auto-released.

But if the time expires before the task handled by the owner isn’t yet finished, another microservice can acquire the lock, and both lock holders can now release the lock causing inconsistency. Remember, no timer assumption can be reliable in asynchronous networks.

![fencing-token-sequence-diagram](/assets/images/202109/fencing-token-sequence-diagram.png)


解决方法：fencing token

We need to use a fencing token which is incremented each time a microservice acquires a lock. This token must be passed to the lock manager when we release the lock, so if the first owner releases the lock before the second owner, the system will refuse the second lock release. Depending on implementation we can also decide to let win the second lock owner.

# 分布式锁方案

## 数据库锁

使用`唯一key`作为数据库表的主键或索引，在访问共享资源时，通过下面的方法来保证互斥。

* 优点：实现简单。
* 缺点：不会自动释放锁，性能受数据库限制。

``` cpp
// 获取锁
int lock() {
    return sql.exec("insert into t_dlm(f_oid, f_remark) values('1', 'task1')");
}

// 释放锁
int unlock() {
    return sql.exec("delete from t_dlm where f_oid='1'");
}

// 超时清理
int timeout_op() {
    ttl = "-00:05:00";
    return sql.exec("delete from t_dlm where f_update_time < ADDTIME(NOW(), ttl)");
}
```

## ZooKeeper

客户端一起竞争写某条数据，例如，`/path/lock`。只有第一个客户端能写入成功，其他的客户端都会写入失败。写入成功的客户端就获得了锁，写入失败的客户端，注册`watch`事件（阻塞锁），等待锁的释放，从而继续竞争该锁。

* 优点：技术成熟，很多大公司（eBay, Yahoo等）在用，周边工具完善。
* 缺点：复杂度，系统维护。



## Redis (SETNX/SET/DEL)

> refer:
> 
> 1. 官方介绍 http://redis.io/topics/distlock
> 
> 2. 其他观点 https://martin.kleppmann.com/2016/02/08/how-to-do-distributed-locking.html

`SETNX`(SET if Not eXists)或者`SET`(EX/PX/NX/XX，Redis 2.6.12)和`DEL`，实现分布式锁。

* 优点：简单的锁定原语，并发能力强。
* 缺点：存在单点（单实例），锁数据可能丢失（Master和Slave是异步复制），可用性不高。

### 一致性问题

Redis的主从同步（replication）是异步进行的，如果向master发送请求修改了数据后master突然出现异常，发生高可用切换，缓冲区的数据可能无法同步到新的master（原replica）上，导致数据不一致。如果丢失的数据跟分布式锁有关，则会导致锁的机制出现问题，从而引起业务异常。

为了提高可用性，Redis作者提出了[Redlock算法](http://redis.io/topics/distlock)。例如，当存在5个独立的Redis节点时，可以允许在最多挂掉2个节点的情况下，分布式锁服务仍然可以正常工作。

红锁的本质是一个概率问题：如果一个主从架构的Redis在高可用切换期间丢失锁的概率是`k%`，那么相互独立的N个Redis同时丢失锁的概率是多少？如果用红锁来实现分布式锁，那么丢锁的概率是`(k%)^N`（即，Redis节点越多则一致性越强）。鉴于Redis极高的稳定性，此时的概率已经完全能满足产品的需求。

> 说明：红锁的实现并非这样严格，一般保证 M(1 < M =< N)个同时锁上即可，但通常仍旧可以满足需求。

红锁的问题在于：

1. 加锁和解锁的延迟较大。
2. 难以在集群版或者标准版（主从架构）的Redis实例中实现。
3. 占用的资源过多，为了实现红锁，需要创建多个互不相关的云Redis实例或者自建Redis。


### 加锁

为`resource_1`这个key设置了`5秒`的过期时间，如果客户端不释放这个key，5秒后key将过期，锁就会被系统回收，此时其它客户端就能够再次为资源加锁并访问资源了。

> SET resource_1 random_value NX EX 5

* resource_1: 分布式锁的key，只要这个key存在，相应的资源就处于加锁状态，无法被其它客户端访问。
* random_value: 一个随机字符串，不同客户端设置的值不能相同。
* EX: 设置过期时间，单位为秒。也可以使用PX选项设置单位为毫秒的过期时间。
* NX: 如果需要设置的key在Redis中已存在，则取消设置。

### 解锁

异常流程：

1. `t1`时刻，App1设置了分布式锁`resource_1`，过期时间为`3秒`。
2. App1由于程序慢等原因等待超过了`3秒`，而`resource_1`已经在`t2`时刻被释放。
3. `t3`时刻，App2获得这个分布式锁。
4. App1从等待中恢复，在`t4`时刻运行`DEL resource_1`将App2持有的分布式锁释放了。

![distributed-locks-del](/assets/images/202109/distributed-locks-del.png)

因此，**一个客户端设置的锁，必须由自己解开**。客户端需要先使用`GET`命令确认锁是不是自己设置的，然后再使用`DEL`解锁。在Redis中通常需要用Lua脚本来实现自锁自解：

``` lua
if redis.call("get", KEYS[1]) == ARGV[1] then
    return redis.call("del", KEYS[1])
else
    return 0
end
```

### 续租

当客户端发现在锁的租期内无法完成操作时，就需要延长锁的持有时间，进行续租（renew）。同解锁一样，客户端应该只能续租自己持有的锁。在Redis中可使用如下Lua脚本来实现续租：

``` lua
if redis.call("get", KEYS[1]) == ARGV[1] then
    return redis.call("expire", KEYS[1], ARGV[2])
else
    return 0
end
```

## ETCD

etcd使用`Raft`算法保持了数据的强一致性，从而可以用于实现分布式锁。由于Raft算法在做决策时需要多数节点的投票，所以etcd一般部署集群推荐奇数个节点，推荐的数量为3、5或者7个节点构成一个集群。

etcd提供了一套实现分布式锁原子操作`CAS`（CompareAndSwap）的API。通过设置`prevExist`值，可以保证在多个节点同时去创建某个目录时，只有一个成功。而创建成功的用户就可以认为是获得了锁。所有想要获得锁的用户都会被安排执行，但是获得锁的顺序也是全局唯一的，同时决定了执行顺序。

* 优点：
  + 使用Go语言编写部署简单。
  + 使用HTTP作为接口使用简单。
  + 使用Raft算法保证强一致性易于理解。
  + 数据持久化。
  + 支持SSL客户端安全认证。


# Refer

* [Everything I know about distributed locks](https://davidecerbo.medium.com/everything-i-know-about-distributed-locks-2bf54de2df71)
* [Distributed Locking with Redis](https://carlosbecker.com/posts/distributed-locks-redis)
* [Distributed Locks are Dead; Long Live Distributed Locks!](https://hazelcast.com/blog/long-live-distributed-locks/)	
* [How to do distributed locking](https://martin.kleppmann.com/2016/02/08/how-to-do-distributed-locking.html)
