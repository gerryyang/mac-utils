---
layout: post
title:  "Git in Action"
date:   2018-10-15 13:00:00 +0800
categories: tools
---

作为程序员日常离不开对代码的管理，Git 采用了分布式版本库的方式，这是它与常用的版本控制工具例如  CVS，Subversion 等很大的不同之处。使用Git，让源代码的发布和团队间的交流极其方便，并且 Git 的速度很快，对于大项目来说尤为重要，这也是为什么越来越多的科技公司都使用 Git 进行代码管理。

版本控制演变：

* cvs：始祖，集中式（每次提交都需要联网，效率低），1985
* svn：集大成者，集中式（每次提交都需要联网，效率低），2000
* git：Linux之父Linus开发的，geek主流，分布式（不需要联网，可以在本地提交），2005
* github：geek社区，托管网站，2008

# [Git Basics]

如果要学习Git，请先忘记之前你知道的关于其他VCSs的用法（例如，svn），这样可以帮助你更好地理解和使用Git，因为Git的设计理念和其他的VCSs完全不同。下图非常形象地解释了二者的不同。

![two_ways_vcs](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/two_ways_vcs.jpg)

```
So, what is Git in a nutshell? This is an important section to absorb, because if you understand what Git is and the fundamentals of how it works, then using Git effectively will probably be much easier for you. As you learn Git, try to clear your mind of the things you may know about other VCSs, such as Subversion and Perforce; doing so will help you avoid subtle confusion when using the tool. Git stores and thinks about information much differently than these other systems, even though the user interface is fairly similar; understanding those differences will help prevent you from becoming confused while using it.
```

## Snapshots, Not Differences 

Git使用版本快照，而非版本差异。

```
The major difference between Git and any other VCS (Subversion and friends included) is the way Git thinks about its data. Conceptually, most other systems store information as a list of file-based changes. These systems (CVS, Subversion, Perforce, Bazaar, and so on) think of the information they keep as a set of files and the changes made to each file over time, as illustrated in Figure 1-4.

Git doesn’t think of or store its data this way. Instead, Git thinks of its data more like a set of snapshots of a mini filesystem. Every time you commit, or save the state of your project in Git, it basically takes a picture of what all your files look like at that moment and stores a reference to that snapshot. To be efficient, if files have not changed, Git doesn’t store the file again—just a link to the previous identical file it has already stored. Git thinks about its data more like Figure 1-5.

This is an important distinction between Git and nearly all other VCSs. It makes Git reconsider almost every aspect of version control that most other systems copied from the previous generation. This makes Git more like a mini filesystem with some incredibly powerful tools built on top of it, rather than simply a VCS. We’ll explore some of the benefits you gain by thinking of your data this way when we cover Git branching in Chapter 3.
```

![git_svn_diff1](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/git_svn_diff1.jpg)
![git_svn_diff2](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/git_svn_diff2.jpg)

## Nearly Every Operation Is Local

当你使用Git的时候，如果要浏览历史版本，是不需要进行网络交互的，因为历史版本的信息都保存在本地，因此，你可以在任何时间任何地点（即离线状态）进行你的工作，到可以使用网络的时候再对你之前的所有工作进行提交。而对比其他VCSs，在离线的状态下这些工作都是不可能完成的。

```
Most operations in Git only need local files and resources to operate — generally no information is needed from another computer on your network. If you’re used to a CVCS where most operations have that network latency overhead, this aspect of Git will make you think that the gods of speed have blessed Git with unworldly powers. Because you have the entire history of the project right there on your local disk, most operations seem almost instantaneous.

For example, to browse the history of the project, Git doesn’t need to go out to the server to get the history and display it for you—it simply reads it directly from your local database. This means you see the project history almost instantly. If you want to see the changes introduced between the current version of a file and the file a month ago, Git can look up the file a month ago and do a local difference calculation, instead of having to either ask a remote server to do it or pull an older version of the file from the remote server to do it locally.

This also means that there is very little you can’t do if you’re offline or off VPN. If you get on an airplane or a train and want to do a little work, you can commit happily until you get to a network connection to upload. If you go home and can’t get your VPN client working properly, you can still work. In many other systems, doing so is either impossible or painful. In Perforce, for example, you can’t do much when you aren’t connected to the server; and in Subversion and CVS, you can edit files, but you can’t commit changes to your database (because your database is offline). This may not seem like a huge deal, but you may be surprised what a big difference it can make.
```

## Git Has Integrity

Git对所有存储的文件通过SHA-1计算散列值来索引文件，以保证文件的完整性。

