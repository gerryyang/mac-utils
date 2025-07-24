---
layout: post
title:  "C++20 in Action"
date:   2025-07-22 12:30:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}


# [C++ attribute: likely, unlikely - C++20](https://en.cppreference.com/w/cpp/language/attributes/likely)

Allow the compiler to optimize for the case where paths of execution including that statement are more or less likely than any alternative path of execution that does not include such a statement.

``` cpp
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
```


refer:

* [How to use C++20's likely/unlikely attribute in if-else statement](https://stackoverflow.com/questions/51797959/how-to-use-c20s-likely-unlikely-attribute-in-if-else-statement)
* [How do the likely/unlikely macros in the Linux kernel work and what is their benefit?](https://stackoverflow.com/questions/109710/how-do-the-likely-unlikely-macros-in-the-linux-kernel-work-and-what-is-their-ben)



# C++ Sized Deallocation - C++14


* [Sized deallocation in C++: What is the correct behaviour of the global operator delete(void* ptr, std::size_t size)](https://stackoverflow.com/questions/46775187/sized-deallocation-in-c-what-is-the-correct-behaviour-of-the-global-operator)
* https://isocpp.org/files/papers/n3778.html





