---
layout: post
title: "My App in Action"
date:   2025-03-16 01:38:00 +0800
categories: 杂记
---

* Do not remove this line (it will not be displayed)
{:toc}



# 开发环境

1. MacBook Pro, Apple M1 Pro, 32GB, MacOS Sonoma
1. GoLang 1.24.1
2. DeepSeek / 豆包 / 混元
3. Cursor 1.96.2 (claude-3.7-sonnet-thinking)


# 思路: LLM-News

## 原始需求描述

我是一名游戏服务器程序员，对服务器相关技术比较了解，包括 C++ / GoLang 等编程语言，并具备一定的问题分析和解决能力。目前对 AGI 技术比较感兴趣，为了提高自己的技术想实时关注 LLM 或 AI 当前最新好用的开源工具，技术论文，技术文章，比较热门的分享文章。目前的想法是实现一个自动化信息采集系统。数据源主要包括 Github 或其他一些比较有价值的权威网站。通过自动化实时信息采集，请用 GoLang 实现一个这样的服务网站帮助我获取这些信息。另外，我想将此系统在 Github 上开源，请符合开源代码的基础规范。

1. 定期抓取 GitHub Trending 页面，通过关键词过滤（LLM、AGI、Agent等）获取实时热门仓库。
2. 获取比较权威的 Top10 论文，并生成一些摘要信息。

## 更符合 Prompt Engineering 的需求描述

**角色定位：**

资深后端开发工程师（游戏服务器方向），寻求AGI领域技术升级路径

**技术栈：**

* 核心语言：C++/GoLang
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

打开服务页面 http://localhost:8081

![myapp6](/assets/images/202503/myapp6.png)

在当前基础上进一步功能优化：

![myapp10](/assets/images/202503/myapp10.png)

![myapp12](/assets/images/202503/myapp12.png)

![myapp11](/assets/images/202503/myapp11.png)

构建：

![myapp13](/assets/images/202503/myapp13.png)

运行：

![myapp14](/assets/images/202503/myapp14.png)

![myapp15](/assets/images/202503/myapp15.png)

停止：

![myapp16](/assets/images/202503/myapp16.png)

## 配置 Nginx 反向代理

需求描述：

在 Linux 上部署实现一个 Nginx 反向代理服务，反向代理的配置为：1. Nginx 监听 80 端口 2. 将 www.gerryyang.com 转发到 8080 端口 3. 将 llmnews.gerryyang.com 转发到 8081 端口。请完善相关的功能实现，并添加 README.md 使用说明文件。还需要提供下载安装 Nginx 可执行程序的脚本，可以将反向服务跑起来。

![myapp17](/assets/images/202503/myapp17.png)



## 优化建议

基于当前实现的功能和状态，有以下几点优化建议：

* 性能优化
  + 懒加载和分页: 当仓库数量变多时，可以实现懒加载或分页功能，避免一次性加载所有数据造成页面卡顿
  + 缓存优化: 在客户端缓存已过滤的结果，避免重复过滤相同条件的数据
  + 图片优化: 如果包含仓库图片，可以实现图片懒加载和压缩

* 用户体验改进
  + 过滤器记忆功能: 记住用户最近使用的过滤条件，下次访问时自动应用
  + 响应式设计增强: 进一步优化移动端显示，确保在小屏幕设备上也有良好的体验
  + 深色模式: 添加深色模式切换功能，减轻夜间使用时的视觉疲劳
  + 热门标签推荐: 基于当前趋势动态生成过滤标签推荐

* 功能扩展
  + 用户自定义过滤器: 允许用户创建和保存自己的自定义过滤条件组合
  + 数据导出功能: 提供导出过滤结果为CSV或JSON的选项
  + 订阅功能: 允许用户订阅特定模型或类别的更新，通过邮件通知
  + 搜索功能强化: 添加高级搜索功能，支持组合条件搜索和正则表达式

* 代码质量提升
  + 代码重构: 将主要的过滤逻辑模块化，提高可维护性
  + 单元测试: 为过滤功能添加单元测试，确保不同过滤条件下的正确性
  + 错误处理改进: 添加更健壮的错误处理和恢复机制
  + 性能监控: 添加前端性能监控，收集用户实际使用中的性能数据

* 数据质量改进
  + 增强模型关联识别: 改进对模型相关仓库的识别算法，减少误判
  + 关联数据丰富: 增加仓库与研究论文之间的关联，展示实现了特定论文的仓库
  + 社区热度指标: 添加更多社区互动指标，如Issue活跃度、贡献者数量等
  + 更新频率优化: 根据仓库活跃度动态调整抓取频率

通过以上优化，LLM News 平台将能提供更高效、更个性化的用户体验，同时更好地满足 AI 研究人员和开发者快速获取和筛选相关信息的需求。


# 思路：小游戏

一个简单的小游戏，比如：1024，贪吃蛇






# Refer

* [提示工程指南](https://www.promptingguide.ai/zh)




