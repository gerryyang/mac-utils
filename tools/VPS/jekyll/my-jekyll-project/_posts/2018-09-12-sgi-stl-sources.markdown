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

# 困惑的C++语法

## 语法1 - 临时对象的产生和运用

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

## 语法2 - 静态常量整数成员在class内部直接初始化

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

## 语法3 - increment/decrement/dereference 操作符

任何迭代器都必须实现`increment, operator++`和取值`dereference, operator*`的功能。前者还分为`前置式prefix`和`后置式postfix`两种，有非常规律的写法。有些迭代器具备双向移动功能，就必须再提供`decrement`，也分前置式和后置式两种。

{% highlight cpp %} 
class INT
{
    friend std::ostream& operator<<(std::ostream& os, const INT& i);

public:
    INT(int i) ： m_i(i) { };

    // prefix: increment and then fetch
    INT& operator++()
    {
        ++m_i;   // 随着class的不同，该行应该有不同的操作
        return *this;
    }

    // postfix: fetch and then increment
    const INT operator++(int)
    {
        INT temp = *this;
        ++(*this);
        return temp;
    }

    // prefix： decrement and then fetch
    INT& operator--()
    {
        --(m_i);   // 随着class的不同，该行应该有不同的操作
        return *this;
    }

    // postfix: fetch and then decrement
    const INT operator--(int)
    {
        INT temp = *this;
        --(*this);
        return temp;
    }

    // dereference
    int& operator*() const
    {
        return (int&)m_i;// 以上转换告诉编译器，你确实要将const int转为non-const lvalue
                         // 如果没有转型，有些编译器会告警甚至视为错误
    }

private:
    int m_i;

};

std::ostream& operator<<(std::ostream& os, const INT& i)
{
    os << '[' << i.m_i << ']';
    return os;
}

// main.cpp
INT i_obj(5);
std::cout << i_obj++;
std::cout << ++i_obj;
std::cout << i_obj--;
std::cout << --i_obj;
std::cout << *i_obj;
{% endhighlight %}

## 语法4 - 前闭后开区间表示法 [)

任何一个STL算法，都需要获得由一对迭代器(泛型指针)所标示的区间，用以表示操作范围。这一对迭代器所表示的是所谓的`前闭后开`区间，以`[first, last)`表示。也就是，整个实际范围从first开始，直到last-1。**迭代器last表示：最后一个元素的下一个位置**。

> 1. 这种`off by one`或者`pass the end`的标示法，带来了许多方便。
> 2. 前闭后开区间，元素之间无需占用连续内存空间。

{% highlight cpp %} 
template<class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value)
{
    while (first != last && *first != value) ++first;
    return first;
}

template<class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f)
{
    for (; first != last; ++first)
        f(*first);
    return f;
}
{% endhighlight %}

## 语法5 - function call 操作符 operator()

函数调用操作`()`也可以被重载。过去C语言时代，欲将函数当做参数传递，唯有通过`函数指针`才能达成。

{% highlight cpp %} 
int fcmp(const void* elem1, const void* elem2)
{
    const int* i1 = (const int*)elem1;
    const int* i2 = (const int*)elem2;

    if (* i1 < *i2) return -1;
    else if (*i1 == *i2) return 0;
    else if (*i1 > *i2) return 1;
}

int ia[10] = {2, 4, 1, 3, 8, 5, 4, 6, 7};
for (int i = 0; i < 10; i++)
    std::cout << ia[i] << " ";
qsort(ia, sizeof(ia)/sizeof(int), sizeof(int), util::fcmp);
for (int i = 0; i < 10; i++)
    std::cout << ia[i] << " ";
{% endhighlight %}

但是，函数指针有缺点：它无法持有自己的状态，也无法达到组件技术中的可适配性(adaptability)，即无法再将某些修饰条件加诸于其上而改变其状态。

为此，STL算法的特殊版本所接受的所谓`条件`，或`策略`，或`一整组操作`，都以**仿函数**形式呈现。

> 所谓仿函数(functor)，就是使用起来像函数一样的东西，如果你针对某个class进行operator()重载，它就成为一个仿函数。至于要成为一个可配接的仿函数，还需要做一些额外的努力。

{% highlight cpp %} 
// 由于将operator()重载了，因此plus成了一个仿函数
template <class T>
struct plus 
{
    T operator()(const T& x, const T& y) const { return x + y; }
};

// 由于将operator()重载了，因此minus成了一个仿函数
template <class T>
struct minus
{
    T operator()(const T& x, const T& y) const { return x - y; }
};

plus<int> plus_obj;    // 仿函数对象
minus<int> minus_obj;  // 仿函数对象
std::cout << plus_obj(1, 2) << std::endl;
std::cout << minus_obj(1, 2) << std::endl;

std::cout << plus<int>(1, 2) << std::endl;
std::cout << minus<int>(1, 2) << std::endl;
{% endhighlight %}

上述的`plus<T>`和`minus<T>`已经非常接近STL的实现了。唯一的差别在它缺乏可配接能力。


