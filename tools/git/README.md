
Abstract
==
版本控制演变：
cvs：始祖，集中式（每次提交都需要联网，效率低），1985
svn：集大成者，集中式（每次提交都需要联网，效率低），2000
git：Linux之父Linus开发的，geek主流，分布式（不需要联网，可以在本地提交），2005
github：geek社区，托管网站，2008

如何回到过去：
git log
git reflog
git reset --hard $commit

多人合作的经验：
1，合理利用工具，以提高效率为目的；
2，每次提交前，diff自己的代码，以免提交错误的代码；
3，下班前整理好自己的工作区；
4，并行的项目使用分支开发；
5，遇到冲突时，搞明白冲突的原因，千万不要随意丢弃别人的代码；
6，产品发布后，记得打tag，方便将来拉分支修复bug；


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


2 useful git commands
===

(0)  wget --no-check-certificate https://github.com/gerryyang/mac-utils/archive/master.zip

(1) git clone https://github.com/gerryyang/mac-utils.git

(2) git add -A

-A, --all, --no-ignore-removal

Update the index not only where the working tree has a file matching <pathspec> but also where the index already has an entry. This adds, modifies, and removes index entries to match the working tree.

(3) git status

(4) git commit -m"update info"

(5) git push origin master

(6) git log / git reflog

(7) git branch -a
显示所有分支

(8) git checkout <branch-name>
切换到某个分支

(9) git submodule add https://github.com/miloyip/rapidjson ./deps/rapidjson
添加外部引用模块


3 Reference
===

[1] Git In Two Minutes (for a solo developer) http://www.garyrobinson.net/2014/10/git-in-two-minutes-for-a-solo-developer.html

[2] Git In Five Minutes http://classic.scottr.org/presentations/git-in-5-minutes/

[3] My GitHub Resume http://resume.github.io/

[4] 版本管理工具介绍—Git篇 http://www.imooc.com/view/208

[5] [阮一峰的Git 使用规范流程](http://www.ruanyifeng.com/blog/2015/08/git-use-process.html)
