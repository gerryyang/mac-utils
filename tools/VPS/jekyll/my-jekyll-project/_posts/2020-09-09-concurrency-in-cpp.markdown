---
layout: post
title:  "Concurrency in CPP"
date:   2020-09-09 08:00:00 +0800
categories: [C/C++, Concurrency]
---

* Do not remove this line (it will not be displayed)
{: toc}



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


## 问题代码

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

std::map<int, int> g_m = {{0,0}};

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

## Mutex

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

## Atomic

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

## Async

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

## Condition variables

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

## Producer-consumer problem

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


## Refer

* [Concurrency examples](https://github.com/uchicago-cs/cmsc12300/tree/master/examples/cpp/concurrency/simple) - Examples of concurrency in C++11 and other languages.
* [C++ reference](https://en.cppreference.com/w/cpp)
* [Concurrency in C++11](https://www.classes.cs.uchicago.edu/archive/2013/spring/12300-1/labs/lab6/)
* [A Tutorial on Modern Multithreading and Concurrency in C++](https://www.educative.io/blog/modern-multithreading-and-concurrency-in-cpp)



