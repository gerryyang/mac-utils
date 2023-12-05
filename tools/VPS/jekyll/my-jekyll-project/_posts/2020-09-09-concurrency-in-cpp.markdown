---
layout: post
title:  "Concurrency in CPP"
date:   2020-09-09 08:00:00 +0800
categories: [C/C++, Concurrency]
---

* Do not remove this line (it will not be displayed)
{: toc}

# 相关理论

## 锁的代价

当很多线程争抢同一把锁时，一些线程无法立刻获得锁，而必须睡眠直到某个线程退出**临界区**。这个争抢过程称之为`contention`。在多核机器上，当多个线程需要操作同一个资源却被一把锁挡住时，便无法充分发挥多个核心的并发能力。**现代 OS 通过提供比锁更底层的同步原语，使得无竞争锁完全不需要系统调用**，只是一两条`wait-free`，耗时`10-20ns`的原子操作，非常快。而锁一旦发生竞争，一些线程就要陷入睡眠，再次醒来触发了 OS 的调度代码，代价至少为`3-5us`。所以让锁尽量无竞争，让所有线程“一起飞”是需要高性能的 server 的永恒话题。

在多线程场景中**消除竞争**的常见方案：

* 如果临界区非常小，竞争又不是很激烈，优先选择使用`mutex`, 之后可以结合`contention profiler`来判断`mutex`是否成为瓶颈。
* 需要有序执行，或者无法消除的激烈竞争但是可以通过批量执行来提高吞吐，可以选择使用`Message passing`。

多线程编程没有万能的模型，需要根据具体的场景，结合丰富的`profliling`工具，最终在复杂度和性能之间找到合适的平衡。特别指出一点，Linux 中`mutex`无竞争的`lock/unlock`只有需要几条原子指令，在绝大多数场景下的开销都可以忽略不计.

## fiber 与 coroutine 的区别

> TL;DR
> fiber 本质上是一个调度实体（实现方式：每个 fiber 拥有各自独有的运行时栈），在 fiber 上可以执行普通函数，或协程。
>
> coroutie 本质是一个函数（实现方式：C++20 基于宏，大多数基于切换运行时栈），可能是 stackful（需要切换栈）的，或者 stackless（如 C++20 的 Coroutines）。

