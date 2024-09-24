---
layout: post
title:  "一种使用机器学习的角色动画系统"
date:   2024-09-24 05:00:00 +0800
categories: 游戏开发
---

* Do not remove this line (it will not be displayed)
{:toc}


# MotorNerve: A Character Animation System Using Machine Learning (一种使用机器学习的角色动画系统)

Category：Programming 游戏技术

Year Belongs：GDC 2024 (Game Developers Conference)

Speaker(s)：Songnan Li,Yuchen Liao

Company Name(s)：Tencent Games

**Overview**：This presentation will introduce to you **a character animation system** using **machine learning**, dubbed **MotorNerve**, which implements two core character animation functions: **locomotion control** and **transition animation generation**. First, by combining **Motion Matching** and **Learned Motion Matching** technologies, **MotorNerve** achieves high-quality, low-consumption locomotion control while speeding up the tuning process for Learned Motion Matching. Secondly, **MotorNerve** also implements an inhouse **Motion In-Betweening** (`MIB`) algorithm, which uses a variational autoencoder to encode leg movements, significantly improving the foot skating problem of existing methods. This method, a joint work by Zhejiang University and TiMi Studio Group, a Tencent Games studio, has been published in SIGGRAPH 2022. **MotorNerve** applies `MIB` to interactive animation scenarios, generating high-quality transition animations while saving the project costs.

![gdc2024_MotorNerve](/assets/images/202409/gdc2024_MotorNerve.png)

---------

Hey everyone, I’m Yuchen from Tencent Games. My colleague Songnan and I will be sharing with you how we apply machine learning in character animation. Let's get started!

MotorNerve, the title of our presentation, is the name of a character animation system that uses machine learning. We aims to use machine learning technology to help improving animation quality, development efficiency, and runtime performance. (本次分享的主题 MotorNerve 是一个使用机器学习的角色动画系统的名字。我们期望使用机器学习技术来提升动画效果、生产效率和运行性能)

We've got an super talented team, and here they are, we're really thankful for the amazing work they've done. Here, I want to introduce the Timi-J1F1 Studio, where both Songnan and I work for. The MotorNerve technology is gonna be used in our studio's games, like QQ Speed and NFS: Mobile. (我们有一个非常有才华的团队，也非常感谢团队成员的付出。这里我要介绍下天美 J1F1 工作室，是我和松南所属的工作室。MotorNerve 这项技术也会被应用在工作室的产品中例如 QQSpeed 和 NFS:Mobile)

![j1f1](/assets/images/202409/j1f1.png)

## 基于改进的 Learned Motion Matching 的行走动画

Alright, let's dive into our presentation! There will be two parts: The one part is Locomotion animation based on an improved Learned motion matching, and the other part is interactive animation based on our in-house motion in-betweening technology. (好了，让我们深入我们的演讲！演讲将分为两个部分：一部分是**基于改进的 Learned Motion Matching 的行走动画**，另一部分是**基于我们自研的动画间补技术的交互动画**)

Here is the outline of Part 1: Firstly, I will talk about some of our methods and tools based on Motion Matching for animation polish. Then, I will discuss the Learned Motion Matching and Base Motion Matching (or BMM and LMM for short), and explain why we use machine learning in Motion Matching. Finally, I will share some of the work we’ve done for performance optimization. (以下是第一部分的大纲：首先，我将介绍一些基于 Motion Matching 的动画优化方法和工具。然后，我将讨论 Learned Motion Matching 和 Base Motion Matching（或简称为 LMM 和 BMM），并解释为什么我们在 Motion Matching 中使用机器学习。最后，我将分享一些我们为性能优化所做的工作)

As we all know, **Motion Matching** is a pretty mature technology, typically used for **locomotion**(n. 移动). Check out these two demos we made using Motion Matching ,we can see the movements are really smooth. To Achieve this, we've came up with some cool methods and tools to polish the animation. Now, let me give you a quick rundown on how we made it happen. (众所周知，Motion Matching 是一项很成熟的技术，通常用于 Locomotion。这里有两个我们完成的 Locomotion Demo，可以看到动作很连贯流畅。为了实现这样的效果，我们实现了一些方法和工具来打磨动画。接下来我会简单谈谈如何实现他们)

