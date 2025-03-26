---
layout: post
title:  "tRPC-Go in Action"
date:   2023-04-13 12:30:00 +0800
categories: GoLang
---

* Do not remove this line (it will not be displayed)
{:toc}


# 业务服务开发步骤

* 每个服务单独创建一个 git，如：`git.woa.com/trpc-go/helloworld`
* 初始化 go mod 文件：`go mod init git.woa.com/trpc-go/helloworld`
* 编写服务协议文件，如：`helloworld.proto`, 协议规范如下：
    * `package` 分成三级 `trpc.app.server`, `app` 是一个业务项目分类，`server` 是具体的进程服务名
    * 指定 `option go_package`，建议为协议的 `git` 地址。开发期间，也可以使用本地的地址，例如：
      + `option go_package="git.woa.com/trpcprotocol/test/helloworld";` 表示在 `git.woa.com/trpcprotocol/test/helloworld` 地址生成 `pb.go` 文件
      + `option go_package = "server/api/trpc/proto";` 表示在本地目录 `server/api/trpc/proto` 下生成 `pb.go` 文件
    * 定义 `service rpc` 方法，一个 `server` 可以有多个 `service`，一般都是一个 `server` 一个 `service`

``` proto
syntax = "proto3";

package trpc.test.helloworld;
option go_package="git.woa.com/trpcprotocol/test/helloworld";

service Greeter {
    rpc SayHello (HelloRequest) returns (HelloReply) {}
}

message HelloRequest {
    string msg = 1;
}

message HelloReply {
    string msg = 1;
}
```

