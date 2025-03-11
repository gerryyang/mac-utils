---
layout: post
title:  "Cursor in Action"
date:   2025-03-11 12:30:00 +0800
categories: 机器学习
---

* Do not remove this line (it will not be displayed)
{:toc}


> Built to make you extraordinarily productive, **Cursor** is the best way to code with AI.

`Cursor` 是 `VS Code` 的一个分支。这使我们能够专注于与 AI 进行编码的最佳方式，同时提供熟悉的文本编辑体验。

# 常用功能

## Cursor Tab

`Cursor Tab` 是本地的自动补全功能。它是一个更强大的 Copilot，能够建议整个差异，并具有特别好的记忆。由自定义模型驱动，`Cursor Tab` 可以：

* 在光标周围建议编辑，而不仅仅是插入额外的代码。
* 同时修改多行。
* 根据您最近的更改和 linter 错误提出建议。

免费用户可以免费获得 2000 个建议。专业版和商业版计划可以获得无限建议。

> 用户界面

当 Cursor 仅添加额外文本时，补全将以灰色文本显示。如果建议修改现有代码，它将作为差异弹出窗口出现在当前行的右侧。

![cursor1](/assets/images/202503/cursor1.png)

![cursor2](/assets/images/202503/cursor2.png)

可以通过按 `Tab` 接受建议，或通过按 `Esc` 拒绝建议。要逐字部分接受建议，请按 `Ctrl/⌘ →`。要拒绝建议，只需继续输入，或使用 `Escape` 取消/隐藏建议。

每次按键或光标移动时，Cursor 将尝试根据您最近的更改提出建议。然而，Cursor 并不总是会显示建议；有时模型预测没有需要更改的内容。

Cursor 可以对当前行上方一行到下方两行进行更改。

> 切换

要打开或关闭此功能，请将鼠标悬停在应用程序右下角状态栏上的“Cursor Tab”图标上。

> 在写评论时 Tab 会妨碍我，我该怎么办？

可以通过进入 `Cursor Settings` > `Tab Completion` 并取消选中“在评论中触发”来禁用评论中的 `Cursor Tab`。


## Cursor Composer

`Composer` 是在编辑器中的 **AI 编码助手**。它帮助探索代码、编写新功能并修改现有代码，而无需离开用户工作流程。使用 `⌘I` 打开它，使用 `⌘N` 创建一个新的 `Composer`。


> 聊天和 Composer 之间有什么区别？

Cursor 聊天帮助您搜索和理解代码。使用它来探索您的代码库、提出问题并获取解释。用户可以使用 `⌘⏎` 搜索代码。

`Composer` 帮助您编写和编辑代码。它提供一个工作区，您可以在其中生成新代码并直接将更改应用到文件中。


## Cursor Chat

`Cursor Chat` 让用户在代码库中提问或解决问题，使用最强大的语言模型，全部在用户的编辑器中进行。

为了使语言模型能够给出良好的答案，它们需要了解与用户的代码库相关的特定信息——上下文。

Cursor 具有多种内置功能，以在聊天中提供上下文，例如自动包含整个代码库的上下文、搜索网络、索引文档以及用户指定的代码块引用。**它们旨在消除与语言模型在代码上工作时所需的繁琐复制粘贴**。

默认情况下，Cursor Chat 位于 AI 面板中，该面板位于用户的主侧边栏的对面。用户可以通过按 `Ctrl/⌘ + L` 切换 AI 面板，打开时将焦点集中到聊天上。要提交用户的查询，请按 `Enter`。

> 用户和 AI 消息

用户消息包含用户输入的文本，以及用户引用的上下文。用户可以返回任何先前的用户消息进行编辑并重新运行查询。这将覆盖之后的任何消息并生成新的消息。

