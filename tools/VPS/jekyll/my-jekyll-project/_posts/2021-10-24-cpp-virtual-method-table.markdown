---
layout: post
title:  "CPP Virtual Method Table"
date:   2021-10-24 12:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}


# 问题

``` cpp
#include <iostream>

class IBaseA
{
public:
    virtual void CallA() = 0;
};

class IBaseB
{
public:
    virtual void CallB() = 0;
};

// 继承 A 和 B
class DerivedAB : public IBaseA, public IBaseB
{
public:
    void CallA() override
    {
        std::cout << "Call A" << std::endl;
    }
    void CallB() override
    {
        std::cout << "Call B" << std::endl;
    }
};

int main()
{
    void* p = new DerivedAB();
    auto* pBaseB = static_cast<IBaseB*>(p);
    pBaseB->CallB();  // error, Call A

    IBaseB* p2 = new DerivedAB();
    p2->CallB(); // ok, Call B

    return 0;
}
```


# Example

``` cpp
#include <iostream>

class A
{
public:
    virtual void f() { std::cout << "A::f\n"; }
};

class B : public A
{
public:
    void f() override { std::cout << "B::f\n"; }
};

void f2(A& a)
{
    a.f();
}

int main()
{
    A* a = new B();
    a->f();  // B::f

    B b;
    f2(b);  // B::f
}
```

``` cpp
#include <iostream>
#include <stdint.h>

class A
{
public:
    virtual void f() { printf("A::f()\n"); }
};

class B : public A
{
protected:
    void f() override { printf("B::f()\n"); }
};

class C : public A
{
protected:
    void f() override { printf("C::f()\n"); }
};

int main()
{
    A* pa = new B;
    pa->f();
    printf("pa(%p)\n", pa);

    A* pa2 = new C;
    pa2->f();
    printf("pa2(%p)\n", pa2);

    const uint8_t* ptr = (const uint8_t*)(const void*)pa;
    printf("ptr(%p)\n", ptr);
}
/*
B::f()
pa(0x21f5f40)
C::f()
pa2(0x21f6f70)
ptr(0x21f5f40)
*/
```

# Virtual Tables

