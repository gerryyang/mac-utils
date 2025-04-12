---
layout: post
title:  "CPP Smart Pointers in Action"
date:   2021-11-24 08:30:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}


# TL;DR

**std::shared_ptr 与 std::unique_ptr 的对比：**

| 特性 | std::shared_ptr | std::unique_ptr
| -- | -- | --
| **所有权** | 共享（多个所有者） | 独占（唯一所有者）
| **性能** | 引用计数原子操作 | 无额外开销
| **拷贝语义** | 允许拷贝 | 禁止拷贝，支持移动
| **适用场景** | 需要共享所有权的资源 | 明确独占所有权的资源


**设计哲学**：**std::unique_ptr** 体现了 C++ 的 资源获取即初始化（`RAII`） 原则，将资源生命周期与对象作用域绑定，避免内存泄漏和资源未释放问题。



# std::shared_ptr

`std::shared_ptr` 是 C++11 引入的智能指针类型，用于自动管理对象的生命周期。当 `std::shared_ptr` 变量被赋值为 `nullptr`（C++11 中引入的空指针字面值）时，它表示该智能指针不指向任何对象。当一个 `std::shared_ptr` 变量被赋值为 nullptr 时，它会自动释放与之前关联的对象的引用。**如果这是指向该对象的最后一个 `std::shared_ptr`，则对象将被自动删除**。

``` cpp
#include <iostream>
#include <memory>

class MyClass {
public:
    MyClass() { std::cout << "MyClass constructed" << std::endl; }
    ~MyClass() { std::cout << "MyClass destroyed" << std::endl; }
};

int main() {
    std::shared_ptr<MyClass> ptr1 = std::make_shared<MyClass>();
    std::shared_ptr<MyClass> ptr2 = ptr1; // ptr1 和 ptr2 共享同一个对象
    std::cout << "Initial use_count: " << ptr1.use_count() << std::endl;

    ptr1 = nullptr; // ptr1 不再引用对象，但 ptr2 仍然引用它
    std::cout << "after ptr1 = nullptr, use_count: " << ptr2.use_count() << std::endl;

    ptr2 = nullptr; // ptr2 不再引用对象，因为没有其他 shared_ptr 引用它，所以对象被删除
    std::cout << "after ptr2 = nullptr, use_count: " << ptr2.use_count() << std::endl;
    return 0;
}
/*
MyClass constructed
Initial use_count: 2
after ptr1 = nullptr, use_count: 1
MyClass destroyed
after ptr2 = nullptr, use_count: 0
*/
```

``` cpp
#include <iostream>
#include <memory>

class A {
    public:
        A() {
            std::cout << "A()\n";
        }
        ~A() {
            std::cout << "~A()\n";
        }
};

int main() {
    std::shared_ptr<A> ptr1(new A, [](A * p) {
        delete p;
        std::cout << "over\n";}
    );

    std::shared_ptr < A > ptr2 = ptr1;
    std::cout << ptr1.use_count() << std::endl;
}
/*
A()
2
~A()
over
*/
```

`std::shared_ptr` is a smart pointer that retains(保持，保存) shared ownership of an object through a pointer. **Several `shared_ptr` objects may own the same object (几个 shared_ptr 对象可以共同持有同一个对象)**. The object is destroyed and its memory deallocated when either of the following happens (**持有对象销毁的时机在满足以下条件**):

* the last remaining `shared_ptr` owning the object is destroyed; (**当所持对象的最后一个 `shared_ptr` 销毁时，才对所持的对象进行销毁**)

``` cpp
#include <iostream>
#include <memory>

class A {
    public:
        A() {
            std::cout << "A()\n";
        }
        ~A() {
            std::cout << "~A()\n";
        }
};

int main() {
    {
        std::shared_ptr<A> ptr1(new A, [](A * p) {
            delete p;
            std::cout << "over\n";}
        );

        std::cout << ptr1.use_count() << std::endl;
    }
    std::cout << "end\n";
}
/*
A()
1
~A()
over
end
*/
```

* the last remaining shared_ptr owning the object is assigned another pointer via `operator=` or `reset()`. (**当所持对象的最后一个 shared_ptr 通过 `operator=` 赋值为另一个指针，或者 `reset()`(replaces the managed object) 替换为另一个对象时，才对所持的对象进行销毁**)

``` cpp
#include <iostream>
#include <memory>

class A {
    public:
        A() {
            std::cout << "A()\n";
        }
        ~A() {
            std::cout << "~A()\n";
        }
};

int main() {
    std::shared_ptr<A> ptr1(new A, [](A * p) {
        delete p;
        std::cout << "over\n";}
    );

    std::cout << ptr1.use_count() << std::endl;
    ptr1 = nullptr;
    std::cout << ptr1.use_count() << std::endl;
}
/*
A()
1
~A()
over
0
*/
```

``` cpp
#include <iostream>
#include <memory>

class A {
    public:
    A() {
        std::cout << "A()\n";
    }
    ~A() {
        std::cout << "~A()\n";
    }
};

int main() {
    std::shared_ptr<A> ptr1(new A, [](A * p) {
        delete p;
        std::cout << "over\n";}
    );
    std::cout << ptr1.use_count() << std::endl;

    std::cout << "before reset\n";
    ptr1.reset(new A);
    std::cout << "after reset\n";
}
/*
A()
1
before reset
A()
~A()
over
after reset
~A()
*/
```

The object is destroyed using **delete-expression** or a **custom deleter** that is supplied to `shared_ptr` during construction.

``` cpp
// Constructs a shared_ptr with ptr as the pointer to the managed object.
template< class Y >
explicit shared_ptr( Y* ptr );

template< class Y, class Deleter >
shared_ptr( Y* ptr, Deleter d );

template< class Y, class Deleter, class Alloc >
shared_ptr( Y* ptr, Deleter d, Alloc alloc );
```


## std::shared_ptr 的存储结构

In a typical implementation, `std::shared_ptr` holds **only two pointers**: ([refer std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr))

* **the stored pointer** (one returned by `get()`)
* **a pointer to control block**

在典型实现中，`std::shared_ptr` 包含 两个指针：

+ **存储指针（Stored Pointer）**
    - 通过 `get()` 方法返回的指针，指向实际管理的对象。
+ **控制块指针（Control Block Pointer）**
    - 指向一个动态分配的控制块（Control Block），用于管理对象的生命周期。

The **control block** is a dynamically-allocated object that holds:

* either a pointer to the managed object or the managed object itself; (对应下述`shared_ptr`两种不同的初始化创建方式)
* the deleter (type-erased);
* the allocator (type-erased);
* the number of shared_ptrs that own the managed object;
* the number of weak_ptrs that refer to the managed object.

**控制块（Control Block）的结构：**

| 成员 | 作用
| -- | --
| 指向对象的指针 | 可能是原始对象的指针，或对象本身
| 删除器（Deleter） | 用于销毁对象的函数或函数对象（类型擦除，支持自定义删除逻辑）
| 分配器（Allocator）| 用于分配内存的分配器（类型擦除，通常使用默认分配器）
| 共享引用计数 | 记录当前有多少个 std::shared_ptr 拥有对象所有权
| 弱引用计数 | 记录当前有多少个 std::weak_ptr 观察该对象（用于延长控制块的生命周期）



**控制块的创建方式：** (控制块的初始化方式直接影响其内存布局)

**场景 1：通过 `new` 创建 `shared_ptr`**

``` cpp
std::shared_ptr<Object> ptr(new Object);
```

* 对象在堆上单独分配（通过 new）
* 控制块在堆上单独分配（包含指向对象的指针）
* 内存碎片化：两次独立分配可能导致内存碎片

通过 `new` 创建：

```
+----------------+      +-----------------+
| shared_ptr     |      | Control Block   |
| - stored_ptr   | ---> | - Object*       |
| - control_block| --+  | - Deleter       |
+----------------+   |  | - Allocator     |
                     |  | - use_count = 1 |
                     |  | - weak_count = 0|
                     |  +-----------------+
                     |
                     +--> +------------+
                          | Object     |
                          +------------+
```

**场景 2：通过 `std::make_shared` 创建** - 对象和控制块在**同一块连续内存中分配**（通过一次内存分配完成）

``` cpp
auto ptr = std::make_shared<Object>();
```

* 优势：减少内存分配次数，提升性能，避免内存碎片
* 代价：对象和控制块生命周期绑定，即使所有 `shared_ptr` 销毁，若仍有 `weak_ptr` 存在，对象占用的内存不会立即释放（需等控制块释放）

