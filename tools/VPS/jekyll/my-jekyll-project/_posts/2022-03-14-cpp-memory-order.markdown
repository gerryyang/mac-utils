---
layout: post
title:  "CPP Memory Order"
date:   2022-03-14 15:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}



# Order of evaluation

Order of evaluation of any part of any expression, including order of evaluation of function arguments is unspecified (with some exceptions listed below). The compiler can evaluate operands and other subexpressions in any order, and may choose another order when the same expression is evaluated again.

**There is no concept of left-to-right or right-to-left evaluation in C++.** This is not to be confused with left-to-right and right-to-left associativity of operators: the expression a() + b() + c() is parsed as (a() + b()) + c() due to left-to-right associativity of operator+, but the function call to c may be evaluated first, last, or between a() or b() at run time:

``` cpp
#include <cstdio>
int a() { return std::puts("a"); }
int b() { return std::puts("b"); }
int c() { return std::puts("c"); }
void z(int, int, int) {}
int main() {
    z(a(), b(), c());       // all 6 permutations of output are allowed
    return a() + b() + c(); // all 6 permutations of output are allowed
}
```

Possible output:

```
b
c
a
c
a 
b
```

https://en.cppreference.com/w/cpp/language/eval_order

# Sequenced-before rules (since C++11)


## Evaluation of Expressions


Evaluation of each expression includes:

* value computations: calculation of the value that is returned by the expression. This may involve determination of the identity of the object (glvalue evaluation, e.g. if the expression returns a reference to some object) or reading the value previously assigned to an object (prvalue evaluation, e.g. if the expression returns a number, or some other value)
* Initiation of side effects: access (read or write) to an object designated by a volatile glvalue, modification (writing) to an object, calling a library I/O function, or calling a function that does any of those operations.

## Ordering

"sequenced-before" is an asymmetric, transitive, pair-wise relationship between evaluations within the same thread.

* If A is sequenced before B, then evaluation of A will be complete before evaluation of B begins.
* If A is not sequenced before B and B is sequenced before A, then evaluation of B will be complete before evaluation of A begins.
* If A is not sequenced before B and B is not sequenced before A, then two possibilities exist:
  + evaluations of A and B are unsequenced: they may be performed in any order and may overlap (within a single thread of execution, the compiler may interleave the CPU instructions that comprise A and B)
  + evaluations of A and B are indeterminately sequenced: they may be performed in any order but may not overlap: either A will be complete before B, or B will be complete before A. The order may be the opposite the next time the same expression is evaluated.


# std::memory_order (C++11)

``` cpp
// <atomic>
typedef enum memory_order {
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
} memory_order;
```

`std::memory_order` specifies how memory accesses, including regular, non-atomic memory accesses, are to be ordered around an atomic operation. Absent any constraints on a multi-core system, when multiple threads simultaneously read and write to several variables, one thread can observe the values change in an order different from the order another thread wrote them. Indeed, the apparent order of changes can even differ among multiple reader threads. Some similar effects can occur even on uniprocessor systems due to compiler transformations allowed by the memory model.

**The default behavior of all atomic operations in the library provides for sequentially consistent ordering (see discussion below). That default can hurt performance,** but the library's atomic operations can be given an additional std::memory_order argument to specify the exact constraints, beyond atomicity, that the compiler and processor must enforce for that operation.


## memory_order_relaxed	

Relaxed operation: there are no synchronization or ordering constraints imposed on other reads or writes, only this operation's atomicity is guaranteed

Example:

Atomic operations tagged memory_order_relaxed are not synchronization operations; they do not impose an order among concurrent memory accesses. They only guarantee atomicity and modification order consistency.

For example, with x and y initially zero,

``` cpp
// Thread 1:
r1 = y.load(std::memory_order_relaxed); // A
x.store(r1, std::memory_order_relaxed); // B

// Thread 2:
r2 = x.load(std::memory_order_relaxed); // C 
y.store(42, std::memory_order_relaxed); // D
```

is allowed to produce `r1 == r2 == 42` because, although A is sequenced-before B within thread 1 and C is sequenced before D within thread 2, nothing prevents D from appearing before A in the modification order of y, and B from appearing before C in the modification order of x. The side-effect of D on y could be visible to the load A in thread 1 while the side effect of B on x could be visible to the load C in thread 2. In particular, this may occur if D is completed before C in thread 2, either due to compiler reordering or at runtime.

Even with relaxed memory model, out-of-thin-air values are not allowed to circularly depend on their own computations, for example, with x and y initially zero,

