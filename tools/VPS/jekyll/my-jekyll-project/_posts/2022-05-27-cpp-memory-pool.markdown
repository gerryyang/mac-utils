---
layout: post
title:  "CPP Memory Pool"
date:   2022-05-27 12:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# 问题描述

``` cpp
#include <iostream>

class A
{
public:
    A() { std::cout << "A()\n"; }
    virtual ~A() { std::cout << "~A()\n"; }
    int a[5];
};

class B
{
public:
    B() { std::cout << "B()\n"; }
    virtual ~B() { std::cout << "~B()\n"; }
    int b;
};

class C : public A, public B
{
public:
    C() { std::cout << "C()\n"; }
    ~C() { std::cout << "~C()\n"; }
    int c;
};

int main()
{
    C* pC = new C();
    std::cout << "pC: " << pC << std::endl;

    A* pA = pC;
    std::cout << "pA: " << pA << std::endl;

    B* pB = pC;
    std::cout << "pB: " << pB << std::endl;

    delete pB; // ok?
}
/*
A()
B()
C()
pC: 0x8fbeb0
pA: 0x8fbeb0
pB: 0x8fbed0
~C()
~B()
~A()
*/
```


# 通过基类指针释放对象

## 问题描述

``` cpp
class Base;
class Derived;

Base *b = MY_NEW(Derived);

MY_DELETE(b); // error, 释放的是基类的长度
MY_DELETE(Derived(b)); // ok，需要显式转换为子类类型
```

如何不需要指定类型转换，且保证底层回收的内存长度是正确的？

例子：显示调用子类 MY_DELETE_IGNORE_VIRTUAL(obj2)，即，Derived(b)的形式，可以不依赖虚函数

``` cpp
#include <iostream>
#include <type_traits>

#define MY_NEW(Type, ...) MyNew<Type>(__VA_ARGS__)
#define MY_DELETE(ptr) MyDelete(ptr)
#define MY_DELETE_IGNORE_VIRTUAL(ptr) MyDeleteIngoreVirtual(ptr)

template<bool HAS_VIRTUAL_DESTRUCTOR>
struct STDeleteInfo
{
    template<typename T>
    inline static void* Get(T* ptr, size_t& uSize)
    {
        //std::cout << "ptr: " << ptr << std::endl;
        return ptr->VirtualDeleteInfo(uSize);
    }
};

template<>
struct STDeleteInfo<false>
{
    template<typename T>
    inline static void* Get(T* ptr, size_t& uSize)
    {
        uSize = sizeof(T);
        return ptr;
    }
};

template<typename Type>
Type* MyNew()
{
    void* ptr = malloc(sizeof(Type));
    std::cout << "Allocate ptr: " << ptr << ", size: " << sizeof(Type) << std::endl;

    new (ptr) Type;
    return (Type*)ptr;
}

template<typename Type>
void MyDelete(Type* ptr)
{
    size_t uSize = 0;
    void* ptr2 = STDeleteInfo<std::has_virtual_destructor<Type>::value>::Get(ptr, uSize);

    ptr->~Type();

    std::cout << "1 Recycle ptr: " << ptr << ", size: " << sizeof(Type) << std::endl;
    std::cout << "2 Recycle ptr: " << ptr2 << ", size: " << uSize << std::endl;

    free(ptr2);
}

template<typename Type>
void MyDeleteIngoreVirtual(Type* ptr)
{
    ptr->~Type();
    std::cout << "Recycle ptr: " << ptr << ", size: " << sizeof(Type) << std::endl;
    free(ptr);
}

struct A
{
    A()
    {
        std::cout << "A()\n";
    }
    ~A()
    {
        std::cout << "~A()\n";
    }

    virtual void* VirtualDeleteInfo(size_t& uSize)
    {
        std::cout << "A::VirtualDeleteInfo\n";
        uSize = sizeof(*this);
        return (void*)this;
    }

    virtual void f()
    {
        std::cout << "A::f()\n";
    }

    int a1;
    int a2;
};

struct B
{
    B()
    {
        std::cout << "B()\n";
    }
    virtual ~B()
    {
        std::cout << "~B()\n";
    }

    virtual void* VirtualDeleteInfo(size_t& uSize)
    {
        std::cout << "B::VirtualDeleteInfo\n";
        uSize = sizeof(*this);
        return (void*)this;
    }

    virtual void f()
    {
        std::cout << "B::f()\n";
    }

    int b;
};

struct C
    : public A
    , public B
{
    C()
    {
        std::cout << "C()\n";
    }
    ~C()
    {
        std::cout << "~C()\n";
    }

    void* VirtualDeleteInfo(size_t& uSize) override
    {
        std::cout << "C::VirtualDeleteInfo\n";
        uSize = sizeof(*this);
        return (void*)this;
    }

    virtual void f()
    {
        std::cout << "C::f()\n";
    }

    int c;
};

int main()
{
    B* obj = MY_NEW(C);
    std::cout << "obj: " << obj << std::endl;

    auto obj2 = dynamic_cast<C*>(obj);
    std::cout << "obj2: " << obj2 << std::endl;
    MY_DELETE_IGNORE_VIRTUAL(obj2);  // 显示调用子类，可以不依赖虚函数
}
/*
Allocate ptr: 0x1d72a80, size: 32
A()
B()
C()
obj: 0x1d72a90
obj2: 0x1d72a80
~C()
~B()
~A()
Recycle ptr: 0x1d72a80, size: 32
*/
```

