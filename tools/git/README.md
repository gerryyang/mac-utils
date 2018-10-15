
Abstract
==
> 版本控制演变

* cvs：始祖，集中式（每次提交都需要联网，效率低），1985
* svn：集大成者，集中式（每次提交都需要联网，效率低），2000
* git：Linux之父Linus开发的，geek主流，分布式（不需要联网，可以在本地提交），2005
* github：geek社区，托管网站，2008


0 git help
===

`git help -a` and `git help -g` lists available subcommands and some
concept guides. See 'git help <command>' or 'git help <concept>'
to read about a specific subcommand or concept.


1 Create a new repository on the command line
===

```
mkdir your_repo; cd your_repo

touch README.md

git init

git add README.md

git commit -m "first commit"

git remote add origin https://github.com/gerryyang/your_repo.git

git remote rm origin

git remote add origin git@github.com:gerryyang/your_repo.git

git push -u origin master
```

2 Useful git commands
===

(1) 克隆到本地
```
wget --no-check-certificate https://github.com/gerryyang/mac-utils/archive/master.zip
git clone https://github.com/gerryyang/mac-utils.git
```

(2) 提交变更
```
git status      // 查看本地代码状态
git reflog      // 查看操作记录

// --- 新增 commit ---
git add .                   // 所有文件添加到暂存区
git commit -m '提交备注'    // 添加文件到缓存区，然后提交到本地仓库


// --- 撤销 commit ---
// 会将提交记录回滚，代码不回滚
git reset b14bb52

// 会将提交记录和代码全部回滚
git reset --hard b14bb52

// 将部分代码文件回滚
git checkout -- files


// --- 合并 commit ---
// 合并 commit，本质上合并两份不同状态下的代码
// Git 提供了两种合并 commit 的方式，那么 git rebase 和 git merge 到底有什么区别呢？
// merge是两个分支处理冲突后，新增一个 commit 追加到master上
// rebase是将someFeature分支上的commit记录追加到主分支上，值得注意的是，这个时候他的commit其实已经发生变化
// 相对来说，git merge 处理冲突更直接，而git rebase 能够保证清晰的 commit 记录
git merge master
git rebase master


// 都是先 git fetch，然后执行合并操作
// 不同的是，git pull 执行的是 git merge，git pull -r 执行的是git rebase
git pull origin master 
git pull -r origin master

git pull -r origin <branch-name> // fetch远端代码到本地，并且以rebase的方式合并代码
git push origin <branch-name>    // 更新本地代码到远端

git checkout .  // 撤销所有

// 基于功能分支的开发流程：用分支来承载功能开发，开发结束之后就合并到 master 分支
// 此流程的优点是能够保证master分支的整洁，同时还能让分支代码逻辑集中，也便于CodeReview
// 注意，不要在master合并代码，保证master的可用性很重要
git checkout <branch-name>
git pull -r origin master // 将master的代码更新下来，并且rebase处理冲突
git push origin master    // 将本地代码更新到远端
```

(3) 回到过去
```
git log
git reflog
git reset --hard $commit
```

(4) 分支操作
```
// 所谓的分支其实就是一个指向 commitID 的指针，你可以去.git/refs/heads里去看看
// 一般情况下，我们应该从master或者其他稳定分支来新增分支

git branch     // 显示local分支
git branch -r  // 显示remote分支
git branch -a  // 显示所有分支

git branch -d                     // 删除本地分支
git branch -D                     // 强制删除本地分支
git push origin -d <branch-name>  // 删除远端分支

git fetch -p   // 同步远端分支状态
```

(5) 新建和切换到某个分支

```
git checkout <branch-name>    // 切换到该分支
git checkout -b <branch-name> // 新建并切换到该分支
```

等价于

```
git branch <branch-name>
git checkout <branch-name>
```

合并分支

```
git checkout master          // 先切回master分支  
git merge <branch-name>      // 合并分支, 有两种情况：(1) master和branch没有分叉, 则直接Fast forward (2) 有分叉, Git会用两个分支的末端以及它们的共同祖先, 进行一次简单的三方合并计算。最后对三方合并后的结果重新做一个新的快照，并自动创建一个指向它的提交对象
git branch -d <branch-name>  // 合并后的分支若不需要也可以删除
```

创建远程分支(本地分支push到远程)

```
git push origin <branch-name>
```