We all know that **Motion Matching** tends to switch from one animation clip to another, which can make the animation seem **jerky**(a. 不平稳的) and lack in smoothness. So, we've came up with some methods in the Motion Matching, like allowing for an increased cost when switching animations, or removing adjacent frames when animation switching, and so on. This helps us avoid unreasonable animation switching and make animation smoother. (我们都知道 Motion Matching 在运行过程中会在不同的动画片段中来回切换，从而影响动画的完整度和连续性。所以我们在 Motion Matching 的算法中增加了一些策略，例如允许增加动画跳转时的 cost，或者跳转时屏蔽附近的帧等等，来避免不合理的动画跳转让整个动画更流畅)

Also, when players control a character, most of the time, there's some **deviation**(n. 偏差) between the animation and the logic's **trajectory**(n. 轨迹). Like in this video, **the yellow line** represents the logic trajectory of character, and **the red one** shows the animation's trajectory, the difference is quite noticeable. So, we came up with a feature we called trajectory warping that automatically adjusts the animation’s trajectory to stick closer to the logic trajectory. We can see **the green one** is the result. (另外玩家操作角色移动的时候，大多数情况下动画与逻辑的轨迹是会有一定误差的。比如这个视频里，黄色的是玩家操作的目标轨迹，红色箭头是 Motion Matching 匹配出来的动画的轨迹，还是偏差比较大的。我们实现了一套动态轨迹修正的功能，可以自动修正动画的轨迹让他贴近玩家操作的目标轨迹，绿色箭头就是结果)

![gdc1](/assets/images/202409/gdc1.png)

On top of that, we’ve came up with some **powerful editor** and **debugging tools** to help us polish animations in no time. On the right video, you'll see our debug tool, which records every frame and displays the details of Motion Matching. It helps us quickly figure out how to make the animations look even better. (另外我们提供了强大的编辑功能和调试工具来帮助我们快速完成动画的打磨工作。右边是我们的调试工具，能够录制每一帧并且显示 Motion Matching 的匹配细节，帮助我们快速分析出让动画表现更好的办法)

So far, we’ve shared how we polish up the animations. **Now, let’s talk about why we use machine learning**. With conventional Motion Matching (or we call it **Base Motion Matching**), we can iterate quickly during development. **But there's a big catch if we use it in the game runtime, what is, high-quality animations require a large number of animation data, leading to a high memory usage, which is usually not a good thing**. That's where machine learning comes in. (前面介绍了我们如何打磨动画表现，接下来让我们来谈谈我们为什么要使用机器学习。使用传统的 Motion Matching（我们称之为 Base Motion Matching）可以进行快速的开发迭代，但如果放在游戏运行时则有一个不可忽视的问题就是：高品质的动画会需要巨大的动画量，从而带来很高的内存占用，这通常是不可接受的)

**Learned Motion Matching** use machine learning to solves this **memory problem** perfectly, but the downside is that it’s hard to fine-tune animations. Whenever you make a small change and want to check the result, you have to wait for the model training, which usually take half a day or even longer. So, what should we do? **A practical way is to use BMM for development and switch to LMM for runtime**. (而基于机器学习的 Learned Motion Matching 则可以很完美的解决这个问题，但 LMM 的缺点是开发困难。每次你简单修改了一些参数并且想看结果都必须等模型训练，通常需要花费数个小时甚至更多，所以我们应该怎么办？一个可行的办法是我们在开发阶段使用 BMM，并且在运行阶段使用 LMM)

![gdc2](/assets/images/202409/gdc2.png)

So, **it’s very important that the motion output of LMM and BMM must be the same**, and we’ve done a lot of work to make sure of that. Check out this video - LMM is on the left, BMM on the right, and I believe you can hardly tell the difference. We've also run some objective tests and subjective experiments, all of which show that motion of LMM is identical with BMM. (所以有一个很重要的事情就是 LMM 和 BMM 的动作表现必须一样，为此我们做了大量的工作。可以看到这个视频，左边是 LMM，右边是 BMM，几乎看不出区别。另外我们也测了一些客观数据以及做了主观双盲实验，都说明我们的 LMM 的表现能对齐 BMM)

![gdc3](/assets/images/202409/gdc3.png)