```
verything in Git is check-summed before it is stored and is then referred to by that checksum. This means it’s impossible to change the contents of any file or directory without Git knowing about it. This functionality is built into Git at the lowest levels and is integral to its philosophy. You can’t lose information in transit or get file corruption without Git being able to detect it.

The mechanism that Git uses for this checksumming is called a SHA-1 hash. This is a 40-character string composed of hexadecimal characters (0–9 and a–f) and calculated based on the contents of a file or directory structure in Git. A SHA-1 hash looks something like this:

24b9da6552252987aa493b52f8696cd6d3b00373

You will see these hash values all over the place in Git because it uses them so much. In fact, Git stores everything not by file name but in the Git database addressable by the hash value of its contents.
```

## Git Generally Only Adds Data

在操作Git时只有添加文件的概念，而没有删除的概念。

```
When you do actions in Git, nearly all of them only add data to the Git database. It is very difficult to get the system to do anything that is not undoable or to make it erase data in any way. As in any VCS, you can lose or mess up changes you haven't committed yet; but after you commit a snapshot into Git, it is very difficult to lose, especially if you regularly push your database to another repository.

This makes using Git a joy because we know we can experiment without the danger of severely screwing things up. For a more in-depth look at how Git stores its data and how you can recover data that seems lost, see Chapter 9.
```

## The Three States

请记住Git中的三种状态：

1. committed (已提交到**本地**数据库)
2. modified (已修改但未提交)
3. staged (对修改进行了标记以待提交)

![git_local_operation](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201810/git_local_operation.jpg)

```
Now, pay attention. This is the main thing to remember about Git if you want the rest of your learning process to go smoothly. Git has three main states that your files can reside in: committed, modified, and staged. Committed means that the data is safely stored in your local database. Modified means that you have changed the file but have not committed it to your database yet. Staged means that you have marked a modified file in its current version to go into your next commit snapshot.

This leads us to the three main sections of a Git project: the Git directory, the working directory, and the staging area.

**The Git directory** is where Git stores the metadata and object database for your project. This is the most important part of Git, and it is what is copied when you clone a repository from another computer.

**The working directory** is a single checkout of one version of the project. These files are pulled out of the compressed database in the Git directory and placed on disk for you to use or modify.

**The staging area** is a simple file, generally contained in your Git directory, that stores information about what will go into your next commit. It’s sometimes referred to as the index, but it’s becoming standard to refer to it as the staging area.

**The basic Git workflow goes something like this:**

1. You modify files in your working directory.
2. You stage the files, adding snapshots of them to your staging area.
3. You do a commit, which takes the files as they are in the staging area and stores that snapshot permanently to your Git directory.

If a particular version of a file is in the git directory, it’s considered committed. If it’s modified but has been added to the staging area, it is staged. And if it was changed since it was checked out but has not been staged, it is modified. In Chapter 2, you’ll learn more about these states and how you can either take advantage of them or skip the staged part entirely.
```

# 安装和配置Git

以MacOS上配置Git为例。

[Git下载地址]，由于Mac上已经默认安装了Git，因此这里就不需要了。注意Git是一个命令行工具，它没有华丽的GUI界面，所以你只能在Terminal里使用它而不会在Mac的dock上找到它。

```
$git --version
git version 1.8.3.4 (Apple Git-47)
```

安装后，对Git进行配置，通过命令行的方式：

```
git config --global user.name "Your Name Here"
# Sets the default name for git to use when you commit

git config --global user.email "your_email@example.com"
# Sets the default email for git to use when you commit
```

注意上面的设置是对全局生效的，若想对某个repo使用不同的username或email可以通过下面方法进行设置：

```
cd my_other_repo
# Changes the working directory to the repository you need to switch info for
git config user.name "Different Name"
# Sets the user's name for this specific repository
git config user.email "differentemail@email.com"
# Sets the user's email for this specific repository
```

设置Password caching，帮助我们缓存用户名和密码，方便我们使用Git：

```
git credential-osxkeychain
# Test for the cred helper
# Usage: git credential-osxkeychain <get|store|erase>

git config --global credential.helper osxkeychain
# Set git to use the osxkeychain credential helper
```

```
The next time you clone an HTTPS URL that requires a password you will be prompted for your username and password, and to grant access to the OSX keychain. After you've done this, the username and password are stored in your keychain and you won't be required to type them in to git again.

Tip: The credential helper only works when you clone an HTTPS repository URL. If you use the SSH repository URL instead, SSH keys are used for authentication.This guide([Connecting to GitHub with SSH]) offers help generating and using an SSH key pair.
```



# Refer

1. [Getting Started - About Version Control]
2. [Git Basics]

[Getting Started - About Version Control]: https://git-scm.com/book/en/v2/Getting-Started-About-Version-Control

[Git Basics]: https://git-scm.com/book/en/v2/Getting-Started-Git-Basics

[Git下载地址]: http://git-scm.com/downloads

[Connecting to GitHub with SSH]: https://help.github.com/articles/connecting-to-github-with-ssh/