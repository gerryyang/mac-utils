---
layout: post
title:  "CPP Template in Action"
date:   2021-10-07 21:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Prepare

* 编译器对标准的支持情况：https://en.cppreference.com/w/cpp/compiler_support
* 模版实例化工具：https://cppinsights.io/
* 编译运行工具：https://wandbox.org/

查看当前环境C++版本：

```
$ ls -l /lib64/libstdc++.so.6
lrwxrwxrwx 1 root root 19 Aug 18  2020 /lib64/libstdc++.so.6 -> libstdc++.so.6.0.25
$ rpm -qf /lib64/libstdc++.so.6
libstdc++-8.3.1-5.el8.0.2.x86_64
```

# Basic

* 模板机制为 C++ 提供了泛型编程的方式，在减少代码冗余的同时仍然可以提供类型安全。
* 特化必须在同一命名空间下进行，可以特化**类模板**也可以特化**函数模板**，但**类模板可以偏特化和全特化**，而**函数模板只能全特化**。
* 模板实例化时会优先匹配"模板参数"最相符的那个特化版本。
* C++ 的模板机制被证明是图灵完备的，即可以通过模板元编程（`Template Metaprogramming，TMP`）的方式在编译期做任何计算。

## 模版的定义

在类模版/函数模板定义之前，声明模板参数列表。

``` cpp
// 类模板
template <class T1, class T2>
class A
{
    T1 data1;
    T2 data2;
};

// 函数模板
template <class T>
T max(const T lhs, const T rhs)
{
    return lhs > rhs ? lhs : rhs;
}
```

## Explicit (full) template specialization

**Explicit specialization** may be declared in any scope where its **primary template** may be defined (which may be different from the scope where the primary template is defined; such as with out-of-class specialization of a member template) . **Explicit specialization has to appear after the non-specialized template declaration.** (特化版本可以声明在主模版作用域之外，特化版本的声明必须出现在非实例化模版声明之后)

``` cpp
namespace N {
    template<class T> class X { /*...*/ }; // primary template
    template<> class X<int> { /*...*/ }; // specialization in same namespace

    template<class T> class Y { /*...*/ }; // primary template
    template<> class Y<double>; // forward declare specialization for double
}

template<>
class N::Y<double> { /*...*/ }; // OK: specialization in same namespace
```

Specialization must be declared before the first use that would cause implicit instantiation, in every translation unit where such use occurs: (在每一个翻译单元若要使用特化版本，则需要在第一次使用时先对其声明，否则会导致隐式的实例化)

``` cpp
class String {};
template<class T> class Array { /*...*/ };
template<class T> void sort(Array<T>& v) { /*...*/ } // primary template

void f(Array<String>& v) {
    sort(v); // implicitly instantiates sort(Array<String>&),
}            // using the primary template for sort()

template<>  // ERROR: explicit specialization of sort(Array<String>)
void sort<String>(Array<String>& v); // after implicit instantiation
```

