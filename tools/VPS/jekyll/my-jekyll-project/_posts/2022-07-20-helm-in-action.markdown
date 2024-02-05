---
layout: post
title:  "Helm in Action"
date:   2022-07-20 12:30:00 +0800
categories: 云原生
---

* Do not remove this line (it will not be displayed)
{:toc}


# 什么是 Helm

`Helm`是`Kubernetes`的一个**包管理工具**，用来简化`Kubernetes`应用的部署和管理。可以把`Helm`比作`CentOS`的`yum`工具。 使用`Helm`可以完成以下事情：

* 管理`Kubernetes manifest files`
* 管理`Helm`安装包`Charts`
* 基于`Chart`的`Kubernetes`应用分发



# 基本概念

* Chart: 是 Helm 管理的安装包，里面包含需要部署的安装包资源。可以把 Chart 比作 CentOS yum 使用的 rpm 文件。每个Chart包含下面两部分：
  * 包的基本描述文件` Chart.yaml`
  * 放在 templates 目录中的一个或多个 Kubernetes manifest 文件模板
* Release：是 chart 的部署实例，一个 chart 在一个 Kubernetes 集群上可以有多个 release，即这个 chart 可以被安装多次
* Repository：chart 的仓库，用于发布和存储 chart


使用 `helm create mychart` 创建一个名为 `mychart` 的示例，再使用 `tree mychart` 命令看一下 chart 的目录结构。


```
mychart
├── Chart.yaml
├── charts                      # 该目录保存其他依赖的 chart（子 chart）
├── templates                   # chart 配置模板，用于渲染最终的 Kubernetes YAML 文件
│   ├── NOTES.txt               # 用户运行 helm install 时候的提示信息
│   ├── _helpers.tpl            # 用于创建模板时的帮助类
│   ├── deployment.yaml         # Kubernetes deployment 配置
│   ├── ingress.yaml            # Kubernetes ingress 配置
│   ├── service.yaml            # Kubernetes service 配置
│   ├── serviceaccount.yaml     # Kubernetes serviceaccount 配置
│   └── tests
│       └── test-connection.yaml
└── values.yaml                 # 定义 chart 模板中的自定义配置的默认值，可以在执行 helm install 或 helm update 的时候覆盖
```

* Chart.yaml：用于描述这个 Chart 的基本信息，包括名字、描述信息以及版本等
* values.yaml ：用于存储 templates 目录中模板文件中用到变量的值。
* templates： 目录里面存放所有 yaml 模板文件。
* charts：目录里存放这个 chart 依赖的所有子chart。
* NOTES.txt ：用于介绍Chart帮助信息， helm install 部署后展示给用户。例如：如何使用这个 Chart、列出缺省的设置等。
* _helpers.tpl：放置模板助手的地方，可以在整个 chart 中重复使用




# Refer

* https://github.com/helm/helm
* https://helm.sh/zh/docs/chart_template_guide/getting_started/
* [是时候使用Helm了：Helm, Kubernetes的包管理工具](https://www.kubernetes.org.cn/3435.html)







