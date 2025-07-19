---
layout: post
title:  "CPP Conventions in Action"
date:   2025-07-14 12:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}


# [SEI CERT C++ Coding Standard](https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=88046682)


## [Rule 08. Exceptions and Error Handling (ERR)](https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=88046339)


### [ERR61-CPP. Catch exceptions by lvalue reference](https://wiki.sei.cmu.edu/confluence/display/cplusplus/ERR61-CPP.+Catch+exceptions+by+lvalue+reference)

When an exception is thrown, the value of the object in the throw expression is used to initialize an anonymous temporary object called the **exception object**. The type of this **exception object** is used to transfer control to the nearest catch handler, which contains an exception declaration with a matching type. The C++ Standard, `[except.handle]`, paragraph 16 `[ISO/IEC 14882-2014]`, in part, states the following:

> The variable declared by the exception-declaration, of type `cv T` or `cv T&`, is initialized from the **exception object**, of type `E`, as follows:
>
> — if `T` is a base class of `E`, the variable is copy-initialized from the corresponding base class **subobject** of the exception object;
> — otherwise, the variable is copy-initialized from the exception object.

Because the variable declared by the exception-declaration is copy-initialized, it is possible to [slice](https://en.wikipedia.org/wiki/Object_slicing) the exception object as part of the copy operation, losing valuable exception information and leading to incorrect error recovery. For more information about object slicing, see [OOP51-CPP. Do not slice derived objects](https://wiki.sei.cmu.edu/confluence/display/cplusplus/OOP51-CPP.+Do+not+slice+derived+objects). Further, if the copy constructor of the exception object throws an exception, the copy initialization of the exception-declaration object results in undefined behavior. (See [ERR60-CPP. Exception objects must be nothrow copy constructible](https://wiki.sei.cmu.edu/confluence/display/cplusplus/ERR60-CPP.+Exception+objects+must+be+nothrow+copy+constructible) for more information.)

Always catch exceptions by [lvalue](https://wiki.sei.cmu.edu/confluence/display/cplusplus/BB.+Definitions#BB.Definitions-lvalue) reference unless the type is a **trivial type**. For reference, the C++ Standard, `[basic.types]`, paragraph 9 `[ISO/IEC 14882-2014]`, defines trivial types as the following:

> Arithmetic types, enumeration types, pointer types, pointer to member types, std::nullptr_t, and cv-qualified versions of these types are collectively called scalar types.... Scalar types, trivial class types, arrays of such types and cv-qualified versions of these types are collectively called **trivial types**.

The C++ Standard, `[class]`, paragraph 6, defines **trivial class** types as the following:

> A trivially copyable class is a class that:
>
> — has no non-trivial copy constructors,
> — has no non-trivial move constructors,
> — has no non-trivial copy assignment operators,
> — has no non-trivial move assignment operators, and
> — has a trivial destructor.
>
> A trivial class is a class that has a default constructor, has no non-trivial default constructors, and is trivially copyable. [Note: In particular, a trivially copyable or trivial class does not have virtual functions or virtual base classes. — end note]

#### Noncompliant Code Example

In this noncompliant code example, an object of type `S` is used to initialize the **exception object** that is later caught by an exception-declaration of type `std::exception`. The exception-declaration matches the exception object type, so the variable `e` is copy-initialized from the exception object, resulting in the exception object being **sliced**. Consequently, the output of this noncompliant code example is the implementation-defined value returned from calling `std::exception::what()` instead of "My custom exception".

如果 throw 一个子类异常对象，catch 的类型是父类，则会发生 **sliced 对象切片问题**，可以捕获到子类的异常但是输出的 `e.what()` 信息是不对的，输出的是父类的错误信息而不是子类的错误信息。

``` cpp
#include <exception>
#include <iostream>

struct S : std::exception {
  const char *what() const noexcept override {
    return "My custom exception";
  }
};

void f() {
  try {
    throw S();
  } catch (std::exception e) {
    std::cout << "catch: " << e.what() << std::endl; // 输出：catch: std::exception
  }
}

int main()
{
  f();
  return 0;
}
```

如果 throw 一个父类异常对象，catch 的类型是子类，则捕获不到，继续在后面的 catch 中进行判断。


``` cpp
#include <exception>
#include <iostream>

struct S : std::exception {
  const char *what() const noexcept override {
    return "My custom exception S";
  }
};

struct S2 : S {
  const char *what() const noexcept override {
    return "My custom exception S2";
  }
};

void f() {
  try {
    throw S();
  } catch (S2 s2) {
    std::cout << "catch: " << s2.what() << std::endl;
  }
}

int main()
{
  try {
    f();
  } catch (S s) {
    std::cout << "catch: " << s.what() << std::endl; // 输出：catch: My custom exception S
  }
  return 0;
}
```

#### Compliant Solution

In this compliant solution, the variable declared by the exception-declaration is an **lvalue reference**. The call to `what()` results in executing `S::what()` instead of `std::exception::what()`.

``` cpp
#include <exception>
#include <iostream>

struct S : std::exception {
  const char *what() const noexcept override {
    return "My custom exception";
  }
};

void f() {
  try {
    throw S();
  } catch (std::exception &e) {
    std::cout << "catch: " << e.what() << std::endl; // 输出：catch: My custom exception
  }
}

int main()
{
  f();
  return 0;
}
```

#### Q&A: [C++ catch blocks - catch exception by value or reference?](https://stackoverflow.com/questions/2522299/c-catch-blocks-catch-exception-by-value-or-reference)

The standard practice for exceptions in C++ is ...

> **Throw by value, catch by reference**

Catching by value is problematic in the face of inheritance hierarchies. Suppose for your example that there is another type `MyException` which inherits from `CustomException` and overrides items like an error code. If a `MyException` type was thrown your catch block would cause it to be converted to a `CustomException` instance which would cause the error code to change.

> 什么是切片？

当派生类对象被当作其基类对象使用时（通常是赋值、传值或返回），派生类特有的部分会被“切掉”，只留下基类的部分。在异常处理中，如果你抛出一个派生类对象，例如 `class MyError : public std::runtime_error`，但在 `catch` 块中按值捕获其基类类型 `catch (std::runtime_error e)`，那么当异常对象被复制到 `catch` 块的 `e` 变量时，就会发生切片。

> 为什么切片在异常捕获中是问题？

1. **丢失信息**：`catch` 块中的 `e` 只是一个基类对象的副本。派生类中添加的任何额外成员变量或重写的虚函数（除了基类中声明的）都无法通过 `e` 访问。你丢失了异常的具体类型信息和它携带的额外数据。
2. **破坏多态性**：即使你试图通过基类指针/引用调用虚函数，因为 `e` 是一个基类对象（不是指向派生类对象的引用），调用的将是基类的虚函数实现，而不是派生类重写的版本。

> 为什么按 **const 引用**捕获 (`catch (const std::runtime_error& e)`) 能解决切片问题？

1. 引用 (`&`) 只是原始对象的一个**别名**。
2. 当使用 `catch (const BaseType& e)` 时：
   * `e` 直接绑定到实际抛出的异常对象上（无论它是 `BaseType` 还是 `DerivedType`）。
   * 不会创建任何副本，因此**不会发生切片**。
   * 可以安全地访问基类接口 (`e.what()`)。
   * 如果基类有虚函数，通过 `e` 调用时会正确地进行动态绑定，执行实际对象类型（派生类）的虚函数实现。
   * 使用 `const` 保证不会意外修改异常对象，这是良好实践。


> 关于 `throw;` 无参数的用法说明

1. `throw;` (不带任何参数) 是一个特殊的语句，只能在 `catch` 块内部使用。
2. 它的作用是：**重新抛出当前正在处理的异常对象**。
3. 无论是通过值捕获 (`catch (T e)`) 还是通过引用捕获 (`catch (const T& e)`) 了异常。**`throw;` 总是重新抛出原始的、未被修改的异常对象**。

> 何时使用无参 `throw;`：在需要重新抛出异常时，总是使用无参 `throw;` 而不是 `throw e`;，以保持原始异常类型和信息的完整性。


1. 在异常处理中途释放资源后重新抛出

``` cpp
catch (...) {
    cleanupResources();  // 释放资源
    throw;              // 保留原始异常继续传播
}
```

2. 在记录日志后保留原始异常信息

``` cpp
catch (const std::exception& e) {
    logError(e.what());  // 记录错误
    throw;               // 继续传播原始异常
}
```

3. 实现异常处理中间层时保持异常类型透明性


代码示例：

``` cpp
#include <iostream>
#include <stdexcept>

// 自定义异常层次结构
class BaseException : public std::exception {
public:
    virtual const char* what() const noexcept override {
        return "BaseException occurred";
    }
};

class DerivedException : public BaseException {
public:
    const char* what() const noexcept override {
        return "DerivedException occurred";
    }
};

void rethrowExample() {
    try {
        try {
            // 1. 抛出派生类异常
            throw DerivedException();

        }
        // 2. 按值捕获（会发生切片）
        catch (BaseException e) {
            std::cout << "Inner catch (by value): " << e.what() << std::endl;

            // 3. 无参throw重新抛出
            throw; // 重新抛出原始异常（仍是DerivedException）
        }
    }
    // 4. 外层按引用捕获
    catch (const BaseException& e) {
        std::cout << "Outer catch (by reference): " << e.what() << std::endl;
    }
}

void referenceExample() {
    try {
        try {
            throw DerivedException();
        }
        // 5. 按引用捕获（无切片）
        catch (const BaseException& e) {
            std::cout << "Inner catch (by reference): " << e.what() << std::endl;
            throw; // 仍重新抛出原始DerivedException
        }
    }
    catch (const BaseException& e) {
        std::cout << "Outer catch (by reference): " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "=== Slice then rethrow ===" << std::endl;
    rethrowExample();

    std::cout << "\n=== Reference rethrow ===" << std::endl;
    referenceExample();

    return 0;
}
/*
输出：

=== Slice then rethrow ===
Inner catch (by value): BaseException occurred
Outer catch (by reference): DerivedException occurred

=== Reference rethrow ===
Inner catch (by reference): DerivedException occurred
Outer catch (by reference): DerivedException occurred
*/
```


`throw;` 的核心行为：

``` cpp
// 在 catch 块内部使用
catch (...) {
    throw;  // 重新抛出当前处理的原始异常
}
```

* 总是重新抛出最初进入 `catch` 块的异常对象
* 不受当前 `catch` 块捕获方式（值/引用）的影响
* 保留原始异常的所有信息（包括 `RTTI`）

对比错误用法：

``` cpp
catch (BaseException e) {
    throw e;  // 错误！抛出的是切片后的副本
}
```

这种带参数的 `throw` 会抛出当前局部对象 `e`（切片后的基类副本），而不是原始异常。




# Google C++ Style Guide

## [Forward Declarations](https://google.github.io/styleguide/cppguide.html#Forward_Declarations) (不建议)

Avoid using forward declarations where possible. Instead, [include the headers you need](https://google.github.io/styleguide/cppguide.html#Include_What_You_Use).

**Definition**:

A "forward declaration" is a declaration of an entity without an associated definition.

``` cpp
// In a C++ source file:
class B;
void FuncInB();
extern int variable_in_b;
ABSL_DECLARE_FLAG(flag_in_b);
```

**Pros**:

* Forward declarations can save compile time, as #includes force the compiler to open more files and process more input.
* Forward declarations can save on unnecessary recompilation. #includes can force your code to be recompiled more often, due to unrelated changes in the header.

**Cons**:

* Forward declarations can hide a dependency, allowing user code to skip necessary recompilation when headers change.
* A forward declaration as opposed to an #include statement makes it difficult for automatic tooling to discover the module defining the symbol.
* A forward declaration may be broken by subsequent changes to the library. Forward declarations of functions and templates can prevent the header owners from making otherwise-compatible changes to their APIs, such as widening a parameter type, adding a template parameter with a default value, or migrating to a new namespace.
* Forward declaring symbols from namespace std:: yields undefined behavior.
* It can be difficult to determine whether a forward declaration or a full #include is needed. Replacing an #include with a forward declaration can silently change the meaning of code:

``` cpp
// b.h:
struct B {};
struct D : B {};

// good_user.cc:
#include "b.h"
void f(B*);
void f(void*);
void test(D* x) { f(x); }  // Calls f(B*)
```

If the #include was replaced with forward decls for B and D, test() would call f(void*).

* Forward declaring multiple symbols from a header can be more verbose than simply #includeing the header.
* Structuring code to enable forward declarations (e.g., using pointer members instead of object members) can make the code slower and more complex.

**Decision**:

Try to avoid forward declarations of entities defined in another project.





## [Casting](https://google.github.io/styleguide/cppguide.html#Casting)

Use C++-style casts like `static_cast<float>(double_value)`, or brace initialization for conversion of arithmetic types like `int64_t y = int64_t{1} << 42`. Do not use cast formats like `(int)x` unless the cast is to `void`. You may use cast formats like `T(x)` only when `T` is a class type.

**Definitions:**

C++ introduced a different cast system from C that distinguishes the types of cast operations.

**Pros:**

The problem with C casts is the ambiguity of the operation; sometimes you are doing a conversion (e.g., `(int)3.5`) and sometimes you are doing a cast (e.g., `(int)"hello"`). Brace initialization and C++ casts can often help avoid this ambiguity. Additionally, C++ casts are more visible when searching for them.

**Cons:**

The C++-style cast syntax is verbose and cumbersome.

**Decision:**

In general, do not use C-style casts. Instead, use these C++-style casts when explicit type conversion is necessary.

* Use brace initialization to **convert arithmetic types** (e.g., `int64_t{x}`). This is the safest approach because code will not compile if conversion can result in information loss. The syntax is also concise.
* Use `absl::implicit_cast` to safely **cast up a type hierarchy**, e.g., casting a `Foo*` to a `SuperclassOfFoo*` or casting a `Foo*` to a `const Foo*`. C++ usually does this automatically but some situations need an explicit up-cast, such as use of the `?:` operator.
* Use `static_cast` as the equivalent of a C-style cast that does value conversion, when you need to explicitly up-cast a pointer from a class to its superclass, or when you need to explicitly cast a pointer from a superclass to a subclass. In this last case, you must be sure your object is actually an instance of the subclass.
* Use `const_cast` to **remove the const qualifier** (see **const**).
* Use `reinterpret_cast` to do **unsafe conversions of pointer types to and from integer and other pointer types**, including `void*`. Use this only if you know what you are doing and you understand the aliasing issues. Also, consider the alternative `absl::bit_cast`.
* Use `absl::bit_cast` to interpret the raw bits of a value using a different type of the same size (a type pun), such as interpreting the bits of a double as int64_t.

See the [RTTI section](https://google.github.io/styleguide/cppguide.html#Run-Time_Type_Information__RTTI_) for guidance on the use of `dynamic_cast`.



## [TODO Comments](https://google.github.io/styleguide/cppguide.html#TODO_Comments)

Use TODO comments for code that is temporary, a short-term solution, or good-enough but not perfect.

TODOs should include the string TODO in all caps, followed by the name, e-mail address, bug ID, or other identifier of the person or issue with the best context about the problem referenced by the TODO. The main purpose is to have a consistent TODO that can be searched to find out how to get more details upon request. A TODO is not a commitment that the person referenced will fix the problem. Thus when you create a TODO with a name, it is almost always your name that is given.

```
// TODO(kl@gmail.com): Use a "*" here for concatenation operator.
// TODO(Zeke) change this to use relations.
// TODO(bug 12345): remove the "Last visitors" feature.
```

If your TODO is of the form "At a future date do something" make sure that you either include a very specific date ("Fix by November 2005") or a very specific event ("Remove this code when all clients can handle XML responses.").


## [Designated Initializers](https://google.github.io/styleguide/cppguide.html#Designated_initializers)

Use designated initializers only in their C++20-compliant form.

[Designated initializers](https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers) are a syntax that allows for initializing an aggregate ("plain old struct") by naming its fields explicitly:

``` cpp
struct Point {
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
  };

  Point p = {  // Designated Initializers
    .x = 1.0,
    .y = 2.0,
    // z will be 0.0
  };
```

The explicitly listed fields will be initialized as specified, and others will be initialized in the same way they would be in a traditional aggregate initialization expression like `Point{1.0, 2.0}`.




# LLVM Coding Standards


## [misc-throw-by-value-catch-by-reference](https://clang.llvm.org/extra/clang-tidy/checks/misc/throw-by-value-catch-by-reference.html)

Finds violations of the rule “Throw by value, catch by reference” presented for example in “C++ Coding Standards” by H. Sutter and A. Alexandrescu, as well as the CERT C++ Coding Standard rule [ERR61-CPP. Catch exceptions by lvalue reference](https://wiki.sei.cmu.edu/confluence/display/cplusplus/ERR61-CPP.+Catch+exceptions+by+lvalue+reference).


Exceptions:

* Throwing string literals will not be flagged despite being a pointer. They are not susceptible to slicing and the usage of string literals is idiomatic.
* Catching character pointers (`char`, `wchar_t`, unicode character types) will not be flagged to allow catching string literals.
* Moved named values will not be flagged as not throwing an anonymous temporary. In this case we can be sure that the user knows that the object can’t be accessed outside catch blocks handling the error.
* Throwing function parameters will not be flagged as not throwing an anonymous temporary. This allows helper functions for throwing.
* Re-throwing caught exception variables will not be flagged as not throwing an anonymous temporary. Although this can usually be done by just writing throw; it happens often enough in real code.

在C++编程中，为了遵循“按值抛出，按引用捕获”的规则，一些特定情况下的异常处理。这个规则出现在H. Sutter和A. Alexandrescu的《C++编程规范》以及CERT C++编程规范的ERR61-CPP条款中。

以下是一些例外情况：

* 抛出字符串字面量不会被标记，尽管它们是指针。字符串字面量不容易被切片，而且使用字符串字面量是惯用的写法。
* 捕获字符指针（char、wchar_t、Unicode字符类型）不会被标记，以便捕获字符串字面量。
* 移动已命名值不会被标记为不抛出匿名临时值。在这种情况下，我们可以确信用户知道该对象无法在处理错误的catch块之外访问。
* 抛出函数参数不会被标记为不抛出匿名临时值。这允许用于抛出异常的辅助函数。
* 重新抛出捕获到的异常变量不会被标记为不抛出匿名临时值。尽管这通常可以通过仅编写throw来完成，但在实际代码中经常会出现这种情况。

总之，这些例外情况是为了在特定场景下更好地遵循“按值抛出，按引用捕获”的规则，以提高代码的可读性和可维护性。

按照规则，应该这样抛出和捕获异常：

``` cpp
class MyException {};

void foo() {
    throw MyException(); // 按值抛出
}

int main() {
    try {
        foo();
    } catch (const MyException& e) { // 按引用捕获
        // 处理异常
    }
}
```

以下是一些例外情况的示例：

* 抛出字符串字面量：

``` cpp
void foo() {
    throw "An error occurred"; // 抛出字符串字面量，不会被标记
}

int main() {
    try {
        foo();
    } catch (const char* e) { // 捕获字符串字面量
        // 处理异常
    }
}
```

* 抛出函数参数：

``` cpp
class MyException {};

// 辅助函数，用于抛出异常
void throwError(const MyException& e) {
    throw e; // 抛出函数参数，不会被标记为不抛出匿名临时值
}

void foo() {
    MyException e;
    throwError(e); // 调用辅助函数抛出异常
}

int main() {
    try {
        foo();
    } catch (const MyException& e) { // 按引用捕获
        // 处理异常
    }
}
```

When an exception is thrown, the value of the object in the throw expression is used to initialize an anonymous temporary object called the exception object. The type of this exception object is used to transfer control to the nearest catch handler, which contains an exception declaration with a matching type. The C++ Standard, except.handle, paragraph 16 [ISO/IEC 14882-2014](https://wiki.sei.cmu.edu/confluence/display/cplusplus/AA.+Bibliography#AA.Bibliography-ISO/IEC14882-2014), in part, states the following:

> The variable declared by the exception-declaration, of type cv T or cv T&, is initialized from the exception object, of type E, as follows:
>
> — if T is a base class of E, the variable is copy-initialized from the corresponding base class subobject of the exception object;
> — otherwise, the variable is copy-initialized from the exception object.

Because the variable declared by the exception-declaration is copy-initialized, it is possible to [slice](https://en.wikipedia.org/wiki/Object_slicing) the exception object as part of the copy operation, losing valuable exception information and leading to incorrect error recovery. For more information about object slicing, see [OOP51-CPP. Do not slice derived objects](https://wiki.sei.cmu.edu/confluence/display/cplusplus/OOP51-CPP.+Do+not+slice+derived+objects). Further, if the copy constructor of the exception object throws an exception, the copy initialization of the exception-declaration object results in undefined behavior. (See [ERR60-CPP. Exception objects must be nothrow copy constructible](https://wiki.sei.cmu.edu/confluence/display/cplusplus/ERR60-CPP.+Exception+objects+must+be+nothrow+copy+constructible) for more information.)

Always catch exceptions by [lvalue](https://wiki.sei.cmu.edu/confluence/display/cplusplus/BB.+Definitions#BB.Definitions-lvalue) reference unless the **type is a trivial type**. For reference, the C++ Standard, basic.types, paragraph 9 [[ISO/IEC 14882-2014](https://wiki.sei.cmu.edu/confluence/display/cplusplus/AA.+Bibliography#AA.Bibliography-ISO/IEC14882-2014)], defines trivial types as the following:

> Arithmetic types, enumeration types, pointer types, pointer to member types, std::nullptr_t, and cv-qualified versions of these types are collectively called scalar types.... Scalar types, trivial class types, arrays of such types and cv-qualified versions of these types are collectively called trivial types.

The C++ Standard, class, paragraph 6, defines trivial class types as the following:

> A trivially copyable class is a class that:
>
> — has no non-trivial copy constructors,
> — has no non-trivial move constructors,
> — has no non-trivial copy assignment operators,
> — has no non-trivial move assignment operators, and
> — has a trivial destructor.
>
> A trivial class is a class that has a default constructor, has no non-trivial default constructors, and is trivially copyable. [Note: In particular, a trivially copyable or trivial class does not have virtual functions or virtual base classes. — end note]

Noncompliant Code Example:

In this noncompliant code example, an object of type S is used to initialize the exception object that is later caught by an exception-declaration of type std::exception. The exception-declaration matches the exception object type, so the variable E is copy-initialized from the exception object, resulting in the exception object being sliced. Consequently, the output of this noncompliant code example is the implementation-defined value returned from calling std::exception::what() instead of "My custom exception".

``` cpp
#include <exception>
#include <iostream>

struct S : std::exception {
  const char *what() const noexcept override {
    return "My custom exception";
  }
};

void f() {
  try {
    throw S();
  } catch (std::exception e) {
    std::cout << e.what() << std::endl;  // 输出 std::exception 而不是 My custom exception
  }
}
```

Compliant Solution:

In this compliant solution, the variable declared by the exception-declaration is an lvalue reference. The call to what() results in executing S::what() instead of std::exception::what().

``` cpp
#include <exception>
#include <iostream>

struct S : std::exception {
  const char *what() const noexcept override {
    return "My custom exception";
  }
};

void f() {
  try {
    throw S();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}
```


参考：[ERR61-CPP. Catch exceptions by lvalue reference](https://wiki.sei.cmu.edu/confluence/display/cplusplus/ERR61-CPP.+Catch+exceptions+by+lvalue+reference)





## [misc-definitions-in-headers](https://clang.llvm.org/extra/clang-tidy/checks/misc/definitions-in-headers.html)

Finds non-extern non-inline function and variable definitions in header files, which can lead to potential ODR violations in case these headers are included from multiple translation units.

``` cpp
// Foo.h
int a = 1; // Warning: variable definition.
extern int d; // OK: extern variable.

namespace N {
  int e = 2; // Warning: variable definition.
}

// Warning: variable definition.
const char* str = "foo";

// OK: internal linkage variable definitions are ignored for now.
// Although these might also cause ODR violations, we can be less certain and
// should try to keep the false-positive rate down.
static int b = 1;
const int c = 1;
const char* const str2 = "foo";
constexpr int k = 1;
namespace { int x = 1; }

// Warning: function definition.
int g() {
  return 1;
}

// OK: inline function definition is allowed to be defined multiple times.
inline int e() {
  return 1;
}

class A {
public:
  int f1() { return 1; } // OK: implicitly inline member function definition is allowed.
  int f2();

  static int d;
};

// Warning: not an inline member function definition.
int A::f2() { return 1; }

// OK: class static data member declaration is allowed.
int A::d = 1;

// OK: function template is allowed.
template<typename T>
T f3() {
  T a = 1;
  return a;
}

// Warning: full specialization of a function template is not allowed.
template <>
int f3() {
  int a = 1;
  return a;
}

template <typename T>
struct B {
  void f1();
};

// OK: member function definition of a class template is allowed.
template <typename T>
void B<T>::f1() {}

class CE {
  constexpr static int i = 5; // OK: inline variable definition.
};

inline int i = 5; // OK: inline variable definition.

constexpr int f10() { return 0; } // OK: constexpr function implies inline.

// OK: C++14 variable templates are inline.
template <class T>
constexpr T pi = T(3.1415926L);
```




## [readability-else-after-return](https://clang.llvm.org/extra/clang-tidy/checks/readability/else-after-return.html)

[LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html) advises to reduce indentation where possible and where it makes understanding code easier. Early exit is one of the suggested enforcements of that. Please do not use `else` or `else if` after something that interrupts control flow - like `return`, `break`, `continue`, `throw`.

建议代码示例：

``` cpp
void foo(int Value) {
  int Local = 0;
  for (int i = 0; i < 42; i++) {
    if (Value == 1) {
      return;
    }
    Local++;

    if (Value == 2)
      continue;
    Local++;

    if (Value == 3) {
      throw 42;
    }
    Local++;
  }
}
```

## [modernize-use-noexcept](https://clang.llvm.org/extra/clang-tidy/checks/modernize/use-noexcept.html)

This check replaces deprecated dynamic exception specifications with the appropriate noexcept specification (introduced in C++11). By default this check will replace `throw()` with `noexcept`, and `throw(<exception>[,...])` or `throw(...)` with `noexcept(false)`.

``` cpp
void foo() throw();    // 列表为空表示不抛出异常
void bar() throw(int) {}

// transforms to:
void foo() noexcept;
void bar() noexcept(false) {}
```

## [readability-use-anyofallof](https://clang.llvm.org/extra/clang-tidy/checks/readability/use-anyofallof.html)

Finds range-based for loops that can be replaced by a call to `std::any_of` or `std::all_of`. In C++ 20 mode, suggests `std::ranges::any_of` or `std::ranges::all_of`.

``` cpp
bool all_even(std::vector<int> V) {
  for (int I : V) {
    if (I % 2)
      return false;
  }
  return true;

  // Replace loop by
  // return std::ranges::all_of(V, [](int I) { return I % 2 == 0; });
}
```

## [readability-simplify-boolean-expr](https://clang.llvm.org/extra/clang-tidy/checks/readability/simplify-boolean-expr.html)

Looks for boolean expressions involving boolean constants and simplifies them to use the appropriate boolean expression directly. Simplifies boolean expressions by application of DeMorgan’s Theorem.

Examples:

| Initial expression	| Result
| -- | --
| if (b == true)	| if (b)
| if (b == false)	 | if (!b)
| if (b && true)	| if (b)
| if (b && false)	| if (false)
| if (b || true)	| if (true)
| if (b || false)	| if (b)
| e ? true : false	| e
| e ? false : true	| !e
| if (true) t(); else f();	| t();
| if (false) t(); else f();	| f();
| if (e) return true; else return false;	| return e;
| if (e) return false; else return true;	| return !e;
| if (e) b = true; else b = false;	| b = e;
| if (e) b = false; else b = true;	| b = !e;
| if (e) return true; return false;	| return e;
| if (e) return false; return true;	| return !e;
| !(!a || b)	| a && !b
| !(a || !b)	| !a && b
| !(!a || !b)	| a && b
| !(!a && b)	| a || !b
| !(a && !b)	| !a || b
| !(!a && !b)	| a || b


## default arguments on virtual or override methods are prohibited

[Can virtual functions have default parameters?](https://stackoverflow.com/questions/3533589/can-virtual-functions-have-default-parameters)


## [modernize-return-braced-init-list](https://clang.llvm.org/extra/clang-tidy/checks/modernize/return-braced-init-list.html)

Replaces explicit calls to the constructor in a return with a braced initializer list. This way the return type is not needlessly duplicated in the function definition and the return statement.

``` cpp
Foo bar() {
  Baz baz;
  return Foo(baz);
}

// transforms to:

Foo bar() {
  Baz baz;
  return {baz};
}
```

## [modernize-use-nodiscard](https://clang.llvm.org/extra/clang-tidy/checks/modernize/use-nodiscard.html)

Adds `[[nodiscard]]` attributes (introduced in C++17) to member functions in order to highlight at compile time which return values should not be ignored.

`[[nodiscard]]` 是 C++17 引入的属性，用于标记函数的返回值不可被忽略。若调用者未处理返回值，编译器会生成警告。

**应使用 modernize-use-nodiscard 的场景：**

1. 函数仅通过返回值传递关键信息。
2. 忽略返回值可能导致逻辑错误、资源泄漏或性能浪费。
3. 函数无副作用，且符合工具列出的静态条件。

通过自动化标记，该工具帮助团队强制执行“**必须处理返回值**”的设计约定，提升代码安全性和可维护性。

**适用场景：**

* 纯计算型函数。
  + 特征：函数仅通过返回值传递结果，不修改对象或外部状态（因为是 const 成员函数），且无副作用。
  + 风险：若忽略返回值，计算逻辑完全浪费，可能隐藏逻辑错误。

``` cpp
class MathUtils {
public:
    // 计算结果，忽略返回值无意义
    [[nodiscard]] int computeValue() const {
        return someComplexCalculation();
    }
};
```

* 资源状态查询
  + 特征：返回对象关键状态（如是否为空、是否有效），但可能被误认为“动作”而非“查询”。
  + 风险：若未检查返回值直接操作资源（如发送数据），可能引发未定义行为。

``` cpp
class Connection {
public:
    // 检查连接是否有效，忽略返回值可能导致后续操作失败
    [[nodiscard]] bool isValid() const {
        return status == Connected;
    }
};
```

* 工厂方法或构造型函数
  + 特征：返回新对象或资源句柄，但可能被误认为修改当前对象。
  + 风险：若忽略返回值，开发者可能误以为原对象被修改，导致逻辑错误。

``` cpp
class StringProcessor {
public:
    // 生成新字符串，原对象未被修改
    [[nodiscard]] std::string toUpper() const {
        return transformToUppercase();
    }
};
```

**错误用法（触发警告）**

``` cpp
auto result = vec.empty(); // 正确：使用返回值
vec.empty();               // 警告：未处理 [[nodiscard]] 值
```

绕过警告。若需主动忽略返回值（少数情况），可显式转换为 void：

``` cpp
static_cast<void>(vec.someNodiscardMethod());
```


## [misc-throw-by-value-catch-by-reference](https://clang.llvm.org/extra/clang-tidy/checks/misc/throw-by-value-catch-by-reference.html)

Finds violations of the rule “Throw by value, catch by reference” presented for example in “C++ Coding Standards” by H. Sutter and A. Alexandrescu, as well as the CERT C++ Coding Standard rule [ERR61-CPP. Catch exceptions by lvalue reference](https://wiki.sei.cmu.edu/confluence/display/cplusplus/ERR61-CPP.+Catch+exceptions+by+lvalue+reference).


## [modernize-pass-by-value](https://clang.llvm.org/extra/clang-tidy/checks/modernize/pass-by-value.html)

With **move semantics** added to the language and the standard library updated with **move constructors** added for many types it is now interesting to **take an argument directly by value, instead of by const-reference**, and then copy. This check allows the compiler to take care of choosing the best way to construct the copy.

The transformation is usually beneficial when the calling code passes an **rvalue** and assumes the **move construction** is a cheap operation. This short example illustrates how the construction of the value happens:

``` cpp
void foo(std::string s);
std::string get_str();

void f(const std::string &str) {
  foo(str);       // lvalue  -> copy construction
  foo(get_str()); // prvalue -> move construction
}
```

> Note: Currently, **only constructors are transformed to make use of pass-by-value**. Contributions that handle other situations are welcome!

## [readability-isolate-declaration](https://clang.llvm.org/extra/clang-tidy/checks/readability/isolate-declaration.html)

Detects local variable declarations declaring more than one variable and tries to refactor the code to one statement per declaration.

The automatic code-transformation will use the same indentation as the original for every created statement and add a line break after each statement. It keeps the order of the variable declarations consistent, too.

``` cpp
void f() {
  int * pointer = nullptr, value = 42, * const const_ptr = &value;
  // This declaration will be diagnosed and transformed into:
  // int * pointer = nullptr;
  // int value = 42;
  // int * const const_ptr = &value;
}
```





# Other

## [Initialization](https://en.cppreference.com/w/cpp/language/initialization)

Initialization of a variable provides its initial value at the time of construction.

* [aggregate initialization](https://en.cppreference.com/w/cpp/language/aggregate_initialization)
* [constant initialization](https://en.cppreference.com/w/cpp/language/constant_initialization)
* [copy initialization](https://en.cppreference.com/w/cpp/language/copy_initialization)
* [direct initialization](https://en.cppreference.com/w/cpp/language/direct_initialization)
* [reference initialization](https://en.cppreference.com/w/cpp/language/reference_initialization)
* [value initialization](https://en.cppreference.com/w/cpp/language/value_initialization)
* [zero initialization](https://en.cppreference.com/w/cpp/language/zero_initialization)


# Refer

* [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
* [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)










