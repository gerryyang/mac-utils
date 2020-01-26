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

## 移动语义

`移动语义`是C++11里引入的一个重要概念，理解这个概念是理解很多现代C++里的**优化的基础**。

### 值分左右

C++标准里定义了**表达式的值类别**（注意，区别术语`值类型`，是与`引用类型`相对而言，在C++里，所有的原生类型，枚举，结构，联合，类都代表`值类型`，只有引用`&`和指针`*`才是引用类型）：

* `expression`分为`glvalue`和`rvalue`
* `glvalue`分为`lvalue`和`xvalue`，`rvalue`分为`xvalue`和`prvalue`

含义：

* 一个`lvalue`通常可以放在等号左边的表达式，称为**左值**
* 一个`rvalue`通常只能放在等号右边的表达式，称为**右值**
* 一个`glvalue`是`generalized lvalue`，称为**广义左值**
* 一个`xvalue`是`expiring lvalue`，称为**将亡值**
* 一个`prvalue`是`pure rvalue`，称为**纯右值**

#### 左值

**左值（lvalue）**是有标识符，可以取地址的表达式，常见的有：

* 变量，函数，或数据成员的名字
* 返回左值引用的表达式，如，`++x`，`x = 1`，`cout << ""`
* 字符串字面量，如"hello world"

在函数调用时，**左值**可以绑定到**左值引用的参数**，如`T&`，一个**常量**只能绑定到**常左值引用**，如`const T&`

#### 右值

**纯右值prvalue**是没有标识符，不可以取地址的表达式，一般称之为**临时对象**，常见有：

* 返回非引用类型的表达式，如，`x++`，`x + 1`，`make_shared<int>(42)`
* 除字符串字面量之外的字面量，如，`42`，`true`

> 总结

* 在C++11之前，**右值**可以绑定到**常左值引用（const lvalue reference）**的参数，如，`const T&`，但不可以绑定到**非常左值引用（non-const lvalue reference）**，如，`T&`。
* 从C++11开始，新增了一种**右值引用（T&&）**，通过不同的引用类型进行重载，从而可以实现不同的行为，比如，性能优化。

``` cpp
smart_ptr<shape> ptr1{new circle()};
smart_ptr<shape> ptr2 = std::move(ptr1);
```

第一个表达式，`new circle()`是一个**纯右值prvalue**，对于指针通常使用值传递，并不关心它是左值还是右值。
第二个表达式，`std::move(ptr1)`的作用是，**把一个左值引用强制转换成一个右值引用，而并不改变其内容**。可以把`std::move(ptr1)`看作是**一个有名字的右值**，为了和无名的纯右值prvalue相区别，C++里目前把这种表达式称为**xvalue**。与左值lvalue不同，xvalue仍然是不能取地址的（xvalue与prvalue相同），因此xvalue和prvalue都被归为**右值rvalue**。

| 表达式值类别 | 不可移动 | 可移动 |
| -- | -- | -- | 
| glvalue | lvalue（有标识符）| xvalue（有标识符）
| rvalue |  | prvalue（无标识符）xvalue（有标识符）


#### 临时对象特殊的生命周期延长规则

