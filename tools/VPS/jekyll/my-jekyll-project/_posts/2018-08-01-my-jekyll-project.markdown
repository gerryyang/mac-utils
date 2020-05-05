---
layout: post
title:  "开始新的学习和写作"
date:   2018-08-01 23:25:00 +0800
categories: 杂记
---

* Do not remove this line (it will not be displayed)
{:toc}

就像[这篇博文]的作者一样，最早在接触互联网的时候就开始养成写博客的习惯，因为上中学的时候对化学老师的`好记性不如烂笔头`一句话铭记于心。当时互联网还没有微博，微信，逛论坛和博客是那时的主流。最早写博文是在沪江英语的博客上，因为经常逛沪江论坛，后来由于沪江英语转型博客服务已经不再提供，之前写的一些文章也找不到了。后来又经常逛CSDN论坛，就改为在[CSDN的博客]上继续写博客，因为是免费的服务，天下没有免费的午餐，博客上的广告投放越来越多，有时还经常显示网站在维护不能访问，故而觉得有必要搭建一个自己的博客。目前是在[DigitalOcean]上购买的[VPS]服务，主要用于公众号和博客。其中博客选择的[jekyll]，因为它的哲学思想就是注重内容，界面简单，可以像hacker一样写文章，正如你看到的样子。工作和生活中难免存在很多琐碎的事情，维护博客目的是将一些有价值的事情记录下来，让自己多一些思考。


[这篇博文]: http://tom.preston-werner.com/2008/11/17/blogging-like-a-hacker.html
[CSDN的博客]: https://blog.csdn.net/delphiwcdj
[DigitalOcean]: https://www.digitalocean.com/
[VPS]: https://en.wikipedia.org/wiki/Virtual_private_server
[jekyll]: https://github.com/jekyll/jekyll

# VPS的选取

通过[我为什么选择DigitalOcean VPS来做开发]一文的介绍，在DigitalOcean上创建了一个VPS。体验后有以下感受：

[我为什么选择DigitalOcean VPS来做开发]: https://timyang.net/container/why-digitalocean-coreos/

**优点：**
* 下载速度快，由于是国外的服务器没有GFW的问题。在下载Dokcer官方镜像的时候可以体会到。
* [Tutorials]丰富，很多分享经验的文章，简单易懂。
* 官方的客服服务响应迅速。如果遇到问题可以向官方提issue获得帮助。
* 环境自由，可以各种折腾，为程序员而生。

```
Simple cloud hosting, built for developers. A droplet is a KVM VPS which you get full root access to so you can install anything you like.
```

[Tutorials]: https://www.digitalocean.com/community/tutorials

**缺点：**
* 需要承担一些费用。比如，每月10美元(1GB Mem/1 Core processor/30GB SSD的配置)。使用[我的推荐码]注册，你可以得到`100美元`的奖励金额用于购买VPS，如果以后继续付费的话，我也可能得到一些奖励。
* 暂时没想到第二条。

```
Give $100, Get $25
Everyone you refer gets $100 in credit over 60 days. Once they’ve spent $25 with us, you'll get $25. There is no limit to the amount of credit you can earn through referrals.
```

[我的推荐码]: https://m.do.co/c/06b03cc8b6ca


## DigitalOcean

完成下面三步，就可以创建和体验自己的VPS服务了。

1. 完成注册
2. 更新账单(可以使用DO赠送的金额)
3. 创建Droplet(有多种image可以选择，比如Ubuntu)

创建完，就可以通过SSH的方式登陆到自己的云服务器了，DigitalOcean会把外网的IP和root密码通过邮件的方式通知我们。可以直接使用root和密码来登录，但更安全便捷(不用每次都输入密码)的方式是使用SSH keys的方式来登录([How to Add SSH Keys to Droplets])。

[How to Add SSH Keys to Droplets]: https://www.digitalocean.com/docs/droplets/how-to/add-ssh-keys/

注意，DO创建完droplet就开始收费，即使你把它power off(sudo poweroff)，它也同样在计费，因为droplet使用的资源没有释放。在Billing的选项里可以查看自己的balance和usage情况。如果不希望继续付费可以把droplet删除(destroy)，但是会把这个droplet上的数据全部清除(scrub up)。

```
Note that when you power off your droplet you are still billed for it. This is because your diskspace, CPU, RAM, and IP address are all reserved while it is powered off.
```

检查环境：

