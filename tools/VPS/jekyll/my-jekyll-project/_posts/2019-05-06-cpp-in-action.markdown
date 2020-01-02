---
layout: post
title:  "CPP in Action"
date:   2019-05-06 17:00:00 +0800
categories: [C/C++, 编程语言]
---

* Do not remove this line (it will not be displayed)
{:toc}

# 经验

1. 对于完成相同的功能，C++需要的代码行数一般是Python的三倍，而性能则可达到其十倍以上。
2. 当目标服务属于运算密集型或者内存密集型，且需要性能且愿意为性能付出额外代价的时候，应该考虑使用C++。 
3. C++是解决性能的利器，尤其在大公司和金融机构里。C++之父Bjarne Stroustrup目前就职的地方便是摩根斯坦利。
4. 学习C++就像学习一门外语，你不要期望能够掌握所有的单词和语法，因此需要多看多写掌握合适的“语感”，而不是记住所有规则。
5. Bjarne有一个洋葱理论，抽象层次就像一个洋葱，是层层嵌套的。如果想用较低的抽象层次表达较高的概念，就好比一次切过了很多层洋葱，会把自己的眼泪熏出来。因此主张学习应该自顶向下，先学习高层的抽象，再层层剥茧，丝丝入扣地一步步进入下层。

# 环境要求

* GCC 8.3
* Clang 9.0
* Visual Studio 2019 16.3 (MSVC 19.23)

# 核心概念

## 值语义和引用语义

* 在C++里所有变量缺省都是`值语义`，如果不使用`*`和`&`的话，变量不会像Java或Python一样引用一个堆上的对象。
* 对于像`智能指针`这样的烈性，写`ptr->call()`和`ptr.get()`，语法上都是对的。而在大部分其他语言里，访问成员只用`.`。

## 异常处理

* 通过`异常`能够将问题的`检测`和问题的`解决`分离，这样程序的问题检测部分可以不必了解如何处理问题。
* C++的异常处理中，需要由问题检测部分`抛出一个对象`给处理代码，通过这个`对象的类型和内容`，两个部分能够就出现了什么错误进行通信。
* 异常是通过抛出（throw）对象而引发（raise）的，该对象的类型决定应该激活哪个处理代码，被选中的处理代码是调用链中与该对象类型匹配且离抛出异常位置最近的那个。


## 堆栈及布局

* 本地变量所需内存在栈上分配，和函数执行所需的其他数据在一起。当函数执行完这些内存也就自然释放掉。
    + 本地变量，包括简单类型`POD`(Plain Old Data)和非POD类型(有构造和析构的类型)，栈上的内存分配是一样的，只不过C++编译器会在生成代码的合适位置插入对构造和析构函数的调用
    + 编译器会自动调用析构函数，包括在函数执行发生异常时。在发生异常时对析构函数的调用，成为`栈展开(stack unwinding)`
    + 栈上的分配简单，移动栈指针而已
    + 栈上的释放也简单，函数执行结束时移动一下栈指针
    + 由于后进先出的执行过程，栈的分配不可能出现内存碎片

``` cpp
#include <cstdio>

class obj {
public:
    obj() { puts("obj()"); }
    ~obj() { puts("~obj()"); }
};

void foo(int n)
{
    obj o;
    if (n == 1) {
        throw "exception";
    }
}

int main()
{
    try {
        foo(0);
        foo(1);
    } catch (const char* err) {
        puts(err);
    }

}
/*
$ g++ -o stack_unwinding stack_unwinding.cpp 
$ ./stack_unwinding 
obj()
~obj()
obj()
~obj()
exception
*/
```


* 《程序员的自我修养》说栈的地址比堆高，栈是向下增长的，堆是向上增长的。但是在Windows上测试却不是。

``` cpp
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    double a0[4] = {1.2, 2.4, 3.6, 4.8};
    double a1[4] = {1.2, 2.4, 3.6, 4.8};

    vector<double> a2(4);
    vector<double> a3(4);

    a2[0] = 1.0/3.0;
    a2[1] = 1.0/5.0;
    a2[2] = 1.0/7.0;
    a2[3] = 1.0/9.0;
    a3[0] = 1.0/3.0;
    a3[1] = 1.0/5.0;
    a3[2] = 1.0/7.0;
    a3[3] = 1.0/9.0;

    cout << "a0[2]: " << a0[2] << " at " << &a0[2] << endl;
    cout << "a1[2]: " << a1[2] << " at " << &a1[2] << endl;
    cout << "a1[3]: " << a1[3] << " at " << &a1[3] << endl;
    cout << "a2[2]: " << a2[2] << " at " << &a2[2] << endl;
    cout << "a2[3]: " << a2[3] << " at " << &a2[3] << endl;
    cout << "a3[2]: " << a3[2] << " at " << &a3[2] << endl;
    cout << "a3[3]: " << a3[3] << " at " << &a3[3] << endl;

    return 0;
}
```

Windows的输出：

![windows_stack](/assets/images/201911/windows_stack.png)