通过 `make_shared` 创建：

```
+----------------+      +-----------------------------+
| shared_ptr     |      | Combined Control Block      |
| - stored_ptr   | ---> | - Object (embedded)         |
| - control_block| --+  | - Deleter                   |
+----------------+   |  | - Allocator                 |
                     |  | - use_count = 1             |
                     |  | - weak_count = 0            |
                     |  +-----------------------------+
                     +---（对象和控制块在同一内存块）
```


**共享引用计数与弱引用计数：**

1. 共享引用计数（use_count）
   * 当 shared_ptr 被拷贝或赋值时递增，销毁时递减
   * 归零时，调用删除器销毁对象（但控制块可能仍存在，直到弱引用计数归零）

2. 弱引用计数
    * 当 weak_ptr 被拷贝或赋值时递增，销毁时递减
    * 归零时，释放控制块内存（若共享引用计数已归零）

**类型擦除（Type Erasure）**

控制块中的 **删除器** 和 **分配器** 通过类型擦除技术存储，使得 `std::shared_ptr` 可以：

* 支持任意类型的删除器和分配器（如自定义的 Deleter 或内存池）
* 保持统一的接口，无需为不同类型生成额外的模板代码



## std::shared_ptr 的两种初始化方式

**方式 1：一次分配 (推荐方式)**

When `shared_ptr` is created by calling `std::make_shared` or `std::allocate_shared`, the memory for **both the control block and the managed object** is created with a single allocation. The managed object is constructed in-place in a data member of the control block.

**方式 2：两次分配**

When `shared_ptr` is created via one of the `shared_ptr` constructors, the managed object and the control block must be allocated separately. In this case, the control block stores a pointer to the managed object.


## std::shared_ptr 的销毁方式

The destructor of `shared_ptr` decrements the number of shared owners of the control block. If that counter reaches zero, the control block calls the destructor of the managed object. The control block does not deallocate itself until the `std::weak_ptr` counter reaches zero as well.

**shared_ptr 的析构过程：**

当 `std::shared_ptr` 的析构函数被调用时：

1. 共享引用计数（`use_count`）递减
   + 表示当前 `shared_ptr` 放弃对对象的所有权。

2. 检查共享引用计数是否归零
   + 若归零，控制块会调用托管对象的析构函数（销毁对象），并释放对象占用的内存（除非使用 `std::make_shared` 且仍有 `weak_ptr` 存在）
   + 若未归零，仅减少引用计数，对象和控制块继续保留

``` cpp
{
    auto sp1 = std::make_shared<int>(42); // use_count = 1
    {
        auto sp2 = sp1; // use_count = 2
    } // sp2 析构，use_count = 1
} // sp1 析构，use_count = 0 → 对象被销毁
```

**控制块的释放条件：**

控制块的生命周期由两个计数器共同决定：

* 共享引用计数（`use_count`）：管理对象的生命周期
* 弱引用计数（`weak_count`）：管理控制块自身的生命周期

当以下条件满足时，控制块才会被释放：

1. `use_count == 0`：对象已被销毁
2. `weak_count == 0`：没有 `weak_ptr` 再观察该控制块

``` cpp
std::weak_ptr<int> wp;

{
    auto sp = std::make_shared<int>(42); // use_count=1, weak_count=0
    wp = sp; // weak_count=1
} // sp 析构 → use_count=0 → 对象被销毁，但控制块保留（weak_count=1）

// 此时 wp 仍指向控制块，但对象已销毁
if (wp.expired()) { /* 对象已释放 */ }

// 当 wp 析构时 → weak_count=0 → 控制块被释放
```

**为什么控制块需要等待 `weak_count` 归零？**

`std::weak_ptr` 需要通过控制块判断对象是否存在（如调用 `lock()` 或 `expired()`）。

若控制块过早释放：

* `weak_ptr` 无法安全检查对象状态
* 尝试通过 `lock()` 获取 `shared_ptr` 会导致未定义行为

因此，控制块必须保留至最后一个 `weak_ptr` 析构。

**特殊情况：std::make_shared 的优化：**

当使用 `std::make_shared` 时，**对象和控制块分配在连续内存中**：

* 对象销毁时机：当 `use_count` 归零时，调用析构函数
* 内存释放时机：当 `weak_count` 归零时，整个内存块（包含对象和控制块）被释放

**这意味着，即使对象已被销毁，其占用的内存可能不会立即归还给系统（需等待控制块释放）**。

``` cpp
#include <memory>
#include <iostream>

struct Object {
    ~Object() { std::cout << "Object destroyed\n"; }
};

int main() {
    std::weak_ptr<Object> wp;

    {
        auto sp = std::make_shared<Object>(); // use_count=1, weak_count=0
        wp = sp; // weak_count=1
        std::cout << "use_count: " << sp.use_count()
                  << ", weak_count: " << wp.use_count() << "\n";
    } // sp 析构 → use_count=0 → 对象销毁，但控制块保留（weak_count=1）

    std::cout << "Control block still exists? " << !wp.expired() << "\n";

    // 最后一个 weak_ptr 析构 → weak_count=0 → 控制块释放
}
/*
use_count: 1, weak_count: 1
Object destroyed
Control block still exists? 0
*/
```

**总结**

1. `shared_ptr` 析构行为：减少共享引用计数，归零时销毁对象。
2. 控制块释放条件：共享和弱引用计数均归零。
3. 设计意义：确保 `weak_ptr` 能安全检测对象状态，避免悬垂指针。
4. 优化权衡：`std::make_shared` 减少内存分配次数，但耦合对象与控制块的生命周期。



## std::shared_ptr 的线程安全

To satisfy thread safety requirements, the reference counters are typically incremented using an equivalent of `std::atomic::fetch_add` with `std::memory_order_relaxed`(**松散内存序，只用来保证对原子对象的操作是原子的**) (decrementing requires stronger ordering to safely destroy the control block).

> All member functions (including copy constructor and copy assignment) can be called by multiple threads on different instances of `shared_ptr` without additional synchronization even if these instances are copies and share ownership of the same object.

所有成员函数（包括拷贝构造函数和拷贝赋值运算符）都可以由多个线程在不同的 shared_ptr 实例上调用，且无需额外的同步机制，即使这些实例是同一对象的副本并共享所有权。


`std::shared_ptr` 的线程安全性涉及多个层面，包括**引用计数的原子性操作**、**成员函数的并发调用**，以及**托管对象本身的线程安全**。

## 引用计数的线程安全性

`std::shared_ptr` 的引用计数（`use_count` 和 `weak_count`）通过原子操作（`std::atomic`）实现。

* 递增操作（拷贝 `shared_ptr` 或 `weak_ptr`）：使用 `std::atomic::fetch_add` 配合 宽松内存序（`std::memory_order_relaxed`），仅保证操作的原子性，不保证内存顺序的同步。
* 递减操作（析构 `shared_ptr` 或 `weak_ptr`）：使用更强的内存序（如 `std::memory_order_acq_rel`），确保在引用计数归零时，正确同步对象的析构和控制块的释放。

设计意义：

1. 引用计数的原子性保证多个线程并发操作时不会导致计数错误。
2. 递减操作需要强内存序，确保析构对象和释放内存的可见性。


## 成员函数的线程安全性

`std::shared_ptr` 的 **所有成员函数**（包括拷贝构造、拷贝赋值、析构等）可以在不同线程的 **不同实例** 上并发调用，无需额外同步。


## 托管对象的线程安全性

`std::shared_ptr` 仅保证引用计数的线程安全，对托管对象的访问需用户自行管理。

若多个线程通过不同的 `shared_ptr` 实例访问同一对象，且至少有一个线程修改对象，需使用同步机制（如互斥锁）。

``` cpp
auto obj = std::make_shared<int>(0);

// 线程1（写操作）
std::thread t1([&obj]() {
    *obj = 42; // 需要同步
});

// 线程2（读操作）
std::thread t2([&obj]() {
    std::cout << *obj; // 需要同步
});

t1.join(); t2.join();
```

线程1和线程2对同一内存地址的读写未同步，导致未定义行为。





## 同一实例的非线程安全场景

以下代码展示了在多线程中直接操作同一 `std::shared_ptr` 实例导致的 **数据竞争** 和 **未定义行为**：

