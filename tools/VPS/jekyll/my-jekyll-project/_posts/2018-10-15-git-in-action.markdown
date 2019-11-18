---
layout: post
title:  "Git in Action"
date:   2018-10-15 13:00:00 +0800
categories: tools
---

* Do not remove this line (it will not be displayed)
{:toc}

Linux的创始人`Linus Torvalds`在2005年开发了[Git]的原型程序，主要用于对**Linux内核开发的版本管理**。作为开发者日常离不开对代码的管理，Git采用了**分布式版本库**的方式，使得开发者更加自由，随时随地在本地可以管理自己的代码，而不像**集中式的版本控制工具**(`VCSs`)，例如`CVS`，`Subversion`，代码管理需要依赖网络。使用Git让源代码的管理，以及团队间的协作更为高效，这也是为什么越来越多的科技公司都使用Git进行代码管理。

[Git]: https://zh.wikipedia.org/wiki/Git

版本控制演变：

* CVS：始祖，集中式(每次提交都需要联网，效率低)，1985
* SVN：集大成者，集中式(每次提交都需要联网，效率低)，2000
* Git：Linux之父Linus开发的，geek主流，分布式(不需要联网，可以在本地提交)，2005
* GitHub：geek社区，托管网站，2008

Git的操作流程：

![git-mode](/assets/images/201810/git-mode.jpg)

```
工作目录 ------------> 暂存区 ------------> 版本历史
        git add files        git commit
```


# [Git的设计思想]

[Git的设计思想]: https://git-scm.com/book/en/v2/Getting-Started-Git-Basics

如果要学习Git，请先忘记之前你知道的关于其他VCSs的用法(例如，SVN)，这样可以帮助你更好地理解和使用Git，因为Git的设计理念和其他的VCSs完全不同，下图非常形象地解释了二者的不同。

![two_ways_vcs](/assets/images/201810/two_ways_vcs.jpg)

关于Git的一些基本概念：

## Snapshots, Not Differences 

Git通过快照的方式来记录每次变更，如果文件没有变更，新的分支不会重复复制此文件，而是通过索引的方式访问历史的快照文件。

* Other systems tend to store data as changes to a base version of each file.

![git_svn_diff1](/assets/images/201810/git_svn_diff1.jpg)

* Git stores data as snapshots of the project over time.

![git_svn_diff2](/assets/images/201810/git_svn_diff2.jpg)

## Nearly Every Operation Is Local

使用Git的时候，如果要浏览历史版本，是不需要进行网络交互的，因为历史版本的信息都保存在本地，因此可以在任何时间任何地点(即离线状态)进行工作，到可以使用网络的时候，再对之前的所有工作提交到远端版本仓库(git push origin master)。而对比其他VCSs，在离线的状态下这些工作都是不可能完成的。

## Git Has Integrity

Git对所有存储的文件通过SHA-1计算散列值来索引文件，以保证文件的完整性。类似区块链的[Merkel Tree]链式结构，来防止数据丢失。Git的很多命令需要依赖这个hash值(只指定`前4位`即可)进行操作，比如，`git reset --hard $hashid`

[Merkel Tree]: https://en.wikipedia.org/wiki/Merkle_tree

Everything in Git is check-summed before it is stored and is then referred to by that checksum. This means it's impossible to change the contents of any file or directory without Git knowing about it. This functionality is built into Git at the lowest levels and is integral to its philosophy. You can't lose information in transit or get file corruption without Git being able to detect it.

The mechanism that Git uses for this checksumming is called a SHA-1 hash. This is a 40-character string composed of hexadecimal characters (0–9 and a–f) and calculated based on the contents of a file or directory structure in Git. A SHA-1 hash looks something like this:

24b9da6552252987aa493b52f8696cd6d3b00373

You will see these hash values all over the place in Git because it uses them so much. In fact, Git stores everything not by file name but in the Git database addressable by the hash value of its contents.


## Git Generally Only Adds Data

