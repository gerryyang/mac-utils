
Abstract
==
> git commands


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

(6) git log

3 Reference
===

[1] Git In Two Minutes (for a solo developer) http://www.garyrobinson.net/2014/10/git-in-two-minutes-for-a-solo-developer.html

[2] Git In Five Minutes http://classic.scottr.org/presentations/git-in-5-minutes/

[3] My GitHub Resume http://resume.github.io/

[4] 版本管理工具介绍—Git篇 http://www.imooc.com/view/208

[5] [阮一峰的Git 使用规范流程](http://www.ruanyifeng.com/blog/2015/08/git-use-process.html)
