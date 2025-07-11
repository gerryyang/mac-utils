---
layout: post
title:  "一种使用机器学习的角色动画系统"
date:   2024-09-24 05:00:00 +0800
categories: [Game, ML]
---

* Do not remove this line (it will not be displayed)
{:toc}

> 机器学习在游戏的设计、开发、运维阶段都可以找到用武之地，但在技术落地过程中会遇到一些困难。比较常见的困难有两个，一个是**训练数据不足**，另一个是**复杂度较高**，而这两方面的问题其实都有各自的解法，前者可以使用迁移学习和相对小规模的落地场景数据对相关大数据训练出的模型做调优，后者有越来越普及的 AI 硬件加持等等。相信机器学习在游戏行业中的应用会变得越发普及，并帮助我们更快速地打通虚拟世界与现实世界的屏障。


# 背景知识

## 动画

### 动作控制

 游戏中会有很多动态角色，而这些动态角色最常做的动作是位置移动，也就是从一个地点移动到另外一个地点。我们可以使用手柄、键盘等外设来控制角色的位置移动。大多数游戏的动作控制方法是状态机[^1]，状态机的优点是可以给用户非常快速的反馈，缺点是随着游戏规模的增加会变得越来越复杂，难以维护[^2]。在2016年的 GDC 上，育碧介绍了一个新的动作控制方法 Motion Matching[^3]，它降低了动画系统的维护难度，同时也提高了动画的生成质量。这个方法首先在游戏《荣耀战魂》中落地，后来被应用在诸如《The Last of Us Part II》、《Control》等写实类风格的游戏当中。Motion Matching 的主要问题是需要把动画数据放在内存中，并且要在这些动画数据中做实时的搜索，所以动画的数据量不能太多（几百 MB 的级别），否则会占用太多内存，搜索也会变慢。基于机器学习的动作控制方法可以在保留 Motion Matching 易用性、高质量的同时，减少对存储的占用。我们可以使用 GB 级别的动画数据集，训练出一个 MB 级别的深度模型，用这种方式自动去除动画数据中可能存在的冗余，并将大量的动画信息“压缩”在深度模型参数中。由于浓缩了大量的动画信息，基于机器学习的方法可以生成更加富于变化的动作。下面的视频来自一个基于机器学习的动作控制方法 Learned Motion Matching[^4]，可以看到这个方法生成的动作质量很高，同时，它的内存占用只有使用相同动画数据的 Motion Matching 方法的三十五分之一。当然，基于机器学习的动作控制方法也有自己的问题，比如复杂度通常比传统方法高、可控性比传统方法差等。

 上面提到动作控制方法属于“**行为学控制**”（kinematic control），也就是直接通过控制关节的位移、旋转来让角色动起来。另一类动作控制方法叫做“**动力学控制**”（dynamic control ），具体实现与我们在真实世界中控制机器人很类似，需要使用**物理控制器**对关节施加内力，**并综合角色受到的外力来进行物理仿真**，进而驱动角色的运动。**使用动力学控制可以让我们的角色更好地与环境互动，产生更逼真的动作**，比如与环境中物体的碰撞、角色在开车或攀爬时身体由于惯性的自然晃动等。传统的方法一般是在**行为学控制**的基础上，通过对部分关节添加动力学控制来实现角色骨骼的物理仿真效果。对全身骨骼做物理仿真通常只有在角色死掉的情况下才会使用，因为用物理控制器让角色用“拟人”的方式移动并不容易，尤其是对于双足角色，即使像波士顿动力 Atlas 这样灵活的双足机器人，它们的动作对游戏来说也还不够“拟人”。近些年，越来越多的工作开始使用**强化学习来训练物理控制器**，这些物理控制器可以控制全身骨骼，让角色使用拟人的动作来完成指定的任务。**角色的物理动画一直被认为是动画技术的未来，机器学习让我们离未来更近了一步**。

[^1]: UE 中的状态机: https://docs.unrealengine.com/5.1/zh-CN/state-machines-in-unreal-engine/
[^2]: Daniel Holden, Character Control with Neural Networks and Machine Learning, GDC2018.
[^3]: Kristjan Zadziuk, Motion Matching, The Future of Games Animation...Today, GDC 2016.
[^4]: Introducing Learned Motion Matching, https://montreal.ubisoft.com/en/introducing-learned-motion-matching/

### 过渡与融合