``` cpp
#include <memory>
#include <thread>
#include <iostream>

struct Object {
    int value = 42;
    ~Object() { std::cout << "Object destroyed\n"; }
};

std::shared_ptr<Object> p = std::make_shared<Object>();

// 线程1：修改 p（reset 操作）
void thread_reset() {
    p.reset(new Object); // 非线程安全！
}

// 线程2：读取 p 的引用计数和值
void thread_read() {
    if (!p.expired()) { // 非线程安全！
        std::shared_ptr<Object> local_p = p; // 非线程安全！
        std::cout << "Value: " << local_p->value
                  << ", use_count: " << local_p.use_count() << "\n";
    }
}

int main() {
    std::thread t1(thread_reset);
    std::thread t2(thread_read);
    t1.join();
    t2.join();
    return 0;
}
```

**数据竞争的产生：**

* 线程1 调用 p.reset(new Object)，这会修改 p 的存储指针和控制块指针
* 线程2 同时调用 p.expired() 或拷贝 p 到 local_p，这些操作需要读取 p 的指针值

若两个线程同时操作 p，p 的内部指针和控制块指针的读写可能交叉执行，导致：

1. 悬垂指针：线程2可能读取到已被 reset() 释放的旧指针
2. 内存泄漏：旧对象的引用计数未正确归零
3. 双重释放：新旧控制块可能被错误管理

**std::shared_ptr 的非原子操作：**

reset() 的步骤：

1. 创建新对象和控制块
2. 递减旧对象的引用计数（可能触发析构）
3. 将 p 的指针指向新对象

这些步骤不是原子的，多线程并发操作会导致中间状态暴露。

expired() 和拷贝操作：

需要读取 p 的控制块指针，若此时 p 正在被修改，可能读到无效指针。


**解决方案：使用互斥锁同步**

``` cpp
#include <mutex>

std::mutex mtx; // 全局互斥锁
std::shared_ptr<Object> p = std::make_shared<Object>();

void thread_reset() {
    std::lock_guard<std::mutex> lock(mtx);
    p.reset(new Object); // 加锁后安全修改
}

void thread_read() {
    std::lock_guard<std::mutex> lock(mtx);
    std::shared_ptr<Object> local_p = p; // 加锁后安全读取
    if (local_p) {
        std::cout << "Value: " << local_p->value
                  << ", use_count: " << local_p.use_count() << "\n";
    }
}
```


结论：

| 操作类型 | 线程安全 | 是否需要同步
| -- | -- | --
| 不同实例的拷贝/析构 | 安全（引用计数原子操作）| 否
| 同一实例的并发 reset() | 不安全（修改内部指针） | 是
| 同一实例的并发读/写访问 | 不安全（指针和控制块状态不一致） | 是

> 补充：避免跨线程直接修改同一 shared_ptr 实例，否则需要使用 `std::atomic<std::shared_ptr>` 同步原语或互斥锁。C++20 提供了 `std::atomic<std::shared_ptr>`，允许原子地修改 shared_ptr。




## 其他