**Another thing to mention is that the original LMM couldn't handle animation curves and Notify**, that means it is hard to used in game. And we‘ve made it work. Animation curves and notify will be processed automatically in MotorNerve. (另外之前的 LMM 方案无法实现曲线和 Notify 功能，这会导致很难实际应用在游戏中。我们实现了这个功能，能自动处理动画曲线和 Notify)

Well, **that’s talk about the performance**. As we known, **Learned Motion Matching is pretty satisfying when it comes to memory usage, but it's not exactly ideal in terms of CPU consumption, which makes it hard to use in games**. We've improved the inference architecture for LMM by using a frame rate independent stepper and a non-linear strategy. This way, we've significantly boosted performance, and another bonus is that, make it possible for the same model to work with different frame rates. As we can be seen in the video on the right, the models are all trained at 30FPS. The original LMM model cannot work properly at 60FPS, while MotorNerve can. (好了，现在来谈谈性能。Learned Motion Matching 算法虽然在内存方面令人满意，但 CPU 耗时方面不太理想，这也导致了很难应用在游戏项目中。我们对 LMM 的推理架构进行了改进，使用独立于帧率的步进器和非线性策略，使得性能得到了很大提升，并且让同一个模型能够应用在不同的帧率下。可以看到右边的视频中，模型都是在 30FPS 下训练的。原始的 LMM 模型在 60FPS 下无法正常工作，而 MotorNerve 则可以)

![gdc4](/assets/images/202409/gdc4.png)

Besides that, **we've also implemented batch inference optimization**, which can save up to 50% of CPU cost when there are multiple characters. Check out our demo – we managed to keep 60 FPS with 160 characters on a single thread. Pretty cool, huh? (另外我们还实现了批处理推理优化，能够使得多人同屏的情况下节约最高 50% 的 CPU 开销。这是我们的 demo，实现了 160 个角色在单线程的情况下达到 60FPS。很酷对吧？)

![gdc5](/assets/images/202409/gdc5.png)

So, that's it for Part 1, We’ve talked about how to use machine learning to come up with an amazing locomotion animation system, which can achieve smooth, high-quality animations with great performance. Next up, my colleague Songnan will take the stage to talk about the second part, about interactive animations. Stay tuned! (这就是第一部分的内容，我们讨论了如何使用机器学习来打造一个令人惊叹的 Locomotion 动画系统，可以实现流畅、高质量的动画并具有出色的性能。接下来由我的同事李松南为大家介绍第二部分关于交互动画的内容)

![gdc6](/assets/images/202409/gdc6.png)


## 动画间补技术的交互动画

Let’s dive into the second part of this talk, which is about motion in-betweening for motion transitions. It will be easy to understand if you have some experiences in animation and machine learning. (现在，让我们深入探讨第二部分的内容，即可以用于生成过渡动画的**间补技术**。如果您对动画和机器学习有一些经验，那么这将很容易理解)

Here is the outline. Firstly, **I will talk about what is motion in-betweening and why we need it in game development**. Then **I will introduce the classification of motion in-betweening methods, our method belongs to which category, how it works and why it works better than its peers**. Lastly, **I will introduce its applications in our interaction and locomotion systems**. (以下是大纲：首先，我将介绍什么是动画间补以及为什么我们在游戏开发中需要它。然后，我将介绍动画间补方法的分类，我们的方法属于哪个类别，它是如何工作的以及为什么它比其他方法更好。最后，我将介绍它在我们的交互和位移系统中的应用)

So, **firstly, what is motion in-betweening?** In the left image, **the orange poses** are from the key frames, which could be manually set up by an **animator**(n. 动画师). And **those blue poses** are generated by our **motion in-betweening algorithm**. So, simply speaking, motion in-betweening algorithm is a method that can generate the in-between animations given some key frames. **Those key frames is quite sparse, so using linear interpolation will not lead to good quality**. (首先，什么是动画间补？在左侧的图像中，橙色姿势是关键帧，可以由动画师手动设置。而蓝色姿势是由我们的动画间补算法生成的。简单来说，动画间补算法是一种可以根据一些关键帧生成中间动画的方法。**这些关键帧相对较稀疏，因此使用线性插值不会得到很好的质量**)

