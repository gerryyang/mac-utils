---
layout: post
title:  "Grafana in Action"
date:   2022-08-19 12:30:00 +0800
categories: Tools
---

* Do not remove this line (it will not be displayed)
{:toc}


# 权限管理

Grafana 的权限分为三个等级：Viewer、Editor 和 Admin，Viewer 只能查看 Grafana 已经存在的面板而不能编辑，Editor 可以编辑面板，Admin 则拥有全部权限例如添加数据源、添加插件、增加 API KEY。



# HTTP API

The Grafana backend exposes an HTTP API, which is the same API that is used by the frontend to do everything from saving dashboards, creating users, and updating data sources. See [HTTP API reference](https://grafana.com/docs/grafana/latest/developers/http_api/)




# Refer

* https://grafana.com/docs/grafana/latest/introduction/
* [Grafana 的一些使用技巧](https://zhuanlan.zhihu.com/p/34005738)








