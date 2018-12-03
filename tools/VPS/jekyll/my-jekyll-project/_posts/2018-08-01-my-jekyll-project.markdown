---
layout: post
title:  "开始新的学习和写作"
date:   2018-08-01 23:25:00 +0800
categories: life
---

* Do not remove this line (it will not be displayed)
{:toc}

就像[这篇博文]的作者一样，自己在开始接触互联网的时候就开始养成写博客的习惯，因为上中学的时候对化学老师的`好记性不如烂笔头`一句话铭记于心。当时互联网还没有微博，微信，逛论坛和博客是那时的主流。最早写博文是在沪江英语的博客上，因为经常逛沪江论坛，后来由于沪江英语转型博客服务已经不再提供了，之前写的一些文章也找不到了。后来又经常逛CSDN论坛，就改为在[CSDN的博客]上继续写博客，因为是免费的服务，天下没有免费的午餐，后来发现在博客上的广告投放越来越多，有时还经常显示网站在维护不能访问，因此觉得还是独立搭建一个博客可以规避这些问题。正好在DigitalOcean上购买了VPS服务，用于
搭建自己的公众号服务，可以同时搭建一个博客。在网上搜集了一些方案，目前选择jekyll，因为它的哲学思想就是注重内容，界面简单，可以像hacker一样写文章。以后也可以尝试一些其他的方式。

# jekyll设置

* TOC

[How I Add a Table of Contents to my Jekyll Blog Written in Markdown]
[kramdown]

# VPS设置

* 账户初始化设置

https://www.digitalocean.com/community/tutorials/initial-server-setup-with-ubuntu-14-04

* 安装nginx

https://www.digitalocean.com/community/tutorials/how-to-install-nginx-on-ubuntu-14-04-lts

反向代理设置：

[NGINX Reverse Proxy]
[nginx配置二级域名]

[NGINX Reverse Proxy]: https://docs.nginx.com/nginx/admin-guide/web-server/reverse-proxy/

[nginx配置二级域名]: https://cloud.tencent.com/developer/article/1198752

* HTTPS证书设置

[证书安装指引]

[证书安装指引]: https://cloud.tencent.com/document/product/400/4143


[Jekyll on Ubuntu]: https://jekyllrb.com/docs/installation/ubuntu/
[这篇博文]: http://tom.preston-werner.com/2008/11/17/blogging-like-a-hacker.html
[CSDN的博客]: https://blog.csdn.net/delphiwcdj

[How I Add a Table of Contents to my Jekyll Blog Written in Markdown]: http://www.seanbuscay.com/blog/jekyll-toc-markdown/
[kramdown]: https://kramdown.gettalong.org/