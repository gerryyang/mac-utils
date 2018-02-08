
Abstract
==
> 版本控制演变

* cvs：始祖，集中式（每次提交都需要联网，效率低），1985
* svn：集大成者，集中式（每次提交都需要联网，效率低），2000
* git：Linux之父Linus开发的，geek主流，分布式（不需要联网，可以在本地提交），2005
* github：geek社区，托管网站，2008


0 git help
===

'git help -a' and 'git help -g' lists available subcommands and some
concept guides. See 'git help <command>' or 'git help <concept>'
to read about a specific subcommand or concept.


1 Create a new repository on the command line
===

mkdir your_repo; cd your_repo

touch README.md

git init

git add README.md

git commit -m "first commit"

\# git remote add origin https://github.com/gerryyang/your_repo.git

git remote rm origin

git remote add origin git@github.com:gerryyang/your_repo.git

git push -u origin master


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

(4) 显示所有分支
```
git branch -a
```

(5) 切换到某个分支
```
git checkout <branch-name>
```

(6) 添加外部引用模块
```
git submodule add https://github.com/miloyip/rapidjson ./deps/rapidjson
```

3 Reference
===

[1] Git In Two Minutes (for a solo developer) http://www.garyrobinson.net/2014/10/git-in-two-minutes-for-a-solo-developer.html

[2] Git In Five Minutes http://classic.scottr.org/presentations/git-in-5-minutes/

[3] My GitHub Resume http://resume.github.io/

[4] 版本管理工具介绍—Git篇 http://www.imooc.com/view/208

[5] [阮一峰的Git 使用规范流程](http://www.ruanyifeng.com/blog/2015/08/git-use-process.html)

[6] [Pro Git book](https://git-scm.com/book/zh/v1) **TODO**