## 方案：通过分配额外的内存空间记录分配的内存大小

若申请 sizeof(Type)，则分配 sizeof(Extra) + sizeof(Type)。其中，pBase 指向分配的地址空间，pMem 指向业务申请的分配空间，在 pMem - pBase 之间的内存用于记录业务申请的内存长度。

此方案不可行。因为在多继承场景下，基类指针会发生偏移，无法根据子类的指针计算得到 pBase。例如：动态申请 C 的地址为 c，即 pBase == c。将 c 赋值给 b 后，b 的指针会发生偏移。若通过 MY_DELETE(b) 的方式，只能获取基类的大小，导致释放的内存空间不正确（即，传入的释放地址不正确）。

``` cpp
    // struct C : public A, public B
    C* c = new C;
    A* a = c;
    B* b = c;

    std::cout << "a: " << a << std::endl;
    std::cout << "b: " << b << std::endl;
    std::cout << "c: " << c << std::endl;
```

参考 C++ 对象的内存布局：

* a: 0x15eaac0
* b: 0x15eaad0  若返回是 B* 的对象，则出现地址偏移，无法计算出 C* 时的地址
* c: 0x15eaac0

## 方案: 通过多态的方式

通过基类指针调用子类的`Get`函数获取子类的大小和地址。

``` cpp
#include <iostream>
#include <type_traits>

#define MY_NEW(Type, ...) MyNew<Type>(__VA_ARGS__)
#define MY_DELETE(ptr) MyDelete(ptr)

template<bool HAS_VIRTUAL_DESTRUCTOR>
struct STDeleteInfo
{
    template<typename T>
    inline static void* Get(T* ptr, size_t& uSize)
    {
        //std::cout << "ptr: " << ptr << std::endl;
        return ptr->VirtualDeleteInfo(uSize);
    }
};

template<>
struct STDeleteInfo<false>
{
    template<typename T>
    inline static void* Get(T* ptr, size_t& uSize)
    {
        uSize = sizeof(T);
        return ptr;
    }
};

template<typename Type>
Type* MyNew()
{
    void* ptr = malloc(sizeof(Type));
    std::cout << "Allocate ptr: " << ptr << ", size: " << sizeof(Type) << std::endl;

    new (ptr) Type;
    return (Type*)ptr;
}

template<typename Type>
void MyDelete(Type* ptr)
{
    size_t uSize = 0;
    void* ptr2 = STDeleteInfo<std::has_virtual_destructor<Type>::value>::Get(ptr, uSize);

    ptr->~Type();

    std::cout << "1 Recycle ptr: " << ptr << ", size: " << sizeof(Type) << std::endl;
    std::cout << "2 Recycle ptr: " << ptr2 << ", size: " << uSize << std::endl;

    free(ptr2);
}

struct A
{
    A()
    {
        std::cout << "A()\n";
    }
    virtual ~A()
    {
        std::cout << "~A()\n";
    }

    virtual void* VirtualDeleteInfo(size_t& uSize)
    {
        std::cout << "A::VirtualDeleteInfo\n";
        uSize = sizeof(*this);
        return (void*)this;
    }

    virtual void f()
    {
        std::cout << "A::f()\n";
    }

    int a1;
    int a2;
};

struct B : public A
{
    B()
    {
        std::cout << "B()\n";
    }
    ~B()
    {
        std::cout << "~B()\n";
    }

    void* VirtualDeleteInfo(size_t& uSize) override
    {
        std::cout << "B::VirtualDeleteInfo\n";
        uSize = sizeof(*this);
        return (void*)this;
    }

    virtual void f()
    {
        std::cout << "B::f()\n";
    }

    int b;
};

int main()
{
    A* obj = MY_NEW(B);
    obj->f();
    MY_DELETE(obj);
}

/*
Allocate ptr: 0x11468f0, size: 24
A()
B()
B::f()
B::VirtualDeleteInfo
~B()
~A()
1 Recycle ptr: 0x11468f0, size: 16
2 Recycle ptr: 0x11468f0, size: 24
*/
```

多继承场景：C -> A, B