# 空间配置器

1. 以STL的运用角度而言，空间配置器是最不需要介绍的东西，因为它总是隐藏在一切组件(或容器)的背后，默默工作。
2. 但若以STL的实现角度而言，第一个需要介绍的就是空间配置器，因为整个STL的操作对象都存放在容器之内。
3. 为什么不说`allocator`是内存配置器，而说是空间配置器，因为空间不一定是内存，也可以是磁盘或其他辅助存储介质。

## 空间配置器的标准接口

根据STL的规范，以下是`allocator`的必要接口：

{% highlight cpp %} 
allocator::value_type
allocator::pointer
allocator::const_pointer
allocator::reference
allocator::const_reference
allocator::size_type
allocator::difference_type
allocator::rebind

allocator::allocator()                                      // default constructor
allocator::allocator(const allocator&)                      // copy constructor
template <class U>allocator::allocator(const allocator<U>&) // 泛化的copy constructor
allocator::~allocator()                                     // destructor

pointer allocator::address(reference x) const
const_pointer allocator::address(const_reference x) const
pointer allocator::allocate(size_type n, const void* = 0)
void allocator::deallocate(pointer p, size_type n)
size_type allocator::max_size() const

void allocator::construct(pointer p, const T& x)            // 等同于 new((void*) p) T(x)
void allocator::destroy(pointer p)                          // 等同于 p->~T()
{% endhighlight %}

## SGI特殊的空间适配器 

考虑到对效率的优化，SGI默认使用特殊的空间适配器`std::alloc`。

一般而言，C++内存配置和释放操作是这样的：

{% highlight cpp %} 
class Foo {...};
Foo* pf = new Foo;    // 配置内存，然后构造对象
delete pf;            // 将对象析构，然后释放内存
{% endhighlight %}

`new`算式内含两阶段操作：
1. 调用`::operator new`配置内存
2. 调用`Foo::Foo()`构造对象内容
`delete`算式也内含两阶段操作：
1. 调用`Foo::~Foo()`将对象析构
2. 调用`::operator delete`释放内存

STL的`allocator`为了精密分工，将这两个阶段操作区分开来：

1. 内存配置操作由`alloc::allocate()`负责
2. 内存释放操作由`alloc::deallocate()`负责
3. 对象构造操作由`::construct()`负责
4. 对象析构操作由`::destroy()`负责

考虑到小型区块所可能造成的内存碎片问题，。SGI容器使用了`两级空间适配器`的设计。

1. 第一级配置器直接使用`malloc()`和`free()`
2. 第二级配置器则视情况采取不同的策略
    * 当配置区块超过128B时，视之为“足够大”，便调用第一级配置器
    * 当配置区块小于128B时，视之为“过小”，为了降低额外负担，便采用复杂的`memory pool`整理方式，而不再求助于第一级配置器。

![sgi_allocate_12_a](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201809/sgi_allocate_12_a.jpg)

![sgi_allocate_12_b](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201809/sgi_allocate_12_b.jpg)


## 内存池 (memory pool)

![sgi_allocate](https://github.com/gerryyang/mac-utils/raw/master/tools/VPS/jekyll/my-jekyll-project/assets/images/201809/sgi_allocate.jpg)

{% highlight cpp %}
template <bool threads, int inst>
char * __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs)
{
    char * result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;// 内存池剩余空间

    if (bytes_left >= total_bytes) {
        // 内存池剩余空间完全满足需求量
        result = start_free;
        start_free += total_bytes;
        return result;

    } else if (bytes_left >= size) {
        // 内存池剩余空间不能完全满足需求量，但足够供应一个(含)以上的区块
        nobjs = bytes_left / size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return result;

    } else {
        // 内存池剩余空间连一个区块的大小都无法提供
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        // 以下试着让内存池中残余零头还有利用价值
        if (bytes_left > 0) {
            // 内存池还有一些零头，先配给适当的free list
            // 首先寻找适当的free list
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
            // 调整free list，将内存池中的残余空间编入
            ((obj *)start_free) -> free_list_link = *my_free_list;
            *my_free_list = (obj *)start_free;
        }

        // 配置heap空间，用来补充内存池
        start_free = (char *)malloc(bytes_to_get);
        if (0 == start_free) {
            // heap空间不足，malloc失败
            int i;
            obj * volatile * my_free_list, * p;

            for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                // free list尚有未用区块
                if (0 != p) {
                    // 调整free list以释放出未用区块
                    *my_free_list = p -> free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    // 递归调用自己，为了修正nobjs
                    return (chunk_alloc(size, nobjs));

                }
            }
            end_free = 0;// 如果出现意外，山穷水尽，到处都没有内存可用了

            // 调用一级适配器，看看out-of-memory机制能否尽点力
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            // 这会导致抛出异常，或内存不足的情况获得改善
        }

        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        // 递归调用自己，为了修正nobjs
        return (chunk_alloc(size, nobjs));
    }
}
{% endhighlight %}



# Refer

1. [The Annotated STL Sources(using SGI STL) by 侯捷]