A template specialization that was declared but not defined can be used just like any other [incomplete type](https://en.cppreference.com/w/cpp/language/incomplete_type) (e.g. pointers and references to it may be used)

``` cpp
template<class T> class X; // primary template
template<> class X<int>; // specialization (declared, not defined)

X<int>* p; // OK: pointer to incomplete type
X<int> x; // error: object of incomplete type
```


## Explicit specializations of function templates

When specializing a function template, its template arguments can be omitted if [template argument deduction](https://en.cppreference.com/w/cpp/language/template_argument_deduction) can provide them from the function arguments: (函数模版实例化时，通过对函数参数的推导，函数模版参数可以省略)

``` cpp
template<class T> class Array { /*...*/ };
template<class T> void sort(Array<T>& v); // primary template
template<> void sort(Array<int>&); // specialization for T = int

// no need to write
// template<> void sort<int>(Array<int>&);
```

## Members of specializations

When defining a member of an explicitly specialized class template outside the body of the class, the syntax `template <>` is not used, except if it's a member of an explicitly specialized member class template, which is specialized as a class template, because otherwise, the syntax would require such definition to begin with `template<parameters>` required by the nested template.

``` cpp
template< typename T>
struct A {
    struct B {};  // member class
    template<class U> struct C { }; // member class template
};

template<> // specialization
struct A<int> {
    void f(int); // member function of a specialization
};
// template<> not used for a member of a specialization
void A<int>::f(int) { /* ... */ }

template<> // specialization of a member class
struct A<char>::B {
    void f();
};
// template<> not used for a member of a specialized member class either
void A<char>::B::f() { /* ... */ }

template<> // specialization of a member class template
template<class U> struct A<char>::C {
    void f();
};

// template<> is used when defining a member of an explicitly
// specialized member class template specialized as a class template
template<>
template<class U> void A<char>::C<U>::f() { /* ... */ }
```

A member or a member template of a class template may be explicitly specialized for a given implicit instantiation of the class template, even if the member or member template is defined in the class template definition.

``` cpp
template<typename T>
struct A {
    void f(T); // member, declared in the primary template
    void h(T) {} // member, defined in the primary template
    template<class X1> void g1(T, X1); // member template
    template<class X2> void g2(T, X2); // member template
};

// specialization of a member
template<> void A<int>::f(int);
// member specialization OK even if defined in-class
template<> void A<int>::h(int) {}

// out of class member template definition
template<class T>
template<class X1> void A<T>::g1(T, X1) { }

// member template specialization
template<>
template<class X1> void A<int>::g1(int, X1);

// member template specialization
template<>
template<> void A<int>::g2<char>(int, char); // for X2 = char
// same, using template argument deduction (X1 = char)
template<>
template<> void A<int>::g1(int, char);
```

Member or a member template may be nested within many enclosing class templates. In an explicit specialization for such a member, there's a `template<>` for every enclosing class template that is explicitly specialized.

``` cpp
template<class T1> struct A {
    template<class T2> struct B {
      template<class T3>
        void mf();
    };
};
template<> struct A<int>;
template<> template<> struct A<char>::B<double>;
template<> template<> template<> void A<char>::B<char>::mf<double>();
```



## 全特化

通过[全特化一个模板](https://en.cppreference.com/w/cpp/language/template_specialization)，可以对一个**特定参数集合**自定义当前模板(Allows customizing the template code for a given set of template arguments)，**类模板和函数模板都可以全特化**。 全特化的模板参数列表应当是空的，并且应当给出"模板实参"列表：

``` cpp
template <> declaration
```

``` cpp
// 全特化 类模板
template <>
class A<int, double>{
    int data1;
    double data2;
};

// 全特化 函数模板

// 注意，类模板的全特化时，在类名后给出了"模板实参"，但函数模板的全特化，函数名后没有给出"模板实参"。这是因为编译器根据 int max(const int, const int) 的函数签名可以推导出来它是 T max(const T, const T) 的特化
template <>
int max(const int lhs, const int rhs){
    return lhs > rhs ? lhs : rhs;
}
```

例外情况：函数模板不需指定"模板实参"是因为编译器可以通过函数签名来推导，但有时这一过程是有歧义的：

``` cpp
#include <iostream>

template <class T>
void f()
{
    T d;
    std::cout << "template <class T> void f()\n";
}

// 此时编译器不知道 f() 是从 f<T>() 特化来的，编译时会有错误，此时需要显式指定"模板实参"
#if 0
template <>
void f()
{
    int d;
    std::cout << "template <> void f()\n";
}
#endif

template <>
void f<int>()
{
    int d;
    std::cout << "template <> void f()\n";
}

int main()
{
    f<int>(); // template <> void f()
}
```

Any of the following can be fully specialized:

* [function template](https://en.cppreference.com/w/cpp/language/function_template)
* [class template](https://en.cppreference.com/w/cpp/language/class_template)
* (since C++14) [variable template](https://en.cppreference.com/w/cpp/language/variable_template)
* [member function](https://en.cppreference.com/w/cpp/language/member_functions) of a class template
* [static data member](https://en.cppreference.com/w/cpp/language/static) of a class template
* [member class](https://en.cppreference.com/w/cpp/language/nested_types) of a class template
* member [enumeration](https://en.cppreference.com/w/cpp/language/enum) of a class template
* [member class template](https://en.cppreference.com/w/cpp/language/member_template) of a class or class template
* [member function template](https://en.cppreference.com/w/cpp/language/member_template#Member_function_templates) of a class or class template

``` cpp
#include <iostream>
template<typename T>   // primary template
struct is_void : std::false_type
{
};

template<>  // explicit specialization for T = void
struct is_void<void> : std::true_type
{
};

int main()
{
    // for any type T other than void, the class is derived from false_type
    std::cout << is_void<char>::value << '\n';  // 0

    // but when T is void, the class is derived from true_type
    std::cout << is_void<void>::value << '\n'; // 1
}
```

* https://stackoverflow.com/questions/58694521/what-is-stdfalse-type-or-stdtrue-type

## 偏特化

* 函数模板不允许偏特化
* 类似于全特化，偏特化也是为了给自定义一个参数集合的模板，但偏特化后的模板需要进一步的实例化才能形成确定的签名
* 偏特化也是以`template`来声明的，需要给出剩余的"模板形参"和必要的"模板实参"

``` cpp
template <class T2>
class A<int, T2>{
    // ...
};
```

## 例子

### 类模版的特化版本

``` cpp
#include <iostream>
#include <type_traits>

template<typename T>
class A
{
public:
    static void f(T a);
};

// 类模版的特化版本
template<>
class A<int>
{
public:
    static void f(int a);
};

// 注意，不需要 template<> 语法
void A<int>::f(int a)
{
    std::cout << "A<int>::f(int a)\n";
}

int main()
{
    A<int>::f(1); // A<int>::f(int a)
}
```

### 类模版的成员函数特化版本

``` cpp
#include <iostream>
#include <type_traits>

template<typename T>
class A
{
public:
    static void f(T a);
};

// 默认版本
template<typename T>
void A<T>::f(T a)
{
    std::cout << "A<T>::f(T a)\n";
}

// 类模版的成员函数特化版本，需要 template<> 语法
template<>
void A<int>::f(int a)
{
    std::cout << "A<int>::f(int a)\n";
}

int main()
{
    A<int>::f(1); // A<int>::f(int a)
}
```

### 使用静态断言显式控制必须定义特化版本

``` cpp
#include <iostream>
#include <type_traits>

class PlaceHolder
{
};

template<typename T>
class A
{
public:
    static void f(T a);
};

template<typename T>
void A<T>::f(T a)
{
    std::cout << "A<T>::f(T a)\n";

    // 若对 primary template 展开，则执行静态断言错误
    static_assert(!std::is_class<T>::value, "should not use base specialization");
}

#if 1
template<>
class A<PlaceHolder>
{
public:
    static void f(PlaceHolder a);
};

void A<PlaceHolder>::f(PlaceHolder a)
{
    std::cout << "A<PlaceHolder>::f(PlaceHolder a)\n";

}
#endif

int main()
{
    PlaceHolder a;
    A<PlaceHolder>::f(a); // A<PlaceHolder>::f(PlaceHolder a)
}
```

``` cpp
#include <type_traits>

class PlaceHolder{};

template<typename T>
class A
{
public:
    A()
    {
        static_assert(!std::is_class<T>::value, "should not use base construct");
    }
};

#if 1
template<>
class A<PlaceHolder>
{
public:
    A()
    {
    }
};
#endif

int main()
{
    A<PlaceHolder> a;
    return 0;
}
```

# Variadic templates


* The **ellipsis** (`...`) operator has **two roles**.
    + When it occurs to the **left** of the name of a parameter, it declares a **parameter pack**. Using the parameter pack, the user can bind **zero or more arguments** to the variadic template parameters. Parameter packs can also be used for non-type parameters.
    + By contrast, when the ellipsis operator occurs to the **right** of a template or function call argument, **it unpacks the parameter packs into separate arguments**, like the `args...` in the body of `printf` below.
* In practice, the use of an **ellipsis** operator in the code causes the whole expression that precedes the ellipsis to be repeated for every subsequent argument unpacked from the argument pack, with the expressions separated by commas.
* The use of **variadic templates** is often **recursive**. The variadic parameters themselves are not readily available to the implementation of a function or class. Therefore, the typical mechanism for defining something like a C++11 variadic `printf` replacement would be as follows:

``` cpp
#include <iostream>
#include <vector>
#include <type_traits>

void func_impl(std::vector<std::string> &str_vec)
{
    for (auto& item: str_vec) {
        std::cout << item << " ";
    }
    std::cout << "\nTODO";
}

static void pack_helper(std::vector<std::string> &str_vec)
{
}

template<typename... Args>
void pack_helper(std::vector<std::string> &str_vec, const std::string& str, const Args&... args)
{
    str_vec.push_back(str);
    pack_helper(str_vec, args ...);
}

// recursive
template<typename... Args>
void func(const std::string& str, const Args& ... args)
{
    std::vector<std::string> str_vec;
    pack_helper(str_vec, str, args...);
    func_impl(str_vec);
}

int main()
{
    func("1", "2", "a");
}
```

* https://en.wikipedia.org/wiki/Variadic_template
* https://www.ibm.com/docs/en/zos/2.4.0?topic=only-variadic-templates-c11

## 数字转换为字符串

``` cpp
namespace detail
{
	template<uint8_t... digits> struct positive_to_chars {
		static const char value[];
		static constexpr size_t size = sizeof...(digits);
	};
	template<uint8_t... digits> const char positive_to_chars<digits...>::value[] = {('0' + digits)..., 0};

	template<uint8_t... digits> struct negative_to_chars {
		static const char value[];
	};
	template<uint8_t... digits> const char negative_to_chars<digits...>::value[] = {'-', ('0' + digits)..., 0};

	template<bool neg, uint8_t... digits>
		struct to_chars : positive_to_chars<digits...> {};

	template<uint8_t... digits>
		struct to_chars<true, digits...> : negative_to_chars<digits...> {};

	// 对 num 每位进行展开，例如，num = 123 则展开为 explode<neg, 0, 1, 2, 3>
	template<bool neg, uintmax_t rem, uint8_t... digits>
		struct explode : explode<neg, rem / 10, rem % 10, digits...> {};

	// 展开终止
	template<bool neg, uint8_t... digits>
		struct explode<neg, 0, digits...> : to_chars<neg, digits...> {};

	template<typename T>
		constexpr uintmax_t cabs(T num) {
			return (num < 0) ? -num : num;
		}
}

template<typename T, T num>
struct string_from : ::detail::explode<num < 0, ::detail::cabs(num)> {};

int main()
{
    auto str = string_from<unsigned, 1>::value;
}
```


# SFINAE

``` cpp
#include <iostream>

// has_member_gc用来判断一个类T是否定义了成员函数void __gc()
template<class T>
struct has_member_gc {
  // 声明辅助模板类sfinae，它接受两个模板参数：U以及U的成员函数指针常量（签名为void()）
  template<class U, void (U::*)()>
  struct sfinae;

  // 声明辅助模板函数test，它有一个模板参数U，它有一个函数参数叫unused，unused的类型是sfinae<U, &U::__gc>*，返回类型char
  template<class U>
  static char test(sfinae<U, &U::__gc>* unused);

  // 声明辅助模板函数test的一个重载版本，它有一个模板参数，函数参数是可变参数，返回类型int
  template<class>
  static int test(...);

  // 核心的判断，考虑函数的调用：test<T>(nullptr)
  // test函数有两个重载，根据重载规则，可变参数的重载优先级是最低的，所以会优先考虑第一个重载版本
  // 编译器会先试着实例化sfinae<T, &T::__gc>，这里分两种情况考虑：
  // 1、如果T定义了成员函数void __gc()
  //   则sfinae<T, &T::__gc>是一个合法的类型，最终会调用test的第一个版本，所以test的返回类型是char
  //   这个时候，sizeof(返回类型) == sizeof(char)，value的值为true
  // 2、否则
  //   sfinae<T, &T::__gc>不是一个合法的类型，根据SFINAE规则，编译器不会报错，继续去找下一个重载版本
  //   此时会调用到test(...)，返回类型是int
  //   这个时候，sizeof(返回类型) != sizeof(char)，value的值为false
  static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(char);
};

struct foo {
  void __gc() {}
};

int main()
{
	std::cout << has_member_gc<foo>::value << " ";
	std::cout << has_member_gc<int>::value;
}
```

``` cpp
#include <type_traits>
#include <iostream>

template<class>
static char CheckIgnoreVirtualDelete(...);

#define IGNORE_VIRTUAL_DELETE(ClassName) \
    template<class> \
    static int CheckIgnoreVirtualDelete(ClassName*);

template<class T>
struct STIgnoreTest
{
    static const bool value = sizeof(CheckIgnoreVirtualDelete<T>((T*)nullptr)) == sizeof(int);
};

class CBase
{
};

class CDerived : public CBase
{
};

IGNORE_VIRTUAL_DELETE(CBase)

int main()
{
    std::cout << STIgnoreTest<int>::value << std::endl;
    std::cout << STIgnoreTest<CBase>::value << std::endl;
    std::cout << STIgnoreTest<CDerived>::value << std::endl;
}
```


# std::conditional

``` cpp
#include <iostream>
#include <type_traits>
#include <typeinfo>

int main()
{
    typedef std::conditional<true, int, double>::type Type1;
    typedef std::conditional<false, int, double>::type Type2;
    typedef std::conditional<sizeof(int) >= sizeof(double), int, double>::type Type3;

    std::cout << typeid(Type1).name() << '\n';
    std::cout << typeid(Type2).name() << '\n';
    std::cout << typeid(Type3).name() << '\n';
}
/*
i
d
d
*/
```

* https://en.cppreference.com/w/cpp/types/conditional



# [std::is_same](https://en.cppreference.com/w/cpp/types/is_same)

``` cpp
#include <type_traits>

template< class T, class U >
struct is_same;
```

* If `T` and `U` name the same type (taking into account `const`/`volatile` qualifications), provides the member constant value equal to `true`. Otherwise value is `false`.

``` cpp
#include <iostream>
#include <type_traits>

int main()
{
     // usually true if int is 32 bit
    std::cout << std::is_same<int, std::int32_t>::value << ' ';
    // possibly true if ILP64 data model is used
    std::cout << std::is_same<int, std::int64_t>::value;
}
/*
1 0
*/
```

`is_same`的实现方式：

``` cpp
#include <iostream>

// 定义is_same模板类，它接受两个模板参数T和U，它在类内定义了一个叫value的bool静态常量字段，值总是false
template<class T, class U>
struct is_same {
  static constexpr bool value = false;
};
// 对is_same偏特化，当T和U这两个类型一样时，它在类内定义了一个叫value的bool静态常量字段，值总是true
template<class T>
struct is_same<T, T> {
  static constexpr bool value = true;
};

int main()
{

  // 定义一个类型别名，将is_same<int, int>绑定到Result1
  // 从模板元编程的角度来看，这里可以看作是调用了元函数is_same，输入类型int和类型int，输出类型is_same<int, int>，赋值给Result1
  using Result1 = is_same<int, int>;

  // Result1就是类型is_same<int, int>，它有一个叫value的bool静态常量字段
  // 由于is_same<int, int>的两个模板参数都是int，所以这里是偏特化后的版本，value的值是true
  std::cout << Result1::value << " ";

  // 调用元函数is_same，输入类型int和类型float，输出类型is_same<int, float>，赋值给Result2
  using Result2 = is_same<int, float>;

  // 由于is_same<int, float>的两个模板参数不一样，所以这里没有偏特化，value的值是false
  std::cout << Result2::value << " ";

}
```

# [std::is_pointer](https://en.cppreference.com/w/cpp/types/is_pointer)

* Checks whether `T` is a **pointer to object** or **a pointer to function** (**but not a pointer to member/member function**). Provides the member constant value which is equal to `true`, if T is a object/function pointer type. Otherwise, value is equal to `false`.

``` cpp
#include <iostream>
#include <type_traits>

class A {};

int main()
{
    std::cout << std::boolalpha;
    std::cout << std::is_pointer<A>::value << '\n';
    std::cout << std::is_pointer<A *>::value << '\n';
    std::cout << std::is_pointer<A &>::value << '\n';
    std::cout << std::is_pointer<int>::value << '\n';
    std::cout << std::is_pointer<int *>::value << '\n';
    std::cout << std::is_pointer<int **>::value << '\n';
    std::cout << std::is_pointer<int[10]>::value << '\n';
    std::cout << std::is_pointer<std::nullptr_t>::value << '\n';
}
/*
false
true
false
false
true
true
false
false
*/
```


# [std::remove_cv/std::remove_const/std::remove_volatile](https://en.cppreference.com/w/cpp/types/remove_cv)

* removes the topmost `const`, or the topmost `volatile`, or both, if present.

``` cpp
#include <iostream>
#include <type_traits>

int main() {
    typedef std::remove_cv<const int>::type type1;
    typedef std::remove_cv<volatile int>::type type2;
    typedef std::remove_cv<const volatile int>::type type3;
    typedef std::remove_cv<const volatile int*>::type type4;
    typedef std::remove_cv<int * const volatile>::type type5;

    std::cout << "test1 " << (std::is_same<int, type1>::value
        ? "passed" : "failed") << '\n';
    std::cout << "test2 " << (std::is_same<int, type2>::value
        ? "passed" : "failed") << '\n';
    std::cout << "test3 " << (std::is_same<int, type3>::value
        ? "passed" : "failed") << '\n';
    std::cout << "test4 " << (std::is_same<const volatile int*, type4>::value
        ? "passed" : "failed") << '\n';
    std::cout << "test5 " << (std::is_same<int*, type5>::value
        ? "passed" : "failed") << '\n';
}
/*
test1 passed
test2 passed
test3 passed
test4 passed
test5 passed
*/
```

# std::decay

Applies lvalue-to-rvalue, array-to-pointer, and function-to-pointer implicit conversions to the type T, removes cv-qualifiers, and defines the resulting type as the member typedef type.

``` cpp
#include <iostream>
#include <type_traits>

template <typename T, typename U>
struct decay_equiv :
    std::is_same<typename std::decay<T>::type, U>::type
{};

int main()
{
    std::cout << std::boolalpha
              << decay_equiv<int, int>::value << '\n'
              << decay_equiv<int&, int>::value << '\n'
              << decay_equiv<int&&, int>::value << '\n'
              << decay_equiv<const int&, int>::value << '\n'
              << decay_equiv<int[2], int*>::value << '\n'
              << decay_equiv<int(int), int(*)(int)>::value << '\n';
}
/*
true
true
true
true
true
true
*/
```

* https://stackoverflow.com/questions/25732386/what-is-stddecay-and-when-it-should-be-used

# [Constexpr If](https://en.cppreference.com/w/cpp/language/if) (C++17)

Conditionally executes another statement. Used where code needs to be executed based on a `run-time` or `compile-time` condition.

``` cpp
#include <iostream>

template <typename T>
auto get_value(T t) {
    if constexpr (std::is_pointer_v<T>) {
        std::cout << "if constexpr\n";
        return *t; // deduces return type to int for T = int*
    }
    else {
        std::cout << "other\n";
        return t;  // deduces return type to int for T = int
    }
}

int main()
{
    int a = 1;
    int *b = &a;
    std::cout << get_value(a) << std::endl;
    std::cout << get_value(b) << std::endl;
}
/*
other
1
if constexpr
1
*/
```

使用[cppinsights](https://cppinsights.io/)实例化后的代码：

``` cpp
#include <iostream>

template <typename T>
auto get_value(T t) {
    if constexpr (std::is_pointer_v<T>) {
        std::cout << "if constexpr\n";
        return *t; // deduces return type to int for T = int*
    }
    else {
        std::cout << "other\n";
        return t;  // deduces return type to int for T = int
    }
}

#ifdef INSIGHTS_USE_TEMPLATE
template<>
int get_value<int>(int t)
{
  if constexpr(false) {
  } else /* constexpr */ {
    std::operator<<(std::cout, "other\n");
    return t;
  }

}
#endif

#ifdef INSIGHTS_USE_TEMPLATE
template<>
int get_value<int *>(int * t)
{
  if constexpr(true) {
    std::operator<<(std::cout, "if constexpr\n");
    return *t;
  }

}
#endif

int main()
{
  int a = 1;
  int * b = &a;
  std::cout.operator<<(get_value(a)).operator<<(std::endl);
  std::cout.operator<<(get_value(b)).operator<<(std::endl);
}
```


# 完美转发

``` cpp
#include <iostream>
using namespace std;

template<typename T>
void func(T&& param)
{
	cout << param << endl;
}

int main()
{
	int a{100};

	func(a); // ok
	func(100); // ok
}
```

这里的 `T&& param` 表示 `param` 是一个万能引用，如果传入的参数是左值，那么 `param` 就是左值 (比如`int&`)，外部传入的是右值，`param` 就是右值 (比如 `int&&`)。

``` cpp
template<typename T>
void func(const T& param) {
	vector<T> v;
	v.emplace_back(param);
}
```

在模版函数 `func` 内部，调用了 `v.emplace_back(param)`，`vector.emplace_back` 为左值和右值有两个不同的实现，左值会调用拷贝构造函数，而右值会调用移动构造函数，性能更好。那么按上面这个实现，因为 `func` 的参数类型是 `const T&`，一定是左值引用，那么传给 `vector.emplace_back` 的也是左值，所以即使调用 `func` 时传的参数是右值，在调用 `vector.emplace_back` 的时候也只会调用到左值的版本。

有了万能引用后，可改进为：

``` cpp
template<typename T>
void func(T&& param) {
	vector<T> v;
	v.emplace_back(param);
}
```

**注意：一个右值引用变量本身是一个左值**，例如：

``` cpp
void Test(const Demo&) {
    cout << "normal version" << endl;
}
void Test(Demo&&) {
    cout << "rvalue version" << endl;
}

Demo&& s = Demo{};
Test(s); // normal version!
Test(Demo{});
```

这里的 `v.emplace_back(param)` 依然会调用左值的版本! 这里就需要 `std::forward` 了，和 `std::move` 类似，`std::forward` 其实也是一个类型转换，当实参是左值时候，它返回的是左值引用，也就是没做任何事；实参是右值的时候，它返回的是右值引用。所以，最终正确的版本应该是:

``` cpp
template<typename T>
void func(T&& param) {
	vector<T> v;
	v.emplace_back(std::forward<T>(param));
}
```

这就是所谓的完美转发 (perfect forwarding)。

总结一下就是，函数模版参数中的 `T&&` 中的 `T` 是函数模版变量时，这代表一个万能引用，对于万能引用，当接收的是左值时，它就是左值引用，接收的是右值时，它就是右值引用。因为引用变量本身是左值，所以直接用引用变量做实参调用函数时，调用的一定是左值版本，如果需要根据引用变量本身的类型不同来调用对应版本的函数，需要使用 `std::forward` 来做类型转换。

`std::forward` 和 `std::move` 的区别在于，`std::move` 是一定转换为右值，而 `std::forward` 是有条件的转换。`std::forward` 通常和万能引用一起使用。



# Q&A

## 模版特化间接引用在debug和release版本行为不一致问题

[My template specialization differs debug version from release version, is this gcc bug?](https://stackoverflow.com/questions/39976307/my-template-specialization-differs-debug-version-from-release-version-is-this-g)

[测试代码](https://github.com/gerryyang/mac-utils/tree/master/programing/cpp/template/template_specialization_odr)

这个问题和强弱符号覆盖有关，按照C++标准建议是在使用特化版本的时候显式包含，否则存在未定义行为。

More: [template_specialization, In detail](https://en.cppreference.com/w/cpp/language/template_specialization)

## 判断一个类T是否定义了成员函数 (不包括继承)

https://wandbox.org/permlink/FAu1kZMDGPW1u9Eg

``` cpp
#include <iostream>
#include <type_traits>

#define CREATE_SPECIFIED_MEMBER_FUNC_DETECTOR(Func)                                \
                                                                                   \
template<typename T, typename MemFunc>                                             \
struct has_specified_member_func_##Func                                            \
{                                                                                  \
  template<class U, MemFunc>                                                       \
  struct HasSpecifiedMemberFunc;                                                   \
                                                                                   \
  template<class U>                                                                \
  static char test(HasSpecifiedMemberFunc<U, &U::Func>* unused);                   \
                                                                                   \
  template<class>                                                                  \
  static int test(...);                                                            \
                                                                                   \
  static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(char);          \
};

CREATE_SPECIFIED_MEMBER_FUNC_DETECTOR(CopyTo)

class A
{
public:
	template<typename T>
	void CopyTo(T&)
	{
		// 可以换成static_assert
		std::cout << has_specified_member_func_CopyTo<T, void(T::*)(T&)>::value << std::endl;
	}
};

class B : public A
{

};

int main()
{
   A a, a1;
   a.CopyTo(a1);
   B b, b1;
   b.CopyTo(b1);
   return 0;
}
/*
1
0
*/
```

# Refer

* [error: explicit specialization of non-template struct](https://stackoverflow.com/questions/49888638/template-specialization-error-explicit-specialization-of-non-template-struct)

