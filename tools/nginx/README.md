
nginx (pronounced "engine x") is a free open source web server written by Igor Sysoev, a Russian software engineer. Since its public launch in 2004, nginx has focused on high performance, high concurrency and low memory usage. Additional features on top of the web server functionality, like load balancing, caching, access and bandwidth control, and the ability to integrate efficiently with a variety of applications, have helped to make nginx a good choice for modern website architectures. Currently nginx is the second most popular open source web server on the Internet.
http://aosabook.org/en/nginx.html

# 关于nginx源码下载

nginx的官网(http://nginx.org/)没有提供所有历史版本的下载链接，但nginx的所有版本源码包都放在目录http://nginx.org/download/下的。


# Debug

Debugging Requests

Nginx is a complicated server at times, thankfully we have an excellent error log available to us to help figure out where things are going wrong. If you check the error log directive in the documentation you will notice that it takes a second argument. This will let you define how much information is output by nginx. A value of warn will give you sufficient info to debug most issues.

For more detailed reading on debugging please see my debugging nginx errors post.

http://blog.martinfjordvald.com/2013/06/debugging-nginx-errors/


# Refer

[Mac OS X 10.9.3 install nginx-1.7.2](http://blog.csdn.net/delphiwcdj/article/details/35291965)

[nginx core configure note](http://blog.csdn.net/delphiwcdj/article/details/35787517)