```
cat /etc/issue
Ubuntu 14.04.1 LTS GNU Linux 3.13.0 37 generic x86_64
```

查看droplet的public ip：

```
ip addr show eth0 | grep inet | awk '{ print $2; }' | sed 's/\/.*$//'
curl http://icanhazip.com
```

## Linode

[Linode](https://www.linode.com/)也是一种选择，目前还没有使用过。


# VPS设置

以`Ubuntu`为例，在申请到一个VPS后我们需要做哪些事情。

## 账户初始化配置

[initial-server-setup-with-ubuntu-14-04](https://www.digitalocean.com/community/tutorials/initial-server-setup-with-ubuntu-14-04)

## 系统安全配置

* HTTPS证书设置

参考[证书安装指引](https://cloud.tencent.com/document/product/400/4143)。

## 开发工具配置

* apache/nginx

```
apt-get install apache2
apt-get install nginx
```

[how-to-install-nginx-on-ubuntu-14-04-lts](https://www.digitalocean.com/community/tutorials/how-to-install-nginx-on-ubuntu-14-04-lts)

反向代理设置：

[NGINX Reverse Proxy](https://docs.nginx.com/nginx/admin-guide/web-server/reverse-proxy/)
[nginx配置二级域名](https://cloud.tencent.com/developer/article/1198752)

* mysql

```
apt-get install mysql-server php5-mysql
```

* php

```
# for apache
apt-get install php5 libapache2-mod-php5 php5-mcrypt
# web root is /var/www/html

# for nginx
apt-get install php5-fpm php5-mysql
# web root /usr/share/nginx/html
```

* phpMyAdmin

```
apt-get install phpmyadmin apache2-utils
http://104.131.173.242/phpmyadmin/
```

* [FileZilla](https://filezilla-project.org/)


# Jekyll 配置

如何通过jekyll发表blog，基本的用法可参考官方文档[Posts章节](https://jekyllrb.com/docs/posts/)。


* 目录索引(TOC)

[How I Add a Table of Contents to my Jekyll Blog Written in Markdown](http://www.seanbuscay.com/blog/jekyll-toc-markdown/)

[kramdown](https://kramdown.gettalong.org/)

* 主题

参考[Overriding theme defaultsPermalink](https://jekyllrb.com/docs/themes/#overriding-theme-defaults)实现如何修改主题。

第三方主题：

[long-haul](https://github.com/brianmaierjr/long-haul) - A minimal, type-focused Jekyll theme. http://brianmaierjr.com/long-haul

More: 

[https://jekyllrb.com/](https://jekyllrb.com/)

参考博客：

* [https://ianding.io/](https://ianding.io/)
* [https://arthurchiao.github.io/about/](https://arthurchiao.github.io/about/)


# 工具

## MAC环境

### 系统

| 工具 | 功能
| -- | --
| [Homebrew](https://brew.sh/) | 软件包管理，第二个AppStore
| [Alfred 4 for Mac](https://www.alfredapp.com/) | 查找文件，搜索web等，默认通过快捷键`option + Space`来唤起，例如，`open xxx`打开一个目录或文件
| [ITerm2](https://www.iterm2.com/) | 相比系统默认的`Terminal`提供了更多高级的功能
| [Oh My ZSH](https://ohmyz.sh/) | 兼容bash，并集成了很多插件，使用Terminal更加高效。`bash`和`zsh`之间的切换方法，通过`chsh`命令，然后重启session即可生效：`$chsh -s /bin/zsh`或则`$chsh -s /bin/bash`。常用插件：`autojump`用于目录跳转（`brew install autojump`）
| Easy New File | 添加右键新建文件
| [Itsycal](https://www.mowglii.com/itsycal/) | 比系统默认日期显示更全，同时可以添加事件通知，与系统的日历是打通的
| [Spectacle](https://www.spectacleapp.com/) | 窗口管理工具
| Cheatsheet | 通过长按`command键`显示当前应用的快捷键，并可将其保存为PDF文档
| [Keka](https://www.keka.io/en/) | 解压缩工具
| [IINA](https://iina.io/) | The modern media player for macOS，比苹果自带quicktime player播放器功能强大
| Amphetamine | 可以保持系统一直处于awake状态，方便网络下载等不中断场景使用，点击它可以开启一个session来阻止系统进入休眠，同时可以设置这个session的生命周期，比系统默认的`节能`配置使用方便
| [Snip](https://snip.qq.com/) | 默认mac可以通过`shift+command+5`来截图或录制屏幕。而Snip的优点是可以滚动截屏（从 Mac App Store 下载的 Snip 不支持滚动截屏）
| [Aerial](https://github.com/JohnCoates/Aerial) | Apple TV Aerial Screensaver for Mac
| [Dozer](https://github.com/Mortennn/Dozer) | Hide status bar icons on macOS，用于隐藏状态栏上不必要的图标，通过`command + drag`将范围内的图标隐藏
| [LICEcap](https://github.com/justinfrankel/licecap) | LICEcap simple animated screen capture tool for Windows and OS X，用于屏幕录制并支持导出GIF


More: https://github.com/hzlzh/Best-App


### 效率

| 工具 | 功能
| -- | --
| Trello | 多终端使用，记录日常工作的TODO
| [欧陆词典](https://www.eudic.net/v4/en/app/eudic) | 相对有道词典的优点是没有广告，支持微信账户同步


### Google Chrome

使用Google账户登录，方便同步书签信息，便于多终端使用，并且有丰富的扩展程序。

| **扩展程序** | **功能**
| -- | --
| 印象笔记剪裁 | 保存网页
| FeHelper | WEB前端助手。1. JSON自动格式化(或命令行工具`jq`) 2. 二维码生成器
| Vimium | 可以使用Vim快捷键浏览网页
| [Video DownloadHelper](https://chrome.google.com/webstore/detail/video-downloadhelper/lmjnegcaeklhafolokijcfjliaokphfk) | 下载页面上的多媒体资源，当检测到有可下载的资源图标会变为彩色，否则为灰色
| Adblock Plus | 浏览器广告拦截
| 替换字体的中文部分为雅黑 | 将页面中文显示雅黑字体
| [类似的网站](http://bit.ly/3bqQvDW) | 发现相关网站
| [Grammarly for Chrome](http://bit.ly/37e0jOs) | 英语写作纠正
| [高效网页截图编辑插件](http://bit.ly/2SDgdwv) | 支持长截图
| [CLEAN crxMouse Gestures](http://bit.ly/2uxtUVF) | 手势，支持各种设置


## 开发

### 代码管理

| 工具 | 功能
| -- | --
| [Sourcetree](https://www.sourcetreeapp.com/) | Mac Git客户端
| [Cornerstone 4](https://cornerstone.assembla.com/) | Mac SVN客户端

### Vim

一些总结和用法，[另见](https://github.com/gerryyang/mac-utils/tree/master/tools/software_documentation_tools/vim)。

**常用插件：**

* [YouCompleteMe - A code-completion engine for Vim](https://github.com/ycm-core/YouCompleteMe)
* [ccls - C/C++/ObjC language server supporting cross references, hierarchies, completion and semantic highlighting](https://github.com/MaskRay/ccls)

**refer:**

* [Configure coc.nvim for C/C++ Development](https://ianding.io/2019/07/29/configure-coc-nvim-for-c-c++-development/)
* [my vim configuration, based on amix's project](https://github.com/ArthurChiao/vim_awesome)

### Sublime

一些总结和用法，[另见](https://github.com/gerryyang/mac-utils/tree/master/tools/software_documentation_tools/sublime)。

| 插件名称 | 用途 | 获取地址
| -- | -- | --
| MarkdownPreview | Markdown文档预览 | https://packagecontrol.io/packages/MarkdownPreview https://facelessuser.github.io/MarkdownPreview/usage/
| AlignTab | 对齐，The most flexible alignment plugin for Sublime Text 3. This plugin is inspired by the excellent VIM plugin, [tabular](https://github.com/godlygeek/tabular) | https://github.com/randy3k/AlignTab
| Sublime​AStyle​Formatter | C/C++/C#/Java code formatter/beautifier with AStyle, For OSX, control + option + f to format current file | https://packagecontrol.io/packages/SublimeAStyleFormatter
| GoSublime (x) | GoLang配置 | https://packagecontrol.io/packages/GoSublime
| ConvertToUTF8 | 字符编码转换 | https://packagecontrol.io/packages/ConvertToUTF8
| SyncedSideBar | | https://packagecontrol.io/packages/SyncedSideBar
| URLEncode | url编解码 | https://packagecontrol.io/packages/URLEncode

* AlignTab 使用说明

安装完AlignTab后，对选择的内容在右键菜单中可以进行常见的格式化，或者根据自己的需求将常用的格式化命令配置为快捷键。(Custom User Bindings for using AlignTab with ='s, =>'s, and :'s in Sublime Text
)

"Preferences" -> "Key Bindings"，配置以下快捷键，之后即可通过`super+ctrl+=`来格式化`=`的语句。

```
[
  {
    "keys": ["super+ctrl+;"], "command": "align_tab",
    "args" : {
      "user_input" : ":/f"
    }
  },
  {
    "keys": ["super+ctrl+="], "command": "align_tab",
    "args" : {
      "user_input" : "=/f"
    }
  },
  {
    "keys": ["super+ctrl+."], "command": "align_tab",
    "args" : {
      "user_input" : "=>/f"
    }
  }
]
```


### VSCode

[VSCode](https://github.com/microsoft/vscode)是微软推出的一款IDE开发工具，拥有丰富的插件，支持Windows，OS X和Linux，此外内置JavaScript，TypeScript和Node.js支持。VSCode定位于编辑器与IDE之间，但是更像一个编辑器。

### Markdown

[Typora](https://www.typora.io)用于Markdown编辑和可视化。

### 代码对比

* [Beyond Compare](http://www.scootersoftware.com/download.php)，是一套由Scooter Software推出的內容比较工具软件，提供Win、Mac、Linux等多平台下载。


### 代码查看

* Source Insight，是一款功能强大的面向项目的编程编辑器，代码浏览器和分析器，可帮助您在工作和计划时更好的理解代码。

### JetBrains全家桶

| IDE | 功能
| -- | --
| CLion | [JetBrains](https://zh.wikipedia.org/wiki/JetBrains)的CLion，是一个跨平台的C/C++ IDE开发工具，支持C++11 、C++14、libc++以及Boost。
| GoLand | GoLand是JetBrains的IDE，旨在为Go开发提供符合人体工程学的环境。
| IntelliJ IDEA | 2001年发布，一套智能的Java集成开发环境，特别专注与强调程序师的开发撰写效率提升。
| PyCharm | PyCharm是一种Python IDE，可以帮助用户在使用Python语言开发时提高其效率的工具。
| PhpStorm | PhpStorm是JetBrains公司开发的一款商业的PHP集成开发工具，旨在提高用户效率，可深刻理解用户的编码，提供智能代码补全，快速导航以及即时错误检查。
| WebStorm | WebStorm是jetbrains公司旗下一款JavaScript 开发工具，被誉为Web前端开发神器。
| Rider | JetBrains Rider是一款快速强大的C#编辑器，用于在Windows、Mac和Linux上进行Unity开发。


## 画图

* [canva](https://www.canva.com/)
	- 在线图形设计平台

* [websequencediagrams](https://www.websequencediagrams.com/)
	- 通过脚本语言生成**时序图**。

* [flowchart.js](https://github.com/adrai/flowchart.js) 
	- 通过脚本语言生成**流程图**。
	- 使用[diagrams](https://github.com/francoislaberge/diagrams/#flowchart)工具可以生成各种图形，包括flowchart。

* [Viz.js](https://github.com/mdaines/viz.js) 
	- 借助[Viz](http://viz-js.com/)Web平台使用`Graphviz`的`DOT`语言在浏览器中生成**广义的图形**。

* [Evolus Pencil](http://pencil.evolus.vn/)
	- An open-source GUI prototyping tool that's available for ALL platforms.

**refer:**

* [使用 Graphviz 生成自动化系统图](https://www.ibm.com/developerworks/cn/aix/library/au-aix-graphviz/index.html)
* [graphviz-dotguide官方教程](http://www.graphviz.org/pdf/dotguide.pdf)


## 云同步

* 印象笔记
* [马克飞象](https://maxiang.io/)，web工具，可以结合印象笔记使用。
* [微云](https://www.weiyun.com/download.html)，文件备份。


# 数学

* [WolframAlpha](http://www27.wolframalpha.com/)，一个在线数学工具。

# 算法

* [Algorithm Visualizer](https://github.com/algorithm-visualizer/algorithm-visualizer)，Interactive Online Platform that Visualizes Algorithms from Code https://algorithm-visualizer.org/
* [数据结构和算法必知必会的50个代码实现](https://github.com/wangzheng0822/algo)


# Computer Blogs

* [ncona.com - Learning about computers](https://ncona.com/)


--------
*END - Enjoy!*



