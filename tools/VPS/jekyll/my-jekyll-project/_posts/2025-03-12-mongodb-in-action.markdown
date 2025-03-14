---
layout: post
title:  "MongoDB in Action"
date:   2025-03-12 12:30:00 +0800
categories: 数据库
---

* Do not remove this line (it will not be displayed)
{:toc}


# MongoDB Shell (mongosh)

MongoDB Shell (mongosh) 是一个 JavaScript 和 Node.js REPL 环境，用于与 [Atlas](https://www.mongodb.com/zh-cn/docs/atlas/)、本地或其他远程主机上的 MongoDB 部署进行交互。使用 MongoDB Shell 测试查询，并与 MongoDB 数据库中的数据进行交互。

连接字符串示例：

* 无认证：`mongodb://localhost:27017/mydb`
* 带认证：`mongodb://username:password@localhost:27017/mydb?authSource=admin`


# Tips

## 基本数据库操作

* 查看所有数据库

``` javascript
show dbs
```

* 切换/创建数据库

``` javascript
use mydb  // 切换到 mydb（若不存在则创建）
```

* 删除当前数据库

``` javascript
db.dropDatabase()  // 谨慎操作
```

## 集合（表）操作

* 创建集合 (表)

``` javascript
db.createCollection("users")  // 显式创建集合
```

* 查看所有集合

``` javascript
show collections
```

* 删除集合

``` javascript
db.users.drop()  // 删除 users 集合
```

## 文档（数据）CRUD 操作

* 插入文档

``` javascript
// 插入单条文档
db.users.insertOne({
  name: "Alice",
  age: 28,
  email: "alice@example.com",
  tags: ["dev", "admin"]
})

// 插入多条文档
db.users.insertMany([
  { name: "Bob", age: 32 },
  { name: "Charlie", age: 25 }
])
```

* 查询文档

``` javascript
// 查询所有文档
db.users.find()

// 条件查询（age > 30）
db.users.find({ age: { $gt: 30 } })

// 限制返回字段（只显示 name 和 email）
db.users.find({}, { name: 1, email: 1, _id: 0 })

// 排序（按 age 降序）
db.users.find().sort({ age: -1 })

// 分页查询（跳过前2条，限制返回5条）
db.users.find().skip(2).limit(5)
```

* 更新文档

``` javascript
// 更新单条文档（将 Alice 的 age 改为 29）
db.users.updateOne(
  { name: "Alice" },
  { $set: { age: 29 } }
)

// 更新多条文档（所有 age < 30 的添加新字段）
db.users.updateMany(
  { age: { $lt: 30 } },
  { $set: { status: "junior" } }
)
```

* 删除文档

``` javascript
// 删除单条文档（name 为 Bob）
db.users.deleteOne({ name: "Bob" })

// 删除多条文档（age < 25）
db.users.deleteMany({ age: { $lt: 25 } })
```

## 用户与权限管理

* 创建用户

``` javascript
use admin
db.createUser({
  user: "admin",
  pwd: "your_password",
  roles: [ { role: "root", db: "admin" } ]
})
```

* 查看用户

``` javascript
db.getUsers()  // 查看当前数据库的用户
```

* 修改用户权限

``` javascript
db.updateUser("admin", {
  roles: [ { role: "readWriteAnyDatabase", db: "admin" } ]
})
```

## 索引管理

* 创建索引

``` javascript
// 单字段索引（按 age 升序）
db.users.createIndex({ age: 1 })

// 复合索引（name 升序 + email 降序）
db.users.createIndex({ name: 1, email: -1 })

// 唯一索引（确保 email 唯一）
db.users.createIndex({ email: 1 }, { unique: true })
```

* 查看索引

``` javascript
db.users.getIndexes()
```

* 删除索引

``` javascript
db.users.dropIndex("age_1")  // 指定索引名称
```

## 聚合与统计

* 统计文档数量

``` javascript
db.users.countDocuments({ age: { $gt: 25 } })
```

* 聚合操作（按年龄分组统计）

``` javascript
db.users.aggregate([
  { $group: { _id: "$age", total: { $sum: 1 } } },
  { $sort: { _id: 1 } }
])
```


## 数据备份与恢复

* 备份数据库

``` bash
mongodump --uri="mongodb://username:password@localhost:27017/mydb" --out=/backup/
```

* 恢复数据库

``` bash
mongorestore --uri="mongodb://username:password@localhost:27017/mydb" /backup/mydb/
```


## 其他实用命令

* 查看数据库状态

``` javascript
db.stats()  // 显示当前数据库的存储状态
```

```
cmgo-otma0539_0 [primary] test_db> db.stats()
{
  db: 'test_db',
  collections: 2,
  views: 0,
  objects: 819,
  avgObjSize: 563.4615384615385,
  dataSize: 461475,
  storageSize: 245760,
  numExtents: 0,
  indexes: 2,
  indexSize: 98304,
  fsUsedSize: 4002519097344,
  fsTotalSize: 15238215393280,
  ok: 1,
  operationTime: Timestamp({ t: 1741766720, i: 71 }),
  '$clusterTime': {
    clusterTime: Timestamp({ t: 1741766720, i: 71 }),
    signature: {
      hash: Binary.createFromBase64('sGY55RSL5TbHLP7XIRr318xM9qw=', 0),
      keyId: Long('7461626217821635101')
    }
  }
}
```

* 查看集合状态

``` javascript
db.users.stats()
```

```
cmgo-otma0539_0 [primary] test_db> db.ImageVersion.stats()
{
  ok: 1,
  capped: false,
  wiredTiger: {
    metadata: { formatVersion: 1 },
    creationString: 'access_pattern_hint=none,allocation_size=4KB,app_metadata=(formatVersion=1),assert=(commit_timestamp=none,read_timestamp=none),block_allocation=best,block_compressor=snappy,cache_resident=false,checksum=on,colgroups=,collator=,columns=,dictionary=0,encryption=(keyid=,name=),exclusive=false,extractor=,format=btree,huffman_key=,huffman_value=,ignore_in_memory_cache_size=false,immutable=false,internal_item_max=0,internal_key_max=0,internal_key_truncate=true,internal_page_max=4KB,key_format=q,key_gap=10,leaf_item_max=0,leaf_key_max=0,leaf_page_max=32KB,leaf_value_max=64MB,log=(enabled=false),lsm=(auto_throttle=true,bloom=true,bloom_bit_count=16,bloom_config=,bloom_hash_count=8,bloom_oldest=false,chunk_count_limit=0,chunk_max=5GB,chunk_size=10MB,merge_custom=(prefix=,start_generation=0,suffix=),merge_max=15,merge_min=0),memory_page_image_max=0,memory_page_max=10m,os_cache_dirty_max=0,os_cache_max=0,prefix_compression=false,prefix_compression_min=4,source=,split_deepen_min_child=0,split_deepen_per_child=0,split_pct=90,type=file,value_format=u',
    type: 'file',
    uri: 'statistics:table:test_db/collection-772-4053121651813767989',
    LSM: {
      'bloom filter false positives': 0,
      'bloom filter hits': 0,
      'bloom filter misses': 0,
      'bloom filter pages evicted from cache': 0,
      'bloom filter pages read into cache': 0,
      'bloom filters in the LSM tree': 0,
      'chunks in the LSM tree': 0,
      'highest merge generation in the LSM tree': 0,
      'queries that could have benefited from a Bloom filter that did not exist': 0,
      'sleep for LSM checkpoint throttle': 0,
      'sleep for LSM merge throttle': 0,
      'total size of bloom filters': 0
    },
    'block-manager': {
      'allocations requiring file extension': 450,
      'blocks allocated': 2831,
      'blocks freed': 682,
      'checkpoint size': 139264,
      'file allocation unit size': 4096,
      'file bytes available for reuse': 20480,
      'file magic number': 120897,
      'file major version number': 1,
      'file size in bytes': 176128,
      'minor version number': 0
    },
    btree: {
      'btree checkpoint generation': 907737,
      'column-store fixed-size leaf pages': 0,
      'column-store internal pages': 0,
      'column-store variable-size RLE encoded values': 0,
      'column-store variable-size deleted values': 0,
      'column-store variable-size leaf pages': 0,
      'fixed-record size': 0,
      'maximum internal page key size': 368,
      'maximum internal page size': 4096,
      'maximum leaf page key size': 2867,
      'maximum leaf page size': 32768,
      'maximum leaf page value size': 67108864,
      'maximum tree depth': 3,
      'number of key/value pairs': 0,
      'overflow pages': 0,
      'pages rewritten by compaction': 0,
      'row-store internal pages': 0,
      'row-store leaf pages': 0
    },
    cache: {
      'bytes currently in the cache': 451401,
      'bytes dirty in the cache cumulative': 20648571,
      'bytes read into cache': 241164923,
      'bytes written from cache': 17434807,
      'checkpoint blocked page eviction': 0,
      'data source pages selected for eviction unable to be evicted': 3,
      'eviction walk passes of a file': 16151689,
      'eviction walk target pages histogram - 0-9': 16108092,
      'eviction walk target pages histogram - 10-31': 43597,
      'eviction walk target pages histogram - 128 and higher': 0,
      'eviction walk target pages histogram - 32-63': 0,
      'eviction walk target pages histogram - 64-128': 0,
      'eviction walks abandoned': 1284,
      'eviction walks gave up because they restarted their walk twice': 16149748,
      'eviction walks gave up because they saw too many pages and found no candidates': 83,
      'eviction walks gave up because they saw too many pages and found too few candidates': 4,
      'eviction walks reached end of tree': 32300892,
      'eviction walks started from root of tree': 16151330,
      'eviction walks started from saved location in tree': 359,
      'hazard pointer blocked page eviction': 3,
      'in-memory page passed criteria to be split': 0,
      'in-memory page splits': 0,
      'internal pages evicted': 0,
      'internal pages split during eviction': 0,
      'leaf pages split during eviction': 24,
      'modified pages evicted': 650,
      'overflow pages read into cache': 0,
      'page split during eviction deepened the tree': 0,
      'page written requiring cache overflow records': 0,
      'pages read into cache': 14297,
      'pages read into cache after truncate': 0,
      'pages read into cache after truncate in prepare state': 0,
      'pages read into cache requiring cache overflow entries': 0,
      'pages requested from the cache': 404130,
      'pages seen by eviction walk': 16181791,
      'pages written from cache': 1414,
      'pages written requiring in-memory restoration': 0,
      'tracked dirty bytes in the cache': 0,
      'unmodified pages evicted': 13627
    },
    cache_walk: {
      'Average difference between current eviction generation when the page was last considered': 0,
      'Average on-disk page image size seen': 0,
      'Average time in cache for pages that have been visited by the eviction server': 0,
      'Average time in cache for pages that have not been visited by the eviction server': 0,
      'Clean pages currently in cache': 0,
      'Current eviction generation': 0,
      'Dirty pages currently in cache': 0,
      'Entries in the root page': 0,
      'Internal pages currently in cache': 0,
      'Leaf pages currently in cache': 0,
      'Maximum difference between current eviction generation when the page was last considered': 0,
      'Maximum page size seen': 0,
      'Minimum on-disk page image size seen': 0,
      'Number of pages never visited by eviction server': 0,
      'On-disk page image sizes smaller than a single allocation unit': 0,
      'Pages created in memory and never written': 0,
      'Pages currently queued for eviction': 0,
      'Pages that could not be queued for eviction': 0,
      'Refs skipped during cache traversal': 0,
      'Size of the root page': 0,
      'Total number of pages currently in cache': 0
    },
    compression: {
      'compressed pages read': 14296,
      'compressed pages written': 706,
      'page written failed to compress': 0,
      'page written was too small to compress': 708
    },
    cursor: {
      'bulk-loaded cursor-insert calls': 0,
      'close calls that result in cache': 0,
      'create calls': 886,
      'cursor operation restarted': 0,
      'cursor-insert key and value bytes inserted': 381421,
      'cursor-remove key bytes removed': 0,
      'cursor-update value bytes updated': 0,
      'cursors reused from cache': 24008,
      'insert calls': 681,
      'modify calls': 0,
      'next calls': 9810562,
      'open cursor count': 0,
      'prev calls': 1,
      'remove calls': 0,
      'reserve calls': 0,
      'reset calls': 117523,
      'search calls': 0,
      'search near calls': 67705,
      'truncate calls': 0,
      'update calls': 0
    },
    reconciliation: {
      'dictionary matches': 0,
      'fast-path pages deleted': 0,
      'internal page key bytes discarded using suffix compression': 883,
      'internal page multi-block writes': 0,
      'internal-page overflow keys': 0,
      'leaf page key bytes discarded using prefix compression': 0,
      'leaf page multi-block writes': 25,
      'leaf-page overflow keys': 0,
      'maximum blocks required for a page': 1,
      'overflow values written': 0,
      'page checksum matches': 1,
      'page reconciliation calls': 1390,
      'page reconciliation calls for eviction': 9,
      'pages deleted': 0
    },
    session: { 'object compaction': 0 },
    transaction: { 'update conflicts': 0 }
  },
  sharded: false,
  size: 402696,
  count: 720,
  storageSize: 176128,
  totalIndexSize: 61440,
  indexSizes: { _id_: 61440 },
  avgObjSize: 559,
  ns: 'test_db.ImageVersion',
  nindexes: 1
}
```

* 清空集合（保留结构）

``` javascript
db.users.deleteMany({})  // 删除所有文档
```




# Refer

* https://www.mongodb.com/zh-cn/docs/mongodb-shell/
* https://www.mongodb.com/zh-cn/docs/manual/installation/
* https://www.mongodb.com/zh-cn/docs/mongodb-shell/crud/#std-label-mdb-shell-crud










