---
layout: post
title:  "Deepseek in Action"
date:   2025-02-07 12:00:00 +0800
categories: 机器学习
---

* Do not remove this line (it will not be displayed)
{:toc}

# 背景介绍

DeepSeek-V3 在推理速度上相较历史模型有了大幅提升。在目前大模型主流榜单中，DeepSeek-V3 在开源模型中位列榜首，与世界上最先进的闭源模型不分伯仲。

![deepseek0](/assets/images/202502/deepseek0.png)

# Deepseek 官方服务使用

* 网页版：https://chat.deepseek.com/
* 手机 APP

![deepseek_demo0](/assets/images/202502/deepseek_demo0.png)

![deepseek_demo](/assets/images/202502/deepseek_demo.png)


# 快速使用 Deepseek-R1 模型

`DeepSeek-R1` 在后训练阶段大规模使用了强化学习技术，在仅有极少标注数据的情况下，极大提升了模型推理能力。在数学、代码、自然语言推理等任务上，性能比肩 `OpenAI o1` 正式版。通过[腾讯云高性能应用服务 HAI](https://cloud.tencent.com/product/hai) 更易用的 GPU 智算服务可快速体验 Deepseek 模型的推理能力。HAI 已提供 DeepSeek-R1 模型预装环境，可在 HAI 中快速启动进行测试并接入业务。


# 创建 Deepseek-R1 应用

* 登录[高性能应用服务 HAI 控制台](https://console.cloud.tencent.com/hai/instance?rid=1)。
* 单击新建，进入[高性能应用服务 HAI 购买页面](https://buy.cloud.tencent.com/hai)。
   + 选择应用：选择社区应用，应用选择 `Deepseek-R1`。
   + 地域：建议选择靠近自己实际地理位置的地域，降低网络延迟、提高您的访问速度。
   + 算力方案：支持基础型及进阶型两类算力方案。
   + 实例名称：自定义实例名称，若不填则默认使用实例 ID 替代。
   + 购买数量：默认1台。

* 单击立即购买。
* 核对配置信息后，单击提交订单，并根据页面提示完成支付。
* 等待创建完成。单击实例任意位置并进入该实例的详情页面。
* 可以在此页面查看 Deepseek-R1 详细的配置信息，到此为止，说明 Deepseek-R1 应用实例购买成功。

![deepseek1](/assets/images/202502/deepseek1.png)

![deepseek2](/assets/images/202502/deepseek2.png)



# 使用 Deepseek-R1

![deepseek3](/assets/images/202502/deepseek3.png)

## 通过可视化界面使用

* 登录[高性能应用服务 HAI 控制台](https://console.cloud.tencent.com/hai/instance?rid=1)，选择“算力连接” > "ChatbotUI"
* 在新窗口中，可以根据页面指引，完成与模型的交互。

[体验地址](http://159.75.154.37:6889/zh)


![deepseek4](/assets/images/202502/deepseek4.png)

![deepseek5](/assets/images/202502/deepseek5.png)

![deepseek8](/assets/images/202502/deepseek8.png)


## 通过命令行使用

* 在[高性能应用服务 HAI 控制台](https://console.cloud.tencent.com/hai/instance?rid=1)，选择“算力连接” > "JupyterLab"
* 新建一个 "Terminal"。
* 输入以下命令加载默认模型：`ollama run deepseek-r1`
* 运行效果

[体验地址](http://159.75.154.37:6888/lab)

![deepseek6](/assets/images/202502/deepseek6.png)

![deepseek7](/assets/images/202502/deepseek7.png)


# Tips

## 切换不同参数量级

若默认的 1.5B 蒸馏模型无法满足需求，可通过以下命令自定义模型参数量级：

DeepSeek-R1-Distill-7B: `ollama run deepseek-r1:7b`
DeepSeek-R1-Distill-8B: `ollama run deepseek-r1:8b`
DeepSeek-R1-Distill-14B: `ollama run deepseek-r1:14b`

## API 调用

实例环境中已预装并启动 `Ollama serve`，该服务支持通过 `REST API` 进行调用。可以参考 [Ollama API 文档](https://github.com/ollama/ollama/blob/main/docs/api.md)，以了解具体的调用方式和方法。

## 搭建个人知识库

* 下载 [Cherry Studio](https://cherry-ai.com/)：一款支持多个大语言模型（LLM）服务商的桌面客户端。

![cherry-studio](/assets/images/202502/cherry-studio.png)

* 配置 API：进入设置界面，选择“模型服务”中的 `Ollama`，填写 API 地址及模型名称。
  + API 地址：将默认的 localhost 替换为 HAI 实例的公网 IP，将端口号由 11434 修改为 6399。
  + 单击下方的“添加”按钮添加模型，模型 ID 输入 `deepseek-r1:7b` 或 `deepseek-r1:1.5b`

![cherry-studio2](/assets/images/202502/cherry-studio2.png)

* 检查连通性：单击 API 密钥右侧的“检查”按钮，API 密钥不需填写，页面显示“连接成功”即可完成配置。

![cherry-studio3](/assets/images/202502/cherry-studio3.png)

* 添加本地知识库文件并使用：完成 API 配置后，可在“知识库”模块中上传本地文件，并进入聊天界面进行使用。

![cherry-studio4](/assets/images/202502/cherry-studio4.png)

![cherry-studio5](/assets/images/202502/cherry-studio5.png)



# Refer

* https://www.deepseek.com/
* https://cloud.tencent.com/product/hai
* https://cherry-ai.com/











