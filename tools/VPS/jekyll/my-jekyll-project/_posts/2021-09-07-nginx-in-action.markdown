---
layout: post
title:  "Nginx in Action"
date:   2021-09-07 20:00:00 +0800
categories: Nginx
---

* Do not remove this line (it will not be displayed)
{:toc}

Keywords: nginx, web server, reverse proxy


# 安装 nginx 服务

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


在 nginx 的 auto 子目录中有一个 options 文件，这个文件里面保存的就是 nginx 编译过程中的所有选项配置，通过 `grep "YES" auto/options` 可查看配置的所有选项。

* 编译并安装 (root 权限)

```
make && make install
```

查看 nginx 安装后在的目录：

```
$ whereis nginx
nginx: /usr/local/nginx
```

启动 nginx：

```
$ ./nginx
nginx: [emerg] getpwnam("www") failed
```

错误的原因是没有创建 www 这个用户，需在系统中添加 www 用户组和用户 www，或者修改 nginx 的配置，在 nginx.conf中 把 user nobody 的注释去掉

```
#/usr/sbin/groupadd -f www
#/usr/sbin/useradd -g www www
```

修改后重新启动：

```
[root@/usr/local/nginx/sbin]$ ./nginx
[root@/usr/local/nginx/sbin]$ ps ux|grep nginx
root       58982  0.0  0.0  47460  1276 ?        Ss   11:04   0:00 nginx: master process ./nginx
```

此时在浏览器中访问本地IP，可看到 nginx 返回的如下页面：

![nginx_hello](/assets/images/202201/nginx_hello.png)


# 常用命令

```
nginx -s reload          # 向主进程发送信号，重新加载配置文件，热重启
nginx -s reopen          # 重启 Nginx
nginx -s stop            # 快速关闭
nginx -s quit            # 等待工作进程处理完成后关闭
nginx -T                 # 查看当前 Nginx 最终的配置
nginx -t -c <配置路径>    # 检查配置是否有问题，如果已经在配置目录，则不需要 -c
```

More: nginx -h

Linux 系统应用管理工具 systemd 关于 nginx 的常用命令：

```
systemctl start nginx    # 启动 Nginx
systemctl stop nginx     # 停止 Nginx
systemctl restart nginx  # 重启 Nginx
systemctl reload nginx   # 重新加载 Nginx，用于修改配置后
systemctl enable nginx   # 设置开机启动 Nginx
systemctl disable nginx  # 关闭开机启动 Nginx
systemctl status nginx   # 查看 Nginx 运行状态
```


# location

## 用法

`location`匹配顺序，优先级从高到低依次为（序号越小优先级越高）：

```
1. location =    # 精准匹配
2. location ^~   # 带参前缀匹配
3. location ~    # 正则匹配（区分大小写）
4. location ~*   # 正则匹配（不区分大小写）
5. location /a   # 普通前缀匹配，优先级低于带参数前缀匹配。
6. location /    # 任何没有匹配成功的，都会匹配这里处理
```

1. 先精准匹配 `=` ，精准匹配成功则会立即停止其他类型匹配；
2. 没有精准匹配成功时，进行前缀匹配。先查找带有 `^~` 的前缀匹配，带有 `^~` 的前缀匹配成功则立即停止其他类型匹配，普通前缀匹配（不带参数 `^~` ）成功则会暂存，继续查找正则匹配；
3. `=` 和 `^~` 均未匹配成功前提下，查找正则匹配 `~` 和 `~*` 。当同时有多个正则匹配时，按其在配置文件中出现的先后顺序优先匹配，命中则立即停止其他类型匹配；
4. 所有正则匹配均未成功时，返回步骤 2 中暂存的普通前缀匹配（不带参数 `^~` ）结果

## location URI 结尾带不带 /

关于 URI 尾部的 `/` 有三点也需要说明一下。第一点与 location 配置有关，其他两点无关。