在操作Git时只有添加文件的概念，而没有删除的概念。类似区块链，只要数据上链就没有办法删除，从而达到防抵赖的特性。


When you do actions in Git, nearly all of them only add data to the Git database. It is very difficult to get the system to do anything that is not undoable or to make it erase data in any way. As in any VCS, you can lose or mess up changes you haven't committed yet; but after you commit a snapshot into Git, it is very difficult to lose, especially if you regularly push your database to another repository.

This makes using Git a joy because we know we can experiment without the danger of severely screwing things up. For a more in-depth look at how Git stores its data and how you can recover data that seems lost, see Chapter 9.

## The Three States

请记住Git中的`三种`状态，通过命令行操作时，在命令行的提示信息中也可以看到当前执行到了哪个状态。

1. **committed** (已提交到`本地数据库`)
2. **modified** (已修改但未提交)
3. **staged** (对修改进行了标记以待提交)

![git_local_operation](/assets/images/201810/git_local_operation.jpg)


Now, pay attention. This is the main thing to remember about Git if you want the rest of your learning process to go smoothly. Git has three main states that your files can reside in: committed, modified, and staged. 

`Committed` means that the data is safely stored in your `local database`. 

`Modified` means that you have changed the file but have not committed it to your database yet. 

`Staged` means that you have marked a modified file in its current version to go into your next commit snapshot.

This leads us to the three main sections of a Git project: `the Git directory`, `the working directory`, and `the staging area`.

`The Git directory` is where Git stores the metadata and object database for your project. This is the most important part of Git, and it is what is copied when you clone a repository from another computer.

`The working directory` is a single checkout of one version of the project. These files are pulled out of the compressed database in the Git directory and placed on disk for you to use or modify.

`The staging area` is a simple file, generally contained in your Git directory, that stores information about what will go into your next commit. It's sometimes referred to as the index, but it's becoming standard to refer to it as the staging area.

**The basic Git workflow goes something like this:**

1. You modify files in your working directory.
2. You stage the files, adding snapshots of them to your staging area.
3. You do a commit, which takes the files as they are in the staging area and stores that snapshot permanently to your Git directory.

If a particular version of a file is in the git directory, it's considered committed. If it's modified but has been added to the staging area, it is staged. And if it was changed since it was checked out but has not been staged, it is modified. In Chapter 2, you'll learn more about these states and how you can either take advantage of them or skip the staged part entirely.


# Git的安装和配置

## 下载安装

[Git下载地址]，Git有两种使用方式，一种是`命令行`，一种是集成了git命令的`GUI客户端`。本文主要使用命令行的方式。

[Git下载地址]: http://git-scm.com/downloads

## 基础配置

安装后，对Git进行配置，通过命令行的方式。首先需要配置用户名和邮箱，如果没有配置在执行Git命令时会提示如下错误：

```
*** Please tell me who you are.

Run

  git config --global user.email "you@example.com"
  git config --global user.name "Your Name"

to set your account's default identity.
Omit --global to set the identity only in this repository.
```

``` bash
# Sets the default name for git to use when you commit
git config --global user.name "Your Name Here"

# Sets the default email for git to use when you commit
git config --global user.email "your_email@example.com"
```

注意上面的设置是对`全局`生效的，若想对`某个repo`使用不同的username或email可以通过下面方法进行设置：

``` bash
# Changes the working directory to the repository you need to switch info for
cd my_other_repo

# Sets the user's name for this specific repository
git config --local user.name "Different Name"

# Sets the user's email for this specific repository
git config --local user.email "differentemail@email.com"
```

查看配置：

``` bash
git config --list [--local | --global | --system]
```

区别:
* local：只对某一个仓库生效
* global：对当前用户的所有仓库生效 (常用)
* system：对本系统的所有用户生效 (少用)

更多：[Customizing-Git-Git-Configuration]

[Customizing-Git-Git-Configuration]: https://git-scm.com/book/en/v2/Customizing-Git-Git-Configuration#_git_config

## 创建Git仓库

两种场景：

* 把已有的项目代码纳入Git管理

