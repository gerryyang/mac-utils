###Mysql
####下载文件
到 Dockerpool 的 github 页面下载需要的文件。
```sh
$ git clone https://github.com/DockerPool/mysql.git
Cloning into 'mysql'...
remote: Counting objects: 13, done.
remote: Compressing objects: 100% (13/13), done.
remote: Total 13 (delta 1), reused 8 (delta 0)
Unpacking objects: 100% (13/13), done.
Checking connectivity... done.
$ cd mysql
$ ls
create_db.sh  Dockerfile  import_sql.sh  LICENSE  my.cnf  mysqld_charset.cnf  README.md  run.sh
```
####创建镜像
根据 Dockerfile 创建镜像。（删除部分输出内容用`...`表示）
```sh
$ sudo docker build mysql .
Sending build context to Docker daemon 95.23 kB
Sending build context to Docker daemon
Step 0 : FROM sshd
 ---> 312c93647dc3
Step 1 : MAINTAINER Waitfish <dwj_zz@163.com>
 ---> Running in a149f8a7933f
 ---> edbbfe8b4895
Removing intermediate container a149f8a7933f
Step 2 : ENV DEBIAN_FRONTEND noninteractive
 ---> Running in e80cbb29cadb
 ---> 81fc6101a236
Removing intermediate container e80cbb29cadb
Step 3 : RUN apt-get update &&   apt-get -yq install mysql-server-5.6 pwgen &&   rm -rf /var/lib/apt/lists/*
 ---> Running in 5d220fe833c2
...
Removing intermediate container 3c3254e8cc1e
Successfully built f008f97bdc14
dwj@iZ23pznlje4Z:~/mysql$ sudo docker images
REPOSITORY          TAG                 IMAGE ID            CREATED              VIRTUAL SIZE
mysql               latest              f008f97bdc14        About a minute ago   539.1 MB
```
####使用示范：

不添加环境变量，使用默认方式启动容器，并映射 `22 3306`端口。
```sh
$ sudo docker run -d -P mysql
```
检查容器进程启动情况。
```sh
$ sudo docker ps
CONTAINER ID        IMAGE               COMMAND             CREATED             STATUS              PORTS                                            NAMES
eef1632ccd4e        mysql:latest        "/run.sh"           8 seconds ago       Up 8 seconds        0.0.0.0:49153->22/tcp, 0.0.0.0:49154->3306/tcp   angry_einstein

$ ssh 127.0.0.1 -p 49153
The authenticity of host '[127.0.0.1]:49153 ([127.0.0.1]:49153)' can't be established.
ECDSA key fingerprint is db:35:7a:60:2d:11:d5:97:5a:e6:84:a6:95:f0:4f:32.
Are you sure you want to continue connecting (yes/no)? yes
Warning: Permanently added '[127.0.0.1]:49153' (ECDSA) to the list of known hosts.
Welcome to Ubuntu 14.04 LTS (GNU/Linux 3.2.0-54-generic x86_64)

 * Documentation:  https://help.ubuntu.com/

The programs included with the Ubuntu system are free software;
the exact distribution terms for each program are described in the
individual files in /usr/share/doc/*/copyright.

Ubuntu comes with ABSOLUTELY NO WARRANTY, to the extent permitted by
applicable law.

root@eef1632ccd4e:~# ps -ef |grep mysql
root         1     0  0 20:14 ?        00:00:00 /bin/sh /usr/bin/mysqld_safe
mysql     1974     1  0 20:14 ?        00:00:00 /usr/sbin/mysqld --basedir=/usr --datadir=/var/lib/mysql --plugin-dir=/usr/lib/mysql/plugin --user=mysql --log-error=/var/log/mysql/error.log --pid-file=/var/run/mysqld/mysqld.pid --socket=/var/run/mysqld/mysqld.sock --port=3306
root      2022  2010  0 20:15 pts/0    00:00:00 grep --color=auto mysql
```
Mysql 的 root 用户默认没有密码只能本地访问。
```sh
mysql> select host, user, password from mysql.user;
+--------------+-------+-------------------------------------------+
| host         | user  | password                                  |
+--------------+-------+-------------------------------------------+
| localhost    | root  |                                           |
| eef1632ccd4e | root  |                                           |
| 127.0.0.1    | root  |                                           |
| ::1          | root  |                                           |
| localhost    |       |                                           |
| eef1632ccd4e |       |                                           |
| %            | admin | *ADDD6793DD97A040C9B039F72682E5AA31A92C35 |
+--------------+-------+-------------------------------------------+
7 rows in set (0.00 sec)
```
拥有远程访问权限的 admin 用户的密码，可以使用 `docker logs + id` 来获取。
```sh
$ sudo docker logs eef
=> An empty or uninitialized MySQL volume is detected in /var/lib/mysql
=> Installing MySQL ...
=> Done!
=> Creating admin user ...
=> Waiting for confirmation of MySQL service startup, trying 0/13 ...
=> Creating MySQL user admin with random password
=> Done!
========================================================================
You can now connect to this MySQL Server using:

    mysql -uadmin -pt1FWuDCgQicT -h<host> -P<port>

Please remember to change the above password as soon as possible!
MySQL user 'root' has no password but only allows local connections
========================================================================
141106 20:14:21 mysqld_safe Can't log to error log and syslog at the same time.  Remove all --log-error configuration options for --syslog to take effect.
141106 20:14:21 mysqld_safe Logging to '/var/log/mysql/error.log'.
141106 20:14:21 mysqld_safe Starting mysqld daemon with databases from /var/lib/mysql
```
上面的 `t1FWuDCgQicT` 就是 admin 的密码。

