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


# 实践之坑

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

# 性能优化

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
