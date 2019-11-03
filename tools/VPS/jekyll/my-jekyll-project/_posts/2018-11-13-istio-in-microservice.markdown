---
layout: post
title:  "Istio一个开源的Service Mesh平台 - 微服务网格技术"
date:   2018-11-13 08:30:00 +0800
categories: microservice
---

* Do not remove this line (it will not be displayed)
{:toc}

# 背景

随着容器技术的快速发展，越来越多的企业从一体化应用转型迁移到了微服务架构。微服务在带来良好的设计和架构理念的同时，也引入了复杂的`微服务治理`等难题。`安全`，`流量控制`，`监控`，`日志`在微服务架构中越来越难实现，更复杂的需求诸如`A/B 测试`，`蓝绿发布`，`访问控制`，`策略管理`更成为了微服务系统中**负担**。为了解决这些问题，**服务网格技术**应运而生，而`Istio`做为**服务网格技术的先驱者**也受到了越来越多的关注。

`Istio`， **一个用来连接，保护，管理和监测微服务的开源平台**，以其先进的理念、强大的企业支持(IBM, Google & Lyft)、活跃的社区越来越多的受到了业界关注。`Istio` **提供一种简单的方式来为已部署的微服务建立网络，该网络具有负载均衡、服务间认证、监控等功能，而且不需要对微服务的代码做任何改动。**

如何在你的微服务网络中引入 Istio 并用它来解决微服务治理中的诸多难题呢？

![istio_arch](/assets/images/201811/istio_arch.jpg)

# 课程安排

# 1. [11 月 1 日-Istio 初探] 

[11 月 1 日-Istio 初探]: https://github.com/dWChina/ibm-opentech-ma/blob/master/istio/Istio-01-intro.pdf

LIN SUN, Senior Technical Staff Member, IBM

Problem: 

modern distributed architecture -> container based services
                                   deployed into dynamic environments
                                   composed via the network

IT's shift to a modern distributed architecture has left enterprises unable to **connect, observe or secure or control** their services in a consistent way.

Service Mesh:

A service mesh provides a **transparent and language-independent** network for connecting, observing, securing and controlling the connectivity between services. 

Istio:

An **open service mesh platform** to connect， observe, secure, and control microservices.

* Connect: Traffic Control, Discovery, Load Balancing, Resiliency
* Observe: Metrics, Logging, Tracing
* Secure: Encryption(TLS), Authentication, and Authorization of service-to-service communication
* Control: Policy Enforcement

How does it work ?


``` 
                call
       A ------------------> B

                |
                |
                v

       A ------------------> B

                call
     Envoy  ------------>   Envoy

                |
                |
                v

       A                     B
                call
     Envoy  ------------>   Envoy
        
       ^                      ^
       |                      |
       |                      |
     Envoy --------------------
     Pilot

                |
                |
                v

       A       call           B
     Envoy  ------------>   Envoy
        
       |                     |
       |------|   |----------|
              |   |          
              v   v

     Envoy    Envoy
     Pilot   Telemetry

                |
                |
                v

       A       call           B
     Envoy  ------------>   Envoy
        
       ^                       ^
       |                       |
       |--------------------|  |
                            |  |          
              
     Envoy    Envoy         Envoy
     Pilot   Telemetry      Citadel

                |
                |
                v

       A               call              B
     Envoy  ------------------------>   Envoy
        
       ^                                   ^
       |                                   |
       |--------------------------------|  |
                                        |  |          
              
     Envoy    Envoy         Envoy       Envoy
     Pilot   Telemetry      Citadel     Policy
``` 

1. Deploy a proxy (`Envoy`) beside your application ("sidecar deployment")
2. Deploy `Pilot` to configure the sidecars
3. Deploy Telemetry to get telemetry
4. Deploy Citadel to assign identities and enable secure communication
5. Deploy Policy to enforce policies

A calls B
A's sidecar intercepts the call
A's sidecar selects a destination
B's sidecar performs policy checks with local cache
B's sidecar performs policy checks with Policy component
B's sidecar forwards the call to B
B's response is sent back
Both sidecars report telemetry data



# 2. [11 月 8 日-Istio 上手]

Iris Ding, 此课程设计者, dingsj@cn.ibm.com

[11 月 8 日-Istio 上手]: https://github.com/dWChina/ibm-opentech-ma/blob/master/istio/istio-02-arch.pdf

Microservice Architecture:

An engineering approach focused on **decomposing** an application into **single function** modules with **well defined interfaces** which are **independently** deployed and operated by a **small team** who owns the **entire lifecycle** of the service.

Service Mesh:

A network for services
* Visibility
* Resiliency & Efficiency
* Traffic Control
* Security
* Policy Enforcement

![istio_arch_2](/assets/images/201811/istio_arch_2.jpg)

**Istio Installation:**

* Istio-demo.yaml - Dev/Test  (开发测试时使用，缺点是需要修改yaml较为麻烦)
* Helm Render   (自定义安装，render出一个yaml文件，然后用kubectl安装)
* Helm
* Minimum Istio (最轻量的一个安装方式)
* Notice: Apply crds first

Example:

https://istio.io/docs/examples/bookinfo

