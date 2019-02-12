---
layout: post
title:  "Nginx in Action"
date:   2019-02-12 20:30:00 +0800
categories: nginx
---

* Do not remove this line (it will not be displayed)
{:toc}

Keywords: nginx, web server, reverse proxy


# 证书申请

可以在腾讯云申请证书，每年承担一定的费用。

# 域名配置

申请完域名后，可以配置不同的二级域名。

![dns_record_config](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201811/dns_record_config.jpg)

# nginx配置

## 配置二级域名路由

修改配置文件：

vim /etc/nginx/sites-available/default

```
server {  
    listen 80;
    server_name blog.gerryyang.com;

    location / {
        proxy_set_header   X-Real-IP $remote_addr;
        proxy_set_header   Host      $http_host;
        proxy_pass         http://127.0.0.1:8080;
    }
}
server {  
    listen 80;
    server_name forum.gerryyang.com;

    location / {
        proxy_set_header   X-Real-IP $remote_addr;
        proxy_set_header   Host      $http_host;
        proxy_pass         http://127.0.0.1:8081;
    }
}
```

重启服务：

service nginx restart

nginx相关错误定位方法：

* 方法1：检查配置文件是否有错误，通过`nginx -t`

```
# nginx -t   
nginx: the configuration file /etc/nginx/nginx.conf syntax is ok
nginx: configuration file /etc/nginx/nginx.conf test is successful
```

* 方法2：查看日志，通过`tail -f /var/log/nginx/error.log` or `tail -f /var/log/nginx/access.log`


TODO 可以转发，但是root设置不对，访问不了网页



# Refer

https://www.digitalocean.com/community/tutorials

[证书安装指引]: https://cloud.tencent.com/document/product/400/4143
[nginx配置二级域名]: https://cloud.tencent.com/developer/article/1198752


https://www.digitalocean.com/community/tutorials/how-to-install-nginx-on-ubuntu-18-04-quickstart

https://www.digitalocean.com/community/tutorials/how-to-install-nginx-on-ubuntu-18-04

