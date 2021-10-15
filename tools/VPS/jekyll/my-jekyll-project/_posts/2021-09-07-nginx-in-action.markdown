---
layout: post
title:  "Nginx in Action"
date:   2021-09-07 20:00:00 +0800
categories: Nginx
---

* Do not remove this line (it will not be displayed)
{:toc}

Keywords: nginx, web server, reverse proxy


# 安装

* nginx-1.19.9 (30-Mar-2021) 版本

```
wget https://nginx.org/download/nginx-1.19.9.tar.gz
tar -xzvf nginx-1.19.9.tar.gz
cd nginx-1.19.9 
```

* 指定配置选项
  
```
./configure  --prefix=/usr/local/nginx  --with-http_ssl_module --with-http_v2_module --with-http_realip_module --with-http_addition_module --with-http_sub_module --with-http_dav_module --with-http_flv_module --with-http_mp4_module --with-http_gunzip_module --with-http_gzip_static_module --with-http_auth_request_module --with-http_random_index_module --with-http_secure_link_module --with-http_degradation_module --with-http_slice_module --with-http_stub_status_module --with-mail --with-mail_ssl_module --with-stream --with-stream_ssl_module --with-stream_realip_module --with-stream_ssl_preread_module --with-threads --user=www --group=www

Configuration summary
  + using threads
  + using system PCRE library
  + using system OpenSSL library
  + using system zlib library

  nginx path prefix: "/usr/local/nginx"
  nginx binary file: "/usr/local/nginx/sbin/nginx"
  nginx modules path: "/usr/local/nginx/modules"
  nginx configuration prefix: "/usr/local/nginx/conf"
  nginx configuration file: "/usr/local/nginx/conf/nginx.conf"
  nginx pid file: "/usr/local/nginx/logs/nginx.pid"
  nginx error log file: "/usr/local/nginx/logs/error.log"
  nginx http access log file: "/usr/local/nginx/logs/access.log"
  nginx http client request body temporary files: "client_body_temp"
  nginx http proxy temporary files: "proxy_temp"
  nginx http fastcgi temporary files: "fastcgi_temp"
  nginx http uwsgi temporary files: "uwsgi_temp"
  nginx http scgi temporary files: "scgi_temp"
```

通过 `grep "YES" auto/options` 可查看配置的所有选项。

* 编译并安装

```
make && make install
```

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