[GotW #88: A Candidate For the “Most Important const”](https://herbsutter.com/2008/01/01/gotw-88-a-candidate-for-the-most-important-const/)提出了一个问题：

``` cpp
// Is the following code legal C++?
string f() { return "abc"; }

void g() {
    const string& s = f();
    cout << s << endl;    // can we still use the "temporary" object? Yes
}
```

如果把`const`去掉呢？

``` cpp
// What if we take out the const… is Example 2 still legal C++?
string f() { return "abc"; }

void g() {
    string& s = f();       // still legal? No
    cout << s << endl;
```

另一个例子，当引用生命周期结束时，对象是如何析构的？

``` cpp
Derived factory(); // construct a Derived object

void g() {
  const Base& b = factory(); // calls Derived::Derived here
  // … use b …
} // calls Derived::~Derived directly here — not Base::~Base + virtual dispatch!
```


``` cpp
// When the reference goes out of scope, which destructor gets called?

#include <iostream>
using namespace std;

class Base
{
public:
        Base() { cout << "Base()\n"; }
        ~Base() { cout << "~Base()\n"; }
};

class Derived : public Base
{
public:
        Derived() { cout << "Derived()\n"; }
        ~Derived() { cout << "~Derived()\n"; }
};

Derived Factory()
{
        return Derived();
}

int main()
{
        //Base* obj = new Derived();
        //const Base* obj = new Derived();
        //delete obj;

        // error: cannot bind non-const lvalue reference of type ‘Base&’ to an rvalue of type ‘Base’
        //Base& obj = Factory();

        const Base& obj = Factory();  // ok
        //Base&& obj = Factory();     // ok

        cout << "end\n";
}
/*
Base()
Derived()
end
~Derived()
~Base()
*/
```
即，你可以把一个没有虚析构函数的子类对象绑定到基类的引用变量上，这个子类对象的析构仍然是完全正常的。这是因为这条规则只是延后了临时对象的析构而已，不是利用引用计数等复杂的方法，因而只要引用绑定成功，其类型并没有什么影响。

> 为了方便对临时对象的使用，C++对临时对象有特殊的生命周期延长规则：
> **如果一个prvalue被绑定到一个引用上，它的生命周期会延长到和这个引用变量一样长。且注意，这条生命周期延长规则只对prvalue有效，而对xvalue无效。**

测试：

``` cpp
#include <cstdio>

class shape {
public:
        virtual ~shape() {}
};

class circle : public shape {
public:
        circle() { puts("circle()"); }
        ~circle() { puts("~circle()"); }
};

class triangle : public shape {
public:
        triangle() { puts("triangle()"); }
        ~triangle() { puts("~triangle()"); }
};

class result {
public:
        result() { puts("result()"); }
        ~result() { puts("~result()"); }
};

result process_shape(const shape& shape1,
                const shape& shape2)
{
        puts("process_shape()");
        return result();
}

int main()
{
        puts("main()");
        process_shape(circle(), triangle());
        puts("something else");
}
/*
main()
triangle()
circle()
process_shape()
result()
~result()
~circle()
~triangle()
something else
*/
```

修改代码，将prvalue绑定到引用（const T& 或者 T&&）后，临时对象的生命周期则会和引用对象的生命周期一致：

``` cpp
int main()
{
        puts("main()");
        //const result &r = process_shape(circle(), triangle());   // ok
        result&& r = process_shape(circle(), triangle());          // ok
        puts("something else");
}
/*
main()
triangle()
circle()
process_shape()
result()
~circle()
~triangle()
something else
~result()
*/
```

如果改为xvalue，则此规则无效。注意，有效变量r指向的对象已经不存在了，对r解引用是一个**未定义行为**。

``` cpp
#include <utility>
int main()
{
        puts("main()");
        //const result &r = process_shape(circle(), triangle());
        //result&& r = process_shape(circle(), triangle());
        result&& r = std::move(process_shape(circle(), triangle())); // xvalue, no
        puts("something else");
}
/*
main()
triangle()
circle()
process_shape()
result()
~result()
~circle()
~triangle()
something else
*/
```

### 移动的意义

使用右值引用的目的是实现移动，而实现移动的意义是减少运行的开销。


在使用容器类的情况下，移动更有意义。例如：

``` cpp
string result = string("Hello, ") + name + ".";
```

执行流程大致如下：

1. 调用构造函数 string(const char*)，生成临时对象1："Hello, "  
2. 调用 operator+(const string&, const string&)，生成临时对象2："Hello, " 
3. 调用 operator+(const string&, const char*)，生成临时对象3："Hello, ." 
4. 假设返回值优化能够生效（最佳情况），对象 3 可以直接在 result 里构造完成
5. 临时对象 2 析构，释放指向 string("Hello, ") + name 的内存
6. 临时对象 1 析构，释放指向 string("Hello, ") 的内存

因此，建议的写法是：

``` cpp
// 只会调用构造函数一次和 string::operator+= 两次，没有任何临时对象需要生成和析构
string result = "Hello, ";
result += name;
result += ".";
```

但是，从 C++11 开始，以上`+=`的写法不再是必须的。同样上面那个单行的语句，执行流程大致如下：

1. 调用构造函数 string(const char*)，生成临时对象 1："Hello, "
2. 调用 operator+(string&&, const string&)，直接在临时对象 1 上面执行追加操作，并把结果移动到临时对象 2
3. 调用 operator+(string&&, const char*)，直接在临时对象 2 上面执行追加操作，并把结果移动到 result
4. 临时对象 2 析构，内容已经为空，不需要释放任何内存
5. 临时对象 1 析构，内容已经为空，不需要释放任何内存

性能上，所有的字符串只复制了一次；虽然比啰嗦的写法仍然要增加临时对象的构造和析构，但由于这些操作不牵涉到额外的内存分配和释放，是相当廉价的。程序员只需要牺牲一点点性能，就可以大大增加代码的可读性。

所有的现代 C++ 的标准容器都针对移动进行了优化。

### 实现移动

让设计的对象支持移动，通常需要：

* 对象有，分开的拷贝构造和移动构造函数（例如，unique_ptr只支持移动，所以只有移动构造，不支持拷贝）
* 对象有，swap成员函数，支持和另一个对象快速交换
* 在对象的命名空间下，应当有一个全局的swap函数，调用成员函数swap来实现交换。（支持这种用法可以方便在其他对象里包含你的对象，并快速实现它们的swap函数）
* 实现通用的 operator=
* 上面各个函数如果不抛异常的话，应当标为`noexcept`，这对**移动构造函数**尤为重要

### 移动和NRVO（Named Return Value Optimization）

在 C++11 之前，返回一个本地对象意味着这个对象会被拷贝，除非编译器发现可以做**返回值优化（named return value optimization，或 NRVO）**，能把对象直接构造到调用者的栈上。**从 C++11 开始，返回值优化仍可以发生，但在没有返回值优化的情况下，编译器将试图把本地对象移动出去，而不是拷贝出去**。这一行为不需要程序员手工用 std::move 进行干预——使用 std::move 对于移动行为没有帮助，反而会影响返回值优化。

例子：

``` cpp
#include <iostream>  // std::cout/endl
#include <utility>   // std::move

using namespace std;

class Obj {
public:
        Obj()
        {
                cout << "Obj()" << endl;
        }
        Obj(const Obj&)
        {
                cout << "Obj(const Obj&)" << endl;
        }
        Obj(Obj&&)
        {
                cout << "Obj(Obj&&)" << endl;
        }
};

Obj simple()
{ 
        Obj obj;
        //  简单返回对象；一般有 NRVO
        return obj;
}

Obj simple_with_move()
{ 
        Obj obj;
        // move会禁止 NRVO
        // 需要生成一个 Obj，给了一个 Obj&&，因此会调用构造函数，所以就是多产生了一次Obj(Obj&&) 的调用
        return std::move(obj);
}

Obj complicated(int n)
{ 
        Obj obj1;
        Obj obj2;
        //  有分支，一般无 NRVO
        if (n % 2 == 0) {
                return obj1;
        } else {
                return obj2;
        }
}

int main()
{
        cout << "*** 1 ***" << endl;
        auto obj1 = simple();
        cout << "*** 2 ***" << endl;
        auto obj2 = simple_with_move();
        cout << "*** 3 ***" << endl;
        auto obj3 = complicated(42);
}
/*
*** 1 ***
Obj()
*** 2 ***
Obj()
Obj(Obj&&)
*** 3 ***
Obj()
Obj()
Obj(Obj&&)
*/
```

### 引用坍缩（引用折叠）和完美转发

对于一个实际的类型`T`，它的左值引用是`T&`，右值引用是`T&&`。那么：

1. 是不是看到`T&`，就一定是个**左值引用**？ -- 是
2. 是不是看到`T&&`，就一定是个**右值引用**？ -- 否

关键在于，在有模板的代码里，对于类型参数的推导结果可能是引用。

* 对于 template foo(T&&) 这样的代码，如果传递过去的参数是**左值**，T 的推导结果是**左值引用**；如果传递过去的参数是**右值**，T 的推导结果是**参数的类型本身**。
* 如果 T 是**左值引用**，那 T&& 的结果仍然是**左值引用**，即 type& && 坍缩成了 type&。
* 如果 T 是一个**实际类型**，那 T&& 的结果自然就是一个**右值引用**。

事实上，很多标准库里的函数，连目标的参数类型都不知道，但我们仍然需要能够保持参数的值类别：左值的仍然是左值，右值的仍然是右值。这个功能在 C++ 标准库中已经提供了，叫 std::forward。它和 std::move 一样都是利用引用坍缩机制来实现。

因为在 T 是模板参数时，T&& 的作用主要是保持值类别进行转发，它有个名字就叫“转发引用”（forwarding reference）。因为既可以是左值引用，也可以是右值引用，它也曾经被叫做“万能引用”（universal reference）。


``` cpp
template <typename T>
void bar(T&& s)
{
  foo(std::forward<T>(s));
}
```


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

一个对象只能被单个`unique_ptr`所拥有；`shared_ptr`允许多个智能指针同时拥有一个对象，当它们全部都失效时（共享计数），这个对象也同时会被删除。以下实现一个类似标准[shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr)的智能指针，但是还缺少部分功能。

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

## 普通容器

### string

`string`是模板`basic_string`对于`char`类型的特化，可以认为是一个只存放字符`char`类型数据的容器。“真正”的容器类与`string`的最大不同点是里面可以存放**任意类型的对象**。

string的内存布局如下图所示：

![string_mem](/assets/images/201911/string_mem.png)

string 当然是为了存放字符串，和简单的 C 字符串不同：

* string 负责自动维护字符串的生命周期
* string 支持字符串的拼接操作（如 + 和 +=）
* string 支持字符串的查找操作（如 find 和 rfind）
* string 支持从 istream 安全地读入字符串（使用 getline）
* string 支持给期待 const char* 的接口传递字符串内容（使用 c_str）
* string 支持到数字的互转（stoi 系列函数和 to_string）
* etc.

**使用经验：**

* 推荐在代码中尽量使用`string`来管理字符串。不过，对于对外暴露的接口，情况有一点复杂。一般不建议在接口中使用`const string&`，除非确知调用者已经持有`string`：如果函数里不对字符串做复杂处理的话，使用`const char* `可以避免在调用者只有 C 字符串时编译器自动构造`string`，这种额外的构造和析构代价并不低。反过来，如果实现较为复杂、希望使用`string`的成员函数的话，那就应该考虑下面的策略
* 如果不修改字符串的内容，使用`const string&`或 C++17 的`string_view`作为参数类型。后者是最理想的情况，因为即使在只有 C 字符串的情况，也不会引发不必要的内存复制
* 如果需要在函数内修改字符串内容、但不影响调用者的该字符串，使用 string 作为参数类型（自动拷贝）
* 如果需要改变调用者的字符串内容，使用 string& 作为参数类型（通常不推荐）

``` cpp
string name;
cout << "What's your name? ";
getline(cin, name);
cout << "Nice to meet you, " << name << "!\n";
```

### vector

在实际应用中，把它当成**动态数组**更为合适。它基本相当于 Java 的 ArrayList 和 Python 的 list。

vector的内存布局如下图所示：

![vector_mem](/assets/images/201911/vector_mem.png)

vector 允许下面的操作（不完全列表）：
* 可以使用中括号的下标来访问其成员（同 string）
* 可以使用 data 来获得指向其内容的裸指针（同 string）
* 可以使用 capacity 来获得当前分配的存储空间的大小，以元素数量计（同 string）
* 可以使用 reserve 来改变所需的存储空间的大小，成功后 capacity() 会改变（同 string）
* 可以使用 resize 来改变其大小，成功后 size() 会改变（同 string）
* 可以使用 pop_back 来删除最后一个元素（同 string）
* 可以使用 push_back 在尾部插入一个元素（同 string）
* 可以使用 insert 在指定位置前插入一个元素（同 string）
* 可以使用 erase 在指定位置删除一个元素（同 string）
* 可以使用 emplace 在指定位置构造一个元素
* 可以使用 emplace_back 在尾部新构造一个元素

**使用经验：**

* vector 适合在尾部操作，这是它的内存布局决定的。**只有在尾部插入和删除时，其他元素才会不需要移动，除非内存空间不足导致需要重新分配内存空间**
* 当 push_back、insert、reserve、resize 等函数**导致内存重分配时**，或当 insert、erase **导致元素位置移动时**，**vector 会试图把元素“移动”到新的内存区域**。**vector 通常保证强异常安全性，如果元素类型没有提供一个保证不抛异常的移动构造函数，vector 通常会使用拷贝构造函数。因此，对于拷贝代价较高的自定义元素类型，我们应当定义移动构造函数，并标其为 `noexcept`，或只在容器中放置对象的智能指针**

> 注意：
> 1. **C++11 开始提供的`emplace…`系列函数是为了提升容器的性能而设计的。** 你可以试试把 v1.emplace_back() 改成 v1.push_back(Obj1())。对于 vector 里的内容，结果是一样的；但使用 push_back 会额外生成临时对象，多一次（移动或拷贝）构造和析构。如果是移动的情况，那会有小幅性能损失；**如果对象没有实现移动的话，那性能差异就可能比较大了。**
> 2. **现代处理器的体系架构使得对连续内存访问的速度比不连续的内存要快得多**。因而，vector 的连续内存使用是它的一大优势所在。当你不知道该用什么容器时，缺省就使用 vector 吧。
> 3. vector 的一个**主要缺陷是大小增长时导致的元素移动**。如果可能，尽早使用 reserve 函数为 vector 保留所需的内存，这在 vector 预期会增长很大时能带来很大的性能提升。

例子：Obj1 和 Obj2 的定义只差了一个`noexcept`，但这个小小的差异就导致了 vector **是否会移动对象**，这点非常重要。

``` cpp
#include <iostream>
#include <vector>
using namespace std;

class Obj1 {
public:
        Obj1() {
                cout << "Obj1()\n";
        }
        Obj1(const Obj1&) {
                cout << "Obj1(const Obj1&)\n";
        }
        Obj1(Obj1&&) {
                cout << "Obj1(Obj1&&)\n";
        }
};

class Obj2 {
public:
        Obj2() {
                cout << "Obj2()\n";
        }
        Obj2(const Obj2&) {
                cout << "Obj2(const Obj2&)\n";
        }
        Obj2(Obj2&&) noexcept {
                cout << "Obj2(Obj2&&)\n";
        }
};

int main()
{
        vector<Obj1> v1;
        v1.reserve(1);      // 没有内存分配
        v1.emplace_back();  // 构造第一个对象
        v1.emplace_back();  // 构造第二个对象，并拷贝第一个对象（移动构造函数没有声明 noexcept）

        vector<Obj2> v2;
        v2.reserve(2);      
        v2.emplace_back();  // 构造第一个对象
        v2.emplace_back();  // 构造第二个对象
        v2.emplace_back();  // 构造第三个对象，并移动第一个和第二个对象（由于移动构造函数声明了 noexcept）
}
/*
Obj1()
Obj1()
Obj1(const Obj1&)
Obj2()
Obj2()
Obj2()
Obj2(Obj2&&)
Obj2(Obj2&&)
*/
```

### deque

`deque`的意思是`double-ended queue`，**双端队列**。它主要是用来满足下面这个需求：

* 容器不仅可以从**尾部**自由地添加和删除元素，也可以从**头部**自由地添加和删除。
* deque 提供 push_front、emplace_front 和 pop_front 成员函数。
* deque 不提供 data、capacity 和 reserve 成员函数。

deque 的内存布局如下图所示：

![deque_mem](/assets/images/201911/deque_mem.png)

* 如果只从头、尾两个位置对 deque 进行增删操作的话，容器里的对象永远不需要移动
* 容器里的元素只是部分连续的（因而没法提供 data 成员函数）
* 由于元素的存储大部分仍然连续，它的遍历性能是比较高的
* 由于每一段存储大小相等，deque 支持使用下标访问容器元素，大致相当于 `index[i / chunk_size][i % chunk_size]`，也保持高效
* 如果你需要一个经常在头尾增删元素的容器，那 deque 会是个合适的选择

### list

list 在 C++ 里代表**双向链表**。和 vector 相比，它**优化了在容器中间的插入和删除**：

* list 提供**高效的 O(1) 复杂度的任意位置的插入和删除操作**。
* list 不提供使用下标访问其元素。
* list 提供 push_front、emplace_front 和 pop_front 成员函数（和 deque 相同）。
* list 不提供 data、capacity 和 reserve 成员函数（和 deque 相同）。

list 的内存布局如下图所示：

![list_mem](/assets/images/201911/list_mem.png)

**使用经验：**

* 虽然 list 提供了任意位置插入新元素的灵活性，但由于每个元素的内存空间都是单独分配、不连续，它的遍历性能比 vector 和 deque 都要低。这在很大程度上抵消了它在插入和删除操作时不需要移动元素的理论性能优势。如果你不太需要遍历容器、又需要在中间频繁插入或删除元素，可以考虑使用 list。
* 因为某些标准算法在 list 上会导致问题，list 提供了成员函数作为替代，比如：merge，remove，remove_if，reverse，sort，unique。

``` cpp
list lst{1, 7, 2, 8, 3};
vector vec{1, 7, 2, 8, 3};

sort(vec.begin(), vec.end());     //  正常
// sort(lst.begin(), lst.end());  //  会出错
lst.sort();                       //  正常
```

### forward_list

既然 list 是**双向链表**，那么 C++ 里有没有**单向链表**呢？答案是肯定的。从 C++11 开始，**前向列表** forward_list 成了标准的一部分。

forward_list 的内存布局如下图所示：

![forward_list_mem](/assets/images/201911/forward_list_mem.png)

为什么会需要这么一个阉割版的 list 呢？原因是，在元素大小较小的情况下，forward_list 能节约的内存是非常可观的；在列表不长的情况下，不能反向查找也不是个大问题。提高内存利用率，往往就能提高程序性能，更不用说在内存可能不足时的情况了。

### queue（类容器）

queue的特别点在于它不是完整的实现，而是依赖于某个现有的容器，因而被称为**容器适配器（container adaptor）**。

queue 缺省用 deque 来实现。它的接口跟 deque 比，有如下改变：

* 不能按下标访问元素
* 没有 begin、end 成员函数
* 用 emplace 替代了 emplace_back，用 push 替代了 push_back，用 pop 替代了 pop_front；没有其他的 push_…、pop_…、emplace…、insert、erase 函数

它的实际内存布局当然是随底层的容器而定的。从概念上讲，它的结构可如下所示：

![queue_mem](/assets/images/201911/queue_mem.png)

鉴于 queue 不提供 begin 和 end 方法，无法无损遍历：

``` cpp
#include <iostream>
#include <queue>

int main()
{
  std::queue<int> q;
  q.push(1);
  q.push(2);
  q.push(3);
  while (!q.empty()) {
    std::cout << q.front()
              << std::endl;
    q.pop();
  }
}
```

### stack（类容器）

类似地，栈 stack 是后进先出（LIFO）的数据结构。

stack 缺省也是用 deque 来实现，但它的概念和 vector 更相似。它的接口跟 vector 比，有如下改变：

* 不能按下标访问元素
* 没有 begin、end 成员函数
* back 成了 top，没有 front
* 用 emplace 替代了 emplace_back，用 push 替代了 push_back，用 pop 替代了 pop_back；没有其他的 push_…、pop_…、emplace…、insert、erase 函数

一般图形表示法会把 stack 表示成一个竖起的 vector：

![stack_mem](/assets/images/201911/stack_mem.png)

``` cpp
#include <iostream>
#include <stack>

int main()
{
  std::stack<int> s;
  s.push(1);
  s.push(2);
  s.push(3);
  while (!s.empty()) {
    std::cout << s.top()
              << std::endl;
    s.pop();
  }
}
```

## 需要函数对象的容器

### 函数对象及其特化

首先来讨论一下两个重要的函数对象，`less` 和 `hash`。在标准库里，通用的 `less` 大致是这样定义的：

``` cpp
template <class T>
struct less
  : binary_function<T, T, bool> {
  bool operator()(const T& x,
                  const T& y) const
  {
    return x < y;
  }
};
```

* less 是一个**函数对象**，并且是个**二元函数**，执行对任意类型的值的比较，返回布尔类型。**作为函数对象，它定义了函数调用运算符（operator()）**，并且缺省行为是对指定类型的对象进行 < 的比较操作。
* 在需要大小比较的场合，C++ 通常默认会使用 less（如果需要产生相反的顺序的话，则可以使用 greater）。

计算哈希值的函数对象 hash，它的目的**是把一个某种类型的值转换成一个无符号整数哈希值，类型为 size_t**。它没有一个可用的默认实现。对于常用的类型，系统提供了需要的特化。

例如，int类型的特化：

``` cpp
template <class T> struct hash;

template <>
struct hash<int>
  : public unary_function<int, size_t> {
  size_t operator()(int v) const
    noexcept
  {
    return static_cast<size_t>(v);
  }
};
```

更复杂的类型，如指针或者 string 的特化。**要点是，对于每个类，类的作者都可以提供 hash 的特化，使得对于不同的对象值，函数调用运算符都能得到尽可能均匀分布的不同数值**。

``` cpp
#include <algorithm>   // std::sort
#include <functional>  // std::less/greater/hash
#include <iostream>    // std::cout/endl
#include <string>      // std::string
#include <vector>      // std::vector
#include "output_container.h"

using namespace std;

int main()
{
  //  初始数组
  vector<int> v{13, 6, 4, 11, 29};
  cout << v << endl;

  //  从小到大排序
  sort(v.begin(), v.end());
  cout << v << endl;

  //  从大到小排序
  sort(v.begin(), v.end(),
       greater<int>());
  cout << v << endl;

  cout << hex;

  auto hp = hash<int*>();
  cout << "hash(nullptr)  = "
       << hp(nullptr) << endl;
  cout << "hash(v.data()) = "
       << hp(v.data()) << endl;
  cout << "v.data()       = "
       << static_cast<void*>(v.data())
       << endl;

  auto hs = hash<string>();
  cout << "hash(\"hello\")  = "
       << hs(string("hello")) << endl;
  cout << "hash(\"hellp\")  = "
       << hs(string("hellp")) << endl;
}
/*
{ 13, 6, 4, 11, 29 }
{ 4, 6, 11, 13, 29 }
{ 29, 13, 11, 6, 4 }
hash(nullptr)  = 0
hash(v.data()) = 55fcc1441e70
v.data()       = 0x55fcc1441e70
hash("hello")  = 26553298fdbe39c8
hash("hellp")  = 62ca8a73f37cbb7b
*/
```

以上结果为在gcc下编译，若改为MSVC下编译则为：

```
hash(nullptr) = a8c7f832281a39c5
hash(v.data()) = 7a0bdfd7df0923d2
v.data() = 000001EFFB10EAE0
hash("hello") = a430d84680aabd0b
hash("hellp") = a430e54680aad322
```

可以看到，在MSVC的实现里，空指针的哈希值是一个非零的数值，指针的哈希值也和指针的数值不一样。要注意不同的实现处理的方式会不一样。事实上，测试结果是 GCC、Clang 和 MSVC 对常见类型的哈希方式都各有不同。

### priority_queue

priority_queue 也是一个**容器适配器**。但是，它用到了**比较函数对象**（默认是 less）。

* 它和 stack 相似，支持 push、pop、top 等有限的操作，但容器内的顺序既不是后进先出，也不是先进先出，**而是（部分）排序的结果**。
* 在使用缺省的 less 作为其 Compare 模板参数时，最大的数值会出现在容器的“顶部”。如果需要最小的数值出现在容器顶部，则可以传递 greater 作为其 Compare 模板参数。

``` cpp
#include <functional>  // std::greater
#include <iostream>    // std::cout/endl
#include <memory>      // std::pair
#include <queue>       // std::priority_queue
#include <vector>      // std::vector
#include "output_container.h"

using namespace std;

int main()
{
  priority_queue<
    pair<int, int>,
    vector<pair<int, int>>,
    greater<pair<int, int>>>
    q;
  q.push({1, 1});
  q.push({2, 2});
  q.push({0, 3});
  q.push({9, 4});
  while (!q.empty()) {
    cout << q.top() << endl;
    q.pop();
  }
}
/*
(0, 3)
(1, 1)
(2, 2)
(9, 4)
*/
```

### 关联容器

关联容器有 `set`（集合）、`map`（映射）、`multiset`（多重集）和 `multimap`（多重映射）。跳出 C++ 的语境，`map`（映射）的更常见的名字是**关联数组和字典**，而在 `JSON` 里直接被称为**对象（object）**。在 C++ 外这些容器常常是**无序的**，而在 C++ 里关联容器则被认为是**有序的**。

* 关联容器是一种有序的容器。
* 名字带“multi”的**允许键重复**，不带的**不允许键重复**。`set` 和 `multiset` 只能用来存放键，而 `map` 和 `multimap` 则存放一个个键值对。
* 与序列容器相比，关联容器没有前、后的概念及相关的成员函数，但同样提供 insert、emplace 等成员函数。此外，关联容器都有 find、lower_bound、upper_bound 等查找函数，结果是一个迭代器。
    + find(k) 可以找到任何一个等价于查找键 k 的元素（!(x < k || k < x)）
    + lower_bound(k) 找到第一个**不小于（ =< ）**查找键 k 的元素（!(x < k)）
    + upper_bound(k) 找到第一个**大于（ > ）**查找键 k 的元素（k < x）
* 如果需要在 `multimap` 里精确查找满足某个键的区间的话，建议使用 `equal_range`，可以一次性取得上下界（半开半闭）。
* 如果在声明关联容器时没有提供比较类型的参数，缺省使用 less 来进行排序。如果键的类型提供了比较算符 < 的重载，我们不需要做任何额外的工作。否则，我们就需要对键类型进行 less 的特化，或者提供一个其他的函数对象类型。
* 对于自定义类型，推荐尽量使用标准的 less 实现，通过重载 <（及其他标准比较运算符）对该类型的对象进行排序。存储在关联容器中的键一般应满足**严格弱序关系（strict weak ordering）**。

### 无序关联容器

通过比较来进行查找、插入和删除，复杂度为对数 **O(log(n))**，**有没有达到更好的性能的方法？**

从 C++11 开始，每一个关联容器都有一个对应的无序关联容器，它们是：

unordered_set，unordered_map，unordered_multiset，unordered_multimap

这些容器和关联容器非常相似，**主要的区别就在于它们是“无序”的。这些容器不要求提供一个排序的函数对象，而要求一个可以计算哈希值的函数对象**。你当然可以在声明容器对象时手动提供这样一个函数对象类型，但更常见的情况是，我们使用标准的 hash 函数对象及其特化。

``` cpp
#include <complex>        // std::complex
#include <iostream>       // std::cout/endl
#include <unordered_map>  // std::unordered_map
#include <unordered_set>  // std::unordered_set
#include "output_container.h"

using namespace std;

namespace std {

template <typename T>
struct hash<complex<T>> {
  size_t
  operator()(const complex<T>& v) const
    noexcept
  {
    hash<T> h;
    return h(v.real()) + h(v.imag());
  }
};

}  // namespace std

int main()
{
  unordered_set<int> s{
    1, 1, 2, 3, 5, 8, 13, 21
  };
  cout << s << endl;

  unordered_map<complex<double>,
                double>
    umc{{{1.0, 1.0}, 1.4142},
        {{3.0, 4.0}, 5.0}};
  cout << umc << endl;
}
```

输出可能是（顺序不能保证）：

```
{ 21, 5, 8, 3, 13, 2, 1 }
{ (3,4) => 5, (1,1) => 1.4142 }
```

* 请注意我们在 std 名空间中添加了特化，这是少数用户可以向 std 名空间添加内容的情况之一。正常情况下，向 std 名空间添加声明或定义是禁止的，属于未定义行为。
* 从实际的工程角度，无序关联容器的主要优点在于其性能。关联容器和 priority_queue 的插入和删除操作，以及关联容器的查找操作，其复杂度都是 `O(log(n))`，而无序关联容器的实现使用哈希表，可以达到平均 `O(1)`！**但这取决于我们是否使用了一个好的哈希函数：在哈希函数选择不当的情况下，无序关联容器的插入、删除、查找性能可能成为最差情况的 O(n)，那就比关联容器糟糕得多了。**


### array

`array`容器是**C 数组**的替代品。C 数组在 C++ 里继续存在，主要是为了保留和 C 的向后兼容性。C 数组本身和 C++ 的容器相差是非常大的。



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
* [My small collection of C++ utilities](https://github.com/adah1972/nvwa)


# 文章

* [6 Tips to supercharge C++11 vector performance](https://www.acodersjourney.com/6-tips-supercharge-cpp-11-vector-performance/)




