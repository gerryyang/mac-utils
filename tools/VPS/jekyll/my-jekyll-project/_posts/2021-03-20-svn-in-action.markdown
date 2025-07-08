---
layout: post
title:  "SVN in Action"
date:   2021-03-20 20:00:00 +0800
categories: 版本控制
---

* Do not remove this line (it will not be displayed)
{:toc}


# 基础概念

* 仓库 (`Repository`)：中央代码库，存储所有版本历史和分支
* 工作副本 (`Working Copy`)：本地目录，从仓库检出（Checkout）的代码副本
* 核心操作流程：
  + 检出 (Checkout)：首次下载代码到本地。
  + 更新 (Update)：同步远程最新代码到本地。
  + 提交 (Commit)：将本地修改推送至仓库。





# Setup for Visual Studio Code

在 vscode 中安装作者为 Chris Johnston 的 SVN 扩展插件。在 `.vscode/settings.json` 配置中添加 `svn.path` 路径配置，然后输入 `Command+Shift+P` 执行 `Reload Window` 使配置修改生效。当 vscode 加载配置生效后，可以在左边栏看到 "S" 图标，随后可以添加需要的 SVN URL 仓库地址。

``` json
    // Path to the svn executable
    "svn.path": "/usr/bin/svn",
```


参考 [Configure SVN repo url in Visual Studio Code](https://stackoverflow.com/questions/52978849/configure-svn-repo-url-in-visual-studio-code) 中的步骤 checkout 所需的代码分支到本地目录。

* Press `Command+Shift+P`
* Type and chose `SVN: Checkout`. Press `Enter`
* Input existing URL of your repository e.g. `file:///C:/Users/user1/Repo/my_scripts`. Press `Enter`
* Input parent directory for your Work copy e.g. `C:\Users\user1\Documents`. Press `Enter`
* Input name of directory your Work copy e.g. `"my_scripts"`. Press `Enter`


# SVN 提交代码的完整流程

1. 检出代码：从 SVN 仓库中检出最新的代码版本到本地工作目录。
2. 更新代码：使用 `svn update` 命令同步本地代码与仓库中的最新版本，解决可能的冲突。
3. 修改代码：在本地工作目录中进行代码编写和修改。
4. 添加文件：使用 `svn add` 命令将新增的文件添加到版本控制中。
5. 查看状态：使用 `svn status` 命令查看工作副本中的改动。
6. 提交代码：使用 `svn commit` 命令将修改后的代码提交到 SVN 仓库，并填写详细的提交日志，描述本次提交的目的和修改内容。
7. 解决冲突：如果提交过程中出现冲突，需要先解决冲突，然后重新提交。
8. 通知团队：建议在提交前通知项目组成员，避免冲突。

以上步骤确保了代码的一致性和版本控制的规范性。

> 注意：`svn commit` 命令本身就会将本地更改直接推送到远程服务器，**不需要额外的推送步骤（这与 Git 的工作方式不同）**。**SVN 的 commit = Git 的 commit + push**，这是集中式版本控制系统的特点。


确认是否能获取到自己的提交：

``` bash
svn update
svn log -v -l 1  # 查看最新日志是否包含你的提交
```

# SVN 处理代码冲突的方法

1. 命令行方法：使用 `svn resolve` 命令指定接受的版本，如`mc`（mine-conflict）、`tc`（theirs-conflict）等。
2. 弹窗页面方法：通过右键菜单处理冲突，选择保留本地修改或服务器版本。
3. 手动解决方法：在编译器中删除冲突文件中的正常代码和同名冲突后缀名文件，或使用文本编辑器手动编辑冲突文件。
4. 使用比较工具：使用 DiffMerge 或 WinMerge 等工具可视化差异，辅助解决冲突。
5. 自动合并：使用 `svn merge` 工具尝试自动合并不同版本代码，但需仔细检查避免遗漏或重复。
6. 标记冲突解决：完成冲突解决后，使用 `svn resolved [文件路径]` 命令标记文件已解决。
7. 提交更改：使用 `svn commit -m "解决冲突"` 命令提交更改。

此外，预防冲突的策略包括频繁更新代码、小而频繁的提交以及明确分工。


更新冲突时，命令行提示可选操作：

* mc：强制采用本地修改（覆盖服务器代码）。
* tc：强制采用服务器代码（丢弃本地修改）。

```
Select: (p) postpone, (df) diff-full, (e) edit,
        (mc) mine-conflict, (tc) theirs-conflict
```



# SVN 合并分支的操作步骤

* 创建分支：首先需要创建一个分支，可以使用 `svn copy` 命令将主干（trunk）复制到分支目录中。例如：

``` bash
svn copy http://svn.example.com/repo/trunk http://svn.example.com/repo/branches/my_branch -m "Creating a new branch"
```

* 切换到分支：在本地工作副本中切换到新创建的分支，以便进行开发和修改。可以使用 `svn switch` 命令切换到分支：

``` bash
svn switch http://svn.example.com/repo/branches/my_branch
```

* 修改分支内容：在分支上进行代码修改，并提交更改到服务器。例如：

``` bash
svn commit -m "My Changes"
```

* 合并分支到主干：当分支上的修改完成后，需要将分支的更改合并到主干（trunk）。可以使用 `svn merge` 命令进行合并。例如：

``` bash
svn merge http://svn.example.com/repo/branches/my_branch
```

* 解决冲突：如果合并过程中出现冲突，需要手动解决冲突文件。在本地编辑器中编辑冲突文件，然后使用 `svn resolve` 命令标记冲突已解决：

``` bash
svn resolve --accept=working path/to/conflict/file
```

* 提交合并结果：解决冲突后，将合并后的更改提交到服务器。例如：

``` bash
svn commit -m "Merged changes from my_branch to trunk"
```

* 验证合并结果：确保合并后的代码在主干中正确无误，可以通过更新主干工作副本并检查代码来验证。

通过以上步骤，可以完成 SVN 中的分支创建和合并操作。需要注意的是，在合并过程中要选择合适的合并范围和目标，以确保预期的合并结果。



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

## 基础操作

``` bash
# 查看仓库信息
svn info

# 查看状态 (Status)
svn status
# 或简写为
svn st
svn st | grep '^C'

# 检出代码 (Checkout)
svn checkout <repository_url> [local_directory]
# 或简写为
svn co <repository_url> [local_directory]

# 更新本地代码 (Update)
# 拉取最新代码，该命令会从远程仓库拉取最新的代码更改，并将其合并到本地工作副本中
# 如果本地工作副本中有未提交的更改，svn update 命令是否会自动合并这些更改？
# 如果本地工作副本中有未提交的更改，svn update 命令会尝试将服务器上的最新更改与本地的未提交更改进行合并。如果合并成功，本地文件将包含最新的更改；如果合并失败，Subversion 会报告冲突，用户需要手动解决冲突
svn update
# 或简写为
svn up


```

## 分支操作

``` bash
# 创建分支
svn copy <source_url> <branch_url> -m "创建分支说明"

# 例如
svn copy http://svn.example.com/trunk http://svn.example.com/branches/feature1 -m "创建feature1分支"

# 创建全新的工作副本
# 适用于首次获取代码或创建全新工作环境
# 不保留任何现有状态
# 需要完整下载指定路径内容
svn checkout <branch_url>

# 切换分支
# 适用于分支间切换
# 保留本地未提交的修改（会自动尝试合并）
# 不需要重新下载整个代码库
# 更新 .svn 元数据指向新位置
svn switch <新分支URL>

# 例如：从 trunk 切换到 feature 分支
svn switch http://svn.example.com/branches/feature-branch

# 合并分支到主干
# 切换到主干目录后执行
svn merge <branch_url>
```

## 提交更改

``` bash
# 添加新文件
svn add <filename>
svn add --force *

# 删除文件
svn delete <filename>
# 或简写为
svn del <filename>

# 提交更改
svn commit -m "提交说明"
# 或简写为
svn ci -m "提交说明"
```



## 解决冲突

``` bash
# 问题类型：系统锁定/损坏
# 触发条件：操作中断或异常
# 前置操作：无
# 结果：恢复工作副本可用状态
svn cleanup

# 问题类型：文件内容冲突
# 触发条件：合并/更新导致文件冲突
# 前置操作：需要先手动编辑解决文件冲突内容
# 结果：标记冲突为已解决
svn resolved <filename>
```

## 查看文件差异

``` bash
# 查看 diff
svn diff
```

## 撤销本地修改

``` bash
svn revert <filename>
svn revert . -R
```


## 查看文件每一行最后修改信息

目的：

`svn blame` 命令是代码审查和问题追踪的强大工具，可以帮助快速定位特定代码变更的来源。

注意：

1. 对于二进制文件，blame 命令可能没有意义
2. 在大文件上执行可能会比较耗时
3. 结果中的日期是提交日期，不是修改日期
4. 如果文件被移动/重命名过，需要使用 --stop-on-copy 参数查看完整历史

``` bash
svn blame -v filename.txt
```

输出格式：

```
  修订号 用户名      日期                 代码行
  12345  user1   2023-05-01 10:30:45  // 这是第一行代码
  12346  user2   2023-05-02 14:15:22  // 这是第二行代码
```

``` bash
# 追踪代码变更来源
svn blame -v utils.py | grep "problem_function"

# 找出特定时间段的修改
svn blame -r {2023-01-01}:{2023-06-30} config.ini

# 生成代码贡献报告
svn blame src/ | awk '{print $2}' | sort | uniq -c | sort -nr
```



## 查看日志

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



# Q&A


## 问题修复和代码合并 (TortoiseSVN)

例如：需要修复发布线 publish_v1 中的问题，先在 trunk 线提交修复代码，然后 checkout publish_v1 的副本到本地，在 publish_v1 的本地副本中，TortoiseSVN -> Merge... ，启动合并向导，选择 Merge a range of revisions，通过 Show log 选择在 truck 线的提交记录，最后选择 Merge 。如果没有冲突，再在 publish_v1 本地副本提交修改，即完成了 trunk 到 publish_v1 代码的合并。


# Refer

* https://tortoisesvn.net/docs/nightly/TortoiseSVN_zh_CN/tsvn-dug-merge.html
* http://svnbook.red-bean.com/en/1.7/svn.ref.svn.c.merge.html
* https://ericsink.com/vcbe/vcbe_usletter_lo.pdf
* https://stackoverflow.com/questions/28035602/resolving-a-merge-conflict-when-i-do-svn-update