``` cpp
// Thread 1:
r1 = y.load(std::memory_order_relaxed);
if (r1 == 42) x.store(r1, std::memory_order_relaxed);

// Thread 2:
r2 = x.load(std::memory_order_relaxed);
if (r2 == 42) y.store(42, std::memory_order_relaxed);
```

is not allowed to produce `r1 == r2 == 42` since the store of 42 to y is only possible if the store to x stores 42, which circularly depends on the store to y storing 42. Note that until C++14, this was technically allowed by the specification, but not recommended for implementors.

Typical use for relaxed memory ordering is incrementing counters, such as the reference counters of std::shared_ptr, since this only requires atomicity, but not ordering or synchronization (note that decrementing the shared_ptr counters requires acquire-release synchronization with the destructor)

``` cpp
#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
 
std::atomic<int> cnt = {0};
 
void f()
{
    for (int n = 0; n < 1000; ++n) {
        cnt.fetch_add(1, std::memory_order_relaxed);
    }
}
 
int main()
{
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n) {
        v.emplace_back(f);
    }
    for (auto& t : v) {
        t.join();
    }
    std::cout << "Final counter value is " << cnt << '\n'; // 10000
}
```


##  memory_order_release / memory_order_acquire / memory_order_consume


memory_order_release

A store operation with this memory order performs the **release operation**: no reads or writes in the current thread can be reordered **after this store**. All writes in the current thread are visible in other threads that acquire the same atomic variable (see Release-Acquire ordering below) and writes that carry a dependency into the atomic variable become visible in other threads that consume the same atomic 

memory_order_acquire

A load operation with this memory order performs the **acquire operation** on the affected memory location: no reads or writes in the current thread can be reordered **before this load**. All writes in other threads that release the same atomic variable are visible in the current thread


memory_order_consume

A load operation with this memory order performs a **consume operation** on the affected memory location: no reads or writes in the current thread dependent on the value currently loaded can be reordered **before this load**. Writes to data-dependent variables in other threads that release the same atomic variable are visible in the current thread. On most platforms, this affects compiler optimizations only


Example: memory_order_release / memory_order_acquire

If an atomic store in thread A is tagged `memory_order_release` and an atomic load in thread B from the same variable is tagged `memory_order_acquire`, all memory writes (non-atomic and relaxed atomic) that happened-before the atomic store from the point of view of thread A, become visible side-effects in thread B. That is, once the atomic load is completed, thread B is guaranteed to see everything thread A wrote to memory.

The synchronization is established only between the threads releasing and acquiring the same atomic variable. Other threads can see different order of memory accesses than either or both of the synchronized threads.

``` cpp
#include <thread>
#include <atomic>
#include <cassert>
#include <string>
 
std::atomic<std::string*> ptr;
int data;
 
void producer()
{
    std::string* p  = new std::string("Hello");
    data = 42;
    ptr.store(p, std::memory_order_release);
}
 
void consumer()
{
    std::string* p2;
    while (!(p2 = ptr.load(std::memory_order_acquire)))
        ;
    assert(*p2 == "Hello"); // never fires
    assert(data == 42); // never fires
}
 
int main()
{
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join(); t2.join();
}
```

The following example demonstrates transitive release-acquire ordering across three threads


``` cpp
#include <thread>
#include <atomic>
#include <cassert>
#include <vector>
 
std::vector<int> data;
std::atomic<int> flag = {0};
 
void thread_1()
{
    data.push_back(42);
    flag.store(1, std::memory_order_release);
}
 
void thread_2()
{
    int expected=1;
    while (!flag.compare_exchange_strong(expected, 2, std::memory_order_acq_rel)) {
        expected = 1;
    }
}
 
void thread_3()
{
    while (flag.load(std::memory_order_acquire) < 2)
        ;
    assert(data.at(0) == 42); // will never fire
}
 
int main()
{
    std::thread a(thread_1);
    std::thread b(thread_2);
    std::thread c(thread_3);
    a.join(); b.join(); c.join();
}
```

Example: memory_order_release / memory_order_consume

If an atomic store in thread A is tagged `memory_order_release` and an atomic load in thread B from the same variable that read the stored value is tagged `memory_order_consume`, all memory writes (non-atomic and relaxed atomic) that happened-before the atomic store from the point of view of thread A, become visible side-effects within those operations in thread B into which the load operation carries dependency, that is, once the atomic load is completed, those operators and functions in thread B that use the value obtained from the load are guaranteed to see what thread A wrote to memory.

The synchronization is established only between the threads releasing and consuming the same atomic variable. Other threads can see different order of memory accesses than either or both of the synchronized threads.