Nowadays, **motion in-betweening algorithms are mostly based on machine learning**. To generate high quality in-between animations, it needs to learn from a large set of animation data. The video on the right shows the result of our method, which is a joint work between Zhejiang University and Tencent Games, published in SIGGRAPH 2022. The blue poses are generated by our method. You can see that the motion quality is high, and the transitions are smooth. (如今，**动画间补算法大多基于机器学习**。为了生成高质量的中间动画，它需要从大量的动画数据中进行学习。右侧的视频显示了我们的方法的结果，这是浙江大学和腾讯游戏的合作作品，发表于 SIGGRAPH 2022。蓝色姿势是由我们的方法生成的。您可以看到动作质量很高，过渡很平滑)

![gdc7](/assets/images/202409/gdc7.png)

OK. So why we use motion in-betweening in the MotorNerve project? There are at least two scenarios in the video game that we can use motion in-betweening. The first one is the transition for interaction animation, and the second one is the transition for locomotion. (好的。那么为什么我们在 MotorNerve 项目中使用**动画间补**？在视频游戏中，我们可以使用动画间补的至少两种场景。**第一种是交互动画的过渡，第二种是位移动画的过渡**。)

In part 1 of this talk, my colleague introduced **locomotion**. Locomotion is a major part of the character activity. But besides locomotion, a character may also need to do **interaction**, that is to interact with an object in the environment or with another character. To achieve that, normally we need a **transition**, to change the status of the character from locomotion to interaction. We can use Motion In-Betweening to do this transition, as shown in the left figure. (在本演讲的第一部分，我的同事介绍了**位移动画**。**行走**是角色活动的重要组成部分。但除了**行走**之外，角色还可能需要进行**交互**，即与环境中的物体或其他角色进行互动。为了实现这一点，通常我们需要一个**过渡**，将角色的状态从位移转变为交互。我们可以使用动画间补来进行这个过渡)

Secondly, the **locomotion** itself has different status, such as walk, run, sprint, moving towards different directions and so on. We can use Motion In-Betweening to generate the transition for these different locomotion status, as shown in the right figure. I will get into more details later when we discuss the applications. (第二，行走本身具有不同的状态，例如行走、奔跑、冲刺、朝不同方向移动等等。我们可以使用动画间补来生成这些不同行走状态的过渡)

![gdc8](/assets/images/202409/gdc8.png)

First, **let me introduce the classification of motion-in-betweening methods**. I will use **MIB** as a short for **motion in-betweening** from now on. Generally, there are offline MIB methods that can be used in a **DCC tool** to generate animations offline, as shown on the right figure. The input is the current frame `f1` and the target frame `fn`. **Offline methods** generate all the middle frames at once, as shown here, which takes quite a long time normally, so it cannot be used in game in real-time. On the other hand, we have **online methods**, that can generate the middle frames one by one. If the model is efficient enough, it can be used in game in real-time. Our method belongs to this category. It has a relatively low complexity, so it can be used both online in the game or offline in a DCC tool. (首先，让我介绍一下动画间补方法的分类。从现在开始，我将使用MIB作为动画间补的缩写。一般来说，有离线 MIB 方法，可以在 DCC 工具中离线生成动画，如右图所示。输入是当前帧 `f1` 和目标帧 `fn`。离线方法一次性生成所有中间帧，如图所示，这通常需要很长时间，因此无法实时在游戏中使用。另一方面，我们还有在线方法，可以逐帧生成中间帧。如果模型足够高效，它可以实时在游戏中使用。我们的方法属于这个类别。它具有相对较低的复杂性，因此既可以在线在游戏中使用，也可以离线在 DCC 工具中使用)

![gdc9](/assets/images/202409/gdc9.png)

**This is the overview figure of our method. Simply speaking, it takes the current frame, target frame and the differences between these two frames as inputs and generates the feature of the next frame in the latent space. Then it uses a decoder to decode the feature to generate the pose in the next frame. Then the output frame is used as the input frame to repeat this process again and again until the end**. (这是我们方法的概览图。简单来说，它以当前帧、目标帧和这两帧之间的差异作为输入，并在潜在空间中生成下一帧的特征。然后使用解码器解码特征，生成下一帧的姿势。然后将输出帧用作输入帧，重复这个过程，直到结束)

