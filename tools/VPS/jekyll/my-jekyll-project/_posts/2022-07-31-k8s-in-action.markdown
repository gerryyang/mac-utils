---
layout: post
title:  "Kubernetes in Action"
date:   2022-07-31 16:30:00 +0800
categories: 虚拟化
---

* Do not remove this line (it will not be displayed)
{:toc}


关键字：容器编排，Kubernetes，云原生

# 容器编排

容器技术的核心概念是容器，镜像，仓库。使用这三大基本要素就可以轻松地完成应用的打包，分发工作，实现“一次分发，到处运行”的梦想。不过，当要在服务器集群里大规模实施的时候，却会发现容器技术的创新只是解决了运维部署工作中的一部分。除此之外，还包括服务发现，负载均衡，状态监控，健康检查，动态扩缩容等。

这些容器之上的管理，调度工作，就是所谓的“容器编排”（Container Orchestration）。面对单机上的几个容器，“人肉”编排调度还可以应付，但如果面对规模成千上万的容器，处理它们之间的复杂联系就必须依靠计算机了，而目前计算机用来调度管理的“事实标准”，就是 Kubernetes。

# Kubernetes

作为世界上最大的搜索引擎，Google 拥有数量庞大的服务器集群，为例提高资源利用率和部署运维效率，专门开发了一个集群管理系统 `Borg`。在 2014 年，因为之前在发表 MapReduce，BigTable，GFS 时吃过亏，被 Yahoo 开发的 Hadoop 占领了市场，所以 Google 决定借着 Docker 的“东风”，在发表论文的同时，把 C++ 开发的 Borg 系统用 Go 语言重写并开源，于是 `Kubernetes` 就这样诞生了。然后，在 2015 年，Google 又联合 Linux 基金会成立了 CNCF (Cloud Native Computing Foundation，云原生基金会)，并把 Kubernetes 捐献出来作为种子项目。有了 Google 和 Linux 这两大家族的保驾护航，再加上宽容开放的社区，Kubernetes 仅用了两年的时间就打败了同期的竞争对手 `Apache Mesos` 和 `Docker Swarm`，成为了这个领域的唯一霸主。

简单来说，Kubernetes 就是一个生产级别的容器编排平台和集群管理系统，不能能够创建，调度容器，还能够监控，管理服务器，从而可以具备运维海量计算节点，即云计算的能力。

> Borg 系统的名字来自于《星际迷航》（Star Trek）里的外星人种族，Kubernetes 在开发之初为了延续与 Borg 的关系，使用了一个代号 Seven of Nine ，即 Borg 与地球文明之间联络人的名字，隐喻从内部系统到开源项目，所以 Kubernetes 的标志有七条轮辐。Kubernetes 这个词来自希腊语，意思是“舵手”，“领航员”，可以理解成是操控着满载集装箱（容器）大船的指挥官。Kubernetes 有时候会缩写成 “k8s”，这个是因为 k 和 s 之间有 8 个字符，类似的还有 i18n (internationalization)

# minikube

Kubernetes 提供了一些快速搭建环境的工具，可以在本机上运行完整的 Kubernetes 环境。

minikube 集成了 Kubernetes 的绝大多数功能特性，不仅有核心的容器编排功能，还有丰富的插件，例如：Dashboard，Ingress，Istio 等。从而可通过 minikube 来学些 Kubernetes。

## 搭建 minikube 环境

minikube 支持 Mac，Windows，Linux 这三种主流平台，可以在 https://minikube.sigs.k8s.io 官网找到详细的安装说明。

> 说明：minikube 不包含在系统自带的 apt/yum 软件仓库里，只能自己去网上找安装包。不过因为它是用 Go 语言开发的，整体就是一个二进制文件，没有多余的依赖，所以安装过程非常简单，只需要用 curl 或者 wget 下载就行。下载的时候，需要注意计算机的硬件架构，Intel 芯片要选择带 amd64 后缀，Apple M1 芯片要选择 arm64 后缀，如果选错了就会因为 CPU 指令集不同而无法运行。

安装脚本：

``` bash
#!/bin/bash

curl -Lo minikube https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64
sudo install minikube /usr/local/bin/
echo "done"
```

安装完成：

```
$ ls -lh `which minikube`
-rwxr-xr-x 1 root root 73M Aug 25 09:44 /usr/local/bin/minikube

$ minikube version
minikube version: v1.26.1
commit: 62e108c3dfdec8029a890ad6d8ef96b6461426dc
```

不过 minikube 只能够搭建 Kubernetes 环境，要操作 Kubernetes，还需要另一个专门的客户端工具 kubectl。所以，在 minikube 环境里，会用到两个客户端：minikube 管理 Kubernetes 集群环境，kubectl 操作实际的 Kubernetes 功能。kubectl 是一个与 Kubernetes、minikube 彼此独立的项目，所以不包含在 minikube 里，但 minikube 提供了安装它的简化方式，只需执行下面的这条命令，就会把与当前 Kubernetes 版本匹配的 kubectl 下载下来，存放在内部目录（例如 .minikube/cache/linux/arm64/v1.23.3），然后就可以使用它来对 Kubernetes“发号施令”了。

