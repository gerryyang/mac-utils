#!/bin/bash
# docker services

# restart docker
service docker restart
# check docker status
service docker status

sleep 2

# ssh service
echo "start ssh service"
docker run -p 10022:22 -d ubuntu_sshd_gcc_gerry:14.04 /run.sh

# personal registry
echo "start personal registry"
docker run -p 5000:5000 -d -v /root/my_registry:/tmp/registry registry

# apache service
echo "start apache service"
#docker run -d -P ubuntu_sshd_gcc_apache_gerry:14.04
#docker run -d -p 20080:80 -p 20022:22 ubuntu_sshd_gcc_apache_gerry:14.04
docker run -d -p 20080:80 -p 20022:22 -e APACHE_SERVERNAME=test -v /root/apache_ubuntu/www:/var/www/html:ro ubuntu_sshd_gcc_apache_gerry:14.04