遇到冲突时的分支合并。有时候合并操作并不会如此顺利。如果在不同的分支中都修改了同一个文件的同一部分，Git就无法干净地把两者合到一起，这种问题只能由人来裁决。例如：

```
git merge <branch-name>
Auto-merging index.html
CONFLICT (content): Merge conflict in index.html
Automatic merge failed; fix conflicts and then commit the result.
```

Git作了合并，但没有提交，它会停下来等你解决冲突。要看看哪些文件在合并时发生冲突，可以用 git status 查阅

```
git status
On branch master
You have unmerged paths.
  (fix conflicts and run "git commit")

Unmerged paths:
  (use "git add <file>..." to mark resolution)

        both modified:      index.html

no changes added to commit (use "git add" and/or "git commit -a")
```

任何包含未解决冲突的文件都会以未合并（unmerged）的状态列出。Git 会在有冲突的文件里加入标准的冲突解决标记，可以通过它们来手工定位并解决这些冲突。可以看到此文件包含类似下面这样的部分：

```
<<<<<<< HEAD
<div id="footer">contact : email.support@github.com</div>
=======
<div id="footer">
  please contact us at support@github.com
</div>
>>>>>>> iss53
```

可以看到 ======= 隔开的上半部分，是 HEAD（即 master 分支，在运行 merge 命令时所切换到的分支）中的内容，下半部分是在 iss53 分支中的内容。解决冲突的办法无非是二者选其一或者由你亲自整合到一起。

在解决了所有文件里的所有冲突后，运行 git add 将把它们标记为已解决状态，实际上就是来一次快照保存到暂存区域。因为一旦暂存，就表示冲突已经解决。如果你想用一个有图形界面的工具来解决这些问题，不妨运行 git mergetool，它会调用一个可视化的合并工具并引导你解决所有冲突。再运行一次 git status 来确认所有冲突都已解决。如果觉得满意了，并且确认所有冲突都已解决，也就是进入了暂存区，就可以用 git commit 来完成这次合并提交。


(6) 添加外部引用模块

git submodule && git subtree，管理第三方模块
这两个命令通常用来管理公用的第三方模块。比如一些通用的底层逻辑、中间件、还有一些可能会频繁变化的通用业务组件。
当然，两者还是有区别的。
git submodule 主要用来管理一些单向更新的公共模块或底层逻辑。
git subtree 对于部分需要双向更新的可复用逻辑来说，特别适合管理。比如一些需要复用的业务组件代码。

```
git submodule add https://github.com/miloyip/rapidjson ./deps/rapidjson
```

(7) 简化 Git 命令

可以通过配置 git alias 来简化需要输入的 Git 命令。
比如前文的 git subtree 需要输入很长的 Git 命令，我们可以配置 .git/config 文件来解决。

```
// git stpull appfe demo/xxx
// git stpush appfe demo/xxx
[alias]
    stpull = !git subtree pull --prefix=$1 appfe $2 \
        && :
    stpush = !git subtree pull --prefix=$1 appfe $2 \
        && git subtree split --rejoin --prefix=$1 $2 \
        && git subtree push --prefix=$1 appfe $2 \
        && :
```


Some tools
===

1. TOC

[GitHub Wiki TOC generator](https://ecotrust-canada.github.io/markdown-toc/)

[github-markdown-toc](https://github.com/ekalinin/github-markdown-toc)

https://coderwall.com/p/fy05hq/how-to-make-beautiful-github-readme

Reference
===

[1] Git In Two Minutes (for a solo developer) http://www.garyrobinson.net/2014/10/git-in-two-minutes-for-a-solo-developer.html

[2] Git In Five Minutes http://classic.scottr.org/presentations/git-in-5-minutes/

[3] My GitHub Resume http://resume.github.io/

[4] 版本管理工具介绍—Git篇 http://www.imooc.com/view/208

[5] [阮一峰的Git 使用规范流程](http://www.ruanyifeng.com/blog/2015/08/git-use-process.html)

[6] [Pro Git book](https://git-scm.com/book/zh/v1) 

[7] https://git-scm.com/book/zh/v1/Git-分支-分支的新建与合并

[8] [廖雪峰的 git 教程](https://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000)

[9] https://gitee.com/progit/8-Git-与其他系统.html#8.2-迁移到-Git

[10] [Pro Git（中文版）](https://gitee.com/progit/index.html)