---
layout: post
title:  "Docker in Action"
date:   2019-02-22 08:00:00 +0800
categories: 虚拟化
---

* Do not remove this line (it will not be displayed)
{:toc}


# 核心技术

Docker底层是基于成熟的`Linux Container(LXC)`技术实现。自Docker 0.9版本起，Docker除了继续支持LXC之外，还开始引入自家的[runc]，试图打造更通用的底层容器虚拟化库。

> runc is a CLI tool for spawning and running containers according to the OCI specification.

从操作系统功能上看，Docker底层依赖的**核心技术**主要包括：

* Namespaces (Linux操作系统的命名空间)
* Control Groups (控制组)
* Union File Systems (联合文件系统，目前支持的联合文件系统种类包括，AUFS, btrfs, vfs和DeviceMapper等)
* Linux虚拟网络

[runc]: https://github.com/opencontainers/runc

# 开发流程对比

* 在Docker应用中，项目架构师的作用贯穿整个开发，测试，生产三个环节。
* 项目伊始，架构师根据项目预期创建好需要的基础Base镜像（比如，Nginx, Tomcat, MySQL等镜像），或者将Dockerfile分发给所有开发，所有开发根据Dockerfile创建的镜像来进行开发，达到开发环境充分一致。
* 若开发过程中需要添加新的程序，需要向架构师申请修改基础的Base镜像的Dockerfile。
* 开发完成后，将Dockerfile提交给测试，消除部署困难的问题。
* 使用`-v`共享文件夹来存储开发的程序代码。
* 利用好Base镜像的**继承特性**来调整镜像的轻微改动。即，只需要修改Base镜像，而不需要修改其他依赖的镜像。
* 对Docker化程序和原生程序进行性能测试对比。
* 如果Docker出现不可控的风险，是否有替代方案。
* 是否需要对Docker容器做资源限制。
* 容器安全管理。
* 内部私有仓库的管理。
* 建议一个容器内只运行一个应用进程。
* Vagrant适合管理虚拟机，而Docker适合管理应用环境。Docker不是虚拟机，而是进程隔离，对于资源的消耗很少，但是目前需要Linux环境支持。Vagrant是虚拟机上做的封装，虚拟机本身会消耗资源。


![docker_dev_flow](/assets/images/201902/docker_dev_flow.jpg)

![docker_dev_flow_example](/assets/images/201902/docker_dev_flow_example.jpg)

## 常用命令

![docker_cmd](/assets/images/201902/docker_cmd.jpg)

## Q&A

## [How can I find a Docker image with a specific tag in Docker registry on the Docker command line?](https://stackoverflow.com/questions/24481564/how-can-i-find-a-docker-image-with-a-specific-tag-in-docker-registry-on-the-dock)

Q:

I try to locate one specific tag for a Docker image. How can I do it on the command line? I want to avoid downloading all the images and then removing the unneeded ones.

A:


``` bash
#!/usr/bin/bashs

# docker_remote_tags.sh
curl -s -S "https://registry.hub.docker.com/v2/repositories/library/$@/tags/" | jq '."results"[]["name"]' |sort
```

Reference:

jq: https://stedolan.github.io/jq/ | apt-get install jq


## 历史文章

[Where are Docker images stored? (杂译)]

[使用Docker registry镜像创建私有仓库]

[Docker使用桥接的通信方案]

[github-docker]

## 官方文档

[Docker Release Notes]

[Docker development best practices]



[Where are Docker images stored? (杂译)]: https://blog.csdn.net/delphiwcdj/article/details/43602877

[使用Docker registry镜像创建私有仓库]: https://blog.csdn.net/delphiwcdj/article/details/43099877

[Docker使用桥接的通信方案]: https://blog.csdn.net/delphiwcdj/article/details/49508045

[github-docker]: https://github.com/gerryyang/mac-utils/tree/master/tools/docker

[Docker Release Notes]: https://docs.docker.com/release-notes/

[Docker development best practices]: https://docs.docker.com/develop/dev-best-practices/