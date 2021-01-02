---
layout: post
title:  "事务在互联网场景中的应用"
date:   2018-08-18 14:08:00 +0800
categories: 分布式
---

* Do not remove this line (it will not be displayed)
{:toc}

在支付场景下一般需要对提供的服务保证事务能力。比如，先付钱后发货，付钱和发货可以看做一个完整的事务。在一些更复杂的场景，可能涉及数据库，消息服务，RPC服务等多个资源的操作。事务的核心是`锁`，在`并发`的场景下如何协调好不同的资源。同时，事务(锁)和性能是两个相悖的特性，因此在不影响业务的前提下，通常对事务的**调优原则**包括。

1. 尽可能减少锁的覆盖范围。例如，MyISAM表锁到Innodb的行锁。
2. 增加锁上可并行的线程数。例如，读锁和写锁分离，允许并行读取数据。
3. 选择正确的锁类型。例如，悲观锁适合并发争抢比较严重的场景，乐观锁适合并发争抢不太严重的场景。

关于MySQL的锁可以参考何登成的这篇文章[MySQL加锁处理分析](http://blog.sae.sina.com.cn/archives/2127)。

# 背景

关于事务的概念：

> A `transaction`(事务) is a logical unit of work which effects can either be made permanent in their entirety (committed) or cancelled in their entirety (rolled back). Before a transaction is committed or rolled back, it is active. Active transactions' effects are typically invisible to other, concurrent transactions. Consequently, only committed transactions' effects are visible.

一般，本地事务可以通过数据库的本地事务(ACID)来保证。当事务A在操作数据且commit或rollback前，其他事务都不能看到事务A对数据的修改。

![acid_lock_time](/assets/images/201808/acid_lock_time.png)

内部是通过**Two-Phase Commit**来保证的。2PL就是将加锁/解锁分为两个完全不相交的阶段。加锁阶段：只加锁，不放锁。解锁阶段：只放锁，不加锁。

```
Two-phase commit works in two phases: `a voting phase` and `a decision phase`.

* In the voting(or prepare) phase, the transaction manager will ask both the RM1 and RM2 whether they can agree with a successful termination or not. Each may return a negative reply, for instance if there was a time-out which caused the RM1's work to be rolled back. If one of them replies positively, then it should make sure it can always make the work permanent.

* After the transaction manager has received all of the replies (also called 'votes') it will make a global decision on the outcome of the transaction. This decision will depend on the collected replies:

	- If both replies were positive (meaning that both the RM1 and RM2 can make the work permanent), then the transaction manager will instruct each to commit.

	- If at least one reply is negative (or missing) then a rollback decision is sent to the remaining resource. This means that the remaining resource cancels (rolls back) the work done for the transaction.
```

但是，当出现**多个不同的本地事务(分布式事务)**时，如何保证一致性？

例如，小Q想去国外旅游，需要在两个不同的网站上预定机票。在A网站预定从北京到深圳的机票，再在B网站预定深圳到国外的机票。如果小Q在预定B网站机票时失败了，就必须取消已经在A网站预定的机票。如果没有及时取消就会发生：

1. 小Q需承担在A网站预定的机票钱。
2. 在A网站预定机票的座位没有及时售卖出去，航空公司承担经济损失。

> PS: 在实际场景中，如果用户没有及时退票(cancel)，通常会根据退票的时间需要承担相应的罚金(penalties)。就像在12306上购买火车票，如果退票在开车时间前15天，不收取退票费，48小时以上的按票价5%计，24小时以上且不足48小时按票价10%计，不足24小时的按票价20%计。

是否有一种方法可以保证小Q在预定机票时类似像本地事务(ACID)提交一样，就可以避免或减轻上述问题。但是还需要具备：

1. 网站A在预定机票成功后，具备实时退票的能力，而不是通过人工介入。
2. 和数据库不同，多个网站(资源方)互不信任，存在DOS攻击的风险。

在实际场景下，上面两个条件一般都不能满足(对比本地数据库)。是否还有其他方法？

可以考虑`compensation`(补偿)的方法。把每个操作都作为一个短的本地ACID事务，减少锁的时间。代价是牺牲了rollback的能力。而补偿(取消操作)需要根据具体的业务场景来实现。

![compensation_lock_time](/assets/images/201808/compensation_lock_time.png)

通常，有两种不同的补偿方式：

1. **Perfect compensation**，把之前的操作全部`clean up`
2. **Imperfect compensation**，将之前的操作状态修改为`canceled`

对应实际的应用场景，比如：

1. 买股票是一个事务，卖股票可以作为一种补偿事务，两个事务分别是独立的。
2. 以定机票为例，先在A网站发起预定机票的请求，将当前的订单状态设置为`PENDING`状态，此时机票只是暂时被锁定，但是并没有出票成功。需要接收到第二阶段的`CONFIRM`或者`CANCEL`**补偿请求**后，此预定机票的事务才结束。

![TCC](/assets/images/201808/TCC.png)

可以看出，有两种常见的补偿方式，一种是无状态的，比如买卖股票的场景。一种是有状态的，采用类似`two-phase protocol`的处理方式。

进一步思考：

1. 如果补偿操作失败了，怎么办？
2. 如果出现了通信失败，或者超时错误，如何防止重复请求？

考虑到事务的完整性，如果补偿操作失败了，一般借助消息服务进行重试(At-Least-Once Message)，但是可能会出现重复请求，因此需要后端RM的接口根据当前请求的ID做到幂等(idempotence)。

# 实践

在实际编程中实现事务有哪些方法？

## JTA

[Java Transaction API (JTA)]提供了一套事务接口规范。

```
`JTA` is short for Sun Microsystems' Java Transaction API and is Sun's (low-level) API for creating transactions in Java and making your data access operations part of those transactions.

The JTA defines how your application can request transactional functionality on the Java platform. **JTA is not a product in itself, but rather a set of Java interfaces.** A vendor-specific JTA implementation referred to as a transaction manager or transaction service (such as Transactions™) is needed to actually use the functionality
defined in these interfaces. In other words, you can program JTA transactions in your application, but you need the implementation classes of a JTA-compliant transaction manager vendor in order to run your application.
```

## 数据库事务

MySQL服务器逻辑架构从上往下可以分为三层：

* 第一层：处理客户端连接、授权认证等。
* 第二层：服务器层，负责查询语句的解析、优化、缓存以及内置函数的实现、存储过程等。
* 第三层：存储引擎，负责MySQL中数据的存储和提取。**MySQL中服务器层不管理事务，事务是由存储引擎实现的**。MySQL支持事务的存储引擎有InnoDB、NDB Cluster等，其中InnoDB的使用最为广泛；其他存储引擎不支持事务，如MyIsam、Memory等。

![mysql_arch](/assets/images/201808/mysql_arch.png)

MySQL中默认采用的是自动提交（`autocommit`）模式，在自动提交模式下，如果没有`start transaction`显式地开始一个事务，那么每个sql语句都会被当做一个事务执行提交操作。通过`set autocommit = 0;`可以关闭autocommit，需要注意的是，autocommit参数是`针对连接的`，在一个连接中修改了参数，不会对其他连接产生影响。如果关闭了autocommit，则所有的sql语句都在一个事务中，直到执行了commit或rollback，该事务结束，同时开始了另外一个事务。

```
mysql> show variables like 'autocommit';
+---------------+-------+
| Variable_name | Value |
+---------------+-------+
| autocommit    | ON    |
+---------------+-------+
1 row in set (0.00 sec)
mysql> set autocommit = 0;
Query OK, 0 rows affected (0.00 sec)

mysql> show variables like 'autocommit';
+---------------+-------+
| Variable_name | Value |
+---------------+-------+
| autocommit    | OFF   |
+---------------+-------+
1 row in set (0.00 sec)
```

也可以用`START TRANSACTION`语句开始一个事务，然后要么使用`COMMIT`提交事务将修改的数据持久保留，要么使用`ROLLBACK`撤销所有的修改。

{% highlight sql %}
START TRANSACTION;
SELECT balance FROM checking WHERE customer_id = 'gerry';
UPDATE checking SET balance = balance - 200.00 WHERE customer_id = 'gerry';
UPDATE savings SET balance = balance + 200.00 WHERE customer_id = 'gerry';
COMMIT;
{% endhighlight %}

### ACID

`ACID`表示原子性，一致性，隔离性和持久性。一个运行良好的事务处理系统，必须具备这些标准特征。事务的ACID特性可以确保银行不会弄丢你的钱。

```
1. 原子性（atomicity） 
对于一个事务来说，不可能只执行其中一部分操作，这就是事务的原子性。
2. 一致性（consistency） 
数据库总是从一个一致性的状态转换到另外一个一致性的状态。
3. 隔离性（isolation） 
通常来说（根据不同的隔离级别），一个事务所做的修改在最终提交以前，对其他事务是不可见的。
4. 持久性（durability） 
一旦事务提交，则其所做的修改就会永久保存在数据库中。
```

### MVCC：Snapshot Read vs Current Read

MySQL InnoDB存储引擎，实现的是基于多版本的并发控制协议`MVCC` (`Multi-Version Concurrency Control`) (注：与MVCC相对的，是基于锁的并发控制，Lock-Based Concurrency Control)。MVCC最大的好处，相信也是耳熟能详：读不加锁，读写不冲突。在读多写少的OLTP应用中，读写不冲突是非常重要的，极大的增加了系统的并发性能，这也是为什么现阶段，几乎所有的RDBMS，都支持了MVCC。

在MVCC并发控制中，读操作可以分成两类：`快照读` (snapshot read)与`当前读` (current read)。快照读，读取的是记录的可见版本 (有可能是历史版本)，不用加锁。当前读，读取的是记录的最新版本，并且，当前读返回的记录，都会加上锁，保证其他事务不会再并发修改这条记录。在一个支持MVCC并发控制的系统中，哪些读操作是快照读？哪些操作又是当前读呢？以MySQL InnoDB为例：

 
`快照读`：简单的select操作，属于快照读，不加锁。(当然，也有例外)
```
select * from table where ?;
 ```

`当前读`：特殊的读操作，插入/更新/删除操作，属于当前读，需要加锁。
```
select * from table where ? lock in share mode;
select * from table where ? for update;
insert into table values (…);
update table set ? where ?;
delete from table where ?;
```
所有以上的语句，都属于当前读，读取记录的最新版本。并且，读取之后，还需要保证其他并发事务不能修改当前记录，对读取记录加锁。其中，除了第一条语句，对读取记录加`S锁` (共享锁)外，其他的操作，都加的是`X锁` (排它锁)。

### Cluster Index：聚簇索引

InnoDB存储引擎的数据组织方式，是`聚簇索引表`：**完整的记录，存储在主键索引中，通过主键索引，就可以获取记录所有的列**。关于聚簇索引表的组织方式，可以参考MySQL的官方文档：Clustered and Secondary Indexes 。

### 2PL：Two-Phase Locking

传统RDBMS加锁的一个原则，就是`2PL `(二阶段锁)：Two-Phase Locking。相对而言，2PL比较容易理解，说的是锁操作分为两个阶段：`加锁阶段`与`解锁阶段`，并且保证加锁阶段与解锁阶段不相交。

### 隔离级别（Isolation Level）

https://stackoverflow.com/tags/innodb/info


在SQL标准中定义了`四种`隔离级别。每一种级别都规定了一个事务中所做的修改，哪些在事务内和事务间是可见的，哪些是不可见的。较低级别的隔离通常可以执行更高的并发，系统的开销也更低。每种存储引擎实现的隔离级别不尽相同，请查阅具体相关手册。

1. `READ UNCOMMITTED`（未提交读） 
在此级别，事务中的修改即使没有提交，对其他事务也都是可见的，即，“赃读”。（实际应用中，很少使用）

2. `READ COMMITTED`（提交读，不可重复读） 
大多数数据的默认隔离级别都是此级别（Oracle是，但MySQL不是）。此级别满足隔离性的简单定义：一个事务开始时，只能看见已经提交的事务所做的修改（或者，一个事务从开始直到提交之前，所做的任何修改对其他事务都是不可见的）。此级别，也称为“不可重复读”，因为两次执行同样的查询，可能会得到不一样的结果。此级别可以解决：`脏读`。

3. `REPEATABLE READ`（可重复读） 
此级别保证了在同一个事务中，多次读取同样纪录的结果是一致的。此级别，是MySQL的默认事务隔离级别。此级别可以解决：`脏读`和`不可重复读`。

4. `SERIALIZABLE`（可串行化） 
此级别是最高的隔离级别。它通过强制事务串行执行，避免了“幻读”的问题。此级别，会在读取的每一行数据上都加锁，所以可能导致大量的超时和锁争用的问题。（实际应用中也很少使用这个隔离级别，只有在非常需要确保数据的一致性而且可以接受没有并发的情况下，才考虑使用此级别）。此级别可以解决：`脏读`，`不可重复读`和`幻读`。

| 隔离级别 | 功能
| -- | --
| RU | 可以看到未提交的数据（脏读），举个例子：别人说的话你都相信了，但是可能他只是说说，并不实际做。
| RC | 读取提交的数据。但是，可能多次读取的数据结果不一致（不可重复读）。用读写的观点就是：读取的行数据，可以写。
| RR | 可以重复读取，但有幻读。读写观点：读取（当前读）的数据行不可写，但是可以往表中新增数据。在MySQL中，其他事务新增的数据，看不到，不会产生幻读。采用多版本并发控制（MVCC）机制解决幻读问题。
| SERIALIZABLE | 可读，不可写。写数据必须等待另一个事务结束。


MySQL/InnoDB定义的4种隔离级别：

| MySQL/InnoDB 隔离级别 | 功能
| -- | --
| Read Uncommited | 可以读取未提交记录。此隔离级别，不会使用，忽略。
| Read Committed (RC) | 快照读忽略，本文不考虑。针对当前读，RC隔离级别保证对读取到的记录加锁 (记录锁)，存在幻读现象。
| Repeatable Read (RR) | 快照读忽略，本文不考虑。针对当前读，RR隔离级别保证对读取到的记录加锁 (记录锁)，同时保证对读取的范围加锁，新的满足查询条件的记录不能够插入 (间隙锁)，不存在幻读现象。
| Serializable | 从MVCC并发控制退化为基于锁的并发控制。不区别快照读与当前读，所有的读操作均为当前读，读加读锁 (S锁)，写加写锁 (X锁)。Serializable隔离级别下，读写冲突，因此并发度急剧下降，在MySQL/InnoDB下不建议使用。

> 所谓幻读，就是同一个事务，连续做两次当前读 (例如：select * from t1 where id = 10 for update;)，那么这两次当前读返回的是完全相同的记录 (记录数量一致，记录本身也一致)，第二次的当前读，不会比第一次返回更多的记录 (幻象)。如何保证两次当前读返回一致的记录，那就需要在第一次当前读与第二次当前读之间，其他的事务不会插入新的满足条件的记录并提交。为了实现这个功能，GAP锁应运而生。GAP锁的目的，是为了防止同一事务的两次当前读，出现幻读的情况。

MySQL中查看和设置隔离级别：

```
-- 查看当前会话隔离级别
SELECT @@tx_isolation;

-- 查看系统当前隔离级别
select @@global.tx_isolation;

-- 设置当前会话隔离级别
set session transaction isolation level repeatable read | serializable | ...;

-- 设置系统当前隔离级别
set global transaction isolation level repeatable read | serializable | ...;

-- 开始事务
set autocommit=off 或者 start transaction
```

例子： 

```
START TRANSACTION;
事务A：在整个执行阶段，会将某数据项的值从1开始，加1操作，直到变成10之后进行事务提交。 
事务B：查看此数据项的值，请问在不同的隔离级别下看到的值是多少？ 
事务C：执行和事务A类似的操作，将此数据项从10累加到20，然后进行提交。
COMMIT;
```
![mysql_trans_example](/assets/images/201808/mysql_trans_example.png)

[MySQL读书笔记－事务，隔离级别，死锁](https://blog.csdn.net/delphiwcdj/article/details/51874401)


关于幻读的一个测试例子：

假设存在表：

```
create table t_gerry(id int(2)) engine=innodb default charset=utf8;
insert into t_gerry(id) values(1);
insert into t_gerry(id) values(2);
```

事务1和事务2，执行顺序如下：

```
mysql> select * from t_gerry;
+------+
| id   |
+------+
|    1 |
|    2 |
+------+
2 rows in set (0.00 sec)

mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> select * from t_gerry;
+------+
| id   |
+------+
|    1 |
|    2 |
+------+
2 rows in set (0.00 sec)

mysql> select * from t_gerry;
+------+
| id   |
+------+
|    1 |
|    2 |
+------+
2 rows in set (0.00 sec)

// 在此中间，事务2，执行 insert into t_gerry(id) values(3); 可以执行成功，因为事务1不会对记录加X锁和GAP锁。


mysql> update t_gerry set id = id + 1;
Query OK, 3 rows affected (0.00 sec)
Rows matched: 3  Changed: 3  Warnings: 0   // 这里出现了幻读，影响了3条记录

// 而此时，事务2，再次执行 insert into t_gerry(id) values(3); 执行后会阻塞，因为事务1对每条记录加了X锁，以及记录之间的GAP锁，防止出现幻读
// 事务2可能阻塞并返回超时错误：ERROR 1205 (HY000): Lock wait timeout exceeded; try restarting transaction

mysql> commit;
Query OK, 0 rows affected (0.00 sec)

mysql> select * from t_gerry;
+------+
| id   |
+------+
|    2 |
|    3 |
|    4 |
+------+
3 rows in set (0.00 sec)
```



## X/Open XA

[X/Open XA]，`XA`是e**X**tended **A**rchitecture的简称，提出了一种**Distributed Transaction Processing (DTP)**处理分布式事务的模型。采用[Two-phase commit protocol]来保证，对多种资源(databases, application servers, message queues, transactional caches, etc.)操作时，具备数据库ACID的事务能力。

![DTP](/assets/images/201808/DTP.png)

```
**DTP**（Distributed Transaction Processing）systems are those where work in support of a
single transaction may occur across RMs. This has several implications:
* The system must have a way to refer to a transaction that encompasses all work
done anywhere in the system. （知道每个子事务的状态）
* The decision to commit or roll back a transaction must consider the status of work
done anywhere on behalf of the transaction. The decision must have uniform effect
throughout the DTP system.  （根据每个子事务的状态来决定commit还是rollback）
```

## MySQL XA

MySQL 5.0或更新版本的数据库已经开始支持XA事务。存储引擎的事务特性能够保证在存储引擎级别实现ACID。而XA分布式事务，可以让ACID扩展到多个数据库。实际上，在MySQL中有两种XA事务。

1. 外部XA事务。跨多MySQL实例的分布式事务，需要应用层介入作为协调者。
2. 内部XA事务。同一MySQL实例下跨多个引擎的事务，例如，协调存储引擎和二进制日志。详见High Performance MySQL 7.11.1章节介绍。

注意：

1. XA事务是一种在多个服务器之间同步数据的方法。如果由于某些原因不能使用MySQL本身的复制，或者性能并不是瓶颈的时候，可以尝试使用。
2. 因为涉及网络通信延迟，外部XA事务比内部XA事务消耗更大，不稳定的网络通信或者用户长时间的等待而不提交，会导致所有参与者都在等待。因此，建议优先考虑其他更高性能的方案。
3. `innodb_support_xa` 选项：控制MySQL内部存储引擎和二进制日志之间的分布式事务。

MySQL XA**事务状态**变化：(详见[XA Transaction States])

1. 使用`XA START`启动一个XA事务，并把它置为**ACTIVE状态**。
2. 对一个ACTIVE XA事务，发布构成事务的SQL语句，然后发布一个`XA END`语句，`XA END`把事务置为**IDLE状态**。
3. 对一个IDLE XA事务， 发布一个`XA PREPARE`语句。把事务置为**PREPARE状态**，此时XA RECOVER 语句的输出包含事务的xid值(XA RECOVER语句会列出所有处于PREPARE状态的XA事务)。 
4. 对一个PREPARE XA事务，可以发布一个`XA COMMIT`语句来提交和终止事务，或者发布一个`XA ROLLBACK`来回滚并终止事务。

下面是`XA Transaction SQL Syntax`的一些用法，其中`xid`，即全局唯一事务ID通常是由`Transaction Manager`生成。

{% highlight sql %}
XA START | BEGIN xid;
XA END xid;
XA PREPARE xid;
XA COMMIT xid;
XA ROLLBACK xid;
XA RECOVER;         -- 显示PREPARED状态的事务
{% endhighlight %}

使用MySQL XA的一个例子：

{% highlight sql %}
xa start xid;  -- for db1
update db1.t_user_balance set balance = balance - 1 where user = 'Bob' and balance > 1;
xa start xid;  -- for db2
update db2.t_user_balance set balance = balance + 1 where user = 'John';
xa prepare xid; -- for db1
xa prepare xid; -- for db2

do_other_something(); -- db连接可以断开，此时可以做一些其他事情（比如rpc操作），然后再提交db事务

-- 如果do_other_something成功，可以提交之前的db事务
xa commit xid; -- for db1
xa commit xid; -- for db2

-- 如果do_other_something失败，需要回滚之前的db事务
xa rollback xid; -- for db1
xa rollback xid; -- for db2
{% endhighlight %}

思考1: MySQL默认是RR隔离级别，分布式事务场景下, 是否需要设置成串行化隔离级别？

如果数据库不支持分布式MVCC，就有必要。否则可能RM1的本地事务提交了，RM2还没提交，这样外部就会读取到RM1已经提交的结果和RM2上未提交的结果，破坏了隔离性。

{% highlight sql %}
set session transaction isolation level SERIALIZABLE;
{% endhighlight %}

思考2：分布式事务的热点数据并发性能最高就是趋近于单机本地事务。所以无论是基于XA协议实现的分布式事务，还是单机本地事务，都是存在热点数据并发性能极限的。如何解决热点数据问题？

1. 排队。把操作热点数据的请求排队，比如用单线程处理，避免版本号冲突或加锁问题。
2. 异步。就是引入多个中间账户，扣费的时候操作中间账户，然后再异步把中间账户的数据同步到热点账户。

## TCC

`Tentative Operation`也称为`Try-Confirm-Cancel`，最早由[Atomikos]提出。 

Pat helland在2007年也发表了一篇相同观点的文章[Life beyond Distributed Transactions: an Apostate’s Opinion]，考虑在无限扩展的应用场景下，业务层不应该关心底层扩展所带来的问题，应该由统一的平台或者框架来屏蔽底层扩展所带来的差异。并提出了一种`Performing Tentative(不确定的) Business Operations`处理流程(**workflow**)，即`TCC流程`(**Tentative Operations, Confirmation, and Cancellation**)，来减少分布式场景可能导致的不一致(**uncertainty**)问题。

![2007_scale_agnostic](/assets/images/201808/2007_scale_agnostic.png)

``` 
To reach an agreement across entities, one entity has to ask another to accept some uncertainty. This is done by sending a message which requests a commitment but leaves open the possibility of cancellation. This is called a tentative operation and it represented by a message
flowing between two entities. At the end of this step, one of the entities agrees to abide by the wishes of the other.
```

[Tentative Operation]进一步对`TCC`的定义进行了说明。对`How can a requestor ensure a consistent outcome across multiple, independent providers?`的问题进行了讨论。

```
* The communication channels used by loosely coupled, distributed systems usually do not provide transactional semantics at the transport level. Instead, participants can send and receive message, often reliably.
* While some operations are inherently reversible, e.g. debiting a bank account after a credit, other operations, such as shipping a package or scrapping a car cannot easily be undone. As a result, the requestor might not be able to do much to rectify the situation.
* Distributed conversations typically involve uncertainty: a participant cannot be certain that a conversation partner continues in the conversation or even exist after the last interaction. Participants should therefore allocate resources cautiously.
```

![requestor_provider](/assets/images/201808/requestor_provider.png)

`Tentative Operation`有两种处理模型：

1. 显式的`cancellation`操作，例如，直到收到取消操作。
2. 隐式的`cancellation`操作，例如，超时则自动取消。此方式类似[Lease]，只不过仅需要一次确认，没有周期的`renewal`。

通过`Tentative Operation`的三种状态[REST]，可以帮助进一步理解这两种处理模型。

1. 如果`Confirm`操作不会失败，则需要显式的`Cancel`操作。此种模型属于**补偿模型**，另见[CompensatingAction]。
2. 如果`Confirm`操作可能失败，则需要显式的`Confirm`操作。

![tcc_state](/assets/images/201808/tcc_state.png)

在蚂蚁金服的[分布式事务解决方案与适用场景分析]一文中，也对`TCC`模型进行了介绍。

```
TCC 分布式事务模型直接作用于服务层。不与具体的服务框架耦合，与底层 RPC 协议无关，与底层存储介质无关，可以灵活选择业务资源的锁定粒度，减少资源锁持有时间，可扩展性好，可以说是为独立部署的 `SOA`(Service-Oriented Architecture) 服务而设计的。
```

文中认为`TCC`有**两个主要优势**：

* 跨服务的分布式事务。这一部分的作用与 XA 类似，服务的拆分。
* 两阶段拆分。就是把两阶段拆分成了**两个独立的阶段**，通过**资源业务锁定的方式进行关联**。资源业务锁定方式的好处在于，既不会阻塞其他事务在第一阶段对于相同资源的继续使用，也不会影响本事务第二阶段的正确执行。


在[程立谈大规模SOA系统]一文中，支付宝公司的首席架构师程立，在讲交易平台如何支持事务的方案时也提到了`TCC`处理模式。有几个观点：

1. 参考Ebay的最佳实践，在满足业务需求的情况下，允许有不一致的情况出现。
2. 关于事务的ACID哪些是必须保证，哪些是可以放宽的。AID是必须保证的。这样，没有隔离性的补偿方案就被排除掉了。
3. 设计了一套支付宝分布式事务方案，在数据库操作这个层次我们建立transaction，账务处理是一种TCC模式，任何一个账务处理你可以try它，最后可以confirm或者cancel它。在这个过程中，有TCC基础支持的话，结合中央事务协调器，我们就可以做分布式服务，而且可以是多层次的，任何一个服务都可以做在事务里面。

一个SOA应用由一系列服务松散复合而成。

![soa](/assets/images/201808/soa.png)

程立在2009年8月的北京IT168系统架构师大会上，关于[面向生产环境的SOA系统设计]的分享里，提出了两种分布式事务处理模式：

> 基于TCC模式的分布事务

![taobao_tcc](/assets/images/201808/taobao_tcc.png)

> 基于补偿模式的分布式事务

![taobao_compensation](/assets/images/201808/taobao_compensation.png)



# 应用

目前在工程上，关于处理事务问题的一些产品介绍。

## 全局事务服务GTS/Fescar

`GTS`已更名为`Fescar`，且已开源[Seata: Simple Extensible Autonomous Transaction Architecture]，[Seata wiki]，[Seata Quick Start]，可参考[阿里开源分布式事务解决方案 Fescar 全解析]。

**Fescar的发展历程：**

阿里是国内最早一批进行应用分布式（微服务化）改造的企业，所以很早就遇到微服务架构下的分布式事务问题。

* 2014年，阿里中间件团队发布 TXC（Taobao Transaction Constructor），为集团内应用提供分布式事务服务。
* 2016年，TXC 经过产品化改造，以 GTS（Global Transaction Service）的身份登陆阿里云，成为当时业界唯一一款云上分布式事务产品，在阿云里的公有云、专有云解决方案中，开始服务于众多外部客户。
* 2019年起，基于 TXC 和 GTS 的技术积累，阿里中间件团队发起了开源项目 Fescar（Fast & EaSy Commit And Rollback, FESCAR），和社区一起建设这个分布式事务解决方案。

TXC/GTS/Fescar 一脉相承，为解决微服务架构下的分布式事务问题交出了一份与众不同的答卷。

```
Ant Financial

* XTS: Extended Transaction Service. Ant Financial middleware team developed the distributed transaction middleware since 2007, which is widely used in Ant Financial and solves the problems of data consistency across databases and services.

* DTX: Distributed Transaction Extended. Since 2013, XTS has been published on the Ant Financial Cloud, with the name of DTX .

Alibaba

* TXC: Taobao Transaction Constructor. Alibaba middleware team start this project since 2014 to meet distributed transaction problem caused by application architecture change from monolithic to microservices.

* GTS: Global Transaction Service. TXC as an Aliyun middleware product with new name GTS was published since 2016.

* Fescar: we start the open source project Fescar based on TXC/GTS since 2019 to work closely with the community in the future.

Seata Community

* Seata :Simple Extensible Autonomous Transaction Architecture. Ant Financial joins Fescar, which make it to be a more neutral and open community for distributed transaction，and Fescar be rename to Seata.
```

**设计的目标：**

一个理想的分布式事务解决方案应该：像使用本地事务一样简单，业务逻辑只关注业务层面的需求，不需要考虑事务机制上的约束。

* **对业务无侵入**
	- 因为分布式事务这个技术问题的制约，要求应用在业务层面进行设计和改造。这种设计和改造往往会给应用带来很高的研发和维护成本。希望把分布式事务问题在中间件这个层次解决掉，不要求应用在业务层面做额外的工作。
	- 业务无侵入的方案。既有的主流分布式事务解决方案中，对业务无侵入的只有基于`XA`的方案，但应用`XA`方案存在3个方面的问题
		+ 要求数据库提供对`XA`的支持。如果遇到不支持`XA`（或支持得不好，比如 MySQL 5.7 以前的版本）的数据库，则不能使用。
		+ 受协议本身的约束，事务资源的锁定周期长。长周期的资源锁定从业务层面来看，往往是不必要的，而因为事务资源的管理器是数据库本身，应用层无法插手。这样形成的局面就是，基于`XA`的应用往往性能会比较差，而且很难优化。
		+ 已经落地的基于`XA`的分布式解决方案，都依托于重量级的应用服务器（Tuxedo/WebLogic/WebSphere等)，这是不适用于微服务架构的。
	- 实际上，最初分布式事务只有`XA`这个唯一方案。`XA`是完备的，但在实践过程中，由于种种原因往往不得不放弃，转而从**业务层面**着手来解决分布式事务问题。这些方案都要求在应用的业务层面把分布式事务技术约束考虑到设计中，**通常每一个服务都需要设计实现正向和反向的幂等接口**。这样的设计约束，往往会导致很高的研发和维护成本。
		+ 基于可靠消息的最终一致性方案
		+ TCC
		+ Saga

* **高性能**

引入分布式事务的保障，必然会有额外的开销，引起性能的下降。希望把分布式事务引入的性能损耗降到非常低的水平，让应用不因为分布式事务的引入导致业务的可用性受影响。



![seata-trans](/assets/images/201808/seata-trans.png)

![seata](/assets/images/201808/seata.png)

**Fescar定义3个组件来协调分布式事务的处理过程：** 

* Transaction Coordinator (TC)：事务协调器，维护全局事务的运行状态，负责协调并驱动全局事务的提交或回滚。
* Transaction Manager (TM)：控制全局事务的边界，负责开启一个全局事务，并最终发起全局提交或全局回滚的决议。
* Resource Manager (RM)：控制分支事务，负责分支注册、状态汇报，并接收事务协调器的指令，驱动分支（本地）事务的提交和回滚。

**一个典型的分布式事务过程：**

1. TM 向 TC 申请开启一个全局事务，全局事务创建成功并生成一个全局唯一的 XID。
2. XID 在微服务调用链路的上下文中传播。
3. RM 向 TC 注册分支事务，将其纳入 XID 对应全局事务的管辖。
4. TM 向 TC 发起针对 XID 的全局提交或回滚决议。
5. TC 调度 XID 下管辖的全部分支事务完成提交或回滚请求。


**Fescar 的协议机制总体上看与 XA 是一致的，与 XA 的差别在什么地方：**

* **剥离了分布式事务方案对数据库在协议支持上的要求**
	- XA 方案的 RM 实际上是在数据库层，RM 本质上就是数据库自身（通过提供支持 XA 的驱动程序来供应用使用）。
	- 而 Fescar 的 RM 是以二方包的形式作为**中间件层**部署在应用程序这一侧的，**不依赖与数据库本身对协议的支持**，当然也不需要数据库支持 XA 协议。这点对于微服务化的架构来说是非常重要的：应用层不需要为本地事务和分布式事务两类不同场景来适配两套不同的数据库驱动。
* **锁的粒度更小**
	- XA 的 2PC 过程，无论 Phase2 的决议是 commit 还是 rollback，事务性资源的锁都要保持到 Phase2 完成才释放。设想一个正常运行的业务，大概率是 90% 以上的事务最终应该是成功提交的，是否可以在 Phase1 就将本地事务提交呢？这样 90% 以上的情况下，可以省去 Phase2 持锁的时间，整体提高效率。
	- 而Fescar的XA，在绝大多数场景减少了事务持锁时间，从而提高了事务的并发度。(当然，你肯定会问：Phase1 即提交的情况下，Phase2 如何回滚呢？)

![xa](/assets/images/201808/xa.png)

![fescar-xa](/assets/images/201808/fescar-xa.png)

**Fescar的几种事务处理模式：**

* [Fescar-AT](https://github.com/fescar-group/awesome-fescar/blob/master/wiki/en-us/Fescar-AT.md) - **Automatic (Branch) Transaction Mode**

基于MySQL Innodb(local ACID transactions) + UNDO_LOG 的方式，这种模式对业务无入侵，业务本身不用关心回滚和提交逻辑。

```
Evolution from the two phases commit protocol:

Phase 1：commit business data and rollback log in the same local transaction, then release local lock and connection resources.
Phase 2：for commit case, do the work asynchronously and quickly. 
         for rollback case, do compensation, base on the rollback log created in the phase 1.
```

但是此模式对支持的SQL回滚语句有限制，并非所有情况都可以UNDO。以下是在业务本地隐式创建的`undo_log`反操作信息表。

``` sql
-- 注意此处0.3.0+ 增加唯一索引 ux_undo_log
CREATE TABLE `undo_log` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `branch_id` bigint(20) NOT NULL,
  `xid` varchar(100) NOT NULL,
  `rollback_info` longblob NOT NULL,
  `log_status` int(11) NOT NULL,
  `log_created` datetime NOT NULL,
  `log_modified` datetime NOT NULL,
  `ext` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `ux_undo_log` (`xid`,`branch_id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
```

* [Fescar-MT](https://github.com/fescar-group/awesome-fescar/blob/master/wiki/en-us/Fescar-MT.md) - **Manual (Branch) Transaction Mode**

此模式不依赖于底层资源具体是什么，分支事务需要应用自己来定义业务本身及提交和回滚的逻辑。需要业务提供`prepare`，`commit`，`rollback`这三个接口供框架调用。MT 模式一方面是 AT 模式的补充。另外，更重要的价值在于，通过 MT 模式可以把众多非事务性资源纳入全局事务的管理中。

```
the MT mode does not rely on transaction support for the underlying data resources:

One phase prepare behavior: Call the prepare logic of custom.
Two phase commit behavior: Call the commit logic of custom.
Two phase rollback behavior: Call the rollback logic of custom.
```

* 混合模式

因为 AT 和 MT 模式的分支从根本上行为模式是一致的，所以可以完全兼容，即，一个全局事务中，可以同时存在 AT 和 MT 的分支。这样就可以达到全面覆盖业务场景的目的：AT 模式可以支持的，使用 AT 模式；AT 模式暂时支持不了的，用 MT 模式来替代。另外，自然的，MT 模式管理的非事务性资源也可以和支持事务的关系型数据库资源一起，纳入同一个分布式事务的管理中。

* [Fescar-XA](https://github.com/seata/seata/wiki/XA-Mode)

XA的原生支持。(TBD)

**未来规划：**

设计的初衷：一个理想的分布式事务解决方案是不应该侵入业务的。MT 模式是在 AT 模式暂时不能完全覆盖所有场景的情况下，一个比较自然的补充方案。希望通过 AT 模式的不断演进增强，逐步扩大所支持的场景，MT 模式逐步收敛。未来会纳入对 XA 的原生支持，用 XA 这种无侵入的方式来覆盖 AT 模式无法触达的场景。

![fescar-future](/assets/images/201808/fescar-future.png)

* 微服务框架的支持
	- 事务上下文在微服务间的传播需要根据微服务框架本身的机制，订制最优的，对应用层透明的解决方案。
* 所支持的数据库类型
	- 因为 AT 涉及 SQL 的解析，所以在不同类型的数据库上工作，会有一些特定的适配。
* 配置和服务注册发现
	- 支持接入不同的配置和服务注册发现解决方案。比如：Nacos、Eureka、ZooKeeper 等。
* MT 模式的场景拓展
	- MT 模式的一个重要作用就是，可以把非关系型数据库的资源，通过 MT 模式分支的包装，纳入到全局事务的管辖中来。比如，Redis、HBase、RocketMQ 的事务消息等。
* 事务协调器的分布式高可用方案
	- 针对不同场景，支持不同的方式作为事务协调器 Server 端的高可用方案。比如，针对事务状态的持久化，可以是基于文件的实现方案，也可以是基于数据库的实现方案；集群间的状态同步，可以是基于 RPC 通信的方案，也可以是基于高可用 KV 存储的方案。


----------------------

`GTS`(Global Transaction Service)在2017年3月开始在阿里云上公测。主要解决的用户诉求是：**数据的一致性**。并保证：

1. 高性能(XA有性能问题)。
2. 易用性(减少入侵)。
3. 强一致(最终一致业务受限多)。 

关于GTS的几个问题：

问题1：GTS支持两种隔离级别，读未提交(RU)和读已提交(RC)。RU相比RC有明显性能优势，因此作为默认的隔离级别，但是RU对业务的影响大吗？

全局事务的隔离性是建立在分支事务的本地隔离级别基础之上的。在数据库本地隔离级别读已提交或以上的前提下，GTS设计了由事务协调器维护的 全局写排他锁，来保证事务间的写隔离，将全局事务默认定义在读未提交的隔离级别上。

RU -> 脏读 -> 会影响业务，导致应用逻辑并发处理不当。解决方法是：

{% highlight sql %}
select value from table where id = 'gerry' for update;
update table set value = value - 100 where id = 'gerry';
{% endhighlight %}

问题2：GTS什么场景下不能保证数据的一致性？

当用户用GTS事务操作数据时，同时并发用非GTS方式更改同一行数据，会影响数据一致性。因为，GTS阻止不了别的方式写数据，使用方需要按规则出牌，如果真需要混用，需要通过时间差的方式来防止冲突。

问题3：GTS目前支持哪些资源？

DRDS，Oracle，MySQL，RDS，PostgreSQL，MQ等。

问题4：`强一致`和`隔离性`没有必然的联系。

强一致性是新的数据一旦写入，在任意副本任意时刻都能读到新值。



# Refer

1. [Business Transactions, Compensation and the TryCancel/Confirm (TCC) Approach for Web Services]
2. [Java Transaction API (JTA)]
3. [Atomikos]
4. [Life beyond Distributed Transactions: an Apostate’s Opinion]
5. [X/Open XA]
6. [XA Transaction States]
7. [Two-phase commit protocol]
8. [Tentative Operation]
9. [分布式事务解决方案与适用场景分析]
10. [Lease]
11. [REST]
12. [CompensatingAction]
13. [程立谈大规模SOA系统]
14. [阿里开源分布式事务解决方案 Fescar 全解析]
15. [Seata: Simple Extensible Autonomous Transaction Architecture]
16. [Seata wiki]
17. [Seata Quick Start]
18. [Pattern: Database per service]
19. [Understanding of Fescar Isolation]
20. [分布式事务中间件 Fescar - RM 模块源码解读]
21. [MySQL Cluster Index：聚簇索引]
22. [MySQL 加锁处理分析]
23. [深入学习MySQL事务：ACID特性的实现原理]
24. [基于大中台架构的电商业务中台最佳实践之三：交易中台技术要点设计之高性能]


[Business Transactions, Compensation and the TryCancel/Confirm (TCC) Approach for Web Services]: https://cdn.ttgtmedia.com/searchWebServices/downloads/Business_Activities.pdf

[Java Transaction API (JTA)]: http://www.oracle.com/technetwork/java/javaee/tech/jta-138684.html

[Atomikos]: http://www.atomikos.com/Publications/TryCancelConfirm

[Life beyond Distributed Transactions: an Apostate’s Opinion]: https://cs.brown.edu/courses/cs227/archives/2012/papers/weaker/cidr07p15.pdf

[X/Open XA]: https://en.wikipedia.org/wiki/X/Open_XA

[XA Transaction States]: https://dev.mysql.com/doc/refman/5.7/en/xa-states.html

[Two-phase commit protocol]: https://en.wikipedia.org/wiki/Two-phase_commit_protocol

[分布式事务解决方案与适用场景分析]: https://zhuanlan.zhihu.com/p/34232350

[Tentative Operation]: https://www.enterpriseintegrationpatterns.com/patterns/conversation/TryConfirmCancel.html

[Lease]: https://www.enterpriseintegrationpatterns.com/patterns/conversation/Lease.html

[REST]: http://www.amazon.com/exec/obidos/ASIN/1441983023/enterpriseint-20

[CompensatingAction]: https://www.enterpriseintegrationpatterns.com/patterns/conversation/CompensatingAction.html

[程立谈大规模SOA系统]: http://www.infoq.com/cn/interviews/soa-chengli

[面向生产环境的SOA系统设计]: /assets/images/201808/面向生产环境的SOA系统设计.ppt

[阿里开源分布式事务解决方案 Fescar 全解析]: https://zhuanlan.zhihu.com/p/55958530

[Seata: Simple Extensible Autonomous Transaction Architecture]: https://github.com/seata/seata

[Seata wiki]: https://github.com/seata/seata/wiki

[Seata Quick Start]: https://github.com/seata/seata/wiki/Quick-Start

[Pattern: Database per service]: https://microservices.io/patterns/data/database-per-service.html

[Understanding of Fescar Isolation]: https://www.jianshu.com/p/4cb127b737cf

[分布式事务中间件 Fescar - RM 模块源码解读]: https://mp.weixin.qq.com/s/EzmZ-DAi-hxJhRkFvFhlJQ

[MySQL Cluster Index：聚簇索引]: http://dev.mysql.com/doc/refman/5.0/en/innodb-index-types.html

[MySQL 加锁处理分析]: http://blog.sae.sina.com.cn/archives/2127

[深入学习MySQL事务：ACID特性的实现原理]: https://www.cnblogs.com/kismetv/p/10331633.html

[基于大中台架构的电商业务中台最佳实践之三：交易中台技术要点设计之高性能]: https://zhuanlan.zhihu.com/p/48693144