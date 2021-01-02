---
layout: post
title:  "MySQL in Action"
date:   2019-12-25 09:15:00 +0800
categories: MySQL
---

* Do not remove this line (it will not be displayed)
{:toc}



# 配置相关

## 设置MySQL的最大链接数

```
show processlist;                  # 查看当前连接数
show variables like 'max%';        # 查看当前配置的最大连接数
```

修改最大连接数的方法：

方法1：修改my.cnf配置。在[mysqld]节点下指定如下设置：
max_connections = 2000

方法2：在使用mysqld命令启动时，显示指定 -O max_connections=2000 参数，缺点是重启则可能失效。

方法3：set global var=xxx;
设置全局变量，不需要重启MySQL


## MySQL访问授权

用root用户登录mysql

```
mysql> use mysql
mysql> show tables;
mysql> select * from user limit 1\G                                 # 查看用户的信息
mysql> grant all on *.* to root@'$ip' identified by '';             # 授权开发机
mysql> flush privileges;                                            # 刷新MySQL的系统权限表
```

```
mysql> show grants\G
```

## 查看和设置字符集

``` sql
SHOW CHARACTER SET;
set names gb2312;
set names utf8;
```

# MySQL事务

## 事务隔离级别

| 隔离级别 | 功能
| -- | --
| RU | 可以看到未提交的数据（脏读），举个例子：别人说的话你都相信了，但是可能他只是说说，并不实际做。
| RC | 读取提交的数据。但是，可能多次读取的数据结果不一致（不可重复读）。用读写的观点就是：读取的行数据，可以写。
| RR | 可以重复读取，但有幻读。读写观点：读取（当前读）的数据行不可写，但是可以往表中新增数据。在MySQL中，其他事务新增的数据，看不到，不会产生幻读。采用多版本并发控制（MVCC）机制解决幻读问题。
| serializable | 可读，不可写。写数据必须等待另一个事务结束。

## 查看和设置隔离级别

