---
layout: post
title: "My App in Action"
date:   2025-03-16 01:38:00 +0800
categories: 杂记
---

* Do not remove this line (it will not be displayed)
{:toc}


# 思路: LLM-News

## 原始需求描述

我是一名游戏服务器程序员，对服务器相关技术比较了解，包括 C++ / GoLang 等编程语言，并具备一定的问题分析和解决能力。目前对 AGI 技术比较感兴趣，为了提高自己的技术想实时关注 LLM 或 AI 当前最新好用的开源工具，技术论文，技术文章，比较热门的分享文章。目前的想法是实现一个自动化信息采集系统。数据源主要包括 Github 或其他一些比较有价值的权威网站。通过自动化实时信息采集，请用 GoLang 实现一个这样的服务网站帮助我获取这些信息。另外，我想将此系统在 Github 上开源，请符合开源代码的基础规范。

1. 定期抓取 GitHub Trending 页面，通过关键词过滤（LLM、AGI、Agent等）获取实时热门仓库。
2. 获取比较权威的 Top10 论文，并生成一些摘要信息。

## 更符合 Prompt Engineering 的需求描述

**角色定位：**

资深后端开发工程师（游戏服务器方向），寻求AGI领域技术升级路径

**技术栈：**

* 核心语言：C++/GoLang/Python
* 脚本能力：Bash/Python
* 系统能力：分布式系统/高并发处理/自动化运维

**需求场景：**

构建智能化的 AGI 技术动态追踪系统，需满足：

* 实时获取AI领域高质量开源项目动态
* 追踪顶尖学术论文进展
* 自动化处理+结构化呈现

**功能规格：**

模块一：开源项目监控

+ 数据源
  - GitHub Trending
  - Papers with Code
+ 采集策略
  - 频率：每2小时增量抓取
  - 关键词：LLM, AGI, Agent, RLHF, Multimodal, Diffusion
  - 过滤维度
    1. Stars增长率 > 50/day
    2. 最近提交 < 7天
    3. 含完整文档/案例
+ 输出格式

``` json
{
  "project_name": "",
  "description": "",
  "tech_stack": [],
  "trend_metrics": {
    "stars_24h": 0,
    "forks_24h": 0
  },
  "relevance_score": 0-1
}
```

模块二：学术论文追踪

+ 数据源
  - arXiv（cs.CL, cs.AI）
  - ACL Anthology
  - Google Scholar Top Publications

+ 处理流程
  - 每日获取各平台 Top20 论文
  - 基于 citation velocity 筛选 Top10
  - 摘要生成要求
    1. 技术新颖性（0-5分）
    2. 实现复现难度评估
    3. 核心公式/架构图示提取

+ 输出样例

``` markdown
## [论文标题]
**评分**：★★★★☆ (4.2/5)
**核心贡献**：
- 创新点1
- 创新点2
**关键技术**：
```python
# 关键代码片段
```

**系统架构约束：**

1. 响应延迟：从采集到呈现 < 15分钟
2. 存储方案：时序数据库 + 向量检索
3. 异常处理：网站反爬策略自适应
4. 安全要求：遵守 robots.txt 协议


**预期交付物：**

1. 可扩展的分布式爬虫框架
2. 智能过滤算法模块
3. 自动化摘要生成
4. 可视化仪表盘（含趋势分析）


## 使用 Cursor 编码

提交需求开始编码：

![myapp1](/assets/images/202503/myapp1.png)

最大自动化执行次数限制：

![myapp2](/assets/images/202503/myapp2.png)

自动纠错：

![myapp3](/assets/images/202503/myapp3.png)

![myapp4](/assets/images/202503/myapp4.png)

问题全部解决后，Cursor 会总结所有实现功能的摘要，以及 Next Steps 下一步计划：

![myapp5](/assets/images/202503/myapp5.png)

运行服务：

![myapp7](/assets/images/202503/myapp7.png)

打开服务页面 http://localhost:8081：

![myapp6](/assets/images/202503/myapp6.png)


# 思路：贪吃蛇

一个简单的小游戏，比如：贪吃蛇



# 辅助 AI 工具

1. DeepSeek / 豆包 (提供思路)
2. Cursor (编码)



# Refer

* [提示工程指南](https://www.promptingguide.ai/zh)




