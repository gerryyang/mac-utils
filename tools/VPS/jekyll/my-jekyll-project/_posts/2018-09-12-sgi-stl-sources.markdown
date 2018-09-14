---
layout: post
title:  "SGI STL Sources 解读"
date:   2018-09-12 22:46:00 +0800
categories: C++
---
STL的实现版本百花齐放，`SGI`(Silicon Graphics Computer Systems, Inc.)版本的STL声名最盛，其被纳为GNU C++标准程序库，任何人皆可以下载GNU C++编译器，从而获得整份STL源代码。学习STL的实现细节，可以得到：

* 通过代码向世界一流的程序员学习。
* 了解各种被广泛运用的数据结构和算法。
* 在代码中如何实现可复用(reusability)。

# STL六大组件

1. 容器(containers)：各种数据结构，如vector, list, deque, set, map用来存放数据。

> 从实现角度看，STL容器是一种class template

2. 算法(algorithms)：各种常用算法，如sort, search, copy, erase等。

> 从实现角度看，STL算法是一种function template

3. 迭代器(iterators)：扮演容器与算法之间的胶合剂，是所谓的“泛型指针”。共有5种类型，以及其他衍生变化。所有STL容器都附带有自己专属的迭代器。只有容器的设计者才知道如何遍历自己的元素。原生指针(native pointer)也是一种迭代器。

> 从实现角度看，迭代器是一种将 operator*, operator->, operator++, operator--等指针相关的操作予以重载的class template

4. 仿函数(functors)：行为类似函数，可作为算法的某种策略。

> 从实现角度看，仿函数是一种重载了 operator() 的class或class template。一般函数指针可视为狭义的仿函数

5. 配接器(adapters)：一种用来修饰容器或仿函数或迭代器接口的东西。STL提供的queue和stack，虽然看似容器，其实只能算是一种容器配接器。因为它们的底部完全借助deque，所有操作都由底层的deque供应。

> 改变container接口者称为container adapter; 改变iterator接口者称为iterator adapter; 改变functor接口者称为function adapter

6. 配置器(allocators)：负责空间配置与管理。

> 从实现角度看，配置器是一个实现了动态空间配置，空间管理，空间释放的class template

![stl](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201809/stl.jpeg)









# Refer

1. [The Annotated STL Sources(using SGI STL) by 侯捷]




