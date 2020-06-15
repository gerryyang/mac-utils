---
layout: post
title:  "MySQL online DDL原理"
date:   2018-09-28 09:15:00 +0800
categories: MySQL
---

* Do not remove this line (it will not be displayed)
{:toc}

MySQL online DDL(Data Definition Language)主要解决的问题是，满足在不中断数据库服务的情况下进行DDL操作。现有的方案有：

* 使用原生的MySQL online DDL
* 在从库上修改表结构，主从切换
* 使用第三方工具

在`MySQL 5.5`之前的版本其DDL的实现方式是：

1. 按照原表的定义创建一个新的临时表
2. 对原表加`写锁`
3. 对新的临时表进行修改
4. 将原表中的数据逐行复制到新表中
5. 释放原表的`写锁`
6. 将旧表删除，并将新的临时表重命名

此方案存在的问题：

* 复制数据的过程需要耗费额外的存储空间，并且执行过程时耗较长
* 复制数据的过程需要`写锁`，无法持续对外提供服务

## MySQL 5.7 online DDL

``` sql
ALTER TABLE testdb.testtable ADD COLUMN _new_column 
ALGORITHM = inplace,LOCK = default;
```

`ALGORITHM`子句用来指定执行DDL所采用的方式，取值为`{DEFAULT|INPLACE|COPY}`

* ALGORITHM = COPY  
	+ 执行DDL的过程中采用表拷贝的方式进行，过程中会阻塞所有的DML
* ALGORITHM = INPLACE 
	+ 执行DDL的过程中不发生表拷贝，过程中允许并发执行DML
* ALGORITHM = DEFAULT 
	+ 默认选项，MYSQL会自动选择最优的执行方式，原则是尽量保证DML的并发操作


`LOCK`子句描述持有的锁的类型来控制DML(Data Manipulation Language)的并发，取值`{DEFAULT|NONE|SHARED|EXCLUSIVE}`

* LOCK = EXCLUSIVE 
	+ 持有排它锁，阻塞所有的请求
* LOCK = SHARED 
	+ 允许SELECT，但是阻塞INSERT UPDATA DELETE
* LOCK = NONE 
	+ 不对表加锁，允许所有请求
* LOCK = DEFAULT 
	+ 根据DDL的类型，在保证最大并发的原则下来选择LOCK的取值

## MySQL 5.7 online DDL的实现原理

![mysql-5-7-online-ddl1](/assets/images/201809/mysql-5-7-online-ddl1.png)

![mysql-5-7-online-ddl2](/assets/images/201809/mysql-5-7-online-ddl2.png)

依然存在的问题：

* 在原表中仍然存在排他锁，有锁等待的风险
* 增量日志大小是有限制的 (innodb_online_alter_log_max_size) 
* 有可能造成较大的主备延迟 (Bug#73196) 
* 无法暂停

## PT-OLINE-SCHEMA-CHANGE

使用`Percona-toolkit`第三方工具。

* 支持并发DML操作
* 经过多年生产环境验证，较为可靠

原理：

1. 创建一张新表，表结构与旧表相同
2. alter新表
3. 在原表上创建INSERT, UPDATE, DELETE三种类型的触发器
4. 将旧表的数据拷贝到新表中，同时通过触发器将旧表中的操作映射到新表
5. 如果原表有外键约束，处理外键
6. 原表重命名为old表，new表重命名为原表，整个过程为原子操作
7. 删除old表(默认) 

## GH-OST

优点：

* 无触发器设计
* 切换方案的设计
* 最大限度的减少对主机的影响

## 最佳实践

![mysql-5-7-online-ddl3](/assets/images/201809/mysql-5-7-online-ddl3.png)

![mysql-5-7-online-ddl4](/assets/images/201809/mysql-5-7-online-ddl4.png)

1. `MYSQL5.6以下`的版本，建议使用第三方工具进行DDL
2. `MYSQL5.6及以上`的版本，除了修改索引、外键、列名时，优先采用online DDL，并指定`ALGORITHM = INPLACE`，其他的均采用PT-OSC


# Refer

1. [The JSON Data Type]
2. [mysql 5.6 原生Online DDL解析]
3. [GitHub's online schema migration for MySQL]

[The JSON Data Type]: https://dev.mysql.com/doc/refman/8.0/en/json.html

[mysql 5.6 原生Online DDL解析]: http://seanlook.com/2016/05/24/mysql-online-ddl-concept/

[GitHub's online schema migration for MySQL]: https://github.com/github/gh-ost