``` bash
$ cd your_project
$ git init 
```

* 新建的项目直接使用Git管理

``` bash
$ cd 某个文件夹
$ git init your_project   # 会在当前路径下创建your_project目录
$ cd your_project
```

## 忽略文件

在实际的项目中，比如，临时文件，编译过程文件(例如C/C++编译过程中的`.o`文件)等，是不需要进行代码管理的(即不用提交)。可以在仓库的根目录下创建一个名为`.gitignore`的文件，并配置当前项目需要忽略的文件列表。

``` bash
*.[oa]
*~
```

* 忽略所有以`.o`或`.a`结尾的文件
* 忽略所有以波浪符结尾的文件

如果忘记添加`.gitignore`文件，不小心将一些不需要的日志文件添加到了暂存区(staging area)，可以这么撤销：

``` bash
git rm --cached *.log
```

此命令会把暂存区域的文件移除，同时文件仍然保留在磁盘。


![gitignore](/assets/images/201810/gitignore.jpg)



# Git的协作模式

使用Git的最大好处，是提高团队的协作效率，以下是两种常见的方式。

## Gitflow工作流

主要是适合复杂的大型项目。

* Master分支：用于存放线上版本代码，可以方便的给代码打版本号。
* Develop分支：用于整合 Feature 分支。
* Hotfix分支：紧急修复的分支，一旦修复了可以合并到 Master 分支和 Develop 分支。
* Feature分支：某个功能的分支，从 Develop 分支切出，并且功能完成时又合并回 Develop 分支，不直接和Master 分支交互。
* Release分支：通常对应一个迭代。将一个版本的功能全部合并到 Develop 分支之后，从 Develop 切出一个Release 分支。这个分支不再追加新需求，可以完成 bug 修复、完善文档等工作。务必记住，代码发布后，需要将其合并到 Master 分支，同时也要合并到 Develop 分支。

![gitflow](/assets/images/201810/gitflow.jpg)

## 功能分支模式  

主要适合日常不太复杂的项目开发，此模式下有三种类型的分支：

1. `master`分支，只能有一个，相当于svn中的trunk，只允许merge requests，不允许直接push。可以通过分支保护设置。
2. `feature`分支，可以有多个，用来做新功能开发，可以随意commit和push
3. `bugfix`分支，可以有多个，用来做bugfix，可以随意commit和push

## 新功能开发流程

1. 从远端master拉取最新的代码到本地
2. 创建一个新的分支，可以在新的分支中随时commit和push
3. 新分支功能开发完成后
	+ 先把主干merge到分支，解决可能的冲突，再发起merge requests到master
	+ 先把分支rebase到master，解决可能存在的冲突，再发起merge requests到master
4. 项目的maintainer接收到merge requests，负责code review，没有问题后再accept并merge到master

> PS：使用rebase，提交记录线性的，会比较清晰。

## bugfix流程

与新功能开发流程基本一致，注意建立分支时请以`bugfix_`开头

## 撤销流程

在未进行`git push`前的所有操作，都是在本地仓库中执行的。将**本地仓库的代码还原操作**叫做**撤销**。

* 文件被修改了，但未执行git add操作(working tree内撤销)
``` bash
git checkout <fileName>
git checkout .
```

* 同时对多个文件执行了`git add`操作，但本次只想提交其中一部分文件(这种情况是把一个文件修改了add到暂存区了但又想重新放回工作区)
``` bash
git add *
git status
# 取消暂存
git reset HEAD <fileName>
```

* 文件执行了`git add`操作，但想撤销对其的修改(index内回滚)
``` bash
# 1. 取消暂存
git reset HEAD <fileName>
# 2. 撤销修改
git checkout <fileName>
```

* 修改的文件已被`git commit`，但想再次修改不再产生新的commit
``` bash
# 修改最后一次提交 
git add sample.txt
git commit --amend -m"说明"
```

* 版本已经commit，但是需要回退，会将提交记录和代码全部回滚
``` bash
git reset --hard <commit-id>
```

