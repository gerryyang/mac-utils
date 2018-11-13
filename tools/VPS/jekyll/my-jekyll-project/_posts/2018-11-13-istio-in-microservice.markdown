---
layout: post
title:  "Istio一个开源的Service Mesh平台 - 微服务网格技术"
date:   2018-11-13 08:30:00 +0800
categories: 微服务
---

* Do not remove this line (it will not be displayed)
{:toc}

# 背景

随着容器技术的快速发展，越来越多的企业从一体化应用转型迁移到了微服务架构。微服务在带来良好的设计和架构理念的同时，也引入了复杂的`微服务治理`等难题。`安全`，`流量控制`，`监控`，`日志`在微服务架构中越来越难实现，更复杂的需求诸如`A/B 测试`，`蓝绿发布`，`访问控制`，`策略管理`更成为了微服务系统中**负担**。为了解决这些问题，**服务网格技术**应运而生，而`Istio`做为**服务网格技术的先驱者**也受到了越来越多的关注。

`Istio`， **一个用来连接，保护，管理和监测微服务的开源平台**，以其先进的理念、强大的企业支持(IBM, Google & Lyft)、活跃的社区越来越多的受到了业界关注。`Istio` **提供一种简单的方式来为已部署的微服务建立网络，该网络具有负载均衡、服务间认证、监控等功能，而且不需要对微服务的代码做任何改动。**

如何在你的微服务网络中引入 Istio 并用它来解决微服务治理中的诸多难题呢？

![istio_arch](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201811/istio_arch.jpg)

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

[11 月 8 日-Istio 上手]: https://github.com/dWChina/ibm-opentech-ma/blob/master/istio/istio-02-arch.pdf

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

[istio官网]: https://istio.io/

[Istio系列-IBM 微讲堂]: https://github.com/dWChina/ibm-opentech-ma#istio%E7%B3%BB%E5%88%97

[Istio 流量管理的基本概念详解-以 Bookinfo 为例详解其如何作用于 Kubernetes 中的 Pod]: https://jimmysong.io/posts/istio-traffic-management-basic-concepts/?from=message&isappinstalled=0








  

	
	