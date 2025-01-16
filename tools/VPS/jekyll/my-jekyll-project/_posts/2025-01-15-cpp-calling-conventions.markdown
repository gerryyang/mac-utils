---
layout: post
title:  "CPP Calling Conventions (函数调用约定)"
date:   2025-01-15 12:30:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# [Calling convention](https://en.wikipedia.org/wiki/Calling_convention)

In computer science, a **calling convention** is an implementation-level (low-level) scheme for how subroutines or functions receive parameters from their caller and how they return a result.

When some code calls a function, design choices have been taken for where and how parameters are passed to that function, and where and how results are returned from that function, with these transfers typically done via certain registers or within a [stack frame](https://en.wikipedia.org/wiki/Stack_frame) on the [call stack](https://en.wikipedia.org/wiki/Call_stack). There are design choices for how the tasks of preparing for a function call and restoring the environment after the function has completed are divided between the caller and the callee. Some calling convention specifies the way every function should get called. The correct calling convention should be used for every function call, to allow the correct and reliable execution of the whole program using these functions.

# [x86 calling conventions](https://en.wikipedia.org/wiki/X86_calling_conventions)

> x86-64 calling conventions

x86-64 calling conventions take advantage of the added register space to pass more arguments in registers. Also, the number of incompatible calling conventions has been reduced. There are two in common use.




# [Problem：What is the effect of extern "C" in C++?](https://stackoverflow.com/questions/1041866/what-is-the-effect-of-extern-c-in-c)

What exactly does putting `extern "C"` into `C++` code do?

For example:

``` cpp
extern "C" {
   void foo();
}
```

## Answer0: Why

In every C++ program, all non-static functions are represented in the binary file as symbols. These symbols are special text strings that uniquely identify a function in the program.

In C, the symbol name is the same as the function name. This is possible because in C no two non-static functions can have the same name.

Because C++ allows overloading and has many features that C does not — like classes, member functions, exception specifications - it is not possible to simply use the function name as the symbol name. To solve that, C++ uses so-called name mangling, which transforms the function name and all the necessary information (like the number and size of the arguments) into some weird-looking string processed only by the compiler and linker.

So if you specify a function to be extern C, the compiler doesn't performs name mangling with it and it can be directly accessed using its symbol name as the function name.

This comes handy while using `dlsym()` and `dlopen()` for calling such functions.


## Answer1: extern "C"

`extern "C"` makes a function-name in `C++` have `C` linkage (**compiler does not mangle the name**) so that client C code can link to (use) your function using a C compatible header file that contains just the declaration of your function. Your function definition is contained in a binary format (that was compiled by your C++ compiler) that the client C linker will then link to using the C name.

Since `C++` has overloading of function names and `C` does not, **the C++ compiler cannot just use the function name as a unique id to link to, so it mangles the name by adding information about the arguments. A C compiler does not need to mangle the name since you can not overload function names in C**. When you state that a function has extern "C" linkage in C++, the C++ compiler does not add argument/parameter type information to the name used for linkage.

Just so you know, you can specify extern "C" linkage to each individual declaration/definition explicitly or use a block to group a sequence of declarations/definitions to have a certain linkage:

``` c
extern "C" void foo(int);
extern "C"
{
   void g(char);
   int i;
}
```

If you care about the technicalities, they are listed in section 7.5 of the C++03 standard, here is a brief summary (with emphasis on **extern "C"**):

* `extern "C"` is a linkage-specification
* Every compiler is required to provide "C" linkage
* A linkage specification shall occur only in namespace scope
* Two function types with distinct language linkages are distinct types even if otherwise identical
* Linkage specs nest, inner one determines the final linkage
* `extern "C"` is ignored for class members
* At most one function with a particular name can have "C" linkage (regardless of namespace)


## Answer2: __cplusplus

You'll very often see code in C headers like so:

``` cpp
#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here

#ifdef __cplusplus
}
#endif
```

What this accomplishes is that **it allows you to use that C header file with your C++ code**, because the macro `__cplusplus` will be defined. But you can also still use it with your legacy C code, where the macro is NOT defined, so it won't see the uniquely C++ construct.

Although, I have also seen C++ code such as:

``` cpp
extern "C" {
#include "legacy_C_header.h"
}
```

which I imagine accomplishes much the same thing.

Not sure which way is better, but I have seen both.


## Answer3: Decompile a g++ generated binary to see what is going on

``` cpp
// main.cpp

void f() {}
void g();

extern "C" {
    void ef() {}
    void eg();
}

/* Prevent g and eg from being optimized away. */
void h() { g(); eg(); }
```

Compile and disassemble the generated [ELF](https://stackoverflow.com/questions/26294034/how-to-make-an-executable-elf-file-in-linux-using-a-hex-editor/30648229#30648229) output:

```
g++ -c -std=c++11 -Wall -Wextra -pedantic -o main.o main.cpp
readelf -s main.o
```

The output contains:

![extern_c](/assets/images/202501/extern_c.png)

We see that:

* `ef` and `eg` were stored in symbols with the same name as in the code
* the other symbols were mangled. Let's unmangle them:

![extern_c2](/assets/images/202501/extern_c2.png)

**Conclusion**: both of the following symbol types were **not mangled**:

* defined
* declared but undefined (Ndx = UND), to be provided at link or run time from another object file

So you will need `extern "C"` both when calling:

* C from C++: tell g++ to expect unmangled symbols produced by gcc
* C++ from C: tell g++ to generate unmangled symbols for gcc to use

> Things that do not work in extern C

It becomes obvious that any C++ feature that requires name mangling will not work inside `extern C`:

``` cpp
extern "C" {
    // Overloading.
    // error: declaration of C function ‘void f(int)’ conflicts with
    void f();
    void f(int i);

    // Templates.
    // error: template with C linkage
    template <class C> void f(C i) { }
}
```

> Minimal runnable C from C++ example

For the sake of completeness and for the newbs out there, see also: [How to use C source files in a C++ project?](https://stackoverflow.com/questions/13694605/how-to-use-c-source-files-in-a-c-project/51912672#51912672)

Calling C from C++ is pretty easy: each C function only has one possible non-mangled symbol, so no extra work is required.

``` cpp
// main.cpp

#include <cassert>

#include "c.h"

int main() {
    assert(f() == 1);
}
```

``` cpp
// c.h

#ifndef C_H
#define C_H

/* This ifdef allows the header to be used from both C and C++
 * because C does not know what this extern "C" thing is. */
#ifdef __cplusplus
extern "C" {
#endif
int f();
#ifdef __cplusplus
}
#endif

#endif
```

``` cpp
// c.c

#include "c.h"

int f(void) { return 1; }
```

Run:

```
g++ -c -o main.o -std=c++98 main.cpp
gcc -c -o c.o -std=c89 c.c
g++ -o main.out main.o c.o
./main.out
```

Without `extern "C"` the link fails with:

```
main.cpp:6: undefined reference to `f()'
```

because g++ expects to find a mangled `f`, which gcc did not produce.

example: https://github.com/cirosantilli/cpp-cheat/tree/bf5f48628d0b01ba6a3fcea6f1162b28539654c9/c-from-cpp

> Minimal runnable C++ from C example

Calling C++ from C is a bit harder: we have to manually create non-mangled versions of each function we want to expose.

Here we illustrate how to expose C++ function overloads to C.

``` cpp
// main.c

#include <assert.h>

#include "cpp.h"

int main(void) {
    assert(f_int(1) == 2);
    assert(f_float(1.0) == 3);
    return 0;
}
```

``` cpp
// cpp.h

#ifndef CPP_H
#define CPP_H

#ifdef __cplusplus
// C cannot see these overloaded prototypes, or else it would get confused.
int f(int i);
int f(float i);
extern "C" {
#endif
int f_int(int i);
int f_float(float i);
#ifdef __cplusplus
}
#endif

#endif
```

``` cpp
// cpp.cpp

#include "cpp.h"

int f(int i) {
    return i + 1;
}

int f(float i) {
    return i + 2;
}

int f_int(int i) {
    return f(i);
}

int f_float(float i) {
    return f(i);
}
```

Run:

```
gcc -c -o main.o -std=c89 -Wextra main.c
g++ -c -o cpp.o -std=c++98 cpp.cpp
g++ -o main.out main.o cpp.o
./main.out
```

Without `extern "C"` it fails with:

```
main.c:6: undefined reference to `f_int'
main.c:7: undefined reference to `f_float'
```

because g++ generated mangled symbols which gcc cannot find.


example: https://github.com/cirosantilli/cpp-cheat/tree/bf5f48628d0b01ba6a3fcea6f1162b28539654c9/cpp-from-c






# More about calling convention and the difference between compilers

Refer to this article: http://www.agner.org/optimize/calling_conventions.pdf It tells you much more about calling convention and the difference between compilers.



# Refer

* [Calling conventions for different C++ compilers and operating systems](https://www.agner.org/optimize/calling_conventions.pdf)
* https://en.wikipedia.org/wiki/Calling_convention
* https://en.wikipedia.org/wiki/X86_calling_conventions