``` bash
#!/bin/bash

minikube kubectl
echo "done"
```

使用命令 minikube start 会从 Docker Hub 上拉取镜像，以当前最新版本的 Kubernetes 启动集群。不过为了保证实验环境的一致性，可以在后面再加上一个参数 --kubernetes-version，明确指定要使用 Kubernetes 版本。

```
#!/bin/bash
# start_minikube.sh

minikube start --kubernetes-version=v1.23.3
echo "done"
```

```
$ ./start_minikube.sh
* minikube v1.26.1 on Centos 7.2 (amd64)
* Automatically selected the docker driver
! For improved Docker performance, Upgrade Docker to a newer version (Minimum recommended version is 20.10.0, minimum supported version is 18.09.0, current version is 18.09.7)
* Using Docker driver with root privileges
* Starting control plane node minikube in cluster minikube
* Pulling base image ...
* Downloading Kubernetes v1.23.3 preload ...
    > preloaded-images-k8s-v18-v1...:  400.43 MiB / 400.43 MiB  100.00% 7.40 Mi
    > gcr.io/k8s-minikube/kicbase:  386.60 MiB / 386.61 MiB  100.00% 5.16 MiB p
    > gcr.io/k8s-minikube/kicbase:  0 B [_______________________] ?% ? p/s 1m8s
! minikube was unable to download gcr.io/k8s-minikube/kicbase:v0.0.33, but successfully downloaded gcr.io/k8s-minikube/kicbase:v0.0.33 as a fallback image
* Creating docker container (CPUs=2, Memory=7900MB) ...
* Preparing Kubernetes v1.23.3 on Docker 20.10.17 ...
  - Generating certificates and keys ...
  - Booting up control plane ...
  - Configuring RBAC rules ...
* Verifying Kubernetes components...
  - Using image gcr.io/k8s-minikube/storage-provisioner:v5
* Enabled addons: storage-provisioner, default-storageclass
* kubectl not found. If you need it, try: 'minikube kubectl -- get pods -A'
* Done! kubectl is now configured to use "minikube" cluster and "default" namespace by default
done
```

```
$ docker image ls
REPOSITORY                                     TAG                 IMAGE ID            CREATED             SIZE
gcr.io/k8s-minikube/kicbase                    v0.0.33             b7ab23e98277        3 weeks ago         1.14GB
```

> 注意：由于国内网络环境的原因，下载 gcr.io 的镜像比较困难，minikube 提供了特殊的启动参数 --image-mirror-country=cn --registry-mirror=xxx --image-repository=xxx 等，如果遇到问题可以尝试下。例如：minikube start --image-mirror-country='cn' --kubernetes-version=v1.23.3 --force

现在 Kubernetes 集群就已经在本地运行了，可以使用下面命令来查看集群的状态：

``` bash
$ minikube status
minikube
type: Control Plane
host: Running
kubelet: Running
apiserver: Running
kubeconfig: Configured
```

``` bash
$ minikube node list
minikube        192.168.49.2
```

可以看到，Kubernetes 集群里现在只有一个节点，名字就叫 minikube，类型是 Control Plane，里面有 host、kubelet、apiserver 三个服务，IP 地址是 192.168.49.2。

可以用命令 minikube ssh 登录到这个节点上，虽然它是虚拟的，但用起来和实机也没什么区别：

``` bash
$ minikube ssh
Last login: Thu Aug 25 02:04:57 2022 from 192.168.49.1
docker@minikube:~$ pwd
/home/docker
```

接下来就可以使用 kubectl 来操作一下，初步体会 Kubernetes 这个容器编排系统。

> 注意：因为使用 minikube 自带的 kubectl 有一点形式上的限制，要在前面加上 minikube 的前缀

```
$ minikube kubectl -- version
Client Version: version.Info{Major:"1", Minor:"23", GitVersion:"v1.23.3", GitCommit:"816c97ab8cff8a1c72eccca1026f7820e93e0d25", GitTreeState:"clean", BuildDate:"2022-01-25T21:25:17Z", GoVersion:"go1.17.6", Compiler:"gc", Platform:"linux/amd64"}
Server Version: version.Info{Major:"1", Minor:"23", GitVersion:"v1.23.3", GitCommit:"816c97ab8cff8a1c72eccca1026f7820e93e0d25", GitTreeState:"clean", BuildDate:"2022-01-25T21:19:12Z", GoVersion:"go1.17.6", Compiler:"gc", Platform:"linux/amd64"}
```

为了避免这个不大不小的麻烦，建议使用 Linux 的 alias 功能，为它创建一个别名，写到当前用户目录下的 .bashrc 里：

``` bash
alias kubectl="minikube kubectl --"
```

之后就可以直接使用 kubectl 命令了。

```
$ kubectl version --short
Client Version: v1.23.3
Server Version: v1.23.3
```

