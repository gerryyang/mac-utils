---
layout: post
title:  "Google CPP Style Guide"
date:   2022-10-17 08:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Google C++ Style Guide

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










