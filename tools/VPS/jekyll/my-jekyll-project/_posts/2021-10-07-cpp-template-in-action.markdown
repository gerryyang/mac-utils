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



  

	
	