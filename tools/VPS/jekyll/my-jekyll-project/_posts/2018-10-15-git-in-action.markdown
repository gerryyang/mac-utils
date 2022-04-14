---
layout: post
title:  "Git in Action"
date:   2018-10-15 13:00:00 +0800
categories: Git
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

## 升级版本

使用源码编译安装：

安装依赖：

```
sudo yum -y install epel-release
sudo yum -y groupinstall "Development Tools"
sudo yum -y install wget perl-CPAN gettext-devel perl-devel  openssl-devel  zlib-devel curl-devel expat-devel  getopt asciidoc xmlto docbook2X
sudo ln -s /usr/bin/db2x_docbook2texi /usr/bin/docbook2x-texi
```

下载git源码，并编译：

```
sudo yum -y install wget
export VER="2.32.0"
wget https://github.com/git/git/archive/v${VER}.tar.gz
tar -xvf v${VER}.tar.gz
rm -f v${VER}.tar.gz
cd git-*
make configure
sudo ./configure --prefix=/usr
sudo make
sudo make install
```

[Install Latest Git ( Git 2.x ) on CentOS 7](https://computingforgeeks.com/how-to-install-latest-version-of-git-git-2-x-on-centos-7/)

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
# Sets the some repo name for git to use when you commit
git config --local user.name "your_name"

# Sets the some repo email for git to use when you commit
git config --local user.email "your_email@example.com"

# Sets the default name for git to use when you commit
git config --global user.name "your_name"

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

A [gitignore](https://git-scm.com/docs/gitignore) file specifies intentionally untracked files that Git should ignore. Files already tracked by Git are not affected.

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

* 生成了新的local untracked files
```
git clean -fd
```

* 文件被修改了，但未执行git add操作(working tree内撤销)
``` bash
git checkout <fileName>
git checkout .
```

* 撤销`typechange`状态的文件  refer: [Git reset files with typechange status](https://stackoverflow.com/questions/24533390/git-reset-files-with-typechange-status/53126931)

```
git status --porcelain | awk '{if ($1=="T") print $2}' | xargs git checkout
```

* 同时对多个文件执行了`git add`操作，但本次只想提交其中一部分文件(这种情况是把一个文件修改了add到暂存区了但又想重新放回工作区)
``` bash
git add *
git status
# 取消暂存
git reset HEAD <fileName>
git reset HEAD
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

## 重写历史

[refer](https://git-scm.com/book/zh/v2/Git-%E5%B7%A5%E5%85%B7-%E9%87%8D%E5%86%99%E5%8E%86%E5%8F%B2)

全局修改邮箱地址

另一个常见的情形是在你开始工作时忘记运行 `git config` 来设置你的名字与邮箱地址， 或者你想要开源一个项目并且修改所有你的工作邮箱地址为你的个人邮箱地址。 任何情形下，你也可以通过 `filter-branch` 来一次性修改多个提交中的邮箱地址。 需要小心的是只修改你自己的邮箱地址，所以你使用 `--commit-filter`：

```
$ git filter-branch --commit-filter '
        if [ "$GIT_AUTHOR_EMAIL" = "schacon@localhost" ];
        then
                GIT_AUTHOR_NAME="Scott Chacon";
                GIT_AUTHOR_EMAIL="schacon@example.com";
                git commit-tree "$@";
        else
                git commit-tree "$@";
        fi' HEAD
```

这会遍历并重写每一个提交来包含你的新邮箱地址。 因为提交包含了它们父提交的 SHA-1 校验和，这个命令会修改你的历史中的每一个提交的 SHA-1 校验和， 而不仅仅只是那些匹配邮箱地址的提交。



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

## 添加变更文件 - git add

* `git add -A` stages **all changes**
* `git add .` stages new files and modifications, **without deletions** (on the current directory and its subdirectories).
* `git add -u` stages modifications and deletions, **without new files**

```
# 设置 git add 别名，以忽略部分目录的变更
git config alias.adds 'add -- ":!protocol" ":!resources"'
git add .
```

refer: 

* [git-add - Add file contents to the index](https://git-scm.com/docs/git-add)
* https://stackoverflow.com/questions/572549/difference-between-git-add-a-and-git-add

## 提交变更信息 - git commit

* `git commit --amend` 追加或修改当前提交的记录信息

refer:

* [git-commit - Record changes to the repository](https://git-scm.com/docs/git-commit)
* [Git 合并多个 commit，保持历史简洁](https://cloud.tencent.com/developer/article/1690638)

## 查看文件最后一次修改信息 - git blame

[From GitHub](https://docs.github.com/en/repositories/working-with-files/using-files/tracking-changes-in-a-file):

> The blame command is a Git feature, designed to help you determine who made changes to a file.
>
> Despite its negative-sounding name, git blame is actually pretty innocuous; its primary function is to point out who changed which lines in a file, and why. It can be a useful tool to identify changes in your code.

Basically, git-blame is used to show what revision and author last modified each line of a file. It's like checking the history of the development of a file.

`git blame -L 1,3 README.md` 显示 1-3 行最后一次修改信息

refer: 

* http://git-scm.com/docs/git-blame
* [What does 'git blame' do?](https://stackoverflow.com/questions/31203001/what-does-git-blame-do)

## 查看历史提交信息 - git log

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

## 对比差异 - git diff

```
# 只显示当前一行的差异，而不是三行
git diff -U0 HEAD^

-U<n>, --unified=<n>
           Generate diffs with <n> lines of context instead of the usual three. Implies --patch. Implies -p.
```



## 分支操作 - git branch

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


修改分支的名字：

``` bash
# Rename the local branch to the new name
git branch -m <old_name> <new_name>

# Delete the old branch on remote - where <remote> is, for example, origin
git push <remote> --delete <old_name>

# Or shorter way to delete remote branch [:]
git push <remote> :<old_name>

# Push the new branch to remote
git push <remote> <new_name>

# Reset the upstream branch for the new_name local branch
git push <remote> -u <new_name>
```


更多：

* [处理合并冲突](https://www.git-tower.com/learn/git/ebook/cn/command-line/advanced-topics/merge-conflicts)
* [How do I rename both a Git local and remote branch name?](https://stackoverflow.com/questions/30590083/how-do-i-rename-both-a-git-local-and-remote-branch-name/30590238#30590238)



## 变基操作 - git rebase

在Git中整合来自不同分支的修改主要有两种方法：merge 和 rebase

这两种整合方法的最终结果没有任何区别，但是变基使得提交历史更加整洁。 你在查看一个经过变基的分支的历史记录时会发现，尽管实际的开发工作是并行的，但它们看上去就像是串行的一样，提交历史是一条直线没有分叉。

一般我们这样做的目的是为了确保在向远程分支推送时能保持提交历史的整洁——例如向某个其他人维护的项目贡献代码时。 在这种情况下，你首先在自己的分支里进行开发，当开发完成时你需要先将你的代码变基到 origin/master 上，然后再向主项目提交修改。 这样的话，该项目的维护者就不再需要进行整合工作，只需要快进合并便可。

请注意，无论是通过变基，还是通过三方合并，整合的最终结果所指向的快照始终是一样的，只不过提交历史不同罢了。 变基是将一系列提交按照原有次序依次应用到另一分支上，而合并是把最终结果合在一起。

例子：你可以检出 experiment 分支，然后将它变基到 master 分支上：

```
$ git checkout experiment
$ git rebase master
First, rewinding head to replay your work on top of it...
Applying: added staged command
```

现在回到 master 分支，进行一次快进合并。

```
$ git checkout master
$ git merge experiment
```

更多：[Git 分支 - 变基](https://git-scm.com/book/zh/v2/Git-%E5%88%86%E6%94%AF-%E5%8F%98%E5%9F%BA)

## 暂存操作 - git stash

当前你在开发feature1分支，开发了一半，还要2天才能开发完成，这时候又不想提交。这时突然来了个bug，你必须今天就得修复bug，修复完了后才继续开发需求，怎么办？这里就使用到了暂存的功能。

``` bash
# 先把所有的修改暂存起来，这时候你的所有改动都好像消失了一样，但其实是被暂存起来了
git stash 
# 新建bugfix分支去修复bug
git checkout -b bugfix/分支名  
# 修复bug
# 回到原来的开发分支
git checkout feature1 
# 恢复暂存的内容
git stash pop 
# 继续开发需求

git stash list
git stash show 0
git stash pop 0
git stash drop 0
```

```
# git stash a specific file
git stash push -m welcome_cart app/views/cart/welcome.thtml
```

* https://stackoverflow.com/questions/5506339/how-can-i-git-stash-a-specific-file

## 子模块 - git submodule


克隆一个含有子模块的项目。当你在克隆这样的项目时，默认会包含该子模块目录，但其中还没有任何文件。你必须运行两个命令：`git submodule init` 用来初始化本地配置文件，而 `git submodule update` 则从该项目中抓取所有数据并检出父项目中列出的合适的提交。现在子目录是处在和之前提交时相同的状态了。不过还有更简单一点的方式。 如果给 `git clone` 命令传递 `--recurse-submodules` 选项，它就会自动初始化并更新仓库中的每一个子模块， 包括可能存在的嵌套子模块。如果你已经克隆了项目但忘记了 `--recurse-submodules`，那么可以运行 `git submodule update --init` 将 `git submodule init` 和 `git submodule update` 合并成一步。如果还要初始化、抓取并检出任何嵌套的子模块， 请使用简明的 `git submodule update --init --recursive`。


``` bash
# 含有子模块项目的clone
git clone --recurse-submodule --remote-submodules git@github.com:gerryyang/mac-utils.git mac-utils 

# 如果忘记了 --recurse-submodules
git submodule update --init --recursive # 等价于 git submodule init 和 git submodule update


# 添加子模块
git submodule add https://github.com/chaconinc/DbConnector
git clone --recursive https://github.com/chaconinc/MainProject

# 清除子模块
git submodule deinit --all
```

* [Git 工具 - 子模块](https://git-scm.com/book/zh/v2/Git-工具-子模块)
* [How to “git clone” including submodules](https://stackoverflow.com/questions/3796927/how-to-git-clone-including-submodules)
* [How do git submodules work?](https://matthew-brett.github.io/curious-git/git_submodules.html#how-do-git-submodules-work)

# Git hooks

```
$ .git/hooks$ls
applypatch-msg.sample  fsmonitor-watchman.sample  pre-applypatch.sample  pre-merge-commit.sample    pre-push.sample    pre-receive.sample       update.sample
commit-msg.sample      post-update.sample         pre-commit.sample      prepare-commit-msg.sample  pre-rebase.sample  push-to-checkout.sample
```

## pre-commit

通过`.git/hooks/pre-commit`可以在commit前执行自定义操作。通过`ln -s pre-commit ${PROJECT_ROOT}/.git/hooks/pre-commit`指定自定义 pre-commit 的脚本，之后 commit 时自动触发 pre-commit 脚本执行，如存在错误则会报错终止 commit，修复完毕后重新发起 commit 即可。以下是`pre-commit.sample`示例：

``` bash
#!/bin/sh
#
# An example hook script to verify what is about to be committed.
# Called by "git commit" with no arguments.  The hook should
# exit with non-zero status after issuing an appropriate message if
# it wants to stop the commit.
#
# To enable this hook, rename this file to "pre-commit".

if git rev-parse --verify HEAD >/dev/null 2>&1
then
        against=HEAD
else
        # Initial commit: diff against an empty tree object
        against=$(git hash-object -t tree /dev/null)
fi

# If you want to allow non-ASCII filenames set this variable to true.
allownonascii=$(git config --type=bool hooks.allownonascii)

# Redirect output to stderr.
exec 1>&2

# Cross platform projects tend to avoid non-ASCII filenames; prevent
# them from being added to the repository. We exploit the fact that the
# printable range starts at the space character and ends with tilde.
if [ "$allownonascii" != "true" ] &&
        # Note that the use of brackets around a tr range is ok here, (it's
        # even required, for portability to Solaris 10's /usr/bin/tr), since
        # the square bracket bytes happen to fall in the designated range.
        test $(git diff --cached --name-only --diff-filter=A -z $against |
          LC_ALL=C tr -d '[ -~]\0' | wc -c) != 0
then
        cat <<\EOF
Error: Attempt to add a non-ASCII file name.

This can cause problems if you want to work with people on other platforms.

To be portable it is advisable to rename the file.

If you know what you are doing you can disable this check using:

  git config hooks.allownonascii true
EOF
        exit 1
fi

# If there are whitespace errors, print the offending file names and fail.
exec git diff-index --check --cached $against --
```

实际使用的例子：

``` bash
#!/bin/bash

<< COMMENT
the scropt for git pre-commit
Usage: $0 $FILE
COMMENT

CUR_DIR=$(dirname $(readlink -f $0))
echo $CUR_DIR

RESULT=0
NEEDCHECK=0

CPPLINT="$CUR_DIR/cpplint.sh"

function CHECK_LOG()
{
    if [ $? -ne 0 ]; then
        echo -e "\033[031;1m[FAILED]\033[0m $1"
    else
        echo -e "\033[032;1m[PASS]\033[0m $1"
    fi
}

echo "pre-commit begin"

if [ ${CODE_CHECK_IGNORE:-0} -eq 1 ]; then
    echo "Ignore code check"
    exit 0
fi

GIT_DIFF=$(git diff --cached --name-status)
while read STATUS FILE; do 

    if [ "$STATUS" == "D" ]; then
        continue;
    fi

    if [ -z $(expr "$f" : ".*\.\(h\|hpp\|cc\|cpp\)$") ]; then
        continue
    fi

    NEEDCHECK=1

    echo "Check cpp style: $FILE"
    $CPPLINT $FILE
    if [ $? -ne 0 ]; then
        CHECK_LOG "$CPPLINT $FILE failed"
        RESULT=1
    else
        CHECK_LOG "$CPPLINT $FILE"
    fi

done <<< "$GIT_DIFF"

if [ $NEEDCHECK -eq 0 ]; then
    CHECK_LOG "No file needs to check"

elif [ $RESULT -gt 0 ]; then
    CHECK_LOG "pre-commit failed, need to check before next commit"
    exit 1
fi

echo "pre-commit end"
```



# Git LFS的原理

存储和管理大型文件的版本，是一个由来已久的难题。 由于需要记录全部历史版本，在托管比较大的文件时，以往的代码仓库往往不太给力。

* SVN。SVN 会把每次提交的文件差异存储下来，当用户新拉取一个文件时，需要取得所有的差异集，并把他们拼装相加，再返回一个完整的文件给用户。如果仅是满足代码文件（文本）的需要，这种机制是十分顺畅的。但当图片、音视频、数据集、程序包、二进制文件时，状况就会开始恶化。如果版本数量较少，计算机凭借优异的性能，可以做到用户无感知。但当版本较多，文件很大时，CPU的计算资源会逐渐成为瓶颈，其体现就是 SVN CO 速度越来越慢。慢并非是网络传输造成的，而是版本数、碎片、文件大小共同作用，使CPU和IO到达瓶颈带来的。

* Git。Git 虽然没有 SVN 合并差异计算的问题，相比 SVN 利用空间替换了 CPU 的计算时间。但由于分布式要求，客户端往往会克隆整个仓库来本地，也包含了文件历史。如果历史版本较多，用户就要等每个版本从网络传输到本地。因此，用户克隆的时间包含了下载所有历史版本到本地的时间。如果文件比较大，会变成所有人的梦魇。

向服务器提交时，Git LFS把文件上传到一个专用的存储区域，同时保存一份指针（文本文件）在原有的版本库中。 当另外一个人拉取，他将首先拉取到指针，此时Git LFS解析这个指针，并把实际文件内容取回到本地。 文件每有一个新版本时，就会创建一个新指针，并向服务器上传，这样服务器上就有了多个历史版本，从而实现了基本的版本管理。

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
   IdentityFile ~/.ssh/oa_id_rsa

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

# GitHub 常用缩写

| 缩写 | 全称 | 含义
| -- | -- | -- 
| PR | Pull Request | 拉取请求，给其他项目提交代码
| MR | Merge Request | 合并请求
| LGTM | Looks Good To Me | 可以合并，没有问题
| SGTM | Sounds Good to Me | 可以合并
| WIP | Work In Progress | 开发中
| PTAL | Please Take A Look | 请求维护者review
| TBR | To Be Reviewed | 请求维护者review
| TL;DR | Too Long; Didn't Read | 太长懒得看
| TBD | To Be Done/Defined/Discussed/Decided/Determined | 待定
| AFAIK | As far as I know | 据我所知
| IIRC | If I recall correctly | 如果我没记错的话
| CC | Carbon Copy | 抄送
| ACK | Acknowledgement | 同意
| NACK/NAK | Negative Acknowledgement | 不同意
| TYPO | | 拼写错误
| WFM | Works For Me | 测试结果对我有效
| MUST | | 必须修改
| Duplicate of #123 | | 关闭重复的issue
| close/closes/closed #issue | | 关闭issue
| fix/fixes/fixed #issue | | 修复issue
| resolve/resolves/resolved | | 解决issue

# Issue 规范

* Duplication Issue

关闭重复的Issue，在重复Issue中通过`Duplicate of #123`关联到当前Open的Issue，并把重复Issue中有价值的信息合并到open的Issue中。

* One issue per issue

每个Issue的信息尽量独立，过多的问题可以分在不同的Issue中。

* Titles are important

Issue的标题尽量精简易懂。

* Formatting

良好的Issue格式可以帮助他人快速理解问题，比如，合理地使用Markdown，可参考：[Basic writing and formatting syntax](https://docs.github.com/en/github/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax)

* Issue as user story

提供额外的[user stroy](https://en.wikipedia.org/wiki/User_story)可以更好的理解问题

* Objectivity

问题的描述尽量客观真实，比如，可以提供一些测试数据。

* Reproduction steps

针对bug的Issue，提供问题可复现的步骤

# Commit 规范

commit消息结构：

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

允许的提交类型type：

```
feat -> feature
fix -> bug fix
docs -> documentation
style -> formatting, lint stuff
refactor -> code restructure without changing exterrnal behavior
test -> adding missing tests
chore -> maintenance
init -> initial commit
rearrange -> files moved, added, deleted etc
update -> update code (versions, library compatibility)
```

refer:

* [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/)
* [Git Commit Style Guide](https://gist.github.com/ericavonb/3c79e5035567c8ef3267)

# Merge Request流程

MR场景：MR提交者对项目`A`进行fork，生成自己的项目`B`。然后在`B`项目上开发功能，然后创建`MR请求`，向项目`A`的Owner申请将项目`B`修改的代码再合入到项目`A`中。

> TL;DR
> 1. MR提交者，先fork要开发的原始项目`A`，生成自己的项目`B`
> 2. MR提交者，在fork的项目`B`上checkout创建新的`dev`开发分支，并在此分支上开发新的功能
> 3. 当MR提交者需要将项目`B`中改动的代码合入项目`A`时，需要发起`Merge Request`流程
>    3.1 MR提交者，先将原始项目`A`的代码同步到自己的项目`B`的`master`分支
>    3.2 MR提交者，再将自己项目`B`开发的`dev`分支提交记录进行`rebase`（将多次commit记录进行精简合并）
>    3.3 MR提交者，最后将自己项目`B`的`dev`分支merge到`master`分支，并解决可能产生的conflicts (此流程会保证MR提交者commit的记录在原始项目commit记录的上面)
>    3.4 MR提交者，提交MR请求等待Owner审核
> 4. MR审批者，执行Comment和Approve，`Create a merge commit`完成代码合入。


## MR发起者处理流程

* Before you can sync your fork with an upstream repository, you must [configure a remote that points to the upstream repository](https://docs.github.com/en/github/collaborating-with-pull-requests/working-with-forks/configuring-a-remote-for-a-fork) in Git.

```
# List the current configured remote repository for your fork
$ git remote -v
> origin  https://github.com/YOUR_USERNAME/YOUR_FORK.git (fetch)
> origin  https://github.com/YOUR_USERNAME/YOUR_FORK.git (push)

# Specify a new remote upstream repository that will be synced with the fork
$ git remote add upstream https://github.com/ORIGINAL_OWNER/ORIGINAL_REPOSITORY.git

# Verify the new upstream repository you've specified for your fork
$ git remote -v
> origin    https://github.com/YOUR_USERNAME/YOUR_FORK.git (fetch)
> origin    https://github.com/YOUR_USERNAME/YOUR_FORK.git (push)
> upstream  https://github.com/ORIGINAL_OWNER/ORIGINAL_REPOSITORY.git (fetch)
> upstream  https://github.com/ORIGINAL_OWNER/ORIGINAL_REPOSITORY.git (push)
```

* 完成本地项目代码与原始项目代码的同步，更新本地项目master代码为最新的代码。[Merge an upstream repo](https://docs.github.com/en/github/collaborating-with-pull-requests/working-with-forks/merging-an-upstream-repository-into-your-fork)

```
# 切换到本地项目master分支
git checkout master

# fetch最新的代码到 upstream/master
git fetch upstream

# 合并upstream的修改到master分支
git merge upstream/master

# 上述步骤也可以简化为git pull命令，拉取远程项目A代码并合并到本地master分支，
git pull upstream master

# 再切换到dev分支，执行rebase合并master代码，如果有代码冲突，则需要先解决
git checkout dev
git rebase master

# 再切换到master分支，合并dev分支的代码
git checkout master
git merge dev

# 将本地master分支的多次commit提交记录合并，比如，合并前7条提交记录，通过`-i`交互操作中将不需要的commit记录从`pick`改为`squash`，保留第一条commit为pick
git rebase -i HEAD~7

# 提交master修改到远端
git push origin master

# 最后提交MR请求
```

* 创建`Merge Request`

* `Source branch`为自己fork项目的`master`分支，`Target branch`为实际项目`master`分支

* 填写`Title`，`Description`信息，最后提交。

## MR审批者处理流程

* 收到`Merge Request`处理请求。
* 选择`Create a merge commit`。通常选择，`Squash and merge`或`Rebase and merge`。
* 进行`Code Review`，若发现问题则通过comment反馈给提交者进行修改，开发者修改后`Close MR`，再`Reopen MR`。
* 没有问题，确认完成MR。
* 若MR过程出现代码冲突，则MR审批者选择`Close Merge request`。由MR发起者解决冲突后再重新提交MR。

refer:

* [Writing a proper GitHub issue](https://medium.com/nyc-planning-digital/writing-a-proper-github-issue-97427d62a20f)
* [About issue and pull request templates](https://docs.github.com/en/communities/using-templates-to-encourage-useful-issues-and-pull-requests/about-issue-and-pull-request-templates)
* [Configuring issue templates for your repository](https://docs.github.com/en/communities/using-templates-to-encourage-useful-issues-and-pull-requests/configuring-issue-templates-for-your-repository)
* [About issue and pull request templates](https://docs.github.com/en/communities/using-templates-to-encourage-useful-issues-and-pull-requests/about-issue-and-pull-request-templates)
* [Setting guidelines for repository contributors](https://docs.github.com/en/communities/setting-up-your-project-for-healthy-contributions/setting-guidelines-for-repository-contributors)
* [Creating a pull request template for your repository](https://docs.github.com/en/communities/using-templates-to-encourage-useful-issues-and-pull-requests/creating-a-pull-request-template-for-your-repository)

* [Creating a pull request](https://docs.github.com/en/github/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request)
* [Linking a pull request to an issue](https://docs.github.com/en/issues/tracking-your-work-with-issues/linking-a-pull-request-to-an-issue)
* [Autolinked references and URLs](https://docs.github.com/en/github/writing-on-github/working-with-advanced-formatting/autolinked-references-and-urls#issues-and-pull-requests)

# Git相关工具

* Sourcetree

A free Git client for Windows and Mac. Sourcetree simplifies how you interact with your Git repositories so you can focus on coding. Visualize and manage your repositories through Sourcetree's simple Git GUI.

https://www.sourcetreeapp.com/

*  TortoiseGit

TortoiseGit is a Windows Shell Interface to Git and based on TortoiseSVN. It's open source and can fully be build with freely available software.

https://tortoisegit.org/

# Q&A

* https://stackoverflow.com/questions/1580596/how-do-i-make-git-ignore-file-mode-chmod-changes


# Refer

* [Scott Chacon, Ben Straub - Pro Git-Apress (2014)](https://git-scm.com/book/en/v2)
* [图解Git](https://marklodato.github.io/visual-git-guide/index-zh-cn.html)
* [阮一峰: 版本控制入门插图教程](http://www.ruanyifeng.com/blog/2008/12/a_visual_guide_to_version_control.html)
* [阮一峰: Git远程操作详解](http://www.ruanyifeng.com/blog/2014/06/git_remote.html)
* [这才是真正的GIT - GIT原理及实用技巧 - PPT](https://www.lzane.com/slide/git-under-the-hood/index.html#/)
* [这才是真正的 Git（freeCodeConf 2019 深圳站）- video](https://www.bilibili.com/video/av77252063)
* [廖雪峰: Git教程](https://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000)