* 将`HEAD`理解为当前分支的别名，`HEAD`表示当前版本，上一个版本就`HEAD^`，上上一个版本就是`HEAD^^`，当然往上100个版本写100个^比较容易数不过来，所以写成`HEAD~100`
``` bash
git reset --hard HEAD^
```

> 问题：如果使用`git reset --hard <commit-id>`回退到某个版本，之后想撤回，使用`git log`已经找不到之前的提交记录，怎么办？
> 解决方法：Git提供了一个`git reflog`命令用来记录你的每一次命令，可以找到之前的commit-id，然后再执行`git reset --hard <commit-id>`。

**总结**：

* 场景1：当你改乱了工作区某个文件的内容，想直接丢弃工作区的修改时，用命令`git checkout -- file`
* 场景2：当你不但改乱了工作区某个文件的内容，还添加到了暂存区时，想丢弃修改，分两步，第一步用命令`git reset HEAD file`，就回到了场景1，第二步按场景1操作。
* 场景3：已经提交了不合适的修改到版本库时，想要撤销本次提交，则按照版本回退的办法，不过前提是没有推送到远程库。

refer: [Git撤销&回滚操作]

[Git撤销&回滚操作]: https://blog.csdn.net/ligang2585116/article/details/71094887

## 回滚流程

已进行`git push`，即已推送到远程仓库中。将**已被提交到远程仓库的代码还原操作**叫做**回滚**。注意：对远程仓库做回滚操作是有风险的，需提前做好备份和通知其他团队成员。

若不小心把错误的代码merge到主干了怎么办？到项目根目录，执行如下操作：

``` bash
git checkout master          # 切换到master
git pull                     # 拉取最新代码
git log -l 5                 # 查看想退回到版本号并copy下来，后面到数字可以自己设置。或者使用git reflog
git reset --hard `版本号`     # 强制将指针回退到指定版本
git push -f                  # 强制push到远端master
```


# Git的常用命令

| 操作 | Git | Subversion |
| -- | -- | --
|复制数据库|	git clone|	svn checkout
|提交|	git commit|	svn commit
|查看提交的详细记录|	git show|	svn cat
|确认状态|	git status|	svn status
|确认差异|	git diff|	svn diff
|确认记录|	git log / git reflog|	svn log
|添加|	git add|	svn add
|移动|	git mv|	svn mv
|删除|	git rm|	svn rm
|取消修改|	git checkout / git reset|	svn revert 
|创建分支|	git branch|	svn copy 
|切换分支|	git checkout|	svn switch
|合并|	git merge|	svn merge
|创建标签|	git tag|	svn copy 
|更新|	git pull / git fetch|	svn update
|反映到远端|	git push|	svn commit 
|忽略档案目录|	.gitignore|	.svnignore

## 查看和设置Git配置

``` bash
# 查看当前Git配置
git config --list

# 设置用户名和邮箱
git config --global user.name "Your Name Here"
git config --global user.email "your_email@example.com"
```

## 创建代码仓库


``` bash
# 初始化并生成.git子目录存放Git仓库的配置文件
git init

# 添加指定的文件到暂存区
git add README.md
# 把所有修改过文件都加入暂存区
git add .  

# 先提交到本地
git commit -m "first commit"

# 或者将以上两步合并为一步

git commit -am "first commit"

# 添加remote仓库
git remote add origin git@github.com:gerryyang/your_repo.git

# 将本地修改的代码推送到remote仓库
git push -u origin master
```

## 远程仓库操作 - git clone