Linux的输出：

```
a0[2]: 3.6 at 0x7ffe0bbce190
a1[2]: 3.6 at 0x7ffe0bbce170
a1[3]: 4.8 at 0x7ffe0bbce178
a2[2]: 0.142857 at 0x1b64020
a2[3]: 0.111111 at 0x1b64028
a3[2]: 0.142857 at 0x1b64050
a3[3]: 0.111111 at 0x1b64058
```

* `进程地址空间的分布`取决于`操作系统`，栈向什么方向增长取决于操作系统与CPU的组合。不要把别的操作系统的实现方式套用到Windows上。x86硬件直接支持的栈确实是“向下增长”的：push指令导致sp自减一个slot，pop指令导致sp自增一个slot。其它硬件有其它硬件的情况。

* 栈的增长方向与栈帧布局。这个上下文里说的“栈”是函数调用栈，是以“栈帧”（stack frame）为单位的。每一次函数调用会在栈上分配一个新的栈帧，在这次函数调用结束时释放其空间。被调用函数（callee）的栈帧相对调用函数（caller）的栈帧的位置反映了栈的增长方向：如果被调用函数的栈帧比调用函数的在更低的地址，那么栈就是向下增长；反之则是向上增长。

* 而在一个栈帧内，局部变量是如何分布到栈帧里的（所谓栈帧布局，stack frame layout），这完全是编译器的自由。至于数组元素与栈的增长方向：C与C++语言规范都规定了数组元素是分布在连续递增的地址上的。

> An array type describes a contiguously allocated nonempty set of objects with a particular member object type, called the element type. A postfix expression followed by an expression in square brackets [] is a subscripted designation of an element of an array object. The definition of the subscript operator [] is that E1[E2] is identical to (*((E1)+(E2))). Because of the conversion rules that apply to the binary + operator, if E1 is an array object (equivalently, a pointer to the initial element of an array object) and E2 is an integer, E1[E2] designates the E2-th element of E1 (counting from zero).

