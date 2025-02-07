---
layout: post
title:  "Deepseek in Action"
date:   2025-02-07 12:00:00 +0800
categories: 机器学习
---

* Do not remove this line (it will not be displayed)
{:toc}

# 背景介绍

[DeepSeek](https://github.com/deepseek-ai) 是由深度求索公司推出的大语言模型。其中：

* `DeepSeek-V3` 是在14.8万亿高质量 token 上完成预训练的一个强大的**混合专家 (MoE) 语言模型**，拥有**6710亿参数**。作为通用大语言模型，其在知识问答、内容生成、智能客服等领域表现出色。
* `DeepSeek-R1` 是基于 DeepSeek-V3-Base 训练生成的高性能推理模型，在数学、代码生成和逻辑推断等复杂推理任务上表现优异。
* `DeepSeek-R1-Distill` 是使用 DeepSeek-R1 生成的样本对开源模型进行有**监督微调（SFT）**得到的小模型，即**蒸馏模型**。拥有更小参数规模，推理成本更低，基准测试同样表现出色。


DeepSeek-V3 在推理速度上相较历史模型有了大幅提升。在目前大模型主流榜单中，DeepSeek-V3 在开源模型中位列榜首，与世界上最先进的闭源模型不分伯仲。论文：https://github.com/deepseek-ai/DeepSeek-R1/blob/main/DeepSeek_R1.pdf

![deepseek0](/assets/images/202502/deepseek0.png)

# [DeepSeek-R1](https://github.com/deepseek-ai/DeepSeek-R1)

We introduce our first-generation reasoning models, `DeepSeek-R1-Zero` and `DeepSeek-R1`. `DeepSeek-R1-Zero`, a model trained via large-scale **reinforcement learning** (`RL`) without **supervised fine-tuning** (`SFT`) as a preliminary step, **demonstrated remarkable performance on reasoning**. With `RL`, `DeepSeek-R1-Zero` naturally emerged with numerous powerful and interesting reasoning behaviors. However, `DeepSeek-R1-Zero` **encounters challenges such as endless repetition, poor readability, and language mixing**. To address these issues and further enhance reasoning performance, we introduce `DeepSeek-R1`, which incorporates cold-start data before `RL`. `DeepSeek-R1` achieves performance comparable to `OpenAI-o1` across **math**, **code**, and **reasoning tasks**. To support the research community, we have open-sourced `DeepSeek-R1-Zero`, `DeepSeek-R1`, and six dense models **distilled** from `DeepSeek-R1` based on `Llama` and `Qwen`. `DeepSeek-R1-Distill-Qwen-32B` outperforms `OpenAI-o1-mini` across various benchmarks, achieving new state-of-the-art results for dense models.

DeepSeek-R1-Zero是一个通过大规模强化学习（RL）训练的模型，没有进行监督微调（SFT）作为预备步骤。这个模型在推理方面表现出了显著的性能。通过RL，DeepSeek-R1-Zero自然地展现出了许多强大且有趣的推理行为。然而，DeepSeek-R1-Zero也遇到了一些挑战，如无尽的重复、可读性差和语言混淆等问题。

为了解决这些问题并进一步提高推理性能，他们引入了DeepSeek-R1，这个模型在RL之前引入了冷启动数据。DeepSeek-R1在数学、编码和推理任务上的性能与OpenAI-o1相当。

为了支持研究社区，他们已经开源了DeepSeek-R1-Zero、DeepSeek-R1以及从DeepSeek-R1中提炼出的六个基于Llama和Qwen的密集模型。其中，DeepSeek-R1-Distill-Qwen-32B在各种基准测试中的表现超过了OpenAI-o1-mini，为密集模型创造了新的最佳结果。

> **NOTE**: Before running DeepSeek-R1 series models locally, we kindly recommend reviewing the **Usage Recommendation** section.

**Usage Recommendations**

**We recommend adhering to the following configurations when utilizing the DeepSeek-R1 series models, including benchmarking, to achieve the expected performance:**

* Set the temperature within the range of 0.5-0.7 (0.6 is recommended) to prevent endless repetitions or incoherent outputs.
* Avoid adding a system prompt; all instructions should be contained within the user prompt.
* For mathematical problems, it is advisable to include a directive in your prompt such as: "Please reason step by step, and put your final answer within \boxed{}."
* When evaluating model performance, it is recommended to conduct multiple tests and average the results.

{% raw %}
Additionally, we have observed that the DeepSeek-R1 series models tend to bypass thinking pattern (i.e., outputting "<think>\n\n</think>") when responding to certain queries, which can adversely affect the model's performance. **To ensure that the model engages in thorough reasoning, we recommend enforcing the model to initiate its response with "<think>\n" at the beginning of every output**.
{% endraw %}


![deepseek_compare](/assets/images/202502/deepseek_compare.png)




## Chat Website & API Platform

You can chat with `DeepSeek-R1` on DeepSeek's official website: chat.deepseek.com, and switch on the button "DeepThink"

We also provide OpenAI-Compatible API at DeepSeek Platform: platform.deepseek.com


![deepseek_demo0](/assets/images/202502/deepseek_demo0.png)

![deepseek_demo](/assets/images/202502/deepseek_demo.png)


## How to Run Locally

Please visit [DeepSeek-V3](https://github.com/deepseek-ai/DeepSeek-V3) repo for more information about running DeepSeek-R1 locally.


# [DeepSeek-V3](https://github.com/deepseek-ai/DeepSeek-V3)

We present `DeepSeek-V3`, a strong **Mixture-of-Experts** (`MoE`) language model with `671B` total parameters with `37B` activated for each token. To achieve efficient inference and cost-effective training, `DeepSeek-V3` adopts **Multi-head Latent Attention** (`MLA`) and DeepSeekMoE architectures, which were thoroughly validated in `DeepSeek-V2`. Furthermore, `DeepSeek-V3` pioneers an auxiliary-loss-free strategy for load balancing and sets a multi-token prediction training objective for stronger performance. We pre-train `DeepSeek-V3` on **14.8 trillion diverse and high-quality tokens**, followed by Supervised Fine-Tuning and Reinforcement Learning stages to fully harness its capabilities. Comprehensive evaluations reveal that `DeepSeek-V3` outperforms other open-source models and achieves performance comparable to leading closed-source models. Despite its excellent performance, `DeepSeek-V3` requires only **2.788M H800 GPU hours** for its full training. In addition, its training process is remarkably stable. Throughout the entire training process, we did not experience any irrecoverable loss spikes or perform any rollbacks.

DeepSeek-V3，这是一个强大的专家混合（Mixture-of-Experts，MoE）语言模型，总共有671B（即6710亿）个参数，每个标记（token）激活了37B（即370亿）个参数。为了实现高效的推理和经济有效的训练，DeepSeek-V3采用了多头潜在注意力（Multi-head Latent Attention，MLA）和DeepSeekMoE架构，这些都在DeepSeek-V2中得到了充分的验证。

此外，DeepSeek-V3首次引入了一个无辅助损失的策略进行负载均衡，并设定了一个多标记预测训练目标以提高性能。我们在14.8万亿个多样化和高质量的标记上预训练了DeepSeek-V3，然后进行了监督微调和强化学习阶段，以充分发挥其能力。

全面的评估显示，DeepSeek-V3的性能超过了其他开源模型，并与领先的闭源模型相当。尽管DeepSeek-V3的性能出色，但其完全训练只需要2.788M H800 GPU小时。此外，其训练过程非常稳定。在整个训练过程中，我们没有遇到任何无法恢复的损失峰值，也没有进行任何回滚操作。


![deepseek_compare2](/assets/images/202502/deepseek_compare2.png)


## Chat Website & API Platform

You can chat with DeepSeek-V3 on DeepSeek's official website: chat.deepseek.com

We also provide OpenAI-Compatible API at DeepSeek Platform: platform.deepseek.com

## [How to Run Locally](https://github.com/deepseek-ai/DeepSeek-V3?tab=readme-ov-file#6-how-to-run-locally)

`DeepSeek-V3` can be deployed locally using the following hardware and open-source community software:

...






# [快速使用 Deepseek-R1 模型](https://cloud.tencent.com/document/product/1721/115966)

`DeepSeek-R1` 在后训练阶段大规模使用了强化学习技术，在仅有极少标注数据的情况下，极大提升了模型推理能力。在数学、代码、自然语言推理等任务上，性能比肩 `OpenAI o1` 正式版。通过[腾讯云高性能应用服务 HAI](https://cloud.tencent.com/product/hai) 更易用的 GPU 智算服务可快速体验 Deepseek 模型的推理能力。HAI 已提供 DeepSeek-R1 1.5B 及 7B 模型预装环境（DeepSeek-R1-Distill-Qwen-1.5B、DeepSeek-R1-Distill-Qwen-7B），用户可在 HAI 中快速启动，进行测试并接入业务。若有更大尺寸模型（14B、32B、70B）的使用需求，也可根据使用说明中的指引进行快速部署。可参考[DeepSeek-R1 高性能应用服务 HAI 开箱即用](https://cloud.tencent.com/developer/article/2492236)

![deepseek_inst](/assets/images/202502/deepseek_inst.png)


## 创建 Deepseek-R1 应用

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



## 使用 Deepseek-R1

![deepseek3](/assets/images/202502/deepseek3.png)

### 通过可视化界面使用

* 登录[高性能应用服务 HAI 控制台](https://console.cloud.tencent.com/hai/instance?rid=1)，选择“算力连接” > "ChatbotUI"
* 在新窗口中，可以根据页面指引，完成与模型的交互。

[体验地址](http://159.75.154.37:6889/zh)


![deepseek4](/assets/images/202502/deepseek4.png)

![deepseek5](/assets/images/202502/deepseek5.png)

![deepseek8](/assets/images/202502/deepseek8.png)


### 通过命令行使用

* 在[高性能应用服务 HAI 控制台](https://console.cloud.tencent.com/hai/instance?rid=1)，选择“算力连接” > "JupyterLab"
* 新建一个 "Terminal"。
* 输入以下命令加载默认模型：`ollama run deepseek-r1`
* 运行效果

[体验地址](http://159.75.154.37:6888/lab)

![deepseek6](/assets/images/202502/deepseek6.png)

![deepseek7](/assets/images/202502/deepseek7.png)


## Tips

### 切换不同参数量级

若默认的 1.5B 蒸馏模型无法满足需求，可通过以下命令自定义模型参数量级：

DeepSeek-R1-Distill-7B: `ollama run deepseek-r1:7b`
DeepSeek-R1-Distill-8B: `ollama run deepseek-r1:8b`
DeepSeek-R1-Distill-14B: `ollama run deepseek-r1:14b`

### API 调用

实例环境中已预装并启动 `Ollama serve`，该服务支持通过 `REST API` 进行调用。可以参考 [Ollama API 文档](https://github.com/ollama/ollama/blob/main/docs/api.md)，以了解具体的调用方式和方法。

### 搭建个人知识库

> [Cherry Studio](https://github.com/CherryHQ/cherry-studio) is a desktop client that supports for multiple LLM providers, available on Windows, Mac and Linux. Cherry Studio 是一个支持多模型服务的开源桌面客户端，可以将多服务集成至桌面 AI 对话应用中。

* 参考教程：https://docs.cherry-ai.com/advanced-basic/knowledge-base

* 下载 [Cherry Studio](https://cherry-ai.com/)：一款支持多个大语言模型（LLM）服务商的桌面客户端。

![cherry-studio](/assets/images/202502/cherry-studio.png)

* 配置 API：进入设置界面，选择“模型服务”中的 `Ollama`，填写 API 地址及模型名称。
  + API 地址：将默认的 localhost 替换为 HAI 实例的公网 IP，将端口号由 `11434` 修改为 `6399`。
  + 单击下方的“添加”按钮添加模型，模型 ID 输入 `deepseek-r1:7b` 或 `deepseek-r1:1.5b`

![cherry-studio2](/assets/images/202502/cherry-studio2.png)

* 检查连通性：单击 API 密钥右侧的“检查”按钮，API 密钥不需填写，页面显示“连接成功”即可完成配置。

![cherry-studio3](/assets/images/202502/cherry-studio3.png)

* 添加本地知识库文件并使用：完成 API 配置后，可在“知识库”模块中上传本地文件，并进入聊天界面进行使用。

![cherry-studio4](/assets/images/202502/cherry-studio4.png)

![cherry-studio5](/assets/images/202502/cherry-studio5.png)

* 使用自己的 agent

![cherry-studio10](/assets/images/202502/cherry-studio10.png)

![cherry-studio7](/assets/images/202502/cherry-studio7.png)

* 上传资料，搜索知识库 (搜索知识库，只有按匹配打分的排序)

![cherry-studio8](/assets/images/202502/cherry-studio8.png)

![cherry-studio9](/assets/images/202502/cherry-studio9.png)

* 对话中引用知识库生成回复 (发现推理总结有时比较混乱)
  + 创建一个新的话题，在对话工具栏中，点击知识库，会展开已经创建的知识库列表，选择需要引用的知识库；
  + 输入并发送问题，模型即返回通过检索结果生成的答案；
  + 同时，引用的数据来源会附在答案下方，可快捷查看源文件。

![cherry-studio11](/assets/images/202502/cherry-studio11.png)




## Q&A

## [获取嵌入维度失败 #932](https://github.com/CherryHQ/cherry-studio/issues/932)

在模型服务中，选择 Ollama，将 deepseek-r1:1.5b 选择模型类型勾选：嵌入。

![cherry-studio6](/assets/images/202502/cherry-studio6.png)

## [出错了，如果没有配置 API 密钥，请前往设置 > 模型提供商中配置密钥 #1027](https://github.com/CherryHQ/cherry-studio/issues/1027)

选择自己的模型，并重置。

问题解释：https://docs.cherry-ai.com/advanced-basic/knowledge-base

> 注意：
>
> 嵌入类模型、对话类模型、绘画类模型等各自有各自的功能，其请求方式跟返回内容、结构都有所不同，请勿强行将其他类别的模型作为嵌入模型使用；
>
> 嵌入类模型 CherryStudio 会自动分类显示在嵌入模型列表中，如果确认为嵌入模型但未被正确分类，可到模型列表中点击对应模型后方的设置按钮勾选嵌入选项；如果无法确认哪些模型是嵌入模型可到对应服务商查询模型信息。


# 大小模型效果对比 (DeepSeek-R1-Distill-Qwen-1.5B VS. DeepSeek-R1)

![deepseek_cmp](/assets/images/202502/deepseek_cmp.png)

从响应结果中可以明显看出，拥有更大参数量的 `DeepSeek-R1` 模型在推理效果上更胜一筹，其正确推理出了杯子倒扣时球会掉出并留在床上，即使杯子随后被移动至房间。而参数量较小的 `DeepSeek-R1-Distill-Qwen-1.5B` 模型仍然认为球在杯中。

另一方面，相比 `DeepSeek-R1` 模型，更小参数的 `DeepSeek-R1-Distill-Qwen-1.5B` 模型的响应速度更快、占用资源更少、部署时长更短，在处理较为简单的任务时，仍是不错的选择。
其中，`DeepSeek-R1-Distill-Qwen-1.5B` 的部署时长预计为 1-2 分钟，`DeepSeek-R1` 预计为 9-10 分钟（模型需预加载到节点的本地数据盘中）。

> 由于 R1、V3 模型的参数量较大，其模型体积达到 641 GB，仅从平台存储加载到机器就需要相当长时间（达2小时以上），因此在模型未提前存储到机器的情况下，模型部署时间整体也会较长。

系列模型清单：

```
DeepSeek-V3
DeepSeek-R1
DeepSeek-R1-Distill-Qwen-1.5B
DeepSeek-R1-Distill-Qwen-7B
DeepSeek-R1-Distill-Llama-8B
DeepSeek-R1-Distill-Qwen-14B
DeepSeek-R1-Distill-Qwen-32B
DeepSeek-R1-Distill-Llama-70B
```

# Refer

* https://github.com/deepseek-ai
* https://www.deepseek.com/
* https://cloud.tencent.com/product/hai
* https://cherry-ai.com/
* https://github.com/CherryHQ/cherry-studio
* https://github.com/ollama/ollama
* https://cloud.tencent.com/document/product/1721/115966
* [快速部署和体验 DeepSeek 系列模型](https://cloud.tencent.com/document/product/851/115962?from=25520)
* [大模型推理所需资源指南](https://cloud.tencent.com/document/product/851/107823)











