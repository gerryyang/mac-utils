---
layout: post
title:  "Vue in Action"
date:   2023-06-13 08:00:00 +0800
categories: Web
---

* Do not remove this line (it will not be displayed)
{:toc}


# 预备知识

文档接下来的内容会假设你对 HTML、CSS 和 JavaScript 已经基本熟悉。如果你对前端开发完全陌生，最好不要直接从一个框架开始进行入门学习——最好是掌握了基础知识再回到这里。你可以通过这篇 [JavaScript 概述](https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/A_re-introduction_to_JavaScript)来检验你的 JavaScript 知识水平。如果之前有其他框架的经验会很有帮助，但也不是必须的。


# Vue (渐进式 JavaScript 框架)

Vue (发音为 /vjuː/，类似 view) 是一款用于构建用户界面的 JavaScript 框架。它基于标准 HTML、CSS 和 JavaScript 构建，并提供了一套声明式的、组件化的编程模型，帮助你高效地开发用户界面。无论是简单还是复杂的界面，Vue 都可以胜任。

下面是一个最基本的示例：

``` js
import { createApp } from 'vue'

createApp({
  data() {
    return {
      count: 0
    }
  }
}).mount('#app')
```

``` xml
<div id="app">
  <button @click="count++">
    Count is: {{ count }}
  </button>
</div>
```

上面的示例展示了 Vue 的两个核心功能：

* **声明式渲染**：Vue 基于标准 HTML 拓展了一套模板语法，使得我们可以声明式地描述最终输出的 HTML 和 JavaScript 状态之间的关系。
* **响应性**：Vue 会自动跟踪 JavaScript 状态并在其发生变化时响应式地更新 DOM。


More: https://cn.vuejs.org/guide/introduction.html#single-file-components


# Refer

* https://github.com/yyx990803 (Evan You)
* https://cn.vuejs.org/
* https://cn.vuejs.org/guide/introduction.html