* 以简化的Linux/x86模型为例。在简化的32位Linux/x86进程地址空间模型里，（主线程的）栈空间确实比堆空间的地址要高——它已经占据了用户态地址空间的最高可分配的区域，并且向下（向低地址）增长。借用Gustavo Duarte的[Anatomy of a Program in Memory](https://link.zhihu.com/?target=http%3A//duartes.org/gustavo/blog/post/anatomy-of-a-program-in-memory/)里的图。

![linuxFlexibleAddressSpaceLayout](/assets/images/201911/linuxFlexibleAddressSpaceLayout.png)

* 虽然传统上Linux上的malloc实现会使用brk()/sbrk()来实现malloc()（这俩构成了上图中“Heap”所示的部分，这也是Linux自身所认为是heap的地方——用pmap看可以看到这里被标记为[heap]），但这并不是必须的——一个malloc()实现完全可以只用或基本上只用mmap()来实现malloc()，此时一般说的“Heap”（malloc-heap）就不一定在上图“Heap”（Linux heap）所示部分，而会在“Memory Mapping Segment”部分散布开来。不同版本的Linux在分配未指定起始地址的mmap()时用的顺序不一样，并不保证某种顺序。而且mmap()分配到的空间是有可能出现在低于主可执行程序映射进来的text Segment所在的位置。

* Linux上多线程进程中，“线程”其实是一组共享虚拟地址空间的进程。只有主线程的栈是按照上面图示分布，其它线程的栈的位置其实是“随机”的——它们可以由pthread_create()调用mmap()来分配，也可以由程序自己调用mmap()之后把地址传给pthread_create()。既然是mmap()来的，其它线程的栈出现在Memory Mapping Segment的任意位置都不出奇，与用于实现malloc()用的mmap()空间很可能是交错出现的。

* Windows的进程地址空间。Windows的进程地址空间分布跟上面说的简化的Linux/x86模型颇不一样。就算在没有ASLR的老Windows上也已经很不一样，有了ASLR之后就更加不一样了。在Windows上不应该对栈和堆的相对位置做任何假设。
要想看个清楚Windows的进程地址空间长啥样，可以用Sysinternals出品的[VMMap](https://link.zhihu.com/?target=https%3A//technet.microsoft.com/en-us/library/dd535533.aspx)看看。该工具简介请见：[VMMap - A Peek Inside Virtual Memory](https://link.zhihu.com/?target=http%3A//blogs.technet.com/b/askperf/archive/2010/01/29/vmmap-a-peek-inside-virtual-memory.aspx)。

refer:
* [堆、栈的地址高低？ 栈的增长方向？](https://www.zhihu.com/question/36103513)
* [Anatomy of a Program in Memory](https://manybutfinite.com/post/anatomy-of-a-program-in-memory/)

## RAII

RAII(Resource Acquisition Is Initialization, pronounced as "R, A, double I")，是C++用于管理资源的方式。

> Resource acquisition is initialization (RAII) is a programming idiom used in several object-oriented languages to describe a particular language behavior. In RAII, holding a resource is a class invariant, and is tied to object lifetime: resource allocation (or acquisition) is done during object creation (specifically initialization), by the constructor, while resource deallocation (release) is done during object destruction (specifically finalization), by the destructor. In other words, resource acquisition must succeed for initialization to succeed. Thus the resource is guaranteed to be held between when initialization finishes and finalization starts (holding the resources is a class invariant), and to be held only when the object is alive. Thus if there are no object leaks, there are no resource leaks.

``` cpp
void foo()
{
    bar* ptr = new bar();
    
    // some operations and throw an exception

    delete ptr;
}
```

上述示例，在不使用RAII的方法时存在的问题：
1. 若delete之前的代码出现异常时，导致delete无法执行从而产生内存泄露。
2. 不符合C++的惯用法，通常应该使用栈内存分配（意思是，借助本地变量，确保其析构函数删除该对象）。资源管理不限于内存，也可以是，关闭文件，释放同步锁，释放其他系统资源。

例如：

方法1（不推荐）：

``` cpp
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

std::mutex mtx;           // mutex for critical section

void print_block (int n, char c) {
        // critical section (exclusive access to std::cout signaled by locking mtx):
        mtx.lock();
        for (int i = 0; i < n; ++i) { std::cout << c; }
        std::cout << '\n';
        mtx.unlock();
}

int main ()
{
        std::thread th1 (print_block,50,'*');
        std::thread th2 (print_block,50,'$');

        th1.join();
        th2.join();

        return 0;
}
/*
$ g++ -o mutex2 mutex2.cpp -lpthread
$ ./mutex2
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
**************************************************
*/
```

方法2（推荐）：

``` cpp
#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
 
std::map<std::string, std::string> g_pages;
std::mutex g_pages_mutex;
 
void save_page(const std::string &url)
{
    // 模拟长页面读取
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::string result = "fake content";
 
    std::lock_guard<std::mutex> guard(g_pages_mutex);
    g_pages[url] = result;
}
 
int main() 
{
    std::thread t1(save_page, "http://foo");
    std::thread t2(save_page, "http://bar");
    t1.join();
    t2.join();
 
    // 现在访问g_pages是安全的，因为线程t1/t2生命周期已结束
    for (const auto &pair : g_pages) {
        std::cout << pair.first << " => " << pair.second << '\n';
    }
}
/*
$ g++ -o mutex mutex.cpp -lpthread
$ ./mutex 
http://bar => fake content
http://foo => fake content
*/
```

方法3（推荐）：

``` cpp
#include <thread>
#include <mutex>
#include <iostream>

int g_i = 0;
std::mutex g_i_mutex;  // 保护 g_i

void safe_increment()
{
        std::lock_guard<std::mutex> lock(g_i_mutex);
        ++g_i;

        std::cout << std::this_thread::get_id() << ": " << g_i << '\n';

        // g_i_mutex 在锁离开作用域时自动释放
}

int main()
{
        std::cout << "main: " << g_i << '\n';

        std::thread t1(safe_increment);
        std::thread t2(safe_increment);

        t1.join();
        t2.join();

        std::cout << "main: " << g_i << '\n';
}
/*
$ g++ -o mutex3 mutex3.cpp -lpthread
$ ./mutex3 
main: 0
139691703760640: 1
139691712153344: 2
main: 2
*/
```

refer:

* [Resource acquisition is initialization (RAII)](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization)
* [lock_guard](https://zh.cppreference.com/w/cpp/thread/lock_guard)
* [https://zh.cppreference.com/w/cpp/thread/mutex](https://zh.cppreference.com/w/cpp/thread/mutex)
* [http://www.cplusplus.com/reference/mutex/mutex/](http://www.cplusplus.com/reference/mutex/mutex/)


## 智能指针

智能指针本质上就是RAII资源管理功能的自然展现。

``` cpp
class shape_wrapper {
public:
        explicit shape_wrapper(shape* ptr = nullptr) : m_ptr(ptr) {}
        ~shape_wrapper() { delete m_ptr; }

        shape* get() const { return m_ptr; }

private:
        shape* m_ptr;
};
```

上面的`shape`类完成了智能指针的最基本功能，对超出作用域的对象进行释放。但是仍缺少以下功能：

* 只适用于某个类
* 该类对象的行为不够像指针
* 拷贝该类对象会引发程序行为异常

### 模板化和易用性

要让这个类能够包装任意类型的指针，需要把它变成一个模板类。在使用的时候将`shape_wrapper`改成`smart_ptr<shape>`。

``` cpp
template <typename T>
class smart_ptr {
public:
        explicit smart_ptr(T* ptr = nullptr) : m_ptr(ptr) {}
        ~smart_ptr() { delete m_ptr; }

        T* get() const { return m_ptr; }

private:
        T* m_ptr;
};
```

然后添加一些成员函数（解引用操作符*, 箭头操作符->, 布尔表达式），从而可以用类似内置的指针方式使用其对象。

``` cpp
template <typename T>
class smart_ptr {
public:
        explicit smart_ptr(T* ptr = nullptr) : m_ptr(ptr) {}
        ~smart_ptr() { delete m_ptr; }

        T* get() const { return m_ptr; }

        // make it like pointer
        T& operator*() const { return *m_ptr; }
        T* operator->() const { return m_ptr; }
        operator bool() const { return m_ptr; }

private:
        T* m_ptr;
};
```

### 拷贝构造和赋值

需要关心如何定义其`行为`。考虑如果允许拷贝，则会存在多次内存释放的问题，因此需要禁止拷贝。

``` cpp
#define DISALLOW_COPY_AND_ASSIGN(Type) \
    Type(const Type&) = delete; \
    Type& operator=(const Type&) = delete

template <typename T>
class smart_ptr {
public:
        explicit smart_ptr(T* ptr = nullptr) : m_ptr(ptr) {}
        ~smart_ptr() { delete m_ptr; }

        // forbidden copy
        //smart_ptr(const smart_ptr&) = delete;
        //smart_ptr& operator=(const smart_ptr&) = delete;

        DISALLOW_COPY_AND_ASSIGN(smart_ptr);


        T* get() const { return m_ptr; }

        // make it like pointer
        T& operator*() const { return *m_ptr; }
        T* operator->() const { return m_ptr; }
        operator bool() const { return m_ptr; }

private:
        T* m_ptr;
};
```
通过禁止`拷贝构造`和`赋值构造`，就可以在`编译时`发现存在拷贝的错误，例如，`smart_ptr<shape> ptr2(ptr1);`的写法，而不是在`运行时`出现两次释放内存的错误导致程序崩溃。

另一种解决思路是，**使用智能指针的目的是，减少对象的拷贝**。因此，可以将拷贝实现为**转移指针的所有权**。在拷贝构造函数中，通过release方法释放指针所有权。在赋值构造函数中，通过拷贝构造产生一个**临时对象**并调用swap来交换对指针的所有权。

注意：
1. 用临时对象是为了把要转移的赋值对象控制权去除，同时在转移后把被赋值对象的资源释放掉
2. 此处赋值构造函数的用法，是一种惯用法（[参考: What is the copy-and-swap idiom?](https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom)），保证了**强异常安全性**。赋值分为拷贝构造和交换两步，异常只可能在第一步发生，而第一步如果发生异常的话，this对象完全不受任何影响。无论拷贝构造成功与否，结果都是明确的两种状态，而不会发生因为赋值破坏了当前对象的场景。

``` cpp
template <typename T>
class smart_ptr {
public:
        explicit smart_ptr(T* ptr = nullptr) : m_ptr(ptr) {}
        ~smart_ptr() { delete m_ptr; }

        // forbidden copy
        //smart_ptr(const smart_ptr&) = delete;
        //smart_ptr& operator=(const smart_ptr&) = delete;

        // move resource to new object, and release old
        smart_ptr(const smart_ptr& other) {
                m_ptr = other.release();
        }

        smart_ptr& operator=(const smart_ptr& rhs) {
                // release old and create tmp object, then to swap
                smart_ptr(rhs).swap(*this);
                return *this;
        }

        T* release() {
                T* ptr = m_ptr;
                m_ptr = nullptr;
                return ptr;
        }

        void swap(smart_ptr& rhs) {
                using std::swap;
                swap(m_ptr, rhs.m_ptr);
        }


        T* get() const { return m_ptr; }

        // make it like pointer
        T& operator*() const { return *m_ptr; }
        T* operator->() const { return m_ptr; }
        operator bool() const { return m_ptr; }

private:
        T* m_ptr;
};
```

以上用法和标准库的`auto_ptr`类（为动态分配的对象提供异常安全）的行为类似（行为解释如下表所示）。注意，auto_ptr是在C++98提出的，因为此语义容易让程序犯错，在C++17时已经被正式从C++标准里删除。

| 方法 | 含义
| -- | --
| auto_ptr<T> ap | 创建名为ap的未绑定的auto_ptr对象
| auto_ptr<T> ap(p) | 创建名为ap的auto_ptr对象，ap拥有指针p指向的对象，该构造函数为explicit
| auto_ptr<T> ap1(ap2) | 创建名为ap1的auto_ptr对象，ap1保存原来存储在ap2中的指针，将所有权转给ap1，使ap2成为未绑定的auto_ptr对象
| ap1 = ap2 | 将所有权从ap2转给ap1，删除ap1指向的对象并且使ap1指向ap2指向的对象，使ap2成为未绑定的
| ~ap |  析构函数，删除ap指向的对象
| *ap | 返回对ap所绑定的对象的引用
| ap-> | 返回ap保存的指针
| ap.reset(p) | 如果p与ap的值不同，则删除ap指向的对象，并且将ap绑定到p
| ap.release() | 返回ap所保存的指针，并且使ap成为未绑定的
| ap.get() | 返回ap保存的指针

> 注意：
> 1. auto_ptr只能用于管理从new返回的一个对象，不能管理动态分配的数组（否则未定义行为）。由于auto_ptr被复制或赋值的时候有不寻常的行为，因此，不能将auto_ptr存储在标准容器类型中，标准库的容器类要求在复制或赋值之后两个对象相等，auto_ptr不满足这一要求。
> 2. 与其他复制或赋值操作不同，auto_ptr的复制和赋值改变右操作数，因此，赋值的左右操作数必须都是可修改的左值。


### 引用计数

实现一个类似标准[shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr)的智能指针，但是还缺少部分功能。

``` cpp
#include <cstdio>
#include <iostream>
#include <utility>  // std::swap

// 共享计数类
class shared_count {
public:
    shared_count() noexcept : count_(1) {}

    void add_count() noexcept        // 增加计数
    {
        ++count_;
    }
    long reduce_count() noexcept     // 减少计数，并返回当前计数以用于调用者判断是否是最后一个共享计数
    {
        return --count_;
    }
    long get_count() const noexcept  // 获取计数
    {
        return count_;
    }

private:
    long count_;
};

// 引用计数智能指针
template <typename T>
class smart_ptr {
public:

    //template <typename U>
    //friend class smart_ptr;

    explicit smart_ptr(T* ptr = nullptr) : ptr_(ptr)
    {
        if (ptr) {
            shared_count_ = new shared_count();
        }
    }

    ~smart_ptr()
    {
        // 当ptr_非空时，此时shared_count_也必然非空
        if (ptr_ && !shared_count_->reduce_count()) {
            delete ptr_;
            delete shared_count_;
        }
    }

    smart_ptr(const smart_ptr& other)
    {
        std::cout << "smart_ptr(const smart_ptr& other)\n";
        ptr_ = other.ptr_;
        if (ptr_) {
            other.shared_count_->add_count();
            shared_count_ = other.shared_count_;
        }
    }

    template <typename U>
        smart_ptr(const smart_ptr<U>& other) noexcept
        {
            std::cout << "smart_ptr(const smart_ptr<U>& other)\n";
            ptr_ = other.ptr_;
            if (ptr_) {
                other.shard_count_->add_count();
                shared_count_ = other.shared_count_;
            }
        }

    template <typename U>
        smart_ptr(smart_ptr<U>&& other)
        {
            std::cout << "smart_ptr(smart_ptr<U>&& other)\n";
            ptr_ = other.ptr_;
            if (ptr_) {
                shared_count_ = other.shared_count_;
                other.ptr_ = nullptr;
            }
        }

    // 指针类型转换
    template <typename U>
        smart_ptr(const smart_ptr<U>& other, T* ptr) noexcept {
            ptr_ = ptr;
            if (ptr_) {
                other.shared_count_->add_count();
                shared_count_ = other.shared_count_;
            }
        }

    smart_ptr& operator=(smart_ptr rhs) noexcept {
        rhs.swap(*this);
        return *this;
    }

    T* get() const noexcept {
        return ptr_;
    }

    long use_count() const
    {
        if (ptr_) {
            return shared_count_->get_count();
        } else {
            return 0;
        }
    }

    void swap(smart_ptr& rhs) noexcept {
        using std::swap;
        swap(ptr_, rhs.ptr_);
        swap(shared_count_, rhs.shared_count_);
    }

    T& operator*() const noexcept { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    operator bool() const noexcept { return ptr_; }

private:
    T* ptr_;
    shared_count* shared_count_;
};

template <typename T>
void swap(smart_ptr<T>& lhs, smart_ptr<T>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename T, typename U>
smart_ptr<T> static_pointer_cast(const smart_ptr<U>& other) noexcept {
    T* ptr = static_cast<T*>(other.get());
    return smart_ptr<T>(other, ptr);
}

template <typename T, typename U>
smart_ptr<T> reinterpret_pointer_cast(const smart_ptr<U>& other) noexcept {
    T* ptr = reinterpret_cast<T*>(other.get());
    return smart_ptr<T>(other, ptr);
}

template <typename T, typename U>
smart_ptr<T> const_pointer_cast(const smart_ptr<U>& other) noexcept {
    T* ptr = const_cast<T*>(other.get());
    return smart_ptr<T>(other, ptr);
}

template <typename T, typename U>
smart_ptr<T> dynamic_pointer_cast(const smart_ptr<U>& other) noexcept {
    T* ptr = dynamic_cast<T*>(other.get());
    return smart_ptr<T>(other, ptr);
}

int main()
{
    int *ptr = new int(1);
    std::cout << "ptr: " << *ptr << std::endl;

    smart_ptr<int> sptr1(ptr);
    std::cout << "sptr1 use conut: " << sptr1.use_count() << std::endl;

    smart_ptr<int> sptr2;
    std::cout << "sptr2 use conut: " << sptr2.use_count() << std::endl;
    sptr2 = sptr1;
    std::cout << "sptr2 use conut: " << sptr2.use_count() << std::endl;

    smart_ptr<int> sptr3(sptr1);
    std::cout << "sptr3 use conut: " << sptr3.use_count() << std::endl;

    smart_ptr<int> sptr4(std::move(sptr1));
    std::cout << "sptr4 use conut: " << sptr4.use_count() << std::endl;

    if (sptr1) {
        std::cout << "sptr1 is not empty\n";
    } else {
        std::cout << "sptr1 is empty\n";
    }

}
/*
$ g++ -std=c++11 -o shared_count shared_count.cpp 
$./shared_count 
ptr: 1
sptr1 use conut: 1
sptr2 use conut: 0
sptr2 use conut: 1
smart_ptr(const smart_ptr& other)
sptr3 use conut: 2
smart_ptr(smart_ptr<U>&& other)
sptr4 use conut: 2
sptr1 is empty
*/

```

关于`shared_ptr`的一个使用例子：

``` cpp
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
 
struct Base
{
    Base() { std::cout << "  Base::Base()\n"; }
    // Note: non-virtual destructor is OK here
    ~Base() { std::cout << "  Base::~Base()\n"; }
};
 
struct Derived: public Base
{
    Derived() { std::cout << "  Derived::Derived()\n"; }
    ~Derived() { std::cout << "  Derived::~Derived()\n"; }
};
 
void thr(std::shared_ptr<Base> p)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::shared_ptr<Base> lp = p; // thread-safe, even though the
                                  // shared use_count is incremented
    {
        static std::mutex io_mutex;
        std::lock_guard<std::mutex> lk(io_mutex);
        std::cout << "local pointer in a thread:\n"
                  << "  lp.get() = " << lp.get()
                  << ", lp.use_count() = " << lp.use_count() << '\n';
    }
}
 
int main()
{
    std::shared_ptr<Base> p = std::make_shared<Derived>();
 
    std::cout << "Created a shared Derived (as a pointer to Base)\n"
              << "  p.get() = " << p.get()
              << ", p.use_count() = " << p.use_count() << '\n';
    std::thread t1(thr, p), t2(thr, p), t3(thr, p);
    p.reset(); // release ownership from main
    std::cout << "Shared ownership between 3 threads and released\n"
              << "ownership from main:\n"
              << "  p.get() = " << p.get()
              << ", p.use_count() = " << p.use_count() << '\n';
    t1.join(); t2.join(); t3.join();
    std::cout << "All threads completed, the last one deleted Derived\n";
}
/*
$ g++ -std=c++11 -o shared_ptr_demo shared_ptr_demo.cpp 
$ ./shared_ptr_demo 
  Base::Base()
  Derived::Derived()
Created a shared Derived (as a pointer to Base)
  p.get() = 0x7f9978c04da8, p.use_count() = 1
Shared ownership between 3 threads and released
ownership from main:
  p.get() = 0x0, p.use_count() = 0
local pointer in a thread:
  lp.get() = 0x7f9978c04da8, lp.use_count() = 6  // TODO ?
local pointer in a thread:
  lp.get() = 0x7f9978c04da8, lp.use_count() = 4
local pointer in a thread:
  lp.get() = 0x7f9978c04da8, lp.use_count() = 2
  Derived::~Derived()
  Base::~Base()
All threads completed, the last one deleted Derived
 */

```


# STL

## const引用减少拷贝

``` cpp
#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

vector<string> func()
{
        vector<string> vec = {"a", "b"};
        return vec;
}

void print(const vector<string>& vec)
{
        for (auto& item : vec) {
                cout << item << " ";
        }
        cout << endl;
}

int main()
{
        const vector<string>& res = func();
        print(res);
}
```

## 字符串分割

``` cpp
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>

std::vector<std::string> split_str(const std::string& src, char split)
{
        std::vector<std::string> res;

        if (src.empty()) {
                return res;
        }

        std::string::size_type start = 0, next;
        while ((next = src.find(split, start)) != std::string::npos) {
                res.push_back(src.substr(start, next - start));
                start = next + 1;
        }
        res.push_back(src.substr(start));
        return res;
}

void print(std::vector<std::string> &vec)
{
        for (auto &item : vec) {
                std::cout << item << "|";
        }
        std::cout << std::endl;
}

int main()
{
        std::vector<int> vec;
        std::cout << "size: " << vec.size() 
                << " capacity: " << vec.capacity() << std::endl;

        vec.reserve(10);
        vec.push_back(1);

        std::cout << "size: " << vec.size() 
                << " capacity: " << vec.capacity() << std::endl;


        std::string s;
        std::vector<std::string> res;

        s = "1,2";
        res = split_str(s, ',');
        print(res);

        s = "1,2,";
        res = split_str(s, ',');
        print(res);

        s = ",1,2";
        res = split_str(s, ',');
        print(res);

        s = "1";
        res = split_str(s, ',');
        print(res);

        s = "";
        res = split_str(s, ',');
        print(res);

        s = ",";
        res = split_str(s, ',');
        print(res);

        s = ",,";
        res = split_str(s, ',');
        print(res);

}
/*
g++ -o test test.cpp -std=c++11
./test
size: 0 capacity: 0
size: 1 capacity: 10
1|2|
1|2||
|1|2|
1|

||
|||
*/
```

[range-for](https://zh.cppreference.com/w/cpp/language/range-for)

[lambda](https://zh.cppreference.com/w/cpp/language/lambda)

[std::chrono](https://blog.csdn.net/u013390476/article/details/50209603)


# 代码片段


## Time

## DateTime and UnixTime

``` cpp
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main()
{
        struct tm tm = {0};
        char *res = strptime("2019-10-01 18:00:00", "%Y-%m-%d %H:%M:%S", &tm);
        if (res == NULL) {
                printf("strptime err\n");
                return -1;
        }
        time_t t = mktime(&tm); 

        // equal to `date -d "2019-10-01 18:00:00" +%s`
        printf("t[%ld]\n", t);

        return 0;
}
```

``` cpp
int datetime_to_unixtime(std::string &end_time, time_t &expired_time, std::string &errinfo)
{
    struct tm tm = {0};
    // end_time's format is like "2019-10-01 18:00:00"
    char *res = strptime(end_time.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    if (res == NULL) {
        errinfo = "tc_subscribe ans, end_time is invalid";
        printf("%s", errinfo.c_str());
        return 1;
    }
    expired_time = mktime(&tm);

    return 0;
}

int get_unixtime(time_t &unixtime, std::string &datetime)
{
    unixtime = time(NULL);

    char buf[64] = {0};
    struct timeval tv;
    struct tm tm_obj;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm_obj);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_obj);

    datetime = buf;

    return 0;
}
```


refer:
* [strptime](https://linux.die.net/man/3/strptime)
* [mktime](http://www.cplusplus.com/reference/ctime/mktime/)
* [How to convert a string variable containing time to time_t type in c++?](https://stackoverflow.com/questions/11213326/how-to-convert-a-string-variable-containing-time-to-time-t-type-in-c/11213640#11213640)
* [Date/time conversion: string representation to time_t](https://stackoverflow.com/questions/321793/date-time-conversion-string-representation-to-time-t/321847#321847)


## Regex

[How do I write a regular expression that matches an IPv4 dotted address?](https://devblogs.microsoft.com/oldnewthing/20060522-08/?p=31113)

``` cpp
#include <boost/regex.hpp>

/*
作用: 使用boost库, 实现IPv4匹配
10.0.0.0 - 10.255.255.255
172.16.0.0 - 172.31.255.255
192.168.0.0 - 192.168.255.255
*/
void test_regex_ip()
{
	std::string ip[] = {
		std::string("10.0.0.1"),
		std::string("172.16.0.1"),
		std::string("192.168.0.1"),
		std::string("10.*.*.*"),
		std::string("172.16.*.*"),
		std::string("192.168.*.*")
	};

	for (int i = 0; i != sizeof(ip) / sizeof(ip[0]); ++i) {
		auto match = [&] {
			//boost::regex pattern("(\\d{1,3}(\\.\\d{1,3}){3})");
			//boost::regex pattern("(\\d{1,3}(\\..*){3})");
			boost::regex pattern("(172(\\.\\d{1,3}){2}(\\..*))");
			boost::smatch match;
			try {
				if (boost::regex_search(ip[i], match, pattern)) {
					std::cout << "ip: " << match[1] << std::endl;
				} else {
					std::cout << ip[i] << " did not match\n";
				}

			} catch (boost::regex_error &e) {
				std::cout << "err: " << e.what() << std::endl;
			}
		};

		match();
	}
}
```

## Unit Test

[参考](https://github.com/idealvin/co/blob/master/base/time.h)

``` cpp
#include "test.h"

std::string ss;

def_test(100000);
def_case(ss = "");
def_case(ss = std::to_string(12345678));
```

``` cpp
// test.h

#include "base/def.h"
#include "base/time.h"
#include "base/log.h"

#define def_test(n) \
    int N = n; \
    int64 _us; \
    Timer _t

#define def_case(func) \
    do { \
        _t.restart(); \
        for (int i = 0; i < N; ++i) { \
            func; \
        } \
        _us = _t.us(); \
        CLOG << #func << ":\t" << (_us * 1.0 / N) << " us"; \
    } while (0)
```

## Log

[参考](https://github.com/idealvin/co/blob/master/base/log.h)


## 浮点数计算精度问题

在`C/C++`中：

``` cpp
double a = 12.03;
double b = 22; 
long long c = a * b * 100000000L;
printf("c[%lld]\n", c);              // 26465999999
c = a * 100000000L * b;
printf("c[%lld]\n", c);              // 26466000000
```

亦或在`python`中：

```
Python 2.7.5 (default, Jun 17 2014, 18:11:42) 
[GCC 4.8.2 20140120 (Red Hat 4.8.2-16)] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> 1.1 + 0.1
1.2000000000000002
```

* Actually, the error is because there is no way to map 0.1 to a finite binary floating point number. 
* Most fractions can't be converted to a decimal with exact precision. A good explanation is here:  [Floating Point Arithmetic: Issues and Limitations](https://docs.python.org/release/2.5.1/tut/node16.html)

> **What can I do to avoid this problem?**
> That depends on what kind of calculations you’re doing.
> * If you really need your results to add up exactly, especially when you work with money: use a special decimal datatype.
> * If you just don’t want to see all those extra decimal places: simply format your result rounded to a fixed number of decimal places when displaying it.
> * If you have no decimal datatype available, an alternative is to work with integers, e.g. do money calculations entirely in cents. But this is more work and has some drawbacks.

refer:

* [What Every Programmer Should Know About Floating-Point Arithmetic](https://floating-point-gui.de/)
* [Floating Point Arithmetic: Issues and Limitations](https://docs.python.org/release/2.5.1/tut/node16.html)
* [如何理解double精度丢失问题？](https://www.zhihu.com/question/42024389/answer/93528601)
* [How to deal with floating point number precision in JavaScript?](https://stackoverflow.com/questions/1458633/how-to-deal-with-floating-point-number-precision-in-javascript)
* [The Perils of Floating Point](http://www.lahey.com/float.htm)

在`C/C++`中的一些解决方案：

* [C++ decimal data types](https://stackoverflow.com/questions/14096026/c-decimal-data-types)
* [开源库- decimal_for_cpp](https://github.com/vpiotr/decimal_for_cpp)

> If you are looking for data type supporting money / currency then try this: decimal_for_cpp

* [boost - cpp_dec_float](https://www.boost.org/doc/libs/1_68_0/libs/multiprecision/doc/html/boost_multiprecision/tut/floats/cpp_dec_float.html)

> The cpp_dec_float back-end is used in conjunction with number: It acts as an entirely C++ (header only and dependency free) floating-point number type that is a drop-in replacement for the native C++ floating-point types, but with much greater precision.

``` cpp
#include <iostream>
#include <iomanip>
#include <boost/multiprecision/cpp_dec_float.hpp>

int main()
{
    namespace mp = boost::multiprecision;
    // here I'm using a predefined type that stores 100 digits,
    // but you can create custom types very easily with any level
    // of precision you want.
    typedef mp::cpp_dec_float_100 decimal;

    decimal tiny("0.0000000000000000000000000000000000000000000001");
    decimal huge("100000000000000000000000000000000000000000000000");
    decimal a = tiny;         

    while (a != huge)
    {
        std::cout.precision(100);
        std::cout << std::fixed << a << '\n';
        a *= 10;
    }    
}
```

* [Exact decimal datatype for C++?](https://stackoverflow.com/questions/15319967/exact-decimal-datatype-for-c)
* [The GNU Multiple Precision arithmetic library](https://gmplib.org/)
* [C++ Data Types](https://www.geeksforgeeks.org/c-data-types/)


# 内联汇编

GCC为内联汇编提供特殊结构，其格式如下。`汇编程序模板`由`汇编指令`组成。`输入操作数`是充当指令输入操作数使用的C表达式。`输出操作数`是将对其执行汇编指令输出的C表达式。内联汇编的重要性体现在它能够灵活操作，而且可以使其输出通过C变量显示出来。因为它具有这种能力，所以"asm"可以用作汇编指令和包含它的C程序之间的接口。**简单内联汇编只包括指令，而扩展内联汇编包括操作数**。

``` asm
asm ( assembler template
     
: output operands               (optional)
     
: input operands                (optional)
     
: list of clobbered registers   
    (optional)
     
);
```

* [Linux中x86的内联汇编](https://www.ibm.com/developerworks/cn/linux/sdk/assemble/inline/index.html)
* [x86 Assembly Guide](http://www.cs.virginia.edu/~evans/cs216/guides/x86.html)
* [[译] 简明 x86 汇编指南](https://arthurchiao.github.io/blog/x86-asm-guide-trans-cn-zh/)
* [What is a clobber?](https://stackoverflow.com/questions/41899881/what-is-a-clobber/41900500)


# 编译器

[is-pragma-once-a-safe-include-guard](https://stackoverflow.com/questions/787533/is-pragma-once-a-safe-include-guard)


# 网络

* [tcpdump/wireshark 抓包及分析（2019）](https://arthurchiao.github.io/blog/tcpdump-practice-zh/)
* [tcpdump: An Incomplete Guide](https://arthurchiao.github.io/blog/tcpdump/)
* [[译] 使用 Linux tracepoint、perf 和 eBPF 跟踪数据包 (2017)](https://arthurchiao.github.io/blog/trace-packet-with-tracepoint-perf-ebpf-zh/)

# 文档

1. [GCC online documentation](https://gcc.gnu.org/onlinedocs/)

# 开源

* [An elegant and efficient C++ basic library for Linux, Windows and Mac](https://github.com/idealvin/co)



# 文章

* [6 Tips to supercharge C++11 vector performance](https://www.acodersjourney.com/6-tips-supercharge-cpp-11-vector-performance/)