参考：[std::shared_ptr thread safety explained](https://stackoverflow.com/questions/9127816/stdshared-ptr-thread-safety-explained)：

* Standard guarantees that reference counting is handled thread safe and it's platform independent, right?

Correct, `shared_ptrs` use atomic increments/decrements of a reference count value.

* Similar issue - standard guarantees that only one thread (holding last reference) will call delete on shared object, right?

The standard guarantees only one thread will call the delete operator on a shared object. I am not sure if it specifically specifies the last thread that deletes its copy of the shared pointer will be the one that calls delete (likely in practice this would be the case).

* `shared_ptr` does not guarantee any thread safety for object stored in it?

No they do not, the object stored in it can be simultaneously edited by multiple threads.

参考：[std::shared_ptr thread safety](https://stackoverflow.com/questions/14482830/stdshared-ptr-thread-safety/65615682#65615682)

`std::shared_ptr` **is not thread safe**.

A shared pointer is a pair of two pointers, one to the object and one to a control block (holding the ref counter, links to weak pointers ...).

There can be multiple `std::shared_ptr` and whenever they access the control block to change the reference counter it's thread-safe **but the `std::shared_ptr` itself is NOT thread-safe or atomic.**

**If you assign a new object to a `std::shared_ptr` while another thread uses it, it might end up with the new object pointer but still using a pointer to the control block of the old object => CRASH**.


## std::shared_ptr 的性能开销

使用[Quick C++ Benchmark](https://quick-bench.com/q/xDjerjaF4ORhGTPtB5P37NZ78Go) GCC8.1 -O2 编译测试对比：

``` cpp
#include <memory>

static void Test1(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    std::shared_ptr<std::string> s1 = std::make_shared<std::string>("hello");
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(s1);
  }
}
// Register the function as a benchmark
BENCHMARK(Test1);

static void Test2(benchmark::State& state) {
  // Code before the loop is not measured
  for (auto _ : state) {
    std::string* s2 = new std::string("hello");
    delete s2;
    benchmark::DoNotOptimize(s2);
  }
}
BENCHMARK(Test2);
```

对比结果：`std::shared_ptr<std::string>`比`std::string*`要慢`1.6`倍。

![smart_pointer1](/assets/images/202111/smart_pointer1.png)

``` cpp
#include <memory>

static void Test1(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    std::shared_ptr<int> a = std::make_shared<int>(0);
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(a);
  }
}
// Register the function as a benchmark
BENCHMARK(Test1);

static void Test2(benchmark::State& state) {
  // Code before the loop is not measured
  for (auto _ : state) {
    int* a = new int(0);
    delete a;
    benchmark::DoNotOptimize(a);
  }
}
BENCHMARK(Test2);
```

使用`-O2`优化编译：`std::shared_ptr<int>`比`int*`要慢`2.1`倍。

![smart_pointer2](/assets/images/202111/smart_pointer2.png)

使用`-O3`优化编译：`std::shared_ptr<int>`比`int*`要慢`2.5`倍。

![smart_pointer4](/assets/images/202111/smart_pointer4.png)

不使用编译优化对比：`std::shared_ptr<int>`比`int*`要慢`17`倍。

![smart_pointer3](/assets/images/202111/smart_pointer3.png)



## std::shared_ptr 性能开销原因分析

通过[How much is the overhead of smart pointers compared to normal pointers in C++?](https://stackoverflow.com/questions/22295665/how-much-is-the-overhead-of-smart-pointers-compared-to-normal-pointers-in-c) 文章：

> 问题：How much is the overhead of smart pointers compared to normal pointers in C++11? In other words, is my code going to be slower if I use smart pointers, and if so, how much slower? Specifically, I'm asking about the C++11 std::shared_ptr and std::unique_ptr.

观点1:

* `std::shared_ptr` always has memory overhead for reference counter, though it is very small.
* `std::shared_ptr` has time overhead in **constructor** (to create the reference counter), in **destructor** (to decrement the reference counter and possibly destroy the object) and in **assignment operator** (to increment the reference counter). **Due to thread-safety guarantees(保证线程安全) of `std::shared_ptr`, these increments/decrements are atomic, thus adding some more overhead**.
* Note that none of them has time overhead in dereferencing (in getting the reference to owned object), while this operation seems to be the most common for pointers.

To sum up, there is some overhead, but it shouldn't make the code slow unless you continuously create and destroy smart pointers.


观点2:

My answer is different from the others and i really wonder if they ever profiled code.

`shared_ptr` **has a significant overhead for creation** because of it's memory allocation for the control block (which keeps the ref counter and a pointer list to all weak references). **It has also a huge memory overhead** because of this and the fact that `std::shared_ptr` is always a 2 pointer tuple (one to the object, one to the control block).

If you pass a shared_pointer to a function as **a value parameter** then **it will be at least 10 times slower than a normal call** and create lots of codes in the code segment for the stack unwinding. If you pass it by **reference** you get an additional indirection which can be also pretty worse in terms of performance.

Thats why you should not do this **unless the function is really involved in ownership management**. Otherwise use "shared_ptr.get()". It is not designed to make sure your object isn't killed during a normal function call.

If you go mad and use shared_ptr on small objects like an abstract syntax tree in a compiler or on small nodes in any other graph structure you will see a huge perfomance drop and a huge memory increase. I have seen a parser system which was rewritten soon after C++14 hit the market and before the programmer learned to use smart pointers correctly. The rewrite was a magnitude slower then the old code.

t is not a silver bullet and raw pointers aren't bad by definition either. Bad programmers are bad and bad design is bad. Design with care, design with clear ownership in mind and try to use the shared_ptr mostly on the subsystem API boundary.

If you want to learn more you can watch [Nicolai M. Josuttis good talk about "The Real Price of Shared Pointers in C++"](https://vimeo.com/131189627)

It goes deep into the implementation details and CPU architecture for write barriers, atomic locks etc. once listening you will never talk about this feature being cheap. If you just want a proof of the magnitude slower, skip the first 48 minutes and **watch him running example code which runs upto 180 times slower (compiled with -O3) when using shared pointer everywhere.**

And if you ask about "std::unique_ptr" than visit this talk ["CppCon 2019: Chandler Carruth “There Are No Zero-cost Abstractions”](https://www.youtube.com/watch?v=rHIkrotSwcc)

Its just not true, that unique_ptr is 100% cost free.


观点3:

As with all code performance, the only really reliable means to obtain hard information is to **measure and/or inspect machine code**.

* You can expect some overhead in debug builds, since e.g. `operator->` must be executed as a function call so that you can step into it (this is in turn due to general lack of support for marking classes and functions as non-debug).

* For `shared_ptr` you can expect some overhead in **initial creation**, since that involves **dynamic allocation of a control block**, and dynamic allocation is very much slower than any other basic operation in C++ (do use `make_shared` when practically possible, to minimize that overhead).

* Also for `shared_ptr` there is some minimal overhead in maintaining a reference count, e.g. when passing a `shared_ptr` by value, but there's no such overhead for `unique_ptr`.

The international C++ standardization committee has published [a technical report on performance](http://www.open-std.org/jtc1/sc22/wg21/docs/TR18015.pdf), but this was in 2006, before `unique_ptr` and `shared_ptr` were added to the standard library. Still, smart pointers were old hat at that point, so the report considered also that. Quoting the relevant part:

> “if accessing a value through a trivial smart pointer is significantly slower than accessing it through an ordinary pointer, the compiler is inefficiently handling the abstraction. In the past, most compilers had significant abstraction penalties and several current compilers still do. However, at least two compilers have been reported to have abstraction penalties below 1% and another a penalty of 3%, so eliminating this kind of overhead is well within the state of the art”

As an informed guess, the “well within the state of the art” has been achieved with the most popular compilers today, as of early 2014.




# std::unique_ptr

`std::unique_ptr` is a smart pointer that owns and manages another object through a pointer and disposes of that object when the unique_ptr goes out of scope.

std::unique_ptr 是 C++ 中一种独占所有权的智能指针，用于管理动态分配对象的生命周期。它通过 **独占式所有权（exclusive ownership）** 确保资源的安全释放，**同时支持高效的移动语义**。

**核心特性：**

1. **独占所有权**
   * 每个 unique_ptr 唯一拥有其指向的对象，不可被其他 unique_ptr 共享
   * 所有权可通过 **移动语义**（move semantics） 转移，但 **不可拷贝**（拷贝构造和拷贝赋值被删除）
2. **自动资源释放**
   * 当 unique_ptr 被销毁（离开作用域）或被 reset() 时，自动调用删除器销毁对象
   * 删除器默认使用 delete（或 delete[] 用于数组），但支持自定义
3. **轻量高效**
   * 与裸指针几乎相同的性能开销（无额外引用计数）
   * 内存占用通常等同于裸指针（除非自定义删除器引入状态）


The object is disposed of, using the associated deleter when either of the following happens:

* the managing unique_ptr object is destroyed
* the managing unique_ptr object is assigned another pointer via `operator=` or `reset()`.

There are two versions of `std::unique_ptr`:

1. Manages a single object (e.g. allocated with `new`)
2. Manages a dynamically-allocated array of objects (e.g. allocated with `new[]`) (支持数组)

**两种版本：**

* 管理单个对象

``` cpp
std::unique_ptr<Object> ptr(new Object);

// 或使用 std::make_unique（C++14 引入）
auto ptr = std::make_unique<Object>();
```

* 管理动态数组

``` cpp
std::unique_ptr<Object[]> arr(new Object[5]);

// 使用 make_unique（C++14）
auto arr = std::make_unique<Object[]>(5);
```


> The class satisfies the requirements of MoveConstructible and MoveAssignable, but of neither CopyConstructible nor CopyAssignable. (构造函数需要满足**移动构造**和**移动赋值**，但是不能满足**拷贝构造**和**拷贝赋值**)

**移动语义与所有权转移：**

* 移动构造/赋值：允许所有权从一个 `unique_ptr` 转移到另一个

``` cpp
auto ptr1 = std::make_unique<Object>();
auto ptr2 = std::move(ptr1); // ptr1 变为 nullptr，ptr2 接管对象
```

* 不可拷贝

``` cpp
// 错误！拷贝构造被删除
std::unique_ptr<Object> ptr3 = ptr2;
```

Only `non-const unique_ptr` can transfer the ownership of the managed object to another unique_ptr. If an object's lifetime is managed by a `const std::unique_ptr`, it is limited to the scope in which the pointer was created.


**const unique_ptr 的限制：** `const` 修饰的 `unique_ptr` 无法转移所有权（无法移动或 `reset()`）

``` cpp
const auto ptr = std::make_unique<Object>();
auto ptr2 = std::move(ptr); // 错误！无法移动 const 对象
```


`std::unique_ptr` is commonly used to manage the lifetime of objects, including:

* providing exception safety to classes and functions that handle objects with dynamic lifetime, by guaranteeing deletion on both normal exit and exit through exception (保障异常安全)

* passing ownership of uniquely-owned objects with dynamic lifetime into functions

* acquiring ownership of uniquely-owned objects with dynamic lifetime from functions

* as the element type in move-aware containers, such as std::vector, which hold pointers to dynamically-allocated objects (e.g. if polymorphic(多态的) behavior is desired)

**关键操作：**

* release()          `返回裸指针并释放所有权，unique_ptr` 置空。需手动管理返回的指针
* reset(ptr)         释放当前对象，接管新指针（若 `ptr` 为空则仅释放当前对象）
* get()              返回裸指针，不释放所有权
* swap(other)        交换两个 `unique_ptr` 的所有权
* operator bool()    判断是否持有对象（等价于 `get() != nullptr`）

**异常安全：**

``` cpp
void process() {
    auto resource = std::make_unique<Resource>();
    // 即使后续代码抛出异常，resource 仍会被正确释放
    may_throw_exception();
}
```

**传递独占所有权到函数：**

``` cpp
// 函数接收所有权
void take_ownership(std::unique_ptr<Object> obj) {
    // 使用 obj
}

auto obj = std::make_unique<Object>();
take_ownership(std::move(obj)); // 转移所有权
```

**从函数返回独占资源：**

``` cpp
std::unique_ptr<Object> create_object() {
    return std::make_unique<Object>();
}

auto obj = create_object(); // 所有权转移到调用者
```

**容器存储多态对象：**

``` cpp
std::vector<std::unique_ptr<Base>> vec;
vec.push_back(std::make_unique<Derived1>());
vec.push_back(std::make_unique<Derived2>());

// 支持多态操作
vec[0]->virtual_method();
```

**自定义删除器：**

允许自定义对象销毁逻辑（如文件句柄、C 风格资源等）：

``` cpp
// 使用函数对象
struct FileDeleter {
    void operator()(FILE* file) {
        if (file) fclose(file);
    }
};

std::unique_ptr<FILE, FileDeleter> file_ptr(fopen("data.txt", "r"));

// 使用 lambda（需指定删除器类型）
auto deleter = [](FILE* file) { if (file) fclose(file); };
std::unique_ptr<FILE, decltype(deleter)> file_ptr(fopen("data.txt", "r"), deleter);
```






# std::weak_ptr

`std::weak_ptr` is a smart pointer that holds a non-owning ("weak") reference to an object that is managed by `std::shared_ptr`. It must be converted to `std::shared_ptr` in order to access the referenced object.

`weak_ptr` 是一个不拥有对象的智能指针，它引用由 `shared_ptr` 管理的对象。因为 `weak_ptr` 本身不增加引用计数，所以 `weak_ptr` 必须转换成 `shared_ptr` 才能访问对象。所以当原来的 `shared_ptr` 都被销毁后，对象会被释放，这时候 `weak_ptr` 就会知道对象已经不存在了。

`std::weak_ptr` models temporary ownership: when an object needs to be accessed only if it exists, and it may be deleted at any time by someone else, `std::weak_ptr` is used to track the object, and it is converted to `std::shared_ptr` to assume temporary ownership. If the original `std::shared_ptr` is destroyed at this time, the object's lifetime is extended until the temporary `std::shared_ptr` is destroyed as well.

**临时所有权是什么意思呢？**

当某个对象存在时，我们可能需要临时访问它，但不确定它是否已经被删除。这时候用 `weak_ptr` 来跟踪对象，需要的时候转换成 `shared_ptr`，这样如果原来的 `shared_ptr` 还存在，转换后的 `shared_ptr` 会增加引用计数，保证在使用期间对象不会被释放。如果原来的已经被销毁了，那么转换得到的 `shared_ptr` 会是空的，这样就能安全处理了。

**Another use for `std::weak_ptr` is to break reference cycles formed by objects managed by `std::shared_ptr`.** If such cycle is orphaned (i,e. there are no outside shared pointers into the cycle), the shared_ptr reference counts cannot reach zero and the memory is leaked. To prevent this, one of the pointers in the cycle can be made weak.

**什么是循环引用的问题？**

比如两个类 `A` 和 `B`，各自有一个 `shared_ptr` 指向对方，这样它们的引用计数都是 1，永远不会变成 0，导致内存泄漏。

如果其中一个使用 `weak_ptr`，比如 `A` 持有指向 `B` 的 `shared_ptr`，`B` 持有指向 `A` 的 `weak_ptr`。这样当外部的 `shared_ptr` 被销毁后，`A` 的引用计数可以降为 0，从而释放 `A`，接着 `B` 的引用计数也会降为 0，释放 `B`。这样就打破了循环。所以 `weak_ptr` 在这里的作用就是避免循环引用导致的内存泄漏。

Like `std::shared_ptr`, a typical implementation of `weak_ptr` stores two pointers:

* a pointer to the control block
* the stored pointer of the shared_ptr it was constructed from

A separate stored pointer is necessary to ensure that converting a `shared_ptr` to `weak_ptr` and then back works correctly, even for aliased shared_ptrs. It is not possible to access the stored pointer in a `weak_ptr` without locking it into a shared_ptr.

**怎么使用 weak_ptr 呢？**

当需要观察某个资源是否存在，但不想拥有所有权的时候。

例如，缓存中的对象可能被其他部分管理，缓存只需要在对象存在时提供访问，不存在时重新加载。这时候缓存可以用 `weak_ptr` 来保存这些对象的引用，当需要时尝试转换为 `shared_ptr`，如果成功则使用，否则重新加载。

另外，当需要将 `weak_ptr` 转换为 `shared_ptr` 时，可以使用 `lock()` 方法。这个方法会返回一个 `shared_ptr`，如果原来的对象还存在，那么这个 `shared_ptr` 会增加引用计数，确保对象在本次使用期间有效。否则返回空的 `shared_ptr`。

或者也可以使用 `weak_ptr` 的构造函数来创建 `shared_ptr`，但这样在对象不存在时会抛出异常，可能不如 `lock()` 安全。

**循环引用的代码示例：**

``` cpp
class B;

class A {
public:
    std::shared_ptr<B> b_ptr;
    ~A() { std::cout << "A destroyed\n"; }
};

class B {
public:
    std::shared_ptr<A> a_ptr; // 这里如果用 shared_ptr 就会循环引用
    ~B() { std::cout << "B destroyed\n"; }
};
```

当 `A` 和 `B` 互相持有对方的 `shared_ptr`，那么它们的引用计数不会归零。但如果将其中一个改为 `weak_ptr`，比如 `B` 中的 `a_ptr` 改为 `std::weak_ptr<A>`，那么当外部的 `shared_ptr` 被销毁后，`A` 的引用计数会减少到 0，从而释放 `A`，接着 `B` 的引用计数也会减少，从而释放 `B`。这样就避免了内存泄漏。

**什么时候应该用 weak_ptr 来避免循环引用呢？**

通常是在有双向引用的情况下，比如树结构中的父节点和子节点，或者观察者模式中的观察者和被观察者。在这些情况下，如果双方都用 `shared_ptr`，就可能出现循环。这时可以将其中一个方向改为 `weak_ptr`。

**那 weak_ptr 的另一个应用场景是缓存**

例如，一个对象缓存可能保存着一些不再被主要拥有的对象的 `weak_ptr`。当需要访问这些对象时，可以尝试提升为 `shared_ptr`，如果成功则使用缓存的对象，否则重新生成。这样，当对象不再被其他 `shared_ptr` 引用时，缓存中的 `weak_ptr` 不会阻止其被销毁，从而节省内存。

**weak_ptr 不会增加引用计数代码示例：**

``` cpp
std::weak_ptr<int> wptr;

{
    auto sptr = std::make_shared<int>(42);
    wptr = sptr;

    // 此时 sptr 引用计数是 1，wptr 不增加
}

// 离开作用域后，sptr 销毁，引用计数归零，int 对象被销毁

auto sptr2 = wptr.lock();
if (sptr2) {
    // 这里的代码不会执行，因为 wptr 已经失效
} else {
    // 处理对象不存在的情况
}
```

在转换的时候，如果原来的 `shared_ptr` 还存在，则 `lock()` 返回有效的 `shared_ptr`，此时引用计数会增加，保证在 `shared_ptr` 存在期间对象不会被释放。

> 总结，weak_ptr 的两个主要用途：

* **临时所有权（Temporary Ownership）**。临时获取 `shared_ptr` 以安全访问可能已被释放的对象
    + 作用：`weak_ptr` 用于观察一个由 `shared_ptr` 管理的对象，但不拥有其所有权（不增加引用计数）。
    + 使用场景：当需要访问一个对象但不确定其是否仍存在时（可能已被其他 `shared_ptr` 释放），可通过 `weak_ptr` 安全地尝试获取临时所有权。
    + 操作：通过 `lock()` 方法将 `weak_ptr` 转换为 `shared_ptr`
      - 若对象仍存在，`lock()` 返回一个有效的 `shared_ptr`，此时引用计数增加，对象的生命周期被延长至该临时 `shared_ptr` 销毁。
      - 若对象已被释放，`lock()` 返回空的 `shared_ptr`，避免悬垂指针。

``` cpp
std::weak_ptr<Object> weak;

{
    auto shared = std::make_shared<Object>();
    weak = shared; // 不增加引用计数
    // shared 离开作用域，对象销毁
}

auto temp = weak.lock(); // 返回空指针
if (temp) {
    /* 安全使用对象 */
}
```

* **打破循环引用（Breaking Reference Cycles）**。打破 shared_ptr 之间的循环引用，防止内存泄漏
    + 问题：若多个 `shared_ptr` 形成环形依赖（如双向链表、父子节点），它们的引用计数无法归零，导致内存泄漏。
    + 解决方案：将环形依赖中的某一环替换为 `weak_ptr`，使其不参与引用计数。

``` cpp
class B; // 前向声明

class A {
public:
    std::shared_ptr<B> b_ptr; // 强引用 B
    ~A() { std::cout << "A destroyed\n"; }
};

class B {
public:
    std::weak_ptr<A> a_weak; // 弱引用 A，打破循环
    ~B() { std::cout << "B destroyed\n"; }
};

int main() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();
    a->b_ptr = b;
    b->a_weak = a; // 使用 weak_ptr 而非 shared_ptr

    // a 和 b 的引用计数均为 1，退出作用域后正常销毁
}
```


# std::auto_ptr

``` cpp
// (deprecated in C++11)
// (removed in C++17)
template< class T > class auto_ptr;

// (deprecated in C++11)
// (removed in C++17)
template<> class auto_ptr<void>;
```

`auto_ptr` is a smart pointer that manages an object obtained via new expression and deletes that object when auto_ptr itself is destroyed. It may be used to provide exception safety for dynamically allocated objects, for passing ownership of dynamically allocated objects into functions and for returning dynamically allocated objects from functions.

Copying an `auto_ptr` copies the pointer and transfers ownership to the destination: **both copy construction and copy assignment of auto_ptr modify their right-hand arguments (会修改右边的操作数), and the "copy" is not equal to the original. Because of these unusual copy semantics, auto_ptr may not be placed in standard containers. `std::unique_ptr` is preferred for this and other uses. (since C++11)**

``` cpp
void f()
{
    std::auto_ptr<Investment> pInv(createInvestment());

    // do something
    // ...

    // 最后由auto_ptr的析构函数自动删除pInv
}
```

* 获得资源后立刻放进管理对象内。即，资源取得时机便是初始化时机（Resource Acquisition Is Initialization; RAII）
* 管理对象运用析构函数确保资源被释放。

不论控制流如何离开区块，一旦对象被销毁，其析构函数会被自动调用，于是资源被释放。

> 注意：由于auto_ptr被销毁时会自动删除它所指之物，所以一定要注意别让多个auto_ptr同时指向同一对象，否则对象会被删除一次以上。为了预防这个问题，auto_ptr有一个特性是，若通过copy构造函数或copy assignment操作符复制它们，它们会变成null，而复制所得的指针将取得资源的唯一拥有权。

``` cpp
// pInv1指向createInvestment返回物
std::auto_ptr<Investment> pInv1(createInvestment());

// 现在pInv2指向对象，pInv1被设为null
std::auto_ptr<Investment> pInv2(pInv1);

// 现在pInv1指向对象，pInv2被设为null
pInv1 = pInv2;
```

带来的问题：由于STL容器要求其元素发挥“正常的”复制行为，而`auto_ptr`的这种诡异的复制行为，导致其不符合STL的容器要求。

https://en.cppreference.com/w/cpp/memory/auto_ptr

# 用法示例

## std::shared_ptr

参考：https://en.cppreference.com/w/cpp/memory/shared_ptr

例子1：构造函数

``` cpp
// shared_ptr constructor example
#include <iostream>
#include <memory>

struct C
{
    int* data;
};

int main ()
{
  std::shared_ptr<int> p1;
  std::shared_ptr<int> p2 (nullptr);
  std::shared_ptr<int> p3 (new int);
  std::shared_ptr<int> p4 (new int, std::default_delete<int>());
  std::shared_ptr<int> p5 (new int, [](int* p){delete p;}, std::allocator<int>());
  std::shared_ptr<int> p6 (p5);
  std::shared_ptr<int> p7 (std::move(p6));
  std::shared_ptr<int> p8 (std::unique_ptr<int>(new int));

  std::shared_ptr<C> obj (new C);
  std::shared_ptr<int> p9 (obj, obj->data);

  std::cout << "use_count:\n";
  std::cout << "p1: " << p1.use_count() << '\n';  // 0
  std::cout << "p2: " << p2.use_count() << '\n';  // 0
  std::cout << "p3: " << p3.use_count() << '\n';  // 1
  std::cout << "p4: " << p4.use_count() << '\n';  // 1
  std::cout << "p5: " << p5.use_count() << '\n';  // 2
  std::cout << "p6: " << p6.use_count() << '\n';  // 0
  std::cout << "p7: " << p7.use_count() << '\n';  // 2
  std::cout << "p8: " << p8.use_count() << '\n';  // 1
  std::cout << "p9: " << p9.use_count() << '\n';  // 2
}
```

例子2：对象管理

``` cpp
#include <iostream>
#include <memory>

class Foo
{
public:
    Foo(int a) : m_a(a) { std::cout << "Foo(" << a << ")\n"; }
    ~Foo() { std::cout << "~Foo(" << m_a << ")\n"; }
    int Get() { return m_a; }
    void Set(int a) { m_a = a; }
private:
    int m_a;
};

int main()
{
    Foo *foo1 = new Foo(1);
    std::cout << "foo1: " <<  foo1 << ", " << foo1->Get() << std::endl;

    {
        std::shared_ptr<Foo> foo2 = std::make_shared<Foo>(2);
        std::cout << "foo2: " << foo2 << ", " << foo2->Get() << std::endl;

        std::cout << "before reset, foo2 use_count: " << foo2.use_count() << std::endl;
        // foo2 is destructed and then constructs with foo1
        foo2.reset(foo1);
        std::cout << "after reset, foo2 use_count: " << foo2.use_count() << std::endl;

        std::cout << "after reset, foo1: " << foo1 << ", " << foo1->Get() << std::endl;
        std::cout << "after reset, foo2: " << foo2 << ", " << foo2->Get() << std::endl;

        foo1->Set(3);
        std::cout << "after foo1->Set(3), foo1: " << foo1 << ", " << foo1->Get() << std::endl;
        std::cout << "after foo1->Set(3), foo2: " << foo2 << ", " << foo2->Get() << std::endl;

        // foo2 destruct, delete foo1 buffer
    }
    std::cout << "foo1 is deleted\n";

    // error, foo1 is deleted
    std::cout << "after foo2 dtor, foo1: " << foo1 << ", " << foo1->Get() << std::endl;
}
/*
Foo(1)
foo1: 0x11b4d30, 1
Foo(2)
foo2: 0x11b51e0, 2
before reset, foo2 use_count: 1
~Foo(2)
after reset, foo2 use_count: 1
after reset, foo1: 0x11b4d30, 1
after reset, foo2: 0x11b4d30, 1
after foo1->Set(3), foo1: 0x11b4d30, 3
after foo1->Set(3), foo2: 0x11b4d30, 3
~Foo(3)
foo1 is deleted
after foo2 dtor, foo1: 0x11b4d30, 18567616
*/
```

例子3：分配与释放

``` cpp
#include <memory>
#include <iostream>

struct Foo {
    Foo() { std::cout << "Foo...\n"; }
    ~Foo() { std::cout << "~Foo...\n"; }
};

struct D {
    void operator()(Foo* p) const {
        std::cout << "Call delete from function object...\n";
        delete p;
    }
};

int main()
{
    {
        std::cout << "constructor with no managed object\n";
        std::shared_ptr<Foo> sh1; // 空的 shared_ptr
    }

    {
        std::cout << "constructor with object\n";
        std::shared_ptr<Foo> sh2(new Foo);
        std::shared_ptr<Foo> sh3(sh2);
        std::cout << sh2.use_count() << '\n';
        std::cout << sh3.use_count() << '\n';
    }

    {
        std::cout << "constructor with object and deleter\n";
        std::shared_ptr<Foo> sh4(new Foo, D());
        std::shared_ptr<Foo> sh5(new Foo, [](auto p) {
           std::cout << "Call delete from lambda...\n";
           delete p;
        });
    }
}
/*
constructor with no managed object

constructor with object
Foo...
2
2
~Foo...

constructor with object and deleter
Foo...
Foo...
Call delete from lambda...
~Foo...
Call delete from function object...
~Foo..
*/
```

例子4：线程安全

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

void thr(std::shared_ptr<Base> p)  // 传值
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

    std::thread t1(thr, p), t2(thr, p), t3(thr, p); // 传值

    p.reset(); // release ownership from main
    std::cout << "Shared ownership between 3 threads and released\n"
              << "ownership from main:\n"
              << "  p.get() = " << p.get()
              << ", p.use_count() = " << p.use_count() << '\n';
    t1.join(); t2.join(); t3.join();
    std::cout << "All threads completed, the last one deleted Derived\n";
}
```

g++ -std=c++11 -lpthread test.cc

```
$./a.out
  Base::Base()
  Derived::Derived()
Created a shared Derived (as a pointer to Base)
  p.get() = 0x170e028, p.use_count() = 1
Shared ownership between 3 threads and released
ownership from main:
  p.get() = 0, p.use_count() = 0
local pointer in a thread:
  lp.get() = 0x170e028, lp.use_count() = 5
local pointer in a thread:
  lp.get() = 0x170e028, lp.use_count() = 3
local pointer in a thread:
  lp.get() = 0x170e028, lp.use_count() = 2
  Derived::~Derived()
  Base::~Base()
All threads completed, the last one deleted Derived
```


例子5：特殊用法

A shared_ptr can share ownership of an object while storing a pointer to another object. This feature can be used to point to member objects while owning the object they belong to.

``` cpp
template< class Y >
shared_ptr( const shared_ptr<Y>& r, element_type* ptr ) noexcept;
```

**The aliasing constructor:** constructs a shared_ptr which shares ownership information with the initial value of `r`, but holds an unrelated and unmanaged pointer `ptr`. If this shared_ptr is the last of the group to go out of scope, it will call the stored deleter for the object originally managed by `r`. However, calling `get()` on this shared_ptr will always return a copy of `ptr`. It is the responsibility of the programmer to make sure that this ptr remains valid as long as this shared_ptr exists, such as in the typical use cases where ptr is a member of the object managed by `r` or is an alias (e.g., downcast) of `r.get()`.

[另一个解释](https://www.cplusplus.com/reference/memory/shared_ptr/shared_ptr/)：

``` cpp
template <class U> shared_ptr (const shared_ptr<U>& x, element_type* p) noexcept;
```

The object does not own `p`, and will not manage its storage. Instead, it co-owns x's managed object and counts as one additional use of `x`. It will also delete x's pointer on release (and not `p`). It can be used to point to members of objects that are already managed.

新构造的shared_ptr对象，假设为`y`，与构造参数shared_ptr对象`x`共享`x`所管理的指针资源的计数，当引用计数为0时，会自动释放`x`关联的指针，而非`element_type* p`。通常用法是，`y`指向的是`x`关联对象的成员`p`，`y`不负责对`p`的内存释放。

测试代码：

``` cpp
#include <iostream>
#include <memory>

class Foo
{
public:
    Foo(int a) : m_a(a) { std::cout << "Foo(" << a << ")\n"; }
    ~Foo() { std::cout << "~Foo(" << m_a << ")\n"; }
    int Get() { return m_a; }
    void Set(int a) { m_a = a; }

public:
    int m_a;
};

int main()
{
    std::shared_ptr<Foo> foo = std::make_shared<Foo>(100);
    std::cout << "foo: " << foo.use_count() << '\n';

    std::shared_ptr<int> m(foo, &(foo.get()->m_a) );
    std::cout << *m.get() << std::endl;
    auto p_m = m.get();
    *p_m = 200;
    std::cout << foo->Get() << std::endl;
    std::cout << *m.get() << std::endl;
    std::cout << "foo: " << foo.use_count() << '\n';
    foo.reset();
    std::cout << "foo: " << foo.use_count() << '\n';
    std::cout << "m: " << m.use_count() << '\n';
}
/*
Foo(100)
foo: 1
100
200
200
foo: 2
foo: 0
m: 1
~Foo(200)
*/
```

``` cpp
#include <iostream>
#include <memory>

class Bar
{
public:
    Bar() { std::cout << "Bar()\n"; }
    Bar(int *p) : m_a_ref(p) { std::cout << "Bar()\n"; }
    ~Bar() { std::cout << "~Bar()\n"; }
public:
    int* m_a_ref;
};

class Foo
{
public:
    Foo(int a) : m_a(a) { std::cout << "Foo(" << a << ")\n"; }
    ~Foo() { std::cout << "~Foo(" << m_a << ")\n"; }
    int Get() { return m_a; }
    void Set(int a) { m_a = a; }

public:
    int m_a;
};

int main()
{
    std::shared_ptr<Foo> foo = std::make_shared<Foo>(100);
    std::cout << "foo: " << foo.use_count() << '\n';

    std::shared_ptr<Bar> bar(foo, new Bar(&(foo.get()->m_a)) ); // memory leak !
    std::cout << "foo: " << foo.use_count() << '\n';
    foo.reset();
    std::cout << "foo: " << foo.use_count() << '\n';
    std::cout << "bar: " << bar.use_count() << '\n';
    std::cout << *bar.get()->m_a_ref << std::endl;

}
/*
Foo(100)
foo: 1
Bar()
foo: 2
foo: 0
bar: 1
100
~Foo(100)
*/
```


## std::unique_ptr

参考：https://en.cppreference.com/w/cpp/memory/unique_ptr

例子1: 构造初始化

``` cpp
#include <iostream>
#include <memory>

struct Foo { // object to manage
    Foo() { std::cout << "Foo ctor\n"; }
    Foo(const Foo&) { std::cout << "Foo copy ctor\n"; }
    Foo(Foo&&) { std::cout << "Foo move ctor\n"; }
    ~Foo() { std::cout << "~Foo dtor\n"; }
};

struct D { // deleter
    D() {};
    D(const D&) { std::cout << "D copy ctor\n"; }
    D(D&) { std::cout << "D non-const copy ctor\n";}
    D(D&&) { std::cout << "D move ctor \n"; }
    void operator()(Foo* p) const {
        std::cout << "D is deleting a Foo\n";
        delete p;
    };
};

int main()
{
    std::cout << "Example constructor(1)...\n";
    std::unique_ptr<Foo> up1;  // up1 is empty
    std::unique_ptr<Foo> up1b(nullptr);  // up1b is empty

    std::cout << "Example constructor(2)...\n";
    {
        std::unique_ptr<Foo> up2(new Foo); //up2 now owns a Foo
    } // Foo deleted

    std::cout << "Example constructor(3)...\n";
    D d;
    {  // deleter type is not a reference
       std::unique_ptr<Foo, D> up3(new Foo, d); // deleter copied
    }
    {  // deleter type is a reference
       std::unique_ptr<Foo, D&> up3b(new Foo, d); // up3b holds a reference to d
    }

    std::cout << "Example constructor(4)...\n";
    {  // deleter is not a reference
       std::unique_ptr<Foo, D> up4(new Foo, D()); // deleter moved
    }

    std::cout << "Example constructor(5)...\n";
    {
       std::unique_ptr<Foo> up5a(new Foo);
       std::unique_ptr<Foo> up5b(std::move(up5a)); // ownership transfer
    }

    std::cout << "Example constructor(6)...\n";
    {
        std::unique_ptr<Foo, D> up6a(new Foo, d); // D is copied
        std::unique_ptr<Foo, D> up6b(std::move(up6a)); // D is moved

        std::unique_ptr<Foo, D&> up6c(new Foo, d); // D is a reference
        std::unique_ptr<Foo, D> up6d(std::move(up6c)); // D is copied
    }

#if (__cplusplus < 201703L)
    std::cout << "Example constructor(7)...\n";
    {
        std::auto_ptr<Foo> up7a(new Foo);
        std::unique_ptr<Foo> up7b(std::move(up7a)); // ownership transfer
    }
#endif

    std::cout << "Example array constructor...\n";
    {
        std::unique_ptr<Foo[]> up(new Foo[3]);
    } // three Foo objects deleted
}
```

输出结果：

```
Example constructor(1)...
Example constructor(2)...
Foo ctor
~Foo dtor
Example constructor(3)...
Foo ctor
D copy ctor
D is deleting a Foo
~Foo dtor
Foo ctor
D is deleting a Foo
~Foo dtor
Example constructor(4)...
Foo ctor
D move ctor
D is deleting a Foo
~Foo dtor
Example constructor(5)...
Foo ctor
~Foo dtor
Example constructor(6)...
Foo ctor
D copy ctor
D move ctor
Foo ctor
D non-const copy ctor
D is deleting a Foo
~Foo dtor
D is deleting a Foo
~Foo dtor
Example constructor(7)...
Foo ctor
~Foo dtor
Example array constructor...
Foo ctor
Foo ctor
Foo ctor
~Foo dtor
~Foo dtor
~Foo dtor
```

例子2: 基本用法

``` cpp
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

// helper class for runtime polymorphism demo below
struct B
{
    virtual ~B() = default;

    virtual void bar() { std::cout << "B::bar\n"; }
};

struct D : B
{
    D() { std::cout << "D::D\n"; }
    ~D() { std::cout << "D::~D\n"; }

    void bar() override { std::cout << "D::bar\n"; }
};

// a function consuming a unique_ptr can take it by value or by rvalue reference
std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
{
    p->bar();
    return p;
}

// helper function for the custom deleter demo below
void close_file(std::FILE* fp)
{
    std::fclose(fp);
}

// unique_ptr-based linked list demo
struct List
{
    struct Node
    {
        int data;
        std::unique_ptr<Node> next;
    };

    std::unique_ptr<Node> head;

    ~List()
    {
        // destroy list nodes sequentially in a loop, the default destructor
        // would have invoked its `next`'s destructor recursively, which would
        // cause stack overflow for sufficiently large lists.
        while (head)
            head = std::move(head->next);
    }

    void push(int data)
    {
        head = std::unique_ptr<Node>(new Node{data, std::move(head)});
    }
};

int main()
{
    std::cout << "1) Unique ownership semantics demo\n";
    {
        // Create a (uniquely owned) resource
        std::unique_ptr<D> p = std::make_unique<D>();   // C++14

        // Transfer ownership to `pass_through`,
        // which in turn transfers ownership back through the return value
        std::unique_ptr<D> q = pass_through(std::move(p));

        // `p` is now in a moved-from 'empty' state, equal to `nullptr`
        assert(!p);
    }

    std::cout << "\n" "2) Runtime polymorphism demo\n";
    {
        // Create a derived resource and point to it via base type
        std::unique_ptr<B> p = std::make_unique<D>();

        // Dynamic dispatch works as expected
        p->bar();
    }

    std::cout << "\n" "3) Custom deleter demo\n";
    std::ofstream("demo.txt") << 'x'; // prepare the file to read
    {
        using unique_file_t = std::unique_ptr<std::FILE, decltype(&close_file)>;
        unique_file_t fp(std::fopen("demo.txt", "r"), &close_file);
        if (fp)
            std::cout << char(std::fgetc(fp.get())) << '\n';
    } // `close_file()` called here (if `fp` is not null)

    std::cout << "\n" "4) Custom lambda-expression deleter and exception safety demo\n";
    try
    {
        std::unique_ptr<D, void(*)(D*)> p(new D, [](D* ptr)
        {
            std::cout << "destroying from a custom deleter...\n";
            delete ptr;
        });

        throw std::runtime_error(""); // `p` would leak here if it were instead a plain pointer
    }
    catch (const std::exception&) { std::cout << "Caught exception\n"; }

    std::cout << "\n" "5) Array form of unique_ptr demo\n";
    {
        std::unique_ptr<D[]> p(new D[3]);
    } // `D::~D()` is called 3 times

    std::cout << "\n" "6) Linked list demo\n";
    {
        List wall;
        for (int beer = 0; beer != 1'000'000; ++beer)   // C++14
            wall.push(beer);

        std::cout << "1'000'000 bottles of beer on the wall...\n";
    } // destroys all the beers
}
```

输出结果：

```
1) Unique ownership semantics demo
D::D
D::bar
D::~D

2) Runtime polymorphism demo
D::D
D::bar
D::~D

3) Custom deleter demo
x

4) Custom lambda-expression deleter and exception safety demo
D::D
destroying from a custom deleter...
D::~D
Caught exception

5) Array form of unique_ptr demo
D::D
D::D
D::D
D::~D
D::~D
D::~D

6) Linked list demo
1'000'000 bottles of beer on the wall...
```


## std::weak_ptr

参考：https://en.cppreference.com/w/cpp/memory/weak_ptr


``` cpp
#include <iostream>
#include <memory>