We use a **variational auto encoder** to encode the lower part body motion into the latent space, and then sample in this high motion quality latent space. This is the key factor that makes our method achieve higher quality than other methods, especially in terms of foot skating. We can generate 50% less foot skating than other methods. Limited by the time, I think I cannot explain this complicated figure clearly, especially for the audiences who are not familiar with **deep learning**. If you are interested in the details, please refer to our paper as listed here. (我们使用变分自编码器将下半身的运动编码到潜在空间中，然后在这个高质量运动的潜在空间中进行采样。这是我们的方法比其他方法实现更高质量的关键因素，尤其是在减少脚滑动方面。我们可以比其他方法减少 50% 的脚滑动。受时间限制，我认为我无法清楚地解释这个复杂的图表，特别是对于不熟悉深度学习的观众。如果您对细节感兴趣，请参考我们的论文，如下所列)

![gdc10](/assets/images/202409/gdc10.png)

In MotorNerve, we extent our paper’s method in two aspects. We add **trajectory control** and **frame number prediction**. (在 MotorNerve 中，我们在两个方面扩展了论文的方法。我们增加了**轨迹控制**和**帧数预测**)

For **trajectory control**, we can not only set the pose of the target frame but also make the character move along a specific trajectory. In the left video, we use just one frame and a trajectory to generate this running sequence. (对于**轨迹控制**，我们不仅可以设置目标帧的姿势，还可以使角色沿着特定轨迹移动。在左侧的视频中，我们仅使用一个帧和一个轨迹来生成这个奔跑序列)

For **frame number prediction**, we developed a method to predict the number of frames to be generated given the poses of the current frame and the target frame. Intuitively, if the target frame is far away from the current frame, we should generate more in-between frames, otherwise if the target frame is near the current frame, we should generate fewer in-between frames. Besides the distance, other factors like the character facing direction, the moving velocity, and so on, also affect the number of in-between frames. In the right video, you can see that if we use a fixed number of in-between frames, the motion is unstable, i.e., sometimes it is too slow, sometimes it is too fast, and sometimes it looks like the character is trying hard to pose for a photographer. On the other hand, as shown on the right, if we can predict a reasonable number of in-between frames, which is adaptive to the inputs, the generated motion is more much natural. (对于**帧数预测**，我们开发了一种方法，根据当前帧和目标帧的姿势来预测要生成的帧数。直观地说，如果目标帧与当前帧相距较远，我们应该生成更多的中间帧，否则如果目标帧接近当前帧，我们应该生成较少的中间帧。除了距离之外，其他因素如角色面朝方向、移动速度等也会影响中间帧的数量。在右侧的视频中，您可以看到如果我们使用固定数量的中间帧，运动是不稳定的，即有时过慢，有时过快，有时看起来像角色在努力摆姿势给摄影师拍照。另一方面，如右侧所示，如果我们能够预测出适应输入的合理数量的中间帧，生成的运动更加自然)

Next, let me introduce the applications of MIB. The first application is to generate transition for interactive animation. The transition is between the current pose of the locomotion and the interactive animation to be played. (接下来，让我介绍一下 MIB 的应用。第一个应用是为交互式动画生成过渡。过渡是从当前的行走姿势到即将播放的交互式动画之间的过渡)

When we press a button to start the interaction, the character normally is in a locomotion state, could be in any poses. Typically, we can use motion blend to generate this transition, but the blended motion is quite linear therefore unnatural. There are a lot of foot skating. In terms of programing, you may need to manually set up some entrance points, which is troublesome. Instead, we can use MIB to generate the transition motion. As shown in these videos, it can be used for various character-object interactions, such as sitting on chair, picking up, opening door, opening box and so on. There are less foot skating, and the transition is natural. You can hardly notice which part of the motion is the transition. (当我们按下按钮开始交互时，角色通常处于行走状态，可能处于任何姿势。通常，我们可以使用动作混合来生成这个过渡，但混合的动作往往线性化，因此不自然。会出现很多脚滑动的问题。从编程的角度来看，您可能需要手动设置一些入口点，这很麻烦。相反，我们可以使用 MIB 来生成过渡动作。如视频所示，它可以用于各种角色与物体的交互，例如坐在椅子上、拾取物品、打开门、打开盒子等等。脚滑动较少，过渡自然。您几乎无法注意到哪部分是过渡动作)