[fiber](https://en.wikipedia.org/wiki/Fiber_%28computer_science%29)是一种轻量的线程，也常被称为“纤程”、“绿色线程”等。其作为一个调度实体接收运行时的调度。为方便使用，也提供了用于 fiber 的 Mutex、ConditionVariable、this_fiber::、fiber 局部存储等基础设施以供使用。使用 fiber 编程时思想与使用 pthread 编程相同，均是使用传统的普通函数（这与 coroutine 形成对比）编写同步代码，并由运行时/操作系统负责在 fiber/pthread 阻塞时进行调度。

> In computer science, a fiber is a particularly lightweight thread of execution.
>
> Like threads, fibers share address space. However, fibers use cooperative multitasking while threads use preemptive multitasking. Threads often depend on the kernel's thread scheduler to preempt a busy thread and resume another thread; fibers yield themselves to run another fiber while executing.

[coroutine](https://en.wikipedia.org/wiki/Coroutine)是一种可以被挂起、恢复（多进多出）的函数（“subroutine”）。其本身是一种被泛化了的函数。由于协程本质上依然是一个函数，因此其不涉及调度、锁、条件变量、局部存储等问题。

> Coroutines are computer program components that generalize subroutines for non-preemptive multitasking, by allowing execution to be suspended and resumed. Coroutines are well-suited for implementing familiar program components such as cooperative tasks, exceptions, event loops, iterators, infinite lists and pipes.

**coroutine 可能存在的问题：**

* 取决于协程库的实现，大多数协程库中单个协程阻塞会导致对应的 pthread 关联的所有的协程的运行被延迟，造成响应时间毛刺。（此问题可通过 Hook 得到优化？比如，libco 提供的 Hook 能力）
* 除 asio 外常见的支持用户态调度的 RPC 框架面对用户的最终形态均是单纯的栈切换而没有体现出协程自身独到的能力（多入多出等）
* 协程的学习成本（区分 stackful vs stackless，理解多入多出）对于业务开发的同学更高
* 基于用户态栈切换实现的协程和 C++20 的协程作为完全不同的两种实现，易于混淆


## Message passing (消息队列)

在多核并发编程领域， [Message passing](https://en.wikipedia.org/wiki/Message_passing) 作为一种解决竞争的手段得到了比较广泛的应用，它按照业务依赖的资源将逻辑拆分成若干个独立 actor，每个 actor 负责对应资源的维护工作，当一个流程需要修改某个资源的时候， 就转化为一个消息发送给对应 actor，这个 actor (通常在另外的上下文中)根据命令内容对这个资源进行相应的修改，之后可以选择唤醒调用者(同步)或者提交到下一个 actor (异步)的方式进行后续处理。

## 如何防止 worker 阻塞

* 动态增加 worker 数

但实际未必如意，当大量的 worker 同时被阻塞时，它们很可能在等待同一个资源(比如同一把锁)，增加 worker 可能只是增加了更多的等待者。

* 区分 io 线程和 worker 线程

worker 线程调用用户逻辑，即使 worker 线程全部阻塞也不会影响 io 线程。但增加一层处理环节(io 线程)并不能缓解拥塞，如果 worker 线程全部卡住，程序仍然会卡住，只是卡的地方从 socket 缓冲转移到了 io 线程和 worker 线程之间的消息队列。换句话说, 在 worker 卡住时，还在运行的 io 线程做的可能是无用功。另一个问题是每个请求都要从 io 线程跳转至 worker 线程，增加了一次上下文切换，在机器繁忙时，切换都有一定概率无法被及时调度，会导致更多的延时长尾。

* 限制最大并发

只要同时被处理的请求数低于 worker 数，自然可以规避掉 "所有 worker 被阻塞" 的情况。


# 无锁编程

利用计算机多核体系架构实现程序并行运行能够显著提高并发与数据吞吐，异步编程提升效率的同时也带来数据不一致的矛盾。使用**线程锁**能够保证资源访问的**互斥性**，但是会**显著降低并发**。如何保证数据一致性的前提下提高并发度？


参考 https://www.1024cores.net/home/lock-free-algorithms/introduction

I bet you had heard terms like "lockfree" and "waitfree". So what it's all about? Let's start with some definitions.


Wait-freedom

Wait-freedom means that each thread moves forward regardless of external factors like contention from other threads, other thread blocking. Each operations is executed in a bounded number of steps. It's the strongest guarantee for synchronization algorithms. Wait-free algorithms usually use such primitives as atomic_exchange, atomic_fetch_add (InterlockedExchange, InterlockedIncrement, InterlockedExchangeAdd, __sync_fetch_and_add), and they do not contain cycles that can be affected by other threads. atomic_compare_exchange primitive (InterlockedCompareExchange, __sync_val_compare_and_swap) is usually not used, because it is usually tied with a "repeat until succeed" cycle.

Below is an example of a wait-free algorithm:

``` cpp
void increment_reference_counter(rc_base* obj)
{
    atomic_increment(obj->rc);
}

void decrement_reference_counter(rc_base* obj)
{
    if (0 == atomic_decrement(obj->rc)) delete obj;
}
```

Each thread is able to execute the function in a bounded number of steps regardless of any external factors.

Lock-freedom

Lock-freedom means that a system as a whole moves forward regardless of anything. Forward progress for each individual thread is not guaranteed (that is, individual threads can starve). It's a weaker guarantee than wait-freedom. Lockfree algorithms usually use atomic_compare_exchange primitive (InterlockedCompareExchange, __sync_val_compare_and_swap).

An example of a lockfree algorithm is:

``` cpp
void stack_push(stack* s, node* n)
{
    node* head;
    do
    {
        head = s->head;
        n->next = head;
    }
    while ( ! atomic_compare_exchange(s->head, head, n));
}
```

As can be seen, a thread can "whirl" in the cycle theoretically infinitely. But every repeat of the cycle means that some other thread had made forward progress (that is, successfully pushed a node to the stack). A blocked/interrupted/terminated thread can not prevent forward progress of other threads. Consequently, the system as a whole undoubtedly makes forward progress.




## Lock-free 编程

Lock-free 编程指利用一组特定的**原子操作**来控制多个线程对于同一数据的并发访问。相比于基于锁的算法而言，Lock-free 算法具有明显的特征：**某个线程在执行数据访问时挂起不会阻碍其他的线程继续执行。这意味着在任意时刻，多个 lock-free 线程可以同时访问同一数据而不产生数据竞争或者损坏。**

### 原子操作

**原子操作**是指不会被线程调度机制打断的操作；这种操作一旦开始，就一直运行到结束，中间不会有任何 context switch。原子性不可能由软件单独保证，必须要有硬件的支持，是和计算机体系架构相关的。在x86 平台上，CPU 提供了在指令执行期间对总线加锁的手段，由于加锁由硬件完成，实际开销可以忽略。

典型实现：

``` cpp
std::atomic_**
```


### CAS2 （128bit compare-and-swap）

通过将内存中的值与指定数据按位进行比较，当数值一样时将内存中的数据替换为新的值。现代 CPU 广泛支持的 CPU 指令级的操作，只有一步原子操作，所以非常快。而且 CAS 避免了请求操作系统来裁定锁的问题，不用麻烦操作系统，直接在 CPU 内部就搞定。

典型实现：

``` cpp
std::atomic<T>::compare_exchange_weak
std::atomic<T>::compare_exchange_strong
```

## 无锁队列

多核心优化是现在游戏开发的一个重点课题，无论是工程实践也好，研究算法也罢，将工作并行化交由多线程去做是一个非常普遍的场景。对于这种场景，我们通常会采用**线程池+消息队列**的方式去实现，其中的消息队列就会使用**互斥锁**或是**无锁队列**。并且由于消息队列的读写是比较频繁的操作，采用**无锁队列**的性能要明显高于有锁队列。

### Wait-free 型无锁队列

特点：

* 仅支持**单一读/写**线程模型的生产-消费场景
* 操作读写指针不需要循环调用 CAS 等待其他线程操作结束，
* 算法复杂度 `O(1)`

典型实现：

* `boost::spsc_queue`
* 使用 ring-buffer 存储结构化类型的数据 T，利用拷贝构造的方式写入数据
* pop 操作对等使用拷贝方式传出，支持传入 functor 直接消费数据，要求 functor 参数为值类型的 T
* 队列判空：`write_pos == read_pos`
* 队列判满：`write_pos + 1 == read_pos`
* 使用建议：存入队列的类型 Ｔ 是 shared_ptr 等指针类型时能有效降低拷贝开销


### Lock-free 型无锁队列

特点：

* 支持**多个读/写**线程同时访问
* 依赖 CAS 操作读/写指针，某个线程会被其他线程的行为所影响，甚至被长时间阻塞
* 算法复杂度 `O(n)`

典型实现：

* `boost::lockfree::queue`
* [Bounded MPMC queue](https://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue)


## Hazard Pointer

**无锁队列**利用**线性存储结构**就能够满足多线程场景下的数据共享需求，但是**线性存储结构**相比 map、hash_table 等面向**查找的数据结构**在性能上具有明显劣势，

在读操作流程包含 find 的场景下需要使用 **RCU** 思想降低多线程互斥，提高程序并发效率。

[RCU](https://zhuanlan.zhihu.com/p/30583695) (**Read-Copy Update**)，是 Linux 中比较重要的一种同步机制。顾名思义就是 “读，拷贝更新”，再直白点是 “随意读，但更新数据的时候，需要先复制一份副本，在副本上完成修改，再一次性地替换旧数据”。

这是 Linux 内核实现的一种针对 “读多写少” 的共享数据的同步机制。

读多写少：典型的生产消费模型下读/写可以认为是 1:1，但是包含查找的场景下读开销会迅速放大，形如 read(key)，write_unique(key, val) 的流程会包含大量的读操作。


* [C++ Lock-free Hazard Pointer](https://sf-zhou.github.io/programming/hazard_pointer.html)








## [Producer-Consumer Queues](https://www.1024cores.net/home/lock-free-algorithms/queues)

Producer-consumer queues are one of the most fundamental components in concurrent systems, they represent means to transfer data/messages/tasks/transactions between threads/stages/agents. If you hope that there is a single magical "one-size-fits-all" concurrent queue (MS PPL and Intel TTB fallacy), sorry, there is no. So what flavours of queues are there?

**Depending on allowed number of producer and consumer threads**:

* Multi-producer/multi-consumer queues (**MPMC**)
* Single-producer/multi-consumer queues (**SPMC**)
* Multi-producer/single-consumer queues (**MPSC**)
* Single-producer/single-consumer queues (**SPSC**)

I hope this aspect is clear - for example, if you have only 1 producer and 1 consumer thread, you can use **SPSC** queue instead of more general **MPMC** queue, and as you may guess it will be significantly faster.

**Depending on underlying data structure**:

* Array-based
* Linked-list-based
* Hybrid

Array-based queues are generally faster, however they are usually not strictly [lockfree](https://www.1024cores.net/home/lock-free-algorithms/introduction). The drawback is that they need to preallocate memory for the worst case. Linked-list queues grow dynamically, thus no need to preallocate any memory up-front. And hybrid queues (linked-list of small fixed-size arrays) try to combine advantages of both.





# C++ Memory Model

* The memory available to a C++ program is one or more contiguous sequences of bytes. Each byte in memory has a unique address.
* A memory location is:
  + an object of scalar type (arithmetic type, pointer type, enumeration type, or std::nullptr_t)
  + or the largest contiguous sequence of bit fields of non-zero length

``` cpp
struct S {
    char a;     // memory location #1
    int b : 5;  // memory location #2
    int c : 11, // memory location #2 (continued)
          : 0,
        d : 8;  // memory location #3
    struct {
        int ee : 8; // memory location #4
    } e;
} obj; // The object 'obj' consists of 4 separate memory locations
```

* Different threads of execution are always allowed to access (read and modify) different memory locations concurrently, with no interference and no synchronization requirements. When an evaluation of an expression writes to a memory location and another evaluation reads or modifies the same memory location, the expressions are said to conflict. A program that has two conflicting evaluations has a data race unless:
  + both evaluations execute on the same thread or in the same signal handler, or
  + both conflicting evaluations are atomic operations (see std::atomic), or
  + one of the conflicting evaluations happens-before another (see std::memory_order)


refer: https://en.cppreference.com/w/cpp/language/memory_model


# ThreadSanitizerCppManual

`ThreadSanitizer` (aka `TSan`) is **a data race detector for C/C++**. Data races are one of the most common and hardest to debug types of bugs in concurrent systems. A data race occurs when two threads access the same variable concurrently and at least one of the accesses is write. [C++11](https://en.wikipedia.org/wiki/C%2B%2B11) standard officially bans data races as **undefined behavior**.

Here is an example of a data race that can lead to crashes and memory corruptions:

``` cpp
#include <pthread.h>
#include <stdio.h>
#include <string>
#include <map>

typedef std::map<std::string, std::string> map_t;

void *threadfunc(void *p) {
  map_t& m = *(map_t*)p;
  m["foo"] = "bar";
  return 0;
}

int main() {
  map_t m;
  pthread_t t;
  pthread_create(&t, 0, threadfunc, &m);
  printf("foo=%s\n", m["foo"].c_str());
  pthread_join(t, 0);
}
```

There are a lot of various ways to trigger a data race in C++, see [ThreadSanitizerPopularDataRaces](https://github.com/google/sanitizers/wiki/ThreadSanitizerPopularDataRaces), TSan detects all of them and more -- [ThreadSanitizerDetectableBugs](https://github.com/google/sanitizers/wiki/ThreadSanitizerDetectableBugs).

ThreadSanitizer is part of `clang 3.2` and `gcc 4.8`. To build the freshest version see [ThreadSanitizerDevelopment](https://github.com/google/sanitizers/wiki/ThreadSanitizerDevelopment) page.


## Usage

Simply compile your program with `-fsanitize=thread` and link it with `-fsanitize=thread`. To get a reasonable performance add `-O2`. Use `-g` to get file names and line numbers in the warning messages.

When you run the program, `TSan` will print a report if it finds a data race. Here is an example:

``` cpp
$ cat simple_race.cc
#include <pthread.h>
#include <stdio.h>

int Global;

void *Thread1(void *x) {
  Global++;
  return NULL;
}

void *Thread2(void *x) {
  Global--;
  return NULL;
}

int main() {
  pthread_t t[2];
  pthread_create(&t[0], NULL, Thread1, NULL);
  pthread_create(&t[1], NULL, Thread2, NULL);
  pthread_join(t[0], NULL);
  pthread_join(t[1], NULL);
}
```

```
$ clang++ simple_race.cc -fsanitize=thread -fPIE -pie -g
$ ./a.out
==================
WARNING: ThreadSanitizer: data race (pid=26327)
  Write of size 4 at 0x7f89554701d0 by thread T1:
    #0 Thread1(void*) simple_race.cc:8 (exe+0x000000006e66)

  Previous write of size 4 at 0x7f89554701d0 by thread T2:
    #0 Thread2(void*) simple_race.cc:13 (exe+0x000000006ed6)

  Thread T1 (tid=26328, running) created at:
    #0 pthread_create tsan_interceptors.cc:683 (exe+0x00000001108b)
    #1 main simple_race.cc:19 (exe+0x000000006f39)

  Thread T2 (tid=26329, running) created at:
    #0 pthread_create tsan_interceptors.cc:683 (exe+0x00000001108b)
    #1 main simple_race.cc:20 (exe+0x000000006f63)
==================
ThreadSanitizer: reported 1 warnings
```

Refer to [ThreadSanitizerReportFormat](https://github.com/google/sanitizers/wiki/ThreadSanitizerReportFormat) for explanation of reports format.

There is a bunch of runtime and compiler flags to tune behavior of TSan -- see [ThreadSanitizerFlags](https://github.com/google/sanitizers/wiki/ThreadSanitizerFlags).


refer: https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual

# Contention Profiler (brpc)

可以分析花在等待锁上的时间及发生等待的函数。

brpc支持contention profiler，可以分析在等待锁上花费了多少时间。等待过程中线程是睡着的不会占用CPU，所以contention profiler中的时间并不是cpu时间，也不会出现在cpu profiler中。cpu profiler可以抓到特别繁忙的操作（花费了很多cpu），但耗时真正巨大的临界区往往不是那么繁忙，而无法被cpu profiler发现。**contention profiler和cpu profiler好似互补关系，前者分析等待时间（被动），后者分析忙碌时间。** 还有一类由用户基于condition或sleep发起的主动等待时间，无需分析。

refer: https://github.com/apache/incubator-brpc/blob/master/docs/cn/contention_profiler.md


# 问题代码

非线程安全（race condition）：

``` cpp
#include <iostream>
#include <thread>
#include <vector>

int main()
{
        int cnt = 0;
        auto f = [&]{cnt++;};

        std::vector<std::thread> thd_vec;
        for (int i = 0; i != 10000; ++i) {
                std::thread t(f);
                thd_vec.emplace_back(std::move(t));
        }

        for (auto &thd : thd_vec) {
                thd.join();
        }

        std::cout << "cnt: " << cnt << std::endl;

        return 0;
}
/*
$ g++ -std=c++11 -lpthread std_thread.cpp
$ for i in {1..10}; do ./a.out; done | sort | uniq -c
      7 cnt: 10000
      3 cnt: 9999
*/
```

stl的map非线程安全：

``` cpp
#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include <ctime>

std::map<int, int> g_m = { {0, 0} };

void f1(int x)
{
    for (;;) {
        auto i = std::rand();
        g_m.emplace(i, i);

        //std::chrono::nanoseconds ns(100 * 1000 * 1000);
        //std::this_thread::sleep_for(ns);
    }
}

void f2(int x)
{
    for (;;) {
        auto iter = g_m.find(0);
        if (iter == g_m.end()) {
                std::cout << "no find\n";
        } else {
                //std::cout << "find\n";
        }
    }
}

int main(int argc, char**argv)
{
    std::srand(std::time(nullptr));

    std::thread thrd1 (f1, 0);
    std::thread thrd2 (f2, 0);

    thrd1.join();
    thrd2.join();

    std::cout << "done\n";
}
```

# Mutex

A mutex (**mut**ual **ex**lusion) allows us to encapsulate blocks of code that should only be executed in one thread at a time.

[std::mutex](https://en.cppreference.com/w/cpp/thread/mutex):

The mutex class is a synchronization primitive that can be used to protect shared data from being simultaneously accessed by multiple threads. mutex offers exclusive, non-recursive ownership semantics:

* A calling thread owns a mutex from the time that it successfully calls either `lock` or `try_lock` until it calls `unlock`.
* When a thread owns a mutex, all other threads will block (for calls to `lock`) or receive a false return value (for `try_lock`) if they attempt to claim ownership of the mutex.
* A calling thread must not own the mutex prior to calling `lock` or `try_lock`.

`std::mutex `通常不会单独使用，而是通过`std::unique_lock`，或`std::lock_guard`，或`std::scoped_lock`等，以封装的形式（mutex wrapper）使用。

The class [lock_guard](https://en.cppreference.com/w/cpp/thread/lock_guard) is a mutex wrapper that provides **a convenient RAII-style mechanism** for owning a mutex for the duration of a scoped block. When a lock_guard object is created, it attempts to take ownership of the mutex it is given. When control leaves the scope in which the lock_guard object was created, the lock_guard is destructed and the mutex is released.


``` cpp
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::mutex g_cnt_mutex;// protect cnt

int main()
{
    int cnt = 0;
    auto f = [&]{
        const std::lock_guard<std::mutex> lock(g_cnt_mutex);
        cnt++;

        // g_cnt_mutex is automatically released when lock goes out of scope
    };

    std::vector<std::thread> thd_vec;
    for (int i = 0; i != 100; ++i) {
        std::thread t(f);
        thd_vec.emplace_back(std::move(t));
    }

    for (auto &thd : thd_vec) {
        thd.join();
    }

    std::cout << "cnt: " << cnt << std::endl;

    return 0;
}
/*
$ for i in {1..1000}; do ./a.out; done | sort | uniq -c
   1000 cnt: 100
*/
```

# Atomic

C++11 提供了一种更好的抽象方式解决这个问题，通过[std::atomic](https://en.cppreference.com/w/cpp/atomic/atomic)模版定义定义操作数为原子类型，从而保证在多线程情况下为原子操作。

``` cpp
#include <iostream>
#include <thread>
#include <vector>

int main()
{
        std::atomic<int> cnt(0);
        auto f = [&]{cnt++;};

        std::vector<std::thread> thd_vec;
        for (int i = 0; i != 10000; ++i) {
                std::thread t(f);
                thd_vec.emplace_back(std::move(t));
        }

        for (auto &thd : thd_vec) {
                thd.join();
        }

        std::cout << "cnt: " << cnt << std::endl;

        return 0;
}
```

# Async

The function template [async](https://en.cppreference.com/w/cpp/thread/async) runs the function `f` asynchronously (potentially in a separate thread which may be part of a thread pool) and returns a `std::future` that will eventually hold the result of that function call.

The `async` construct uses **an object pair** called a `promise` and a `future`. The former has made a promise to eventually provide a value. The future is linked to the promise and can at any time try to retrieve the value by `get()`. **If the promise hasn't been fulfilled yet, it will simply wait until the value is ready.**


非线程安全：

``` cpp
#include <iostream>
#include <vector>
#include <future>

int main()
{
        int cnt = 0;
        auto f = [&]{cnt++;};

        std::vector<decltype(std::async(std::launch::async, f))> handle_vec;
        for (int i = 0; i != 10000; ++i) {
                auto handle = std::async(std::launch::async, f);
                handle_vec.emplace_back(std::move(handle));
        }

#if 0
        for (auto &h : handle_vec) {
                h.get();
        }
#endif

        std::cout << "cnt: " << cnt << std::endl;

        return 0;
}

```

线程安全：若async另起的异步线程没有执行完，`get()`操作会阻塞，因此不会出现并发操作的问题。

``` cpp
#include <iostream>
#include <vector>
#include <future>

int main()
{
        int cnt = 0;
        auto f = [&]{cnt++;};

        for (int i = 0; i != 10000; ++i) {
                // start a new thread to carry out f function
                auto handle = std::async(std::launch::async, f);

                // block thread
                handle.get();
        }

        std::cout << "cnt: " << cnt << std::endl;

        return 0;
}
```

# Condition variables

If we return to threads, it would be useful to be able to have one thread wait for another thread to finish processing something, essentially sending a signal between the threads. This can be done with mutexes, but it would be awkward. It can also be done using a global boolean variable called notified that is set to true when we want to send the signal. The other thread would then run a for loop that checks if notified is true and stops looping when that happens. Since setting notified to true is atomic and in this example we're only setting it once, we don't even need a mutex. However, on the receiving thread we are running a for loop at full speed, wasting a lot of CPU time. We could add a short sleep_for inside the for loop, making the CPU idle most of the time.

First of all, we're using some new syntax from C++11, that enables us to define the thread functions in-place as anynomous functions. They are implicitly passed the local scope, so they can read and write value and notified. If you compile it as it is, it will output 100 most of the time.

多线程没有同步，输出 100：

``` cpp
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <queue>

std::condition_variable cond_var;
std::mutex m;

int main() {
    int value = 100;
    bool notified = false;
    std::thread reporter([&]() {
        /*
        unique_lock<mutex> lock(m);
        while (!notified) {
            cond_var.wait(lock);
        }
        */
        std::cout << "The value is " << value << std::endl;
    });

    std::thread assigner([&]() {

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        value = 20;
        /*
        notified = true;
        cond_var.notify_one();
        */
    });

    reporter.join();
    assigner.join();

    return 0;
}
```

However, we want the reporter thread to wait for the assigner thread to give it the value 20, before outputting it. In the assigner thread, it will set notified to true and send a signal through the condition variable `cond_var`. In the reporter thread, we're looping as long as notified is false, and in each iteration we wait for a signal.

多线程同步，输出20:

``` cpp
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <queue>

std::condition_variable cond_var;
std::mutex m;

int main() {
    int value = 100;
    bool notified = false;
    std::thread reporter([&]() {

        std::unique_lock<std::mutex> lock(m);
        while (!notified) {
            cond_var.wait(lock);
        }

        std::cout << "The value is " << value << std::endl;
    });

    std::thread assigner([&]() {

        value = 20;

        notified = true;
        cond_var.notify_one();

    });

    reporter.join();
    assigner.join();

    return 0;
}
```

But wait, if `cond_var` can send a signal that will make the call `cond_var.wait(lock)` blocking until it receives it, why are we still using notified and a for loop? Well, **that's because the condition variable can be spuriously awaken even if we didn't call `notify_one`, and in those cases we need to fall back to checking notified. This for loop will iterate that many times.**

This is a simplified description since we are also giving wait the object lock, which is associated with a mutex m. What happens is that when wait is called, it not only waits for a notification, but also for the mutex m to be unlocked. When this happens, it will acquire the lock itself. If cond_var has acquired a lock and wait is called again, it will be unlocked as long as it's waiting to acquire it again. This gives us some structure of mutual exclusion between the two threads.

# Producer-consumer problem

错误的例子：

``` cpp
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <queue>
using namespace std;

int main() {
    int c = 0;
    bool done = false;
    queue<int> goods;

    thread producer([&]() {
        for (int i = 0; i < 500; ++i) {
            goods.push(i);
            c++;
        }

        done = true;
    });

    thread consumer([&]() {
        while (!done) {
            while (!goods.empty()) {
                goods.pop();
                c--;
            }
        }
    });

    producer.join();
    consumer.join();
    cout << "Net: " << c << endl;
}
```

# 线程安全检查工具

C++ 中有多种工具可以帮助检查线程安全问题。以下是一些常见的工具：

## ThreadSanitizer（TSan）

安装方法：

```
sudo yum install libtsan
```

ThreadSanitizer 是一个用于检测多线程数据竞争的动态分析工具。它是 Clang 和 GCC 编译器的一部分，可以在编译时启用。要使用 ThreadSanitizer，只需在编译命令中添加 `-fsanitize=thread` 标志。例如：

```
g++ -fsanitize=thread -g -O1 -o my_program my_program.cpp
```

然后运行程序，ThreadSanitizer 会报告潜在的线程安全问题。


ThreadSanitizer (TSan) 是一个用于检测多线程数据竞争的动态分析工具。为了实现这一目标，TSan 会在运行时检查程序的内存访问和同步操作。为了实现这一功能，TSan 需要在程序中插入额外的检查和检测代码。这些检查和检测代码通常作为动态链接的运行时库提供。

当使用动态链接库时，程序需要使用位置无关代码（PIC，Position Independent Code）。位置无关代码允许在运行时将代码加载到任意内存地址，而不需要进行重定位。这使得多个程序可以共享同一个库副本，从而节省内存和磁盘空间。

由于 TSan 需要使用动态链接的运行时库，因此在编译和链接程序时，需要确保所有库和源文件都使用 -fPIC 选项进行编译。这样，程序才能正确地使用 TSan 的运行时库，以检测潜在的线程安全问题。

总之，TSan 依赖于使用 -fPIC，因为它需要动态链接的运行时库，而这些库需要位置无关代码。在编译和链接时确保使用 -fPIC 选项，以确保程序可以正确地使用 TSan。



## Valgrind / Helgrind

安装方法：

```
sudo yum install valgrind
```

Valgrind 是一个用于内存调试、内存泄漏检测和性能分析的工具。Helgrind 是 Valgrind 的一个工具，用于检测多线程程序中的同步错误。要使用 Helgrind，首先安装 Valgrind，然后使用以下命令运行程序：

```
valgrind --tool=helgrind ./my_program
```

Helgrind 会报告潜在的线程安全问题，如数据竞争、死锁等。




# Refer

* [Concurrency examples](https://github.com/uchicago-cs/cmsc12300/tree/master/examples/cpp/concurrency/simple) - Examples of concurrency in C++11 and other languages.
* [C++ reference](https://en.cppreference.com/w/cpp)
* [Concurrency in C++11](https://www.classes.cs.uchicago.edu/archive/2013/spring/12300-1/labs/lab6/)
* [A Tutorial on Modern Multithreading and Concurrency in C++](https://www.educative.io/blog/modern-multithreading-and-concurrency-in-cpp)