在游戏或过场动画中通常会涉及到一段位移动画向一段交互动画的“过渡”（或称为衔接、切换），比如从走路过渡到坐椅子，从跑步过渡到开门等等。传统的过渡方法使用镜头切换或者一个固定的过渡位置。镜头切换会破环画面的连贯性，而固定的单一过渡位置会让动作看起来单调，缺乏变化。GDC 上的这个讲座[^5]介绍了一种使用多过渡点的方式，效果不错但开发和调试都会比较麻烦。使用机器学习的方法[^6]，我们可以进一步将多过渡点拓展为一片“过渡区域”，在这个区域内的任意位置都可以开启过渡，这样可以提高动作的连贯性。同时，这样的算法只需要前一段动画的末尾几帧和后一段动画的前几帧作为输入，所以使用起来非常方便。我们与浙大在2022年的 SIGGRAPH 上发表了一个使用机器学习生成过渡动画的方法[^6]，下面的这个视频展示了这个方法的效果。除了到交互动画的过渡，我们还利用这个方法来生成各种位移动画之间的过渡，来优化动作控制算法的表现。

除了过渡，我们还可以对多段动画做“融合”来生成新的动画。比如，融合“走”和“跑”来生成速度介于走跑之间的动画；在射击模式下融合几段不同脚步移动方向的动画，来实现任意方向的脚步移动。这些周期性动画的融合相对简单，但对于一些复杂的情况，比如下面的视频中不同身体部位的动作融合，基于机器学习的方法效果要优于传统方法。

[^5]: Almudena Soria, Animation Prototyping in 'Uncharted 4', GDC 2017.
[^6]: Xiangjun Tang, et al., Real time Controllable Motion Transition for Characters, SIGGRAPH 2022.

### 动作捕捉与动画编辑

动作捕捉在影视、动画、游戏、机器人、体育训练等场景中被大量使用。在游戏中，我们经常使用动作捕捉来快速生产写实的动画。动作捕捉的方法有很多，从机械动捕、电磁动捕，到光学动捕、惯性动捕，再到低成本的计算机视觉动捕。其中光学动捕的精确度高（毫米、亚毫米），标记点（marker）的放置灵活，所以是游戏行业中经常被使用到的动捕方法。但由于自遮挡、快速运动等原因，一些标记点的位置会出现较大的误差，甚至会与周边的标记点混淆。机器学习可以帮助我们对标记点的位置信息做降噪处理，恢复出标记点的正确位置。下面的视频来自Daniel Holden在GDC 2019上的演讲[^7]，可以看到使用机器学习做降噪处理的显著效果。

**除了降噪，动画师还经常需要对动捕后的数据做离线的编辑**，比如修改它的移动速度、起止时刻的角色位置和角色朝向等。在游戏中，我们也可能需要对动画做实时的编辑，比如控制 NPC 的脸朝向主角、控制主角的手伸向掉落在地面的物体等等。这些场景下我们会用到 **Inverse Kinematic**（IK），也就是通过手、脚或头关节在全局坐标系（世界坐标系）中的变换（包括位置和旋转）来反推这些关节以及其他需要联动的关节在局部坐标系（父关节坐标系）中的变换。为了使编辑出的动画更真实，我们会尽量使用更多的联动关节。在 Full Body IK 中，所有身体关节都会参与到 IK 过程。而 Full Body IK 可以使用机器学习来实现，效果要优于传统方法。

除了使用 IK 来编辑手、脚、头的位置外，我们还可以使用机器学习在更高的语义层级上对动画做编辑。比如对动作的风格做转换，将男性风格的动画转换成女性风格，将中年人风格的位移动画转换为老人或者小孩的风格等。

[^7]: Daniel Holden, A New Era of Performance Capture with Machine Learning, GDC2019.

### 剧情对话与多模态动画生成

在剧情动画中通常会有很多对话场景，会涉及到角色嘴唇、舌头、目光、眨眼、表情、头部和身体的动画。对于追求高质量的镜头，这些动画可以通过对演员全身的动作捕捉来实现。但为了节约成本和开发时间，对于一些并不十分重要的剧情，只有演员的语音会被录制下来，甚至对语音的录制也可能会被文本转语音技术（Text To Speech）替代。机器学习可以通过语音来生成动画。**在2019年的GDC上，来自育碧的 Francois Paradis 介绍了《刺客信条：奥德赛》中使用的程序化生成剧情对话系统**[^8]，其中机器学习被用来定位英语语音中的音素，因为音素与嘴型有对应关系，所以可以被用来生成嘴型动画。对于非英语的语音，如法语、西班牙语、日语，《刺客信条：奥德赛》使用了 Ubisoft La Forge 开发的基于机器学习的语音转动画方法，这个方法可以直接将语音映射成嘴型动画，下面的视频展示了它的效果。类似的技术在 IEG 的 Tech Future 程序化剧情生成项目中也有被实现。

