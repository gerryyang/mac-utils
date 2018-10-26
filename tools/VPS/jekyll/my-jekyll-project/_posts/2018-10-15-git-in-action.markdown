---
layout: post
title:  "Git in Action"
date:   2018-10-15 13:00:00 +0800
categories: tools
---

* Do not remove this line (it will not be displayed)
{:toc}

Linux的创始人Linus Torvalds在2005年开发了[Git]的原型程序，主要用于对Linux内核开发的版本管理。作为开发者日常离不开对代码的管理，Git 采用了分布式版本库的方式，使得开发者更加自由，随时随地可以管理自己的代码，而不像集中式的版本控制工具(`VCSs`)，例如`CVS`，`Subversion`，需要对网络的依赖。使用Git让源代码的管理，以及团队间的协作更为方便，这也是为什么越来越多的科技公司都使用Git进行代码管理。

[Git]: https://zh.wikipedia.org/wiki/Git

版本控制演变：

* cvs：始祖，集中式（每次提交都需要联网，效率低），1985
* svn：集大成者，集中式（每次提交都需要联网，效率低），2000
* git：Linux之父Linus开发的，geek主流，分布式（不需要联网，可以在本地提交），2005
* github：geek社区，托管网站，2008

Git的操作流程：

![git-mode](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/git-mode.jpg)


更多：[阮一峰: 版本控制入门插图教程]

[阮一峰: 版本控制入门插图教程]: http://www.ruanyifeng.com/blog/2008/12/a_visual_guide_to_version_control.html

# [Git的设计思想]

[Git的设计思想]: https://git-scm.com/book/en/v2/Getting-Started-Git-Basics

如果要学习Git，请先忘记之前你知道的关于其他VCSs的用法(例如，svn)，这样可以帮助你更好地理解和使用Git，因为Git的设计理念和其他的VCSs完全不同，下图非常形象地解释了二者的不同。

![two_ways_vcs](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/two_ways_vcs.jpg)

关于Git的一些基本概念：

## Snapshots, Not Differences 

Git通过快照的方式来记录每次变更，如果文件没有变更，新的分支不会重复复制此文件，而是通过索引的方式访问历史的快照文件。

![git_svn_diff1](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/git_svn_diff1.jpg)

![git_svn_diff2](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/git_svn_diff2.jpg)

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

1. **committed** (已提交到**本地**数据库)
2. **modified** (已修改但未提交)
3. **staged** (对修改进行了标记以待提交)

![git_local_operation](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/git_local_operation.jpg)


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

[Git下载地址]，Git有两种使用方式，一种是命令行，一种是集成了git命令的客户端。安装后，对Git进行配置，通过命令行的方式：

[Git下载地址]: http://git-scm.com/downloads

## 基础配置

``` bash
# Sets the default name for git to use when you commit
git config --global user.name "Your Name Here"

# Sets the default email for git to use when you commit
git config --global user.email "your_email@example.com"
```

注意上面的设置是对**全局**生效的，若想对某个repo使用不同的username或email可以通过下面方法进行设置：

``` bash
# Changes the working directory to the repository you need to switch info for
cd my_other_repo

# Sets the user's name for this specific repository
git config user.name "Different Name"

# Sets the user's email for this specific repository
git config user.email "differentemail@email.com"
```

更多：[Customizing-Git-Git-Configuration]

[Customizing-Git-Git-Configuration]: https://git-scm.com/book/en/v2/Customizing-Git-Git-Configuration#_git_config

## 忽略文件

在实际的项目中，比如，临时文件，编译过程文件(例如C/C++编译过程中的`.o`文件)等，是不需要进行代码管理的(即不用提交)。可以在仓库的根目录下创建一个名为`.gitignore`的文件，并配置当前项目需要忽略的文件列表。

```
*.[oa]
*~
```

* 忽略所有以`.o`或`.a`结尾的文件
* 忽略所有以波浪符结尾的文件

如果忘记添加`.gitignore`文件，不小心将一些不需要的日志文件添加到了暂存区(staging area)，可以这么撤销：

```
git rm --cached *.log
```

此命令会把暂存区域的文件移除，同时文件仍然保留在磁盘。


![gitignore](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/gitignore.jpg)



