
git commands
===
===

# 0 git help

'git help -a' and 'git help -g' lists available subcommands and some
concept guides. See 'git help <command>' or 'git help <concept>'
to read about a specific subcommand or concept.

===

#1 Create a new repository on the command line

mkdir your_repo; cd your_repo

touch README.md

git init

git add README.md

git commit -m "first commit"

\# git remote add origin https://github.com/gerryyang/your_repo.git

git remote rm origin

git remote add origin git@github.com:gerryyang/your_repo.git

git push -u origin master

===

#2 useful git commands

(1) git clone https://github.com/gerryyang/mac-utils.git

(2) git add -A

-A, --all, --no-ignore-removal

Update the index not only where the working tree has a file matching <pathspec> but also where the index already has an entry. This adds, modifies, and removes index entries to match the working tree.

(3) git status

(4) git commit -m"update info"

(5) git push origin master


===
#3 ended