在《刺客信条：奥德赛》中，除了嘴型动画，其他身体部位的动画并没有使用机器学习来生成，而是使用人工标签和剧本关键词，在动画数据库中搜索最相关的片段。在学术界已经有一些通过语音来生成肢体动作的研究，生成的动作与语音的节奏搭配度很高，但动作细节以及生成内容的可控性还有待提高。

通过语音来生成嘴型、肢体动画是多模态动画生成技术中的一种。除此之外，其他的多模态动画生成技术还有很多，比如用音乐来生成舞蹈、用文本来成动画等。随着这些技术的成熟，相信它们在游戏中都可以找到应用场景。

[^8]: Francois Paradis, Procedural Generation of Cinematic Dialogues in Assassin's Creed Odyssey, GDC2019.


## 物理仿真

除了使用手工关键帧、动作捕捉等方法外，我们还可以使用“物理仿真”让游戏中的对象动起来。比如奔驰在道路上的车辆、被子弹击碎的玻璃、飘荡的裙摆、流动的河水等等，这些对象的动态效果都可以使用物理仿真来生成。

**按照仿真对象的不同，物理仿真大体可以分为刚体仿真、柔性体仿真和流体仿真**。它们使用到的数学建模方式不尽相同，对应的解算方法也各有千秋。这些解算方法的效果和复杂度差异较大，效果更好的方法通常运行速度也更慢。我们可以使用复杂度高但效果更准确的物理仿真方法来为机器学习生成训练数据，再用训练出来的模型模拟解算过程，这样就可以实现物理仿真若干个数量级的加速。


## 建模与渲染

游戏中常用“**网格**（Mesh）”来**表达形状**，比如三角形网格、四边形网格等，这些网格虽然自身简单，但组合在一起就可以表达非常复杂的形状。**为了对游戏对象进行渲染，除了需要对形状进行建模外，我们还需要对游戏对象的材质以及环境光照进行建模**。为了实现真实的渲染效果，我们通常会使用”图像”来表达材质和光照中的一部分重要信息。基于机器学习的建模与渲染方法，很多会使用神经网络来隐式的表达形状、材质、光照信息，并在神经网络的推理过程中实现渲染。由于表示方式和渲染方式的差异，如果要将这样的基于神经网络的方法融合到当代的游戏渲染管线当中，需要付出的转换成本会是比较高的。另有一类基于神经网络的方法，使用可微渲染来显式地重建出用网格表达的形状以及用图像表达的材质和光照，这样的重建结果就可以直接在游戏引擎里使用。这类方法还可以被用来减少网格模型的面数，从而达到降低渲染耗时的目的。

除了上述同时对形状、材质、光照进行建模的方法外，更多的方法**单独对其中的一部分属性**进行建模，比如在已知其他属性的前提下单独重建材质、单独重建光照等。

**高质量的渲染方法同时也有很高的计算复杂度，所以机器学习在渲染过程中的另一个应用场景是降低渲染复杂度**。英伟达在这方面有很多成功的实践，比如用深度学习来做超分和插帧[^9]，从而提高渲染的分辨率和帧率，同时消除锯齿；使用基于机器学习的辐照度缓存(Irradiance Caching)来加速路径追踪渲染(Path Tracing)等。

[^9]: NVIDIA DLSS 3: AI-Powered Performance Multiplier Boosts Frame Rates By Up To 4X, https://www.nvidia.com/en-us/geforce/news/dlss3-ai-powered-neural-graphics-innovations/


## PCG

育碧的程序化生成剧情对话系统，它是程序化内容生成（**Procedural Content Generation**, `PCG`）技术的一个例子。除了用来生成剧情对话相关的内容（动画、角色占位、相机位置、灯光等），我们还可以用程序化的方式生成更多类型的内容，比如关卡、地形、水体、道路、建筑、植被、角色、纹理、音乐、语音、美术原画、对话文本、叙事文本等等。传统的 PCG 技术大多是基于语法规则、随机信号、搜索、解算等方法。基于机器学习的 PCG 方法在实际游戏中的应用目前还不多见，但随着游戏数据的不断积累和 AIGC 技术的升温，机器学习在游戏内容生成中起到的作用应该会日渐凸显，尤其在 3A 大世界游戏的开发中想必会发挥越来越大的作用。