> A virtual method table (VMT) is a mechanism used in a programming language to support dynamic dispatch. – [Wikipedia](https://en.wikipedia.org/wiki/Virtual_method_table)

https://pabloariasal.github.io/2017/06/10/understanding-virtual-tables/


# const_cast/dynamic_cast/static_cast/reinterpret_cast/std::dynamic_pointer_cast

* [When should static_cast, dynamic_cast, const_cast and reinterpret_cast be used?](https://stackoverflow.com/questions/332030/when-should-static-cast-dynamic-cast-const-cast-and-reinterpret-cast-be-used)

## [std::dynamic_pointer_cast](http://www.cplusplus.com/reference/memory/dynamic_pointer_cast/)

Dynamic cast of `shared_ptr`. Returns a copy of `sp` of the proper type with its stored pointer casted dynamically from `U*` to `T*`.

``` cpp
template <class T, class U>
shared_ptr<T> dynamic_pointer_cast (const shared_ptr<U>& sp) noexcept;
```

``` cpp
#include <iostream>
#include <memory>

struct A {
  static const char* static_type;
  const char* dynamic_type;
  A() { dynamic_type = static_type; }
};

struct B: public A {
  static const char* static_type;
  B() { dynamic_type = static_type; }
};

const char* A::static_type = "class A";
const char* B::static_type = "class B";

int main () {
  std::shared_ptr<A> foo;
  std::shared_ptr<B> bar;

  bar = std::make_shared<B>();

  foo = std::dynamic_pointer_cast<A>(bar); // 类型转换 B -> A

  std::cout << "foo's static  type: " << foo->static_type << '\n';
  std::cout << "foo's dynamic type: " << foo->dynamic_type << '\n';
  std::cout << "bar's static  type: " << bar->static_type << '\n';
  std::cout << "bar's dynamic type: " << bar->dynamic_type << '\n';
}
/*
foo's static  type: class A
foo's dynamic type: class B
bar's static  type: class B
bar's dynamic type: class B
*/
```

`std::dynamic_pointer_cast` 是 C++ 标准库中的一个函数模板，用于在运行时执行类型安全的指针转换。它主要用于将 std::shared_ptr 指向的基类对象转换为派生类对象的指针。

当我们使用智能指针（如 std::shared_ptr）管理具有多态性的类层次结构时，可能需要在基类和派生类之间进行类型转换。std::dynamic_pointer_cast 提供了一种类型安全的方法来执行这些转换。

std::dynamic_pointer_cast 的作用如下：

1. 进行类型安全的指针转换：std::dynamic_pointer_cast 在运行时检查转换是否有效。如果转换失败（即指向的对象不是目标类型的实例），它将返回一个空的 std::shared_ptr。这有助于避免潜在的错误，并确保类型安全。
2. 管理引用计数：与 std::static_pointer_cast 和 std::const_pointer_cast 类似，std::dynamic_pointer_cast 也会正确地处理 std::shared_ptr 的引用计数。这意味着在转换过程中，原始智能指针和转换后的智能指针都将共享相同的引用计数，从而确保资源的正确管理。

``` cpp
#include <iostream>
#include <memory>

class Base {
public:
    virtual ~Base() {}
};

class Derived : public Base {
public:
    void DerivedSpecificFunction() {
        std::cout << "Derived specific function called." << std::endl;
    }
};

int main() {
    std::shared_ptr<Base> base_ptr = std::make_shared<Derived>();

    // 使用 std::dynamic_pointer_cast 进行类型安全的转换
    std::shared_ptr<Derived> derived_ptr = std::dynamic_pointer_cast<Derived>(base_ptr);

    if (derived_ptr) {
        derived_ptr->DerivedSpecificFunction();
    } else {
        std::cout << "Conversion failed." << std::endl;
    }

    return 0;
}
/*
Derived specific function called.
*/
```

## [dynamic_cast](https://en.cppreference.com/w/cpp/language/dynamic_cast)

`dynamic_cast` 是面向对象语言中常被称为类似“**是某种**”的概念的 C++ 版本：

``` cpp
void do_something(Shape* p)
{
    if (Circle* pc = dynamic_cast<Circle*>(p)) { // p 是某种 Circle？
        // ... 使用 pc 指向的 Circle ...
    }
    else {
        // ... 不是 Circle，做其他事情 ...
    }
}
```

`dynamic_cast` 是一个运行期操作，依赖于存储在 `Shape` 的虚拟函数表中的数据。它通用、易用，并且与其他语言类似的功能一样高效。然而，`dynamic_cast` 变得非常不受欢迎，因为它的实现往往是复杂的，特殊情况下手动编码可能更高效（可以说这导致 `dynamic_cast` **违反了零开销原则**）。

一种更简单的变种是使用引用而不是指针：

``` cpp
void do_something2(Shape& r)
{
    Circle& rc = dynamic_cast<Circle&>(r);  // r 是某种 Circle！
    // ... 使用 rc 引用的 Circle ...
}
```

这简单地断言 `r` 指代一个 `Circle`，如果不是则抛出一个异常。


----------------------------------------------------------------

Safely converts pointers and references to classes `up`, `down`, and sideways along the inheritance hierarchy.

```
A -> V
B -> V
D -> A, B

D d;       // the most derived object
A& a = d;  // upcast, dynamic_cast may be used, but unnecessary  子类 -> 父类

D& new_d = dynamic_cast<D&>(a); // downcast 父类 -> 子类

B& new_b = dynamic_cast<B&>(a); // sidecast  父类 -> 子类（另一个）
```

Syntax:

`dynamic_cast < new-type > ( expression )`

Notes:

* A downcast can also be performed with `static_cast`, which avoids the cost of the runtime check, but it's only safe if the program can guarantee (through some other logic) that the object pointed to by expression is definitely Derived.
* Some forms of `dynamic_cast` rely on [runtime type identification (RTTI)](https://en.wikipedia.org/wiki/Run-time_type_information), that is, information about each polymorphic class in the compiled program. Compilers typically have options to disable the inclusion of this information.

``` cpp
#include <iostream>

struct V {
    virtual void f() {}  // must be polymorphic to use runtime-checked dynamic_cast
};
struct A : virtual V {};
struct B : virtual V {
  B(V* v, A* a) {
    // casts during construction (see the call in the constructor of D below)
    dynamic_cast<B*>(v); // well-defined: v of type V*, V base of B, results in B*
    dynamic_cast<B*>(a); // undefined behavior: a has type A*, A not a base of B
  }
};
struct D : A, B {
    D() : B(static_cast<A*>(this), this) { }
};

struct Base {
    virtual ~Base() {}
};

struct Derived: Base {
    virtual void name() {}
};

int main()
{
    D d; // the most derived object
    A& a = d; // upcast, dynamic_cast may be used, but unnecessary
    [[maybe_unused]]
    D& new_d = dynamic_cast<D&>(a); // downcast
    [[maybe_unused]]
    B& new_b = dynamic_cast<B&>(a); // sidecast


    Base* b1 = new Base;
    if(Derived* d = dynamic_cast<Derived*>(b1))   // error
    {
        std::cout << "downcast from b1 to d successful\n";
        d->name(); // safe to call
    }

    Base* b2 = new Derived;
    if(Derived* d = dynamic_cast<Derived*>(b2))    // ok
    {
        std::cout << "downcast from b2 to d successful\n";
        d->name(); // safe to call
    }

    delete b1;
    delete b2;
}
/*
downcast from b2 to d successful
*/
```

``` cpp
#include <iostream>
#include <memory>

class foo
{
public:
     foo() { std::cout << "foo()\n"; }
     virtual ~foo() { std::cout << "~foo()\n"; }
     virtual void f() {
        std::cout << "foo::f()\n";
    }
};

class bar
{
public:
   bar() { std::cout << "bar()\n"; }
   virtual ~bar() { std::cout << "~bar()\n"; }
   void f()  {
        std::cout << "bar::f()\n";
   }

   int a;
};

class xyz : public foo, public bar
{
public:
   xyz() { std::cout << "xyz()\n"; }
   virtual ~xyz() { std::cout << "~xyz()\n"; }
   void f()  {
        std::cout << "xyz::f()\n";
    }

   int b;
};

int main(int argc, char**argv)
{
    bar* obj = new xyz();
    std::cout << "bar obj:" << obj << std::endl;
    obj->f();

    auto p1 = dynamic_cast<xyz*>(obj);   // ok
    std::cout << "xyz obj:" << p1 << std::endl;
    p1->f();

    auto p2 = static_cast<xyz*>(obj);    // ok
    std::cout << "xyz obj:" << p2 << std::endl;
    p2->f();

    auto p3 = reinterpret_cast<xyz*>(obj);        // error
    std::cout << "xyz obj:" << p3 << std::endl;
    //p3->f(); // error

    delete obj;

}
/*
foo()
bar()
xyz()
bar obj:0x1757018
bar::f()
xyz obj:0x1757010
xyz::f()
xyz obj:0x1757010
xyz::f()
xyz obj:0x1757018
~xyz()
~bar()
~foo()
*/
```

``` cpp
#include <iostream>
#include <map>
#include <memory>

class A
{
public:
    A() { std::cout << "A::A()\n"; }
    virtual ~A() { std::cout << "~A::A()\n"; }
    virtual void f() { std::cout << "A::f()\n"; }
    int a;
};

class A2
{
public:
    A2() { std::cout << "A2::A2()\n"; }
    virtual ~A2() { std::cout << "~A2::A2()\n"; }
    virtual void f() { std::cout << "A2::f()\n"; }
    int a2;
};

class B : public A, public A2
{
public:
    B() { std::cout << "B::B()\n"; }
    virtual ~B() { std::cout << "~B::B()\n"; }
    void f() override { std::cout << "B::f()\n"; }
    int b;
};

struct NoSensePlaceHolder {};
constexpr static NoSensePlaceHolder NoSenseHolder{};

class Buffer
{
public:
    explicit Buffer(NoSensePlaceHolder holder) : m_size(0), m_mem_ptr(nullptr) { std::cout << "Buffer(NoSensePlaceHolder holder)\n";  }
    virtual ~Buffer() { std::cout << "~Buffer()\n"; }

    int m_size;
    char* m_mem_ptr;
};
using BufferPtr = std::shared_ptr<Buffer>;


template <typename ObjType>
class ObjBuffer : public Buffer
{
public:
    ObjBuffer() : Buffer(NoSenseHolder) {
        std::cout << "ObjBuffer()\n";
        m_mem_ptr = reinterpret_cast<char*>(new ObjType());
    }

    explicit ObjBuffer(const std::shared_ptr<ObjType>& ref) : Buffer(NoSenseHolder) {
        std::cout << "ObjBuffer(const std::shared_ptr<ObjType>& ref)\n";
        m_obj_ptr_ref = ref;
        m_mem_ptr = reinterpret_cast<char*>(ref.get());
    }

    virtual ~ObjBuffer() { std::cout << "~ObjBuffer()\n"; }

    ObjType* Cast() { return reinterpret_cast<ObjType*>(m_mem_ptr); }

protected:
    std::shared_ptr<ObjType> m_obj_ptr_ref;

};

// B -> A, A2
// ObjBuffer -> Buffer
int main()
{
    auto bPtr = std::make_shared<B>();
    std::cout << "bPtr: " << bPtr.get() << std::endl;
    bPtr->f();

    // 类型转换 B --> ObjBuffer<B> --> Buffer
    BufferPtr buff(new ObjBuffer<B>(bPtr));

    // 类型转换 Buffer --> ObjBuffer<>

    // dynamic_cast
    //auto objbuff = dynamic_cast<ObjBuffer<A>*>(buff.get());     // error, type Buffer != ObjBuffer<A>
    //auto objbuff = dynamic_cast<ObjBuffer<A2>*>(buff.get());    // error, type Buffer != ObjBuffer<A2>
    //auto objbuff = dynamic_cast<ObjBuffer<B>*>(buff.get());     // ok, type Buffer = ObjBuffer<B>

    // static_cast
    auto objbuff = static_cast<ObjBuffer<A>*>(buff.get());     // ok
    //auto objbuff = static_cast<ObjBuffer<A2>*>(buff.get());    // ok
    //auto objbuff = static_cast<ObjBuffer<B>*>(buff.get());     // ok

    // reinterpret_cast
    //auto objbuff = reinterpret_cast<ObjBuffer<A>*>(buff.get());   // 不安全
    //auto objbuff = reinterpret_cast<ObjBuffer<A2>*>(buff.get());  // 不安全
    //auto objbuff = reinterpret_cast<ObjBuffer<B>*>(buff.get());   // 不安全

    // c convert
    //auto objbuff = (ObjBuffer<A>*)(buff.get());    // ok
    //auto objbuff = (ObjBuffer<A2>*)(buff.get());   // ok
    //auto objbuff = (ObjBuffer<B>*)(buff.get());    // ok


    if (!objbuff) {
        std::cout << "objbuff is nullptr\n";
        return 1;
    }

    // 类型转换 ObjBuffer<> --> B
    auto origobj = objbuff->Cast();
    std::cout << "origobj: " << origobj << std::endl;
    origobj->f();

}
```


# 虚继承

C++指出，当derived class对象经由一个base class指针被删除，而该base class带着一个non-virtual析构函数，其结果未定义 —— 实际执行时通常发生的是，对象的derived成分没被销毁。于是造成一个诡异的“局部销毁”对象，从而导致资源泄露。

> 观点1：任何class只要带有virtual函数，都几乎确定应该也有一个virtual析构函数。

> 观点2：如果class不含virtual函数，通常表示它并不意图被用做一个base class。当class不企图被当做base class，令其析构函数为virtual往往是一个馊主意。因为，欲实现出virtual函数，对象必须携带某些信息，主要用来在运行期决定哪一个virtual函数该被调用。这份信息通常是由一个所谓vptr(virtual table pointer)指出，vptr指向一个由函数指针构成的数组，称为vtbl(virtual table)。每一个带有virtual函数的class都有一个相应的vtbl。当对象调用某一virtual函数，实际被调用的函数取决于该对象的vptr所指的那个vtbl（编译器在其中寻找合适的函数指针）。这样，如果base class内含virtual函数，那么其对象的体积会增加，在32-bits计算机体系结构中将多占用32bits（vptr大小）；而在64-bits计算机体系结构中多占用64bits（指针大小为8字节）。

> 观点3：标准库string不含任何virtual函数，但有时程序员会错误地把它当做base class。那么，当你在程序任意某处无意间将一个pointer-to-specialstring转换为一个pointer-to-string，然后将转换所得的那个string指针delete掉，则立刻被流放到”不明确行为上”。很不幸C++目前没有提供类似Java的final classes禁止派生的机制。

> 请记住
> 1. 从里向外构造（ctor），从外向里析构（dtor）
> 2. polymorphic (带多态性质) base classes应该声明一个virtual析构函数。如果class带有任何virtual函数，它就应该拥有一个virtual析构函数。
> 3. classes的设计目的如果不是作为base classes使用，或不是为了具备多态性使用，此class就不该声明virtual析构函数。

测试代码：

继承和组合：

``` cpp
#include <iostream>

class A1
{
public:
    A1() {std::cout << "A1()\n"; m_a1 = new int(1); }
    ~A1() {std::cout << "~A1()\n"; delete m_a1;}

private:
    int* m_a1;
};

class A2
{
public:
    A2() {std::cout << "A2()\n"; m_a2 = new int(1); }
    virtual ~A2() {std::cout << "~A2()\n"; delete m_a2;}

private:
    int* m_a2;
};

class B : public A2
{
public:
    B() {std::cout << "B()\n"; m_a = new int(1); }
    ~B() {std::cout << "~B()\n"; delete m_a;}

private:
    int* m_a;
    A1 m_obj;
};

int main()
{
    A2* a = new B();
    delete a;      // 当A2的析构函数为virtual类型时, 此时delete才会调用B的析构函数
}
/*
A1()
A2()
B()
~B()
~A2()
~A1()
*/
```

``` cpp
#include <iostream>

class A1
{
public:
    A1() {std::cout << "A1()\n"; m_a1 = new int(1); }
    virtual ~A1() {std::cout << "~A1()\n"; delete m_a1; }

private:
    int* m_a1;
};

class A2 : A1
{
public:
    A2() {std::cout << "A2()\n"; m_a2 = new int(1); }
    virtual ~A2() {std::cout << "~A2()\n"; delete m_a2;}

private:
    int* m_a2;
};

class B : public A2
{
public:
    B() {std::cout << "B()\n"; m_a = new int(1); }
    ~B() {std::cout << "~B()\n"; delete m_a;}

private:
    int* m_a;
};

int main()
{
    A2* a = new B(); // A1或A2有一个析构函数为virtual才会调用B的析构函数
    delete a;
}
/*
A1()
A2()
B()
~B()
~A2()
~A1()
*/
```

多重继承：

``` cpp
#include <iostream>

class A1
{
public:
    A1() {std::cout << "A1()\n"; m_a1 = new int(1); }
    ~A1() {std::cout << "~A1()\n"; delete m_a1;}

private:
    int* m_a1;
};

class A2
{
public:
    A2() {std::cout << "A2()\n"; m_a2 = new int(1); }
    virtual ~A2() {std::cout << "~A2()\n"; delete m_a2;}

private:
    int* m_a2;
};

class B : public A1, public A2
{
public:
    B() {std::cout << "B()\n"; m_a = new int(1); }
    ~B() {std::cout << "~B()\n"; delete m_a;}

private:
    int* m_a;
};

int main()
{
    A2* a = new B();
    delete a;   // A2的析构函数需为virtual
}
/*
A1()
A2()
B()
~B()
~A2()
~A1()
*/
```

# virtual 函数的调用开销

参考：http://ithare.com/infographics-operation-costs-in-cpu-clock-cycles/

普通函数调用大概耗费 15-30 时钟周期， virtual function 调用大概是 30-60 时钟周期，跟 L3 cache 访问的耗费差不多，内存访问大概是 100-150 时钟周期。

> If you call your virtual function 100K times per second on a 3GHz CPU – it probably won’t cost you more than 0.2% of your CPU total; however, if you’re calling the same virtual function 10M times per second – it can easily mean that virtualisation eats up double-digit percentages of your CPU core.

参考：[2006 年的 C++ 标准文档 "Technical Report on C++ Performance"](https://www.open-std.org/jtc1/sc22/wg21/docs/TR18015.pdf)

其中的 5.3.3 节给出了 Virtual Functions 虚函数这个语言特性的性能 overhead






# Refer

* [What is a non-virtual thunk?](https://reverseengineering.stackexchange.com/questions/4543/what-is-a-non-virtual-thunk)