``` cpp
#include <iostream>
#include <type_traits>

#define MY_NEW(Type, ...) MyNew<Type>(__VA_ARGS__)
#define MY_DELETE(ptr) MyDelete(ptr)

template<bool HAS_VIRTUAL_DESTRUCTOR>
struct STDeleteInfo
{
    template<typename T>
    inline static void* Get(T* ptr, size_t& uSize)
    {
        //std::cout << "ptr: " << ptr << std::endl;
        return ptr->VirtualDeleteInfo(uSize);
    }
};

template<>
struct STDeleteInfo<false>
{
    template<typename T>
    inline static void* Get(T* ptr, size_t& uSize)
    {
        uSize = sizeof(T);
        return ptr;
    }
};

template<typename Type>
Type* MyNew()
{
    void* ptr = malloc(sizeof(Type));
    std::cout << "Allocate ptr: " << ptr << ", size: " << sizeof(Type) << std::endl;

    new (ptr) Type;
    return (Type*)ptr;
}

template<typename Type>
void MyDelete(Type* ptr)
{
    size_t uSize = 0;
    void* ptr2 = STDeleteInfo<std::has_virtual_destructor<Type>::value>::Get(ptr, uSize);

    ptr->~Type();

    std::cout << "1 Recycle ptr: " << ptr << ", size: " << sizeof(Type) << std::endl;
    std::cout << "2 Recycle ptr: " << ptr2 << ", size: " << uSize << std::endl;

    free(ptr2);
}

struct A
{
    A()
    {
        std::cout << "A()\n";
    }
    virtual ~A()
    {
        std::cout << "~A()\n";
    }

    virtual void* VirtualDeleteInfo(size_t& uSize)
    {
        std::cout << "A::VirtualDeleteInfo\n";
        uSize = sizeof(*this);
        return (void*)this;
    }

    virtual void f()
    {
        std::cout << "A::f()\n";
    }

    int a1;
    int a2;
};

struct B
{
    B()
    {
        std::cout << "B()\n";
    }
    virtual ~B()
    {
        std::cout << "~B()\n";
    }

    virtual void* VirtualDeleteInfo(size_t& uSize)
    {
        std::cout << "B::VirtualDeleteInfo\n";
        uSize = sizeof(*this);
        return (void*)this;
    }

    virtual void f()
    {
        std::cout << "B::f()\n";
    }

    int b;
};

struct C
    : public A
    , public B
{
    C()
    {
        std::cout << "C()\n";
    }
    ~C()
    {
        std::cout << "~C()\n";
    }

    void* VirtualDeleteInfo(size_t& uSize) override
    {
        std::cout << "C::VirtualDeleteInfo\n";
        uSize = sizeof(*this);
        return (void*)this;
    }

    virtual void f()
    {
        std::cout << "C::f()\n";
    }

    int c;
};

int main()
{
    B* obj = MY_NEW(C);
    std::cout << "obj: " << obj << std::endl;

    MY_DELETE(obj);
}
/*
Allocate ptr: 0x8eda80, size: 32
A()
B()
C()
obj: 0x8eda90
C::VirtualDeleteInfo
~C()
~B()
~A()
1 Recycle ptr: 0x8eda90, size: 16
2 Recycle ptr: 0x8eda80, size: 32
*/
```

此方案需要考虑一个问题，如何兼容成员函数`VirtualDeleteInfo`没有定义的情况？

解决方法：编译器检查。通过`SFINAE`模版推导的方式，检查类中是否定义了某个成员函数，若存在函数定义，则可以支持业务直接传父类指针进行销毁；若不存在函数定义，则忽略函数调用，由分配器底层进行校验检查。

``` cpp
#include <iostream>
#include <type_traits>

struct A {
    A() { std::cout << "A()\n"; }
    virtual ~A() { std::cout << "~A()\n"; }

    virtual void f() { std::cout << "A::f()\n"; }

    int a1;
    int a2;
};

struct B : public A {
    B() { std::cout << "B()\n"; }
    ~B() { std::cout << "~B()\n"; }

    //virtual void f() { std::cout << "B::f()\n"; }

    int b;
};

struct C : public B {
    C() { std::cout << "C()\n"; }
    ~C() { std::cout << "~C()\n"; }

    virtual void f() { std::cout << "C::f()\n"; }

    int c;
};

template<class T>
struct has_member_f {
  template<class U, void (U::*)()>
  struct sfinae;

  template<class U>
  static char test(sfinae<U, &U::f>* unused);

  template<class>
  static int test(...);

  static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(char);
};

struct foo {
  void f() {}
};

int main()
{
    std::cout << has_member_f<foo>::value << std::endl;
    std::cout << has_member_f<int>::value << std::endl;

    std::cout << has_member_f<A>::value << std::endl;
    std::cout << has_member_f<B>::value << std::endl;
    std::cout << has_member_f<C>::value << std::endl;
}
```




## 方案: 通过基类指针调用析构时，获取子类大小和地址

TODO


# Refer