This example demonstrates dependency-ordered synchronization for pointer-mediated publication: the integer data is not related to the pointer to string by a data-dependency relationship, thus its value is undefined in the consumer.

``` cpp
#include <thread>
#include <atomic>
#include <cassert>
#include <string>
 
std::atomic<std::string*> ptr;
int data;
 
void producer()
{
    std::string* p  = new std::string("Hello");
    data = 42;
    ptr.store(p, std::memory_order_release);
}
 
void consumer()
{
    std::string* p2;
    while (!(p2 = ptr.load(std::memory_order_consume)))
        ;
    assert(*p2 == "Hello"); // never fires: *p2 carries dependency from ptr
    assert(data == 42); // may or may not fire: data does not carry dependency from ptr
}
 
int main()
{
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join(); t2.join();
}
```


## memory_order_acq_rel

A read-modify-write operation with this memory order is **both an acquire operation and a release operation**. No memory reads or writes in the current thread can be reordered **before or after this store**. All writes in other threads that release the same atomic variable are visible before the modification and the modification is visible in other threads that acquire the same atomic variable.


## memory_order_seq_cst

A load operation with this memory order performs an acquire operation, a store performs a release operation, and read-modify-write performs both an acquire operation and a release operation, plus a single total order exists in which all threads observe all modifications in the same order


Example: Sequentially-consistent ordering

Atomic operations tagged `memory_order_seq_cst` not only order memory the same way as release/acquire ordering (everything that happened-before a store in one thread becomes a visible side effect in the thread that did a load), but also establish a single total modification order of all atomic operations that are so tagged.

This example demonstrates a situation where sequential ordering is necessary. Any other ordering may trigger the assert because it would be possible for the threads c and d to observe changes to the atomics x and y in opposite order.

``` cpp

#include <thread>
#include <atomic>
#include <cassert>
 
std::atomic<bool> x = {false};
std::atomic<bool> y = {false};
std::atomic<int> z = {0};
 
void write_x()
{
    x.store(true, std::memory_order_seq_cst);
}
 
void write_y()
{
    y.store(true, std::memory_order_seq_cst);
}
 
void read_x_then_y()
{
    while (!x.load(std::memory_order_seq_cst))
        ;
    if (y.load(std::memory_order_seq_cst)) {
        ++z;
    }
}
 
void read_y_then_x()
{
    while (!y.load(std::memory_order_seq_cst))
        ;
    if (x.load(std::memory_order_seq_cst)) {
        ++z;
    }
}
 
int main()
{
    std::thread a(write_x);
    std::thread b(write_y);
    std::thread c(read_x_then_y);
    std::thread d(read_y_then_x);
    a.join(); b.join(); c.join(); d.join();
    assert(z.load() != 0);  // will never happen
}
```


# Relationship with volatile


Within a thread of execution, accesses (reads and writes) through `volatile glvalues` cannot be reordered past observable side-effects (including other volatile accesses) that are sequenced-before or sequenced-after within the same thread, but this order is not guaranteed to be observed by another thread, since volatile access does not establish inter-thread synchronization.

**In addition, volatile accesses are not atomic** (concurrent read and write is a data race) and do not order memory (non-volatile memory accesses may be freely reordered around the volatile access).

One notable exception is Visual Studio, where, with default settings, every volatile write has release semantics and every volatile read has acquire semantics (Microsoft Docs), and thus volatiles may be used for inter-thread synchronization. **Standard volatile semantics are not applicable to multithreaded programming,** although they are sufficient for e.g. communication with a std::signal handler that runs in the same thread when applied to `sig_atomic_t` variables.

https://en.cppreference.com/w/cpp/language/cv

> volatile object - an object whose type is **volatile-qualified**, or a subobject of a volatile object, or a mutable subobject of a const-volatile object. Every access (read or write operation, member function call, etc.) made through a glvalue expression of volatile-qualified type is treated as a visible side-effect for the purposes of optimization (**that is, within a single thread of execution, volatile accesses cannot be optimized out or reordered with another visible side effect that is sequenced-before or sequenced-after the volatile access. This makes volatile objects suitable for communication with a signal handler, but not with another thread of execution, see std::memory_order**). Any attempt to refer to a volatile object through a glvalue of non-volatile type (e.g. through a reference or pointer to non-volatile type) results in undefined behavior.


# Function

## is_lock_free

``` cpp
bool is_lock_free() const noexcept;
bool is_lock_free() const volatile noexcept;
```

Checks whether the atomic operations on all objects of this type are lock-free. (可以检查这个原子对象上的操作是否是无锁的)

