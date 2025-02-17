---
layout: post
title:  "Machine Learning in Action"
date:   2024-02-23 17:00:00 +0800
categories: 机器学习
---

* Do not remove this line (it will not be displayed)
{:toc}


# TTS (Text To Speech 从文本到语音)

它是同时运用语言学和心理学的杰出之作，在内置芯片的支持之下，通过神经网络的设计，把文字智能地转化为自然语音流。TTS技术对文本文件进行实时转换，转换时间之短可以秒计算。在其特有智能语音控制器作用下，文本输出的语音音律流畅，使得听者在听取信息时感觉自然，毫无机器语音输出的冷漠与生涩感。

* [GPT-SoVITS指南](https://www.yuque.com/baicaigongchang1145haoyuangong/ib3g1e)
* [GPT-SoVITS-WebUI](https://github.com/RVC-Boss/GPT-SoVITS) A Powerful Few-shot Voice Conversion and Text-to-Speech WebUI.
* [耗时两个月自主研发的低成本AI音色克隆软件](https://www.bilibili.com/video/BV12g4y1m7Uw/?vd_source=a93a41a77eb9d3f6d0145e87094f8289)
* [GPT-SoVITS语音合成官方镜像，3080Ti卡测试通过](https://www.codewithgpu.com/i/RVC-Boss/GPT-SoVITS/GPT-SoVITS-Official)

测试使用：

* 各种游戏600多个角色在线试用：https://gsv.acgnai.top/
* zero shot试用：https://gsv-zs.acgnai.top/&https://huggingface.co/spaces/lj1995/GPT-SoVITS-v2




# 静态的图像识别技术 OpenCV

OpenCV（Open Source Computer Vision Library）是一个开源的计算机视觉和机器学习库，广泛应用于静态图像识别技术中。它由英特尔公司开发，并且支持多种编程语言，包括C++、Python、Java和MATLAB，适用于多个操作系统，如Windows、Linux、Mac OS、Android和iOS。

[OpenCV By Example](https://kingcall.oss-cn-hangzhou.aliyuncs.com/blog/pdf/OpenCV%20By%20Example64091604821968890.pdf)


# [推荐系统](https://en.wikipedia.org/wiki/Recommender_system) (Recommender systems)


## 召回

推荐系统如何根据已有的用户画像和内容画像去推荐，涉及到两个关键问题：**召回**和**排序**。

> **召回**(match) 指从全量信息集合中触发尽可能多的正确结果，并将结果返回给**排序**。

**召回**的方式有多种：协同过滤、主题模型、内容召回和热点召回等，而**排序**(rank)则是对所有召回的内容进行打分排序，选出得分最高的几个结果推荐给用户。

![zhaohui](/assets/images/202412/zhaohui.png)

在搜索系统中，用户会输入明确的搜索词，根据搜索词进行内容的召回和呈现即可，但在推荐系统中，用户没有一个明确的检索词(Query)输入，推荐系统需要做的，就是根据用户画像、内容画像等各种信息为用户推荐他可能感兴趣的内容。另外，由于没有明确的检索词，推荐系统就需要从整个信息集合中挑选出尽可能多的相关结果，同时又需要剔除相关性较弱的结果，降低排序阶段的工作量。

> 怎样的召回策略是好的？

召回策略的评估主要根据两个评价指标：**召回率**和**准确率**。

* 召回率 (Recall) = 系统检索到的相关内容 / 系统**所有相关的**内容总数
* 准确率 (Precision) = 系统检索到的相关内容 / 系统**所有检索到的**内容总数

例如在搜索情境下，当用户搜索“北京大学”时，想看到北大相关的网站、新闻等，系统返回了以下三个网页：

```
a. 北京大学保安考上研究生；
b. 北京互联网工作招聘；
c. 大学生活是什么样的；
```

可以看到，只有 a 是用户真正想看到的，其他两个和用户搜索词无关，而事实上数据库里还有这种网页：

```
d. 北大开学季；
e. 未名湖的景色；
```

d、e 这两个网页没被搜索到，但它们和“北京大学”的相关度其实是超过 b、c 的，也就是应该被搜索（被召回）到但实际并没有显示在结果里，所以，这个系统的：

```
召回率 = a / (a + d + e) = 0.33
准确率 = a / (a + b + c) = 0.33
```

这是搜索情境下的召回率和准确率，而“推荐”其实就是没有检索词输入时的搜索，例如，用户并没有输入“北京大学”这样的关键词，但其实他是北京大学的学生，对自己学校相关的新闻很感兴趣，推荐系统的召回就是要根据用户画像、内容画像等各种信息，为用户提供他感兴趣的相关内容，所以也存在召回率和准确率的问题。

召回率和准确率有时存在**相互制衡的情况**，**好的召回策略应该在保证高准确率的情况下也保证高召回率**。

> 今日头条的召回策略

今日头条作为业界推荐系统方面的顶级选手，不免有人会好奇，它的召回策略是怎样的？

今日头条的算法架构师曾在接受采访时表示，今日头条有一个世界范围内比较大的在线训练推荐模型，包括几百亿特征和几十亿的向量特征。但因为头条目前的内容量非常大，加上小视频内容有千万级别，推荐系统不可能所有内容全部由模型预估。所以需要设计一些召回策略，从庞大内容中筛选一个模型组成内容库。

**召回策略种类有很多，今日头条主要用的是倒排的思路**。离线维护一个倒排，这个倒排的key可以是分类，topic，实体，来源等，排序考虑热度、新鲜度、动作等。线上召回可以迅速从倒排中根据用户兴趣标签对内容做截断，高效地从很大的内容库中筛选比较靠谱的一小部分内容。**基于召回策略，把一个海量、无法把握的内容库，变成一个相对小、可以把握的内容库，再进入推荐模型。这样有效平衡了计算成本和效果**。




## [协同过滤](https://en.wikipedia.org/wiki/Collaborative_filtering) (Collaborative filtering)

**Collaborative filtering** (CF) is, besides content-based filtering, one of two major techniques used by [recommender systems](https://en.wikipedia.org/wiki/Recommender_system). Collaborative filtering has two senses, **a narrow one** and [a more general one](https://en.wikipedia.org/wiki/Recommender_system).

In the newer, **narrower sense**, collaborative filtering is a method of making automatic predictions (filtering) about a user's interests by utilizing preferences or taste information collected from many users (collaborating). **This approach assumes that if persons A and B share similar opinions on one issue, they are more likely to agree on other issues compared to a random pairing of A with another person**. For instance, a collaborative filtering system for television programming could predict which shows a user might enjoy based on a limited list of the user's tastes (likes or dislikes). These predictions are specific to the user, but use information gleaned from many users. This differs from the simpler approach of giving an average (non-specific) score for each item of interest, for example based on its number of votes.

In the **more general sense**, collaborative filtering is the process of filtering information or patterns using techniques involving collaboration among multiple agents, viewpoints, data sources, etc. Applications of collaborative filtering typically involve very large data sets. Collaborative filtering methods have been applied to many kinds of data including: sensing and monitoring data, such as in mineral exploration, environmental sensing over large areas or multiple sensors; financial data, such as financial service institutions that integrate many financial sources; and user data from electronic commerce and web applications.

协同过滤（collaborative filtering）是一种在推荐系统中广泛使用的技术。该技术通过分析用户或者事物之间的相似性（“协同”），来预测用户可能感兴趣的内容并将此内容推荐给用户。这里的相似性可以是人口特征（性别、年龄、居住地等）的相似性，也可以是历史浏览内容的相似性（比如都关注过和中餐相关的内容），还可以是个人通过一定机制给予某个事物的回应（比如一些教学网站会让用户对授课人进行评分）。比如，用户A和B都是居住在北京的年龄在20-30岁的女性，并且都关注过化妆品和衣物相关的内容。这种情况下，协同过滤可能会认为，A和B相似程度很高。于是可能会把A关注B没有关注的内容推荐给B，反之亦然。

> 发展简史

**电子商务的推荐系统**

最著名的电子商务推荐系统应属**亚马逊网络书店**，顾客选择一本自己感兴趣的书籍，马上会在底下看到一行“**Customer Who Bought This Item Also Bought**”，亚马逊是在“对同样一本书有兴趣的读者们兴趣在某种程度上相近”的假设前提下提供这样的推荐，此举也成为亚马逊网络书店为人所津津乐道的一项服务，各网络书店也跟进做这样的推荐服务如台湾的博客来网络书店。另外一个例子是Facebook的广告，系统根据个人资料、周遭朋友感兴趣的广告等等对个人提供广告推销，也是一项协同过滤重要的里程碑，和前二者Tapestry、GroupLens不同的是在这里虽然商业气息浓厚同时还是带给用户很大的方便。以上为三项协同过滤发展上重要的里程碑，从早期单一系统内的邮件、文件过滤，到跨系统的新闻、电影、音乐过滤，乃至于今日横行互联网的电子商务，虽然目的不太相同，但带给用户的方便是大家都不能否定的。

> 推荐系统如何召回

召回策略主要包含两大类，即**基于内容匹配的召回**和**基于系统过滤的召回**。

* 基于内容匹配的召回

内容匹配即将用户画像与内容画像进行匹配，又分为**基于内容标签的匹配**和**基于知识的匹配**。

例如，A用户的用户画像中有一条标签是“杨幂的粉丝”，那么在他看了《绣春刀2》这部杨幂主演的电影后，可以为他推荐杨幂主演的其他电影或电视剧，这就是**基于内容标签的匹配**。

**基于知识的匹配**则更进一步，需要系统存储一条“知识”——《绣春刀2》是《绣春刀1》的续集，这样就可以为看过《绣春刀2》的用户推荐《绣春刀1》。基于内容匹配的召回较为简单、刻板，召回率较高，但准确率较低（因为标签匹配并不一定代表真的感兴趣），比较适用于冷启动的语义环境。

* 基于协同过滤的召回

如果仅使用上述较简单的召回策略，推荐内容会较为单一，目前业界最常用的基于协同过滤的召回，它又分为基于用户、基于项目和基于模型的协同过滤。

**基于用户（User-based）的协同推荐**是最基础的，它的基础假设是“相似的人会有相同的喜好”，推荐方法是，发现与用户相似的其他用户，用用户的浏览记录做相互推荐。例如，通过浏览记录发现用户一与用户二的偏好类似，就将用户一点击的内容推送给用户二。

**基于项目（Item-based）的协同过滤**中的“项目”可以视场景定为信息流产品中的“内容”或者电商平台中的“商品”，其基础假设是“喜欢一个物品的用户会喜欢相似的物品”计算项目之间的相似性，再根据用户的历史偏好信息将类似的物品推荐给该用户。

**基于模型的协同过滤推荐（Model-based）**就是基于样本的用户喜好信息，训练一个推荐模型，然后根据实时的用户喜好的信息进行预测推荐。

总体来说，基于协同过滤的召回即建立用户和内容间的行为矩阵，依据“相似性”进行分发。这种方式准确率较高，但存在一定程度的冷启动问题。

在实际运用中，采用单一召回策略的推荐结果实际会非常粗糙，通用的解决方法是将规则打散，将上述几种召回方式中提炼到的各种细小特征赋予权重，分别打分，并计算总分值，预测 CTR。

例如，根据内容匹配召回策略，用户A和内容甲的标签匹配度为0.6，同时，根据协同过滤召回策略，应该将内容甲推荐给用户A的可能性为0.7，那么就为0.6和0.7这两个数值分别赋予权重（这个权重可能会根据算法的具体情况来确定），得出总分，用它来预测用户可能点击的概率，从而决定是否返回该结果。





## refer

* [推荐策略中的“召回”](https://www.woshipm.com/pd/2051274.html)




# [Stable Diffusion](https://en.wikipedia.org/wiki/Stable_Diffusion)

Stable Diffusion 是一种基于潜在扩散模型（Latent Diffusion Models, LDMs）的文图生成（text-to-image）模型，由 CompVis、Stability AI 和 LAION 研究人员实现并开源。该模型的核心思想是通过前向扩散过程和反向扩散过程来生成图像，其中前向扩散过程将高斯噪声加入到初始图像中，逐步增强噪声以达到所需的分布状态；反向扩散过程则是在这个过程中去除噪声，从而生成与文本描述相符的图像。

Stable Diffusion is a [deep learning](https://en.wikipedia.org/wiki/Deep_learning), [text-to-image model](https://en.wikipedia.org/wiki/Text-to-image_model) released in 2022 based on [diffusion](https://en.wikipedia.org/wiki/Diffusion_model) techniques. The [generative artificial intelligence](https://en.wikipedia.org/wiki/Generative_artificial_intelligence) technology is the premier product of [Stability AI](https://en.wikipedia.org/wiki/Stability_AI) and is considered to be a part of the ongoing [artificial intelligence boom](https://en.wikipedia.org/wiki/AI_boom).

It is primarily used to generate detailed images conditioned on text descriptions, though it can also be applied to other tasks such as [inpainting](https://en.wikipedia.org/wiki/Inpainting), outpainting, and generating image-to-image translations guided by a [text prompt](https://en.wikipedia.org/wiki/Prompt_engineering). Its development involved researchers from the CompVis Group at Ludwig Maximilian University of Munich and Runway with a computational donation from Stability and training data from non-profit organizations.

Stable Diffusion is a latent [diffusion model](https://en.wikipedia.org/wiki/Diffusion_model), a kind of deep generative artificial [neural network](https://en.wikipedia.org/wiki/Neural_network). Its code and model weights have been released publicly, and it can run on most consumer hardware equipped with a modest GPU with at least 4 GB VRAM. This marked a departure from previous proprietary text-to-image models such as DALL-E and Midjourney which were accessible only via cloud services.

## 相关工具

### [stable-diffusion-webui](https://github.com/AUTOMATIC1111/stable-diffusion-webui)

A web interface for Stable Diffusion, implemented using Gradio library.

### [sd-webui-controlnet](https://github.com/Mikubill/sd-webui-controlnet)

The WebUI extension for ControlNet and other injection-based SD controls.

https://github.com/AUTOMATIC1111/stable-diffusion-webui



# 了解如何避免过拟合，进而能够准确泛化模型外部的数据

https://www.ibm.com/cn-zh/topics/overfitting

在统计学和机器学习中，偏差和方差是两个非常重要的概念。它们都描述了预测模型的错误，但从不同的角度。

偏差（Bias）：描述的是预测值（估计值）的期望与真实值之间的差距。偏差越大，表示预测模型的预测平均值与真实值越偏离，模型越偏离真实数据，也就是说模型欠拟合。

方差（Variance）：描述的是预测值的变化范围，离散程度，也就是该量的离散程度或者说稳定性。方差越大，数据的分布越分散，模型的预测结果对于学习样本的扰动越敏感，模型过拟合。

偏差和方差通常存在一个权衡的关系，这被称为偏差-方差权衡（Bias-Variance Tradeoff）。如果模型的复杂度较低，拟合的能力较弱，可能会出现偏差较大，但方差较小的情况，这被称为欠拟合（Underfitting）。如果模型的复杂度较高，拟合的能力过强，可能会出现偏差较小，但方差较大的情况，这被称为过拟合（Overfitting）。理想的模型应该在偏差和方差之间找到一个好的平衡。


## 什么是过拟合 (overfitting)？

**过拟合**是一个数据科学概念，在统计模型与其训练数据完全拟合时，就会出现过拟合。在这种情况下，遗憾的是，无法对看不到的数据执行该算法，因而违背了算法的目的。通过将模型泛化至新数据，最终我们能够每天使用机器学习算法做出预测，并对数据进行分类。

在构建机器学习算法时，会利用样本数据集来训练模型。然而，当模型在样本数据上训练的时间过长或模型过于复杂时，它就会开始学习数据集中的"噪声"或不相关的信息。当模型记住"噪声"并且与训练集过于紧密地拟合时，模型就会变得**过拟合**，无法很好地泛化至新数据。如果模型无法很好地泛化至新数据，那么就无法执行预期的分类或预测任务。

低错误率和高方差是过拟合的明显标志。为了防止出现这种行为，通常会将部分训练数据集留作"测试集"，用来检查是否存在过拟合。如果训练数据的错误率低，测试数据的错误率高，那么就表示过拟合。

## 过拟合与欠拟合

如果过度训练模型或模型过于复杂导致出现过拟合，那么逻辑上的预防应对措施就是提前暂停训练过程（也称为"早停法"），或者通过消除不太相关的输入来降低模型的复杂性。但是，如果过早暂停或排除太多重要特征，就可能会走向另一个极端 - 模型欠拟合。当模型没有训练足够长的时间，或者输入变量不够显著，无法确定输入变量和输出变量之间的有意义关系时，就会发生欠拟合。

在这两种情况下，模型都无法在训练数据集中确定主导趋势。因此，在泛化到看不见的数据上时，欠拟合的表现也不佳。 但与过拟合不同，欠拟合的模型在预测时的偏差较高，方差较小。这就是偏差方差权衡，在欠拟合的模型转变为过拟合状态时，就会发生这种情况。随着模型不断学习，其偏差会减小，但在过拟合状态下，其方差可能会增加。拟合模型的目标是在欠拟合和过拟合之间找到最佳位置，以便确定主导趋势，并将其广泛应用于新数据集。


# Sora (OpenAI)

## Video generation models as world simulators

https://openai.com/research/video-generation-models-as-world-simulators

We explore large-scale training of generative models on video data. Specifically, we train text-conditional diffusion models jointly on videos and images of variable durations, resolutions and aspect ratios. We leverage a transformer architecture that operates on spacetime patches of video and image latent codes. Our largest model, Sora, is capable of generating a minute of high fidelity video. Our results suggest that scaling video generation models is a promising path towards building general purpose simulators of the physical world.

> 这是一项关于大规模生成模型在视频数据上训练的研究。具体来说，作者在不同持续时间、分辨率和宽高比的视频和图像上，共同训练了基于文本条件的扩散模型。在这个研究中，作者使用了一种基于Transformer架构的方法，该方法在视频和图像的潜在编码的时空块上进行操作。作者提出的最大模型，名为Sora，能够生成高保真度的长达一分钟的视频。
>
>这项研究的结果表明，扩大视频生成模型的规模是一条有前景的道路，有望构建通用的物理世界模拟器。这意味着，通过继续发展和扩大这种方法，我们可能会开发出能够以更高质量和更广泛的应用来生成和模拟视频的模型，从而为各种任务提供更强大的工具。
