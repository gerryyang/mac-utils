# Docker与Kubernetes的研究与实践
@(马克飞象的笔记)[dezhi, gerry, kevint, will, xixi]


**目录**

[TOC]

-------------------

# 1 现实

理想是美好的，现实是`残酷的`。

* **环境差异**
	* DTO环境迥异，可能导致最终交付不一致。
* **资源分配**
	* 机器类型不同，可能出现资源利用不均衡。
* **集群管理**
	* 机器数量众多，可能带来系统发布不高效。

# 2 对策

利用Docker技术，将`系统容器化` = **Runtime** + **Image分发机制** + **Orchestration**。

![p0|right|350*0](./d32f4052f100acf8b945ce515850859f1451444049.jpg)

* 解决环境复杂依赖 —— DevOps
* 更多粒度资源隔离 —— 降低成本
* 容器编排秒级发布 —— 动态伸缩，容灾容错

因此，我们认为目前Docker最适合的使用的场景有：

> **1. 无状态的服务**
> **2. 需要经常部署的系统或模块**
> **3. 测试环境**


# 3 问题

在系统容器化的过程中，我们可能遇到`哪些问题`(Keng)？

## 3.1 基础

* **宿主机的选择**

> 物理机/虚拟机（Xen, VMWare, KVM, etc.）+ Docker。很多企业先使用OpenStack构建IaaS（Infrastructure as a Service），然后在IaaS的基础上构建容器服务。
> * 外部应用，安全的考虑。
> * Gaia + 游戏云基于腾讯云
> * 阿里容器服务基于阿里云ECS。

* **操作系统的要求**