# Git的协作模式

使用Git的最大好处，是提高团队的协作效率，以下是两种常见的方式。

## Gitflow工作流

主要是适合复杂的大型项目。

* Master分支：用于存放线上版本代码，可以方便的给代码打版本号。
* Develop分支：用于整合 Feature 分支。
* Hotfix分支：紧急修复的分支，一旦修复了可以合并到 Master 分支和 Develop 分支。
* Feature分支：某个功能的分支，从 Develop 分支切出，并且功能完成时又合并回 Develop 分支，不直接和Master 分支交互。
* Release分支：通常对应一个迭代。将一个版本的功能全部合并到 Develop 分支之后，从 Develop 切出一个Release 分支。这个分支不再追加新需求，可以完成 bug 修复、完善文档等工作。务必记住，代码发布后，需要将其合并到 Master 分支，同时也要合并到 Develop 分支。

![gitflow](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/gitflow.jpg)

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

## 回滚流程

若不小心把错误的代码merge到主干了怎么办？到项目根目录，执行如下操作：

```
git checkout master          # 切换到master
git pull                     # 拉取最新代码
git log -l 5                 # 查看想退回到版本号并copy下来，后面到数字可以自己设置。或者使用git reflog
git reset --hard `版本号`    # 强制将指针回退到指定版本
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

```
# 查看当前Git配置
git config --list

# 设置用户名和邮箱
git config --global user.name "Your Name Here"
git config --global user.email "your_email@example.com"
```

## 创建代码仓库

```
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

## 克隆仓库

```
# 获取master分支
git clone https://github.com/gerryyang/mac-utils.git

# 获取指定分支
git clone -b $branch https://github.com/gerryyang/mac-utils.git
```

## 查看代码状态和历史提交信息

```
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
```
more: [Git-基础-查看提交历史]

[Git-基础-查看提交历史]: https://git-scm.com/book/zh/v1/Git-基础-查看提交历史

## 分支操作

Git最核心的特性就是，创建新分支操作几乎能在瞬间完成，并且在不同分支之间的切换操作也是一样高效。在切换分支时，会发现当前工作目录里的文件会改变成切换后的分支代码。

由于Git的分支实质上仅是包含所指对象校验和(长度为40的SHA-1值字符串)的文件，所以它的创建和销毁都异常高效。创建一个新分支就相当于往一个文件中写入41 个字节(40个字符和 1个换行符)，如此的简单能不快吗？

这与过去大多数版本控制系统形成了鲜明的对比，它们在创建分支时，将所有的项目文件都复制一遍，并保存到一个特定的目录。完成这样繁琐的过程通常需要好几秒钟，有时甚至需要好几分钟。所需时间的长短，完全取决于项目的规模。而在Git中，任何规模的项目都能在瞬间创建新分支。

```
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
# 最新的修改已经合到 master分支，hotfix分支可以退出历史舞台了，可以删掉   (建议，除非分支确实太多了)
git branch -d hotfix

```

注意：若在执行`git merge`时遇到代码冲突，需要先解决冲突。解决完冲突后，执行`git status`可以查看，任何因包含合并冲突而有待解决的文件，都会以未合并状态标识出来。而如果没有，说明所有的冲突都修复了，并且提醒你去提交一下。再执行`git commit -am <comment>`，至此冲突解决完毕并已提交代码。



## 撤销操作

```
# 取消暂存的文件 (这种情况是把一个文件修改了add到暂存区了但又想重新放回工作区，这种不会更改本地磁盘的文件)
git reset HEAD <filename>

# 版本回退(针对已经commit)，会将提交记录和代码全部回滚
git reset --hard <commit-id>

# 将HEAD理解为当前分支的别名
# HEAD表示当前版本，上一个版本就HEAD^，上上一个版本就是HEAD^^，当然往上100个版本写100个^比较容易数不过来，所以写成HEAD~100
git reset --hard HEAD^

# 撤消对文件的修改  (这种情况是在工作区把一个文件修改了，但发现有问题，想撤销修改，这种会更改本地磁盘的文件，并且不可逆，所以这是一个危险的命令)
git checkout -- files
```