1. location 中的字符有没有 `/` 都没有影响。也就是说 `/user/` 和 `/user` 是一样的。
2. 如果 URI 结构是 `https://domain.com/` 的形式，尾部有没有 `/` 都不会造成重定向。因为浏览器在发起请求的时候，默认加上了 `/` 。虽然很多浏览器在地址栏里也不会显示 `/` 。这一点，可以访问 baidu 验证一下。
3. 如果 URI 的结构是 `https://domain.com/some-dir/` 。尾部如果缺少 `/` 将导致重定向。因为根据约定，URL 尾部的 `/` 表示目录，没有 `/` 表示文件。所以访问 `/some-dir/` 时，服务器会自动去该目录下找对应的默认文件。如果访问 `/some-dir` 的话，服务器会先去找 some-dir 文件，找不到的话会将 some-dir 当成目录，重定向到 `/some-dir/` ，去该目录下找默认文件。可以去测试一下你的网站是不是这样的。


## 例子

例子1:

```
server {

listen       80;
server_name  localhost;

location /gerry {
    # for test
    return 701;
}

location ~* ^/gerry {
    # for test
    return 702;
}
}
```

使用 `~*` 正则匹配（不区分大小写），相对于第一个匹配，其优先级更高。通过`curl -I`测试或查看 access 日志，可以看到返回码为 702

```
$curl -I localhost/gerry
HTTP/1.1 702
Server: nginx/1.19.9
Date: Wed, 26 Jan 2022 06:49:41 GMT
Content-Length: 0
Connection: keep-alive
```

```
==> access.log <==
10.99.16.41 - - [26/Jan/2022:14:23:48 +0800] "GET /gerry/ HTTP/1.1" 702 0 "-" "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36"
```

例子2:

```
server {

listen       80;
server_name  localhost;

location /gerry {
    # for test
    return 701;
}

location ~* ^/gerry {
    # for test
    return 702;
}

location ^~ /ge {
        # for test
        return 703;
}
}
```

前缀匹配 `^~` 命中以后不会再搜寻正则匹配，返回码为 703

```
$curl -I localhost/ge
HTTP/1.1 703
Server: nginx/1.19.9
Date: Wed, 26 Jan 2022 06:50:37 GMT
Content-Length: 0
Connection: keep-alive
```

例子3:

```
server {

listen       80;
server_name  localhost;

location ~ ^/gerr[a-z]+ {
    # for test
    return 701;
}

location ~ ^/ger[a-z]+ {
    # for test
    return 702;
}
}
```

正则匹配和配置顺序有关，先匹配成功的返回，返回 701

```
$curl -I localhost/gerry
HTTP/1.1 701
Server: nginx/1.19.9
Date: Wed, 26 Jan 2022 06:55:03 GMT
Content-Length: 0
Connection: keep-alive
```


# rewrite

## 用法

> 语法：`rewrite regex replacement [flag];`
>
> 作用域：server、location、if
>
> 功能：如果一个URI匹配指定的正则表达式 regex，URI就按照 replacement 重写。

`rewrite` 按配置文件中出现的顺序执行。可以使用 `flag` 标志来终止指令的进一步处理。如果 replacement 以 http://、https:// 或 $scheme 开始，将不再继续处理，这个重定向将返回给客户端。

nginx 通过 `ngx_http_rewrite_module` 模块支持 `URI` 重写、支持 `if` 条件判断，但不支持 `else`。

`rewrite` 只能放在 **server { }** 、 **location { }** 、 **if { } **中，并且只能对域名后边的除去传递的参数外的字符串起作用，例如:：`http://aaa.com/a/we/index.php?id=1&u=str` 只对 `/a/we/index.php` 重写。

表面看 `rewrite` 和 `location` 功能有点像，都能实现跳转，主要区别在于 `rewrite` 是在同一域名内更改获取资源的路径，而 `location` 是对一类路径做控制访问或反向代理，可以 `proxy_pass` 到其他机器。很多情况下 `rewrite` 也会写在 `location` 里，它们的执行顺序是：

1. 执行 `server` 块的 `rewrite` 指令（这里的块指的是 server 关键字后{}包围的区域，其它 xxx 块类似）
2. 执行 `location` 匹配
3. 执行选定的 `location` 中的 `rewrite` 指令