std::weak_ptr<int> gw;

void observe()
{
    std::cout << "use_count == " << gw.use_count() << ": ";

    // lock: creates a shared_ptr that manages the referenced object
    if (auto spt = gw.lock()) { // Has to be copied into a shared_ptr before usage
        std::cout << *spt << "\n";
    }
    else {
        std::cout << "gw is expired\n";
    }
}

int main()
{
  {
    auto sp = std::make_shared<int>(42);
    gw = sp;
    observe();
  }
  observe();
}
```

输出结果：

```
use_count == 1: 42
use_count == 0: gw is expired
```

# Tips

## std::shared_ptr 循环引用问题

循环引用是 `std::shared_ptr` 的一个常见问题，当两个或更多的 `std::shared_ptr` 对象相互引用时，就会形成一个循环，导致内存泄漏。A 和 B 互相持有对方的 `std::shared_ptr`，形成了一个循环引用。当 main 函数中的作用域结束时，a 和 b 的析构函数不会被调用，因为它们的引用计数不为零，这就导致了内存泄漏。

要检测 `std::shared_ptr` 的循环引用，可以使用内存分析工具，如 Valgrind 或 AddressSanitizer。但是，最好的方法是设计良好的程序结构以避免循环引用。例如，可以使用 std::weak_ptr 来打破循环。`std::weak_ptr` 是一种弱引用，不会增加引用计数，因此不会导致循环引用。在上面的例子中，可以将 B 中的 `std::shared_ptr<A>` 替换为 `std::weak_ptr<A>`，这样就可以避免循环引用。

``` cpp
#include <iostream>
#include <memory>

