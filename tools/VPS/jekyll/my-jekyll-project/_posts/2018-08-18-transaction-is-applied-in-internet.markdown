---
layout: post
title:  "事务在互联网场景中的应用"
date:   2018-08-18 14:08:00 +0800
categories: tech
---
工作中经常遇到一些场景需要对提供的服务保证事务和可靠。比如，先付钱后发货，付钱和发货可以看做一个完整的事务。在一些更复杂的场景，可能涉及数据库，消息服务，RPC服务等多个资源的操作。

# 背景

关于事务的概念：

> A `transaction`(事务) is a logical unit of work which effects can either be made permanent in their entirety (committed) or cancelled in their entirety (rolled back). Before a transaction is committed or rolled back, it is active. Active transactions' effects are typically invisible to other, concurrent transactions. Consequently, only committed transactions' effects are visible.

一般，本地事务可以通过数据库的本地事务(ACID)来保证。当事务A在操作数据且commit或rollback前，其他事务都不能看到事务A对数据的修改。

![acid_lock_time](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201808/acid_lock_time.png)

内部是通过**Two-Phase Commit**来保证的。

```
Two-phase commit works in two phases: `a voting phase` and `a decision phase`.

* In the voting(or prepare) phase, the transaction manager will ask both the RM1 and RM2 whether they can agree with a successful termination or not. Each may return a negative reply, for instance if there was a time-out which caused the RM1's work to be rolled back. If one of them replies positively, then it should make sure it can always make the work permanent.

* After the transaction manager has received all of the replies (also called 'votes') it will make a global decision on the outcome of the transaction. This decision will depend on the collected replies:

	- If both replies were positive (meaning that both the RM1 and RM2 can make the work permanent), then the transaction manager will instruct each to commit.

	- If at least one reply is negative (or missing) then a rollback decision is sent to the remaining resource. This means that the remaining resource cancels (rolls back) the work done for the transaction.
```

但是，当出现多个不同的本地事务时，如何保证一致性？

例如，小Q想去国外旅游，需要在两个不同的网站上预定机票。在A网站预定从北京到深圳的机票，再在B网站预定深圳到国外的机票。如果小Q在预定B网站机票时失败了，就必须取消已经在A网站预定的机票。如果没有及时取消就会发生：

1. 小Q需承担在A网站预定的机票钱。
2. 在网站A预定机票的座位没有及时售卖出去，航空公司承担经济损失。

> PS: 在实际场景中，如果用户没有及时退票(cancel)，通常会根据退票的时间需要承担相应的罚金(penalties)。就像在12306上购买火车票，如果退票在开车时间前15天，不收取退票费，48小时以上的按票价5%计，24小时以上且不足48小时按票价10%计，不足24小时的按票价20%计。

是否有一种方法可以保证小Q在预定机票时类似像本地事务(ACID)提交一样，就可以避免或减轻上述问题。但是还需要具备：

1. 网站A在预定机票成功后，具备实时退票的能力，而不是通过人工介入。
2. 和数据库不同，多个网站(资源方)互不信任，存在DOS攻击的风险。

在实际场景下，上面两个条件一般都不能满足。是否还有其他方法？

可以考虑`compensation`(补偿)的方法。把每个操作都作为一个短的本地ACID事务，减少锁的时间。代价是牺牲了rollback的能力。而补偿(取消操作)需要根据具体的业务场景来实现。

![compensation_lock_time](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201808/compensation_lock_time.png)

通常，有两种不同的补偿方式：

1. **Perfect compensation**，把之前的操作全部`clean up`
2. **Imperfect compensation**，将之前的操作状态修改为`canceled`

对应实际的应用场景，比如：

1. 买股票是一个事务，卖股票可以作为一种补偿事务，两个事务分别是独立的。
2. 以定机票为例，先在A网站发起预定机票的请求，将当前的订单状态设置为`PENDING`状态，此时机票只是暂时被锁定，但是并没有出票成功。需要接收到第二阶段的`CONFIRM`或者`CANCEL`**补偿请求**后，此预定机票的事务才结束。

![TCC](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201808/TCC.png)

可以看出，有两种常见的补偿方式，一种是无状态的，比如买卖股票的场景。一种是有状态的，比如`TCC模型`，采用`two-phase protocol`处理方式。

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

