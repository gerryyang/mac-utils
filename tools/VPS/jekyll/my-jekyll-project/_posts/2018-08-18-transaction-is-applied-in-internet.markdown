---
layout: post
title:  "事务在互联网场景中的应用"
date:   2018-08-18 14:08:00 +0800
categories: jekyll update
---
工作中经常遇到一些场景需要对提供的服务保证事务和可靠。比如，先付钱后发货，付钱和发货可以看做一个完整的事务。在一些更复杂的场景，可能涉及数据库，消息服务，RPC服务等多个资源的操作。

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
2. 如果出现了通信失败，或者超时错误，如何方式重复请求？

考虑到事务的完整性，如果补偿操作失败了，一般借助消息服务进行重试，但是可能会出现重复请求，因此需要后端RM的接口根据当前请求的ID做到幂等。



## Refer

1. [Business Transactions, Compensation and the TryCancel/Confirm (TCC) Approach for Web Services]
2. [Java Transaction API (JTA)]




[Business Transactions, Compensation and the TryCancel/Confirm (TCC) Approach for Web Services]: https://cdn.ttgtmedia.com/searchWebServices/downloads/Business_Activities.pdf

[Java Transaction API (JTA)]: http://www.oracle.com/technetwork/java/javaee/tech/jta-138684.html