``` cpp
#include <iostream>
#include <utility>
#include <atomic>
 
struct A { int a[100]; };
struct B { int x, y; };
int main()
{
    std::cout << std::boolalpha
              << "std::atomic<A> is lock free? "
              << std::atomic<A>{}.is_lock_free() << '\n'
              << "std::atomic<B> is lock free? "
              << std::atomic<B>{}.is_lock_free() << '\n';
}
/*
$ g++ prog.cc -Wall -Wextra -std=c++11 -latomic

std::atomic<A> is lock free? false
std::atomic<B> is lock free? true
*/
```

## fetch_add

member only of `atomic<Integral>`(C++11)

``` cpp
T fetch_add( T arg,
             std::memory_order order = std::memory_order_seq_cst ) noexcept;
T fetch_add( T arg,
             std::memory_order order = std::memory_order_seq_cst ) volatile noexcept;
```

Atomically replaces the current value with the result of arithmetic addition of the value and arg. That is, it performs atomic post-increment. The operation is read-modify-write operation. Memory is affected according to the value of order.


``` cpp
#include <iostream>
#include <thread>
#include <atomic>
 
std::atomic<long long> data{10};
 
void do_work(int thread_num)
{
    long long val = data.fetch_add(1, std::memory_order_relaxed);
    std::cout << "thread:" << thread_num << ", val:" << val << std::endl;
}
 
int main()
{
    std::thread th0{do_work, 0};
    std::thread th1{do_work, 1};
    std::thread th2{do_work, 2};
    
    th0.join(); th1.join(); th2.join();
 
    std::cout << "Result : " << data << '\n';
}
/*
thread:0, val:10
thread:1, val:11
thread:2, val:12
Result : 13
*/
```

## compare_exchange_weak / compare_exchange_strong

并发队列的实现，经常是用原子量来达到无锁和高性能的。

* 单生产者、单消费者的并发队列，用原子量和获得、释放语义就能简单实现
* 对于多生产者或多消费者的情况，那实现就比较复杂了，一般会使用 `compare_exchange_strong` 或 `compare_exchange_weak`

``` cpp
bool compare_exchange_weak( T& expected, T desired,
                            std::memory_order success,
                            std::memory_order failure ) noexcept;

bool compare_exchange_weak( T& expected, T desired,
                            std::memory_order order = std::memory_order_seq_cst ) noexcept;

bool compare_exchange_strong( T& expected, T desired,
                              std::memory_order success,
                              std::memory_order failure ) noexcept;

bool compare_exchange_strong( T& expected, T desired,
                              std::memory_order order = std::memory_order_seq_cst ) noexcept;
```

Atomically compares the object representation (until C++20) value representation (since C++20) of *this with that of expected, and if those are bitwise-equal, replaces the former with desired (performs read-modify-write operation). Otherwise, loads the actual value stored in *this into expected (performs load operation).

Return value: true if the underlying atomic value was successfully changed, false otherwise.

Compare-and-exchange operations are often used as basic building blocks of [lockfree](https://en.wikipedia.org/wiki/Non-blocking_algorithm) data structures

``` cpp
#include <atomic>
template<typename T>
struct node
{
    T data;
    node* next;
    node(const T& data) : data(data), next(nullptr) {}
};
 
template<typename T>
class stack
{
    std::atomic<node<T>*> head;
 public:
    void push(const T& data)
    {
      node<T>* new_node = new node<T>(data);
 
      // put the current value of head into new_node->next
      new_node->next = head.load(std::memory_order_relaxed);
 
      // now make new_node the new head, but if the head
      // is no longer what's stored in new_node->next
      // (some other thread must have inserted a node just now)
      // then put that new head into new_node->next and try again
      while(!head.compare_exchange_weak(new_node->next, new_node,
                                        std::memory_order_release,
                                        std::memory_order_relaxed))
          ; // the body of the loop is empty
 
// Note: the above use is not thread-safe in at least 
// GCC prior to 4.8.3 (bug 60272), clang prior to 2014-05-05 (bug 18899)
// MSVC prior to 2014-03-17 (bug 819819). The following is a workaround:
//      node<T>* old_head = head.load(std::memory_order_relaxed);
//      do {
//          new_node->next = old_head;
//       } while(!head.compare_exchange_weak(old_head, new_node,
//                                           std::memory_order_release,
//                                           std::memory_order_relaxed));
    }
};
int main()
{
    stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
}
```



# Refer

* https://en.cppreference.com/w/cpp/atomic/atomic
* https://en.cppreference.com/w/cpp/atomic/memory_order#Release-Acquire_ordering
* https://en.cppreference.com/w/cpp/language/eval_order