问题：如果使用`git reset --hard <commit-id>`回退到某个版本，之后想撤回，使用`git log`已经找不到之前的提交记录，怎么办？
解决方法：Git提供了一个`git reflog`命令用来记录你的每一次命令，可以找到之前的commit-id，然后再执行`git reset --hard <commit-id>`。

**总结**：

* 场景1：当你改乱了工作区某个文件的内容，想直接丢弃工作区的修改时，用命令`git checkout -- file`
* 场景2：当你不但改乱了工作区某个文件的内容，还添加到了暂存区时，想丢弃修改，分两步，第一步用命令`git reset HEAD file`，就回到了场景1，第二步按场景1操作。
* 场景3：已经提交了不合适的修改到版本库时，想要撤销本次提交，则按照版本回退的办法，不过前提是没有推送到远程库。

## 远程仓库操作

```
# 查看远程仓库的名称 (`origin`是Git默认的远程仓库名字)
git remote

# 查看远程仓库的详细信息
git remote show origin

# 将代码推送到远程仓库
# git push $remote-name $branch-name
git push origin master

# 拉取远程仓库代码
git fetch
get merge
# 等价于
git pull

# 将所有远程分支拉取下来
git fetch -- all

# 删除远端分支  (本地分支和远程分支的删除互不影响)                  
git push origin -d <branch-name> 
# 或者
git push origin :<branch-name> 
```

例如：

```
$ git remote
origin
$ git remote show origin
* remote origin
  Fetch URL: git@git.code.oa.com:gerryyang/portal.git
  Push  URL: git@git.code.oa.com:gerryyang/portal.git
  HEAD branch: master
  Remote branches:
    V1.0R010 tracked
    V1.0R020 tracked
    master   tracked
  Local branch configured for 'git pull':
    master merges with remote master
  Local refs configured for 'git push':
    V1.0R010 pushes to V1.0R010 (local out of date)
    V1.0R020 pushes to V1.0R020 (up to date)
    master   pushes to master   (local out of date)
```

更多：[阮一峰: Git远程操作详解]

[阮一峰: Git远程操作详解]: http://www.ruanyifeng.com/blog/2014/06/git_remote.html

## 变基(rebase)操作

在Git中整合来自不同分支的修改主要有两种方法：
* merge
* rebase

这两种整合方法的最终结果没有任何区别，但是变基(rebase)使得提交历史更加整洁。在查看一个经过变基的分支的历史记录时会发现，尽管实际的开发工作是并行的，但它们看上去就像是串行的一样，提交历史是一条直线没有分叉。但不合理的使用变基，会丢失别人的提交记录，这时候人民群众会仇恨你，你的朋友和家人也会嘲笑你，唾弃你。

## 暂存操作

当前你在开发feature1分支，开发了一半，还要2天才能开发完成，这时候又不想提交。这时突然来了个bug，你必须今天就得修复bug，修复完了后才继续开发需求，怎么办？这里就使用到了暂存的功能。

```
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

```
# 使用 git log 找到起始 commit-id
git reset commit-id  # 切记不要用 -- hard 参数
# 重新 git add && git commit
```


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

```
cd $HOME/.ssh
ssh-keygen -t rsa -C <email>                                      # 默认生成id_rsa私钥文件和id_rsa.pub公钥文件
ssh-keygen -t rsa -C <email> -f <id_rsa> -C "公钥文件中的备注"    # 指定生成的私钥文件名
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


更多：[Connecting to GitHub with SSH]

[Connecting to GitHub with SSH]: https://help.github.com/articles/connecting-to-github-with-ssh/


# Git相关工具

1. Sourcetree

A free Git client for Windows and Mac. Sourcetree simplifies how you interact with your Git repositories so you can focus on coding. Visualize and manage your repositories through Sourcetree's simple Git GUI.

https://www.sourcetreeapp.com/

2. TortoiseGit

TortoiseGit is a Windows Shell Interface to Git and based on TortoiseSVN. It's open source and can fully be build with freely available software.

https://tortoisegit.org/


# Refer

更多关于Git的内容可以参考以下一些资料。

1. Git官方的使用文档[Getting Started - About Version Control]

[Getting Started - About Version Control]: https://git-scm.com/book/en/v2/Getting-Started-About-Version-Control



