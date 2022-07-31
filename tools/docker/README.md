

# About Docker

Develop, Ship and Run Any Application, Anywhere

Docker is a platform for developers and sysadmins to develop, ship, and run applications. Docker lets you quickly assemble applications from components and eliminates the friction that can come when shipping code. Docker lets you get your code tested and deployed into production as fast as possible.

```
Docker consists of:

    The Docker Engine - our lightweight and powerful open source container virtualization technology combined with a work flow for building and containerizing your applications.
    Docker Hub - our SaaS service for sharing and managing your application stacks.
```

Docker是一个开源项目，诞生于2013年初，最初是dotCloud公司内部的一个业余项目。它基于Google公司推出的Go语言实现。项目后来加入了Linux基金会，遵从了Apache 2.0协议，项目代码在GitHub (https://github.com/docker/docker) 上进行维护。

Docker自开源后受到广泛的关注和讨论，以至于dotCloud公司后来都改名为Docker Inc。Redhat已经在其RHEL6.5中集中支持Docker；Google也在其PaaS产品中广泛应用。

Docker项目的目标是实现轻量级的操作系统虚拟化解决方案。Docker的基础是Linux 容器（LXC）等技术。

在LXC的基础上Docker进行了进一步的封装，让用户不需要去关心容器的管理，使得操作更为简便。用户操作Docker的容器就像操作一个快速轻量级的虚拟机一样简单。


# 为什么要用Docker

作为一种新兴的虚拟化方式，Docker跟传统的虚拟化方式相比具有众多的优势。

首先，Docker容器的启动可以在秒级实现，这相比传统的虚拟机方式要快得多。 其次，Docker对系统资源的利用率很高，一台主机上可以同时运行数千个Docker容器。

容器除了运行其中应用外，基本不消耗额外的系统资源，使得应用的性能很高，同时系统的开销尽量小。传统虚拟机方式运行10个不同的应用就要起10个虚拟机，而Docker只需要启动10个隔离的应用即可。

对比传统虚拟机总结
特性           容器                       虚拟机
启动           秒级                       分钟级
硬盘使用       一般为 MB                  一般为 GB
性能           接近原生                   弱于
系统支持量     单机支持上千个容器         一般几十个


# 基本概念

(1) Docker 镜像

Docker镜像就是一个只读的模板。

例如：一个镜像可以包含一个完整的ubuntu操作系统环境，里面仅安装了Apache或用户需要的其它应用程序。

镜像可以用来创建Docker容器。

Docker提供了一个很简单的机制来创建镜像或者更新现有的镜像，用户甚至可以直接从其他人那里下载一个已经做好的镜像来直接使用。

(2) Docker 容器

Docker利用容器来运行应用。

容器是从镜像创建的运行实例。它可以被启动、开始、停止、删除。每个容器都是相互隔离的、保证安全的平台。

可以把容器看做是一个简易版的Linux环境（包括root用户权限、进程空间、用户空间和网络空间等）和运行在其中的应用程序。

注：镜像是只读的，容器在启动的时候创建一层可写层作为最上层。

(3) Docker 仓库

仓库是集中存放镜像文件的场所。有时候会把仓库和仓库注册服务器（Registry）混为一谈，并不严格区分。实际上，仓库注册服务器上往往存放着多个仓库，每个仓库中又包含了多个镜像，每个镜像有不同的标签（tag）。

仓库分为公开仓库（Public）和私有仓库（Private）两种形式。

最大的公开仓库是 Docker Hub，存放了数量庞大的镜像供用户下载。国内的公开仓库包括 Docker Pool 等，可以提供大陆用户更稳定快速的访问。

当然，用户也可以在本地网络内创建一个私有仓库。

当用户创建了自己的镜像之后就可以使用 push 命令将它上传到公有或者私有仓库，这样下次在另外一台机器上使用这个镜像时候，只需要从仓库上 pull 下来就可以了。

注：Docker仓库的概念跟 Git 类似，注册服务器可以理解为 GitHub 这样的托管服务。


Docker系统有两个程序：

docker服务端和docker客户端。其中docker服务端是一个服务进程，管理着所有的容器。docker客户端则扮演着docker服务端的远程控制器，可以用来控制docker的服务端进程。大部分情况下，docker服务端和客户端运行在一台机器上。

docker容器可以理解为在沙盒中运行的进程。这个沙盒包含了该进程运行所必须的资源，包括文件系统、系统类库、shell 环境等等。但这个沙盒默认是不会运行任何程序的。你需要在沙盒中运行一个进程来启动某一个容器。这个进程是该容器的唯一进程，所以当该进程结束的时候，容器也会完全的停止。

Docker支持的安装方式：

Docker有很多种安装的选择，我们推荐您在Ubuntu下面安装，因为docker是在Ubuntu下面开发的，安装包测试比较充分，可以保证软件包的可用性。Mac, windows和其他的一些linux发行版本无法原生运行Docker，可以使用虚拟软件创建一个ubuntu的虚拟机并在里面运行docker。

```
gerryyang@mba:docker$docker run learn/tutorial uname -a
Linux 0038d4f77199 3.16.7-tinycore64 #1 SMP Tue Dec 16 23:03:39 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux

gerryyang@mba:docker$docker version
Client version: 1.4.1
Client API version: 1.16
Go version (client): go1.4
Git commit (client): 5bc2ff8
OS/Arch (client): darwin/amd64
Server version: 1.4.1
Server API version: 1.16
Go version (server): go1.3.3
Git commit (server): 5bc2ff8

gerryyang@mba:~$docker run --help

Usage: docker run [OPTIONS] IMAGE [COMMAND] [ARG...]

Run a command in a new container

  -a, --attach=[]            Attach to STDIN, STDOUT or STDERR.
  --add-host=[]              Add a custom host-to-IP mapping (host:ip)
  -c, --cpu-shares=0         CPU shares (relative weight)
  --cap-add=[]               Add Linux capabilities
  --cap-drop=[]              Drop Linux capabilities
  --cidfile=""               Write the container ID to the file
  --cpuset=""                CPUs in which to allow execution (0-3, 0,1)
  -d, --detach=false         Detached mode: run the container in the background and print the new container ID
  --device=[]                Add a host device to the container (e.g. --device=/dev/sdc:/dev/xvdc:rwm)
  --dns=[]                   Set custom DNS servers
  --dns-search=[]            Set custom DNS search domains (Use --dns-search=. if you don't wish to set the search domain)
  -e, --env=[]               Set environment variables
  --entrypoint=""            Overwrite the default ENTRYPOINT of the image
  --env-file=[]              Read in a line delimited file of environment variables
  --expose=[]                Expose a port or a range of ports (e.g. --expose=3300-3310) from the container without publishing it to your host
  -h, --hostname=""          Container host name
  -i, --interactive=false    Keep STDIN open even if not attached
  --ipc=""                   Default is to create a private IPC namespace (POSIX SysV IPC) for the container
                               'container:<name|id>': reuses another container shared memory, semaphores and message queues
                               'host': use the host shared memory,semaphores and message queues inside the container.  Note: the host mode gives the container full access to local shared memory and is therefore considered insecure.
  --link=[]                  Add link to another container in the form of name:alias
  --lxc-conf=[]              (lxc exec-driver only) Add custom lxc options --lxc-conf="lxc.cgroup.cpuset.cpus = 0,1"
  -m, --memory=""            Memory limit (format: <number><optional unit>, where unit = b, k, m or g)
  --mac-address=""           Container MAC address (e.g. 92:d0:c6:0a:29:33)
  --name=""                  Assign a name to the container
  --net="bridge"             Set the Network mode for the container
                               'bridge': creates a new network stack for the container on the docker bridge
                               'none': no networking for this container
                               'container:<name|id>': reuses another container network stack
                               'host': use the host network stack inside the container.  Note: the host mode gives the container full access to local system services such as D-bus and is therefore considered insecure.
  -P, --publish-all=false    Publish all exposed ports to the host interfaces
  -p, --publish=[]           Publish a container's port to the host
                               format: ip:hostPort:containerPort | ip::containerPort | hostPort:containerPort | containerPort
                               (use 'docker port' to see the actual mapping)
  --privileged=false         Give extended privileges to this container
  --restart=""               Restart policy to apply when a container exits (no, on-failure[:max-retry], always)
  --rm=false                 Automatically remove the container when it exits (incompatible with -d)
  --security-opt=[]          Security Options
  --sig-proxy=true           Proxy received signals to the process (non-TTY mode only). SIGCHLD, SIGSTOP, and SIGKILL are not proxied.
  -t, --tty=false            Allocate a pseudo-TTY
  -u, --user=""              Username or UID
  -v, --volume=[]            Bind mount a volume (e.g., from the host: -v /host:/container, from Docker: -v /container)
  --volumes-from=[]          Mount volumes from the specified container(s)
  -w, --workdir=""           Working directory inside the container
```



# Tips

``` bash
# 查看所有docker命令
docker help

# 更新docker
boot2docker stop
boot2docker download
boot2docker start/up

# 显示版本
docker version

# 查看所有正在运行中的容器列表
docker ps -l

# 查看更详细的关于某一个容器的信息
docker inspect $containerid


# docker info
gerryyang@mba:~$docker info
Containers: 18
Images: 27
Storage Driver: aufs
 Root Dir: /mnt/sda1/var/lib/docker/aufs
 Dirs: 63
Execution Driver: native-0.2
Kernel Version: 3.16.7-tinycore64
Operating System: Boot2Docker 1.4.1 (TCL 5.4); master : 86f7ec8 - Tue Dec 16 23:11:29 UTC 2014
CPUs: 4
Total Memory: 1.961 GiB
Name: boot2docker
ID: V5OZ:LVOS:25OJ:3AY2:DGIF:SV3U:RTJ6:7JIY:FXU4:JCU3:ACQW:IT54
Debug mode (server): true
Debug mode (client): false
Fds: 18
Goroutines: 18
EventsListeners: 0
Init Path: /usr/local/bin/docker
Docker Root Dir: /mnt/sda1/var/lib/docker
Username: gerryyang
Registry: [https://index.docker.io/v1/]

# 停止运行某个容器
docker stop $containerid

# 重启某个容器
docker restart $containerid

# 如果要完全移除容器，需要将该容器停止，然后才能移除
docker stop $containerid && docker rm $containerid

# 将容器的状态保存为镜像，注意，镜像名称只能取字符[a-z]和数字[0-9]，镜像是存储在Docker registry
# Create a new image from a container's changes
# 当你对某一个容器做了修改之后（通过在容器中运行某一个命令），可以把对容器的修改保存下来，这样下次可以从保存后的最新状态运行该容器。docker中保存状态的过程称之为committing，它保存的新旧状态之间的区别，从而产生一个新的版本。执行完docker commit命令之后，会返回新版本镜像的id号
docker commit $containerid job1

# 搜索镜像
# 在docker的镜像索引网站上面，镜像都是按照用户名/镜像名的方式来存储的。有一组比较特殊的镜像，比如ubuntu这类基础镜像，经过官方的验证，值得信任，可以直接用镜像名来检索到
docker search image-name

# 查看镜像的历史版本
docker history image-name

# 下载镜像(docker命令和git有一些类似的地方)
docker pull learn/tutorial

# 发布镜像到docker的index网站(你只能将镜像发布到自己的空间下面)
docker push gerryyang/ping

# 显示本地所有的镜像
docker images

# docker tag
# 为本地镜像添加新的标签，例如：
docker tag dl.dockerpool.com:5000/ubuntu:14.04 dockerpool_ubuntu_gerry:v14.04

# docker inspect
# 获取该镜像的详细信息
docker inspect -f {{".Architecture"}}  550
amd64

# docker logs
# 查看docker启动的日志信息
docker logs $containerid
# 类似tail -f
docker logs -f $cid
# 显示最近的10行日志
docker logs --tail=10 $cid
# 不推荐
docker logs contaiername >& logs/myFile.log
# 推荐
# No need to redirect logs, Docker by default store logs to one log file. To check log file path run command
root@ubuntu-s-1vcpu-3gb-nyc3-01:~/debug# docker inspect --format='{{.LogPath}}' 9d0329060dbb
/var/lib/docker/containers/9d0329060dbb3d878f6bac1a7cbb09c29b3e9b31458f55a372372cd36d6aec9f/9d0329060dbb3d878f6bac1a7cbb09c29b3e9b31458f55a372372cd36d6aec9f-json.log
root@ubuntu-s-1vcpu-3gb-nyc3-01:~/debug# docker inspect --format='{{.LogPath}}' peer1.org2.example.com
/var/lib/docker/containers/5ed79df36f3510aeec0186803b6b74253287e9fd6ae8dd8bdc1ca76603684b45/5ed79df36f3510aeec0186803b6b74253287e9fd6ae8dd8bdc1ca76603684b45-json.log


# docker rmi REPOSITORY:TAG
# 删除镜像，例如：
docker rmi ubuntu:lucid
Untagged: ubuntu:lucid
# 注意：应该先删除依赖该镜像的所有容器，再来删除镜像。否则会有如下错误提示：
root@gerryyang:~# docker rmi 550
Error response from daemon: Conflict, cannot delete image 550 because it is tagged in multiple repositories, use -f to force
2015/01/10 06:06:22 Error: failed to remove one or more images

# 若一个容器内运行了多个进程，可以通过docker top来查看进程信息
docker top $containerid

# 进入某个容器
docker exec -it $cid /bin/bash
```



# Refer

入门手册

* https://www.docker.com/
* http://www.docker.io/gettingstarted/#0
* http://www.docker.org.cn/
* http://www.docker.org.cn/book/docker.html

镜像

* index.docker.io
* http://docs.docker.com/
* http://docs.docker.com/installation/#installation
* http://docs.docker.com/installation/mac/
* https://www.docker.com/tryit/#

Docker入门教程

* http://dockerone.com/article/111

Dockerfile Best Practices

* http://crosbymichael.com/dockerfile-best-practices.html
* https://github.com/yeasy/docker_practice

Docker —— 从入门到实践

* http://yeasy.gitbooks.io/docker_practice/content/

Install Docker

* http://blog.csdn.net/delphiwcdj/article/details/41780063
* http://blog.tankywoo.com/docker/2014/05/08/docker-4-summary.html
* https://raw.githubusercontent.com/dotcloud/docker/master/contrib/check-config.sh
* http://blog.thoward37.me/articles/where-are-docker-images-stored/
* http://kencochrane.net/blog/2013/08/the-docker-guidebook/#terminology
* http://segmentfault.com/a/1190000000366923
* http://cdn.oreillystatic.com/en/assets/1/event/115/Introduction%20to%20Docker_%20Containerization%20is%20the%20New%20Virtualization%20Presentation.pdf
* http://viget.com/extend/how-to-use-docker-on-os-x-the-missing-guide

boycott docker

* http://www.boycottdocker.org/