## 本地事务

可以用`START TRANSACTION`语句开始一个事务，然后要么使用`COMMIT`提交事务将修改的数据持久保留，要么使用`ROLLBACK`撤销所有的修改。

{% highlight sql %}
START TRANSACTION;
SELECT balance FROM checking WHERE customer_id = 'gerry';
UPDATE checking SET balance = balance - 200.00 WHERE customer_id = 'gerry';
UPDATE savings SET balance = balance + 200.00 WHERE customer_id = 'gerry';
COMMIT;
{% endhighlight %}

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

在SQL标准中定义了`四种`隔离级别。每一种级别都规定了一个事务中所做的修改，哪些在事务内和事务间是可见的，哪些是不可见的。较低级别的隔离通常可以执行更高的并发，系统的开销也更低。每种存储引擎实现的隔离级别不尽相同，请查阅具体相关手册。

```
1. READ UNCOMMITTED（未提交读） 
在此级别，事务中的修改即使没有提交，对其他事务也都是可见的，即，“赃读”。（实际应用中，很少使用）

2. READ COMMITTED（提交读，不可重复读） 
大多数数据的默认隔离级别都是此级别（但MySQL不是）。此级别满足隔离性的简单定义：一个事务开始时，只能看见已经提交的事务所做的修改（或者，一个事务从开始直到提交之前，所做的任何修改对其他事务都是不可见的）。此级别，也称为“不可重复读”，因为两次执行同样的查询，可能会得到不一样的结果。

3. REPEATABLE READ（可重复读） 
此级别保证了在同一个事务中，多次读取同样纪录的结果是一致的。此级别，是MySQL的默认事务隔离级别。

4. SERIALIZABLE（可串行化） 
此级别是最高的隔离级别。它通过强制事务串行执行，避免了“幻读”的问题。此级别，会在读取的每一行数据上都加锁，所以可能导致大量的超时和锁争用的问题。（实际应用中也很少使用这个隔离级别，只有在非常需要确保数据的一致性而且可以接受没有并发的情况下，才考虑使用此级别）
```

## X/Open XA

[X/Open XA]，`XA`是e**X**tended **A**rchitecture的简称，提出了一种**Distributed Transaction Processing (DTP)**处理分布式事务的模型。采用[Two-phase commit protocol]来保证，对多种资源(databases, application servers, message queues, transactional caches, etc.)操作时，具备数据库ACID的事务能力。

![DTP](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201808/DTP.png)

```
**DTP**（Distributed Transaction Processing）systems are those where work in support of a
single transaction may occur across RMs. This has several implications:
* The system must have a way to refer to a transaction that encompasses all work
done anywhere in the system. （知道每个子事务的状态）
* The decision to commit or roll back a transaction must consider the status of work
done anywhere on behalf of the transaction. The decision must have uniform effect
throughout the DTP system.  （根据每个子事务的状态来决定commit还是rollback）
```

MySQL 5.0或更新版本的数据库已经开始支持XA事务。存储引擎的事务特性能够保证在存储引擎级别实现ACID。而XA分布式事务，可以让ACID扩展到多个数据库。实际上，在MySQL中有两种XA事务。

1. 外部XA事务。
2. 内部XA事务。协调存储引擎和二进制日志。详见High Performance MySQL 7.11.1章节介绍。

注意：

1. XA事务是一种在多个服务器之间同步数据的方法。如果由于某些原因不能使用MySQL本身的复制，或者性能并不是瓶颈的时候，可以尝试使用。
2. 因为涉及网络通信延迟，外部XA事务比内部XA事务消耗更大，不稳定的网络通信或者用户长时间的等待而不提交，会导致所有参与者都在等待。因此，建议优先考虑其他更高性能的方案。
3. innodb_support_xa 选项：控制MySQL内部存储引擎和二进制日志之间的分布式事务。

MySQL XA事务状态变化：(详见[XA Transaction States])

