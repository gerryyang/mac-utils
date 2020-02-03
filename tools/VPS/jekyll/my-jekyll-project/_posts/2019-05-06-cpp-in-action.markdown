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

编译器对标准的支持情况，可见 [https://en.cppreference.com/w/cpp/compiler_support](https://en.cppreference.com/w/cpp/compiler_support)。

通过工具 [https://cppinsights.io/](https://cppinsights.io/) 可以展示实例化的过程。


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

然后添加一些成员函数（解引用操作符`*`, 箭头操作符`->`, 布尔表达式），从而可以用类似内置的指针方式使用其对象。

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

  unordered_map< complex<double>, double > umc{ { {1.0, 1.0}, 1.4142}, { {3.0, 4.0}, 5.0} };

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

`array`容器是**C 数组**的替代品。C 数组在 C++ 里继续存在，主要是为了保留和 C 的向后兼容性。C 数组本身和 C++ 的容器相差是非常大的：

* C 数组没有 begin 和 end 成员函数（虽然可以使用全局的 begin 和 end 函数）
* C 数组没有 size 成员函数（得用一些模板技巧来获取其长度）
* C 数组作为参数有退化行为，传递给另外一个函数后那个函数不再能获得 C 数组的长度和结束位置

获得数组的长度：

``` cpp
#define ARRAY_LEN(a) \
  (sizeof(a) / sizeof((a)[0]))
```

C++17 直接提供了一个 `size` 方法，可以用于提供数组长度，并且在数组退化成指针的情况下会直接失败：

``` cpp
#include <iostream>  // std::cout/endl
#include <iterator>  // std::size

void test(int arr[])
{
  //  不能编译
  // std::cout << std::size(arr)
  //           << std::endl;
}

int main()
{
  int arr[] = {1, 2, 3, 4, 5};
  std::cout << "The array length is "
            << std::size(arr)
            << std::endl;
  test(arr);
}
```

如果不用 C 数组的话，该用什么来替代？

* 如果数组较大的话，应该考虑 vector。vector 有最大的灵活性和不错的性能。
* 对于字符串数组，当然应该考虑 string。
* 如果数组大小固定（C 的数组在 C++ 里本来就是大小固定的）并且较小的话，应该考虑 array。array 保留了 C 数组在栈上分配的特点，同时，提供了 begin、end、size 等通用成员函数。

``` cpp
#include <array>     // std::array
#include <iostream>  // std::cout/endl
#include <map>       // std::map
#include "output_container.h"

typedef std::array<char, 8> mykey_t;

int main()
{
  std::map<mykey_t, int> mp;
  mykey_t mykey{"hello"};
  mp[mykey] = 5;  // OK
  std::cout << mp << std::endl;
}
```

## 异常

异常，用还是不用，这是个问题。

首先，开宗明义，如果你不知道到底该不该用异常的话，那答案就是**该用**。**如果你需要避免使用异常，原因必须是你有明确的需要避免使用异常的理由**。

### 没有异常的世界（C）

我们可能有大量需要判断错误的代码，零散分布在代码各处。

``` cpp
  matrix c;

  //  不清零的话，错误处理和资源清理会更复杂
  memset(c, 0, sizeof(matrix));

  errcode = matrix_multiply(c, a, b);
  if (errcode != MATRIX_SUCCESS) {
    goto error_exit;
  }
  //  使用乘法的结果做其他处理

error_exit:
  matrix_dealloc(&c);
  return errcode;
```

上面还只展示了单层的函数调用。事实上，如果出错位置离处理错误的位置相差很远的话，每一层的函数调用里都得有判断错误码的代码，这就既对写代码的人提出了严格要求，也对读代码的人造成了视觉上的干扰。

上面是 C 代码无法用异常，如果是 C++ 用异常可以改善吗？（当然可以，但你会发现结果好不了多少）

### 使用异常（C++）

* 异常处理并不意味着需要写显式的 try 和 catch。**异常安全的代码，可以没有任何 try 和 catch。**
* **异常安全**，是指当异常发生时，既不会发生资源泄漏，系统也不会处于一个不一致的状态。
* 只要我们适当地组织好代码、利用好 RAII，实现的代码都可以更短、更清晰。我们可以统一在外层某个地方处理异常——通常会记日志、或在界面上向用户报告错误了。

### 避免异常的风格指南？

但大名鼎鼎的 Google 的 C++ 风格指南不是说**要避免异常吗**？这又是怎么回事呢？

> Given that Google’s existing code is not exception-tolerant, the costs of using exceptions are somewhat greater than the costs in a new project. The conversion process would be slow and error-prone. We don’t believe that the available alternatives to exceptions, such as error codes and assertions, introduce a significant burden.Our advice against using exceptions is not predicated on philosophical or moral grounds, but practical ones. Because we’d like to use our open-source projects at Google and it’s difficult to do so if those projects use exceptions, we need to advise against exceptions in Google open-source projects as well. Things would probably be different if we had to do it all over again from scratch.
> 鉴于 Google 的现有代码不能承受异常，**使用异常的代价要比在全新的项目中使用异常大一些**。转换[代码来使用异常的]过程会缓慢而容易出错。我们不认为可代替异常的方法，如错误码或断言，会带来明显的负担。我们反对异常的建议并非出于哲学或道德的立场，而是出于实际考虑。因为我们希望在 Google 使用我们的开源项目，而如果这些项目使用异常的话就会对我们的使用带来困难，我们也需要反对在 Google 的开源项目中使用异常。**如果我们从头再来一次的话，事情可能就会不一样了**。

这个如果还比较官方、委婉的话，Reddit 上还能找到一个更个人化的表述：

> I use [sic] to work at Google, and Craig Silverstein, who wrote the first draft of the style guideline, said that he regretted the ban on exceptions, but he had no choice; when he wrote it, it wasn’t only that the compiler they had at the time did a very bad job on exceptions, but that they already had a huge volume of non-exception-safe code.
> 我过去在 Google 工作，写了风格指南初稿的 Craig Silverstein 说过**他对禁用异常感到遗憾**，但他当时别无选择。在他写风格指南的时候，不仅**他们使用的编译器在异常上工作得很糟糕，而且他们已经有了一大堆异常不安全的代码了**。

当然，除了历史原因以外，也有出于性能等其他原因禁用异常的。美国国防部的联合攻击战斗机（JSF）项目的 C++ 编码规范就禁用异常，因为工具链不能保证抛出异常时的实时性能。不过在那种项目里，被禁用的 C++ 特性就多了，比如动态内存分配都不能使用。

一些游戏项目为了追求高性能，也禁用异常。这个实际上也有一定的历史原因，**因为今天的主流 C++ 编译器，在异常关闭和开启时应该已经能够产生性能差不多的代码（在异常未抛出时）。代价是产生的二进制文件大小的增加，因为异常产生的位置决定了需要如何做栈展开，这些数据需要存储在表里**。典型情况，**使用异常和不使用异常比，二进制文件大小会有约`百分之十到二十`的上升**。[LLVM 项目的编码规范里就明确指出这是不使用 RTTI 和异常的原因](https://llvm.org/docs/CodingStandards.html#do-not-use-rtti-or-exceptions)。

> In an effort to reduce code and executable size, LLVM does not use RTTI (e.g. dynamic_cast<>;) or exceptions.

你得想想是否值得这么去做。你的项目对二进制文件的大小和性能有这么渴求吗？需要这么去拼吗？

### 异常的问题

异常当然不是一个完美的特性，否则也不会招来这些批评和禁用了。对它的批评主要有两条：

1. **异常违反了“你不用就不需要付出代价”的 C++ 原则。只要开启了异常，即使不使用异常你编译出的二进制代码通常也会膨胀。**
2. **异常比较隐蔽，不容易看出来哪些地方会发生异常和发生什么异常。**

解释：

GCC/Clang 下的 `-fexceptions`（缺省开启）。用 GCC，加上 `-fno-exceptions` 命令行参数，对于下面这样的小程序，也能看到产生的可执行文件的大小的变化。

``` cpp
#include <vector>
int main()
{
    std::vector<int> v{1, 2, 3, 4, 5};
    v.push_back(20);
}
```

1. 对于第一条，开发者没有什么可做的。事实上，这也算是 C++ 实现的一个折中了。目前的主流异常实现中，都倾向于牺牲可执行文件大小、提高主流程（happy path）的性能。[只要程序不抛异常，C++ 代码的性能比起完全不做错误检查的代码，都只有几个百分点的性能损失](https://isocpp.org/wiki/faq/exceptions)。除了非常有限的一些场景，可执行文件大小通常不会是个问题。
2. 第二条可以算作是一个真正有效的批评。和 Java 不同，C++ 里不会对异常规约进行编译时的检查。**从 C++17 开始，C++ 甚至完全禁止了以往的动态异常规约，你不再能在函数声明里写你可能会抛出某某异常。你唯一能声明的，就是某函数不会抛出异常——noexcept、noexcept(true) 或 throw()。这也是 C++ 的运行时唯一会检查的东西了。如果一个函数声明了不会抛出异常、结果却抛出了异常，C++ 运行时会调用 std::terminate 来终止应用程序**。不管是程序员的声明，还是编译器的检查，都不会告诉你哪些函数会抛出哪些异常。**当然，不声明异常是有理由的。特别是在泛型编程的代码里，几乎不可能预知会发生些什么异常**。

对避免异常带来的问题有几点建议：

* 写异常安全的代码，尤其在模板里。可能的话，[提供强异常安全保证](https://en.cppreference.com/w/cpp/language/exceptions)，在任何第三方代码发生异常的情况下，不改变对象的内容，也不产生任何资源泄漏。
* 如果你的代码可能抛出异常的话，在文档里明确声明可能发生的异常类型和发生条件。确保使用你的代码的人，能在不检查你的实现的情况，了解需要准备处理哪些异常。
* 对于肯定不会抛出异常的代码，将其标为 noexcept。注意类的特殊成员（构造函数、析构函数、赋值函数等）会自动成为 noexcept，如果它们调用的代码都是 noexcept 的话。所以，像 swap 这样的成员函数应当尽可能标成 noexcept。

### 使用异常的理由

**异常是渗透在 C++ 中的标准错误处理方式。标准库的错误处理方式就是异常**。其中不仅包括运行时错误，甚至包括一些逻辑错误。比如，在说容器的时候，在能使用 [] 运算符的地方，C++ 的标准容器也提供了 at 成员函数，能够在下标不存在的时候抛出异常，作为一种额外的帮助调试的手段。

``` cpp
vector<int> v{1, 2, 3};
int a = v[0];
int b = v.at(0)

int c = v[3];    // no exception, 越界的数值

// at函数会抛异常
try {
  c = v.at(3);
}
catch (const out_of_range& e) {
  cerr << e.what() << endl;
}
```

* **C++ 的标准容器在大部分情况下提供了强异常保证，即，一旦异常发生，现场会恢复到调用函数之前的状态，容器的内容不会发生改变，也没有任何资源泄漏**。前面提到过，vector 会在元素类型没有提供保证不抛异常的**移动构造函数**的情况下，在移动元素时会使用**拷贝构造函数**。这是因为一旦某个操作发生了异常，被移动的元素已经被破坏，处于只能析构的状态，异常安全性就不能得到保证了。
* **只要你使用了标准容器，不管你自己用不用异常，你都得处理标准容器可能引发的异常**。至少有 bad_alloc，除非你明确知道你的目标运行环境不会产生这个异常。
* 虽然对于运行时错误，开发者并没有什么选择余地；但对于代码中的逻辑错误，开发者则是可以选择不同的处理方式的：你**可以使用异常，也可以使用 assert**，在调试环境中报告错误并中断程序运行。由于测试通常不能覆盖所有的代码和分支，assert 在发布模式下一般被禁用，两者并不是完全的替代关系。在允许异常的情况下，使用异常可以获得在调试和发布模式下都良好、一致的效果。
* 标准 C++ 可能会产生哪些异常，可以查看[参考资料](https://zh.cppreference.com/w/cpp/error/exception)。


## 迭代器

[迭代器](https://en.cppreference.com/w/cpp/iterator)是一个很通用的概念，并不是一个特定的类型。**它实际上是一组对类型的要求**。

迭代器通常是对象。但需要注意的是，**`指针`可以满足上面所有的迭代器要求，因而也是迭代器**。这应该并不让人惊讶，**因为本来迭代器就是根据指针的特性，对其进行抽象的结果**。事实上，vector 的迭代器，在很多实现里就直接是使用指针的。

迭代器的**基本要求**是：

* 对象可以被`拷贝构造`、`拷贝赋值`和`析构`。
* 对象支持 `*` 运算符。
* 对象支持前置 `++` 运算符。

### 常用迭代器

最常用的迭代器就是容器的 `iterator` 类型了。以顺序容器为例，它们都定义了嵌套的 `iterator` 类型和 `const_iterator` 类型。一般而言，iterator 可写入，const_iterator 类型不可写入，但这些迭代器都被定义为**输入迭代器**或其**派生类型**：

* vector::iterator 和 array::iterator 可以满足到**连续迭代器**
* deque::iterator 可以满足到**随机访问迭代器**（记得它的内存只有部分连续）
* list::iterator 可以满足到**双向迭代器**（链表不能快速跳转）
* forward_list::iterator 可以满足到**前向迭代器**（单向链表不能反向遍历）

如果一个类型像输入迭代器，但 `*i` 只能作为左值来写而不能读，那它就是个**输出迭代器（output iterator）**。

``` cpp
#include <algorithm>  // std::copy
#include <iterator>   // std::back_inserter
#include <vector>     // std::vector
#include <iostream>   // std::cout
using namespace std;

vector<int> v1{1, 2, 3, 4, 5};
vector<int> v2;
copy(v1.begin(), v1.end(), back_inserter(v2));

// output v2 is { 1, 2, 3, 4, 5 }

copy(v2.begin(), v2.end(), ostream_iterator<int>(cout, " "));
```

* 输出迭代器是`back_inserter` 返回的类型 `back_inserter_iterator` ，用它可以很方便地在容器的尾部进行插入操作。
* 输出迭代器`ostream_iterator`，方便把容器内容“拷贝”到一个输出流。

总结：

``` cpp
cout << *it  // 输入迭代器，就是读
*it = 42     // 输出迭代器，就是写
```

### 使用输入行迭代器（例子）

通过自定义的输入迭代器。它的功能本身很简单，就是把一个输入流（istream）的内容一行行读进来。配上 C++11 引入的基于范围的 for 循环的语法，我们可以把遍历输入流的代码以一种自然、非过程式的方式写出来。

``` cpp
for (const string& line : istream_line_reader(is)) {
  //  示例循环体中仅进行简单输出
  cout << line << endl;
}
```

对比一下以传统的方式写的 C++ 代码，其中需要照顾不少细节：(从 is 读入输入行的逻辑，在前面的代码里一个语句就全部搞定了，在这儿用了 5 个语句)

``` cpp
string line;
for (;;) {
  getline(is, line);
  if (!is) {
    break;
  }
  cout << line << endl;
}
```

基于范围的 for 循环这个语法。虽然这可以说是个语法糖，但它对提高代码的可读性真的非常重要。如果不用这个语法糖的话，简洁性上的优势就小多了。我们直接把这个循环改写成等价的普通 for 循环的样子。

``` cpp
{
  // auto&& 是用一个“万能”引用捕获一个对象，左值和右值都可以。C++ 的生命期延长规则，保证了引用有效期间，istream_line_reader 这个“临时”对象一直存在。没有生命期延长的话，临时对象在当前语句执行结束后即销毁
  auto&& r = istream_line_reader(is);
  auto it = r.begin();
  auto end = r.end();
  for (; it != end; ++it) {
    const string& line = *it;
    cout << line << endl;
  }
}
```

### 定义输入行迭代器（例子）

如何实现这个输入行迭代器？

C++ 里有些固定的类型要求规范。对于一个迭代器，我们需要定义下面的类型：

``` cpp
class istream_line_reader {
public:
  class iterator {  //  实现  InputIterator
  public:
    typedef ptrdiff_t difference_type;
    typedef string value_type;
    typedef const value_type* pointer;
    typedef const value_type& reference;
    typedef input_iterator_tag iterator_category;
    …
  };
  …
};
```

仿照一般的容器，我们把迭代器定义为 istream_line_reader 的嵌套类。它里面的这**五个类型是必须定义的（其他泛型 C++ 代码可能会用到这五个类型**。[之前标准库定义了一个可以继承的类模板 std::iterator 来产生这些类型定义，但这个类目前已经被废弃](https://www.fluentcpp.com/2018/05/08/std-iterator-deprecated/)）。其中：

* `difference_type` 是代表迭代器之间距离的类型，定义为 `ptrdiff_t` 只是种标准做法（指针间差值的类型），对这个类型没什么特别作用。
* `value_type` 是迭代器指向的对象的值类型，我们使用 `string`，表示迭代器指向的是字符串。
* `pointer` 是迭代器指向的对象的指针类型，这儿就平淡无奇地定义为 `value_type` 的常指针了（我们可不希望别人来更改指针指向的内容）。
* 类似的，reference 是 value_type 的常引用。
* `iterator_category` 被定义为 `input_iterator_tag`，标识这个迭代器的类型是 input iterator（输入迭代器）。

一种实现方式如下：

让 `++` 负责读取，`*` 负责返回读取的内容。这个 iterator 类需要有一个数据成员指向输入流，一个数据成员来存放读取的结果。

``` cpp
class istream_line_reader {
public:
  class iterator {
    …
    iterator() noexcept
      : stream_(nullptr) {}
    explicit iterator(istream& is)
      : stream_(&is)
    {
      ++*this;   // 注意，调用 前置++，读取内容，保证构造后可以通过 * 获取内容
    }

    reference operator*() const noexcept
    {
      return line_;
    }
    pointer operator->() const noexcept
    {
      return &line_;
    }

    // 前置++
    iterator& operator++()
    {
      getline(*stream_, line_);
      if (!*stream_) {
        stream_ = nullptr;
      }
      return *this;
    }

    // 后置++，通过 前置++ 和 拷贝构造 实现（迭代器要求前置和后置 ++ 都要定义）
    iterator operator++(int)
    {
      iterator temp(*this);
      ++*this;
      return temp;
    }

  private:
    istream* stream_;
    string line_;
  };
  …
};
```

* 定义了默认构造函数，将 stream_ 清空
* 在带参数的构造函数里，根据传入的输入流来设置 stream_
* 定义了 `*` 和 `->` 运算符来取得迭代器指向的文本行的引用和指针
* 用 `++` 来读取输入流的内容（`后置 ++` 则以惯常方式使用`前置 ++` 和`拷贝构造`来实现）

注意：唯一“特别”点的地方，是在构造函数里调用了 `++`，确保在构造后调用 `*` 运算符时可以读取内容，符合日常先使用 `*`、再使用 `++` 的习惯。一旦文件读取到尾部（或出错），则 stream_ 被清空，回到默认构造的情况。

对于迭代器之间的比较，则主要考虑文件有没有读到尾部的情况，简单定义为：

``` cpp
    bool operator==(const iterator& rhs) const noexcept
    {
      return stream_ == rhs.stream_;
    }

    bool operator!=(const iterator& rhs) const noexcept
    {
      return !operator==(rhs);
    }
```

有了这个 iterator 的定义后，istream_line_reader 的定义就简单得很了：

``` cpp
class istream_line_reader {
public:
  class iterator {…};

  istream_line_reader() noexcept
    : stream_(nullptr) {}
  explicit istream_line_reader(
    istream& is) noexcept
    : stream_(&is) {}

  iterator begin()
  {
    return iterator(*stream_);
  }

  iterator end() const noexcept
  {
    return iterator();
  }

private:
  istream* stream_;
};
```

* 构造函数只是简单地把输入流的指针赋给 stream_ 成员变量
* begin 成员函数则负责构造一个真正有意义的迭代器
* end 成员函数则只是返回一个默认构造的迭代器而已

以上就是一个完整的基于输入流的行迭代器了。这个行输入模板的设计动机和性能测试结果可参见参考资料 [Python yield and C++ coroutines](https://yongweiwu.wordpress.com/2016/08/16/python-yield-and-cplusplus-coroutines/) 和 [Performance of my line readers](https://yongweiwu.wordpress.com/2016/11/12/performance-of-my-line-readers/)；完整的工程可用代码，请参见[https://github.com/adah1972/nvwa/](https://github.com/adah1972/nvwa/)。该项目中还提供了利用 C 文件接口的 file_line_reader 和基于内存映射文件的 mmap_line_reader。

注意以上实现存在一定的使用限制，不能多次调用begin。

``` cpp
#include <fstream>
#include <iostream>
#include "istream_line_reader.h"

using namespace std;

int main()
{
    ifstream ifs{"test.cpp"};
    istream_line_reader reader{ifs};
    auto begin = reader.begin();
    for (auto it = reader.begin();
         it != reader.end(); ++it) {
        cout << *it << '\n';
    }
}

// 以上代码，因为 begin 多调用了一次，输出就少了一行……
```

## C++易用性改进

### auto

`auto` 自动类型推断，顾名思义，就是编译器能够根据表达式的类型，自动决定变量的类型（从 C++14 开始，还有函数的返回类型），不再需要程序员手工声明。**但需要说明的是，auto 并没有改变 C++ 是静态类型语言这一事实——使用 auto 的变量（或函数返回值）的类型仍然是编译时就确定了，只不过编译器能自动帮你填充而已**。

```cpp
// 完整的写法
vector<int> v;
for (vector<int>::iterator
       it = v.begin(),
       end = v.end();
     it != end; ++it) {
  //  循环体
}

// auto的简化写法
for (auto it = v.begin(), end = v.end();
     it != end; ++it) {
  //  循环体
}
```

不使用自动类型推断时，**如果容器类型未知的话，还需要加上 typename**：

``` cpp
template <typename T>
void foo(const T& container)
{
  for (typename T::const_iterator
         it = v.begin(),
         // …
      ) {}
}
```

### decltype

`decltype` 的用途是获得一个表达式的类型，结果可以跟类型一样使用。它有两个基本用法：

* `decltype(变量名)` 可以获得变量的精确类型。
* `decltype(表达式)` （表达式不是变量名，但包括 decltype((变量名)) 的情况）可以获得表达式的**引用类型**；除非表达式的结果是个纯右值（prvalue），此时结果仍然是值类型。

例如：`int a;`，那么：

* decltype(a) 会获得 int（因为 a 是 int）
* decltype((a)) 会获得 int&（因为 a 是 lvalue）
* decltype(a + a) 会获得 int（因为 a + a 是 prvalue）

### decltype(auto)

decltype(expr) 既可以是值类型，也可以是引用类型。

``` cpp
decltype(expr) a = expr;
```

这种写法明显不能让人满意，特别是表达式很长的情况（而且，任何代码重复都是潜在的问题）。为此，C++14 引入了 `decltype(auto)` 语法。对于上面的情况，只需要像下面这样写就行了。

``` cpp
decltype(auto) a = expr;
```
这种代码主要用在通用的转发函数模板中：你可能根本不知道你调用的函数是不是会返回一个引用。这时使用这种语法就会方便很多。

### 函数返回值类型推断

后置返回值类型声明。通常，在返回类型比较复杂、特别是返回类型跟参数类型有某种推导关系时会使用这种语法。

``` cpp
auto foo(参数) ->  返回值类型声明
{
  //  函数体
}
```

### 类模板的模板参数推导

因为函数模板有模板参数推导，使得调用者不必手工指定参数类型；但 C++17 之前的类模板却**没有这个功能，也因而催生了像 make_pair 这样的工具函数**：
``` cpp
pair pr{1, 42};            // 一般不这样写
auto pr = make_pair(1, 42);// 一般这样写
```

在进入了 C++17 的世界后，这类函数变得不必要了。现在可以直接写：

``` cpp
pair pr{1, 42};
```

这种自动推导机制，可以是编译器根据构造函数来自动生成：

``` cpp
template <typename T>
struct MyObj {
  MyObj(T value);
  …
};

MyObj obj1{string("hello")};
//  得到  MyObj<string>
MyObj obj2{"hello"};
//  得到  MyObj<const char*>
```

### 结构化绑定

``` cpp
multimap<string, int>::iterator lower, upper;
std::tie(lower, upper) = mmp.equal_range("four");
```

返回值是个 pair，希望用两个变量来接收数值，就不得不声明了两个变量，然后使用 tie 来接收结果。在 C++11/14 里，这里是没法使用 auto 的。好在 C++17 引入了一个新语法，解决了这个问题。可以把上面的代码简化为：


``` cpp
auto [lower, upper] = mmp.equal_range("four");
```

### 列表初始化

在 C++98 里，标准容器比起 C 风格数组至少有一个明显的劣势：不能在代码里方便地初始化容器的内容。比如，对于数组可以写：

``` cpp
int a[] = {1, 2, 3, 4, 5};
```

而对于 vector 却得写：

``` cpp
vector<int> v;
v.push(1);
v.push(2);
```

于是，C++ 标准委员会引入了**列表初始化**，允许以更简单的方式来初始化对象。现在初始化容器也可以和初始化数组一样简单了：

``` cpp
vector<int> v{1, 2, 3, 4, 5};
```

从技术角度，编译器的魔法只是对 {1, 2, 3} 这样的表达式自动生成一个初始化列表，在这个例子里其类型是 initializer_list。程序员只需要声明一个接受 initializer_list 的构造函数即可使用。

### 统一初始化

几乎可以在所有初始化对象的地方使用大括号而不是小括号。

``` cpp
Obj getObj()
{
  return {1.0};
}
```

{1.0} 跟 Obj(1.0) 的主要区别是，后者可以用来调用 Obj(int)，而使用大括号时编译器会拒绝“窄”转换，不接受以 {1.0} 或 Obj{1.0} 的形式调用构造函数 Obj(int)。

这个语法主要的限制是，如果一个类既有使用初始化列表的构造函数，又有不使用初始化列表的构造函数，那编译器会千方百计地试图调用使用初始化列表的构造函数，导致各种意外。所以，如果给一个推荐的话，那就是：

* 如果一个类没有使用初始化列表的构造函数时，初始化该类对象可全部使用统一初始化语法。
* 如果一个类有使用初始化列表的构造函数时，则只应用在初始化列表构造的情况。


### 类数据成员的默认初始化

按照 C++98 的语法，数据成员可以在构造函数里进行初始化。这本身不是问题，**但实践中，如果数据成员比较多、构造函数又有多个的话，逐个去初始化是个累赘，并且很容易在增加数据成员时漏掉在某个构造函数中进行初始化**。为此，**C++11 增加了一个语法，允许在声明数据成员时直接给予一个初始化表达式**。这样，当且仅当构造函数的初始化列表中不包含该数据成员时，这个数据成员就会自动使用初始化表达式进行初始化。

使用数据成员的默认初始化的话，可以这么写：

``` cpp
class Complex {
public:
  Complex() {}
  Complex(float re) : re_(re) {}
  Complex(float re, float im)
    : re_(re) , im_(im) {}

private:
  float re_{0};
  float im_{0};
};
```

* 第一个构造函数没有任何初始化列表，所以类数据成员的初始化全部由默认初始化完成，re_ 和 im_ 都是 0。
* 第二个构造函数提供了 re_ 的初始化，im_ 仍由默认初始化完成。
* 第三个构造函数则完全不使用默认初始化。


### 自定义字面量

字面量（literal）是指在源代码中写出的固定常量，它们在 C++98 里只能是原生类型，如：

* `"hello"`，字符串字面量，类型是 `const char[6]`
* `1`，整数字面量，类型是 `int`
* `0.0`，浮点数字面量，类型是 `double`
* `3.14f`，浮点数字面量，类型是 `float`
* `123456789ul`，无符号长整数字面量，类型是 `unsigned long`

C++11 引入了自定义字面量，可以使用 `operator""` 后缀，来将用户提供的字面量转换成实际的类型。

``` cpp
#include <chrono>
#include <complex>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

int main()
{
  cout << "i * i = " << 1i * 1i
       << endl;
  cout << "Waiting for 500ms"
       << endl;
  this_thread::sleep_for(500ms);
  cout << "Hello world"s.substr(0, 5)
       << endl;
}
```

上面这个例子展示了 C++ 标准里提供的帮助生成虚数、时间和 basic_string 字面量的后缀。

如何在自己的类里支持字面量？

``` cpp
struct length {
  double value;
  enum unit {
    metre,
    kilometre,
    millimetre,
    centimetre,
    inch,
    foot,
    yard,
    mile,
  };
  static constexpr double factors[] =
    {1.0,    1000.0,  1e-3,
     1e-2,   0.0254,  0.3048,
     0.9144, 1609.344};
  explicit length(double v,
                  unit u = metre)
  {
    value = v * factors[u];
  }
};

length operator+(length lhs,
                 length rhs)
{
  return length(lhs.value +
                rhs.value);
}

//  可能有其他运算符
```

可以手写 `length(1.0, length::metre)` 这样的表达式，但估计大部分开发人员都不愿意这么做，而更希望是 `1.0_m + 10.0_cm`。要允许这个表达式，只需要提供下面的运算符即可：

``` cpp
length operator"" _m(long double v)
{
  return length(v, length::metre);
}

length operator"" _cm(long double v)
{
  return length(v, length::centimetre);
}
```

### 二进制字面量

从 C++14 开始，对于二进制也有了直接的字面量：

``` cpp
unsigned mask = 0b111000000;
```

这在需要比特级操作等场合还是非常有用的。


### 数字分隔符

数字长了之后，看清位数就变得麻烦了。有了二进制字面量，这个问题变得分外明显。C++14 开始，允许在数字型字面量中任意添加 `'` 来使其更可读。具体怎么添加，完全由程序员根据实际情况进行约定。某些常见的情况可能会是：

* 十进制数字使用三位的分隔，对应英文习惯的 thousand、million 等单位。
* 十进制数字使用四位的分隔，对应中文习惯的万、亿等单位。
* 十六进制数字使用两位或四位的分隔，对应字节或双字节。
* 二进制数字使用三位的分隔，对应文件系统的权限分组。
* 等等。

例子：

``` cpp
unsigned mask = 0b111'000'000;
long r_earth_equatorial = 6'378'137;
double pi = 3.14159'26535'89793;
const unsigned magic = 0x44'42'47'4E;
```

### 静态断言

C++98 的 assert 允许在**运行时**检查一个函数的前置条件是否成立。**没有一种方法允许开发人员在编译的时候检查假设是否成立**。

C++11 直接从语言层面提供了静态断言机制，不仅能输出更好的信息，而且适用性也更好，可以直接放在类的定义中。

``` cpp
static_assert(编译期条件表达式,
               可选输出信息);
```

例如：

``` cpp
static_assert((alignment & (alignment - 1)) == 0,
  "Alignment must be power of two");
```

### default 和 delete 成员函数

在类的定义时，C++ 有一些规则决定是否生成**默认的特殊成员函数**。这些特殊成员函数可能包括：

默认构造函数/析构函数/拷贝构造函数/拷贝赋值函数/移动构造函数/移动赋值函数

生成这些特殊成员函数（或不生成）的规则比较复杂。

每个特殊成员函数**有几种不同的状态**：

* 隐式声明还是用户声明
* 默认提供还是用户提供
* 正常状态还是删除状态

这三个状态是可组合的，虽然不是所有的组合都有效。隐式声明的必然是默认提供的；默认提供的才可能被删除；用户提供的也必然是用户声明的。

**经验：**

* 如果正常情况不需要复制行为、只是想防止其他开发人员误操作时，可以简单地在类的定义中加入：

``` cpp

class shape_wrapper {
  …
  shape_wrapper(
    const shape_wrapper&) = delete;
  shape_wrapper& operator=(
    const shape_wrapper&) = delete;
  …
};
```

**在 C++11 之前，我们可能会用在 private 段里声明这些成员函数的方法，来达到相似的目的**。但目前这个语法效果更好，可以产生更明确的错误信息。另外，你可以注意一下，用户声明成删除也是一种声明，因此编译器不会提供默认版本的移动构造和移动赋值函数。

### override 和 final 说明符

override 和 final 是两个 C++11 引入的新说明符。它们不是关键词，仅在出现在函数声明尾部时起作用，不影响我们使用这两个词作变量名等其他用途。这两个说明符可以单个或组合使用，都是加在类成员函数声明的尾部。

override 显式声明了成员函数是一个虚函数且覆盖了基类中的该函数。如果有 override 声明的函数不是虚函数，或基类中不存在这个虚函数，编译器会报告错误。这个说明符的主要作用有两个：

1. 给开发人员更明确的提示，这个函数覆写了基类的成员函数；
2. 让编译器进行额外的检查，防止程序员由于拼写错误或代码改动没有让基类和派生类中的成员函数名称完全一致；

final 则声明了成员函数是一个虚函数，且该虚函数不可在派生类中被覆盖。如果有一点没有得到满足的话，编译器就会报错。

final 还有一个作用是标志某个类或结构不可被派生。同样，这时应将其放在被定义的类或结构名后面。

例子：

``` cpp
class A {
public:
  virtual void foo();
  virtual void bar();
  void foobar();
};

class B : public A {
public:
  void foo() override; // OK
  void bar() override final; // OK
  //void foobar() override;   //  非虚函数不能  override
  
};

class C final : public B {
public:
  void foo() override; // OK
  //void bar() override;  // final  函数不可  override
  
};

class D : public C {
  //  错误：final  类不可派生
  …
};
```

## 到底应不应该返回对象？

《C++ 核心指南》的 [Bjarne Stroustrup and Herb Sutter (editors), “C++ core guidelines”, item F.20](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-out) 或者[非官方版本](https://github.com/lynnboy/CppCoreGuidelines-zh-CN)，这一条款是这么说的：

> F.20: For “out” output values, prefer return values to output parameters

在函数输出数值时，尽量使用返回值而非输出参数。

**之前的做法：调用者负责管理内存，接口负责生成**

一种常见的做法是，接口的调用者负责分配一个对象所需的内存并负责其生命周期，接口负责生成或修改该对象。这种做法意味着对象可以默认构造（甚至只是一个结构），代码一般使用错误码而非异常。例如：

``` cpp
MyObj obj;
ec = initialize(&obj);
// …
```

**另一种做法：接口负责对象的堆上生成和内存管理**

另外一种可能的做法是接口提供生成和销毁对象的函数，对象在堆上维护。fopen 和 fclose 就是这样的接口的实例。


### 如何返回一个对象？

* 一个用来返回的对象，通常应当是**可移动构造 / 赋值的**，一般也同时是**可拷贝构造 / 赋值的**。
* 如果这样一个对象同时又可以**默认构造**，我们就称其为**一个半正则（semiregular）的对象**。如果可能的话，应当尽量让我们的类满足**半正则**这个要求。

``` cpp
class matrix {
public:
  //  普通构造
  matrix(size_t rows, size_t cols);

  //  半正则要求的构造
  matrix();
  matrix(const matrix&);
  matrix(matrix&&);

  //  半正则要求的赋值
  matrix& operator=(const matrix&);
  matrix& operator=(matrix&&);
};
```

在**没有返回值优化**的情况下 C++ 是怎样返回对象的？

``` cpp
matrix operator*(const matrix& lhs,
                 const matrix& rhs)
{
  if (lhs.cols() != rhs.rows()) {
    throw runtime_error("sizes mismatch");
  }

  matrix result(lhs.rows(), rhs.cols());

  //  具体计算过程
  return result;
}
```

* 注意对于一个本地变量，我们永远不应该返回其引用（或指针），不管是作为左值还是右值。从标准的角度，这会导致未定义行为（undefined behavior）
* 从实际的角度，这样的对象一般放在栈上可以被调用者正常覆盖使用的部分，随便一个函数调用或变量定义就可能覆盖这个对象占据的内存。这还是这个对象的析构不做事情的情况：如果析构函数会释放内存或破坏数据的话，那你访问到的对象即使内存没有被覆盖，也早就不是有合法数据的对象了……
* **返回非引用类型的表达式结果是个纯右值（prvalue）**。在执行 auto r = … 的时候，编译器会认为我们实际是在构造 matrix r(…)，而“…”部分是一个纯右值。因此编译器会首先试图匹配 `matrix(matrix&&)`，在没有时则试图匹配 `matrix(const matrix&)`；也就是说，有移动支持时使用移动，没有移动支持时则拷贝。

### 返回值优化（拷贝消除）

``` cpp
#include <iostream>

using namespace std;

// Can copy and move
class A {
public:
  A() { cout << "Create A\n"; }
  ~A() { cout << "Destroy A\n"; }
  A(const A&) { cout << "Copy A\n"; }
  A(A&&) { cout << "Move A\n"; }
};

A getA_unnamed()
{
  return A();
}

int main()
{
  auto a = getA_unnamed();
}
```

如果你认为执行结果里应当有一行“Copy A”或“Move A”的话，你就忽视了**返回值优化**的威力了。

**即使完全关闭优化**，三种主流编译器（GCC、Clang 和 MSVC）都只输出两行：

```
Create A
Destroy A
```

把代码稍稍改一下：

``` cpp
A getA_named()
{
  A a;
  return a;
}

int main()
{
  auto a = getA_named();
}
```

这回结果有了一点点小变化。虽然 GCC 和 Clang 的结果完全不变，但 MSVC 在非优化编译的情况下产生了不同的输出（优化编译——使用命令行参数 /O1、/O2 或 /Ox——则不变）：

```
Create A
Move A
Destroy A
Destroy A
```

也就是说，返回内容被移动构造了。

继续变形一下：

``` cpp
#include <stdlib.h>

A getA_duang()
{
  A a1;
  A a2;
  if (rand() > 42) {
    return a1;

  } else {
    return a2;
  }
}

int main()
{
  auto a = getA_duang();
}
```

这回所有的编译器都被难倒了，输出是：

```
Create A
Create A
Move A
Destroy A
Destroy A
Destroy A
```

关于返回值优化的实验我们就做到这里。下一步，我们试验一下把移动构造函数删除：

``` cpp
A(A&&) = delete;
```

可以立即看到“Copy A”出现在了结果输出中，说明目前结果变成**拷贝构造**了。

如果再进一步，把**拷贝构造函数**也删除呢？是不是上面的 getA_unnamed、getA_named 和 getA_duang 都不能工作了？


在 C++14 及之前确实是这样的。但从 C++17 开始，对于类似于 getA_unnamed 这样的情况，即使对象不可拷贝、不可移动，这个对象仍然是可以被返回的！**C++17 要求对于这种情况，对象必须被直接构造在目标位置上，不经过任何拷贝或移动的步骤**。[refer: cppreference.com, “Copy elision”](https://en.cppreference.com/w/cpp/language/copy_elision)


理解了 C++ 里的对返回值的处理和返回值优化之后，我们再回过头看一下 `F.20` 里陈述的理由的话，应该就显得很自然了：

> A return value is self-documenting, whereas a & could be either in-out or out-only and is liable to be misused.
> 
> 返回值是可以自我描述的；而 & 参数既可能是输入输出，也可能是仅输出，且很容易被误用。（当然也有一些例外的情况）

**总结：**

在 C++11 之前，返回一个本地对象意味着这个对象会被拷贝，除非编译器发现可以做**返回值优化（named return value optimization，或 NRVO）**，能把对象直接构造到调用者的栈上。从 C++11 开始，返回值优化仍可以发生，但在没有返回值优化的情况下，编译器将试图把本地对象移动出去，而不是拷贝出去。这一行为不需要程序员手工用 `std::move` 进行干预——使用`std::move` 对于移动行为没有帮助，反而会影响返回值优化。

## Unicode：进入多文字支持的世界

从编码的历史谈起，讨论编程中对**中文**和**多语言**的支持，然后重点看一下 C++ 中应该如何处理这些问题。

### 一些历史

**[ASCII](https://en.wikipedia.org/wiki/ASCII) （American Standard Code for Information Interchange）** 是一种创立于 1963 年的 7 位编码，用 0 到 127 之间的数值来代表最常用的字符，包含了控制字符（很多在今天已不再使用）、数字、大小写拉丁字母、空格和基本标点。它在编码上具有简单性，字母和数字的编码位置非常容易记忆。时至今日，ASCII 可以看作是字符编码的基础，主要的编码方式都保持着与 ASCII 的兼容性。

![ASCII](/assets/images/201911/ASCII.gif)

ASCII 里只有**基本的拉丁字母**，它既没有带变音符的拉丁字母（如 é 和 ä ），也不支持像希腊字母（如 α、β、γ）、西里尔字母（如 Пушкин）这样的其他欧洲文字（也难怪，毕竟它是 American Standard Code for Information Interchange）。很多其他编码方式纷纷应运而生，包括 ISO 646 系列、ISO/IEC 8859 系列等等；**大部分编码方式都是头 128 个字符与 ASCII 兼容，后 128 个字符是自己的扩展，总共最多是 256 个字符**。**每次只有一套方式可以生效，称之为一个代码页（code page）**。这种做法，只能适用于文字相近、且字符数不多的国家。比如，`ISO-8859-1`（也称作 `Latin-1`）和后面的 Windows 扩展代码页 1252，就只能适用于西欧国家。

最早的中文字符集标准是 1980 年的国标 `GB2312`，**其中收录了 6763 个常用汉字和 682 个其他符号**。我们平时会用到编码 `GB2312`，其实更正确的名字是 `EUC-CN`，它是一种与 ASCII 兼容的编码方式。它用单字节表示 ASCII 字符而用双字节表示 GB2312 中的字符；由于 GB2312 中本身也含有 ASCII 中包含的字符，在使用中逐渐就形成了“半角”和“全角”的区别。

国标字符集后面又有扩展，这个扩展后的字符集就是 `GBK`，是中文版 Windows 使用的标准编码方式。`GB2312` 和 `GBK` 所占用的编码位置可以参看下面的图（由 John M. Długosz 为 Wikipedia 绘制）：

![GBK](/assets/images/201911/GBK.png)

图中 `GBK/1` 和 `GBK/2` 为 `GB2312` 中已经定义的区域，其他的则是后面添加的字符，总共定义了两万多个编码点，支持了绝大部分现代汉语中还在使用的字。

`Unicode`作为一种统一编码的努力，诞生于八十年代末九十年代初，标准的第一版出版于 1991—1992 年。**由于最初发明者的目标放得太低，只期望对活跃使用中的现代文字进行编码，他们认为 16 比特的“宽 ASCII”就够用了**。这就导致了早期采纳 Unicode 的组织，特别是微软，在其操作系统和工具链中广泛采用了 16 比特的编码方式。在今天，微软的系统中宽字符类型 `wchar_t` 仍然是 16 位的，操作系统底层接口大量使用 16 位字符编码的 API，说到 Unicode 编码时仍然指的是 16 位的编码 `UTF-16`（这一不太正确的名字，跟中文 GBK 编码居然可以被叫做 ANSI 相比，实在是小巫见大巫了）。在微软以外的世界，Unicode 本身不作编码名称用，并且最主流的编码方式并不是 `UTF-16`，而是和 ASCII 全兼容的 `UTF-8`。

### Unicode 简介

Unicode 在今天已经大大超出了最初的目标。到 Unicode 12.1 为止，Unicode 已经包含了 137,994 个字符，囊括所有主要语言（使用中的和已经不再使用的），并包含了表情符号、数学符号等各种特殊字符。

Unicode 的编码点是从 `0x0` 到 `0x10FFFF`，一共 1,114,112 个位置。**一般用“U+”后面跟 16 进制的数值来表示一个 Unicode 字符**，如 U+0020 表示空格，U+6C49 表示“汉”，U+1F600 表示“😀”，等等（不足四位的一般写四位）。

Unicode 字符的常见编码方式有：

* UTF-32
    - 32 比特，是编码点的直接映射
* UTF-16
    - 对于从 U+0000 到 U+FFFF 的字符，使用 16 比特的直接映射
* UTF-8
    - 1 到 4 字节的变长编码

例子：

```
UTF-32：U+0020 映射为 0x00000020，U+6C49 映射为 0x00006C49，U+1F600 映射为 0x0001F600。

UTF-16：U+0020 映射为 0x0020，U+6C49 映射为 0x6C49，而 U+1F600 会映射为 0xD83D DE00。

UTF-8：U+0020 映射为 0x20，U+6C49 映射为 0xE6 B1 89，而 U+1F600 会映射为 0xF0 9F 98 80。
```

在上面三种编码方式里，只有 UTF-8 完全保持了和 ASCII 的兼容性，目前得到了最广泛的使用。

任何一种编码方式需要跟传统的编码方式容易区分，因此，Unicode 文本文件通常有一个使用 `BOM（byte order mark）`字符的约定，即字符 `U+FEFF`。在文件开头加一个 BOM 即可区分各种不同编码。

例如：

如果文件开头是 `0xEF BB BF`，那这是 `UTF-8` 编码。

编辑器可以（有些在配置之后）根据 BOM 字符来自动决定文本文件的编码。

比如，一般在 Vim 中配置 `set fileencodings=ucs-bom,utf-8,gbk,latin1`。这样，Vim 在读入文件时，会首先检查 BOM 字符，有 BOM 字符按 BOM 字符决定文件编码；否则，试图将文件按 UTF-8 来解码（由于 UTF-8 有格式要求，非 UTF-8 编码的文件通常会导致失败）；不行，则试图按 GBK 来解码（失败的概率就很低了）；还不行，就把文件当作 Latin1 来处理（永远不会失败）。

在 UTF-8 编码下使用 BOM 字符并非必需，尤其在 Unix 上。但 Windows 上通常会使用 BOM 字符，以方便区分 UTF-8 和传统编码。


## 编译期多态：泛型编程和模板入门

### 面向对象和多态

**在面向对象的开发里，最基本的一个特性就是“多态” —— 用相同的代码得到不同结果**。以 shape 类为例，它可能会定义一些通用的功能，然后在子类里进行实现或覆盖：

``` cpp
class shape {
public:
  …
  void draw(const position&) = 0;
};
```

上面的类定义意味着所有的子类必须实现 draw 函数，所以可以认为 shape 是定义了一个接口（按 Java 的概念）。在面向对象的设计里，接口抽象了一些基本的行为，实现类里则去具体实现这些功能。当我们有着接口类的指针或引用时，我们实际可以唤起具体的实现类里的逻辑。

比如，在一个绘图程序里，我们可以在用户选择一种形状时，把形状赋给一个 shape 的（智能）指针，在用户点击绘图区域时，执行 draw 操作。根据指针指向的形状不同，实际绘制出的可能是圆，可能是三角形，也可能是其他形状。

但这种面向对象的方式，并不是唯一一种实现多态的方式。在很多动态类型语言里，有所谓的**“鸭子”类型**：

> 如果一只鸟走起来像鸭子、游起泳来像鸭子、叫起来也像鸭子，那么这只鸟就可以被当作鸭子。

在这样的语言里，你可以不需要继承来实现 circle、triangle 等类，然后可以直接在这个类型的变量上调用 draw 方法。如果这个类型的对象没有 draw 方法，你就会在执行到 draw() 语句的时候得到一个错误（或异常）。

**鸭子类型使得开发者可以不使用继承体系来灵活地实现一些“约定”**，尤其是使得混合不同来源、使用不同对象继承体系的代码成为可能。唯一的要求只是，这些不同的对象有“共通”的成员函数。这些成员函数应当有相同的名字和相同结构的参数（并不要求参数类型相同）。

### 容器类的共性

容器类是有很多共性的。其中，一个最最普遍的共性就是，容器类都有 `begin` 和 `end` 成员函数——这使得通用地遍历一个容器成为可能。容器类不必继承一个共同的 Container 基类，而我们仍然可以写出通用的遍历容器的代码，如使用基于范围的循环。

大部分容器是有 size 成员函数的，在“泛型”编程中，我们同样可以取得一个容器的大小，而不要求容器继承一个叫 SizeableContainer 的基类。

很多容器具有 push_back 成员函数，可以在尾部插入数据。同样，我们不需要一个叫 BackPushableContainer 的基类。在这个例子里，push_back 函数的参数显然是都不一样的，但明显，所有的 push_back 函数都只接收一个参数。

我们可以清晰看到的是，**虽然 C++ 的标准容器没有对象继承关系，但彼此之间有着很多的同构性。这些同构性很难用继承体系来表达，也完全不必要用继承来表达。C++ 的模板，已经足够表达这些鸭子类型**。

当然，**作为一种静态类型语言，C++ 是不会在运行时才报告“没找到 draw 方法”这类问题的。这类错误可以在编译时直接捕获，更精确地来说，是在模板实例化的过程中**。

### C++ 模板

#### 定义模板函数

``` cpp
// 求最大公约数的辗转相除法
template <typename E>
E my_gcd(E a, E b)
{
  while (b != E(0)) {
    E r = a % b;
    a = b;
    b = r;
  }
  return a;
}
```

除了函数模版，还有类模板，比如智能指针类。

#### 实例化模板

不管是**类模板**还是**函数模板**，编译器在看到其定义时只能做最基本的语法检查，**真正的类型检查要在实例化（instantiation）的时候才能做**。一般而言，这也是编译器会报错的时候。

实例化失败的话，编译当然就出错退出了。如果成功的话，模板的实例就产生了。在整个的编译过程中，可能产生多个这样的（相同）实例，**但最后链接时，会只剩下一个实例**。这也是为什么 **C++ 会有一个单一定义的规则：如果不同的编译单元看到不同的定义的话，那链接时使用哪个定义是不确定的，结果就可能会让人吃惊。**

模板还可以**显式实例化（使用 template 关键字并给出完整的类型来声明函数）**和**外部实例化**。

注意：**显式实例化**和**外部实例化**通常在大型项目中可以用来集中模板的实例化，从而加速编译过程——不需要在每个用到模板的地方都进行实例化了——但这种方式有额外的管理开销，如果实例化了不必要实例化的模板的话，反而会导致可执行文件变大。因而，显式实例化和外部实例化应当谨慎使用。

#### 特化模板

我们需要使用的模板参数类型，不能完全满足模板的要求，应该怎么办？

实际上有好几个选择：

1. 添加代码，让那个类型支持所需要的操作（对成员函数无效，在很多情况下，尤其是对对象的成员函数有要求的情况下，这个方法不可行）。
2. 对于**函数模板**，可以直接针对那个类型进行**重载**。
3. 对于**类模板和函数模板**（，可以针对那个类型进行**特化**。

特化的例子：

``` cpp
template <typename E>
E my_mod(const E& lhs,
         const E& rhs)
{
  return lhs % rhs;
}

// 针对 cln::cl_I 类型的特化
template <>
cln::cl_I my_mod<cln::cl_I>(
  const cln::cl_I& lhs,
  const cln::cl_I& rhs)
{
  return mod(lhs, rhs);
}
```

**注意：**
** 1. 特化和重载在行为上没有本质的区别。就一般而言，特化是一种更通用的技巧，最主要的原因是特化可以用在类模板和函数模板上，而重载只能用于函数。**
** 2. Herb Sutter 给出了明确的建议：对函数使用重载，对类模板进行特化。[refer: Herb Sutter, “Why not specialize function templates?”](http://www.gotw.ca/publications/mill17.htm)** （**重载比特化优先。一般而言，函数特化是不推荐的**）


展示**特化的更好的例子**是 C++11 之前的**静态断言**。使用特化技巧可以大致实现 `static_assert` 的功能：

``` cpp
template <bool>
struct compile_time_error;
template <>
struct compile_time_error<true> {};

#define STATIC_ASSERT(Expr, Msg)   \
  {                                \
    compile_time_error<bool(Expr)> \
      ERROR_##_Msg;                \
    (void)ERROR_##_Msg;            \
  }
```

**原理：**

上面首先声明了一个 struct 模板，然后仅对 true 的情况进行了特化，产生了一个 struct 的定义。这样。如果遇到 `compile_time_error<false>` 的情况——也就是下面静态断言里的 Expr 不为真的情况——编译就会失败报错，**因为 `compile_time_error<false>` 从来就没有被定义过**。

### “动态”多态和“静态”多态的对比

**面向对象的“动态”多态** 和 **基于泛型编程的“静态”多态**，两者解决的实际上是不太一样的问题。

* **“动态”多态解决的是运行时的行为变化**——例如，选择了一个形状之后，再选择在某个地方绘制这个形状——这个是无法在编译时确定的。
* **“静态”多态或者“泛型”**——解决的是很不同的问题，让适用于不同类型的“同构”算法可以用同一套代码来实现，**实际上强调的是对代码的复用**。

问题：为什么并非所有的语言都支持这些不同的多态方式？

以 Python 为例，它是动态类型的语言。所以它不会有真正的静态多态。但和静态类型的面向对象语言（如 Java）不同，它的运行期多态不需要继承。没有参数化多态初看是个缺陷，但由于 Python 的动态参数系统允许默认参数和可变参数，并没有什么参数化多态能做得到而 Python 做不到的事。



## RTTI（运行期）

`decltype` 得到的是**编译期的类型**。**运行期的类型**，C++ 里挺有争议（跟异常类似）的功能——`RTTI`。（还是要强调一句，你应该考虑是否用**虚函数**可以达到你需要的功能。很多项目，如 Google 的，会禁用 RTTI。）

可以用 `typeid` 直接来获取对象的实际类型，例如：


``` cpp
#include <iostream>
#include <typeinfo>
#include <boost/core/demangle.hpp>

using namespace std;
using boost::core::demangle;

class shape {
public:
  virtual ~shape() {}
};

class circle : public shape {
};

int main()
{
  shape* ptr = new circle();
  auto& type = typeid(*ptr);
  cout << type.name() << endl;
  cout << demangle(type.name()) << endl;
  cout << boolalpha;
  cout << (type == typeid(shape) ? "is shape\n" : "");
  cout << (type == typeid(circle) ? "is circle\n" : "");
  delete ptr;
}
```

在 GCC 下的输出：

```
6circle
circle
is circle
```

## 编译期计算（模板元编程）

编译期能做些什么？一个完整的计算世界

首先，我们给出一个已经被证明的结论：**C++ 模板是图灵完全的** [refer: Todd L. Veldhuizen, “C++ templates are Turing complete”](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.14.3670)。这句话的意思是，**使用 C++ 模板，你可以在编译期间模拟一个完整的图灵机，也就是说，可以完成任何的计算任务**。

当然，这只是理论上的结论。从实际的角度，我们并不想、也不可能在编译期完成所有的计算，更不用说编译期的编程是很容易让人看不懂的——因为这并不是语言设计的初衷。

但是，即便如此，我们也还是需要了解一下模板元编程的基本概念：它仍然有一些实用的场景，并且在实际的工程中你也可能会遇到这样的代码。虽然我们在开篇就说过不要炫技，但使用模板元编程写出的代码仍然是可理解的，尤其是如果你对递归不发怵的话。

### 计算阶乘

``` cpp
template <int n>
struct factorial {
  static const int value =
    n * factorial<n - 1>::value;
};

template <>
struct factorial<0> {
  static const int value = 1;
};
```

注意，**要先定义，才能特化**。那怎么知道这个计算是不是在编译时做的呢？可以直接看编译输出。下面直接贴出对上面这样的代码加输出（`printf("%d\n", factorial<10>::value);`）在 x86-64 下的编译结果：

```
.LC0:
        .string "%d\n"
main:
        push    rbp
        mov     rbp, rsp
        mov     esi, 3628800
        mov     edi, OFFSET FLAT:.LC0
        mov     eax, 0
        call    printf
        mov     eax, 0
        pop     rbp
        ret
```

编译结果直接出现了常量 3628800。上面那些递归什么的，完全都没有了踪影。

如果我们传递一个**负数**给 factorial 呢？这时的结果就应该是编译期间的递归溢出。如 GCC 会报告：

```
fatal error: template instantiation depth exceeds maximum of 900 (use -ftemplate-depth= to increase the maximum)
```

通用的解决方案是使用 `static_assert`，确保参数永远不会是负数。

``` cpp
template <int n>
struct factorial {
  static_assert(
    n >= 0,
    "Arg must be non-negative");

  static const int value =
    n * factorial<n - 1>::value;
};
```

这样，当 factorial 接收到一个负数作为参数时，就会得到一个干脆的错误信息：

```
error: static assertion failed: Arg must be non-negative
```

> 结论：可以看到，要进行编译期编程，最主要的一点，是需要把**计算**转变成**类型推导**。

### 条件语句

再看一个例子，下面的模板可以代表**条件语句**：

``` cpp
template <bool cond,
          typename Then,
          typename Else>
struct If;

template <typename Then,
          typename Else>
struct If<true, Then, Else> {
  typedef Then type;
};

template <typename Then,
          typename Else>
struct If<false, Then, Else> {
  typedef Else type;
};
```

* If 模板有三个参数，第一个是布尔值，后面两个则是代表不同分支计算的类型
* 第一个 struct 声明规定了模板的形式，然后我们不提供通用定义，而是提供了两个特化
* 第一个特化是真的情况，定义结果 type 为 Then 分支；第二个特化是假的情况，定义结果 type 为 Else 分支

下面的函数和模板是基本等价的：

``` cpp
int foo(int n)
{
  if (n == 2 || n == 3 || n == 5) {
    return 1;
  } else {
    return 2;
  }
}

template <int n>
struct Foo {
  typedef typename If<
    (n == 2 || n == 3 || n == 5),
    integral_constant<int, 1>,
    integral_constant<int, 2>>::type
    type;
};

// foo(3) 等价于 Foo<3>::type::value
```

### 循环

另一个例子，**循环**：

``` cpp
template <bool condition,
          typename Body>
struct WhileLoop;

template <typename Body>
struct WhileLoop<true, Body> {
  typedef typename WhileLoop<
    Body::cond_value,
    typename Body::next_type>::type
    type;
};

template <typename Body>
struct WhileLoop<false, Body> {
  typedef
    typename Body::res_type type;
};

template <typename Body>
struct While {
  typedef typename WhileLoop<
    Body::cond_value, Body>::type
    type;
};
```

* 首先，我们对循环体类型有一个约定，它必须提供一个静态数据成员，cond_value，及两个子类型定义，res_type 和 next_type。
    + cond_value 代表循环的条件（真或假）
    + res_type 代表退出循环时的状态
    + next_type 代表下面循环执行一次时的状态
* 这里面比较绕的地方是用类型来代表执行状态，把例子多看两遍，自己编译、修改、把玩一下，就会渐渐理解的。
* 用 `::` 取一个成员类型、并且 `::` 左边有模板参数的话，得额外加上 `typename` 关键字来标明结果是一个类型

###  通用的代表数值的类型

下面这个模板可以通用地代表一个整数常数：

``` cpp
template <class T, T v>
struct integral_constant {
  static const T value = v;
  typedef T value_type;
  typedef integral_constant type;
};
```

* integral_constant 模板同时包含了整数的**类型**和**数值**，而通过这个类型的 value 成员我们又可以重新取回这个数值。
* 有了这个模板的帮忙，我们就可以进行一些更通用的计算了。

``` cpp
template <int result, int n>
struct SumLoop {
  static const bool cond_value =
    n != 0;
  static const int res_value =
    result;
  typedef integral_constant<
    int, res_value>
    res_type;
  typedef SumLoop<result + n, n - 1>
    next_type;
};

template <int n>
struct Sum {
  typedef SumLoop<0, n> type;
};
```

然后使用 `While<Sum<10>::type>::type::value` 就可以得到 1 加到 10 的结果。

### 编译期类型推导

C++ 标准库在 `<type_traits>` 头文件里定义了很多**工具类模板**，**用来提取某个类型（type）在某方面的特点（trait）** [refer: cppreference.com, “Standard library header ”](https://en.cppreference.com/w/cpp/header/type_traits)。

``` cpp
typedef std::integral_constant<
  bool, true> true_type;
typedef std::integral_constant<
  bool, false> false_type;


template <typename T>
class SomeContainer {
public:
  …
  static void destroy(T* ptr)
  {
    _destroy(ptr,
      is_trivially_destructible<
        T>());
  }

private:
  static void _destroy(T* ptr,
                       true_type)
  {}
  static void _destroy(T* ptr,
                       false_type)
  {
    ptr->~T();
  }
};
```

* 类似上面，很多容器类里会有一个 destroy 函数，通过指针来析构某个对象。为了确保最大程度的优化，常用的一个技巧就是用 **is_trivially_destructible 模板**来判断类是否是**可平凡析构的——也就是说，不调用析构函数，不会造成任何资源泄漏问题**。
* 模板返回的结果还是一个类，要么是 true_type，要么是 false_type。如果要得到布尔值的话，当然使用 `is_trivially_destructible<T>::value` 就可以，但此处不需要。我们需要的是，使用 () 调用该类型的构造函数，让编译器根据数值类型来选择合适的重载。这样，在优化编译的情况下，编译器可以把不需要的析构操作彻底全部删除。

另外一些模板，可以用来做一些类型的转换。以一个常见的模板 remove_const 为例（用来去除类型里的 const 修饰）:

``` cpp
template <class T>
struct remove_const {
  typedef T type;
};
template <class T>
struct remove_const<const T> {
  typedef T type;
};
```

* 利用模板的特化，针对 const 类型去掉相应的修饰。
* 例如，`remove_const<const string&>::type` 等价于 `string&`。

### 通用的 fmap 函数模板

从概念本源来看，map 和 reduce 都来自**函数式编程**。

``` cpp
template <
  template <typename, typename>
  class OutContainer = vector,
  typename F, class R>
auto fmap(F&& f, R&& inputs)
{
  typedef decay_t<decltype(
    f(*inputs.begin()))>
    result_type;

  OutContainer<
    result_type,
    allocator<result_type>>
    result;

  for (auto&& item : inputs) {
    result.push_back(f(item));
  }
  return result;
}
```

* 用 decltype 来获得用 f 来调用 inputs 元素的类型
* 用 decay_t 来把获得的类型变成一个普通的值类型
* 缺省使用 vector 作为返回值的容器，但可以通过模板参数改为其他容器
* 使用基于范围的 for 循环来遍历 inputs，对其类型不作其他要求
* 存放结果的容器需要支持 push_back 成员函数

下面的代码可以验证其功能：

``` cpp
vector<int> v{1, 2, 3, 4, 5};
int add_1(int x)
{
    return x + 1;
}

// 在 fmap 执行之后，在 result 里得到一个新容器，其内容是 2, 3, 4, 5, 6
auto result = fmap(add_1, v);
```

完整代码：使用 C++17、GCC 7编译

``` cpp
#include <iostream>
#include <vector>
#include <type_traits>

using namespace std;

template< class T >
using decay_t = typename decay<T>::type;

template < template <typename, typename> class OutContainer = vector,
           typename F, class R>
auto fmap(F&& f, R&& inputs)
{
    typedef decay_t<decltype( f(*inputs.begin()))> result_type;
    OutContainer< result_type, allocator<result_type>> result;
    for (auto&& item : inputs) {
        result.push_back(f(item));
    }
    return result;
}

int add_1(int x)
{
    return x + 1;
}

int main()
{
    vector<int> v{1, 2, 3, 4, 5};

    auto result = fmap(add_1, v);

    for (auto &&v : result) {
        cout << v << endl;
    }
}
```

总结：**模板元编程，其本质是把`计算过程用编译期`的`类型推导`和`类型匹配`表达出来**。


## SFINAE：不是错误的替换失败是怎么回事?

**替换失败非错（Substituion Failure is Not An Error）**，英文简称为 `SFINAE`。

### 函数模板的重载决议

当一个**函数名称**和**某个函数模板名称**匹配时，重载决议过程大致如下：

* 根据名称找出所有适用的函数和函数模板
* 对于适用的函数模板，要根据实际情况对模板形参进行替换；替换过程中如果发生错误，这个模板会被丢弃
* 在上面两步生成的可行函数集合中，编译器会寻找一个最佳匹配，产生对该函数的调用
* 如果没有找到最佳匹配，或者找到多个匹配程度相当的函数，则编译器需要报错

例子：
``` cpp
#include <stdio.h>

struct Test {
  typedef int foo;
};

template <typename T>
void f(typename T::foo)
{
  puts("1");
}

template <typename T>
void f(T)
{
  puts("2");
}

int main()
{
  f<Test>(10);
  f<int>(10);
}
```

输出为：

```
1
2
```

首先看 `f<Test>(10)` 的情况：
* 有两个模板符合名字 f
* 替换结果为 f(Test::foo) 和 f(Test)
* 使用参数 10 去匹配，只有前者参数可以匹配，因而第一个模板被选择

再看一下 `f<int>(10)` 的情况：
* 还是两个模板符合名字 f
* 替换结果为 f(int::foo) 和 f(int)；显然前者不是个合法的类型，被抛弃
* 使用参数 10 去匹配 f(int)，没有问题，那就使用这个模板实例了

**总结：体现的是 SFINAE 设计的最初用法：如果模板实例化中发生了失败，没有理由编译就此出错终止，因为还是可能有其他可用的函数重载的。** 这儿的失败仅指函数模板的原型声明，即参数和返回值。函数体内的失败不考虑在内。如果重载决议选择了某个函数模板，而函数体在实例化的过程中出错，那仍然会得到一个编译错误。

### 编译期成员检测

根据某个实例化的成功或失败来在编译期检测类的特性。

下面这个模板，就可以检测一个类是否有一个名叫 reserve、参数类型为 size_t 的成员函数：

``` cpp
template <typename T>
struct has_reserve {

  struct good { char dummy; };

  struct bad { char dummy[2]; };

  template <class U,
            void (U::*)(size_t)>
  struct SFINAE {};

  template <class U>
  static good
  reserve(SFINAE<U, &U::reserve>*);

  template <class U>
  static bad reserve(...);

  static const bool value =
    sizeof(reserve<T>(nullptr))
    == sizeof(good);
};
```

* 首先定义了两个结构 good 和 bad；它们的内容不重要，我们只关心它们的大小必须不一样。
* 然后定义了一个 SFINAE 模板，内容也同样不重要，但模板的第二个参数需要是第一个参数的成员函数指针，并且参数类型是 size_t，返回值是 void。
* 随后，定义了一个要求 SFINAE* 类型的 reserve 成员函数模板，返回值是 good；再定义了一个对参数类型无要求的 reserve 成员函数模板，返回值是 bad。
* 最后，我们定义常整型布尔值 value，结果是 true 还是 false，取决于 nullptr 能不能和 SFINAE* 匹配成功，而这又取决于模板参数 T 有没有返回类型是 void、接受一个参数并且类型为 size_t 的成员函数 reserve。

### SFINAE 模板技巧

C++11 开始，标准库里有了一个叫 `enable_if` 的模板（定义在 `<type_traits>` 里）([refer: cppreference.com, “std::enable_if”](https://en.cppreference.com/w/cpp/types/enable_if))，可以用它来选择性地启用**某个函数的重载**。

假设有一个函数，用来往一个容器尾部追加元素。我们希望原型是这个样子的：

``` cpp
template <typename C, typename T>
void append(C& container, T* ptr,
            size_t size);
```

显然，container 有没有 `reserve` 成员函数，是对性能有影响的——如果有的话，我们通常应该预留好内存空间，以免产生不必要的对象移动甚至拷贝操作。利用 `enable_if` 和上面的 `has_reserve` 模板，我们就可以这么写：

``` cpp
// 有 reserve 的 append 版本
template <typename C, typename T>
enable_if_t<has_reserve<C>::value,
            void>
append(C& container, T* ptr,
       size_t size)
{
  container.reserve(
    container.size() + size);
  for (size_t i = 0; i < size;
       ++i) {
    container.push_back(ptr[i]);
  }
}

// 没有 reserve 的 append 版本
template <typename C, typename T>
enable_if_t<!has_reserve<C>::value,
            void>
append(C& container, T* ptr,
       size_t size)
{
  for (size_t i = 0; i < size;
       ++i) {
    container.push_back(ptr[i]);
  }
}
```

我们可以用 `enable_if_t` 来取到结果的类型。

`enable_if_t<has_reserve<C>::value, void>` 的意思可以理解成：如果类型 C 有 reserve 成员的话，那我们启用下面的成员函数，它的返回类型为 void。


### decltype 返回值

如果只需要在某个操作有效的情况下启用某个函数，而不需要考虑相反的情况的话。

``` cpp
template <typename C, typename T>
auto append(C& container, T* ptr,
            size_t size)
  -> decltype(
    declval<C&>().reserve(1U),
    void())
{
  container.reserve(container.size() + size);

  for (size_t i = 0; i < size;
       ++i) {
    container.push_back(ptr[i]);
  }
}
```

上面使用到了 `declval` （[refer: cppreference.com, “std::declval”](https://en.cppreference.com/w/cpp/utility/declval)）

* 这个模板用来声明一个某个类型的参数，但这个参数只是用来参加模板的匹配，不允许实际使用。使用这个模板，我们可以在某类型没有默认构造函数的情况下，假想出一个该类的对象来进行类型推导。
* `declval<C&>().reserve(1U)` 用来测试 C& 类型的对象是不是可以拿 1U 作为参数来调用 reserve 成员函数。此外，我们需要记得，C++ 里的逗号表达式的意思是按顺序逐个估值，并返回最后一项。所以，上面这个函数的返回值类型是 void。
* 这个方式和 `enable_if` 不同，很难表示否定的条件。如果要提供一个专门给没有 reserve 成员函数的 C 类型的 append 重载，这种方式就不太方便了。因而，**这种方式的主要用途是避免错误的重载**。

### void_t

void_t 是 C++17 新引入的一个模板。它的定义简单得令人吃惊：

``` cpp
template <typename...>
using void_t = void;
```

这个类型模板会把任意类型映射到 void。它的特殊性在于，在这个看似无聊的过程中，编译器会检查那个“任意类型”的有效性。

## constexpr：一个常态的世界

存在两类 constexpr 对象：

* 一个 constexpr 变量是一个编译时完全确定的常数。
* 一个 constexpr 函数至少对于某一组实参可以在编译期间产生一个编译期常数。

``` cpp
#include <array>

constexpr int sqr(int n)
{
  return n * n;
}

int main()
{
  constexpr int n = sqr(3);
  std::array<int, n> a;
  int b[n];
}
```

以阶乘函数为例：

``` cpp
#include <cstdio>
#include <stdexcept>

constexpr int factorial(int n)
{
        if (n < 0) {
                throw std::invalid_argument(
                                "Arg must be non-negative");
        }

        if (n == 0) {
                return 1;
        } else {
                return n * factorial(n - 1);
        }
}

int main()
{
        //constexpr int n = factorial(10);   // ok
        constexpr int n = factorial(-1);     // compile error
        printf("%d\n", n);
}
/*
$ g++ -o constexpr constexpr.cpp 
constexpr.cpp: In function ‘int main()’:
constexpr.cpp:21:29:   in constexpr expansion of ‘factorial(-1)’
constexpr.cpp:8:31: error: expression ‘<throw-expression>’ is not a constant expression
     "Arg must be non-negative");
                               ^
*/
```

验证确实得到了一个编译期常量。如果看产生的汇编代码的话，一样可以直接看到常量 3628800。

注意：这里有一个问题：在这个 constexpr 函数里，是不能写 `static_assert(n >= 0)` 的。一个 constexpr 函数仍然可以作为普通函数使用——显然，传入一个普通 int 是不能使用静态断言的。

如果没有检查判断，编译展开时会报错：

```
$ g++ -o constexpr constexpr.cpp 
constexpr.cpp: In function ‘int main()’:
constexpr.cpp:23:29:   in constexpr expansion of ‘factorial(-1)’
constexpr.cpp:16:23:   in constexpr expansion of ‘factorial((n + -1))’
constexpr.cpp:16:23:   in constexpr expansion of ‘factorial((n + -1))’
...

constexpr.cpp:16:23:   in constexpr expansion of ‘factorial((n + -1))’
constexpr.cpp:23:32: error: constexpr evaluation depth exceeds maximum of 512 (use -fconstexpr-depth= to increase the maximum)
  constexpr int n = factorial(-1);
                                ^
```

### constexpr 和 const

初学 `constexpr` 时，一个很可能有的困惑是，它跟 `const` 用法上的区别到底是什么？产生这种困惑是正常的，毕竟 `const` 是个重载了很多不同含义的关键字。`const` 的原本和基础的含义，自然是表示**它修饰的内容不会变化**，如：

``` cpp
const int n = 1:
n = 2;  //  出错！
```

注意: **`const` 在类型声明的不同位置会产生不同的结果**。对于常见的 `const char*` 这样的类型声明，意义和 `char const*` 相同，**是指向常字符的指针，指针指向的内容不可更改**；但和 `char * const` 不同，那代表**指向字符的常指针，指针本身不可更改**。本质上，`const` 用来表示一个**运行时常量**。

在 C++ 里，`const` 后面渐渐带上了现在的 `constexpr` 用法，也代表**编译期常数**。现在在有了 `constexpr` 之后——我们应该使用 `constexpr` 在这些用法中替换 `const` 了。从编译器的角度，为了向后兼容性，`const` 和 `constexpr` 在很多情况下还是等价的。但有时候，它们也有些细微的区别，其中之一为是否内联的问题。

**constexpr 变量仍是 const。**

一个 constexpr 变量仍然是 const 常类型。需要注意的是，就像 const char* 类型是指向常量的指针、自身不是 const 常量一样，**下面这个表达式里的 const 也是不能缺少的**：

``` cpp
constexpr int a = 42;
constexpr const int& b = a;
```

constexpr 表示 b 是一个编译期常量，const 表示这个引用是常量引用。去掉这个 const 的话，编译器就会认为你是试图将一个普通引用绑定到一个常数上，报一个类似下面的错误信息：`error: binding reference of type ‘int&’ to ‘const int’ discards qualifiers`


## 函数对象和lambda：进入函数式编程

### C++98 的函数对象

**[函数对象（function object）](https://en.wikipedia.org/wiki/Function_object)** 自 C++98 开始就已经被标准化了。从概念上来说，函数对象是一个可以被当作函数来用的对象。它有时也会被叫做 functor，但这个术语在范畴论里有着完全不同的含义，还是不用为妙——否则玩函数式编程的人可能会朝着你大皱眉头的。

下面定义了一个简单的加 n 的**函数对象类**：

``` cpp
struct adder {
  adder(int n) : n_(n) {}
  int operator()(int x) const
  {
    return x + n_;
  }
private:
  int n_;
};

adder add_2(2);
int res = add_2(5);       // 2 + 5 = 7 
```

### 函数的指针和引用











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

网络就看 Boost.Asio 吧。这个将是未来 C++ 网络标准库的基础。

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








