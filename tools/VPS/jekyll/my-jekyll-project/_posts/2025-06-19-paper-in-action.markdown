---
layout: post
title:  "Paper in Action"
date:   2025-06-19 12:30:00 +0800
categories: Papers
---

* Do not remove this line (it will not be displayed)
{:toc}


# AI CodeReview

* [BitsAI-CR: Automated Code Review via LLM in Practice](https://arxiv.org/pdf/2501.15134)


# 代码静态分析

* [Static Analysis](https://dl.acm.org/doi/pdf/10.1145/3486592)

[Stephen C. Johnson](https://en.wikipedia.org/wiki/Stephen_C._Johnson) 开发的 [Lint](https://en.wikipedia.org/wiki/Lint_(software))

> **Lint** is the computer science term for a static code analysis tool used to flag programming errors, bugs, stylistic errors and suspicious constructs. The term originates from a Unix utility that examined C language source code. A program which performs this function is also known as a "linter" or "linting tool".

例如：C++ 中使用的 [Cpplint](https://en.wikipedia.org/wiki/Cpplint)，Cpplint 是一个 Python 编写的基于 Google 代码规范的检测工具。它只是一个代码风格检测工具，其并不对代码逻辑、语法错误等进行检查。

如何减少误报？莱斯定理 [Rice's theorem](https://en.wikipedia.org/wiki/Rice%27s_theorem)

In computability theory, Rice's theorem states that all non-trivial semantic properties of programs are undecidable. A semantic property is one about the program's behavior (for instance, "does the program terminate for all inputs?"), unlike a syntactic property (for instance, "does the program contain an if-then-else statement?"). A non-trivial property is one which is neither true for every program, nor false for every program.

对于计算机程序（更精确地说，是图灵机）而言，任何关于程序行为（语义）的、非平凡的（non-trivial）属性，都是不可判定的（undecidable）。

**非平凡属性**：指这个属性不是所有程序都具备的（恒真），也不是所有程序都不具备的（恒假）。平凡属性是可判定的（直接回答“是”或“否”就行）。

**不可判定**：指不存在一个通用算法（一个能处理任何程序的程序），能够精确地、总是正确地判断任意给定的程序是否具有该属性。

计算机程序的行为极其复杂且不可预测。你不可能编写出一个“万能检查器”，让它只通过阅读任何程序的源代码，就能百分之百可靠地回答出关于这个程序运行时行为（比如它会不会出错、算得对不对、会不会停）的任何非平凡问题。 这是计算理论中一个非常基础且重要的不可行性结果，深刻影响了程序语言理论、软件工程和形式化方法的发展。






