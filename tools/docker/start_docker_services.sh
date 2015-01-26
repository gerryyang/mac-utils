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
docker run -d -P ubuntu_sshd_gcc_apache_gerry:14.04
