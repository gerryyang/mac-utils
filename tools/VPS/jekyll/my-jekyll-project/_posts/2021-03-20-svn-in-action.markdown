---
layout: post
title:  "SVN in Action"
date:   2021-03-20 20:00:00 +0800
categories: 版本控制
---

* Do not remove this line (it will not be displayed)
{:toc}

# SVN 状态

```
A  已添加
D  已删除
U  已更新
C  合并冲突
G  合并成功
E  已存在
R  已替换
```

# 常用命令

```

# 查看版本信息
svn info

# 检出工程代码到本地
svn checkout http://xxx your_proj_name

# 更新代码
svn cleanup
svn update

# 查看状态
svn status -u

# 查看 diff
svn diff

# 添加新的文件
svn add --force *

svn st | grep '^C'

svn revert . -R
```

## 日志相关

与 [How do you see recent SVN log entries?](https://stackoverflow.com/questions/4881129/how-do-you-see-recent-svn-log-entries) 反馈的问题感受一样，如果执行 `svn log` 命令默认会输出所有 svn 的历史提交记录到终端且不会暂停分页输出，交互体验比较差。如果要分页显示需要执行 `svn log | less`。或者通过 `--limit` 选项限制查询输出的记录条数，例如：查询最近 4 条历史提交记录 `svn log --limit 4` 或 `svn log -l 4`。

``` bash
`-l [--limit] ARG`         : maximum number of log entries
```

另外可以根据 `revision` 查询一个返回的历史提交记录。

``` bash
`-r [--revision] ARG`      : ARG (some commands also take ARG1:ARG2 range)
                             A revision argument can be one of:
                                NUMBER       revision number
                                '{' DATE '}' revision at start of the date
                                'HEAD'       latest in repository
                                'BASE'       base rev of item's working copy
                                'COMMITTED'  last commit at or before BASE
                                'PREV'       revision just before COMMITTED
```

使用示例：

``` bash
# To list everything in ascending order
svn log -r 1:HEAD

# To list everything in descending order
svn log -r HEAD:1

# To list everything from the thirteenth to the base of the currently checked-out revision in ascending order
svn log -r 13:BASE

# To get everything between the given dates
svn log -r {2011-02-02}:{2011-02-03}
```

You can combine all the above expressions with the `--limit` option, so that can you have a quite granular control over what is printed. For more info about these `-r` expressions refer to `svn help log` or the relevant chapter in the book [Version Control with Subversion](http://svnbook.red-bean.com/en/1.4/svn.tour.revs.specifiers.html)


I like to use `-v` for **verbose** mode. It'll give you the commit id, comments and all affected files.

``` bash
svn log -v --limit 4
```

Example of output:

```
I added some migrations and deleted a test xml file
------------------------------------------------------------------------
r58687 | mr_x | 2012-04-02 15:31:31 +0200 (Mon, 02 Apr 2012) | 1 line Changed
paths:
A /trunk/java/App/src/database/support
A /trunk/java/App/src/database/support/MIGRATE
A /trunk/java/App/src/database/support/MIGRATE/remove_device.sql
D /trunk/java/App/src/code/test.xml
```

查看当前代码到最新提交之间的历史记录：

In case anybody is looking at this old question, a handy command to see the changes since your last update:

``` bash
svn log -r BASE:HEAD -v

# or
svn log -r $(svn info | grep Revision | cut -f 2 -d ' '):HEAD -v
```




# 问题修复和代码合并

例如：需要修复发布线 publish_v1 中的问题，先在 trunk 线提交修复代码，然后 checkout publish_v1 的副本到本地，在 publish_v1 的本地副本中，TortoiseSVN -> Merge... ，启动合并向导，选择 Merge a range of revisions，通过 Show log 选择在 truck 线的提交记录，最后选择 Merge 。如果没有冲突，再在 publish_v1 本地副本提交修改，即完成了 trunk 到 publish_v1 代码的合并。


# Refer

* https://tortoisesvn.net/docs/nightly/TortoiseSVN_zh_CN/tsvn-dug-merge.html
* http://svnbook.red-bean.com/en/1.7/svn.ref.svn.c.merge.html
* https://ericsink.com/vcbe/vcbe_usletter_lo.pdf
* https://stackoverflow.com/questions/28035602/resolving-a-merge-conflict-when-i-do-svn-update