从远程主机克隆一个版本库，使用`git clone`命令。该命令会在本地主机生成一个目录，与远程主机的版本库同名。如果要指定不同的目录名，可以将目录名作为`git clone`命令的第二个参数。`git clone`支持多种协议，除了HTTP(s)以外，还支持SSH、Git、本地文件协议等。通常来说，Git协议下载速度最快，SSH协议用于需要用户认证的场合。各种协议优劣的详细讨论请参考[官方文档](http://git-scm.com/book/en/Git-on-the-Server-The-Protocols)。

``` bash
# 克隆一个版本库
git clone <版本库的网址>

# 指定不同的目录名
git clone <版本库的网址> <本地目录名>

# 获取master分支
git clone https://github.com/gerryyang/mac-utils.git

# 获取指定分支
git clone -b $branch https://github.com/gerryyang/mac-utils.git

# 不同协议
git clone http[s]://example.com/path/to/repo.git/
git clone ssh://example.com/path/to/repo.git/    # ssh协议
git clone [user@]example.com:path/to/repo.git/   # ssh协议另一种表示方法
git clone git://example.com/path/to/repo.git/
git clone /opt/git/project.git 
git clone file:///opt/git/project.git
git clone ftp[s]://example.com/path/to/repo.git/
git clone rsync://example.com/path/to/repo.git/
```


## 远程仓库操作 - git remote

为了便于管理，Git要求每个远程主机都必须指定一个`主机名`。`git remote`命令就用于管理主机名。不带选项的时候，git remote命令列出所有远程主机。使用`-v`选项，可以参看远程主机的网址。克隆版本库的时候，所使用的远程主机自动被Git命名为origin。如果想用其他的主机名，需要用git clone命令的`-o`选项指定。`git remote show`命令加上`主机名`，可以查看该主机的详细信息。

``` bash
# 命令列出所有远程主机
$ git remote
origin

# 参看远程主机的网址
$ git remote -v
origin  git@github.com:jquery/jquery.git (fetch)
origin  git@github.com:jquery/jquery.git (push)

# 用其他的主机名
$ git clone -o jQuery https://github.com/jquery/jquery.git
$ git remote
jQuery

# 查看该远程主机的详细信息
$ git remote show <主机名>
$ git remote show origin
* remote origin
  Fetch URL: git@github.com:gerryyang/mac-utils.git
  Push  URL: git@github.com:gerryyang/mac-utils.git
  HEAD branch: master
  Remote branch:
    master tracked
  Local branch configured for 'git pull':
    master merges with remote master
  Local ref configured for 'git push':
    master pushes to master (up to date)

# 添加远程主机
$ git remote add <主机名> <网址>

# 删除远程主机
$ git remote rm <主机名>

# 远程主机的改名
$ git remote rename <原主机名> <新主机名>
```

## 远程仓库操作 - git fetch

一旦远程主机的版本库有了更新（Git术语叫做commit），需要将这些更新取回本地，这时就要用到`git fetch`命令。`git fetch`命令通常用来查看其他人的进程，因为它取回的代码对你本地的开发代码没有影响。取回远程主机的更新以后，可以在它的基础上，使用`git checkout`命令创建一个新的分支。

``` bash
# 将某个远程主机的更新，全部取回本地
$ git fetch <远程主机名>

# 默认情况下，git fetch取回所有分支（branch）的更新。如果只想取回特定分支的更新，可以指定分支名
$ git fetch <远程主机名> <分支名>

# 取回origin主机的master分支
# 所取回的更新，在本地主机上要用"远程主机名/分支名"的形式读取。比如origin主机的master，就要用origin/master读取
$ git fetch origin master
$ git branch -a
* master
  remotes/origin/master

# 在origin/master的基础上，创建一个新分支
$ git checkout -b newBrach origin/master

# 也可以使用git merge命令或者git rebase命令，在本地分支上合并远程分支
# 在当前分支上，合并origin/master
$ git merge origin/master
$ git rebase origin/master
```

## 远程仓库操作 - git pull

`git pull`命令的作用是，取回远程主机某个分支的更新，再与本地的指定分支合并。它的完整格式稍稍有点复杂。

``` bash
$ git pull <远程主机名> <远程分支名>:<本地分支名>

# 取回origin主机的next分支，与本地的master分支合并
$ git pull origin next:master

# 如果远程分支是与当前分支合并，则冒号后面的部分可以省略
$ git pull origin next

# 上面命令表示，取回origin/next分支，再与当前分支合并。实质上，这等同于先做git fetch，再做git merge
$ git fetch origin
$ git merge origin/next
```

## 远程仓库操作 - git push

`git push`命令用于将本地分支的更新，推送到远程主机。它的格式与git pull命令相仿。注意，分支推送顺序的写法是`<来源地>:<目的地>`，所以`git pull`是`<远程分支>:<本地分支>`，而`git push`是`<本地分支>:<远程分支>`。如果省略远程分支名，则表示将本地分支推送与之存在"追踪关系"的远程分支（通常两者同名），如果该远程分支不存在，则会被新建。


``` bash
$ git push <远程主机名> <本地分支名>:<远程分支名>

# 将本地的master分支推送到origin主机的master分支。如果后者不存在，则会被新建
$ git push origin master

# 如果省略本地分支名，则表示删除指定的远程分支，因为这等同于推送一个空的本地分支到远程分支。即表示删除origin主机的master分支
$ git push origin :master
# 等同于
$ git push origin --delete master
```


## 查看代码状态和历史提交信息

``` bash

# 查看当前仓库代码状态
git status

# 对比修改的内容
git diff

# 查看最近2次的提交统计信息
git log --stat -2

# 按格式化显示提交信息
git log --pretty=format:"%h - %an, %ar : %s"
git log --pretty=format:"%h %s" --graph
git log --since=2.weeks

# 将每个提交版本信息缩减为一行
git log --pretty=oneline 
git log --oneline -n3 
```
更多: [Git-基础-查看提交历史]

[Git-基础-查看提交历史]: https://git-scm.com/book/zh/v1/Git-基础-查看提交历史

## 分支操作

Git最核心的特性就是，创建新分支操作几乎能在瞬间完成，并且在不同分支之间的切换操作也是一样高效。在切换分支时，会发现当前工作目录里的文件会改变成切换后的分支代码。

由于Git的分支实质上仅是包含所指对象校验和(长度为40的SHA-1值字符串)的文件，所以它的创建和销毁都异常高效。创建一个新分支就相当于往一个文件中写入41 个字节(40个字符和 1个换行符)，如此的简单能不快吗？

这与过去大多数版本控制系统形成了鲜明的对比，它们在创建分支时，将所有的项目文件都复制一遍，并保存到一个特定的目录。完成这样繁琐的过程通常需要好几秒钟，有时甚至需要好几分钟。所需时间的长短，完全取决于项目的规模。而在Git中，任何规模的项目都能在瞬间创建新分支。

``` bash
# 查看分支详细信息 (可以看出代码是否已经push)
git branch -av

# 删除本地分支
git branch -d  

# 强制删除本地分支                   
git branch -D  

# 删除远端分支                   
git push origin -d <branch-name> 

# 通过查看远程分支信息，也可以看出每个分支的进度
git remote show origin

# 切换到某个分支
git checkout <branch-name> 

# 切换到上一个分支
git checkout -

# 新建并切换到该分支
git checkout -b <branch-name> 
# 等价于
git branch <branch-name>
git checkout <branch-name>

# 分支合并，例如修复了一个bug然后合并到master
git checkout master
git checkout -b hotfix
# 在hotfix分支修改bug代码
git commit -a -m 'fix the bug'
git checkout master
git merge hotfix
# 最新的修改已经合到 master分支，hotfix分支可以退出历史舞台了，可以删掉(建议，除非分支确实太多了)
git branch -d hotfix

# 如何撤销一个合并
git merge --abort
# 如果合并后发现有问题需要撤销
git reset --hard

# 删除无效的远程追踪分支
$ git remote show origin
Warning: Permanently added the RSA host key for IP address '140.82.113.4' to the list of known hosts.
* remote origin
  Fetch URL: git@github.com:gerryyang/mac-utils.git
  Push  URL: git@github.com:gerryyang/mac-utils.git
  HEAD branch: master
  Remote branches:
    master                  tracked
    refs/remotes/origin/tmp stale (use 'git remote prune' to remove)        # 此分支为无效分支，在远程仓库已经不存在，可以使用 git remote prune orign 命令来同步删除本地此仓库
  Local branch configured for 'git pull':
    master merges with remote master
  Local ref configured for 'git push':
    master pushes to master (up to date)

$ git remote prune origin
Pruning origin
URL: git@github.com:gerryyang/mac-utils.git
 * [pruned] origin/tmp

```

注意：若在执行`git merge`时遇到代码冲突，需要先解决冲突。解决完冲突后，执行`git status`可以查看，任何因包含合并冲突而有待解决的文件，都会以未合并状态标识出来。而如果没有，说明所有的冲突都修复了，并且提醒你去提交一下。再执行`git commit -am <comment>`，至此冲突解决完毕并已提交代码。

更多：

* [处理合并冲突](https://www.git-tower.com/learn/git/ebook/cn/command-line/advanced-topics/merge-conflicts)
* [How To Rename a Local and Remote Git Branch](https://linuxize.com/post/how-to-rename-local-and-remote-git-branch/)



## 变基(rebase)操作

在Git中整合来自不同分支的修改主要有两种方法：
* merge
* rebase

这两种整合方法的最终结果没有任何区别，但是变基(rebase)使得提交历史更加整洁。在查看一个经过变基的分支的历史记录时会发现，尽管实际的开发工作是并行的，但它们看上去就像是串行的一样，提交历史是一条直线没有分叉。但不合理的使用变基，会丢失别人的提交记录，这时候人民群众会仇恨你，你的朋友和家人也会嘲笑你，唾弃你。

## 暂存操作

当前你在开发feature1分支，开发了一半，还要2天才能开发完成，这时候又不想提交。这时突然来了个bug，你必须今天就得修复bug，修复完了后才继续开发需求，怎么办？这里就使用到了暂存的功能。

``` bash
# 先把所有的修改暂存起来，这时候你的所有改动都好像消失了一样，但其实是被暂存起来了
git stash 
# 新建bugfix分支去修复bug
git checkout -b bugfix/分支名  
# 修复bug...
# 回到原来的开发分支
git checkout feature1 
# 恢复暂存的内容
git stash pop 
# 继续开发需求...
```

## 压缩提交

在开发中的时候尽量保持一个较高频率的代码提交，这样可以避免不小心代码丢失。但是真正合并代码的时候，我们并不希望有太多冗余的提交记录。那么，如何压缩commit记录呢？

``` bash
# 使用 git log 找到起始 commit-id
git reset commit-id  # 切记不要用 --hard 参数
# 重新 git add && git commit
```

## 子模块submodule

默认情况下，子模块会将子项目放到一个与仓库同名的目录中，本例中是 “DbConnector”。 如果你想要放到其他地方，那么可以在命令结尾添加一个不同的路径。虽然 DbConnector 是工作目录中的一个子目录，但 Git 还是会将它视作一个子模块。当你不在那个目录中时，Git 并不会跟踪它的内容， 而是将它看作该仓库中的一个特殊提交。如果你想看到更漂亮的差异输出，可以给 git diff 传递 --submodule 选项。

``` bash
# 添加子模块
git submodule add https://github.com/chaconinc/DbConnector
git clone --recursive https://github.com/chaconinc/MainProject

# 清除子模块
git submodule deinit --all
```

[Git 工具 - 子模块]

[Git 工具 - 子模块]: https://git-scm.com/book/zh/v2/Git-工具-子模块


# Git与GitHub 

`GitHub`主要为开发者提供`Git`仓库的托管服务，拥有一只`octocat`的吉祥物。截止2013年12月，GitHub托管的仓库数已超过1000万。GitHub这一服务为开源世界带来了社会化编程的概念。在GitHub上进行交流时用到的Issue, Wiki等都可以用`FGM(Git Flavored Markdown)`语法表述。

在GitHub上公开源代码时可以指定相关的许可协议。实际使用时，只需将`LICENSE`文件加入仓库，并在`README.md`文件中声明使用了何种许可协议即可。

* 修正的BSD许可协议
* Apache许可协议
* MIT许可协议 (https://opensource.org/licenses/mit-license.php)

```
Copyright <YEAR> <COPYRIGHT HOLDER>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

关于GitHub的更多功能：

* https://github.com/features
* https://github.com/trending

# SSH免密提交

使用`ssh认证方式`可以实现免密提交，通过`ssh-keygen`可以生成ssh认证所需的`公钥`和`私钥`。执行`ssh-keygen`后直接回车，不用填写东西，之后会让你输入密码(根据安全性选择是否需要此密码，可以直接回车)，然后就生成一个`.ssh`目录，目录里会生成两个(默认)文件：`id_rsa`和`id_rsa.pub`。

``` bash
cd $HOME/.ssh
ssh-keygen -t rsa -C <email>                                 # 默认生成id_rsa私钥文件和id_rsa.pub公钥文件
ssh-keygen -t rsa -C <email> -f <id_rsa> -C "公钥文件中的备注"  # 指定生成的私钥文件名
```

生成公私钥后，复制公钥信息配置在Github上。并注意本地`.git/config`配置中的`url`为ssh要求的格式：

```
[core]
        repositoryformatversion = 0
        filemode = false
        bare = false
        logallrefupdates = true
        symlinks = false
        ignorecase = true
[remote "origin"]
        #url = http://git.code.oa.com/gerryyang/portal.git
        url = git@git.code.oa.com:gerryyang/portal.git
        fetch = +refs/heads/*:refs/remotes/origin/*
        puttykeyfile =
[branch "master"]
        remote = origin
        merge = refs/heads/master
[user]
        name = gerryyang
        email = gerryyang@tencent.com
```

测试是否生效，如果ok以后执行`git push`时就不用再输入用户名和密码验证了。

``` bash
 ssh -vT git@github.com
```

有时需要配置多个ssh key，例如，一个用于Github，一个用于公司内部的git。此时要为不同的域名指定不同的私钥配置。在`$HOME/.ssh`目录下，创建一个`config`配置文件，根据不同域名指定不同的私钥。这样就可以实现多个不同的ssh key免密提交了。

```
# oa
Host git.code.oa.com
   HostName git.code.oa.com
   PreferredAuthentications publickey
   IdentityFile ~/.ssh/oa_rsa

# github
Host github.com
   HostName github.com
   PreferredAuthentications publickey
   IdentityFile ~/.ssh/id_rsa
```

若配置使用http协议，通过在`.gitconfig`配置中添加`credential`选项也可以实现免密操作，但是用户名和密码是以明文的形式存储的，存在安全风险。

```
[credential]
        helper = store --file=/c/Users/gerryyang/.git_credentials
```


更多：[Connecting to GitHub with SSH]

[Connecting to GitHub with SSH]: https://help.github.com/articles/connecting-to-github-with-ssh/


# Git相关工具

* Sourcetree

A free Git client for Windows and Mac. Sourcetree simplifies how you interact with your Git repositories so you can focus on coding. Visualize and manage your repositories through Sourcetree's simple Git GUI.

https://www.sourcetreeapp.com/

*  TortoiseGit

TortoiseGit is a Windows Shell Interface to Git and based on TortoiseSVN. It's open source and can fully be build with freely available software.

https://tortoisegit.org/


# Refer


* [阮一峰: 版本控制入门插图教程]
* [阮一峰: Git远程操作详解]
* [Git官方的使用文档: Getting Started - About Version Control]
* [廖雪峰: Git教程]



[阮一峰: 版本控制入门插图教程]: http://www.ruanyifeng.com/blog/2008/12/a_visual_guide_to_version_control.html
[阮一峰: Git远程操作详解]: http://www.ruanyifeng.com/blog/2014/06/git_remote.html
[Git官方的使用文档: Getting Started - About Version Control]: https://git-scm.com/book/en/v2/Getting-Started-About-Version-Control
[廖雪峰: Git教程]: https://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000



