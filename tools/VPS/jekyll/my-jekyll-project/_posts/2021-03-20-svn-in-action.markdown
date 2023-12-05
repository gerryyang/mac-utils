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

# 问题修复和代码合并

例如：需要修复发布线 publish_v1 中的问题，先在 trunk 线提交修复代码，然后 checkout publish_v1 的副本到本地，在 publish_v1 的本地副本中，TortoiseSVN -> Merge... ，启动合并向导，选择 Merge a range of revisions，通过 Show log 选择在 truck 线的提交记录，最后选择 Merge 。如果没有冲突，再在 publish_v1 本地副本提交修改，即完成了 trunk 到 publish_v1 代码的合并。


# Refer

* https://tortoisesvn.net/docs/nightly/TortoiseSVN_zh_CN/tsvn-dug-merge.html
* http://svnbook.red-bean.com/en/1.7/svn.ref.svn.c.merge.html
* https://ericsink.com/vcbe/vcbe_usletter_lo.pdf
* https://stackoverflow.com/questions/28035602/resolving-a-merge-conflict-when-i-do-svn-update