AI 消息是用户选择的 AI 模型生成的响应。它们与之前的用户消息配对。AI 消息可能包含解析的代码块，可以通过[即时应用](https://cursordocs.com/docs/chat/apply)添加到用户的代码库中。

所有用户/AI 消息在同一线程中称为**聊天线程**，**每个聊天线程都保存在用户的聊天历史中**。

> 聊天历史

通过点击 AI 面板右上角的“先前聊天”按钮，或按 `Ctrl/⌘ + Alt/Option + L`，用户可以查看聊天历史。用户可以点击任何聊天线程以返回查看构成该线程的消息，用户还可以通过点击笔图标修改线程的标题，或在悬停在线程上时点击垃圾桶图标删除该线程。

Cursor 线程的标题仅为第一条用户消息的前几个词。

> 默认上下文

默认情况下，`Cursor Chat` 包含当前文件作为上下文。用户可以通过从消息中移除当前文件标签来提交不包含任何上下文的查询。当用户输入时，用户可以在输入框上方的标签中看到将包含的上下文。

> 添加上下文

默认情况下，用户消息将包含用户输入的文本，以及用户引用的上下文。用户可以通过 `@` 符号向每个气泡添加更多自定义上下文，默认情况下，当前查看的文件也将作为上下文在用户消息中使用。

有关更多信息，请参见 [@ 符号](https://cursordocs.com/docs/context/@-symbols/@-files) 页面。

> 聊天中的 AI 修复

修复代码库中的 `linter` 错误的一个方便功能是使用聊天中的 AI 修复。要做到这一点，请在编辑器中悬停在错误上，然后点击出现的蓝色 AI 修复按钮。

此操作的键盘快捷键为 `Ctrl/⌘ + Shift + E`。

> 设置

可以在 Cursor 设置 > 功能 > 聊天 下自定义 Cursor Chat。

这些设置包括：

* 始终在网上搜索答案。这将使 AI 模型在每次查询时浏览网络以获取最新信息。
* 添加聊天淡出动画。这为生成的 AI 消息添加了平滑的动画效果。
* 自动滚动聊天。当在线程底部时，这将自动滚动聊天，以便 AI 生成文本。
* 聊天窗格中的窄滚动条。
* 开始新聊天时显示聊天历史。

> 默认代码库聊天

如果代码库没有被[索引](https://cursordocs.com/docs/context/codebase-indexing)，Cursor Chat 将首先尝试计算几个搜索查询，以便在用户的代码库中进行搜索。为了更好的准确性，建议使用[嵌入搜索](https://cursordocs.com/docs/chat/codebase#embeddings-search)。


> 嵌入搜索

通过[代码库索引](https://cursordocs.com/docs/context/codebase-indexing)，Cursor Chat 可以根据用户的代码库准确生成响应。

在输入消息后按 `Ctrl/⌘ + Enter`，Cursor Chat 会扫描用户的索引代码库，以查找相关代码片段。这通常适合快速包含代码片段，以便在对话中使用。为了更好地控制代码库搜索并提高准确性，用户可以使用 `@codebase`。

> 高级代码库搜索

当使用 `@Codebase` 时，Cursor 代码库聊天会进行更详细的搜索。

有关 `@Codebase` 的更多信息，请[点击这里](https://cursordocs.com/docs/context/@-symbols/@-codebase)。

> 应用

Cursor 的**应用**允许用户快速将聊天中的代码块建议集成到用户的代码中。

要应用代码块建议，用户可以点击每个聊天代码块右上角的播放按钮。这将编辑用户的文件，以纳入 Chat 生成的代码。**由于用户可以在聊天中添加最多的上下文并与模型进行最多的互动，我们建议使用聊天 + 应用来进行更复杂的 AI 驱动代码更改**。

![cursor3](/assets/images/202503/cursor3.png)

一旦用户应用了代码块，用户可以查看差异并**接受或拒绝**更改。用户还可以点击聊天代码块右上角的“接受”或“拒绝”按钮。

`Ctrl/⌘ Enter` 以接受，`Ctrl/⌘ Backspace` 以拒绝。

![cursor4](/assets/images/202503/cursor4.png)


## Cmd K

`Cmd K`，也称为在 Windows/Linux 上的“Ctrl K”，允许用户在编辑器窗口中生成新代码或编辑现有代码。

> 提示栏

在 Cursor 中，我们称用户按下 `Ctrl/Cmd K` 时出现的栏为“提示栏”。它的工作方式类似于聊天的 AI 输入框，用户可以正常输入，或使用 [@ 符号](https://cursordocs.com/docs/cmdk/context/@-symbols) 来引用其他上下文。

> 行内生成

如果在按下 `Ctrl/Cmd K` 时没有选择任何代码，Cursor 将根据用户在提示栏中输入的提示生成新代码。

> 行内编辑

对于就地编辑，用户只需选择要编辑的代码并在提示栏中输入即可。

> 后续指令

在每次生成后，用户可以通过向提示栏添加更多指令来进一步细化提示，然后按 Enter 以便 AI 根据您的后续指令重新生成。

> 默认上下文

默认情况下，Cursor 将尝试查找不同类型的有用信息以改善代码生成，除了您包含的手动 [@ 符号](https://cursordocs.com/docs/context/@-symbols/@-files)。

额外的上下文可能包括相关文件、最近查看的文件等。在收集后，Cursor 根据与用户的编辑/生成的相关性对上下文项进行排名，并将最相关的项保留在大型语言模型的上下文中。


## codebase-indexing (代码库索引)

为了更好、更准确地使用 `@codebase` 或 `Ctrl/⌘ Enter` 获取代码库答案，用户可以索引用户的代码库。在后台，Cursor 为用户代码库中的每个文件计算嵌入，并将使用这些嵌入来提高代码库答案的准确性。

用户的代码库索引将自动与用户最新的代码库更改同步。

用户代码库索引的状态在 `Cursor 设置` > `功能` > `代码库索引` 下。

![cursor5](/assets/images/202503/cursor5.png)

> 高级设置

默认情况下，Cursor 将索引用户代码库中的所有文件。

用户还可以展开**显示设置**部分以访问更多高级选项。在这里，**用户可以决定是否要为新仓库启用自动索引，并配置 Cursor 在仓库索引期间将忽略的文件**，除了用户的 `.gitignore` 设置。

**如果用户的项目中有任何大型内容文件，AI 确实不需要读取，[忽略这些文件](https://cursordocs.com/docs/context/ignore-files)可能会提高答案的准确性**。


> 基本用法

在 Cursor 的 AI 输入框中，例如在 Cmd K、聊天或终端 Cmd K，用户可以通过输入 `@` 符号来使用 @ 符号。弹出菜单将出现，列出建议，并会根据用户的输入自动过滤，仅显示最相关的建议。

用户可以使用上下箭头键在建议列表中导航。用户可以按 `Enter` 选择一个建议。如果建议是一个类别，例如**文件**，建议将被过滤，仅显示该类别中最相关的项目。

![cursor6](/assets/images/202503/cursor6.png)

用户可以使用上下箭头键在选定的 Cmd K @ 符号列表中导航，按 Enter 展开/折叠选定的上下文项。对于文件引用，用户可以使用 `Ctrl/⌘ M` 切换文件读取策略。有关文件读取策略的更多信息，请[点击这里](https://cursordocs.com/docs/context/@-symbols/@-files#cmd-k-chunking-strategy)。




# Tips

## 安装

Cursor 是一款集成了 AI 技术的代码编辑器，支持多种操作系统，包括 Windows、macOS 和 Linux。

> 注册与登录

首次使用 Cursor 时，需要注册一个新账号或使用已有账号进行登录。注册过程简单，只需填写必要的个人信息。登录后，可以创建新项目或打开已有项目。

> 使用功能

Cursor 提供了强大的代码生成和优化功能。用户可以通过输入需求或描述来生成代码，并与代码进行对话以获取反馈和建议。

## 导入扩展、主题、设置和快捷键

可以通过一键将已有的 VS Code 配置导入到 Cursor。导航到 `Cursor Settings` > `General` > `Account`。

## 为什么不使用扩展？

作为一个独立应用程序，Cursor 对编辑器的 UI 有更多控制，从而实现更大的 AI 集成。我们的一些功能，如 `Cursor Tab` 和 `CMD-K`，作为现有编码环境的插件是不可行的。

## Cursor 特定的设置面板

可以通过点击右上角的齿轮按钮，按 `Ctrl/⌘ + Shift + J`，或使用 `Ctrl/⌘ + Shift + P` 并输入 `Cursor Settings` 来打开 Cursor 特定的设置面板。

## 为什么 Cursor 中的活动栏是水平的？

活动栏默认是水平的，以节省聊天空间。如果更喜欢正常的垂直活动栏，可以前往 VS Code 设置，将 `workbench.activityBar.orientation` 设置为 `vertical`，然后重启 Cursor。

## 有关定价信息

请访问 [Cursor Pricing](https://cursor.com/pricing)。Cursor 提供多个订阅层级以满足用户的需求。

* 爱好者
  + 14 天 专业试用（250 次快速 premium 模型使用）
  + 50 次慢速 premium 模型使用
  + 2000 次 完成 使用

* 专业版
  + 每月 500 次快速 premium 模型使用
  + 无限次慢速 premium 模型使用
  + 无限次 完成 完成
  + 每月 10 次 o1+mini 使用

* 企业版
  + 使用信息与 专业版 相同
  + 全组织强制隐私模式
  + 集中团队计费
  + 带有使用统计的管理员仪表板
  + SAML/OIDC 单点登录

> Premium 模型

`GPT-4`、`GPT-4o` 和 `Claude 3.5 Sonnet` 都被视为 **premium** 模型。每次请求 `Claude 3.5 Haiku` 被计为 `1/3` 次 **premium** 模型请求。

> 专业试用

所有新用户均可获得 14 天的专业试用，允许访问所有专业功能和 250 次快速 **premium** 模型请求。在 14 天期满后，未升级的用户将恢复到爱好者计划。

> 快速和慢速请求

默认情况下，Cursor 服务器会尽量为所有用户提供快速 premium 模型请求。然而，在高峰期，快速 premium 信用用尽的用户将被转移到慢速池，这基本上是等待快速 premium 请求可用的用户队列。

这个队列是公平的，Cursor 将尽一切可能保持队列尽可能短。然而，如果需要更多快速 premium 信用且不想等待，可以在[设置页面](https://cursor.com/settings)上添加更多请求。

> 检查您的使用情况

可以在 Cursor 设置 页面上检查使用情况。也可以在 Cursor 应用程序内访问此页面，路径为 `Cursor Settings` > `General` > `Account`，并按“管理订阅”以供专业用户使用，或“管理”以供企业用户使用。

Cursor 使用情况每月重置，基于用户的订阅开始日期。




# Refer

* https://github.com/getcursor/cursor
* https://www.cursor.com/features
* https://www.cursor.com/en
* [Cursor 中文文档](https://cursordocs.com/)

