---
layout: post
title:  "开始新的学习和写作"
date:   2018-08-01 23:25:00 +0800
categories: life
---

* Do not remove this line (it will not be displayed)
{:toc}

就像[这篇博文]的作者一样，最早在接触互联网的时候就开始养成写博客的习惯，因为上中学的时候对化学老师的`好记性不如烂笔头`一句话铭记于心。当时互联网还没有微博，微信，逛论坛和博客是那时的主流。最早写博文是在沪江英语的博客上，因为经常逛沪江论坛，后来由于沪江英语转型博客服务已经不再提供，之前写的一些文章也找不到了。后来又经常逛CSDN论坛，就改为在[CSDN的博客]上继续写博客，因为是免费的服务，天下没有免费的午餐，博客上的广告投放越来越多，有时还经常显示网站在维护不能访问，故而觉得有必要搭建一个自己的博客。目前是在[DigitalOcean]上购买的[VPS]服务，主要用于
公众号和博客。其中博客选择的[jekyll]，因为它的哲学思想就是注重内容，界面简单，可以像hacker一样写文章，正如你看到的样子。工作和生活中难免存在很多琐碎的事情，维护博客目的是将一些有价值的事情记录下来，在以后的岁月里可以看到自己之前的轨迹。


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

[Linode]

[Linode]: https://www.linode.com/

# VPS设置

以下以`Ubuntu`为例。

## 账户初始化配置

[initial-server-setup-with-ubuntu-14-04]

[initial-server-setup-with-ubuntu-14-04]: https://www.digitalocean.com/community/tutorials/initial-server-setup-with-ubuntu-14-04

## 系统安全配置

* HTTPS证书设置

[证书安装指引]

[证书安装指引]: https://cloud.tencent.com/document/product/400/4143

## 开发工具配置

注意先更新源 apt-get update。

* git


* gcc 

```
apt-get install gcc
```

* docker 

```
apt-get install -y docker.io
ln -sf /usr/bin/docker.io /usr/local/bin/docker
sed -i `$acomplete -F _docker docker` /etc/bash_completion.d/docker.io
```

* golang

```
apt-get install gccgo-go
```

* apache/nginx

```
apt-get install apache2
apt-get install nginx
```

[how-to-install-nginx-on-ubuntu-14-04-lts]

[how-to-install-nginx-on-ubuntu-14-04-lts]: https://www.digitalocean.com/community/tutorials/how-to-install-nginx-on-ubuntu-14-04-lts

反向代理设置：

[NGINX Reverse Proxy]
[nginx配置二级域名]

[NGINX Reverse Proxy]: https://docs.nginx.com/nginx/admin-guide/web-server/reverse-proxy/

[nginx配置二级域名]: https://cloud.tencent.com/developer/article/1198752

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

* [FileZilla]

[FileZilla]: https://filezilla-project.org/


# 科学上网

## Shadowsocks

Shadowsocks is a cross-platform tunnel proxy which can help you get through firewalls.

[Shadowsocks]: https://github.com/gerryyang/shadowsocks


```
sudo ssserver -p 443 -k password -m rc4-md5 --user nobody -d start
sudo ssserver -d stop

tail -f /var/log/shadowsocks.log
```

# jekyll配置

* 目录索引(TOC)

[How I Add a Table of Contents to my Jekyll Blog Written in Markdown]
[kramdown]

[How I Add a Table of Contents to my Jekyll Blog Written in Markdown]: http://www.seanbuscay.com/blog/jekyll-toc-markdown/
[kramdown]: https://kramdown.gettalong.org/




# 收藏站点

## 博客

[永远不上线-借筏度岸]

[永远不上线-借筏度岸]: http://www.cnblogs.com/lookbackinside/


# 工具

## 基础工具

* 浏览器
	- chrome

chrome常用扩展：

| 名字 | 功能
| -- | --
| DragIt | 方便页面浏览
| FeHelper | WEB前端助手。1. JSON自动格式化(或命令行工具`jq`) 2. 二维码生成器

[FeHelper]: https://chrome.google.com/webstore/search/fehelper?hl=zh-CN


## MacOS

TODO

## 其他

* [WolframAlpha]，一个在线数学工具。
* [时区图]


[WolframAlpha]: http://www27.wolframalpha.com/
[时区图]: https://zh.wikipedia.org/wiki/%E6%97%B6%E5%8C%BA#/media/File:Standard_World_Time_Zones.png



# Refer