如果其中某步 `URI` 被重写，则重新循环执行 `1-3`，直到找到真实存在的文件；

如果循环超过 `10` 次，则返回 `500 Internal Server Error` 错误。

## flag 四种参数

* `last` 停止处理后续 rewrite 指令集，然后对当前重写的新 URI 在 rewrite 指令集上重新查找。
* `break` 停止处理后续 rewrite 指令集，并不再重新查找，但是当前 location 内剩余非 rewrite 语句和 location 外的 非 rewrite 语句可以执行。
* `redirect` 如果 replacement 不是以 http:// 或 https:// 开始，返回 302 临时重定向
* `permanent` 返回 301 永久重定向

## rewrite 后的请求参数

如果替换字符串 replacement 包含新的请求参数，则在它们之后附加先前的请求参数。如果你不想要之前的参数，则在替换字符串 replacement 的末尾放置一个问号，避免附加它们。

```
# 由于最后加了个 ?，原来的请求参数将不会被追加到 rewrite 之后的 URI 后面
rewrite ^/users/(.*)$ /show?user=$1? last;
```

## 例子

例子1:

```
location ^~ /redirect {
    # 当匹配前缀表达式 /redirect/(.*)时 请求将被临时重定向到 http://www.$1.com
    # 相当于 flag 写为 redirect
    rewrite ^/redirect/(.*)$ http://www.$1.com;
    return 200 "ok";
}
```

执行返回 302 重定向，并通过应答头部的 Location: http://www.baidu.com 返回重定向地址，但是，由于 replacement 以 `http://` 开始，将不再继续处理，因此没有执行 return 200 "ok"。

```
$curl -v localhost/redirect/baidu
* About to connect() to localhost port 80 (#0)
*   Trying 127.0.0.1...
* Connected to localhost (127.0.0.1) port 80 (#0)
> GET /redirect/baidu HTTP/1.1
> User-Agent: curl/7.29.0
> Host: localhost
> Accept: */*
>
< HTTP/1.1 302 Moved Temporarily
< Server: nginx/1.19.9
< Date: Wed, 26 Jan 2022 08:09:55 GMT
< Content-Type: text/html
< Content-Length: 145
< Connection: keep-alive
< Location: http://www.baidu.com
<
<html>
<head><title>302 Found</title></head>
<body>
<center><h1>302 Found</h1></center>
<hr><center>nginx/1.19.9</center>
</body>
</html>
* Connection #0 to host localhost left intact
```

修改配置去除 `http://`，由于没有带 `http://` 所以只是简单的重写。请求的 URI 由 `/redirect/baidu` 重写为 `www.baidu.com` 因为会顺序执行 `rewrite` 指令，所以 下一步执行 `return` 指令，响应后返回 ok


```
location ^~ /redirect {
    # 当匹配前缀表达式 /redirect/(.*)时 请求将被临时重定向到 http://www.$1.com
    # 相当于 flag 写为 redirect
    rewrite ^/redirect/(.*)$ http://www.$1.com;
    return 200 "ok";
}
```

```
$curl -v localhost/redirect/baidu
* About to connect() to localhost port 80 (#0)
*   Trying 127.0.0.1...
* Connected to localhost (127.0.0.1) port 80 (#0)
> GET /redirect/baidu HTTP/1.1
> User-Agent: curl/7.29.0
> Host: localhost
> Accept: */*
>
< HTTP/1.1 200 OK
< Server: nginx/1.19.9
< Date: Wed, 26 Jan 2022 08:16:09 GMT
< Content-Type: application/octet-stream
< Content-Length: 2
< Connection: keep-alive
<
* Connection #0 to host localhost left intact
ok
```


例子2:

```
# rewrite 后面没有任何 flag 时就顺序执行
# 当 location 中没有 rewrite 模块指令可被执行时 就重写发起新一轮 location 匹配
location / {
    # 顺序执行如下两条 rewrite 指令
    rewrite ^/test1 /test2;
    rewrite ^/test2 /test3;  # 此处发起新一轮 location 匹配 URI为 /test3
}

location = /test2 {
    return 200 “/test2”;
}

location = /test3 {
    return 200 “/test3”;
}
```