> Docker官方建议，Linux操作系统，`内核版本大于3.8`。因此，在公司内Docker的宿主机需要使用[TLinux 2.0](http://uwork.oa.com)。

* **基础镜像的选择**

> 基础镜像建议选择[架平提供](http://10.189.53.112/docker_images/)的，目前主要提供TLinux版本的基础镜像，因此容器化的系统需要`支持TLinux环境编译`。

* **镜像如何存放**

> Docker镜像默认存储目录在`/var/lib/docker/`，CentOS下修改/etc/sysconfig/docker配置文件通过`-g`将其挂载到`/data`分区下，避免占用root分区空间。

* **镜像升级问题**

> Docker升级1.10+[迁移镜像问题](https://github.com/docker/docker/wiki/Engine-v1.10.0-content-addressability-migration)，如何避免升级过程中服务不可用。

* **Docker daemon的缺陷**

> * Docker里没有init进程，会出现僵尸进程，导致daemon异常退出。
> * 垃圾文件不清理，导致磁盘inode被耗尽。
> * atomic issues。
> * daemon重启，container要重启。

* **Dockfile的问题**

> * 不会写，学习成本。
> * 三行命令 = FROM + ADD + CMD。

* **单/多租户**

![p1|right|350*0](./Fid_205-205_31696927_43351af4dda703f.png)

>  由于Docker本身安全隔离是基于Linux内核的Namespace/CGroups提供的隔离和资源调度机制。所有进程运行在同一个内核中，这对于多租户环境而言，会存在安全性不足的问题。例如：
> * 容器中/proc目录下meminfo/cpuinfo显示的是宿主机的信息，而非容器实际使用的信息。此问题导致公司网管系统TNM2无法使用。可查看[宿主机（172.27.208.238）](http://tnm2.oa.com/host/home/172.27.208.238)和[容器（172.27.208.135）](http://tnm2.oa.com/host/home/172.27.208.135)的资源使用对比。
> * 有些操作必须在宿主机上完成。
> * 特殊系统对内核的要求。
> 
> 多租户有解决方法么？


## 3.2 网络 

* **NAT**

> Docker提供了[四种网络通信](https://docs.docker.com/engine/userguide/networking/dockernetworks/)方式，默认使用`NAT模式`。但是鉴于安全的考虑（入侵追溯难度大），以及对网络性能方面的影响（相差10%左右），生产环境不建议使用此方式。

* **Bridge + VLAN**

> 根据公司网络环境，建议采用`NONE模式`（固定IP网络模式）。
> 
> * 使用桥接方式。将宿主机的网卡桥接到虚拟网桥中，再给Docker容器分配一个本地局域网IP。即，将Docker容器网络配置到本地主机网络的网段中，以实现节点之间、各节点与宿主机以及跨主机之间的通信（二层通信）。
> * 通过[网平申请内网IP](http://sniper.oa.com/iprms_v2/request/allot)，并为需要创建的每个容器绑定一个独立IP。
> * 在出现单机故障时，可以通过上层调度系统（比如TSM）实现IP漂移功能。

![p2|center|0*0](./QQ截图20160421201158.png)

*  **Overlay Network**

> Docker1.9推出了Overlay Netowrk，底层使用[VxLan](https://tools.ietf.org/html/rfc7348)（Virtual eXtensible Local Area Network），通过MAC in UDP的方式对以太网数据帧进行封装实现大二层网络通信，通过创建一个Overlay网络，实现跨宿主机的容器之间通信。

如何在大二层网络解决二层网络与生俱来的广播风暴和环路问题。
在传统的网络架构中，一般使用VLAN和STP协议来解决以上的问题，但是在大二层网络中VLAN面临着VLAN ID的数量有限的问题，STP协议则会造成大量的网络设备的浪费。利用传统的STP（生成树协议）和VLAN技术很显然已经无法解决这些问题了，为了解决这些必须解决的问题一批技术被发明创造出来。如：网络设备虚拟化技术（堆叠技术）、TRILL over L3、Overlay。

Overlay技术：Overlay技术其实就是用隧道或者协议封装的方式，将二层的数据帧进行封装形成逻辑上的隧道或者胶囊，Overlay技术中数据的传输技术不用修改现有的网络模式，只需要在两个端点加上封装和解封装的网络设备即可。
我们所说的逻辑上的隧道其实就是GRE技术，而逻辑上的胶囊就是Vxlan技术。这个两种技术是目前Overlay技术体系中比较常用的，GRE技术是点对点的隧道技术，这种技术实现起来比较简单，就相当于两台主机间的网线直连，在少量的需要进行Overlay技术通信的场景来说还是可以接受的，如CISCO就提出了基于GRE的同城双活数据中心解决方案，对于大量的相互之间通信的场景就会遇到传统网络没有交换机的尴尬。因此就提出了可以通过交换机进行数据交换的技术，这里面包括Vxlan、NVGRE等。
总的来讲，Vxlan采用的是Mac in UDP的方式，这是对数据帧进行封装，我形象的把这个称为“胶囊”。从源主机发出的数据包会在VTEP中进行VXLAN。帧头的封装（这个和TRILL技术中的nickname是一样的，只不过后面的转发机制是不一样的），在完成封装后会被自动的封装在UDP数据包中，然后就是使用在最外层使用当前网络的IP或者MAC作为外层的封装，之后就在当前的网络中进行正常的传输。这其实就是将需要传输的以太网数据帧当成被传输的数据，封装在UDP中进行数据的传输。这样在目的端的VTEP设备会类似于解析TCP/IP数据包一样，逐层的解封装，不过在解析到VXLAN header时，会根据类似VLAN的转发规则一样进行数据的转发，这样整个的数据转发都是在控制平面的，都是进行有规则的转发。
在Vxlan的模式下，所以的宿主机或者容器在逻辑上连接到同一个具体的交换机上的，在这个交换机上有VXLAN的划分，那就是在同一个VXLAN内IP是可以随主机或者容器飘移的。这也是可以实现带IP跨数据中心进行飘移的前提条件，这也是整个体系非常重要的一环。

![p3|center|450*0](./QQ截图20160426115806.png)


* **SR-IOV**

> 硬件虚拟化技术。
> * 一个网卡硬件虚拟出多个功能接口，每个功能接口可以作为一个网卡使用。硬件取代内核的虚拟网络设备，极大地提高网络性能，并且减少了物理机的CPU消耗。优点是，既保留了直接I/O 访问的高性能，又保证了虚拟化技术的优势，即设备的共享性；缺点是，需要特殊硬件的支持，对于给定的物理设备，其所能虚拟出来的虚拟设备数量是一定的。
> * Docker 1.9.0+版本支持网络插件方式，可以实现SR-IOV的网络插件。
> * 具体性能数据可参考[Docker容器网络性能（SR-IOV）测试](http://km.oa.com/group/23205/articles/show/235296?kmref=search&from_page=1&no=1&is_from_iso=1)。
> * 支持SR-IOV的设备结构。

![p4|center|450*0](./doc_image_20_w902_h668.jpg)

Docker中使用SR-IOV：
``` bash
激活VF
# echo "options igb max_vfs=7" >> /etc/modprobe.d/igb.conf
# reboot

设置VF的VLAN
# ip link set eth1 vf 0 vlan 12

将VF移到container network namespace
# ip link set eth4 netns $pid
# ip netns exec $pid ip link set dev eth4 name eth1
# ip netns exec $pid ip link set dev eth1 up

In container:
设置IP
#ip addr add 10.217.121.107/21 dev eth1

网关
#ip route add default via 10.217.120.1
```

* **HOST**

> 性能不受影响，不趟SDN浑水。

## 3.3 存储

* **存储驱动哪种合适**

> 目前[Docker Graphdrivers](https://github.com/docker/docker/tree/master/daemon/graphdriver)支持的类型有六种，但是每种都类型都存在[一些问题](https://blog.jessfraz.com/post/the-brutally-honest-guide-to-docker-graphdrivers/)。在启动Docker daemon时使用`docker -d -s some_driver_name`来指定使用的存储驱动。
> 
>  下面是`存储驱动的创建过程`：
>  * 依次检查环境变量DOCKER_DRIVER和DefaultDriver是否存在，若存在则根据驱动名称调用对应的初始化方法创建一个对应的Driver对象。
>  * 若第一步没查到，则Graphdriver会从驱动的优先列表中查找一个可用的驱动。
>  * 若仍没找到可用的，则Graphdriver会查找所有注册过的驱动，找到第一个注册过可用的并返回。

``` go
	// driver_linux.go:51
	// Slice of drivers that should be used in an order
	priority = []string{
		"aufs",
		"btrfs",
		"zfs",
		"devicemapper",
		"overlay",
		"vfs",
	}
```

* **AUFS**

> Docker最先使用aufs，但在公司内并不完美。根据最新[tkernel2 changelogs](http://km.oa.com/group/799/articles/show/242230)TLinux内核更新和BUG日志，aufs目前还存在很多问题，因此，在生产环境暂时不建议使用。
> * 容器中AUFS的挂载点是：`/var/lib/docker/aufs/mnt/$CONTAINER_ID/`
> * AUFS的分支(只读和读写)位置在：`/var/lib/docker/aufs/diff/$CONTAINER_OR_IMAGE_ID/`

``` roboconf
[root@TENCENT64 /data/home/gerryyang/mini_docker/bin]# docker info
Containers: 13
Images: 25
Storage Driver: aufs
 Root Dir: /data/home/gerryyang/root_docker/aufs
 Backing Filesystem: extfs
 Dirs: 51
Execution Driver: native-0.2
Kernel Version: 3.10.83-1-tlinux2-0021.tl2
Operating System: Tencent tlinux 2.0 (Final)
CPUs: 4
Total Memory: 7.665 GiB
Name: TENCENT64.site
ID: J2W3:5VDW:KED5:4TSX:C2KL:J3NU:4SU7:MN7Y:KDTL:ST23:FHG6:FDSF
```

* **DeviceMapper**

> DM相对比较稳定，但是也存在`一些问题`。
> * 默认一个容器最大存储空间`不超过10G`，数平基于1.9版本修改了Docker，通过docker run启动参数扩展指定的存储空间。Docker支持要到[正式版本1.12发布](https://github.com/docker/docker/pull/19367)。
> * DM默认使用`loop-lvm`，虽然配置简单，但是性能不好。建议使用`direct-lvm`，在高负载环境下会有更好的性能，具体可以参考[Comprehensive Overview of Storage Scalability in Docker](https://developerblog.redhat.com/2014/09/30/overview-storage-scalability-docker/)。官方也是不建议生产环境系统使用默认的`LVM thin pool/sparse file`。
> * docker -d --storage-opt dm.datadev=/dev/sdb1 --storage-opt dm.metadatadev=/dev/sdc1。

``` roboconf
[root@TENCENT64 /var/lib/docker]# docker info
Containers: 0
Images: 0
Storage Driver: devicemapper
 Pool Name: docker-8:4-21661799-pool
 Pool Blocksize: 65.54 kB
 Backing Filesystem: extfs
 Data file: /dev/loop0
 Metadata file: /dev/loop1
 Data Space Used: 307.2 MB
 Data Space Total: 107.4 GB
 Data Space Available: 107.1 GB
 Metadata Space Used: 733.2 kB
 Metadata Space Total: 2.147 GB
 Metadata Space Available: 2.147 GB
 Udev Sync Supported: true
 Data loop file: /data/home/gerryyang/root_docker/devicemapper/devicemapper/data
 Metadata loop file: /data/home/gerryyang/root_docker/devicemapper/devicemapper/metadata
 Library Version: 1.02.84-RHEL7 (2014-03-26)
Execution Driver: native-0.2
Kernel Version: 3.10.83-1-tlinux2-0021.tl2
Operating System: Tencent tlinux 2.0 (Final)
CPUs: 4
Total Memory: 7.665 GiB
Name: TENCENT64.site
ID: J2W3:5VDW:KED5:4TSX:C2KL:J3NU:4SU7:MN7Y:KDTL:ST23:FHG6:FDSF
[root@TENCENT64 /data/home/gerryyang/root_docker/devicemapper/devicemapper]# ls -lsh
total 294M
293M -rw------- 1 root root 100G Apr 22 16:18 data
752K -rw------- 1 root root 2.0G Apr 22 16:18 metadata

```

* **Docker Volume**

> 通过Docker Volume，把宿主机上的目录挂载到容器内部，可以实现持久存储。但是存在一些问题：
> * 容器迁移，数据无法迁移。
> * 多个容器之间不能共享数据。

* **Docker + Ceph**

> [IEG的TDocker](http://km.oa.com/group/1171/articles/show/233723?kmref=search&from_page=1&no=1&is_from_iso=1)通过Ceph的RBD递归快照（thin-provisioning snapshot），支持Docker rootfs数据的共享存储，但是，目前还不支持元数据的共享存储。若实现后，就可以实现`容器故障无数据迁移`。

* **Registry**

> 镜像仓库需要考虑的问题。
> * 安全鉴权。例如，`docker pull 172.27.198.179:5000/xxx`
> * 负载容灾。

## 3.4 监控

* **docker stats**

> 在宿主机上通过[docker stats](https://docs.docker.com/engine/reference/commandline/stats/)可以查看每个容器的`CPU利用率`、`内存的使用量`、`可用内存总量`以及`网络发送和接收数据总量`。可以使用Docker Remote API获取[更详细的stats信息](https://gist.github.com/usmanismail/0c4922ffec4a0220d385)。
> **优点**：易于使用和部署
> **缺点**：没有监测非Docker资源的能力

``` roboconf
CONTAINER           CPU %               MEM USAGE/LIMIT     MEM %               NET I/O
portal_tdf          0.00%               552 KiB/7.665 GiB   0.01%               0 B/0 B
```

* **cAdvisor + Prometheus **

> Google用来分析运行中Docker容器的资源占用以及性能特性的工具。通过[cAdvisor](https://github.com/google/cadvisor)实时监控容器的资源使用情况，并将数据上报监控平台。IEG使用cAdvisor对CPU/MEM/NETWORK的[采集算法](http://km.oa.com/group/1171/articles/show/214186)。
> [Prometheus](http://prometheus.io/)实现告警和过滤。
> **优点**：提供容器监控可视化图表
> **缺点**：没有监测非Docker资源的能力，只能监控一台宿主机，占用一定资源消耗，上报TNM2网管系统

* **FUSE**

> 通过[FUSE filesystem](https://github.com/lxc/lxcfs)（Filesystem in Userspace）解决容器内资源使用[显示不正确的问题](https://github.com/opencontainers/runc/issues/400)，从而可以无缝对接TNM2网管系统。
> 原理：通过FUSE实现用户态的fs，使用cgroups的数据统计container的实际资源使用，然后生成仿真的meminfo,cpuinfo,stats,diskstats文件，bind mount到container中。
> * 需要安装`fuse`。例如，sudo apt-get install fuse
> * 需要Docker升级到1.11+，否则老版本需要打patch。
> 
> **优点**：与老的监控系统兼容，对业务使用者透明
> **缺点**：对Docker版本有要求或者修改Docker

``` roboconf
container_id=`docker create -v /tmp/cgroupfs/meminfo:/proc/meminfo -m=15m ubuntu sleep 213133`
root@gerryyang:/tmp# docker start $container_id
e1379cf7e3ba4611201699e77cac28417bf98260b9e0ca07b0f9579b575a6e3c
root@gerryyang:/tmp# cat /tmp/cgroupfs/meminfo
MemTotal:       15360 kB
MemFree:        15144 kB
MemAvailable:   15144 kB
Buffers:        0 kB
Cached:         124 kB
SwapCached:     0 kB
root@gerryyang:/tmp# docker exec -it $container_id bash
root@e1379cf7e3ba:/# free -m
             total       used       free     shared    buffers     cached
Mem:            15          0         14          0          0          0
-/+ buffers/cache:          0         14
Swap:            0          0          0
```

## 3.5 安全

Docker团队已经明确了一些[安全问题](https://blog.docker.com/2013/08/containers-docker-how-secure-are-they/)，正在着手解决。借助[docker-bench-security](https://github.com/docker/docker-bench-security)工具可以检查Docker环境下是否存在的一些安全隐患。

``` roboconf
docker run -it --net host --pid host --cap-add audit_control \
    -v /var/lib:/var/lib \
    -v /var/run/docker.sock:/var/run/docker.sock \
    -v /usr/lib/systemd:/usr/lib/systemd \
    -v /etc:/etc --label docker_bench_security \
    docker/docker-bench-security
# ------------------------------------------------------------------------------
# Docker Bench for Security v1.0.0
#
# Docker, Inc. (c) 2015-
#
# Checks for dozens of common best-practices around deploying Docker containers in production.
# Inspired by the CIS Docker 1.11 Benchmark:
# https://benchmarks.cisecurity.org/downloads/show-single/index.cfm?file=docker16.110
# ------------------------------------------------------------------------------

Initializing Thu May  5 02:12:34 UTC 2016


[INFO] 1 - Host Configuration
[WARN] 1.1  - Create a separate partition for containers
......
```

## 3.6 编排

容器多了，如何管理？

* **Swarm**

> Docker社区原生

* **Kubernetes**

> Borg -> Omega -> K8S
> Docker集群管理工具[K8S部署和测试](http://blog.csdn.net/delphiwcdj/article/details/50411711)。

* **MM**

> Mesos + Marathon

* **Deis/Flynn**

资源调度管理需要`解决的问题`：对机器资源CPU、内存、网络、磁盘空间的隔离进行弹性管理，最大化地利用集群的所有资源。业务以后再也不用关心机器申请、程序的交付和部署、系统扩缩容等。业务只需要构建好镜像，在Web页面填上需要申请的资源、副本数量，其它的事情全部交给调度平台搞定。

## 3.7 实践

通过将Portal容器化，测试系统的基本功能是否符合预期。具体参考[Portal TDF Docker容器化实践](http://km.oa.com/group/17078/articles/show/256520)。

# 4 其他

**公司内部**：
* [数平的Docker on Gaia](http://km.oa.com/group/17516/articles/show/251253)，底层资源管理和调度系统，提升公司的整体服务器利用率。基于Docker社区版本的[自研定制开发](http://pub.code.oa.com/project/releases?projectName=docker)，将自研的新特性尽可能提交社区。
* [基架的CAE over Docker](http://km.oa.com/group/15624/articles/show/212924)，腾讯内部云的业务托管平台，提升设备利用率，秒级扩容速度，配置环境标准化，并使用[自研的调度系统](http://km.oa.com/group/15624/articles/show/237280)。将互娱官网原有的1500台TVM虚拟机替换为docker虚拟化容器，[单机服务能力提升15%](http://km.oa.com/group/1200/articles/show/249541)。
* [架平的CI自动化测试环境Docker化](http://km.oa.com/group/11879/articles/show/261595)，解决设备环境冲突问题，将问题环境生产镜像便于开发定位。
* [IEG的TDocker云平台](http://km.oa.com/group/23205/articles/show/210894) ，为游戏所用，[Tencent Docker for Online Service](http://tdos.oa.com/)。
* [MIG的Sumeru项目](http://km.oa.com/group/sumeru/articles/show/251266)支撑业务在Docker平台上运行，对业务接入要求是业务是否能随意放置到任何IP端口上运行，否则需要业务进行改造。
* [OMG的视频索引docker容器部署规范和性能测试报告](http://km.oa.com/group/26513/articles/show/261740?kmref=guess_post)。

**公司外部**：
* [阿里云容器服务](https://yq.aliyun.com/teams/11)，容器服务基于阿里云ECS。
* 平安科技，[修改MM框架](http://item.jd.com/11841636.html)，mesos_dns，mesos_lb不适用，与内部DNS接口对接实现LB，Mesos+Docker+Marathon+ELK+DockerUI。
* [DaoCloud](https://www.daocloud.io/)，主打Docker原生工具。
* [数人云](http://www.dataman-inc.com/)，基于Mesos 和 Docker 技术打造下一代DCOS，将应用弹性做到极致，提供容器化整体解决方案。


# 5 结论
我们理解的系统容器化就是：`代码化的生产环境，把人脑记忆的东西表现在机器里`。
* **包起来**
* **挪得动**
* **可复制**

进一步：

* **代码化**
* **工具化**
* **自动化**

| 应用类型      |    例子 | 收益  | 成本
| :-------- | :--------:| :--: |
| 无状态  | CGI/Portal |  快速部署、弹性伸缩、自动容错、高利用率   | 制作自己的image，容器化配置分离
| 有状态     |   DB |  快速部署  | 更高的失败率

```flow
st=>start: Start
e=>end
op=>operation: My Operation
cond=>condition: Yes or No?

st->op->cond
cond(yes)->e
cond(no)->op
```

[docker-ecosystem](https://www.mindmeister.com/zh/389671722/docker-ecosystem)

[项目构建](http://tapd.oa.com/TDOS/markdown_wikis/#1010124601005194699)
[Tdocker容器资源配置规范](http://km.oa.com/group/1171/articles/show/226475?kmref=search&from_page=1&no=3&is_from_iso=1)

帝国的毁灭 

 
 





