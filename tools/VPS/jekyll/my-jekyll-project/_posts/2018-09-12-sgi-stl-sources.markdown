---
layout: post
title:  "SGI STL Sources 解读"
date:   2018-09-12 22:46:00 +0800
categories: C++
---
STL的实现版本百花齐放。`HP`版本是所有STL实现版本的始祖。每一个HP STL头文件都有一份声明，允许任何人免费使用，拷贝，修改，传播，贩卖整份软件及其说明文件，唯一需要遵守的是，必须在所有头文件中加上HP的版本声明。`SGI`(Silicon Graphics Computer Systems, Inc.)版本的STL声名最盛，其继承自HP版本，所以它的每一个头文件都有HP的版本声明，SGI版本被GCC采用。学习STL的实现细节，可以得到：

* 通过代码向世界一流的程序员学习。
* 了解各种被广泛运用的数据结构和算法。
* 在代码中如何实现可复用(reusability)。

# STL六大组件

* 容器(containers)：各种数据结构，如vector, list, deque, set, map用来存放数据。

> 从实现角度看，STL容器是一种class template

* 算法(algorithms)：各种常用算法，如sort, search, copy, erase等。

> 从实现角度看，STL算法是一种function template

* 迭代器(iterators)：扮演容器与算法之间的胶合剂，是所谓的“泛型指针”。共有5种类型，以及其他衍生变化。所有STL容器都附带有自己专属的迭代器。只有容器的设计者才知道如何遍历自己的元素。原生指针(native pointer)也是一种迭代器。

> 从实现角度看，迭代器是一种将 operator*, operator->, operator++, operator--等指针相关的操作予以重载的class template

* 仿函数(functors)：行为类似函数，可作为算法的某种策略。

> 从实现角度看，仿函数是一种重载了 operator() 的class或class template。一般函数指针可视为狭义的仿函数

* 配接器(adapters)：一种用来修饰容器或仿函数或迭代器接口的东西。STL提供的queue和stack，虽然看似容器，其实只能算是一种容器配接器。因为它们的底部完全借助deque，所有操作都由底层的deque供应。

> 改变container接口者称为container adapter; 改变iterator接口者称为iterator adapter; 改变functor接口者称为function adapter

* 配置器(allocators)：负责空间配置与管理。

> 从实现角度看，配置器是一个实现了动态空间配置，空间管理，空间释放的class template

![stl](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201809/stl.jpeg)


# 语法1 - 临时对象的产生和运用

所谓临时对象，就是一种无名对象。它的出现如果不在程序员的预期之下，往往造成效率上的负担。但有时刻意制造一些临时对象，却又是使程序干净清爽的技巧。STL最常将此技巧应用于仿函数functor与算法的搭配上。

{% highlight cpp %}
// overload operator()
template <typename T>
class print
{
public:
    void operator()(const T& elem) {
        std::cout << elem << ' ';
    }
};

int a[] = {0, 1, 2, 3, 4, 5};
std::vector<int> v(a, a + 6);
for_each(v.begin(), v.end(), print<int>());// print<int>() 是一个临时对象，不是一个函数调用操作。当for_each结束时，这个临时对象也就结束了它的生命
{% endhighlight %}

# 语法2 - 静态常量整数成员在class内部直接初始化

如果class内含const static integral data member，那么根据C++的标准，我们可以在class之内直接给予初值。

{% highlight cpp %}
// in-class static constant integer initialization
template <typename T>
class testClass 
{
public: 
    static const int _x = 1;
    static const long _y = 2L;
    static const char _z = 'a';
};
std::cout << testClass<int>::_x << std::endl;
std::cout << testClass<int>::_y << std::endl;
std::cout << testClass<int>::_z << std::endl;
{% endhighlight %}

# 语法3 - increment/decrement/dereference 操作符

任何迭代器都必须实现`increment, operator++`和取值`dereference, operator*`的功能。前者还分为`前置式prefix`和`后置式postfix`两种，有非常规律的写法。有些迭代器具备双向移动功能，就必须再提供`decrement`，也分前置式和后置式两种。

{% highlight cpp %} 
// TODO
{% endhighlight %}



# Refer

1. [The Annotated STL Sources(using SGI STL) by 侯捷]




