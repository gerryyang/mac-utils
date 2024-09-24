---
layout: post
title:  "Machine Learning in Action"
date:   2024-02-23 17:00:00 +0800
categories: 机器学习
---

* Do not remove this line (it will not be displayed)
{:toc}


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
