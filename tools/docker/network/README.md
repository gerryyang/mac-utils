
## 网络方案
使用桥接模式，将Docker容器网络配置到本地主机网络的网段中。以实现节点之间、各节点与宿主机以及跨主机之间的通信。

## 原理
将宿主机的网卡桥接到虚拟网桥中，再给Docker容器分配一个本地局域网IP。

## 网络拓扑
在宿主机通过执行`init_network.sh`脚本初始化容器的网络环境

![pic](https://github.com/gerryyang/mac-utils/blob/master/tools/docker/network/1.png)


---