![bookinfo_without_istio](/assets/images/201811/bookinfo_without_istio.jpg)

This application is polyglot, i.e., the microservices are written in different languages. It’s worth noting that these services have no dependencies on Istio, but make an interesting service mesh example, particularly because of the multitude of services, languages and versions for the reviews service.

To run the sample with Istio requires no changes to the application itself. Instead, we simply need to configure and run the services in an Istio-enabled environment, with Envoy sidecars injected along side each service. The needed commands and configuration vary depending on the runtime environment although in all cases the resulting deployment will look like this:

![bookinfo_with_istio](/assets/images/201811/bookinfo_with_istio.jpg)

All of the microservices will be packaged with an Envoy sidecar that intercepts incoming and outgoing calls for the services, providing the hooks needed to externally control, via the Istio control plane, routing, telemetry collection, and policy enforcement for the application as a whole.

Envoy - sidecar proxy in Istio:

* A C++ based L4/L7 proxy (支持4层和7层转发)
* Battle-tested (经过大规模验证)
* Low memory footprint (非常高效)

![envoy](/assets/images/201811/envoy.jpg)

[Envoy源码]

[Envoy源码]: https://github.com/istio/proxy


## Istio setup

[How to deploy Istio in various environments (e.g., Kubernetes, Consul)]

[How to deploy Istio in various environments (e.g., Kubernetes, Consul)]: https://istio.io/docs/setup/

### download istio

https://istio.io/docs/setup/kubernetes/download-release/

```
root@ubuntu-s-2vcpu-4gb-sfo2-01:~# istioctl 

Istio configuration command line utility.

Create, list, modify, and delete configuration resources in the Istio
system.

Available routing and traffic management configuration types:

        [virtualservice gateway destinationrule serviceentry httpapispec httpapispecbinding quotaspec quotaspecbinding servicerole servicerolebinding policy]

See https://istio.io/docs/reference/ for an overview of Istio routing.

Usage:
  istioctl [command]

Available Commands:
  authn          Interact with Istio authentication policies
  context-create Create a kubeconfig file suitable for use with istioctl in a non kubernetes environment
  create         Create policies and rules
  delete         Delete policies or rules
  deregister     De-registers a service instance
  experimental   Experimental commands that may be modified or deprecated
  gen-deploy     Generates the configuration for Istio's control plane.
  get            Retrieve policies and rules
  help           Help about any command
  kube-inject    Inject Envoy sidecar into Kubernetes pod resources
  proxy-config   Retrieve information about proxy configuration from Envoy [kube only]
  proxy-status   Retrieves the synchronization status of each Envoy in the mesh [kube only]
  register       Registers a service instance (e.g. VM) joining the mesh
  replace        Replace existing policies and rules
  version        Prints out build version information

Flags:
      --context string                The name of the kubeconfig context to use
  -h, --help                          help for istioctl
  -i, --istioNamespace string         Istio system namespace (default "istio-system")
  -c, --kubeconfig string             Kubernetes configuration file
      --log_as_json                   Whether to format output as JSON or in plain console-friendly format
      --log_caller string             Comma-separated list of scopes for which to include caller information, scopes can be any of [ads, default, model, rbac]
      --log_output_level string       Comma-separated minimum per-scope logging level of messages to output, in the form of <scope>:<level>,<scope>:<level>,... where scope can be one of [ads, default, model, rbac] and level can be one of [debug, info, warn, error, none] (default "default:info")
      --log_rotate string             The path for the optional rotating log file
      --log_rotate_max_age int        The maximum age in days of a log file beyond which the file is rotated (0 indicates no limit) (default 30)
      --log_rotate_max_backups int    The maximum number of log file backups to keep before older files are deleted (0 indicates no limit) (default 1000)
      --log_rotate_max_size int       The maximum size in megabytes of a log file beyond which the file is rotated (default 104857600)
      --log_stacktrace_level string   Comma-separated minimum per-scope logging level at which stack traces are captured, in the form of <scope>:<level>,<scope:level>,... where scope can be one of [ads, default, model, rbac] and level can be one of [debug, info, warn, error, none] (default "default:none")
      --log_target stringArray        The set of paths where to output the log. This can be any path as well as the special values stdout and stderr (default [stdout])
  -n, --namespace string              Config namespace
  -p, --platform string               Istio host platform (default "kube")

Use "istioctl [command] --help" for more information about a command.
```

### Quick Start with Kubernetes

https://istio.io/docs/setup/kubernetes/quick-start/
https://kubernetes.io/docs/tasks/tools/install-minikube/

#### install kubectl

https://kubernetes.io/docs/tasks/tools/install-kubectl/

Install kubectl binary using native package management:

``` bash
#!/bin/bashls
# install-kubectl.sh
# ubuntu

sudo apt-get update && sudo apt-get install -y apt-transport-https
curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -
echo "deb http://apt.kubernetes.io/ kubernetes-xenial main" | sudo tee -a /etc/apt/sources.list.d/kubernetes.list
sudo apt-get update
sudo apt-get install -y kubectl
```

安装后：

