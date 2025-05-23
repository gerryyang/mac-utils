---
layout: post
title:  "CPP Reflection in Action"
date:   2025-05-22 20:30:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# 反射的定义

> Reflection: Introspection (内省) + Intercession (调解)

* **Introspection**: Ability of a program to **examine** itself
* **Intercession**: Ability of a program to **modify** itself

* Runtime feature of modern programming languages: Java, C#, Go, …


* Use cases: **Code generation**
  + Serialization
  + O/R-Mapping
  + CLI Argument Parsing
  + …


反射 (`Reflection`) 是程序在运行时动态访问、检测和修改自身结构的能力，包括类型信息、成员变量、方法等元数据的操作。其核心功能可分为：

1. **内省 (Introspection)**：运行时获取类型名称、成员列表、继承关系等元信息
2. **动态操作**：通过类型名创建对象、调用方法、修改属性值
3. **元编程支持**：结合代码生成技术实现编译时反射（如 C++ 提案 `P2996`）

C++ 的挑战在于缺乏原生反射支持，需通过 `RTTI`、宏、模板或第三方库实现。例如 Java 的 `Class` 对象可直接获取元信息，而 C++ 的 `typeid` 仅能提供有限类型名称。


# Evaluating Reflection Support in C++23

* Introspection (**runtime**):
  + Run-time type information (`RTTI`)

* Introspection (**compile-time**):
  + Type Traits
  + Template metaprogramming
  + Concepts and constraints

* Intercession (**compile-time**):
  + Template metaprogramming
  + Compile-time function execution
  + Preprocessor metaprogramming

# Preview: C++26 Compile-Time Reflection (P2996, P1306)

示例代码 (TODO: 有编译错误 )

``` cpp
#include <print>
#include <string>
#include <experimental/meta>

template<typename T>
requires std::is_class_v<T>
void print(const T& self) {
    namespace meta = std::meta;
    template for(constexpr meta::info i : meta::define_static_array(meta::nonstatic_data_members_of(^^T))) {
        std::println("{} = {}", meta::identifier_of(i), self.[:i:]);
    }
}

struct test {
    int x;
    double y;
    std::string z;
};

int main() {
    test t{1, 3,14, "hello"};
    print(t);
}
```




# C++ 反射的实现方式

## 基于 RTTI 的有限反射

**运行期的类型，C++ 里挺有争议（跟异常类似）的功能 `RTTI`**。（还是要强调一句，你应该考虑是否用**虚函数**可以达到你需要的功能。很多项目，如 Google 的，会禁用 `RTTI`。）

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


# Refer

* [Reflection for C++26](https://isocpp.org/files/papers/P2996R4.html)
* [Reflection in C++?! Michael Hava 08/2021](https://eventos.uc3m.es/file_manager/getfile/195780)
* [C++ Reflection: Back on Track David Olsen, Compiler Engineer, NVIDIA Meeting C++, 15 Nov 2024](https://meetingcpp.com/mcpp/slides/2024/CppReflection-Olsen905433.pdf)
* [Introduction to C++ Reflection July 25, 2024](https://indico.cern.ch/event/1440627/contributions/6062471/attachments/2903678/5093676/Introduction_to_C___Reflection__PPP_25_07_2024_.pdf)
* [How to Use C++26 Reflections Properly and Make the Best Use of Them?](https://stackoverflow.com/questions/78767830/how-to-use-c26-reflections-properly-and-make-the-best-use-of-them)