In these videos, we use MIB in game in real-time. For complexity, on PC it takes about 1ms to generate one frame. (在这些视频中，我们实时在游戏中使用 MIB。在PC上，生成一帧大约需要1毫秒的时间)

We also implemented our MIB method in the motion builder to generate transition motions offline. This video shows the results of transition from idle to an opening door pose. (我们还在动作构建器中实现了我们的 MIB 方法，以离线生成过渡动作。这个视频展示了从静止到打开门姿势的过渡结果)

The other application of MIB is transition for locomotion, precisely, to generate locomotion that matches trajectory. For locomotion, besides the loop animations, such as idle loop, walk loop, run loop and so on, there are much more animations that are the transitions between these loop animations, to transition the character to different status, such as from idle to walk, from walk to run, or changing the moving directions. (MIB 的另一个应用是**用于行走过渡**，准确地说，是生成与轨迹匹配的行走动作。除了循环动画，如静止循环、行走循环、奔跑循环等，还有更多的动画是这些循环动画之间的过渡，用于将角色过渡到不同的状态，例如从静止到行走，从行走到奔跑，或者改变移动方向)

In motion matching, it is important that these locomotion animations match the control trajectory, otherwise there will be foot skating. During motion capture, the actor try his best to follow the predefined trajectory in the dance card but can only follow the trajectory approximately. So, post processing is needed to finetune these locomotion animations, which is time consuming. As mentioned before, our MIB method can generate character animation that follows trajectory. So, we can use MIB to generate those animations that mismatches the predefined trajectories. (在 Motion Matching 中，这些**位移动画**与**控制轨迹**的匹配非常重要，否则就会出现**脚滑动的问题**。在运动捕捉过程中，演员尽力按照预定的舞蹈动作表来跟随轨迹，但只能近似地跟随轨迹。因此，需要进行后期处理来微调这些位移动画，这需要耗费时间。正如之前提到的，我们的 MIB 方法可以生成与轨迹匹配的角色动画。因此，我们可以使用 MIB 来生成那些与预定轨迹不匹配的动画)

As an experiment, we asked the actor to perform freely on the motion capture stage, rather than strictly following the dance card. Then we used these motion captured animations to train our MIB model. Finally use our MIB model to generate the locomotion animations that matches the control trajectories. The video on the left shows that our DCC tool can generate a bunch of locomotion animations in a short time. Most of these animations have high motion quality. In the right video, we compare using MIB animation and using blending for the pivot motion. As can be seen, the character on the left side has detailed motion such as leg pedaling on the ground. (作为一个实验，我们要求演员在运动捕捉舞台上自由表演，而不是严格按照舞蹈动作表来执行。然后，我们使用这些运动捕捉动画来训练我们的 MIB 模型。最后，使用我们的 MIB 模型生成与控制轨迹匹配的位移动画。左侧的视频显示，我们的 DCC 工具可以在短时间内生成大量的位移动画。其中大多数动画具有高质量的运动效果。在右侧的视频中，我们比较了使用 MIB 动画和使用混合动画进行转向运动的效果。可以看到，左侧的角色具有腿部在地面上踩踏等细节动作)

OK. That’s all for the second part. Here is the take aways. Firstly, MIB is a deep-learning based motion generation method. Secondly, we can use MIB to generate motion transitions for interaction and locomotion. Lastly, we can use MIB directly in game since our MIB method is time-efficient, and we also can use MIB in the DCC tool to generate transition animations offline. We believe that our work is only a start. Machine-learning based motion generation methods will play a more and more import role in game development in the near future. (好的，这就是第二部分的内容。**我们做个总结：首先，MIB 是一种基于深度学习的运动生成方法。其次，我们可以使用 MIB 来生成交互和位移的运动过渡。最后，由于我们的 MIB 方法具有高效的时间性能，我们可以直接在游戏中使用 MIB，还可以在 DCC 工具中使用 MIB 生成离线的过渡动画**。我们相信，基于机器学习的运动生成方法在游戏开发中将在不久的将来发挥越来越重要的作用)

![gdc11](/assets/images/202409/gdc11.png)

# Refer

* https://gdcvault.com/play/1034685/MotorNerve-A-Character-Animation-System