```
# minikube start
Starting local Kubernetes v1.10.0 cluster...
Starting VM...
Getting VM IP address...
Moving files into cluster...
Setting up certs...
Connecting to cluster...
Setting up kubeconfig...
Starting cluster components...
Kubectl is now configured to use the cluster.
Loading cached images from config file.

# kubectl version
Client Version: version.Info{Major:"1", Minor:"12", GitVersion:"v1.12.2", GitCommit:"17c77c7898218073f14c8d573582e8d2313dc740", GitTreeState:"clean", BuildDate:"2018-10-24T06:54:59Z", GoVersion:"go1.10.4", Compiler:"gc", Platform:"linux/amd64"}
Server Version: version.Info{Major:"1", Minor:"10", GitVersion:"v1.10.0", GitCommit:"fc32d2f3698e36b93322a3465f63a14e9f0eaead", GitTreeState:"clean", BuildDate:"2018-03-26T16:44:10Z", GoVersion:"go1.9.3", Compiler:"gc", Platform:"linux/amd64"}
```

#### install Hypervisor

https://kubernetes.io/docs/tasks/tools/install-minikube/#install-a-hypervisor

```
sudo apt-get update
# 5.2
sudo apt-get install virtualbox
# or
sudo dpkg -i packagename.deb
# check version
vboxmanage --version
5.2.18_Ubuntur123745
```

#### install minikube

https://github.com/kubernetes/minikube/releases

```
curl -Lo minikube https://storage.googleapis.com/minikube/releases/v0.30.0/minikube-linux-amd64 && chmod +x minikube && sudo cp minikube /usr/local/bin/ && rm minikube
```

https://istio.io/docs/setup/kubernetes/platform-setup/minikube/

启动minkube的方法：

``` bash
#!/bin/bash
# start_minikube.sh

export MINIKUBE_WANTUPDATENOTIFICATION=false
export MINIKUBE_WANTREPORTERRORPROMPT=false
export MINIKUBE_HOME=$HOME
export CHANGE_MINIKUBE_NONE_USER=true
mkdir $HOME/.kube || true
touch $HOME/.kube/config

export KUBECONFIG=$HOME/.kube/config
sudo -E minikube start --vm-driver=none

# 使用kubectl检查api server, 确保集群创建成功
for i in {1..150}; do # timeout for 5 minutes
	kubectl get po &> /dev/null
	if [ &? -ne 1 ]; then
		break
	fi
	sleep 2
done
```


```
# minikube start --vm-driver=virtualbox
Starting local Kubernetes v1.10.0 cluster...
Starting VM...
Getting VM IP address...
Moving files into cluster...
Downloading kubeadm v1.10.0
Downloading kubelet v1.10.0
Finished Downloading kubelet v1.10.0
Finished Downloading kubeadm v1.10.0
Setting up certs...
Connecting to cluster...
Setting up kubeconfig...
Starting cluster components...
Kubectl is now configured to use the cluster.
Loading cached images from config file.

# ps ux|grep mini
root      3075  166 31.4 3338904 1270108 ?     Sl   08:24  24:56 /usr/lib/virtualbox/VBoxHeadless --comment minikube --startvm fd44ccb2-d4ca-4923-a09e-e9dce20f02ca --vrde config
```


#### Install Istio’s Custom Resource Definitions via kubectl apply

```
kubectl apply -f install/kubernetes/helm/istio/templates/crds.yaml
```

#### Install Istio without mutual TLS authentication between sidecars

```
kubectl apply -f install/kubernetes/istio-demo.yaml
```

#### Verifying the installation

查看Istio启动pods的情况，比较慢。

![istio_pods](/assets/images/201811/istio_pods.png)

![istio_svc](/assets/images/201811/istio_svc.png)





# 3. 11 月 15 日-Istio 的安全管理

# 4. 11 月 22 日-Envoy

# 5. 11 月 29 日-使用 Istio 来监控和可视化微服务

# 6. 12 月 6 日-Istio mixer – 基本概念，策略、遥测与扩展

# 7. 12 月 13 日-Istio 跨云管理方案解析

# 8. 12 月 20 日-Istio 使用案例：Serverless 平台knative



# Refer

1. [istio官网]
2. [Istio系列-IBM 微讲堂]
3. [Istio 流量管理的基本概念详解-以 Bookinfo 为例详解其如何作用于 Kubernetes 中的 Pod]
4. [kubernetes-vagrant-centos-cluster]
5. [什么是 istio]


[istio官网]: https://istio.io/

[Istio系列-IBM 微讲堂]: https://github.com/dWChina/ibm-opentech-ma#istio%E7%B3%BB%E5%88%97

[Istio 流量管理的基本概念详解-以 Bookinfo 为例详解其如何作用于 Kubernetes 中的 Pod]: https://jimmysong.io/posts/istio-traffic-management-basic-concepts/?from=message&isappinstalled=0

[kubernetes-vagrant-centos-cluster]: https://github.com/rootsongjc/kubernetes-vagrant-centos-cluster

[什么是 istio]: https://cizixs.com/2018/08/26/what-is-istio/

[jimmysong-istio-handbook]: https://jimmysong.io/istio-handbook/setup/quick-start.html


  

	
	