* 通过命令行生成服务模型：`trpc create --protofile=helloworld.proto`（首先需要先[安装 trpc 工具](https://git.woa.com/trpc-go/trpc-go-cmdline)）

``` bash
#!/bin/bash

# 只生成协议代码
trpc create --protofile=qqchatsvr.proto --rpconly
```

* 可以在 `trpc_go.yaml` 的 server service 中额外添加 HTTP RPC 服务：

``` yaml
    - name: trpc.test.helloworld.Greeter  # service 的名字服务路由名称
      ip: 127.0.0.1                       # 服务监听 ip 地址
      port: 8080                          # 服务监听端口
      network: tcp                        # 网络监听类型 tcp udp
      protocol: http                      # 应用层协议 trpc http
      timeout: 1000                       # 请求最长处理时间 单位 毫秒
```

* 开发具体业务逻辑
* 开发完成，开始编译，根目录执行：`go build`
* 执行单元测试：`go test -v`
* 启动服务：`./helloworld &`
* 自测 trpc 协议：`trpc-cli -func "/trpc.test.helloworld.Greeter/SayHello" -target "ip://127.0.0.1:8000" -body '{"msg":"hello"}' -v`
* 自测 http 协议：`curl -X POST -d '{"msg":"hello"}' -H "Content-Type:application/json" http://127.0.0.1:8080/trpc.test.helloworld.Greeter/SayHello`

> 注意：`trpc-cli` 工具支持很多参数，使用时注意指定。
>
> 1. `func` 为 pb 协议定义的 `/package.service/method`，如上面的 `helloworld.proto`，则为 `/trpc.test.helloworld.Greeter/SayHello`，千万注意：不是 yaml 里面配置的 service。
>
> 2. `target` 为被调服务的目标地址，格式为 `selectorname://servicename`，这里只是本地自测，没有接入名字服务，直接指定 `ip:port` 寻址，使用 `ip selector` 就可以了，格式是 `ip://${ip}:${port}`，例如：`ip://127.0.0.1:8000`。
>
> 3. `body` 为请求包体数据的 `json` 结构字符串，内部 `json` 字段要跟 `pb` 定义的字段完全一致，注意大小写不要写错。


# tRPC-Go 拦截器功能及实现 (filter 过滤器)

tRPC-Go 框架的拦截器，也称之为过滤器。tRPC 框架利用拦截器的机制，将接口请求相关的特定逻辑组件化，插件化，从而同具体的业务逻辑解除耦合，达到复用的目的。例如监控拦截器，分布式追踪拦截器，日志拦截器，鉴权拦截器等。

触发时机：拦截器可以拦截到接口的请求和响应，并对请求，响应，上下文进行处理（用通俗的语言阐述也就是，可以在请求接收前做一些事情，请求处理后做一些事情），因此，拦截器从功能上说是分为两个部分：前置（业务逻辑处理前）和 后置（业务逻辑处理后）。

顺序性：拦截器是有明确的顺序性，根据拦截器的注册顺序依次执行前置部分逻辑，并逆序执行拦截器的后置部分。

## 自定义一个拦截器

### 定义处理逻辑函数

``` golang
func ServerFilter() filter.ServerFilter {
	return func(ctx context.Context, req interface{}, handler filter.ServerHandleFunc) (rsp interface{}, err error) {

		// 前置逻辑
		log.Debug("ServerFilter before")
		begin := time.Now()

		rsp, err = handler(ctx, req)

		// 后置逻辑
		log.Debug("ServerFilter post")
		cost := time.Since(begin) // 业务逻辑处理后计算耗时
		log.Debugf("cost: %v", cost)

		// 必须返回 next 的 rsp 和 err，要格外注意不要被自己的逻辑的 rsp 和 err 覆盖
		return rsp, err
	}
}

func ClientFilter() filter.ClientFilter {
	return func(ctx context.Context, req, rsp interface{}, handler filter.HandleFunc) (err error) {

		// 前置逻辑
		log.Debug("ClientFilter before")
		begin := time.Now()

		err = handler(ctx, req, rsp)

		// 后置逻辑
		log.Debug("ClientFilter post")
		cost := time.Since(begin)
		log.Debugf("cost: %v", cost)

		return err
	}
}
```

### 注册到框架中

``` golang
filter1 := ServerFilter()
filter2 := ClientFilter()

filter.Register("name", filter1, filter2) // 拦截器名字自己随便定义，供后续配置文件使用，必须放在 trpc.NewServer() 之前
```

### 配置文件开启使用

``` yaml
server:
 filter:  # 对所有 service 全部生效
   - name1  # 上面第三步注册到框架中的 server 拦截器名字
 service:
   - name: trpc.app.server.service
     filter:  # 只对当前 service 生效
       - name2
client:
 ...
 filter:
  ...
  - name

```

### 完整拦截器代码示例

``` golang
// Package metirc is a tRPC filter used to report service metrics to monitor
package metric

import (
	"context"
	"time"

	"git.code.oa.com/trpc-go/trpc-go/filter"
	"git.code.oa.com/trpc-go/trpc-go/log"
)

func init() {
	filter.Register("metric", ServerFilter(), ClientFilter())
}

func ServerFilter() filter.ServerFilter {
	return func(ctx context.Context, req interface{}, handler filter.ServerHandleFunc) (rsp interface{}, err error) {

		// 前置逻辑
		log.Debug("ServerFilter before")
		begin := time.Now()

		rsp, err = handler(ctx, req)

		// 后置逻辑
		log.Debug("ServerFilter post")
		cost := time.Since(begin) // 业务逻辑处理后计算耗时
		log.Debugf("cost: %v", cost)

		// 必须返回 next 的 rsp 和 err，要格外注意不要被自己的逻辑的 rsp 和 err 覆盖
		return rsp, err
	}
}

func ClientFilter() filter.ClientFilter {
	return func(ctx context.Context, req, rsp interface{}, handler filter.HandleFunc) (err error) {

		// 前置逻辑
		log.Debug("ClientFilter before")
		begin := time.Now()

		err = handler(ctx, req, rsp)

		// 后置逻辑
		log.Debug("ClientFilter post")
		cost := time.Since(begin)
		log.Debugf("cost: %v", cost)

		return err
	}
}
```