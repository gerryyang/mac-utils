---
layout: post
title:  "VS Code in Action"
date:   2021-04-09 08:00:00 +0800
categories: [VS Code,]
---

* Do not remove this line (it will not be displayed)
{:toc}


# VS Code in Action

VS Code [下载地址](https://code.visualstudio.com/)，当前版本：1.55 版本，2021-04

## Using C++ on Linux in VS Code

In this tutorial, you will configure Visual Studio Code to use the `GCC C++ compiler (g++)` and `GDB` debugger on Linux. GCC stands for GNU Compiler Collection; GDB is the GNU debugger.

https://code.visualstudio.com/docs
https://code.visualstudio.com/docs/cpp/config-linux


## VS Code Remote 配置

在VS Code的应用商店搜索`Remote Development`插件并安装。这是一个插件包，会同时安装`Remote Container`，`Remote SSH`，`Remote-WSL`几个组件。安装后VS Code侧边栏会多出一个`Remote Development`的图标。

### Remote-SSH

* 在`~/.ssh/`目录执行`ssh-keygen -t rsa -f vscode_id_rsa`命令生成公私钥。
* 将`公钥`复制到Remote机器的`~/.ssh/authorized_keys`文件中。
* VS Code和VS Code Server的通信走SSH通道，需要开启SSH端口转发。打开Remote机器上的`/etc/ssh/sshd_config`文件 ，将`AllowTcpForwarding`选项改成`yes`，然后重启sshd服务`service sshd restart`。
* 在`~/.ssh/`目录下添加或修改`config`配置。
```
# VS Code Remote Development
Host $ip_ssh_config
  User $user_name
  HostName $remote_ip
  Port $ssh_port
  IdentityFile ~/.ssh/vscode_id_rsa
```

* 执行`Remote-SSH`->`Remote-SSH: Connect to Host…`->`配置的Remote Host`，第一次连接默认会在Remote机器`~/.vscode-server`目录下安装`VS Code Server`。

```
# Remote Server
~/.vscode-server$du -csh *
109M    bin
56K     data
4.0K    extensions
109M    总用量
```

* 链接成功后，可以执行`Open folder...`打开Remote机器的Project查看和编辑代码，同时，可以打开`Remote-SSH`集成的`Terminal`查看和执行Remote服务器的相关操作。

## 快捷键（macOS）

* `shift + command + E`: 文件浏览（`⇧⌘E`）
* `shift + command + F`: 全局查找（`⇧⌘F`）
* `command + F`: 查找（在当前文件）
* `option + command + F`: 替换（在当前文件）
* `shift + command + P`: 运行命令（`⇧⌘P`）
* `shift + command + B`:  运行任务（`⇧⌘B`）
* `shift + command + X`: 在Marketplace搜索扩展（`⇧⌘X`）
* `control + ~`: 打开terminal

## 代码快捷键

* `command + tick`:  查找跳转
* `control + -`/`shift + control + -`: 返回/前进
* `command + P`:  
	+ 查找文件，显示最近打开的文件
	+ 当前文件行数（`:行号`）或函数（`@函数名`）跳转。其中，函数跳转等价于：`shift + command + o`

## 常用配置

### 标题显示完整的文件路径名

```
"window.title": "${dirty}${activeEditorLong}${separator}${rootName}${separator}${appName}"
```

### 自动删除文件行尾空格

```
"files.trimTrailingWhitespace": true
```


## 其他插件

### Vim

* 提供Vim快捷键操作

### Terminal

* 在编辑器中提供终端操作

### TabNine

* 智能的代码自动补全工具

### Bracket Pair Colorizer2

* 括号通过不同颜色区分，多层级括号便于区分

###  Indenticator

* 代码层级对齐

###  GitLens — Git supercharged

* 分支管理，代码对比

### Git Graph

* View a Git Graph of your repository, and perform Git actions from the graph.

### Git (Submodule) Assistant

* VS Code extension that detects common git (and submodule) issues and helps to solve them.

### Git History

* View git log, file history, compare branches or commits.

### One Dark Pro

* VS Code主题，方便阅读代码
* 用法：`Code->Preferences->Color Theme`

### Peacock

* 设置每个VS Code窗口边框的颜色，打开多个VS Code时，便于快速找到想要的哪个窗口
* 用法：`>peacock …`

### Markdown All in One

* 提供Markdown文档的一些功能，比如自动生成/更新目录
* 用法：`>markdown …`

### Edit csv

* 阅读/编辑 csv 文件
* 用法：`>edit as csv`

### shellman

* shell script snippet，提供bash常用模版

### vscode-proto3

Protobuf 3 support for Visual Studio Code







  

	
	