## AI

游戏 AI 是目前机器学习在游戏行业中应用最广泛的场景。从 2016 年 AlphaGo 在围棋上超越人类顶级玩家后，业界对基于机器学习尤其是基于强化学习的游戏 AI 热情持续升温，应用范围不断扩展：从 2D 雅达利类型游戏扩展到 3D 大世界类型游戏；从单智能体 AI 扩展到多智能体 AI；从棋牌游戏扩展到 RTS（星际争霸）、MOBA（王者荣耀、Dota2）、格斗（金属对决）、竞速（QQ飞车）、FPS（穿越火线、使命召唤），再到研发通用游戏 AI；游戏 AI 的作用也从对抗、陪玩，扩展到寻路、关卡 bug 测试、数值平衡测试等。相较于与真实世界，游戏中的虚拟世界是强化学习更好的试验场，游戏AI可以向人类玩家学习，也可以在不断试错中自我学习。由于物理仿真技术的拟真度越来越高，在虚拟世界中学习到的经验还可以反哺到真实世界中，加速真世界的智能体研发步伐。同时，我也非常期待数字人在虚拟世界中大放异彩，在大世界游戏中成为玩家与虚拟世界的重要交互渠道。


## 网络同步

在网络游戏中，我们需要互相同步每个客户端以及服务器上的游戏对象状态。常用的网络同步方法包括状态同步、帧同步、快照同步[^10]等。由于所处地域的差异，不同客户端以及服务器之间存在着网络延时。对于一些容易预测的对象（如车辆），我们可以使用外推的方法通过分析这些对象的历史状态，来预测它们在未来时刻的状态，从而降低网络延时的影响。基于机器学习的方法可以从大量过往数据中学习，减少对手动调参的依赖，提升开发效率。根据我们的经验，以车辆位置同步为例，相较于传统方法如 Projective Velocity Blending[^11]，基于机器学习的方法在高延时的情况下优势明显。

[^10]: 网络游戏的架构基础 (Part 2) GAMES104-现代游戏引擎：从入门到实践 https://www.bilibili.com/video/BV1HN4y157Zq/
[^11]: Matt Delbosc, Replicating Chaos: Vehicle Replication in Watch Dogs 2, GDC 2017.


## 音频

目前基于机器学习的音频技术在游戏行业中的应用还很少见，但很多都已经展现出了巨大的潜力。比如文本转语音技术可以用来为 NPC 配音，音色转换技术可以让一位配音演员为游戏中的所有角色配音；音乐 AIGC 技术可以为游戏生成背景音乐，音乐编辑技术可以改变背景音乐的节奏来适配游戏情节的紧张度；语音识别技术可能会被大规模应用在 AR/VR 游戏中的人机交互当中等等。


## 研效

育碧在尝试结合机器学习与搜索的方法来预测代码提交中是否存在 Bug 并推荐 Bug 的解决方案[^12]。Uber使用传统的机器学习方法预测一个commit或多个commits组合的构建成功概率，从而提高大规模并行提交情况下的代码构建效率，保证主干的持续可用状态。我们还可以让大语言模型 Large language models (LLMs) [^13] [^14] 来帮助程序员写代码（非常期盼这项技术大的大规模应用，提高咱程序员们的生活质量）。

[^12]: Mathieu Nayrolles, et al., CLEVER: Combining Code Metrics with Clone Detection for Just-In-Time Fault Prevention and Resolution in Large Industrial Projects, IEEE/ACM 15th International Conference on Mining Software Repositories, 2018.
[^13]: ChatGPT, https://openai.com/blog/chatgpt/
[^14]: CodePolit, https://github.com/features/copilot

## 运营

在运营方面，游戏与其他互联网娱乐方式存在很多共性。比如都需要做推荐，游戏平台向玩家推荐他们可能感兴趣的游戏，或者游戏商城向玩家推荐角色、装备、皮肤；做内容的审核保证合规；做舆情分析，获取游戏在玩家中受欢迎的程度；做二次内容生成（如精彩游戏片段集锦）投放到流量平台从而提升游戏热度。上面提到的这些运营相关的活动经常会使用到基于机器学习的方法。除此以外，我们还可以用机器学习的方法来做玩家匹配、做反作弊、甚至做游戏直播解说等等。




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