请求 `localhost/test1` 返回 "/test3"

```
$curl -v localhost/test1
* About to connect() to localhost port 80 (#0)
*   Trying 127.0.0.1...
* Connected to localhost (127.0.0.1) port 80 (#0)
> GET /test1 HTTP/1.1
> User-Agent: curl/7.29.0
> Host: localhost
> Accept: */*
>
< HTTP/1.1 200 OK
< Server: nginx/1.19.9
< Date: Wed, 26 Jan 2022 08:33:36 GMT
< Content-Type: application/octet-stream
< Content-Length: 12
< Connection: keep-alive
<
* Connection #0 to host localhost left intact
“/test3”
```

修改添加 flag 为 last：

```
location / {
    rewrite ^/test1 /test2;
    rewrite ^/test2 /test3 last;  # 此处发起新一轮location匹配 uri为 /test3
    rewrite ^/test3 /test4;
    proxy_pass http://www.baidu.com;
}

location = /test2 {
    return 200 "/test2";
}

location = /test3 {
    return 200 "/test3";
}
location = /test4 {
    return 200 "/test4";
}
```

rewrite 为 /test3 后不会继续执行：

```
$curl -v localhost/test1
* About to connect() to localhost port 80 (#0)
*   Trying 127.0.0.1...
* Connected to localhost (127.0.0.1) port 80 (#0)
> GET /test1 HTTP/1.1
> User-Agent: curl/7.29.0
> Host: localhost
> Accept: */*
>
< HTTP/1.1 200 OK
< Server: nginx/1.19.9
< Date: Wed, 26 Jan 2022 08:37:37 GMT
< Content-Type: application/octet-stream
< Content-Length: 6
< Connection: keep-alive
<
* Connection #0 to host localhost left intact
/test3
```

修改为如下配置：

```
location / {
    rewrite ^/test1 /test2;
    # 此处不会发起新一轮 location 匹配；当是会终止执行后续 rewrite 模块指令重写后的 URI 为 /more/index.html
    rewrite ^/test2 /more/index.html break;
    rewrite /more/index\.html /test4; # 这条指令会被忽略

    # 因为 proxy_pass 不是 rewrite 模块的指令 所以它不会被 break 终止
    proxy_pass https://www.baidu.com;
}
```

发送请求 127.0.0.1/test1，代理到 https://www.baidu.com


例子3:

```
server {

location /gerry/ {
    # redirect $ip/gerry to http://gerryyang.com
    rewrite ^/gerry/ http://gerryyang.com;
}

location /nginx/ {
    # redirect $ip/nginx/2021/09/07/nginx-in-action.html to http://gerryyang.com/nginx/2021/09/07/nginx-in-action.html
    rewrite ^/nginx/(.*)$ http://gerryyang.com/nginx/$1?;
}

}
```

# Directive

## proxy_pass

```
Syntax:	proxy_pass URL;
Default:	—
Context:	location, if in location, limit_except
```

http://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_pass

# 证书申请

可以在腾讯云申请证书，每年承担一定的费用。

# 域名配置

申请完域名后，可以配置不同的二级域名。

![dns_record_config](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201811/dns_record_config.jpg)


# 配置二级域名路由

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



# Refer

* [Status Code Definitions](https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html)
* [证书安装指引](https://cloud.tencent.com/document/product/400/4143)
* [nginx配置二级域名](https://cloud.tencent.com/developer/article/1198752)
* [一文理清 nginx 中的 location 配置（系列一）](https://segmentfault.com/a/1190000022315733)
* [一文理清 nginx 中的 rewrite 配置（系列二）](https://segmentfault.com/a/1190000022407797)
* [Creating NGINX Rewrite Rules](https://www.nginx.com/blog/creating-nginx-rewrite-rules/)
* [How To Install Nginx on Ubuntu 18.04](https://www.digitalocean.com/community/tutorials/how-to-install-nginx-on-ubuntu-18-04)
* [TCP and UDP Load Balancing](https://docs.nginx.com/nginx/admin-guide/load-balancer/tcp-udp-load-balancer/)