struct B;

struct A {
    std::shared_ptr<B> b_ptr;
    ~A() { std::cout << "A destructor called!\n"; }
};

struct B {
    std::shared_ptr<A> a_ptr;
    ~B() { std::cout << "B destructor called!\n"; }
};

int main() {
    {
        std::shared_ptr<A> a = std::make_shared<A>();
        std::shared_ptr<B> b = std::make_shared<B>();
        a->b_ptr = b;
        b->a_ptr = a;
    }  // a 和 b 的析构函数在这里不会被调用，因为存在循环引用

    return 0;
}
```

## 自定义 custom_deleter

``` cpp
#include <iostream>
#include <memory>
#include <set>

std::set<void*> g_objects;

template<typename T>
void custom_deleter(T* ptr) {
    g_objects.insert(ptr);
    std::cout << "Object " << ptr << " created." << std::endl;
    delete ptr;
    g_objects.erase(ptr);
    std::cout << "Object " << ptr << " destroyed." << std::endl;
}

int main() {
    {
        std::shared_ptr<int> p1(new int(42), custom_deleter<int>);
        std::shared_ptr<int> p2(new int(100), custom_deleter<int>);
        std::shared_ptr<int> p3(p2);
        std::cout << "p1.use_count() = " << p1.use_count() << std::endl;
        std::cout << "p2.use_count() = " << p2.use_count() << std::endl;
        std::cout << "p3.use_count() = " << p3.use_count() << std::endl;
    }
    std::cout << "Objects still held: " << g_objects.size() << std::endl;
    for (auto obj : g_objects) {
        std::cout << "Object " << obj << " still held." << std::endl;
    }
    return 0;
}
```

输出：

```
p1.use_count() = 1
p2.use_count() = 2
p3.use_count() = 2
Object 0xb21f00 created.
Object 0xb21f00 destroyed.
Object 0xb21eb0 created.
Object 0xb21eb0 destroyed.
Objects still held: 0
```

## 检查 shared_ptr 对象是否被释放

使用 `std::weak_ptr`，它是一种弱引用智能指针，可以用来检查 `std::shared_ptr` 对象是否已经被释放。参考：https://en.cppreference.com/w/cpp/memory/weak_ptr/lock

``` cpp
#include <iostream>
#include <memory>

