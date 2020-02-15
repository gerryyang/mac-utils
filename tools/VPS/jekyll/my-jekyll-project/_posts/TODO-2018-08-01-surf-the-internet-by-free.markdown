---
layout: post
title:  "如何科学的上网"
date:   2018-08-01 23:25:00 +0800
categories: 杂记
---

* Do not remove this line (it will not be displayed)
{:toc}


# 科学上网

## 搭建ss服务

Shadowsocks is a cross-platform tunnel proxy which can help you get through firewalls.

[Shadowsocks](https://github.com/shadowsocks/shadowsocks)已被河蟹。可以找到一些fork的版本[release 2.8.2](https://github.com/ziggear/shadowsocks)。


### iOS 客户端

Step 1: 下载iOS系统可用的Shadowsocks软件，由于苹果中国区下架的原因，可以切换到`非中国区`下载(需要其他区域的Apple ID)。以下香港ID供参考使用，注意，为防止自己的手机信息泄露，请务必不要使用以下Apple ID登录iCloud，使用下载过后及时退出该账号。

* [hk账号](https://okmobiledev.github.io/download/okex/ios-store.html)
* [免费注册香港Apple ID教程：总会有需要的时候](https://zhuanlan.zhihu.com/p/65771804)

Step 2: 注册好后就可以下载下面几款客户端软件。

* Sockswitch (免费，验证可用)
* Potatso Lite (免费)
* Potatso 2 ($2.99)
* Shadowrocket($2.99)
* Wingy - Proxy For Http(s),Socks5,Shadowsocks

Step 3: 打开下载的客户端软件，输入ss的`服务器信息`配置即可使用。另见[iOS的开源客户端](https://github.com/shadowsocks/shadowsocks-iOS/releases)。


### Mac OS X 客户端

下载[Mac OS X 客户端](https://github.com/shadowsocks/ShadowsocksX-NG/releases/)并添加服务器，然后选择“自动代理模式”。


### Windows 客户端

下载[Windows 客户端](https://github.com/shadowsocks/shadowsocks-windows/releases/)，方法同上。


问题：OpenSSL升级到1.1.0版本以后，运行ss提示`AttributeError: /usr/lib/x86_64-linux-gnu/libcrypto.so.1.1: undefined symbol: EVP_CIPHER_CTX_cleanup`错误。

解决方法：

```
EVP_CIPHER_CTX was made opaque in OpenSSL 1.1.0. As a result, EVP_CIPHER_CTX_reset() appeared and EVP_CIPHER_CTX_cleanup() disappeared.
EVP_CIPHER_CTX_init() remains as an alias for EVP_CIPHER_CTX_reset().
```

修改`/usr/local/lib/python2.7/dist-packages/shadowsocks/crypto/openssl.py`，将`EVP_CIPHER_CTX_cleanup`替换为`EVP_CIPHER_CTX_reset`。


refer: [Create a SOCKS5 Proxy Server with Shadowsocks on Ubuntu and CentOS 7](https://www.linode.com/docs/networking/vpn/create-a-socks5-proxy-server-with-shadowsocks-on-ubuntu-and-centos7/)

## 付费服务

* [devpn.live](https://devpn.live/)  

[我的推广链接](https://goen.win/mygki8)


* [云梯](https://yunti-16.xyz/index.html) 
	- 主站：www.yuntiss.top
	- 备用站：www.yuntiss.site


## 其他

[科学/自由上网，免费ss/ssr/v2ray/goflyway账号，搭建教程](https://github.com/Alvin9999/new-pac)