``` sql
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

# MySQL索引

## 索引原理

* Indexes are used to find rows with specific column values quickly. Without an index, MySQL must begin with the first row and then read through the entire table to find the relevant rows. The larger the table, the more this costs. If the table has an index for the columns in question, MySQL can quickly determine the position to seek to in the middle of the data file without having to look at all the data. This is much faster than reading every row sequentially.
* Most MySQL indexes (PRIMARY KEY, UNIQUE, INDEX, and FULLTEXT) are stored in [B-trees](https://dev.mysql.com/doc/refman/8.0/en/glossary.html#glos_b_tree). Exceptions: Indexes on spatial data types use R-trees; MEMORY tables also support [hash indexes](https://dev.mysql.com/doc/refman/8.0/en/glossary.html#glos_hash_index); InnoDB uses inverted lists for FULLTEXT indexes.


## 联合索引

``` sql
CREATE TABLE test (
    id         INT NOT NULL,
    last_name  CHAR(30) NOT NULL,
    first_name CHAR(30) NOT NULL,
    PRIMARY KEY (id),
    INDEX name (last_name, first_name)
);
```

* MySQL can create composite indexes (that is, indexes on multiple columns). An index may consist of up to 16 columns. 
* MySQL can use multiple-column indexes for queries that test all the columns in the index, or queries that test just the first column, the first two columns, the first three columns, and so on. If you specify the columns in the right order in the index definition, a single composite index can speed up several kinds of queries on the same table.
* MySQL cannot use the index to perform lookups if the columns do not form `a leftmost prefix of the index`.

refer:

* [Multiple-Column Indexes](https://dev.mysql.com/doc/refman/8.0/en/multiple-column-indexes.html)
* [How MySQL Uses Indexes](https://dev.mysql.com/doc/refman/8.0/en/mysql-indexes.html)


# MySQL Json操作

测试用例：

```
CREATE DATABASE test_conf;
USE test_conf;
CREATE TABLE `t_test` (
`FId` int NOT NULL AUTO_INCREMENT,
`FExtAttr` json NOT NULL COMMENT 'extend attr',
`FUpdateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'update time',
PRIMARY KEY (`FId`) 
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=utf8mb4;

insert into t_test(FExtAttr) values('[{"Key":"Creator","Value":"gerry"}, {"Key":"Remark","Value":"ok"}]');
insert into t_test(FExtAttr) values('[{"Key":"Creator","Value":"bob"}, {"Key":"Remark","Value":"ok"}]');
update t_test set FExtAttr = '[{"Key":"Creator","Value":"gerry"}, {"Key":"Creator","Value":"yang"}, {"Key":"Remark","Value":"ok"}]';

```

常见用法：


```
mysql> select * from t_test;
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
| FId | FExtAttr                                                                                                      | FUpdateTime         |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
|  10 | [{"Key": "Creator", "Value": "gerry"}, {"Key": "Creator", "Value": "yang"}, {"Key": "Remark", "Value": "ok"}] | 2020-07-24 18:49:19 |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
1 row in set (0.03 sec)

mysql> select * from t_test where JSON_CONTAINS(FExtAttr, '{"Key": "Creator", "Value": "gerry"}', "$[0]");
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
| FId | FExtAttr                                                                                                      | FUpdateTime         |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
|  10 | [{"Key": "Creator", "Value": "gerry"}, {"Key": "Creator", "Value": "yang"}, {"Key": "Remark", "Value": "ok"}] | 2020-07-24 18:49:19 |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
1 row in set (0.01 sec)

mysql> select * from t_test where JSON_CONTAINS_PATH(FExtAttr, 'one', "$[0].Key");
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
| FId | FExtAttr                                                                                                      | FUpdateTime         |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
|  10 | [{"Key": "Creator", "Value": "gerry"}, {"Key": "Creator", "Value": "yang"}, {"Key": "Remark", "Value": "ok"}] | 2020-07-24 18:49:19 |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
1 row in set (0.00 sec)

mysql> select * from t_test where JSON_CONTAINS_PATH(FExtAttr, 'all', "$[0].Key");
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
| FId | FExtAttr                                                                                                      | FUpdateTime         |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
|  10 | [{"Key": "Creator", "Value": "gerry"}, {"Key": "Creator", "Value": "yang"}, {"Key": "Remark", "Value": "ok"}] | 2020-07-24 18:49:19 |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
1 row in set (0.00 sec)

mysql> select JSON_EXTRACT(JSON_EXTRACT(FExtAttr, '$[0]'), '$.Value') from t_test where JSON_EXTRACT(JSON_EXTRACT(FExtAttr, '$[0]'), '$.Key') = 'Creator';
+---------------------------------------------------------+
| JSON_EXTRACT(JSON_EXTRACT(FExtAttr, '$[0]'), '$.Value') |
+---------------------------------------------------------+
| "gerry"                                                 |
| "bob"                                                   |
+---------------------------------------------------------+
2 rows in set (0.00 sec)

mysql> select JSON_EXTRACT(FExtAttr, "$[0]") from t_test;
+--------------------------------------+
| JSON_EXTRACT(FExtAttr, "$[0]")       |
+--------------------------------------+
| {"Key": "Creator", "Value": "gerry"} |
+--------------------------------------+
1 row in set (0.00 sec)

mysql> select FExtAttr->"$[0]" from t_test;
+--------------------------------------+
| FExtAttr->"$[0]"                     |
+--------------------------------------+
| {"Key": "Creator", "Value": "gerry"} |
+--------------------------------------+
1 row in set (0.01 sec)

mysql> select JSON_EXTRACT(FExtAttr->"$[0]", "$.Key") from t_test;
+-----------------------------------------+
| JSON_EXTRACT(FExtAttr->"$[0]", "$.Key") |
+-----------------------------------------+
| "Creator"                               |
+-----------------------------------------+
1 row in set (0.00 sec)

mysql> select JSON_EXTRACT(FExtAttr->"$[0]", "$.Value") from t_test;
+-------------------------------------------+
| JSON_EXTRACT(FExtAttr->"$[0]", "$.Value") |
+-------------------------------------------+
| "gerry"                                   |
+-------------------------------------------+
1 row in set (0.00 sec)

mysql> select * from t_test where FExtAttr->"$[0]" = '{"Key": "Creator", "Value": "gerry"}';
Empty set (0.00 sec)

mysql> select * from t_test where FExtAttr->>"$[0]" = '{"Key": "Creator", "Value": "gerry"}';
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
| FId | FExtAttr                                                                                                      | FUpdateTime         |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
|  10 | [{"Key": "Creator", "Value": "gerry"}, {"Key": "Creator", "Value": "yang"}, {"Key": "Remark", "Value": "ok"}] | 2020-07-24 18:49:19 |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
1 row in set (0.00 sec)

mysql> select * from t_test where JSON_UNQUOTE(FExtAttr->"$[0]") = '{"Key": "Creator", "Value": "gerry"}';
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
| FId | FExtAttr                                                                                                      | FUpdateTime         |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
|  10 | [{"Key": "Creator", "Value": "gerry"}, {"Key": "Creator", "Value": "yang"}, {"Key": "Remark", "Value": "ok"}] | 2020-07-24 18:49:19 |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
1 row in set (0.00 sec)

mysql> select JSON_KEYS(FExtAttr) from t_test;
+---------------------+
| JSON_KEYS(FExtAttr) |
+---------------------+
| NULL                |
+---------------------+
1 row in set (0.00 sec)

mysql> select JSON_KEYS(FExtAttr->"$[0]") from t_test;
+-----------------------------+
| JSON_KEYS(FExtAttr->"$[0]") |
+-----------------------------+
| ["Key", "Value"]            |
+-----------------------------+
1 row in set (0.00 sec)

mysql> select JSON_KEYS(FExtAttr, "$[0]") from t_test;
+-----------------------------+
| JSON_KEYS(FExtAttr, "$[0]") |
+-----------------------------+
| ["Key", "Value"]            |
+-----------------------------+
1 row in set (0.00 sec)

mysql> select * from t_test;
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
| FId | FExtAttr                                                                                                      | FUpdateTime         |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
|  10 | [{"Key": "Creator", "Value": "gerry"}, {"Key": "Creator", "Value": "yang"}, {"Key": "Remark", "Value": "ok"}] | 2020-07-24 18:49:19 |
|  11 | [{"Key": "Creator", "Value": "bob"}, {"Key": "Remark", "Value": "ok"}]                                        | 2020-07-27 09:32:16 |
+-----+---------------------------------------------------------------------------------------------------------------+---------------------+
2 rows in set (0.00 sec)

mysql> select FId, JSON_SEARCH(FExtAttr, 'all', 'Creator', NULL, '$[*]') from t_test;
+-----+-------------------------------------------------------+
| FId | JSON_SEARCH(FExtAttr, 'all', 'Creator', NULL, '$[*]') |
+-----+-------------------------------------------------------+
|  10 | ["$[0].Key", "$[1].Key"]                              |
|  11 | "$[0].Key"                                            |
+-----+-------------------------------------------------------+
2 rows in set (0.00 sec)

mysql> select JSON_VALUE(FExtAttr, '$[0]') from t_test;
+--------------------------------------+
| JSON_VALUE(FExtAttr, '$[0]')         |
+--------------------------------------+
| {"Key": "Creator", "Value": "gerry"} |
| {"Key": "Creator", "Value": "bob"}   |
+--------------------------------------+
2 rows in set (0.00 sec)

mysql> select FExtAttr->"$[0]" from t_test;
+--------------------------------------+
| FExtAttr->"$[0]"                     |
+--------------------------------------+
| {"Key": "Creator", "Value": "gerry"} |
| {"Key": "Creator", "Value": "bob"}   |
+--------------------------------------+
2 rows in set (0.00 sec)

mysql> select FId, FExtAttr from t_test where CAST('{"Key": "Creator", "Value": "gerry"}' AS JSON) MEMBER OF(FExtAttr->"$[*]");
+-----+---------------------------------------------------------------------------------------------------------------+
| FId | FExtAttr                                                                                                      |
+-----+---------------------------------------------------------------------------------------------------------------+
|  10 | [{"Key": "Creator", "Value": "gerry"}, {"Key": "Creator", "Value": "yang"}, {"Key": "Remark", "Value": "ok"}] |
+-----+---------------------------------------------------------------------------------------------------------------+
1 row in set (0.00 sec)

mysql> select FId, FExtAttr from t_test where CAST('{"Key": "Creator", "Value": "gerry"}' AS JSON) MEMBER OF(FExtAttr);
+-----+---------------------------------------------------------------------------------------------------------------+
| FId | FExtAttr                                                                                                      |
+-----+---------------------------------------------------------------------------------------------------------------+
|  10 | [{"Key": "Creator", "Value": "gerry"}, {"Key": "Creator", "Value": "yang"}, {"Key": "Remark", "Value": "ok"}] |
+-----+---------------------------------------------------------------------------------------------------------------+
1 row in set (0.00 sec)
```


Refer:

* https://dev.mysql.com/doc/refman/8.0/en/json-function-reference.html


# 实践之坑

## 利用 force index 优化sql语句性能


``` sql
select customer,count(1) c  
from upv_1  force index(idx_created)  
where created between "2015-07-06" and "2015-07-07"  
group by customer   
having c > 15  
order by c desc  
```

https://www.jianshu.com/p/df2bb6ca178e


## MySQL 事务 `set autocommit = 0` 与 `start transaction` 的区别

首先，数据库的sql作为事务提交，可以分为：单个sql的事务提交 和 多个sql的事务提交。

* 执行 `set autocommit = 0`，表示对当前session禁用自动提交事务，之后的每次的SQL操作都需要显式`commit/rollback`才能提交事务。
* 执行 `start transaction`（通常在`set autocommit = 1`模式下），表示对当前session启动一个新事务，之后只有当前的SQL操作需要显式`commit/rollback`才能提交事务，而其他SQL操作不受影响，仍是自动的模式提交事务。	


之前遇到一个对 `set autocommit = 0` 误用的场景：

某个订单系统，提供了db事务的操作接口，此事务接口在每次执行前都会执行`set autocommit = 0`，然后执行业务sql，最后根据结果执行`commit`或`rollback`。最后在测试时发现会偶现，非事务的接口（单条sql的事务）提交成功了，但是却没有生效。原因是，由于事务操作接口对当前的db连接使用`set autocommit = 0`后，会使当前连接上的sql操作都不会自动提交，并且此连接会被其他非事务的s	ql请求复用，而非事务的sql请求不会主动commit或rollback。当事务的sql操作在非事务的sql操作后执行了rollback，就会导致非事务的sql操作结果一起rollback。


以下是数据库日志，可以看到非事务sql操作在提交后，没有显式commit，最后被其他事务sql操作的rollback执行后，之前非事务sql操作也rollback了：

```
[2020-03-28 10:04:00 114016] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.294(ms) inj_id:5 sql:3,24 "set session autocommit=0"
[2020-03-28 10:04:00 114248] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.536(ms) inj_id:7 sql:3,41 "XA START '36c73128-ba-87787c-5e7eb090-39'"
[2020-03-28 10:04:00 114700] ERROR tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:1.002(ms) inj_id:1 sql:3,314 "insert into `order_center`.t_midas_order_info (FOfferId, FOrderId, FOrderType, FLoginId, FLoginIdType, FProvideId, FProvideIdType, FPayId, FPayIdType, FCreateTime)  values  ('TC10100', 'skycfwu_tdeatest003', '2', '773632134', 'hy_gameid', '773632134', 'hy_gameid', '773632134', 'hy_gameid', '2020-03-28 10:03:59') "
[2020-03-28 10:04:00 150632] DEBUG tid:13583 con:0x7f8ae8c33c00 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.247(ms) inj_id:9 sql:3,39 "XA END '36c73128-ba-87787c-5e7eb090-39'"
[2020-03-28 10:04:00 150833] DEBUG tid:13583 con:0x7f8ae8c33c00 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.450(ms) inj_id:14 sql:3,44 "XA ROLLBACK '36c73128-ba-87787c-5e7eb090-39'"
[2020-03-28 10:04:00 187419] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.305(ms) inj_id:7 sql:3,41 "XA START '36c73128-ba-87787d-5e7eb090-39'"
[2020-03-28 10:04:00 188104] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:1.001(ms) inj_id:1 sql:3,205 "select FLoginId,FLoginIdType,FProvideId,FProvideIdType,FPayId,FPayIdType from `order_center`.t_midas_order_info where ((FOfferId = 'TC10100') and (FOrderId = 'skycfwu_tdeatest003') and (FOrderType = '2')) "
[2020-03-28 10:04:00 224509] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.795(ms) inj_id:1 sql:3,204 "select FSubOrderId,FState,FPayChannel,FPayChannelSubId,FGoodsDetail from `order_center`.t_midas_suborder_state where ((FOfferId = 'TC10100') and (FOrderId = 'skycfwu_tdeatest003') and (FOrderType = '2')) "
[2020-03-28 10:04:00 305495] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.737(ms) inj_id:1 sql:3,180 "select FOfferId,FOrderId,FSubOrderId,FState from `order_center`.t_midas_suborder_state where ((FOfferId = 'TC10100') and (FOrderId = 'skycfwu_tdeatest003') and (FOrderType = '2')) "
[2020-03-28 10:04:00 343019] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.708(ms) inj_id:1 sql:3,174 "update `order_center`.t_midas_suborder_state SET FState='601' where ((FOfferId = 'TC10100') and (FOrderId = 'skycfwu_tdeatest003') and (FOrderType = '2') and (FState = '0')) "
[2020-03-28 10:04:00 380000] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.740(ms) inj_id:1 sql:3,210 "select FOfferId,FOrderId,FPayChannel,FPayChannelSubId,FState,FChannelOrderId from `order_center`.t_midas_order_state where ((FOfferId = 'TC10100') and (FOrderId = 'skycfwu_tdeatest003') and (FOrderType = '2')) "
[2020-03-28 10:04:00 416701] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.462(ms) inj_id:1 sql:3,339 "insert into `order_center`.t_midas_order_state (FOfferId, FOrderId, FPayChannel, FPayChannelSubId, FOrderType, FPortalSerialNo, FState, FChannelOrderId, FCreateTime, FOrderTime)  values  ('TC10100', 'skycfwu_tdeatest003', 'account', '1', '2', 'OPENTCGSX107943-20200328-A401oiVYCcGW', '0', '', '2020-03-28 10:04:00', '2020-03-28 10:04:00') "
[2020-03-28 10:04:00 453952] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:32340 S:100.125.130.85:4002 timecost:0.367(ms) inj_id:5 sql:3,24 "set session autocommit=0"
[2020-03-28 10:04:00 454150] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:32340 S:100.125.130.85:4002 timecost:0.572(ms) inj_id:7 sql:3,41 "XA START '36c73128-ba-87787d-5e7eb090-39'"
[2020-03-28 10:04:00 454751] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:32340 S:100.125.130.85:4002 timecost:1.185(ms) inj_id:1 sql:3,251 "select FOfferId,FOrderId,FSubOrderId,FRefundInfo,FPayAmt from `order_center`.t_midas_suborder_state where ((FOfferId = 'TC10100') and (FOrderId = '20200323676000000000066606_6') and (FSubOrderId = '20200323676000000001117752') and (FOrderType = '1')) "
[2020-03-28 10:04:00 527307] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:32340 S:100.125.130.85:4002 timecost:0.609(ms) inj_id:1 sql:3,385 "update order_center.t_midas_suborder_state SET FRefundInfo=JSON_SET(FRefundInfo,'$.refund_id','skycfwu_tdeatest003') where ((FOfferId = 'TC10100') and (FOrderId = '20200323676000000000066606_6') and (FSubOrderId = '20200323676000000001117752') and (FOrderType = '1') and (JSON_EXTRACT(FRefundInfo,'$.refund_id') = '') and ((JSON_EXTRACT(FRefundInfo,'$.refund_amt') + '1') <= FPayAmt)) "
[2020-03-28 10:04:00 563267] DEBUG tid:13583 con:0x7f8ae8c33c00 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:32340 S:100.125.130.85:4002 timecost:0.331(ms) inj_id:9 sql:3,39 "XA END '36c73128-ba-87787d-5e7eb090-39'"
[2020-03-28 10:04:00 563610] DEBUG tid:13583 con:0x7f8ae8c34800 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.666(ms) inj_id:9 sql:3,39 "XA END '36c73128-ba-87787d-5e7eb090-39'"
[2020-03-28 10:04:00 563671] DEBUG tid:13583 con:0x7f8ae8c33c00 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:32340 S:100.125.130.85:4002 timecost:0.751(ms) inj_id:14 sql:3,44 "XA ROLLBACK '36c73128-ba-87787d-5e7eb090-39'"
[2020-03-28 10:04:00 564393] DEBUG tid:13583 con:0x7f8ae8c34800 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:1.453(ms) inj_id:14 sql:3,44 "XA ROLLBACK '36c73128-ba-87787d-5e7eb090-39'"
[2020-03-28 10:04:00 637401] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.296(ms) inj_id:7 sql:3,41 "XA START '36c73128-ba-87787e-5e7eb090-39'"
[2020-03-28 10:04:00 637855] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.767(ms) inj_id:1 sql:3,234 "update `order_center`.t_midas_suborder_state SET FState='604' where ((FOfferId = 'TC10100') and (FOrderId = 'skycfwu_tdeatest003') and (FSubOrderId = '20200323676000000001117752') and (FOrderType = '2') and (FState in ('601','502'))) "
[2020-03-28 10:04:00 673744] DEBUG tid:13583 con:0x7f8ae8c34800 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.285(ms) inj_id:9 sql:3,39 "XA END '36c73128-ba-87787e-5e7eb090-39'"
[2020-03-28 10:04:00 673994] DEBUG tid:13583 con:0x7f8ae8c34800 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.539(ms) inj_id:14 sql:3,44 "XA ROLLBACK '36c73128-ba-87787e-5e7eb090-39'"
[2020-03-28 10:04:00 711017] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:0.433(ms) inj_id:7 sql:3,41 "XA START '36c73128-ba-87787f-5e7eb090-39'"
[2020-03-28 10:04:00 711617] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:35352 S:100.125.130.84:4003 timecost:1.045(ms) inj_id:1 sql:3,258 "select FOfferId,FOrderId,FSubOrderId,FPayChannel,FPayChannelSubId,FState from `order_center`.t_midas_suborder_state where ((FOfferId = 'TC10100') and (FOrderId = 'skycfwu_tdeatest003') and (FSubOrderId = '20200323676000000001117752') and (FOrderType = '2')) "
[2020-03-28 10:04:30 748321] DEBUG tid:13583 con:0x7f8ae8c35400 user:midas_w C:100.99.70.99:49055 G:100.125.130.83:32340 S:100.125.130.85:4002 timecost:0.299(ms) inj_id:7 sql:3,41 "XA START '36c73128-ba-87787f-5e7eb090-39'"

```

* [Difference between SET autocommit=1 and START TRANSACTION in mysql](https://stackoverflow.com/questions/2950676/difference-between-set-autocommit-1-and-start-transaction-in-mysql-have-i-misse)
* [13.3.7 SET TRANSACTION Statement](https://dev.mysql.com/doc/refman/8.0/en/set-transaction.html#isolevel_serializable)


## MySQL乱码之utf8mb4

Q:

* What is the difference between utf8mb4 and utf8 charsets in MySQL? I already know about ASCII, UTF-8, UTF-16 and UTF-32 encodings; but I'm curious to know whats the difference of utf8mb4 group of encodings with other encoding types defined in MySQL Server. Are there any special benefits/proposes of using utf8mb4 rather than utf8?

A:

[UTF-8](https://en.wikipedia.org/wiki/UTF-8) is **a variable-length encoding**. In the case of UTF-8, this means that **storing one code point requires one to four bytes**. However, MySQL's encoding called "utf8" (alias of "utf8mb3") **only stores a maximum of three bytes** per code point. So the character set "utf8"/"utf8mb3" cannot store all Unicode code points: it only supports the range 0x000 to 0xFFFF, which is called the ["Basic Multilingual Plane"](https://en.wikipedia.org/wiki/Plane_%28Unicode%29#Basic_Multilingual_Plane). See also [Comparison of Unicode encodings](https://en.wikipedia.org/wiki/Comparison_of_Unicode_encodings#In_detail).

This is what (a previous version of the same page at) the [MySQL documentation - 10.9.1 The utf8mb4 Character Set (4-Byte UTF-8 Unicode Encoding)](https://dev.mysql.com/doc/refman/5.5/en/charset-unicode-utf8mb4.html) has to say about it:

> The character set named utf8[/utf8mb3] uses a maximum of three bytes per character and contains only BMP characters. As of MySQL 5.5.3, the utf8mb4 character set uses a maximum of four bytes per character supports supplemental characters:
> For a BMP character, utf8[/utf8mb3] and utf8mb4 have identical storage characteristics: same code values, same encoding, same length.
> For a supplementary character, **utf8[/utf8mb3] cannot store the character at all**, while utf8mb4 requires four bytes to store it. Since utf8[/utf8mb3] cannot store the character at all, you do not have any supplementary characters in utf8[/utf8mb3] columns and you need not worry about converting characters or losing data when upgrading utf8[/utf8mb3] data from older versions of MySQL.

So if you want your column to support storing characters lying outside the BMP (and you usually want to), such as [emoji](https://en.wikipedia.org/wiki/Emoji), use "utf8mb4". See also [What are the most common non-BMP Unicode characters in actual use?](https://stackoverflow.com/questions/5567249/what-are-the-most-common-non-bmp-unicode-characters-in-actual-use).

**Emoji** are now the most common **non-BMP characters** by far. Otherwise known as U+1F602 FACE WITH TEARS OF JOY, is the most common one on Twitter's public stream. It occurs more frequently than the tilde!

From [10.10.1 Unicode Character Sets](https://dev.mysql.com/doc/refman/8.0/en/charset-unicode-sets.html):

* utf8mb4: A UTF-8 encoding of the Unicode character set using one to four bytes per character.
* utf8mb3: A UTF-8 encoding of the Unicode character set using one to three bytes per character.

In MySQL `utf8` is currently an alias for `utf8mb3` which **is deprecated** and will be removed in a future MySQL release. At that point `utf8` **will become a reference to** `utf8mb4`.

refer:

* [What is the difference between utf8mb4 and utf8 charsets in MySQL?](https://stackoverflow.com/questions/30074492/what-is-the-difference-between-utf8mb4-and-utf8-charsets-in-mysql)
* [How to support full Unicode in MySQL databases](https://mathiasbynens.be/notes/mysql-utf8mb4)

## INSERT ... ON DUPLICATE KEY UPDATE Statement

If you specify an ON DUPLICATE KEY UPDATE clause and a row to be inserted would cause a duplicate value in a UNIQUE index or PRIMARY KEY, an UPDATE of the old row occurs. For example, if column a is declared as UNIQUE and contains the value 1, the following two statements have similar effect:

``` sql
INSERT INTO t1 (a,b,c) VALUES (1,2,3)
  ON DUPLICATE KEY UPDATE c=c+1;

UPDATE t1 SET c=c+1 WHERE a=1;
```

测试：

``` sql
mysql> desc t_gerry;
+-------+-------------+------+-----+---------+-------+
| Field | Type        | Null | Key | Default | Extra |
+-------+-------------+------+-----+---------+-------+
| id    | int(2)      | YES  | MUL | NULL    |       |
| name  | varchar(64) | YES  | UNI | NULL    |       |
+-------+-------------+------+-----+---------+-------+
2 rows in set (0.00 sec)
mysql> show index from t_gerry;
+---------+------------+-----------+--------------+-------------+-----------+-------------+----------+--------+------+------------+---------+---------------+
| Table   | Non_unique | Key_name  | Seq_in_index | Column_name | Collation | Cardinality | Sub_part | Packed | Null | Index_type | Comment | Index_comment |
+---------+------------+-----------+--------------+-------------+-----------+-------------+----------+--------+------+------------+---------+---------------+
| t_gerry |          0 | Fname_idx |            1 | name        | A         |           2 |     NULL | NULL   | YES  | BTREE      |         |               |
| t_gerry |          1 | Fid_idx   |            1 | id          | A         |           8 |     NULL | NULL   | YES  | BTREE      |         |               |
+---------+------------+-----------+--------------+-------------+-----------+-------------+----------+--------+------+------------+---------+---------------+
2 rows in set (0.00 sec)

mysql> select * from t_gerry;
+------+------+
| id   | name |
+------+------+
|   10 | NULL |
|   11 | 10   |
|   12 | NULL |
|   13 | NULL |
|   14 | NULL |
|   15 | NULL |
|   15 | NULL |
|   10 | NULL |
+------+------+
8 rows in set (0.00 sec)

mysql> insert into t_gerry(id, name) values(11, 10) on duplicate key update name = 20;
Query OK, 2 rows affected (0.00 sec)

mysql> select * from t_gerry;
+------+------+
| id   | name |
+------+------+
|   10 | NULL |
|   11 | 20   |
|   12 | NULL |
|   13 | NULL |
|   14 | NULL |
|   15 | NULL |
|   15 | NULL |
|   10 | NULL |
+------+------+
8 rows in set (0.00 sec)
mysql> insert into t_gerry(name) values(20) on duplicate key update name = 30;
Query OK, 2 rows affected (0.00 sec)

mysql> select * from t_gerry;
+------+------+
| id   | name |
+------+------+
|   10 | NULL |
|   11 | 30   |
|   12 | NULL |
|   13 | NULL |
|   14 | NULL |
|   15 | NULL |
|   15 | NULL |
|   10 | NULL |
+------+------+
10 rows in set (0.00 sec)
mysql> insert into t_gerry(name) values(20) on duplicate key update name = 20;
Query OK, 1 row affected (0.00 sec)

mysql> select * from t_gerry;
+------+------+
| id   | name |
+------+------+
|   10 | NULL |
|   11 | 30   |
|   12 | NULL |
|   13 | NULL |
|   14 | NULL |
|   15 | NULL |
|   15 | NULL |
|   10 | NULL |
|   11 | 10   |
|   13 | NULL |
| NULL | 20   |
+------+------+
11 rows in set (0.00 sec)
```

refer:
* [INSERT ... ON DUPLICATE KEY UPDATE Statement](https://dev.mysql.com/doc/refman/8.0/en/insert-on-duplicate.html)

## 批量操作

By default, [mysql_query()](https://dev.mysql.com/doc/refman/8.0/en/mysql-query.html) and [mysql_real_query()](https://dev.mysql.com/doc/refman/8.0/en/mysql-real-query.html) interpret their statement string argument as **a single statement to be executed**, and you process the result according to whether the statement produces a result set (a set of rows, as for SELECT) or an affected-rows count (as for INSERT, UPDATE, and so forth).

MySQL also supports the execution of a string containing multiple statements separated by semicolon (;) characters. This capability is enabled by special options that are specified either when you connect to the server with mysql_real_connect() or after connecting by calling [mysql_set_server_option()](https://dev.mysql.com/doc/refman/8.0/en/mysql-set-server-option.html).

```
If you enable multiple-statement support, **you should retrieve results from calls to mysql_query() or mysql_real_query() by using a loop that calls [mysql_next_result()](https://dev.mysql.com/doc/refman/8.0/en/mysql-next-result.html) to determine whether there are more results**. 

Enabling multiple-statement support with **MYSQL_OPTION_MULTI_STATEMENTS_ON** does not have quite the same effect as enabling it by passing the **CLIENT_MULTI_STATEMENTS** flag to **mysql_real_connect()**.
```

Executing a multiple-statement string can produce multiple result sets or row-count indicators. Processing these results involves a different approach than for the single-statement case: After handling the result from the first statement, it is necessary to check whether more results exist and process them in turn if so. To support multiple-result processing, the C API includes the [mysql_more_results()](https://dev.mysql.com/doc/refman/8.0/en/mysql-more-results.html) and [mysql_next_result()](https://dev.mysql.com/doc/refman/8.0/en/mysql-next-result.html) functions. These functions are used at the end of a loop that iterates as long as more results are available. Failure to process the result this way may result in a dropped connection to the server.

refer:

* [Multiple Updates in MySQL](https://stackoverflow.com/questions/3432/multiple-updates-in-mysql)
* [PHP- Multiple Statements](https://www.php.net/manual/en/mysqli.quickstart.multiple-statement.php)
* [C API Multiple Statement Execution Support](https://dev.mysql.com/doc/refman/8.0/en/c-api-multiple-queries.html)

## MySQL事务能否嵌套

根据官方[13.3.3 Statements That Cause an Implicit Commit](https://dev.mysql.com/doc/refman/8.0/en/implicit-commit.html)说明，MySQL不支持事务嵌套，若在同一个会话中，第二次执行`START TRANSACTION`开启一个事务，则第一个事务的操作会被隐式提交。

> Transactions cannot be nested. This is a consequence of the implicit commit performed for any current transaction when you issue a START TRANSACTION statement or one of its synonyms.

refer:

* [Are nested transactions allowed in MySQL?](https://stackoverflow.com/questions/1306869/are-nested-transactions-allowed-in-mysql)

## 数据库表记录复制

[Duplicate / Copy records in the same MySQL table](https://stackoverflow.com/questions/729489/duplicate-copy-records-in-the-same-mysql-table)

``` sql
CREATE TEMPORARY TABLE tmp SELECT * FROM invoices WHERE id = 99;
UPDATE tmp SET id=100 WHERE id = 99;
INSERT INTO invoices SELECT * FROM tmp WHERE id = 100;
```

或者

``` sql
create table t1 like t2;
insert into t1 select * from t2;
update t1 set a=xxx;
insert into t2 select * from t1;
```

## 创建DB和库表，更新库表

创建DB：

``` bash
#!/bin/sh

echo ""
echo ""
if [ $# -lt 1 ] 
then
	echo "usage : $0 DBInfo"
	echo "Sample: $0 \"-u$usr -h$ip -P3306\""
	exit 1  
fi

Info=$1

echo "`date +%y-%m-%d-%X`: begin to create" 

printf "
create database $db_xxx;
" | mysql $Info

echo "`date +%y-%m-%d-%X`: end to create" 
```

创建库表，以下为分库分表的方式：

``` bash
#!/bin/sh

echo ""
echo ""
if [ $# -lt 3 ] 
then
    echo "usage : $0 BeginID EndID DBInfo"
    echo "Sample: $0 0 999 \"-uroot -h$ip -P3306\""
    exit 1  
fi

BeginID=$1
EndID=$2
Info=$3

echo "`date +%y-%m-%d-%X`: begin to create" 

printf "
use db_xxx;
" | mysql $Info

iDTable=$BeginID
while test $iDTable -le $EndID; do    
    if [ $iDTable -lt 10 ];then
        iTable=`printf "00%d " $iDTable`
    elif [ $iDTable -lt 100 ];then
        iTable=`printf "0%d " $iDTable`
    else 
        iTable=`printf "%d " $iDTable`
    fi

    printf "
    CREATE TABLE t_xxx_$iTable (
    FOrderId             varchar(50)    NOT NULL COMMENT 'OrderId',
    FUpdateTime          timestamp      NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP COMMENT 'LastModifyTime',
    PRIMARY KEY (FOrderId)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
    " | mysql $Info --default-character-set=utf8 db_xxx 

    echo "create table $iTable "

    iDTable=$(($iDTable+1))
done

echo "`date +%y-%m-%d-%X`: end to create" 
```

更新库表：

``` sql
ALTER TABLE t_xxx ADD Fxxx1 varchar(24) NOT NULL default ''  COMMENT 'xxx' AFTER Fxxx2;
```


## 索引创建，更新和删除

在创建表时指定索引字段（INDEX）：

``` sql
CREATE TABLE t_xxx (
    Fusr_id    varchar(50)    NOT NULL COMMENT 'usr id',
    Fusr_name  varchar(1024)  NOT NULL default '' COMMENT 'usr name',
    PRIMARY KEY (Fusr_id),
    INDEX Fusr_name_idx (Fusr_name)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

创建表后添加索引字段：

``` sql
alter table t_xxx add index Fusr_id_idx(Fusr_id);
```

删除索引字段：

``` sql
alter table t_xxx drop index Fusr_name_idx;
```

查看索引字段查询性能：

```
explain select * from t_xxx where Fusr_name='10001'\G
```

显示表的索引信息：

```
show index from t_xxx;
```

创建唯一索引（UNIQUE INDEX）：

``` sql
CREATE UNIQUE INDEX index_name ON table_name(index_column_1,index_column_2,...);
```

创建唯一性限制（UNIQUE KEY）：

``` sql
CREATE TABLE table_name(
   UNIQUE KEY(index_column_,index_column_2,...)
);

ALTER TABLE table_name ADD CONSTRAINT constraint_name UNIQUE KEY(column_1,column_2,...);
```

**MySQL UNIQUE Index & NULL的关系：**

Unlike other database systems, MySQL considers `NULL` values as `distinct values`. Therefore, you can have multiple NULL values in the `UNIQUE index`. This is how MySQL was designed. It is not a bug even though it was reported as [a bug](http://bugs.mysql.com/bug.php?id=25544).


refer:

* [Using MySQL UNIQUE Index To Prevent Duplicates](http://www.mysqltutorial.org/mysql-unique/)

## 更改字段类型


``` sql
ALTER TABLE tablename MODIFY columnname INTEGER;
```

https://dev.mysql.com/doc/refman/8.0/en/alter-table.html


## 时间函数

```
mysql> select unix_timestamp();
+------------------+
| unix_timestamp() |
+------------------+
|       1404968745 | 
+------------------+
1 row in set (0.00 sec)

mysql> select from_unixtime(1404968745);
+---------------------------+
| from_unixtime(1404968745) |
+---------------------------+
| 2014-07-10 13:05:45       | 
+---------------------------+
1 row in set (0.00 sec)

mysql> select now();
+---------------------+
| now()               |
+---------------------+
| 2014-07-10 13:00:41 | 
+---------------------+
1 row in set (0.00 sec)

mysql> select current_timestamp();
+---------------------+
| current_timestamp() |
+---------------------+
| 2014-07-10 13:00:44 | 
+---------------------+
1 row in set (0.00 sec)

mysql> select localtime();
+---------------------+
| localtime()         |
+---------------------+
| 2014-07-10 13:00:47 | 
+---------------------+
1 row in set (0.00 sec)
```

## MySQL 5.7.8 JSON

JSON data type provides these advantages over storing JSON-format strings in a string column:

* Automatic validation of JSON documents stored in JSON columns. Invalid documents produce an error.
* Optimized storage format. JSON documents stored in JSON columns are converted to an internal format that permits quick read access to document elements. When the server later must read a JSON value stored in this binary format, the value need not be parsed from a text representation. The binary format is structured to enable the server to look up subobjects or nested values directly by key or array index without reading all values before or after them in the document.

The size of JSON documents stored in JSON columns is limited to the value of the `max_allowed_packet` system variable. (While the server manipulates a JSON value internally in memory, it can be larger; the limit applies when the server stores it.)

* [https://dev.mysql.com/doc/refman/5.7/en/json.html](https://dev.mysql.com/doc/refman/5.7/en/json.html)
* [Max JSON column length in MySQL](https://stackoverflow.com/questions/40711101/max-json-column-length-in-mysql)
* [Storing Data in MySQL as JSON](https://stackoverflow.com/questions/3564024/storing-data-in-mysql-as-json)
* [How FriendFeed uses MySQL to store schema-less data](https://backchannel.org/blog/friendfeed-schemaless-mysql)


# 性能优化

## MySQL EXPLAIN

* The EXPLAIN statement provides information about how MySQL executes statements. EXPLAIN works with SELECT, DELETE, INSERT, REPLACE, and UPDATE statements. 
* When you issue a query, the MySQL Query Optimizer tries to devise an optimal plan for query execution. You can see information about the plan by prefixing the query with EXPLAIN.

refer :

* [EXPLAIN Output Format](https://dev.mysql.com/doc/refman/8.0/en/explain-output.html#explain_key)
* [Using EXPLAIN to Write Better MySQL Queries](https://www.sitepoint.com/using-explain-to-write-better-mysql-queries/)


## MySQL查询缓存

原理：Query Cache作用于整个MySQL实例，主要用于缓存MySQL中的ResultSet（一条SQL语句执行的结果集），只能针对select语句。当打开Query Cache功能，MySQL在接收到一条select语句的请求后，如果该语句满足Query Cache的要求，MySQL会直接根据预先设定好的HASH算法将接收到的select语句以字符串方式进行hash，然后到Query Cache中直接查找是否已经缓存。如果已经有结果在缓存中，该select请求就会直接将数据返回，从而省略了后面所有的步骤（如SQL语句的解析，优化器优化，向存储引擎请求数据），从而极大地提高了性能。**但是，当数据变化频繁的情况下，使用Query Cache效果反而会更差**。

query_cache_type (开关)

* 0(OFF): 完全不使用Query Cache
* 1(ON): 除显式要求不使用Query Cache之外的所有select都使用Query Cache
* 2(DEMAND): 只有显式要求才使用Query Cache

几个重要指标：

* Qcache_lowmem_prunes: 值越大，表明经常出现缓冲不够的情况。
* Qcache_hits：表示多少次命中，值越大， 表明查询缓存使用非常频繁，如果该值较小反而会影响效率，则应考虑不使用查询缓存。
* Qcache_inserts: 表示多少次未命中然后插入。

> Query Cache命中率 = Qcache_hits / (Qcache_hits + Qcache_inserts)

```
mysql> show variables like '%query_cache%';
+------------------------------+---------+
| Variable_name                | Value   |
+------------------------------+---------+
| have_query_cache             | YES     |
| query_cache_limit            | 1048576 |
| query_cache_min_res_unit     | 4096    |
| query_cache_size             | 0       |
| query_cache_strip_comments   | OFF     |
| query_cache_type             | OFF     |
| query_cache_wlock_invalidate | OFF     |
+------------------------------+---------+

mysql> show status like 'Qcache%';
+-------------------------+-------+
| Variable_name           | Value |
+-------------------------+-------+
| Qcache_free_blocks      | 0     |
| Qcache_free_memory      | 0     |
| Qcache_hits             | 0     |
| Qcache_inserts          | 0     |
| Qcache_lowmem_prunes    | 0     |
| Qcache_not_cached       | 0     |
| Qcache_queries_in_cache | 0     |
| Qcache_total_blocks     | 0     |
+-------------------------+-------+
```

## InnoDB的缓存性能

当使用InnoDB存储引擎时，`innodb_buffer_pool_size`是影响性能的关键参数，用来设置用于缓存InnoDB索引及数据块的内存区域大小。即，当操作一个InnoDB表的时候，返回的所有数据或者查询过程中用到的任何一个索引块，都会在这个内存区域中查询一遍。如果有足够的内存，尽可能将该参数设置到足够大，将尽可能多的InnoDB的索引及数据都放到该缓冲区中。

```
mysql> show variables like '%innodb_buffer%';
+-------------------------------------+----------------+
| Variable_name                       | Value          |
+-------------------------------------+----------------+
| innodb_buffer_pool_dump_at_shutdown | OFF            |
| innodb_buffer_pool_dump_now         | OFF            |
| innodb_buffer_pool_filename         | ib_buffer_pool |
| innodb_buffer_pool_instances        | 64             |
| innodb_buffer_pool_load_abort       | OFF            |
| innodb_buffer_pool_load_at_startup  | OFF            |
| innodb_buffer_pool_load_now         | OFF            |
| innodb_buffer_pool_populate         | OFF            |
| innodb_buffer_pool_size             | 1073741824     |
+-------------------------------------+----------------+
```


# 代码实践

## C++封装的MySQL操作

[https://github.com/gerryyang/mac-utils/tree/master/tools/MySQL](https://github.com/gerryyang/mac-utils/tree/master/tools/MySQL)


# 官方API

* [MySQL 8.0 Reference Manual](https://dev.mysql.com/doc/refman/8.0/en/introduction.html)


# Refer

* [Install MySQL on Mac OS X 10.9.1(x86, 64-bit)](http://blog.csdn.net/delphiwcdj/article/details/19297283)
* [MySQL客户端2006(CR_SERVER_GONE_ERROR)错误测试和优化方法](http://blog.csdn.net/delphiwcdj/article/details/41576615)
* [数据库：Mysql中“select ... for update”排他锁分析](https://blog.csdn.net/claram/article/details/54023216)