下面在 Kubernetes 里运行一个 Nginx 应用，命令与 Docker 一样，也是 run，不过形式上有点区别，需要用 --image 指定镜像，然后 Kubernetes 会自动拉取并运行：

```
$ kubectl run ngx --image=nginx:alpine
pod/ngx created
$ kubectl get pod
NAME   READY   STATUS    RESTARTS   AGE
ngx    1/1     Running   0          3s
$ kubectl delete pod ngx
pod "ngx" deleted
$ kubectl get pod
No resources found in default namespace.
```


# Kubernetes 工具

https://kubernetes.io/zh-cn/docs/tasks/tools/

## kubectl

kubectl 是 Kubernetes 的命令行工具，使得可以对 Kubernetes 集群运行命令。可使用 kubectl 来部署应用、监测和管理集群资源以及查看日志。关于 kubectl 的更多用法可参考：https://kubernetes.io/zh-cn/docs/reference/kubectl/

## minikube

minikube 是一个工具，能在本地运行 Kubernetes。 minikube 在本地的个人计算机（包括 Windows、macOS 和 Linux PC）运行一个单节点的 Kubernetes 集群，以便来尝试 Kubernetes 或者开展每天的开发工作。[开始使用](https://minikube.sigs.k8s.io/docs/start/)

> minikube is local Kubernetes, focusing on making it easy to learn and develop for Kubernetes.
>
> All you need is Docker (or similarly compatible) container or a Virtual Machine environment, and Kubernetes is a single command away: **minikube start**


# Tips

## kubectl completion (补全功能)

参考`kubectl completion -h`

```
source <(kubectl completion bash)
```



# Q&A

## [How does kubernetes guarantee reliability of kube proxy and kubelet?](https://stackoverflow.com/questions/57177943/how-does-kubernetes-guarantee-reliability-of-kube-proxy-and-kubelet)

If `Kube proxy` is down, the pods on a kubernetes node will not be able to communicate with the external world. Anything that Kubernetes does specially to guarantee the reliability of kube-proxy?

Similarly, how does Kubernetes guarantee reliability of `kubelet`?

Answers:

It guarantees their reliability by:

* **Having multiple nodes**: If one `kubelet` crashes, one node goes down. Similarly, every node runs a `kube-proxy` instance, which means losing one node means losing the `kube-proxy` instance on that node. Kubernetes is designed to handle node failures. And if you designed your app that is running on Kubernetes to be scalable, you will not be running it as single instance but rather as multiple instances - and `kube-scheduler` will distribute your workload across multiple nodes - which means your application will still be accessible.

* **Supporting a Highly-Available Setup**: If you set up your Kubernetes cluster in [High-Availability mode](https://kubernetes.io/docs/setup/production-environment/tools/kubeadm/high-availability/) properly, there won't be one master node, but multiple. This means, you can even tolerate losing some master nodes. The managed Kubernetes offerings of the cloud providers are always highly-available.

These are the first 2 things that come to my mind. However, this is a broad question, so I can go into details if you elaborate what you mean by "reliability" a bit.


## [How to stop/pause a pod in kubernetes](https://stackoverflow.com/questions/54821044/how-to-stop-pause-a-pod-in-kubernetes)

I have a MySQL pod running in my cluster.

I need to temporarily pause the pod from working without deleting it, something similar to docker where the docker stop container-id cmd will stop the container not delete the container. Are there any commands available in kubernetes to pause/stop a pod?

Answers:

So, like others have pointed out, Kubernetes doesn't support stop/pause of current state of pod and resume when needed. However, you can still achieve it by having no working deployments which is setting number of replicas to 0.

```
kubectl scale --replicas=0 deployment/<your-deployment>
```

see the help

``` bash
# Set a new size for a Deployment, ReplicaSet, Replication Controller, or StatefulSet.
kubectl scale --help
```

Scale also allows users to specify one or more preconditions for the scale action.

If --current-replicas or --resource-version is specified, it is validated before the scale is attempted, and it is guaranteed that the precondition holds true when the scale is sent to the server.

Examples:

``` bash
# Scale a replicaset named 'foo' to 3.
kubectl scale --replicas=3 rs/foo

# Scale a resource identified by type and name specified in "foo.yaml" to 3.
kubectl scale --replicas=3 -f foo.yaml

# If the deployment named mysql's current size is 2, scale mysql to 3.
kubectl scale --current-replicas=2 --replicas=3 deployment/mysql

# Scale multiple replication controllers.
kubectl scale --replicas=5 rc/foo rc/bar rc/baz

# Scale statefulset named 'web' to 3.
kubectl scale --replicas=3 statefulset/web
```

如果也可以使用删除操作：

With Kubernets, it's not possible to stop/pause a Pod. However, you can delete a Pod, given the fact you have the manifest to bring that back again.

If you want to delete a Pod, you can run the following kubectl command:

```
kubectl delete -n default pod <your-pod-name>
```

# Refer

* [Kubernetes 官网](https://kubernetes.io/zh-cn/)