void observe(std::weak_ptr<int> weak)
{
    if (auto observe = weak.lock()) {
        std::cout << "\tobserve() able to lock weak_ptr<>, value=" << *observe << "\n";
    } else {
        std::cout << "\tobserve() unable to lock weak_ptr<>\n";
    }
}

int main()
{
    std::weak_ptr<int> weak;
    std::cout << "weak_ptr<> not yet initialized\n";
    observe(weak);

    {
        auto shared = std::make_shared<int>(42);
        weak = shared;
        std::cout << "weak_ptr<> initialized with shared_ptr.\n";
        observe(weak);
    }

    std::cout << "shared_ptr<> has been destructed due to scope exit.\n";
    observe(weak);
}
```

输出：

```
weak_ptr<> not yet initialized
        observe() unable to lock weak_ptr<>
weak_ptr<> initialized with shared_ptr.
        observe() able to lock weak_ptr<>, value=42
shared_ptr<> has been destructed due to scope exit.
        observe() unable to lock weak_ptr<>
```


``` cpp
#include <iostream>
#include <memory>
#include <vector>

std::vector<std::weak_ptr<int>> g_weak_objects;

void check_objects() {
    std::cout << "Checking objects..." << std::endl;
    for (auto obj : g_weak_objects) {
        if (auto p = obj.lock()) {
            std::cout << "Object " << *p << " still held." << std::endl;
        } else {
            std::cout << "Object has been released." << std::endl;
        }
    }
}

