---
layout: post
title:  "Google CPP Style Guide"
date:   2022-10-17 08:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Google C++ Style Guide

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


# Refer

* [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
* [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)


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










