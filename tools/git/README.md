
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
git add -A
git status
git commit -m"update info"
git push origin master
```

(3) 回到过去
```
git log
git reflog
git reset --hard $commit
```

(4) 显示分支信息
```
git branch     // 显示local分支
git branch -r  // 显示remote分支
git branch -a  // 显示所有分支
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
```
git submodule add https://github.com/miloyip/rapidjson ./deps/rapidjson
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