int main() {
    {
        auto p1 = std::make_shared<int>(42);
        auto p2 = std::make_shared<int>(100);
        auto p3 = p2;

        g_weak_objects.emplace_back(p1);
        g_weak_objects.emplace_back(p2);
        g_weak_objects.emplace_back(p3);

        std::cout << "p1.use_count() = " << p1.use_count() << std::endl;
        std::cout << "p2.use_count() = " << p2.use_count() << std::endl;
        std::cout << "p3.use_count() = " << p3.use_count() << std::endl;
    }

    check_objects();

    return 0;
}
```

输出：

```
p1.use_count() = 1
p2.use_count() = 2
p3.use_count() = 2
Checking objects...
Object has been released.
Object has been released.
Object has been released.
```



# Reference

* [Technical Report on C++ Performance](http://www.open-std.org/jtc1/sc22/wg21/docs/TR18015.pdf)
* [The Real Price of Shared Pointers in C++ - Nico Josuttis](https://vimeo.com/131189627)
* [CppCon 2019: Chandler Carruth “There Are No Zero-cost Abstractions](https://www.youtube.com/watch?v=rHIkrotSwcc)
* [CppCon 2019: Chandler Carruth “There Are No Zero-cost Abstractions, unique_ptr 部分](https://youtu.be/rHIkrotSwcc?t=1063)