1. 使用`XA START`启动一个XA事务，并把它置为**ACTIVE状态**。
2. 对一个ACTIVE XA事务，发布构成事务的SQL语句，然后发布一个`XA END`语句，`XA END`把事务置为**IDLE状态**。
3. 对一个IDLE XA事务， 发布一个`XA PREPARE`语句。把事务置为**PREPARE状态**，此时XA RECOVER 语句的输出包含事务的xid值(XA RECOVER语句会列出所有处于PREPARE状态的XA事务)。 
4. 对一个PREPARE XA事务，可以发布一个`XA COMMIT`语句来提交和终止事务，或者发布一个`XA ROLLBACK`来回滚并终止事务。

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

Pat helland在2007年也发表了一篇相同观点的文章[Life beyond Distributed Transactions: an Apostate’s Opinion]，考虑在无限扩展的应用场景下，业务层不应该关心底层扩展所带来的问题，应该由统一的平台或者框架来屏蔽底层扩展所带来的差异。并提出了一种`Performing Tentative(不确定的) Business Operations`处理流程(**workflow**)(**Tentative Operations, Confirmation, and Cancellation**)，来减少分布式场景可能导致的不一致(**uncertainty**)问题。

![2007_scale_agnostic](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201808/2007_scale_agnostic.png)

``` 
To reach an agreement across entities, one entity has to ask another to accept some uncertainty. This is done by sending a message which requests a commitment but leaves open the possibility of cancellation. This is called a tentative operation and it represented by a message
flowing between two entities. At the end of this step, one of the entities agrees to abide by the wishes of the other.
```

在蚂蚁金服的[分布式事务解决方案与适用场景分析]一文中，也对`TCC`模型进行了介绍。

```
TCC 分布式事务模型直接作用于服务层。不与具体的服务框架耦合，与底层 RPC 协议无关，与底层存储介质无关，可以灵活选择业务资源的锁定粒度，减少资源锁持有时间，可扩展性好，可以说是为独立部署的 SOA 服务而设计的。
```

文中认为`TCC`有**两个主要优势**：

* 跨服务的分布式事务。这一部分的作用与 XA 类似，服务的拆分。
* 两阶段拆分。就是把两阶段拆分成了**两个独立的阶段**，通过**资源业务锁定的方式进行关联**。资源业务锁定方式的好处在于，既不会阻塞其他事务在第一阶段对于相同资源的继续使用，也不会影响本事务第二阶段的正确执行。


[Tentative Operation]文中也对`How can a requestor ensure a consistent outcome across multiple, independent providers?`的问题进行了讨论。

```
* The communication channels used by loosely coupled, distributed systems usually do not provide transactional semantics at the transport level. Instead, participants can send and receive message, often reliably.
* While some operations are inherently reversible, e.g. debiting a bank account after a credit, other operations, such as shipping a package or scrapping a car cannot easily be undone. As a result, the requestor might not be able to do much to rectify the situation.
* Distributed conversations typically involve uncertainty: a participant cannot be certain that a conversation partner continues in the conversation or even exist after the last interaction. Participants should therefore allocate resources cautiously.
```

![requestor_provider](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201808/requestor_provider.png)

`Tentative Operation`有两种处理模型：

1. 显式的`cancellation`操作，例如，直到收到取消操作。
2. 隐式的`cancellation`操作，例如，超时自动取消。此方式类似[Lease]，只不过仅需要一次确认，没有周期的`renewal`。

通过`Tentative Operation`的三种状态[REST]，可以帮助进一步理解这两种处理模型。

1. 如果`Confirm`操作不会失败，则需要显式的`Cancel`操作。此种模型属于**补偿模型**，另见[CompensatingAction]。
2. 如果`Confirm`操作可能失败，则需要显式的`Confirm`操作。

![tcc_state](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201808/tcc_state.png)

 

# 应用

TODO


# Refer

1. [Business Transactions, Compensation and the TryCancel/Confirm (TCC) Approach for Web Services]
2. [Java Transaction API (JTA)]
3. [Atomikos]
4. [Life beyond Distributed Transactions: an Apostate’s Opinion]
5. [X/Open XA]
6. [XA Transaction States]
7. [Two-phase commit protocol]
8. [分布式事务解决方案与适用场景分析]
9. [Tentative Operation]
10. [Lease]
11. [REST]
12. [CompensatingAction]
13. [程立谈大规模SOA系统]


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