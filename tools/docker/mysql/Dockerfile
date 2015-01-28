#本文参考了「tutum」的 Dockerfile
#FROM sshd
FROM ubuntu_sshd_gcc_gerry:14.04
MAINTAINER gerry
#安装软件 
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
  apt-get -yq install mysql-server-5.6 pwgen && \
  rm -rf /var/lib/apt/lists/*

# 删除预安装的数据库文件
RUN rm -rf /var/lib/mysql/*

# 添加文件夹下的 MYSQL 配置文件
ADD my.cnf /etc/mysql/conf.d/my.cnf
ADD mysqld_charset.cnf /etc/mysql/conf.d/mysqld_charset.cnf

# 添加 MYSQL 的脚本
ADD import_sql.sh /import_sql.sh
ADD run.sh /run.sh
RUN chmod 755 /*.sh

# 设置环境变量，用户名以及密码
ENV MYSQL_USER admin
ENV MYSQL_PASS **Random**

# 设置主从复制模式下的环境变量
ENV REPLICATION_MASTER **False**
ENV REPLICATION_SLAVE **False**
ENV REPLICATION_USER replica
ENV REPLICATION_PASS replica

# 设置可以允许挂载的卷，可以用来备份数据库和配置文件
VOLUME  ["/etc/mysql", "/var/lib/mysql"]

# 设置可以映射的端口，如果是从我们的 sshd 镜像继承的话，默认还会开启 22 端口
EXPOSE 3306
CMD ["/run.sh"]
