---
layout: post
title:  "AI Tools in Action"
date:   2025-08-01 12:30:00 +0800
categories: ML
---

* Do not remove this line (it will not be displayed)
{:toc}




# AI Agent


## [Coze Studio](https://github.com/coze-dev/coze-studio)


[Coze Studio](https://www.coze.cn/home) is an **all-in-one AI agent development tool**. Providing the latest large models and tools, various development modes and frameworks, Coze Studio offers the most convenient AI agent development environment, from development to deployment.

* **Provides all core technologies needed for AI agent development**: `prompt`, `RAG`, `plugin`, `workflow`, enabling developers to focus on creating the core value of AI.

* **Ready to use for professional AI agent development at the lowest cost**: Coze Studio provides developers with complete app templates and build frameworks, allowing you to quickly construct various AI agents and turn creative ideas into reality.



## [Eino](https://github.com/cloudwego/eino) (基于 GoLang 的 AI 应用开发框架)

**Eino**`['aino]` (**pronounced similarly to "I know"**) aims to be **the ultimate LLM application development framework in Golang**. Drawing inspirations from many excellent LLM application development frameworks in the open-source community such as `LangChain` & `LlamaIndex`, etc., as well as learning from cutting-edge research and real world applications, **Eino** offers an LLM application development framework that emphasizes on **simplicity**, **scalability**, **reliability** and **effectiveness** that better aligns with Golang programming conventions.

What **Eino** provides are:

* a carefully curated list of **component** abstractions and implementations that can be easily reused and combined to build LLM applications
* a powerful **composition** framework that does the heavy lifting of strong type checking, stream processing, concurrency management, aspect injection, option assignment, etc. for the user.
* a set of meticulously(细致地) designed **API** that obsesses on simplicity and clarity.
* an ever-growing collection of best practices in the form of bundled **flows** and **examples**.
* a useful set of tools that covers the entire development cycle, from visualized development and debugging to online tracing and evaluation.

With the above arsenal, **Eino** can **standardize, simplify, and improve efficiency at different stages of the AI application development cycle**:

![eino_concept](/assets/images/202508/eino_concept.png)


More:

* https://www.cloudwego.io/zh/docs/eino/
* [大语言模型应用开发框架 —— Eino 正式开源](https://www.cloudwego.io/zh/docs/eino/overview/eino_open_source/)
* [Eino: 编排的设计理念](https://www.cloudwego.io/zh/docs/eino/core_modules/chain_and_graph_orchestration/orchestration_design_principles/)


## [Dify](https://github.com/langgenius/dify)

**Dify** is an open-source platform for developing LLM applications. Its intuitive interface combines agentic AI workflows, RAG pipelines, agent capabilities, model management, observability features, and more—allowing you to quickly move from prototype to production.



# Workflow Engine

## [Temporal](https://github.com/temporalio/temporal) (golang)

Temporal 产品和 Demo 介绍：https://www.youtube.com/watch?v=wIpz4ioK0gI

> Developing your application on the **Temporal** platform gives you a secret weapon—durable execution—which guarantees that your code runs to completion no matter what. The result? Bulletproof applications that are faster to develop and easier to support.


**Temporal** is a durable execution platform that enables developers to build scalable applications without sacrificing productivity or reliability. The Temporal server executes units of application logic called Workflows in a resilient manner that automatically handles **intermittent**(断断续续的) failures, and retries failed operations.

**Temporal** is a mature technology that originated as a fork of Uber's Cadence. It is developed by [Temporal Technologies](https://temporal.io/), a startup by the creators of Cadence.


![temporal](/assets/images/202508/temporal.png)

![temporal2](/assets/images/202508/temporal2.png)


### Deployments

实际的部署方案可参考 [Temporal Platform production deployments](https://docs.temporal.io/production-deployment)

To take your application to production, you deploy your application code, including your Workflows, Activities, and Workers, on your infrastructure using your existing build, test and deploy tools.**Then you need a production-ready Temporal Service to coordinate the execution of your Workflows and Activities**.You can use **Temporal Cloud**, a fully-managed platform, or you can **self-host the service**.

> Use Temporal Cloud

You can let us handle the operations of running the Temporal Service, and focus on your application. Follow the [Temporal Cloud guide](https://docs.temporal.io/cloud) to get started.


![temporal3](/assets/images/202508/temporal3.png)


> Run a Self-hosted Temporal Service

Alternatively, you can run your own production level Temporal Service to orchestrate your durable applications. Follow the [Self-hosted guide](https://docs.temporal.io/self-hosted-guide) to get started.

![temporal4](/assets/images/202508/temporal4.png)


### [How Temporal works and how it works for you](https://temporal.io/how-it-works)


> Write your business logic as code

**Temporal SDKs** provide a foundation for your application. Choose the SDK for your preferred language and write your business logic. You can use your favorite IDE, libraries, and tools as you code and then you can build, deploy, and run the application wherever and however you choose.

> Create Workflows that guarantee execution

A **Temporal Workflow** defines your business logic. It might involve moving money between bank accounts, processing orders, deploying cloud infrastructure, training an AI model, or something else entirely.

Because the full running state of a workflow is durable and fault tolerant by default, your business logic can be recovered, replayed or paused from any arbitrary point.

> Code Activities to handle and retry failure-prone logic

Applications need to interact with the messy real world of services, users and devices that are inherently failure prone or take a long time to respond.

With Temporal, these parts of your logic are implemented as Activities, separate functions or methods orchestrated by the Workflow. **Temporal Activities can be run for as long as necessary (with heartbeat), can be automatically retried forever (set as policy) and can be routed to specific services or processes**.

> Deploy your application on YOUR infrastructure

You deploy your application code (Workflow and Activities) on your infrastructure using your existing build, test and deploy tools.

It's up to you how you structure your application. Temporal can be gradually introduced into your existing architecture, be it a monolith or a complex web of microservices.

> Connect your application to a Temporal Service

The **Temporal Service** coordinates the execution of your application code — **Workflows and Activities** — by exchanging events with Workers. The Service can be self-hosted or fully managed and serverless with Temporal Cloud.

Workers poll a Task Queue that feeds it tasks to execute. It reports the result back to the Service, which responds by adding additional tasks to the queue, repeating this process until execution is finished.

The Service maintains a detailed history of every execution. If the app crashes, another Worker automatically takes over by replaying this history in order to recover the state prior to the crash, and then continues.

> Temporal is secure by design

**Workflow** and **Activities** are deployed as part of your application, on your infrastructure and all connections from your app to the **Temporal Service** are **unidirectional(单向的)** so you never need to open up the firewall.

All communications are secure and your data is **encrypted** within your app, with your encryption library and keys. The **Temporal Service** never needs to access your data in clear text.

> Tune Workers to increase scale and availability

At runtime, applications can grow from a few to several thousand instances depending on your requirements for scale, performance and availability. **We recommend that every application has at least two Workers, so there’s no single point of failure**.

You can tune your Workers for optimal performance and we've tested **Temporal Cloud** to scale beyond 200 million executions per second.

> Manage the Temporal Server using terminal

A command-line interface available for macOS, Windows, and Linux enables you to interact with the Temporal Service from your terminal. It allows you to manage Workflow executions and view history.

Additionally, it can be used to spin up (start-dev) a self-contained lightweight Server in less than two seconds, allowing you to develop and test Temporal applications right from your laptop.

> Monitor individual executions of your application

Finally, the **Temporal Web UI** allows you to inspect details of past and present Workflow Executions from a convenient browser-based interface. It can help you to quickly isolate, debug, and resolve production problems.

![temporal5](/assets/images/202508/temporal5.png)