####给 admin 用户指定用户名和密码
```sh
$ sudo docker run -d -P -e MYSQL_PASS="mypass" mysql
1b32444ebb7232f885961faa15fb1a052ca93b81c308cc41d16bd3d276c77d75
```
####将宿主主机的文件夹挂载到容器的数据库文件夹
默认情况数据库的数据库文件和日志文件都会存在容器的 AUFS 层，这不仅使得容器变得越来越臃肿，不便于迁移、备份等管理，而且数据库的 IOPS 也会受到影响。
```sh
$ docker run -d -P -v /opt/mysqldb:/var/lib/mysql mysql
```
这样，容器就会将数据文件和日志文件都放到你指定的 主机目录下面。
```sh
$ tree /opt/mysqldb/
/opt/mysqldb/
|-- auto.cnf
|-- ib_logfile0
|-- ib_logfile1
|-- ibdata1
|-- mysql
|   |-- columns_priv.MYD
|   |-- columns_priv.MYI
|   |-- columns_priv.frm
|   |-- db.MYD
|   |-- db.MYI
|   |-- db.frm
|   |-- event.MYD
|   |-- event.MYI
|   |-- event.frm
|   |-- func.MYD
|   |-- func.MYI
|   |-- func.frm
|   |-- general_log.CSM
...
```
####使用主从复制模式
创建一个叫 mysql 的容器。
```sh
$ docker run -d -e REPLICATION_MASTER=true  -P  --name mysql  mysql
```
创建从 mysql 容器，并连接到刚刚创建的主容器。
```sh
$ docker run -d -e REPLICATION_SLAVE=true -P  --link mysql:mysql  mysql
```
注意：这里的主 mysql 服务器的名字必须叫 mysql，否则会提示 `Cannot configure slave, please link it to another MySQL container with alias as 'mysql'！
```sh
# docker ps
CONTAINER ID        IMAGE               COMMAND             CREATED              STATUS              PORTS                                            NAMES
a781d1c74024        mysql:latest        "/run.sh"           About a minute ago   Up About a minute   0.0.0.0:49167->22/tcp, 0.0.0.0:49168->3306/tcp   romantic_fermi
38c73b5555aa        mysql:latest        "/run.sh"           About a minute ago   Up About a minute   0.0.0.0:49165->22/tcp, 0.0.0.0:49166->3306/tcp   mysql
```
现在，你就可以通过相应的端口来连接主或者从 mysql 服务器了。
`
