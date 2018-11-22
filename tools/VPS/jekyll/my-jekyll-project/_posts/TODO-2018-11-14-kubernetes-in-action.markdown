---
layout: post
title:  "Kubernetes in Action"
date:   2018-11-14 20:22:00 +0800
categories: https
---

* Do not remove this line (it will not be displayed)
{:toc}

[k8s官网]的介绍：

> Kubernetes is an open-source system for automating deployment, scaling, and management of containerized applications. It groups containers that make up an application into logical units for easy management and discovery. Kubernetes builds upon [15 years of experience of running production workloads at Google], combined with best-of-breed ideas and practices from the community.

![the_old_and_new_way](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201811/the_old_and_new_way.jpg)


k8s的好处：

* Planet Scale
	+ Designed on the same principles that allows Google to run billions of containers a week, Kubernetes can scale without increasing your ops team.
* Never Outgrow
	+ Whether testing locally or running a global enterprise, Kubernetes flexibility grows with you to deliver your applications consistently and easily no matter how complex your need is.
* Run Anywhere
	+ Kubernetes is open source giving you the freedom to take advantage of on-premises, hybrid, or public cloud infrastructure, letting you effortlessly move workloads to where it matters to you. 


# [k8s setup]

[k8s setup]: https://kubernetes.io/docs/setup/

方案1：minikube

https://kubernetes.io/docs/setup/minikube/

方案2：microk8s

https://microk8s.io/

方案3：Kubernetes on Ubuntu

https://kubernetes.io/docs/getting-started-guides/ubuntu/

方案4：kubeadm

https://www.digitalocean.com/community/tutorials/how-to-create-a-kubernetes-1-11-cluster-using-kubeadm-on-ubuntu-18-04

方案5：Kubernetes + Virtualbox + Vagrant

[kubernetes-vagrant-centos-cluster]
[jimmysong-istio-handbook]

[kubernetes-vagrant-centos-cluster]: https://github.com/rootsongjc/kubernetes-vagrant-centos-cluster

[jimmysong-istio-handbook]: https://jimmysong.io/istio-handbook/setup/quick-start.html


# 方案5安装记录

Setting up a distributed Kubernetes cluster along with Istio service mesh locally with Vagrant and VirtualBox for PoC or Demo use cases, see [kubernetes-vagrant-centos-cluster].

安装后：

```
root@ubuntu-s-8vcpu-32gb-sfo2-01:~# kubectl get nodes
NAME    STATUS   ROLES    AGE   VERSION
node1   Ready    <none>   1h    v1.11.0
node2   Ready    <none>   1h    v1.11.0
node3   Ready    <none>   1h    v1.11.0

root@ubuntu-s-8vcpu-32gb-sfo2-01:~# kubectl get namespaces
NAME          STATUS   AGE
default       Active   1h
kube-public   Active   1h
kube-system   Active   1h

root@ubuntu-s-8vcpu-32gb-sfo2-01:~# kubectl cluster-info
Kubernetes master is running at https://172.17.8.101:6443
Heapster is running at https://172.17.8.101:6443/api/v1/namespaces/kube-system/services/heapster/proxy
CoreDNS is running at https://172.17.8.101:6443/api/v1/namespaces/kube-system/services/kube-dns:dns/proxy
Grafana is running at https://172.17.8.101:6443/api/v1/namespaces/kube-system/services/monitoring-grafana/proxy
InfluxDB is running at https://172.17.8.101:6443/api/v1/namespaces/kube-system/services/monitoring-influxdb:http/proxy

To further debug and diagnose cluster problems, use 'kubectl cluster-info dump'.


root@ubuntu-s-8vcpu-32gb-sfo2-01:~# kubectl get pods -n kube-system
NAME                                              READY   STATUS    RESTARTS   AGE
coredns-549f985987-kw5rx                          1/1     Running   0          1h
coredns-549f985987-vqgks                          1/1     Running   0          1h
heapster-v1.5.0-76c9b966c-4dh9p                   4/4     Running   0          50m
kubernetes-dashboard-574589d477-vbs6s             1/1     Running   0          1h
monitoring-influxdb-grafana-v4-5bbb9b766d-8x8bz   2/2     Running   0          52m
traefik-ingress-controller-n2gt6                  1/1     Running   0          1h
```


---


[15 years of experience of running production workloads at Google]: https://queue.acm.org/detail.cfm?id=2898444

[k8s官网]: https://kubernetes.io/

[k8s官方下载地址]: https://kubernetes.io/docs/imported/release/notes/
  
[How to install Vagrant on Ubuntu 18.04]: https://linuxize.com/post/how-to-install-vagrant-on-ubuntu-18-04/

[kubernetes github]: https://github.com/kubernetes/kubernetes