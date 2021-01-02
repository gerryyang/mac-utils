---
layout: post
title:  "CPP Effective"
date:   2020-07-13 09:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{: toc}



# Effective C++ - Accustoming Yourself to C++

## 1 构造函数的explicit

被声明为`explicit`的构造函数通常比`non-explicit`更受欢迎，因为它们禁止编译器执行非预期的类型转换。除非有一个好理由允许构造函数被用于隐式类型转换，否则把它声明为`explicit`。

```cpp
class foo {
public:
    explicit foo(int x);
};
```

## 2 对象的复制
 
 `copy构造函数`被用来“以同型对象初始化自我对象”，`copy assignment操作符`被用来“从另一个同型对象中拷贝其值到自我对象”。

```cpp
class Widget {
public:
    Widget();                               // default构造函数
    Widget(const Widget& rhs);              // copy构造函数
    Widget& operator=(const Widget& rhs);   // copy assignment操作符
};

Widget w1;        // 调用default构造函数
Widget w2(w1);    // 调用copy构造函数
w1 = w2;          // 调用copy assignment操作符
Widget w3 = w2;   // 调用copy构造函数
```

> **copy构造和copy assignment操作符的区别**：如果一个新对象被定义，一定会有一个构造函数被调用，不可能调用赋值操作。如果没有新对象被定义，就不会有构造函数被调用，那么就是赋值操作被调用。

## 3 命名习惯

构造函数和析构函数分别使用缩写`ctor`和`dtor`代替。
使用`lhs`（left-hand side）和`rhs`（right-hand side）表示参数名称。

## 4 TR1和Boost

* `TR1`（Technical Report 1）是一份规范，描述加入C++标准程序库的诸多新机能。这些机能以新的`class templates`和`function templates`形式体现。所有`TR1`组件都被置于命名空间`tr1`内。
* `Boost`是个组织，亦是一个网站，提供可移植，源代码开放的C++程序库。大多数`TR1`机能是以`Boost`的工作为基础。

## 5 视C++为一个语言联邦

今天的C++已经是个**多重范型编程语言（multiparadigm programming language）**，一个同时支持以下特性的语言：
* 过程形式（procedural）
* 面向对象形式（object-oriented）
* 函数形式（functional）
* 泛型形式（generic）
* 元编程形式（metaprogramming）

为了理解C++，你必须认识其主要的**次语言（sublanguage）**：

### 5.1 C
说到底C++仍是以C为基础。blocks, statements, preprocessor, built-in data types, arrays, pointers等统统来自C。许多时候C++对问题的解法其实不过就是较高级的C解法，但是C++提供了C没有的templates, exceptions, overloading(重载)等功能。

> [C语言可以重载吗](https://zhidao.baidu.com/question/688652288977496364.html)

```cpp
// http://www.cplusplus.com/reference/cstdlib/qsort/
/* qsort example */
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* qsort */

int values[] = { 40, 10, 100, 90, 20, 25 };

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

void fun()
{
  printf("fun()\n");
}

/*
$gcc -o overload_test overload_test.c 
overload_test.c:18:6: error: redefinition of 'fun'
void fun(int a)
     ^
overload_test.c:13:6: note: previous definition is here
void fun()
     ^
1 error generated.
 */
#if 0
void fun(int a)
{
  printf("fun(int a)\n");
}
#endif

int main ()
{
  // 测试C语言是否支持overload重载
  fun();

  // C语言可以通过不同的函数指针来模拟overload重载
  int n;
  qsort (values, 6, sizeof(int), compare);
  for (n = 0; n < 6; ++n) {
    printf ("%d ",values[n]);
  }

  return 0;
}
```

### 5.2 Object-Oriented C++

这部分就是`C with Classes`所诉求的：
* classes（包括构造函数和析构函数）
* encapsulation（封装）
* inheritance（继承）
* polymorphism（多态）
* virtual function（虚函数动态绑定）
* etc.

### 5.3 Template C++

这是C++的`泛型编程（generic programming）`部分，也是大多数程序员经验最少的部分。

### 5.4 STL

`STL`是个template程序库，它对`containers`, `iterators`, `algorithms`以及`function objects`的规约有极佳的紧密配合与协调。

## 6 尽量以const, enum, inline替换#define

宁可以`编译器`替换`预处理器`。当你做出这样的事情：
```cpp
#define ASPECT_RATIO 1.653
```
记号名称ASPECT_RATIO也许从未被编译器看见，也许在编译器开始处理源码之前就被预处理器替换了，于是记号名称有可能没有进入`记号表（symbol table）`内，当你运用此常量但获得一个编译错误时可能会带来困惑，因为这个错误信息提到的是1.653而不是ASPECT_RATIO。尤其是如果ASPECT_RATIO被定义在一个非你所写的头文件内，你肯定对1.653来自何处毫无概念。解决的方法是：以一个常量替换上述的`宏（#define）`。
```cpp
const double AspectRatio = 1.653;   // 大写名称通常用于宏
```
好处是：

* 作为一个语言常量，AspectRatio肯定会被编译器看到，当然就会进入记号表内。
* 使用常量可能比使用#define导致较小量的目标代码，因为预处理器盲目地将宏名称进行替换会导致目标代码出现多份1.653，而若改用常量则不会出现。
* 字符串常量，`string`对象通常比`char*-based`合适。

```cpp
const char* const authorName = "gerry";
const std::string authorName("gerry");
```

* class专属常量。为了将常量的作用域（scope）限制在class内，你必须让它成为class的一个`成员（member）`，另外为了保证此常量至多只有一份实体，必须让它成为一个`static成员`。

```cpp
#include<stdio.h>

class GamePlayer {

public:
  void set_scores() {
    for (int i = 0; i != NumTurns; ++i) {
      scores[i] = i;
    }
  }
  void get_scores() {
    for (int i = 0; i != NumTurns; ++i) {
      printf("%d ", scores[i]);
    }
    printf("\n");
  }

  static int get_numturns() {
    //printf("addr GamePlayer::NumTurns[%p]\n", &GamePlayer::NumTurns);
    return GamePlayer::NumTurns;
  }

private:
  static const int NumTurns = 5;   // 常量声明
  int scores[NumTurns];            // 使用该常量
};

int main()
{
  printf("GamePlayer::NumTurns[%d]\n", GamePlayer::get_numturns());

  GamePlayer player;
  player.set_scores();
  player.get_scores();

  GamePlayer player2;
  printf("player.NumTurns[%d] player2.NumTurns[%d]\n", 
      player.get_numturns(), player2.get_numturns());

  return 0;
}
/*
GamePlayer::NumTurns[5]
0 1 2 3 4 
player.NumTurns[5] player2.NumTurns[5]
 */
```
然而，上面你所看到的是NumTurns的`声明式`，而非`定义式`。通常C++要求所使用的任何东西提供一个定义式，**但如果它是class专属常量且又是static整数类型，只要不取它们的地址，你可以声明并使用它们而无须提供定义式**。

但是，如果你需要取某个class专属常量的地址，或者编译器要求（比如，老编译器）需要看到一个定义式，**那么需要另外提供定义式**。

```cpp
#include<stdio.h>

class GamePlayer {

public:
  void set_scores() {
    for (int i = 0; i != NumTurns; ++i) {
      scores[i] = i;
    }
  }
  void get_scores() {
    for (int i = 0; i != NumTurns; ++i) {
      printf("%d ", scores[i]);
    }
    printf("\n");
  }

  static int get_numturns() {
    printf("addr GamePlayer::NumTurns[%p]\n", &GamePlayer::NumTurns);
    return GamePlayer::NumTurns;
  }

private:
  static const int NumTurns = 5;   // 常量声明
  int scores[NumTurns];            // 使用该常量
};

const int GamePlayer::NumTurns;   // NumTurns的定义

int main()
{
  printf("GamePlayer::NumTurns[%d]\n", GamePlayer::get_numturns());

  GamePlayer player;
  player.set_scores();
  player.get_scores();

  GamePlayer player2;
  printf("player.NumTurns[%d] player2.NumTurns[%d]\n", 
      player.get_numturns(), player2.get_numturns());

  return 0;
}
/*
addr GamePlayer::NumTurns[0x102092f30]
GamePlayer::NumTurns[5]
0 1 2 3 4 
addr GamePlayer::NumTurns[0x102092f30]
addr GamePlayer::NumTurns[0x102092f30]
player.NumTurns[5] player2.NumTurns[5]
*/
```
通过提供定义式，我们就可以获取class专属常量的地址。

> 注意：
> 
> * NumTurns的定义式中没有赋值是因为，class常量已在声明时获得了初值，因此定义时不可以再设置初值。
> * 我们无法利用`#define`创建一个class专属常量，因为#define并不能限制作用域（scope），一旦宏被定义，它就在其后的编译过程中有效，除非在某处被`#undef`。因此，`#define`不仅不能用来定义class专属常量，也不能提供任何封装性。

如果想具备作用域，但又不想取地址，可以使用`enum`来实现这个约束。

```cpp
class GamePlayer {

public:
  void set_scores() {
    for (int i = 0; i != NumTurns; ++i) {
      scores[i] = i;
    }
  }
  void get_scores() {
    for (int i = 0; i != NumTurns; ++i) {
      printf("%d ", scores[i]);
    }
    printf("\n");
  }

  static int get_numturns() {
    //printf("addr GamePlayer::NumTurns[%p]\n", &GamePlayer::NumTurns);
    return GamePlayer::NumTurns;
  }

private:
  //static const int NumTurns = 5;   // 常量声明
  enum {
    NumTurns = 5,    // 令NumTurns成为5的一个记号名称
  };
  int scores[NumTurns];            // 使用该常量
};
```

预处理器和宏的陷阱：

宏看起来像函数，但是不会招致`函数调用（function call）`带来的额外开销。
糟糕的做法：（有效率，但不安全）

```cpp
// 以a和b的较大值调用f函数
#define CALL_WITH_MAX(a, b) f((a) > (b) ? (a) : (b))
```

好的做法：（效率和安全同时得到保证）

```cpp
template<typename T>
inline void callWithMax(const T& a, const T& b)
{
    f(a > b ? a : b);
}
```
这个`template`根据实例化可以产出一整群函数，每个函数都接受两个同类型对象，并以其中较大的调用f。这里不需要在函数本体中为参数加上括号，也不需要操心参数被计算的次数，同时，由于callWithMax是个真正的函数，它遵守作用域和访问规则，因此可以写出**一个class内的private inline函数，而对于宏是无法完成的**。

> 请记住：
> 
> * 对于单纯常量，最好以`const`对象或`enum`替换`#define`
> * 对于形似函数的宏，最好改用`inline函数`替换`#define`

## 7 尽量使用const

`const`允许你指定一个语义约束，也就是指定一个“不该被改动”的对象，而编译器会强制实施该项约束。

```cpp
char greeting[] = "Hello";

char* p = greeting;             // non-const pointer, non-const data
const char* p = greeting;       // non-const pointer, const data
char* const p = greeting;       // const pointer, non-const data
const char* const p = greeting; // const pointer, const data
```
如果关键字`const`出现在星号左边，表示被指物是常量；如果出现在星号右边，表示指针自身是常量；如果出现在星号两边，表示被指物和指针两者都是常量。

注意：如果被指物是常量，将关键字`const`写在类型之前，和写在类型之后星号之前，这两种写法的意义相同。

```cpp
void f1(const Widget* pw);
void f2(Widget const * pw);
```

`STL迭代器`系以`指针`为根据塑模出来，所以迭代器的作用就像个`T*`指针。如果你希望迭代器所指的东西不可被改变，则需要使用`const_iterator`。

```cpp
std::vector<int> vec;

const std::vector<int>::iterator iter = vec.begin();
*iter = 10;     // ok
++iter;         // error

std::vector<int>::const_iterator citer = vec.begin();
*citer = 10;    // error
++citer;        // ok 
```

const成员函数

将`const`实施于成员函数的目的，是为了确认该成员函数可作用于`const`对象身上。这一类成员函数之所以重要，是因为：

* 它们使class接口比较容易被理解，可以得知哪个函数可以改动对象内容，而哪个函数不行。
* 它们使“操作`const`对象”成为可能，这对编写高效代码是个关键，比如，改善程序效率的一个根本方法是以`pass by reference-to-const`方式传递对象，而此技术可行的前提是，我们有const成员函数可用来处理取得的const对象。

> 注意：**两个成员函数如果只是常量性不同，可以被重载（overload）**。只有返回值类型不同的两个函数不能重载（functions that differ only in their return type cannot be overloaded）。

```cpp
#include<stdio.h>
#include<iostream>
#include<string>

class TextBlock {
public:
  TextBlock()
  {
  }
  TextBlock(const char* lhs)
  {
    text = lhs;
  }
public:
  // operator[] for const object
  const char& operator[] (std::size_t position) const 
  {
    return text[position];
  }

  // operator[] for non-const object
  char& operator[] (std::size_t position)
  {
    return text[position];
  }

private:
  std::string text;
};

int main()
{
  TextBlock tb("gerry");
  std::cout << tb[0] << std::endl;         // 调用non-const TextBlock::operator[]

  const TextBlock ctb("yang");             // 调用const TextBlock::operator[]
  std::cout << ctb[0] << std::endl;

  return 0;
}
```

成员函数如果是`const`意味着什么？—— `bitwise constness或者physical constness` VS `logical constness`

`bitwise const`指的是，成员函数只有在不更改对象之任何成员变量（static除外）时才可以说是`const`，即，const成员函数不可以更改对象内任何non-static成员变量。

> 注意：许多成员函数虽然不完全具备`const`性质，却能通过`bitwise`测试。比如，一个更改了"指针所指物"的成员函数，如果只有指针隶属于对象，那么此函数为`bitwise const`不会引发编译器异议，但是实际不能算是`const`。

下面这段代码，可以通过`bitwise`测试，但是实际上改变了对象的值。

```cpp
#include<stdio.h>
#include<iostream>
#include<string>

class TextBlock {
public:
  TextBlock()
  {
  }
  TextBlock(char* lhs)
  {
    pText = lhs;
  }
public:
  // operator[] for const object
  char& operator[] (std::size_t position) const 
  {
    return pText[position];
  }

#if 0
  // operator[] for non-const object
  char& operator[] (std::size_t position)
  {
    return pText[position];
  }
#endif

private:
  char* pText;
};

int main()
{
  char name[] = "gerry";
  const TextBlock ctb(name);
  std::cout << ctb[0] << std::endl;         // 调用const TextBlock::operator[]

  char* pc = &ctb[0];
  *pc = 'J';
  std::cout << ctb[0] << std::endl;         // 调用const TextBlock::operator[]

  return 0;
}
```

`logical constness`主张，一个`const`成员函数可以修改它所处理的对象的某些`bits`，但只有在客户端侦测不出的情况才可以（即，对客户端是透明的，但是实际上对象的某些值允许改变）。正常情况下，由于`bitwise const`的约束，`const`成员函数内是不允许修改non-static成员变量的，但是通过将一些变量声明为`mutable`则可以躲过编译器的`bitwise const`约束。

```cpp
#include<stdio.h>
#include<iostream>
#include<string>
#include<string.h>

class TextBlock {
public:
  TextBlock() : lengthIsValid(false)
  {
  }
  TextBlock(char* lhs) : lengthIsValid(false)
  {
    pText = lhs;
  }
public:
  std::size_t length() const
  {
    if (!lengthIsValid) {
      printf("do strlen... ");
      textLength = strlen(pText);    // error? 在const成员函数内不能修改non-static成员变量
      lengthIsValid = true;               // 同上
    }
    return textLength;
  }

  // operator[] for const object
  char& operator[] (std::size_t position) const 
  {
    return pText[position];
  }

#if 0
  // operator[] for non-const object
  char& operator[] (std::size_t position)
  {
    return pText[position];
  }
#endif

private:
  char* pText;

  mutable std::size_t textLength;    // 最近一次计算的文本区域块长度
  mutable bool lengthIsValid;        // 目前的长度是否有效
};

int main()
{
  char name[] = "gerry";
  const TextBlock ctb(name);
  std::cout << ctb[0] << std::endl;         // 调用const TextBlock::operator[]
  std::cout << "length: " << ctb.length() << std::endl;

  char* pc = &ctb[0];
  *pc = 'J';
  std::cout << ctb[0] << std::endl;         // 调用const TextBlock::operator[]
  std::cout << "length: " << ctb.length() << std::endl;

  return 0;
}
/*
$./mutable 
g
length: do strlen... 5
J
length: 5
 */
```

在`const`和`non-const`成员函数中避免重复

方法是：运用`const`成员函数实现出其`non-const`孪生兄弟。

不好的做法（因为有重复代码）：

```cpp
  // operator[] for const object
  const char& operator[] (std::size_t position) const 
  {
    // bounds checking
    // log access data
    // verify data integrity
    // ...

    return text[position];
  }

  // operator[] for non-const object
  char& operator[] (std::size_t position)
  {
    // bounds checking
    // log access data
    // verify data integrity
    // ...

    return text[position];
  }
```

好的做法（实现`operator[]`的机能一次并使用它两次，令其中一个调用另一个）：

```cpp
#include<stdio.h>
#include<iostream>
#include<string>

class TextBlock {
public:
  TextBlock()
  {
  }
  TextBlock(const char* lhs)
  {
    text = lhs;
  }
public:
  // operator[] for const object
  const char& operator[] (std::size_t position) const 
  {
    // bounds checking
    // log access data
    // verify data integrity
    // ...

    std::cout << "const char& operator[]() const\n";
    return text[position];
  }

#if 0
  // operator[] for non-const object
  char& operator[] (std::size_t position)
  {
    // bounds checking
    // log access data
    // verify data integrity
    // ...

    return text[position];
  }
#endif

  char& operator[] (std::size_t position)
  {
    std::cout << "char& operator[]()\n";
    return const_cast<char&>(static_cast<const TextBlock&>(*this)[position]);
  }

private:
  std::string text;
};

int main()
{
  TextBlock tb("gerry");
  std::cout << tb[0] << std::endl;         // 调用non-const TextBlock::operator[]

  const TextBlock ctb("yang");             // 调用const TextBlock::operator[]
  std::cout << ctb[0] << std::endl;

  return 0;
}
/*
char& operator[]()
const char& operator[]() const
g
const char& operator[]() const
y 
 */
```

> 请记住：
> 
> * 将某些东西声明为`const`可帮助编译器侦测出错误用法。`const`可被施加于任何作用域内的对象、函数参数、函数返回类型、成员函数本体。
> * 编译器强制实施`bitwise constness`，但你编写程序时应该使用“概念上的常量性”。
> * 当`const`和`non-const`成员函数有着实质等价的实现时，令`non-const`版本调用`const`版本可避免代码重复。

## 8 确定对象被使用前已先被初始化

关于“将对象初始化”这事，C++似乎反复无常（对象的初始化动作何时一定发生，何时不一定发生）。针对这种复杂的规则，最佳的处理方法是：**永远在使用对象之前先将它初始化**。

> 对于内置类型，必须手工完成初始化；对于内置类型以外的其他类型，初始化责任落在构造函数（constructors）身上，即，确保每一个构造函数都将对象的每一个成员初始化。

构造函数初始化的正确方法是：**使用`member initialization list(成员初值列)`，而不是在构造函数中的赋值。因为第一种方法的执行效率通常较高**（对于大多数类型而言，比起先调用`default`构造函数，然后再调用`copy assignment`操作符，单只调用一次`copy`构造函数是比较高效的。对于内置类型，其初始化和赋值的成本相同，但为了一致性最好也通过成员初值列来初始化）。

```cpp
ABEntry:ABEntry(const std::string& name, const std::string& address,
                 const std::list<PhoneNumber>& phones)
: theName(name),        // 成员初值列表，这些都是初始化
  theAddress(address),
  thePhones(phones),
  numTimesConsulted(0)
{ }

ABEntry::ABEntry()
: theName(),            // 调用theName的`default`构造函数
  theAddress(),         // 同上
  thePhones(),          // 同上
  numTimesConsulted(0)  // 将内置类型int显示初始化为0
{ }
```

C++有着十分固定的"成员初始化次序"：总是`base classes`更早于其`derived classes`被初始化。而class的成员变量总是以其声明次序被初始化，而和它们在成员初始值列中的出现次序无关。**建议，当你在成员初值列中初始化各个成员时，最好总是和其声明的次序一致**。

最后一个问题：不同编译单元内定义的`non-local static`对象的初始化顺序是怎么样的？

`函数内的static`对象称为`local static`对象，其他static对象称为`non-local static`对象。

> C++对定义于不同编译单元内的`non-local static`对象的初始化次序并无明确定义。因此，如果某编译单元内的某个`non-local static`对象的初始化动作依赖另一编译单元内的某个`non-local static`对象，那么它所用到的这个对象可能尚未被初始化。

**针对上面这个问题的解决方法是**：
将每个`non-local static`对象搬到自己的专属函数内，这些函数返回一个reference指向它所含的对象。即，`non-local static`对象被`local static`对象替换了。

```cpp
class FileSystem { ... };
FileSystem& tfs()
{
    static FileSystem fs;
    return fs;
}
```

注意：这些函数内含static对象的事实使它们在多线程系统中带有不确定性。处理这种麻烦的方法是，在程序的单线程启动阶段，手工调用所有reference-returning函数，这可消除与初始化有关的`race conditions（竞速形势）`。

> 请记住
> 
> * 为内置类型对象进行手工初始化，因为C++不保证初始化它们。
> * 构造函数最好使用成员初值列（`member initialization list`），而不要在构造函数本体内使用赋值操作（`assignment`）。初值列列出的成员变量，其排列次序应该和它们在class中的声明次序相同。
> * 为免除跨编译单元的初始化次序问题，请以`local static`对象替换`non-local static`对象。


# Effective C++ - Constructors, Destructors, and Assignment Operators

## 1 了解C++默默编写并调用了哪些函数

如果是一个空类，那么编译器可能会自动生成：
* copy构造函数
* copy assignment操作符
* 析构函数
* default构造函数

以上这些函数都是`public`且`inline`的。

```cpp
class Empty {};

// 等价于
class Empty {
public:
    Empty()                       // default构造函数
    {}        
    Empty(const Empty& rhs)       // copy构造函数
    {} 
    ~Empty()                      // 析构函数（是否是virtual呢？）
    {}
    Empty& operator=(const Empty& rhs)  // copy assignment操作符
    {}
};
```

注意：

* 惟有当这些函数被调用，它们才会被编译器创建出来。

```cpp
Empty e1;       // default构造函数
                // 析构函数
Empty e2(e1);   // copy构造函数
e2 = e1;        // copy assignment操作符
```

* 编译器生成的析构函数是个`non-virtual`，除非这个class的base class自身声明有`virtual`析构函数。
* `copy`构造函数和`copy assignment`操作符，编译器创建的版本只是单纯地将来源对象的每一个`non-static`成员变量拷贝到目标对象。

```cpp
#include<iostream>
#include<string>

template<typename T>
class NamedObject {
public:
#if 1
  NamedObject(const char* name, const T& value) :
    nameValue(name), objectValue(value)
  {
    std::cout << "NamedObject(const char* name, const T& value)\n";
  }
#endif

  NamedObject(const std::string& name, const T& value) :
    nameValue(name), objectValue(value)
  {
    std::cout << "NamedObject(const std::string& name, const T& value)\n";
  }

public:
  std::string nameValue;
  T objectValue;
};

int main()
{
  NamedObject<int> no1("gerry", 1);
  NamedObject<int> no2(no1);         // 调用copy构造函数
  NamedObject<int> no3("yang", 2);
  
  no3 = no1;
  std::cout << no3.nameValue << "\n";

  return 0;
}
/*
NamedObject(const char* name, const T& value)
NamedObject(const char* name, const T& value)
gerry
*/
```

NamedObject没有声明`copy`构造函数，也没有声明`copy assignment`操作符，所以编译器会创建这些函数当它们被调用的时候。编译器生成的`copy`构造函数必须以`no1.nameValue`和`no1.objectValue`为初值设定`no2.nameValue`和`no2.objectValue`。两者之中，`nameValue`的类型是`string`，而标准的`string`有个`copy`构造函数，所以`no2.nameValue`的初始化方式是调用`string`的`copy`构造函数并以`no1.nameValue`为实参。另一个成员`NameObject<int>::objectValue`的类型是`int`（对此template具现体而言`T`是`int`），是个内置类型，所以`no2.objectValue`会以拷贝`no1.objectValue`内的**每一个bits**来完成初始化。编译器为`NamedObject<int>`所生成的`copy assignment`操作符，其行为基本上与`copy`构造函数一样。

**请记住**
> 编译器可以暗自为class创建`default`构造函数、`copy`构造函数、`copy assignment`操作符，以及析构函数。

## 2 若不想使用编译器自动生成的函数，就应该明确拒绝

Explicitly disallow the use of compiler-generated functions you do not want.

通常如果你不希望class支持某一特定机能，只要不声明对应函数就是了。但这个策略对`copy构造函数`和`copy assignment操作符`却不起作用。因为，如果你不声明它们，而某些人尝试调用它们，编译器会为你声明它们。
如果你不声明`copy构造函数`和`copy assignment操作符`，编译器可能会为你产出一份，于是你的class支持`copying`；如果你声明它们，你的class还是支持`copying`。**但这里的目标却是要阻止copying!**

答案的关键是，所有编译器产出的函数都是`public`。为阻止这些函数被创建出来，你得自行声明它们，但这里并没有什么需求使你必须将它们声明为`public`。因此你可以将`copy构造函数`和`copy assignment操作符`声明为`private`。**这样明确声明一个成员函数，可以阻止编译器暗自创建其专属版本；而令这些函数为private，使得成功阻止人们调用它。**

**一般而言，这个做法并不绝对安全**。因为`member函数`和`friend函数`还是可以调用你的`private函数`。除非你非常聪明不去定义它，那么如果有人不慎调用任何一个，会获得一个连接错误（`linkage error`）。
> 将成员函数声明为private而且故意不实现它们，这一伎俩是如此为大家接受，因而被用在`C++ iostream`程序库中阻止copying行为。

例子：

```cpp
#include<iostream>
#include<string>

class HomeForSale;
void copy_friend(HomeForSale& lhs, HomeForSale& rhs)
{
  lhs = rhs;
}

class HomeForSale 
{
  friend void copy_friend(HomeForSale& lhs, HomeForSale& rhs);

public:
  HomeForSale() 
  {
  }
  HomeForSale(const std::string& lhs) :
    name(lhs)
  {
  }

  HomeForSale& copy_ctor(HomeForSale& lhs)
  {
    // error LNK2019: 无法解析的外部符号 "private: __thiscall HomeForSale::HomeForSale(class HomeForSale const &)" (??0HomeForSale@@AAE@ABV0@@Z)，该符号在函数 "public: class HomeForSale & __thiscall HomeForSale::copy_ctor(class HomeForSale &)" (?copy_ctor@HomeForSale@@QAEAAV1@AAV1@@Z) 中被引用
    return HomeForSale(lhs);
  }

  void copy_assignment(HomeForSale& lhs)
  {
    // error LNK2019: 无法解析的外部符号 "private: class HomeForSale & __thiscall HomeForSale::operator=(class HomeForSale const &)" (??4HomeForSale@@AAEAAV0@ABV0@@Z)，该符号在函数 "public: void __thiscall HomeForSale::copy(class HomeForSale &)" (?copy@HomeForSale@@QAEXAAV1@@Z) 中被引用
    *this = lhs;
  }

  std::string name;

private:
  HomeForSale(const HomeForSale&);             // 未实现
  HomeForSale& operator=(const HomeForSale&);  // 未实现
};

int main()
{
  HomeForSale h1("first");

  HomeForSale h3(h1);     // error
  HomeForSale h2 = h1;    // error

  HomeForSale h4("fouth");
  h4 = h1;                // error

  HomeForSale h5("fifth");        // linkage error
  h5.copy_ctor(h1);

  HomeForSale h6("sixth");
  h6.copy_assignment(h1);         // linkage error
  std::cout << h6.name << "\n";

  HomeForSale h7("seventh");
  copy_friend(h7, h1);              // error?

  return 0;
}
```

当用户企图拷贝HomeForSale对象，编译器会阻挠他。如果你不慎在member函数或friend函数之内那么做，会轮到连接器发出抱怨。

> 若为private且提供了实现，则通过friend的方式仍然可以实现复制。

``` cpp
#include<iostream>
#include<string>

class HomeForSale 
{
  friend void copy_friend(HomeForSale& lhs, HomeForSale& rhs);

public:
  HomeForSale() 
  {
  }
  HomeForSale(const std::string& lhs) :
    name(lhs)
  {
  }

  std::string name;

private:
  HomeForSale(const HomeForSale& rhs) { name = rhs.name; }
  HomeForSale& operator=(const HomeForSale& rhs) { name = rhs.name; return *this; }
};

void copy_friend(HomeForSale& lhs, HomeForSale& rhs)
{
  lhs = rhs;
}

int main()
{
  HomeForSale h1("first");
  HomeForSale h2("second");
  copy_friend(h2, h1);
  std::cout << h2.name << std::endl;

  return 0;
}
```

*另一种方法*

将`链接器`错误移植`编译器`是可能的，而且那是好事，毕竟越早发现错误越好。方法是：在一个专门为了阻止copying动作而设计的`base class`内，将`copy构造函数`和`copy assignment操作符`声明为`private`。

因为，只要任何人，甚至是member函数或friend函数，尝试拷贝HomeForSale对象，编译器便试着生成一个`copy构造函数`和一个`copy assignment操作符`，这些函数的“编译器生成版”会尝试调用其`base class`的对应兄弟，那些调用会被编译器拒绝，因为其`base class`的拷贝构造函数是`private`。

这种方法也有一个问题，由于它总是扮演`base class`，因此使用此项技术可能导致**多重继承**，因为你往往还可能需要继承其他class，而多重继承有时会阻止`empty base class optimization`。

```cpp
#include<iostream>
#include<string>

class Uncopyable
{
protected:
  Uncopyable() {}      // 允许derived对象构造和析构
  ~Uncopyable() {}

private:
  Uncopyable(const Uncopyable&);             // 阻止copying
  Uncopyable& operator=(const Uncopyable&);
};

class HomeForSale : private Uncopyable
{
public:
  HomeForSale() 
  {
  }
  HomeForSale(const std::string& lhs) :
    name(lhs)
  {
  }

  HomeForSale& copy_ctor(HomeForSale& lhs)
  {
    return HomeForSale(lhs);
  }

  void copy_assignment(HomeForSale& lhs)
  {
    *this = lhs;
  }

  std::string name;

private:
  //HomeForSale(const HomeForSale&);
  //HomeForSale& operator=(const HomeForSale&);
};

int main()
{
  HomeForSale h1("first");
  
  HomeForSale h2 = h1;    // error
  HomeForSale h3(h1);     // error

  HomeForSale h4("fouth");
  h4 = h1;                // error

  HomeForSale h5("fifth");        // complie err, not linkage error
  h5.copy_ctor(h1);

  HomeForSale h6("sixth");
  h6.copy_assignment(h1);         // complie err, not linkage error
  std::cout << h6.name << "\n";

  return 0;
}
```

**请记住**
> 为驳回编译器自动提供的机能，可将相应的成员函数声明为private并且不予实现。使用像Uncopyable这样的base class也是一种做法。

## 3 为多态基类声明virtual析构函数

C++指出，当`derived class`对象经由一个`base class`指针被删除，而该`base class`带着一个`non-virtual`析构函数，其结果未定义 —— **实际执行时通常发生的是，对象的`derived`成分没被销毁**。于是造成一个诡异的“局部销毁”对象，从而导致资源泄露。

base类没有使用virtual析构函数：

```cpp
#include <stdio.h>
#include <iostream>
using namespace std;

class base {
public:
  base() {cout << "base()\n";}
  ~base() {cout << "~base()\n";} // note, have no virtual

private:
  int v1;
};

class derived : public base {
public:
  derived() {cout << "derived()\n";}
  ~derived() {cout << "~derived()\n";}

private:
  int v2;
};

int main() 
{
  // derived obj;
  base *b = new derived;
  // do something
  delete b;

  return 0;
}
/*
output:
base()
derived()
~base()
 */
```

base类使用virtual析构函数：

```cpp
#include <stdio.h>
#include <iostream>
using namespace std;

class base {
public:
  base() {cout << "base()\n";}
  virtual ~base() {cout << "~base()\n";} // have virtual

private:
  int v1;
};

class derived : public base {
public:
  derived() {cout << "derived()\n";}
  ~derived() {cout << "~derived()\n";}

private:
  int v2;
};

int main() 
{
  // derived obj;
  base *b = new derived;
  // do something
  delete b;

  return 0;
}
/*
output:
base()
derived()
~derived()
~base()
 */
```

> *观点1*：任何class只要带有virtual函数，都几乎确定应该也有一个virtual析构函数。
> 
> *观点2*：如果class不含virtual函数，通常表示它并不意图被用做一个base class。当class不企图被当做base class，令其析构函数为virtual往往是一个馊主意。因为，欲实现出virtual函数，对象必须携带某些信息，主要用来在运行期决定哪一个virtual函数该被调用。这份信息通常是由一个所谓`vptr(virtual table pointer)`指出，`vptr`指向一个由函数指针构成的数组，称为`vtbl(virtual table)`。每一个带有virtual函数的class都有一个相应的`vtbl`。当对象调用某一virtual函数，实际被调用的函数取决于该对象的`vptr`所指的那个`vtbl`（编译器在其中寻找合适的函数指针）。这样，如果base class内含virtual函数，那么其对象的体积会增加，在32-bits计算机体系结构中将多占用32bits（vptr大小）；而在64-bits计算机体系结构中多占用64bits（指针大小为8字节）。
> 
> *观点3*：**标准库string不含任何virtual函数，但有时程序员会错误地把它当做base class**。那么，当你在程序任意某处无意间将一个`pointer-to-specialstring`转换为一个`pointer-to-string`，然后将转换所得的那个`string指针`delete掉，则立刻被流放到"不明确行为上"。很不幸C++目前没有提供类似Java的`final classes`禁止派生的机制。

``` cpp
#include <iostream>
#include <string>

using namespace std;

// https://en.cppreference.com/w/cpp/string/basic_string
class Dummy : public std::string
{

public:
    Dummy() { 
        cout << "Dummy()" << endl; 
    }
    ~Dummy() { 
        cout << "~Dummy()" << endl; 
        delete[] no_del_member;
        cout << "delete[] no_del_member" << endl;
    }
    
    Dummy(const char *str) : std::string(str) { 
        cout << "Dummy(const char *str)" << endl; 
        no_del_member = new char[8]();
        snprintf(no_del_member, 8, "hello");
        cout << no_del_member << endl;
    }

private:
    char* no_del_member;
};

int main()
{
    std::string *str = new Dummy("gerry");
    cout << str->size() << endl;

    delete str; // error, no delete no_del_member, Memory leak !
    
    return 0;
}
/*
Dummy(const char *str)
hello
5
*/
```

**请记住**
> 1. 从里向外构造（ctor），从外向里析构（dtor）
> 2. polymorphic (带多态性质) base classes应该声明一个virtual析构函数。如果class带有任何virtual函数，它就应该拥有一个virtual析构函数。
> 3. classes的设计目的如果不是作为base classes使用，或不是为了具备多态性使用，此class就不该声明virtual析构函数。

## 4 别让异常逃离析构函数

C++`并不禁止析构函数吐出异常`，但它不鼓励你这样做。
```cpp
#include <iostream>
#include <exception>
#include <vector>
using namespace std;

class Widget {
public:
  Widget()
  {
    cout << "Widget()\n";
  }
  ~Widget()
  {
    cout << "~Widget()\n";
    throw std::runtime_error("~Widget()");
  }
private:
  int v;
};

int main()
{
  vector<Widget> w_vec;
  w_vec.resize(3);

  return 0;
}
/*
output:
Widget()
Widget()
Widget()
~Widget()
libc++abi.dylib: terminating with unexpected exception of type std::runtime_error: ~Widget()
Abort trap: 6
 */
```
当vector对象被销毁，它有责任销毁其内含的所有对象。假设vector内含10个对象，而在析构第一个元素期间，有个异常抛出，其他9个对象还是应该被销毁，否则它们保存的任何资源都会发生泄漏。因此，应该调用它们各个析构函数。

*正确的处理方法*：在析构函数里捕获每一个异常

```cpp
#include <iostream>
#include <exception>
#include <vector>
using namespace std;

class Widget {
public:
  Widget()
  {
    cout << "Widget()\n";
  }
  ~Widget()
  {
    // 析构函数里如果抛出异常，需要自己捕获处理，否则会资源泄漏
    try {
      cout << "~Widget()\n";
      throw std::runtime_error("~Widget()");
    } catch (std::runtime_error &e) {
      cout << "catch exception at ~Widget()\n";
    }
  }
private:
  int v;
};

int main()
{
  try {
    vector<Widget> w_vec;
    w_vec.resize(3);

  } catch (...) {
    cout << "catch exception at main()\n";
  }

  return 0;
}
/*
Widget()
Widget()
Widget()
~Widget()
catch exception at ~Widget()
~Widget()
catch exception at ~Widget()
~Widget()
catch exception at ~Widget()
*/
```

**请记住**
> 1. 析构函数绝对不要吐出异常。如果一个被析构函数调用的函数可能抛出异常，析构函数应该捕捉任何异常，然后吞下它们（不传播）或结束程序。
> 2. 如果客户需要对某个操作函数运行期间抛出的异常做出反应，那么`class`应该提供一个普通函数（而非在析构函数中）执行该操作。

## 5 绝不在构造和析构过程中调用virtual函数

你不该在构造函数和析构函数期间调用virtual函数，因为这样的调用不会带来你预期的效果。

例如：假设你有个class继承体系，用来塑模股市交易如买进、卖出的订单等等，这样的交易一定要经过审计，所以每当创建一个交易对象，在审计日志中也需要创建一笔适当记录。

```cpp
#include<stdio.h>
#include<iostream>

class Transaction { // base class
public:
  Transaction();

  // 做出一份因类型不同而不同的日志记录
  virtual void logTransaction() const = 0;
};

Transaction::Transaction()
{
  std::cout << "Transaction()\n";

  // 最后动作是记录日志
  logTransaction();
}

class BuyTransaction: public Transaction { // derived class
public:
  virtual void logTransaction() const {
    std::cout << "BuyTransaction::logTransaction()\n";
  }
};

class SellTransaction: public Transaction { // derived class
public:
  virtual void logTransaction() const {
    std::cout << "SellTransaction::logTransaction()\n";
  }
};

int main()
{
  BuyTransaction bt;

}
/*
g++ -o no_virtual_in_ctor_dtor no_virtual_in_ctor_dtor.cpp
no_virtual_in_ctor_dtor.cpp:17:2: warning: call to pure virtual member function 'logTransaction';
      overrides of 'logTransaction' in subclasses are not available in the constructor of
      'Transaction'
        logTransaction();
        ^
no_virtual_in_ctor_dtor.cpp:9:2: note: 'logTransaction' declared here
        virtual void logTransaction() const = 0;
        ^
1 warning generated.
 
 */
```
发现无法调用`derived class`的函数，在编译期间就报错了。把`pure virtual`去掉：

```cpp
#include<stdio.h>
#include<iostream>

class Transaction { // base class
public:
  Transaction();

  // 做出一份因类型不同而不同的日志记录
  virtual void logTransaction() const {
    std::cout << "Transaction::logTransaction()\n";
  }
};

Transaction::Transaction()
{
  std::cout << "Transaction()\n";

  // 最后动作是记录日志
  logTransaction();
}

class BuyTransaction: public Transaction { // derived class
public:
  virtual void logTransaction() const {
    std::cout << "BuyTransaction::logTransaction()\n";
  }
};

class SellTransaction: public Transaction { // derived class
public:
  virtual void logTransaction() const {
    std::cout << "SellTransaction::logTransaction()\n";
  }
};

int main()
{
  BuyTransaction bt;

}
/*
g++ -o no_virtual_in_ctor_dtor no_virtual_in_ctor_dtor.cpp
./no_virtual_in_ctor_dtor 
Transaction()
Transaction::logTransaction()
 */

```
这次可以编译过了，但是发现调用的并不是派生类的virtual函数。

**原因分析**：
> 1. 在创建派生类对象时，`derived class`对象内的`bass class`成分会在`derived class`自身成分被构造**之前**先构造妥当。Transaction构造函数调用virtual函数logTransaction，这时被调用的logTransaction是Transaction内的版本，不是BuyTransaction内的版本。base class构造期间virtual函数绝不会下降到derived classes阶层，在base class构造期间，virtual函数不是virtual函数。
> 2. 由于`base class`构造函数的执行更早于`derived class`构造函数，当`base class`构造函数执行时`derived class`的成员变量尚未初始化。如果此期间调用的virtual函数下降至`derived classes`阶层，而`derived class`的函数使用的local成员变量尚未初始化，将导致不明确行为。
> 3. 在`derived class`对象的的`base class`构造期间，对象的类型是`base class`而不是`derived class`。不只virtual函数会被编译器解析至`base class`，若使用运行期类型信息（runtime type information），例如`dynamic_cast`和`typeid`，也会把对象视为`base class`类型。

**相同的道理也适用于析构函数**。一旦`derived class`析构函数开始执行，对象内的`derived class`成员变量便呈现未定义值，所以C++视它们仿佛不再存在，进入`base class`析构函数后对象就成为一个`base class`对象。

**解决方法**：

如何确保每次一有Transaction继承体系上的对象被创建，就会有适当版本的logTransaction被调用呢？

**一种做法**：
是在class Transaction内将logTransaction函数改为`non-virtual`，然后要求`derived class`构造函数传递必要信息给Transaction构造函数，而后那个构造函数便可安全地调用`non-virtual`logTransaction。

```cpp
#include <stdio.h>
#include <iostream>
#include <string>

class Transaction { // base class
public:
  explicit Transaction(const std::string& logInfo);

  // 做出一份因类型不同而不同的日志记录
  void logTransaction(const std::string& logInfo) const {
    std::cout << "Transaction::logTransaction(): " << logInfo << "\n";
  }
};

Transaction::Transaction(const std::string& logInfo)
{
  std::cout << "Transaction()\n";

  // 最后动作是记录日志
  logTransaction(logInfo);
}

class BuyTransaction: public Transaction { // derived class
public:
  BuyTransaction(const std::string &paras)
    : Transaction(createLogString(paras)) // 将log信息传给base class构造函数
  {}
private:
  static std::string createLogString(const std::string& paras);

};

std::string BuyTransaction::createLogString(const std::string& paras)
{
  if (paras == "1") return "1+";
  else if (paras == "2") return "2+";
  else return "+";
}

int main()
{
  BuyTransaction bt1("1");
  BuyTransaction bt2("2");
}
/*
g++ -o no_virtual_in_ctor_dtor no_virtual_in_ctor_dtor.cpp
./no_virtual_in_ctor_dtor
Transaction()
Transaction::logTransaction(): 1+
Transaction()
Transaction::logTransaction(): 2+ 
 */
```

**请记住**
> 在构造和析构期间不要调用virtual函数，因为这类调用从不下降至`derived class`。

## 6 令operator=返回一个`reference to *this`

```cpp
int x, y, z;
x = y = z = 10;      // 赋值连锁形式
x = (y = (z = 10));  // 赋值采用右结合律
```

为了实现“连锁赋值”，赋值操作符必须返回一个reference指向操作符的左侧实参。这是你为classes实现赋值操作符时应该遵循的协议。

```cpp
#include <iostream>

class Widget {
public:
  explicit Widget(int rhs) : a(rhs)
  {
  }

  Widget& operator=(const Widget& rhs) // 返回类型是个reference，指向当前对象
  {
    a = rhs.a;
    return *this;   // 返回左侧对象
  }

  Widget& operator=(int rhs) // 此函数也适用，即使此操作符的参数类型不符协定
  {
    a = rhs;
    return *this;
  }

  Widget& operator+=(const Widget& rhs)
  {
    this->a += rhs.a;
    return *this;
  }

  void print()
  {
    std::cout << a << "\n";
  }

private:
  int a;
};

int main()
{
  Widget w(1);
  w.print();

  Widget w2(2);
  w = w2;
  w.print();

  int i = 100;
  w = 100;
  w.print();

  w += w2;
  w.print();
}
/*
 ./operator 
1
2
100
102
 */
```

**请记住**
> 令赋值（assignment）操作符返回一个`reference to *this`。

## 7 在operator=中处理自我赋值

自我赋值发生在对象被赋值给自己时，这看起来有点愚蠢，但是它合法。所以不要认定客户绝不会那么做。此外自我赋值动作并不总是可以一眼看出来。

```cpp
// 潜在的自我赋值
a[i] = a[j];  
*px = *py;
```

这些并不明显的自我赋值，是名带来的结果。实际上，两个对象只要来自同一个继承体系，它们甚至不需要声明为相同类型就可能造成别名，因为一个base class的reference或pointer可以指向一个derived class对象。

```cpp
class Base { ... };
class Derived: public Base { ... };

// rb和*pb有可能其实是同一对象
void doSomething(const Base& rb, Derived* pd);
```

因此，在处理自我赋值时应该注意保证：
1. 自我赋值安全问题
2. 异常问题

```cpp
class Bitmap { ... };

class Widget {
public:
  Widget& operator=(const Widget& rhs);
private:
  Bitmap* pb;
};

// 不安全的版本
Widget& Widget::operator=(const Widget& rhs)
{
  delete pb;
  pb = new Bitmap(*rhs.pb);
  return *this;
}

// 安全的版本，但不具备异常安全性
// 如果new异常，Widget最终会持有一个指针指向一块被删除的Bitmap
Widget& Widget::operator=(const Widget& rhs)
{
  if (this == &rhs) return *this;   // identity test
  
  delete pb;
  pb = new Bitmap(*rhs.pb);
  return *this;
}

// 异常安全的版本，同时也是自我赋值安全的
// 现在如果new异常，pb保存原状
// 即使没有identity test，这段代码还是能够处理自我赋值，虽然不是最高效的方法，但是行得通
Widget& Widget::operator=(const Widget& rhs)
{
  Bitmap* pOrig = pb;
  pb = new Bitmap(*rhs.pb);
  delete pOrig;
  return *this;
}
```

对于第三个版本的补充说明：
如果你很关心效率，可以把identity test再次放回函数起始处。然而这样做之前先问问自己，你估计自我赋值的发生概率有多高？因为这项测试也需要成本，它会使代码变得大一些并导入一个新的控制流分支，而两者都会降低执行速度。Prefetching, caching和pipelining等指令的效率都会因此降低。

另一个替代方案是：使用copy and swap技术。此方法，为了伶俐巧妙而牺牲了清晰性。

**请记住**
> 1. 确保当对象自我赋值时operator=有良好行为，其中技术包括，比较来源对象和目标对象的地址，精心周到的语句顺序，以及copy-and-swap。
> 2. 确定任何函数如果操作一个以上的对象，而其中多个对象是同一个对象时，其行为仍然正确。

## 8 复制对象时勿忘其每一个成分

1. 设计良好的OO系统会将对象的内部封装起来，只留两个函数负责对象拷贝，copy构造函数和copy assignment操作符，我们称它们为copying函数。
2. 编译器会在必要的时候为我们的class创建copying函数，并说明这些“编译器生成版”的行为是，将被拷对象的所有成员变量都做一份拷贝。 如果你声明自己的copying函数，意思就是告诉编译器你不喜欢缺省实现中的某些行为，编译器仿佛被冒犯似的，会以一种奇怪的方式回敬你，当你的实现代码出错时却不告诉你。
3. 如果你为class添加一个成员变量，你必须同时修改copying函数，如果你忘记了，编译器也不会告诉你。
4. 任何时候，只要你承担起为derived class撰写copying函数的责任，必须很小心地也复制其base class成分，那些成分往往是private，所以你无法直接访问它们，你应该让derived class的copying函数调用相应的base class函数。
5. 如果你发现你的copy构造函数和copy assginment操作符有相近的代码，消除重复代码的做法是，建立一个新的成员函数给两者调用，这样的函数往往是private而且常被命名为init。

```cpp
// 调用base class的copy构造函数
Derived::Derived(const Derived& rhs): Base(rhs), xxx(rhs.xxx)
{
}

Derived& Derived::operator=(const Derived& rhs)
{
  Base::operator=(rhs); // 对base class成分进行赋值
  xxx = rhs.xxx;
  return *this;
}
```

当你编写一个copying函数，请确保：
* 复制所有local成员变量
* 调用所有base classes内的适当的copying函数

**请记住**
> 1. copying函数应该确保复制对象内的所有成员变量，及所有base class成分。
> 2. 不要尝试以某个copying函数实现另一个copying函数，应该将相近的代码放在第三个函数中，并由两个copying函数调用。



# Effective C++ - Resource Management

资源，就是一旦用了它，将来必须还给系统。C++中最常使用的资源就是动态分配内存，但内存只是你必须管理的众多资源之一。其他常见的资源还包括文件描述符，互斥锁，数据库连接，网络sockets等。

> 不论哪一种资源，重要的是，当你不再使用它时，必须将它还给系统。但是，难点是，当你考虑到异常、函数内多重回传路径、程序维护不正确的改动等情况，就很难做到上面的保证。

## 1 以对象管理资源

> Use objects to manage resources.

```cpp
void f()
{
    Investment* pInv = createInvestment();  // 调用factory函数，返回一个动态对象
    // do something
    // ...
    
    delete pInv;  // 释放动态对象
}
```
问题：在若干情况下（例如，中途过早返回，或抛出异常等），f可能无法删除动态对象。当然，谨慎地编写程序可以防止上面的错误，但是随着代码的修改和维护，这种保障总是显得吃力。

好的做法：为确保返回的资源总是被释放，我们需要将资源放进对象内，当控制流离开f，该对象的析构函数会自动释放那些资源。许多资源被动态分配于heap内，而后被用于单一区块或函数内。它们应该在控制流离开那个区域或函数时被释放。

### 解决方案，使用auto_ptr

标准程序库提供的`auto_ptr`正是针对这种形势而设计的，`auto_ptr`是个类指针对象（智能指针），其析构函数自动对其所指对象调用delete。

```cpp
void f()
{
    std::auto_ptr<Investment> pInv(createInvestment());

    // do something
    // ...

    // 最后由auto_ptr的析构函数自动删除pInv
}
```

**想法：**

* 获得资源后立刻放进管理对象内。

即，资源取得时机便是初始化时机（`Resource Acquisition Is Initialization; RAII`）

* 管理对象运用析构函数确保资源被释放。

不论控制流如何离开区块，一旦对象被销毁，其析构函数会被自动调用，于是资源被释放。

> 注意：**由于auto_ptr被销毁时会自动删除它所指之物，所以一定要注意别让多个auto_ptr同时指向同一对象，否则对象会被删除一次以上**。为了预防这个问题，auto_ptr有一个特性是，**若通过copy构造函数或copy assignment操作符复制它们，它们会变成null，而复制所得的指针将取得资源的唯一拥有权**。

```cpp
// pInv1指向createInvestment返回物
std::auto_ptr<Investment> pInv1(createInvestment());

// 现在pInv2指向对象，pInv1被设为null
std::auto_ptr<Investment> pInv2(pInv1);

// 现在pInv1指向对象，pInv2被设为null
pInv1 = pInv2;
```

**带来的问题**：由于STL容器要求其元素发挥**“正常的”复制行为**，而auto_ptr的这种诡异的复制行为，导致其不符合STL的容器要求。

### 替代方案

auto_ptr的替代方案是**"引用计数型智能指针（reference-counting smart pointer; RCSP）**"。

> 所谓RCSP，也是个智能指针，持续追踪共有多少对象指向某笔资源，并在无人指向它时自动删除该资源。RCSP提供的行为类似垃圾回收（garbage collection），不同的是，RCSP无法打破环状引用（cycles of references）。例如，两个其实已经没被使用的对象彼此互指，因而好像还处在“被使用”的状态。

TR1的`tr1::shared_ptr`就是个RCSP，所以你可以这么写f：

```cpp
void f()
{
    // 使用shared_ptr
    std::tr1::shared_ptr<Investment> pInv(createInvestment());
    
    // do something
    // ...

    // 经由shared_ptr析构函数自动删除pInv
}

// 看下复制行为
void f()
{
    // pInv1指向createInvestment返回物
    std::tr1::shared_ptr<Investment> pInv1(createInvestment());

    // pInv1和pInv2指向同一个对象
    std::tr1::shared_ptr<investment> pInv2(pInv1);

    // 同上，无任何改变
    pInv1 = pInv2;

    // pInv1和pInv2被销毁，它们所指的对象也就被自动销毁
}
```

> 解决auto_ptr的问题：由于tr1::shared_ptr的复制行为"一如预期"，因此，它们可以被用于STL容器。

注意：
`auto_ptr`和`tr1::shared_ptr`两者都在其析构函数内**做delete，而不是delete[]动作**，那么意味着，在动态分配而得的array身上使用auto_ptr或tr1::shared_ptr是个馊主意。

问题
```cpp
#include <iostream>
#include <string>
#include <memory>

using namespace std;

int main()
{  
  // 馊主意，会用上错误的delete形式，而且编译器不是提示编译错误
  std::auto_ptr<std::string> aps(new std::string[10]);
  std::shared_ptr<int> spi(new int[1024]);
}
```

你会发现：并没有特别针对“C++动态分配数组”而设计的类似auto_ptr或tr1::shared_ptr那样的东西。那是因为，`vecotr`和`string`几乎总是可以取代动态分配而得的数组。

> 请记住：
> * 为防止资源泄露，请使用`RAII对象`，它们在构造函数中获得资源，并在析构函数中释放资源。
> * 两个常被使用的RAII classes分别是`tr1::shared_ptr`和`auto_ptr`。前者通常是较佳选择，因为其copy行为比较直观。若选择auto_ptr，复制动作会使它（被复制物）指向null。

TODO scope_ptr



## 2 在资源管理中小心coping行为

问题：
`auto_ptr`和`shared_ptr`可以实现对`heap-based`资源的`RAII`，然后对于非`heap-based`的资源并不合适。因此，有时需要建立自己的`资源管理类`。

例如：
```cpp
void lock(Mutex* pm);     // 加锁
void unlock(Mutex* pm);   // 解锁
```
为了确保不会将一个被锁住的Mutex解锁，需要建立一个class用来管理锁。这样的class的基本结构由RAII守则支配，也就是“资源在构造期间获得，在析构期间释放”。

```cpp
class Lock
{
    public:
        // 获得资源
        explicit Lock(Mutex* pm) : mutexPtr(pm)
        { lock(mutexPtr); }
        // 释放资源
        ~Lock()
        { unlock(mutexPtr); }
       
    private:
        Mutex *mutexPtr;
};
```

客户在使用时：

```cpp
Mutex m; // 定义互斥器

// 建立一个区块用来定义critical section
{
    Lock m1(&m); // 锁定互斥器
                 // 执行critical section内的操作
                 // 在区块最末尾，自动解除互斥器锁定
}
```

> 问题：如果Lock对象被复制，会发生什么事情？

```cpp
Lock m1(&m);  // 锁定m
Lock m2(m1);  // 将m1复制到m2身上，会发生什么？
```

一般有**两种选择**：

* 禁止复制。

许多时候允许RAII对象被复制并不合理。如果复制动作对RAII class并不合理，你便应该禁止之。

* 对底层资源进行"引用计数法（reference-count）"

有时候，我们希望保有资源直到它的最后一个使用者被销毁。这种情况下，复制RAII对象时，应该将资源的“被引用数”递增。`shared_ptr`便是如此。

> 请记住：
> * 复制RAII对象，必须一并复制它所管理的资源。
> * 普通常见的RAII class copying行为是，抑制copying，使用引用计数。


## 3 在资源管理类中提供对原始资源的访问

> Provide access to raw resources in resource-managing classes.

### 问题
我们期望通过使用resource-managing classes对抗资源泄露，但是许多APIs直接指涉资源，导致下面问题。

```cpp
std::tr1::shared_ptr<Investment> pInv(createInvestment());

// 假设需要下面的函数处理Investment对象
int daysHeld(const Investment* pi);// 返回投资天数

// 正常需要这样调用
int days = daysHeld(pInv); // 错误，无法通过编译，因为daysHeld需要的是Investment* 指针，而我们传递的却是个类型为tr1::shared_ptr<Investment>的对象
```

### 解决方法

这个时候需要一个函数可将RAII class对象（`tr1::shared_ptr`）转换为其所内含之原始资源（`Investment*`）。有两个方法可以达到：

* 显示转换

`shared_ptr`和`auto_ptr`都提供了一个`get成员函数`（它会返回智能指针内部的原始指针），用来执行显式转换。

```cpp
int days = daysHeld(pInv.get()); // ok
```

* 隐式转换

`shared_ptr`和`auto_ptr`也重载了指针取值操作符（`operator->`和`operator*`），它们允许隐式转换至底部原始指针。

```cpp
bool taxable1 = !(pInv->isTaxFree());   // 经由operator->访问资源
bool taxable2 = !((*pInv).isTaxFree()); // 经由operator*访问资源
```

### 最佳实践

是否应该提供一个`显式转换函数`将`RAII class`转换为其底部资源，或是应该提供`隐式转换`，答案主要取决于`RAII class`被设计执行的特定工作，以及它被使用的情况。最佳的设计原则是：**让接口容易被正确使用，不易被误用**。

> 请记住
> * APIs往往要求访问原始资源（raw resources），所以每一个RAII class应该提供一个"取得其所管理之资源"的办法。
> * 对原始资源的访问可能经由显式转换或隐式转换。一般而言，显式转换比较安全，但隐式转换对客户比较方便。


## 4 成对使用new和delete时要采取相同形式

### 问题

```cpp
std::string* array = new std::string[100];
// ...
delete array;
```

上面array所含的100个string对象中的99个不太可能被适当删除，因为它们的析构函数很可能没有被调用。

当你使用`new`，有两件事情发生：
1. 内存被分配出来（通过`operator new`的函数）
2. 针对此内存会有一个（或更多）**构造函数**被调用

当你使用`delete`，也有两件事情发生：
1. 针对此内存会有一个（或更多）**析构函数**被调用
2. 内存被释放（通过`operator delete`的函数）

> delete的最大问题在于：即将被删除的内存究竟存有多少对象，这个问题的答案决定了有多少个析构函数必须被调用。

| 单一对象 | 对象数组
| -- | -- 
| object | n Object Object Object ...

当你对着一个指针使用delete，唯一能够让delete知道内存中是否存在一个数组大小记录的办法就是：**使用delete时加上中括号（方括号）**，delete便认定指针指向一个数组，否则，它便认定指针指向单一对象。

### 正确做法

```cpp
std::string* ptr1 = new std::string;
std::string* ptr2 = new std::string[100];

// ...

delete ptr1;    // 删除一个对象
delete [] ptr2; // 删除一个由对象组成的数组
```

> 请记住
> 如果调用`new`时使用`[]`，那么必须在对应调用`delete`时也使用`[]`。如果调用`new`时没有使用`[]`，那么也不应该在对应调用`delete`时使用`[]`。


## 5 以独立语句将newed对象置入智能指针

### 问题

```cpp
processWidget(std::tr1::shared_ptr<Widget>(new Widget), priority());
```

`new Widget`一定执行于`tr1::shared_ptr`构造函数被调用之前，因为这个表达式的结果还要被传递作为`tr1::shared_ptr`构造函数的一个实参，但对`priority`的调用则可以排在第一或第二或第三执行。如果编译器选择以第二执行它，则操作序列为：

1. 执行`new Widget`
2. 调用`priority`
3. 调用`tr1:shared_ptr`构造函数

> 如果对`priority`的调用导致异常，会发生什么？

在此情况下，`new Widget`返回的指针将会遗失，因为它尚未被置入`tr1::shared_ptr`内。因此，避免此类问题的办法是，**使用分离语句**。

```cpp
std::tr1::shared_ptr<Widget> pw(new Widget);
// 这个调用动作不会造成泄漏
processWidget(pw, priority());
```

> 请记住
> 以独立语句将`new`对象存储于智能指针内。如果不这样做，一旦异常被抛出，有可能导致难以察觉的资源泄漏。


# Effective C++ - Designs and Declarations


## 1 Make interfaces easy to use correctly and hard to use incorrectly

理想上，如果客户企图使用某个接口而却没有获得他所预期的行为，这个代码不该通过编译；如果代码通过了编译，它的作为就该是客户所想要的。

一个例子：假设你为一个用来表现日期的class设计构造函数。

```cpp
class Data {
public:
    Data(int month, int day, int year);
    // ...
};
```

咋见之下，这个接口通情达理（至少在美国如此），但它的客户很容易犯下至少两个错误。
1. 他们也许会以错误的次序传递参数。
2. 他们可能传递一个无效的月份或天数。

好的做法：
许多客户端错误可以因为导入新类型而获得`预防`。在防范"不值得拥有的代码"上，`类型系统（type system）`是你的主要同盟国。既然这样，我们可以导入简单的`外覆类型（wrapper types）`来区别天数，月份和年份，然后于`Data构造函数`中使用这些类型。

```cpp
struct Day {
explicit Day(int d) : val(d) {}
int val;
};

struct Month {
explicit Month(int m) : val(m) {}
int val;
};

struct Year {
explicit Year(int y) : val(y) {}
int val;
};

class Date {
public:
    Date(const Month& m, const Day& d, const Year& y);
    // ...
};

Date d(30, 3, 1995);                   // 错误，类型不匹配
Date d(Day(30), Month(3), Year(1995)); // 错误，类型不匹配
Date d(Month(3), Day(30), Year(1995)); // OK 
```

可见，明智而审慎地导入`新类型`对预防"接口被误用"有神奇疗效。但是，当保证了正确的类型后，如何限制其合理的值呢。例如，一年只有12个月，所以Month应该反映这一事实。一个办法是利用enum表现月份，但enum不具备我们系统拥有的类型安全性，例如，enum可被拿来当一个int使用。比较安全的做法是，`预先定义所有有效的Month`。

```cpp
class Month {
public:
    static Month Jan() { return Month(1); }
    static Month Feb() { return Month(2); }
    // ...
    static Month Dec() { return Month(12); }
    
private:
    explicit Month(int m);   // 阻止生成新的月份
};

Date d(Month::Mar(), Day(30), Year(1995));
```

其他预防方法还包括：

1. 限制类型内什么事可做，什么事不能做。常见的限制是加上`const`。
2. 除非有好理由，否则应该尽量令你的`types`的行为与内置`types`一致。
3. 提供行为一致的接口。STL容器的接口十分一致（虽然不是完美地一致），这使它们非常容易被使用。例如，每个STL容器都有一个名为`size`的成员函数，它会告诉调用者目前容器内有多少个对象。有些开发人员会以为IDE能使这些不一致变得不重要，但他们错了。不一致性对开发人员造成的心理和精神上的摩擦与争执，没有任何一个IDE可以完全抹除。

> 注意：任何接口，如果要求客户必须记得做某些事情，就是有着“不正确使用”的倾向。因为客户可能会忘记做那件事。

 例如：
```cpp
// 返回一个指针指向一个动态分配对象，为避免资源泄漏，返回的指针最终必须被删除，但客户有可能忘记
Investment* createInvestment();

// 将返回值存储于一个智能指针，因而将delete责任推给智能指针
std::tr1::shared_ptr<Investment> createInvestment();

```
返回`tr1::shared_ptr`让接口设计者得以阻止一大群客户犯下资源泄漏的错误。因为，`tr1::shared_ptr`允许当智能指针被建立起来时指定一个资源释放函数（所谓删除器，deleter）绑定于智能指针身上（但是，`auto_ptr`没有这个能力）。

比如，下面的方法：

```cpp
std::tr1::shared_ptr<Investment> createInvestment()
{
    std::tr1::shared_ptr<Investment> retVal(static_cast<Investment*>(0), getRidofInvestment);
    retVal = ...; // 令retVal指向正确对象
    return retVal;
}
```
`tr1::shared_ptr`构造函数坚持其第一个参数必须是个`指针`，而0不是指针，是个int。是的，它可以被转换为指针，使用`转型(cast)`可以解决这个问题。

> 任何事情都有两面性，`tr1::shared_ptr`使用上有什么副作用吗？

最常见的`tr1::shared_ptr`实现品来自`Boost`，`Boost`的`shared_ptr`是原始指针（raw pointer）的两倍大，以动态内存作为辅助。在许多应用程序中，这些额外的执行成本并不显著，然而其“降低客户错误”的成效却是每个人都可以看到的。

> 请记住：
> 1. 好的接口，很容易被正确使用，不容易被误用。
> 2. "促进正确使用"的办法包括，接口的一致性，内置类型的行为兼容。
> 3. “阻止误用”的办法包括，建立新类型，限制类型上的操作，束缚对象值，消除客户的资源管理责任。
> 4. `tr1::shared_ptr`支持定制删除器（custom deleter）。这可防范`cross-DLL problem`，可被用来自动解除互斥锁等等。 

## 2 Treat class design as type design

C++就像在其他OOP语言一样，当你定义一个新`class`，也就定义了一个新`type`。身为C++程序员，你的许多时间主要用来扩张你的类型系统。重载（overloading）函数和操作符、控制内存的分配和归还、定义对象的初始化和终结，等等，全部在你手上。

> 因此，你应该带着和"语言设计者当初设计语言内置类型时"一样的谨慎来研讨class的设计。

设计优秀的`classes`是一项艰巨的工作，因为设计好的`types`是一项艰巨的工作。好的`types`有自然的语法，直观的语义，以及一或多个高效实现品。

那么，如何设计高效地`classes`呢？几乎每一个`class`都要求你面对以下提问，你的回答往往导致你的设计规范：

> 1. **新type的对象应该如何被创建和销毁？**
> 这会影响到你的class的构造函数和析构函数，以及内存分配函数，和释放函数的设计。
> 2. **对象的初始化和对象的赋值该有什么样的差别？**
> 这个答案决定你的构造函数和赋值操作符的行为，以及期间的差异。很重要的是别混淆了“初始化”和"赋值"，因为它们对应于不同的函数调用。
> 3. **新type的对象如果被passed by value（以值传递），意味着什么？**
> 记住，copy构造函数用来定义一个type的`pass-by-value`该如何实现。
> 4. **什么是新type的“合法值”？**
> 对class的成员变量而言，通常只有某些数值集是有效的。那些数值集决定了你的class必须维护的约束条件（`invariants`），也就是决定了你的成员函数必须进行的错误检查工作。
> 5. **你的新type需要配合某个继承图系（inheritance graph）吗？**
> 如果你继承自某系既有的classes，你就受到那些classes的设计的束缚，特别是受到“它们的函数是`virtual`或`non-virtual`”的影响。如果你允许其他classes继承你的class，那会影响你所声明的函数，尤其是析构函数，是否为`virtual`。
> 6. **你的新type需要什么样的转换？**
> 你的type生存于其他一海票types之间，因而彼此该有转换行为吗？如果你希望允许类型T1之物被隐式转换为类型T2之物，就必须在class T1内写一个类型转换函数，或在class T2内写一个`non-explicit-one-argument`(可被单一实参调用)的构造函数。
> 7. **什么样的操作符和函数对此新type而言是合理的？**
> 这个问题的答案决定你将为你的class声明哪些函数。其中，某些该是memeber函数，某些则否。
> 8. **什么样的标准函数应该驳回？**
> 那些正是你必须声明为`private`者。
> 9. **谁该取用新type的成员？**
> 这个问题可以帮助你决定哪个成员为`public`，哪个为`protected`，哪个为`private`。它也帮助你决定哪一个classes，functions应该是`friends`，以及将它们嵌套于另一个之内是否合理。
> 10. **什么是新type的"未声明接口"（undeclared interface）？**
> 它对效率，异常安全性，以及资源运用提供何种保证？你在这些方面提供的保证将为你的class实现代码加上相应的约束条件。
> 11. **你的type有多么一般化？**
> 或许你其实并非定义一个新type，而是定义一整个types家族。果真如此，你应该定义一个新的`class template`。
> 12. **你真的需要一个新type吗？**
> 如果只是定义新的`derived class`以便为既有的class添加机能，那么说不定单纯定义一或多个non-member函数或templates，更能够达到目标。

> 请记住：
> 上述这些问题都不容易回答，所以定义出高效地classes是一种挑战。然而如果能够设计出至少像C++内置类型一样好的用户自定义classes，一切汗水便都值得。

## 3 Prefer pass-bye-reference-to-const  to pass-by-value

缺省情况下，C++以`by value`方式（一个继承自C的方式）传递对象至函数。除非你另外指定，否则函数参数都是以实际实参的副本为初值，而调用端所获得的亦是函数返回值的一个副本。这些`副本`系由`对象的copy构造函数`产出，这可能使得`pass-by-value`成为昂贵的操作。

例子：

```cpp
class Person {
public:
    Person();
    virtual ~Person();
private:
    std::string name;
    std::string address;
};

class Student: public Person {
public:
    Student();
    ~Student();
private:
    std::string schoolName;
    std::string schoolAddress;
};
```

现在考虑以下代码：
```cpp
bool validateStudent(Student s); // 函数以by value方式接受学生

Student plato;
bool platoIsOK = validateStudent(plato);
```

当上述函数被调用时，发生什么事？

无疑地Student的`copy构造函数`会被调用，以plato为蓝本将s初始化。同样，当validateStudent返回，s会被销毁。因此，**对此函数而言，参数的传递成本是：一个Student copy构造函数调用，加上一次Student析构函数调用。**

但那还不是故事的全部：

Student对象内有两个string对象，所以每次构造一个Student对象也就构造了两个string对象。此外，Student对象继承自Person对象，所以每次构造Student对象，也必须构造出一个Person对象。一个Person对象又有两个string对象在其中。

> **最终结果是**：以`by value`方式传递一个Student对象，会导致调用一次Student的`copy构造函数`、一次Person的`copy构造函数`、四次string的`copy构造函数`，同时，当函数内的那个Student*复件被销毁*，每一个构造函数调用动作都需要一个对应的`析构函数`调用动作。因此，总体成本是，**六次构造函数和六次析构函数**。

优化方法：

虽然上面的行为是正确的，但是不是推荐的。如何回避所有那些构造和析构呢？

```cpp
// pass by reference-to-const
bool validateStudent(const Student& s);
```
这种传递方式的效率高很多：没有任何构造函数和析构函数被调用，因为，没有任何新对象被创建。原先的的`by value`方式，调用者知道参数受到保护，函数内绝不会对传入的参数做任何改变，而只能对参数的复件（副本）做修改。而现在的`by reference`方式，将它声明为`const`是必要的，因此不这样的话调用者会忧虑函数内部会不会改变他们传入的参数。

同时，以`by reference`方式传递参数，也可以避免`slicing（对象切割）问题`。
> 当一个derived class对象以by value方式传递，并被视为一个base class对象。base class的copy构造函数会被调用，而"造成此对对象的行为像个derived class对象"的那些特化性质全被切割掉了，仅仅留下了一个base class对象。

```cpp
void printNameAndDisplay(const Window& w) // 很好，参数不会被切割
{
    std::cout << w.name();
    w.display(); // 现在，传进来的窗口是什么类型，w就表现出那种类型
}
```
如果窥视C++编译器的底层，`references`往往以指针实现出来，因此，`pass by reference`通常意味真正传递的是`指针`。如果对象为`内置类型`（例如，int），`pass by value`往往比`pass by reference`的效率高些。

例外：上面这个忠告，也适用于STL的迭代器和函数对象，因为，习惯上它们都被设计为`passed by value`。它们的设计者有责任看看它们是否高效且不受切割问题的影响。

> **请记住：**
> 一般而言，你可以合理假设`pass-by-value`并不昂贵的唯一对象，就是内置类型，以及STL的迭代器和函数对象。至于其他任何东西都请尽量以`pass-by-reference-to-const`替换`pass-by-value`。

## 4 Do not try to return a reference when you must return an object

虽然`pass-by-value`存在传值效率的问题，但是在某些情况下必须使用`pass-by-value`。

一个“必须返回新对象”的函数的正确写法是：就是让那个函数返回一个新对象
```cpp
inline const Rational operator * (const Rational& lhs, const Rational& rhs)
{
    return Rational(lhs.n * rhs.n, lhs.d * rhs.d);
}
```
你需要承受`operator *`返回值的构造成本和析构成本，然而长远来看那只是为了获得正确行为而付出的一个小小代价。

但是，别忘了C++允许编译器实现者施行最优化，用以改善产出码的效率却不改变其可观察的行为。因此，某些情况下`operator *`返回值的构造和析构可被安全地消除。

**总结：**
当你必须在"返回一个reference和返回一个object之间抉择时"，你的工作就是选出行为正确的那个。让编译器厂商为"尽可能降低成本"鞠躬尽瘁，你可以享受你的生活。

> 请记住
> 绝不要返回pointer或reference指向一个local stack对象，或返回reference指向一个heap-allocated对象，或返回pointer或reference指向一个local static对象。

## 5 Declare data members private

切记将成员变量声明为`private`。这可赋予客户访问数据的一致性、可细微划分访问控制、许诺约束条件获得保证，并提供class作者以充分的实现弹性。

`protected`并不比`public`更具封装性。

## 6 Prefer non-member non-friend functions to member functions

有时，选择`member`函数，还是`non-member`函数好呢？

能够访问`private`成员变量的函数只有class的`member`函数，以及`friend`函数。如果你要在一个`member`函数和一个`non-member`，`non-friend`函数之间做抉择，而且两者提供相同机能，那么，导致较大封装的是`non-member`和`non-friend`函数，因为它并不增加“能够访问class内之private成分”的函数数量。

在C++，比较自然的做法是让`clearBrowser`成为一个`non-member`函数，并且位于`WebBrowser`所在的同一个`namespace`内。

```cpp
namespace WebBrowserStuff {
    class WebBrowser { // ... };
    void clearBrowser(WebBrowser& wb);
    // ...
}
```

`namespace`和`classes`不同，前者可以跨多个源码文件，而后者不能。

将所有便利函数，放在多个头文件内，但隶属同一个命名空间。意味着客户可以轻松扩展这一组便利函数。他们需要做的就是添加更多`non-member`和`non-friend`函数到此命名空间内。

## 7 Declare non-member functions when type conversions should apply to all parameters

混合式算数运算：

方法：让`operator*`成为一个`non-member`函数，这样允许编译器在每一个实参上执行隐式类型转换。
```cpp
class Rational {
   // ...
};

const Rational operator* (const Rational& lhs, const Rational& rhs)
{
    return Rational(lhs.numerator() * rhs.numerator(),
                    lhs.denominator() * rhs.denominator());
}

Rational oneFourth(1, 4);
Rational result;
result = oneFourth * 2;   // ok
result = 2 * oneFourth;   // 也可以支持
```

## 8 Consider support for a non-throwing swap

缺省情况下，标准程序库提供的swap算法如下：

```cpp
namespace std {
    template<typename T>
    void swap(T& a, T& b)
    {
        T temp(a);
        a = b;
        b = temp;
    }
}
```

只要类型`T`支持`copying`（copy构造函数和copy assignment操作符），缺省的swap实现代码就会帮你置换类型`T`的对象，你不需要为此另外再做任何工作。

> 问题
但是，缺省的swap实现版本会涉及三个对象的复制，对某些类型而言（pimpl），这些复制操作无一必要。

`pimpl手法`：就是`pointer to implementation`，即，以指针指向一个对象，内含真正的数据。

```cpp
class WidgetImpl {
public:
    // ...
private:
    int a, b, c;
    std::vector<double> v;
    // 更多数据，意味着复制时间很长
};

class Widget {
public:
    Widget(const Widget& rhs);
    Widget& operator=(const Widget& rhs) {
        *pImpl = *(rhs.pImpl);
        // ...
    }
private:
    WidgetImpl* pImpl;   // 指针，所指对象内含Widget数据
};
```

> 问题
> 置换两个Widget对象值，我们唯一需要做的就是置换其`pImpl`指针，但是，缺省的swap算法不知道这一点，它不只复制三个Widgets，还复制三个WidgetImpl对象，效率非常低。

我们希望，能够告诉`std::swap`，当`Widgets`被置换时真正该做的是置换其内部的`pImpl`指针。

一种做法是：**将`std::swap`针对`Widget`特化。**

```cpp
class Widget {
public:
    void swap(Widget& other) {
        using std::swap;           // 必要
        swap(pImpl, other.pImpl);
    }
};

// 特化
namespace std {
    template<>
    void swap<Widget>(Widget& a, Widget& b) {
        a.swap(b);
    }
}
```

# Effective C++ - Implementations


## 1 尽可能延后变量的定义

只要你定义了一个变量，而其类型带有一个构造函数或析构函数，那么当程序的控制流到达这个变量定义式时，你便要承受构造成本；当这个变量离开作用域时，便要承受析构成本。

例子：

方法A

```cpp
Widget w;
for (int i = 0; i < n; ++i) {
    w = "取决于i的某个值";
    // ...
}
```

方法B

```cpp
for (int i = 0; i < n; ++i) {
    Widget w("取决于i的某个值");
    // ...
}
```
上面两种方法，哪种好？
方法A：1个构造函数 + 1个析构函数 + n个赋值操作
方法B：n个构造函数 + n个析构函数

因此，除非你知道`赋值成本`比`构造+析构`成本低，否则，你应该使用方法B。

## 2 尽量少做转型动作

C++规则的设计目标之一是，**保证"类型错误"绝不可能发生**。理论上，如果你的程序很"干净地"通过编译，就表示它并不企图在任何对象身上执行任何不安全，无意义，愚蠢荒谬的操作。**这是一个极具价值的保证，可别草率地放弃它**。

不幸的是，`转型（cast）`破坏了类型系统。那可能导致任何种类的麻烦，有些容易识别，有些非常隐晦。**在C++中`转型`是一个你会想带着极大尊重去亲近的一个特性。(意思是，坑比较多)**

### 转型语法

1. 旧式转型（C风格）

```cpp
(T) expression;   // 将expression转型为T
T(expression);    // 同上
```

2. C++的风格

```cpp
// 通常被用来将对象的常量性移除（cast away the constness）
const_cast<T>(expression);

// 主要用来执行"安全向下转型"（safe downcasting），也就是用来决定某对象是否归属继承体系中的某个类型。它是唯一无法由旧式转型执行的动作，也是唯一可能耗费重大运行成本的转型动作
dynamic_cast<T>(expression);

// 低级转型。实际动作及结果，可能取决于编译器，也就表示它不可移植
reinterpret_cast<T>(expression);

// 用来强迫隐士转换（implicit conversions）。例如，将non-const对象转为cosnt对象，或将int转为double。但是，它无法将const转换为non-const，这个只有const_cast才能办到
static_cast<T>(expression);
```

旧式转型仍然合法，但**新式转型更受欢迎** 。原因是：
* 它们很容易在代码中识别出来，不论是人工识别还是使用工具如grep，因此可以简化"找出类型系统在哪个点被破坏的过程"。
* 各转型动作的目标愈窄化。编译器可能诊断出错误的运用。例如，如果你打算将常量性去掉，除非使用新式转型中的const_cast，否则无法编译通过。

> 注意：许多程序员认为转型其实什么都没做，只是告诉编译器把某种类型视为另一种类型。这是错误的观念。任何一个类型转换（不论是通过转型操作而进行的显示转换，或通过编译器完成的隐式转换），往往真的令编译器编译出运行期间执行的代码。

例子：

```cpp
class Base { // ... };
class Derived: public Base { // ... };

Derived d;
Base* pb = &d; // 隐式地将Derived* 转换为Base*
```

这里建立了一个base class指针指向一个derived class对象，**但有时候上述的两个指针值并不相同。这种情况下，会有一个偏移量在运行期被施行于Derived*指针身上，用以取得正确的Base*指针值。**


> 上面这个例子表明：单一对象（例如，一个类型为Derived的对象）可能拥有一个以上的地址（例如，以Base*指向它时的地址和以Derived*指向它时的地址）。C，Java，C#都不可能发生这种事，但C++可以。实际上，一旦使用多重继承，这事几乎一直发生着。即使在单一继承中也可能发生。意味着，你通常应该避免做出“对象在C++中如何布局”的假设。当然更不该以此假设为基础执行任何转型动作。例如，将对象地址转型为`char*`指针然后在它们身上进行指针算术，这几乎总是会导致`无定义`不明确的行为。

### 尽量避免使用`dynamic_cast`

之所以需要`dynamic_cast`，通常是因为你想在一个你认定为`derived class`对象身上执行`derived class`操作函数，**但是你手上却只有一个"指向base"的pointer或reference。**你只能靠它们来处理对象。

有**两个方法**可以避免这个问题：

1. **使用容器并在其中存储直接指向derived class对象的指针（通常是智能指针）**，如此便消除了“通过base class”接口处理对象的需要。（但是，这种做法使你无法在同一个容器内存储指针，指向所有可能之各种派生类，如果真要处理多种派生类对象，那就需要多个容器）

2. **在base class内提供virtual函数做你想对各个派生类做的事，即，虚函数的方法。**

例如：

```cpp
class Base {
public: 
    virtual void dosomething() {} // 空实现
};

class Derived : public Base {
public:
    virtual void dosomething() {
        // 真正的实现
    }
};

typedef std::vector<std::tr1::shared_ptr<Base> > base_ptr_t;
base_ptr_t bp;
// ...

for (base_ptr_t::iterator iter = bp.begin(); iter != bp.end(); ++iter) {
    (*iter)->dosomething();  // 注意，这里没有使用dynamic_cast，而使用虚函数的特性
}
```

> 请记住
> * 如果可以，尽量避免转型，特别是在注重效率的代码中避免dynamic_casts。如果有个设计需要转型动作，**试着发展无需转型的代替设计**。
> * **如果转型是必要的，试着将它隐藏于某个函数背后。**客户随后可以调用该函数，而不需将转型放进他们自己的代码内。
> * 宁可使用`C++-style（新式）转型`，不要使用旧式转型。前者很容易辨识出来，而且也比较有着分门别类的职掌。

## 3 避免返回handles指向对象内部成分

不论handle是个指针，或迭代器，或reference，也不论这个handle是否为const，也不论那个返回handle的成员函数是否为const。这里的唯一关键是，有个handle被传出去了，一旦如此你就暴露在**handle比其所指对象更长寿**的风险下。

例子：
```cpp
#include <iostream>

class Point {
public:
  Point(int x, int y) {
    m_x = x;
    m_y = y;
  }
  void setX(int newVal) {
    m_x = newVal;
  }
  void setY(int newVal) {
    m_y = newVal;
  }

  void show() const {
    std::cout << m_x << "," << m_y << std::endl;
  }

private:
  int m_x;
  int m_y;
};

class PointMgr {
public:
  PointMgr() : m_point(1, 1) {
  }
  
  // error: binding of reference to type 'Point' to a value of type 'const Point' drops qualifiers
  //Point& getPoint() const {

#if 0
  // ok, but not suggested
  Point& getPoint() {
    return m_point;
  }
#endif

  // ok, suggested
  const Point& getPoint() const {
    return m_point;
  }


  void showPoint() const {
    m_point.show();
  }

private:
  Point m_point;
};

int main()
{
  PointMgr point_mgr;
  point_mgr.showPoint(); // 1,1

  // error
  //point_mgr.getPoint().setX(2);
  //point_mgr.getPoint().setY(2);
  
  point_mgr.getPoint().show(); // 1,1

}
```

**例外：**
这并不意味你绝对不可以让成员函数返回handle。有时候你必须这么做。例如，`operator[]`就允许你获取strings和vectors的个别元素，而这些`operator[]s`就是返回`reference指向容器内的数据`，那些数据会随着容器被销毁而销毁。尽管如此，这样的函数毕竟是例外，不是常态。

> 请记住
> 避免返回handles（包括reference，指针，迭代器）指向对象内部。**遵循这个条款可增加封装性**，帮助const成员函数的行为像个const，**并将发生“虚吊号码牌”（dangling handles）的可能性降至最低**。

## 4 为"异常安全"而努力是值得的

> Strive for exception-safe code.

一个不符合异常安全的代码：
```cpp
void PrettyMenu::changeBackground(std::istream& imgSrc)
{
    lock(&mutex);                 // 取得互斥器
    delete bgImage;               // 摆脱旧的背景图像
    ++imageChanges;               // 修改图像变更次数
    bgImage = new Image(imgSrc);  // 安装新的背景图像
    unlock(&mutex);               // 释放互斥器
}
```

异常安全有**两个条件**：当异常被抛出时，带有异常安全性的函数会：
1. **不泄露任何资源**。上述代码中，一旦`new Image(imgSrc)`导致异常，对`unlock`的调用就绝不会执行，于是互斥器就永远被把持住了。
2. **不允许数据破坏**。如果`new Image(imgSrc)`抛出异常，`bgImage`就是指向一个已被删除的对象，`imageChanges`也已被累加，而其实并没有新的图像被成功安装其起来。

**异常安全函数（`Exception-safe functions`）**提供以下**三个保证之一**：

* **基本承诺**。如果异常被抛出，程序内的任何事物仍然保持在有效状态。没有任何对象或数据结构会因此而败坏。所有对象都处于一种内部前后一致的状态。
*  **强烈保证**。如果异常抛出，程序状态不改变。调用这样的函数需要有这样的认知：如果函数成功，就是完全成功；如果函数失败，程序会恢复到“调用函数之前”的状态。
*  **不抛掷（`nothrow`）保证**。承诺绝不抛出异常，因为它们总是能够完成它们原先承诺的功能。作用于内置类型身上的所有操作都提供`nothrow`保证。

> 异常安全代码，必须提供上述三种保证之一。如果它不这样做，它就不具备异常安全性。

修改后，异常安全地代码（强烈保证）：
有个一般化的设计策略，可以很典型地会导致`强烈保证`，这个策略被称为`copy and swap`。（原则很简单，为你打算修改的对象（原件）做出一份副本，然后在那副本身上做一切必要修改，若有任何修改动作抛出异常，原对象仍保持未改变状态。待所有改变都成功后，再将修改过的那个副本和原对象在一个不抛出异常的操作中置换（swap））

**pimpl idiom**
实现上，通常是将所有"隶属对象的数据"从原对象放进另一个对象内，然后赋予原对象一个指针，指向那个所谓的实现对象。

```cpp
struct PMImpl {
    std::tr1::shared_ptr<Image> bgImage; // PMImpl = PrettyMenu Impl
    int imageChanges;
};

class PrettyMenu {
public:
    // ...
private:
    Mutex mutex;
    std::tr1::shared_ptr<PMImpl> pImpl;
};

void PrettyMenu::changeBackground(std::istream& imgSrc)
{
    using std::swap;
    Lock ml(&mutex);
    
    std::tr1::shared_ptr<PMImpl> pNew(new PMImpl(*pImpl)); // 获取副本
    pNew->bgImage.reset(new Image(imgSrc));                // 修改副本
    ++pNex->imageChanges;

    swap(pImpl, pNew);  // 置换数据，释放mutex
}
```

> 请记住
> 1. 异常安全函数（Exception-safe functions）即使发生异常，也不会泄露资源，或允许任何数据结构败坏。这样的函数区分为三种可能的保证：**基本型、强烈型、不抛异常性**。
> 2. “强烈保证”往往能够以`copy-and-swap`实现出来，但"强烈保证"并非对所有函数都可实现，或具备现实意义（时间和空间成本）。
> 3. 函数提供的"异常安全保证"，通常最高只等于其所调用之各个函数的“异常安全保证”中的**最弱者**。

## 5 透彻了解inlining的里里外外

Inline函数，可以调用它们又不需蒙受函数调用所招致的额外开销。

> 没有白吃的午餐

inline函数，背后的整体观念是，将“对此函数的每一个调用”都以函数体替换之。但这样做可能增加你的目标代码大小。在一台内存有限的机器上，过度热衷inlining会造成程序体积太大，即使拥有虚内存，inline造成的代码膨胀亦会导致额外的换页行为，降低指令高速缓存装置的命中率（instruction cache hit rate），以及伴随而来的效率损失。

> 记住
> inline只是对编译器的一个申请，不是强制命令。这项申请可以`隐喻提出`，也可以`明确提出`。

* 隐喻方式是将函数定义于class定义式内。

例如：

```cpp
class Person {
public:
  int age() const { return theAge }; // 一个隐喻的inline申请，age被定义于class定义式内
private:
  int theAge;
};
```

* 明确声明inline函数的做法则是在其定义式前加上关键字inline。

例如：标准的`max template`（来自`<algorithm>`）

```cpp
template<typename T>
inline const T& std::max(const T& a, const T& b)
{
    return a < b ? b : a;
}
```

总结：

1. 一个表面上看似inline的函数是否真是inline？取决于你的建置环境，主要取决于编译器。
2. 编译器通常不对"通过函数指针而进行的调用"实施inlining（编译器没有能力提出一个指针指向并不存在的函数）。
3. 构造函数和析构函数，是否选择inline化，并非是个轻松的决定。因为空的构造函数里，编译器可能会做很多事情。
4. 影响升级。`inline`函数无法随着程序库的升级而升级，也就是，如果f是程序库内的一个inline函数，客户将f函数本体编进其程序中，一旦程序库设计者决定改变f，所有用到f的客户端程序都必须重新编译。这往往是大家不愿意见到的。然而，如果f是`non-inline`函数，一旦它有任何修改，客户端只需要重新连接就好，远比重新编译的负担少的多。如果程序采取动态链接，升级版函数甚至可以不知不觉地被应用程序吸纳。
5. 可能影响调试。大部分调试器面对inline函数都束手无策。毕竟你如何在一个并不存在的函数内设立断点呢？虽然某些建置环境勉力支持对inlined函数的调试，其他许多建置环境仅仅只能“在调试版程序中禁止发生inlining”。
6. 80-20经验法则。平均而言，一个程序往往将80%的执行时间花费在20%的代码上头。这是一个重要的法则，因为它提醒你，作为一个软件开发者，你的目标是找出这可以有效增进程序整体效率的20%代码，然后将它inline或者竭尽所能地将它瘦身。但除非你选对目标，否则一切都是虚功。

> **请记住**
> 1. 将大多数inlining限制在小型，被频繁调用的函数身上。这可使日后的调试过程和二进制升级（binary upgradability）更容易，也可使潜在的代码膨胀问题最小化，使程序的速度提升机会最大化。
> 2. 不要只因为function templates出现在头文件，就将它们声明为inline。

## 6 将文件间的编译依存关系降至最低

```cpp
int main()
{
    int x;              // 定义一个int
    Person p(params);   // 定义一个Person
}
```

当编译器看到`x`的定义式，它知道必须分配多少内存（通常位于stack内）才能够持有一个`int`。（每个编译器都知道`int`有多大）

当编译器看到`p`的定义式，它也知道必须分配足够空间以放置一个`Person`，但是，它如何知道一个`Person`对象有多大呢？编译器获得这项信息的唯一办法就是询问**class定义式**。然而，如果**class定义式**可以合法地不列出实现细目，编译器该如何知道分配多少空间呢？

> 对于C++代码，你可以：将对象实现细目隐藏于一个指针背后。

针对`Person`我们可以这样做：把`Person`分隔为两个classes，一个只**提供接口**，另一个负责**实现该接口**。

例如：

```cpp
class PersonImpl;   // Person实现类的前置声明
class Date;
class Address;

class Person {
public:
    Person(const std::string& name, const Date& birthday, const Address& addr);
    std::string name() const;
    std::string birthDate() const;
    std::string address() const;

private:
    std::tr1::shared_ptr<PersonImpl> pImpl; // 指针，指向实现物，隐藏实现细节
};
```

`Person`class只内含一个指针成员，指向其实现类`PersonImpl`。这种设计被称为：**pimpl idion (pimpl是 Pointer to implementation的缩写)**。这样的设计下，`Person`的客户端就完全与`Date`,`Addresses`以及`Persons`的实现细节分离了。这些class的任何实现修改都不需要`Person`客户端重新编译。同时，由于客户无法看到`Person`的实现细节，也就不会写出什么：取决于内部细节的代码。**这真正是“接口与实现分离”**。

> **分离的关键在于**：以“声明的依赖性”代替“定义的依赖性”。现实中，让头文件尽可能自我满足，万一做不到，则让它与其他文件内的声明式（而非定义式）相依。
> 

其他每一件事，都源于这个简单的设计策略：
1，如果使用`object references`或`object pointers`可以完成任务，**就不要使用**`object`。（你可以只靠一个类型声明式，就定义出指向该类型的`references`和`pointer`；但如果定义某类型的`objects`，就需要用到该类型的定义式）。
2，如果能够，尽量以class声明式替换class定义式。（注意，当你声明一个函数，而它用到某个class时，你并不需要该class的定义，即使函数以`by value`的方式传递该类型参数或返回值）。

例如：定义func函数，但不需要Person的定义。但是，在调用func函数时，就需要知道Person的定义。也就是，比如一个函数库有非常多的函数，但是我们可能只用到了其中很少的函数，对我们用到的函数，在客户端通过前置声明的方式（而不是包含所有定义的方式），可以减少对不必要类型定义的依赖。
```cpp
#include <stdio.h>

class Person;
void func(Person &p)
{
  printf("func\n");
}

int main()
{
  printf("main\n");
  return 0;
}
$g++ -o declare_var declare_var.cpp 
$./declare_var 
main
```

3，为声明式和定义式提供不同的头文件。为了促进严守上述准则，需要两个头文件，一个用于声明式，一个用于定义式。当然，这些文件必须保持一致性，如果有一个声明式被改变了，两个文件都得改变。**因此，程序库客户应该总是#include一个声明文件而非前置声明若干函数，程序库作者也应该提供这两个头文件。**

例如：
C++标准程序库头文件`<iosfwd>`内含`iostream`各组件的声明式，其对应定义则分布在若干不同的头文件内，包括`<sstream>`，`<streambuf>`，`<fstream>`和`<iostream>`。

### Handle classes
 像`Person`这样使用`pimpl idiom`的classes，往往被称为`Handle classes`。意思是，对于`Person`这样的class，如果要做点实事：

一种办法是，将它们的所有函数转交给相应的实现类，并由后者完成实际工作。

例如：下面是`Person`两个成员函数的实现。

```cpp
#include "Person.h"
#include "PersonImpl.h"

Person::Person(const std::string&name, const Date& birthday, const Address& addr) : pImpl(new PersonImpl(name, birthday, addr))
{}

std::string Person::name() const
{
    return pImpl->name();   // 相同的名字
}
```

### Interface classes

另一种办法是，令`Person`成为一种特殊的`abstract base class(抽象基类)`，称为"Interface class"。这种class的目的是详细一一描述`derived classes`的接口，因此它通常不带成员变量，也没有构造函数，只有一个`virtual`析构函数，以及一组`pure virtual`函数，用来叙述整个接口。

例如：

```cpp
class Person {
public:
    virtual ~Person();
    virtual std::string name() const = 0;
    virtual std::string birthDate() const = 0;
    virtual std::string address() const = 0;
    // ...
};

class RealPerson: public Person {
public:
    RealPerson(const std::string& name, const Date& birthday, const Address& addr) : theName(name), theBirthDate(birthday), theAddress(addr)
    {}
   
    virtual ~RealPerson() {}
    std::string name() const;
    std::string birthDate() const;
    std::string address() const;

private:
    std::string theName;
    Date theBirthDate;
    Address theAddress;
};
```

### Handle classes和Interface classes的利弊

> **利**
> Handle classes和Interface classes**解除了接口和实现之间的耦合关系，从而降低文件间的编译依赖。**

> **弊**
> 但是，这种设计使你在运行期丧失了若干速度，同时，又让你为每个对象超额付出若干内存。

在Handle classes身上，成员函数必须通过implementation pointer取得对象数据，那会为每一次访问增加一层间接性。而每一个对象消耗的内存数量必须增加implementation pointer的大小。

在Interface classes身上，由于每个函数都是virtual，所以你必须为每次函数调用付出一个间接跳跃成本。此外，Interface class派生的对象必须内含一个vptr，这个指针可能会增加存放对象所需的内存数量。

Handle classes和Interface classes，由于设计上用来隐藏实现细节，因此无法实现`inline`优化。

那是否应该使用Handle classes和Interface classes呢？你应该考虑以渐进的方式使用这些技术。在程序发展过程中使用，以求实现代码有变化时，对客户端带来最小的冲击。而当它们导致速度或大小差异成为主要矛盾时，就用具象类（concrete classes）替换Handle classes和Interface classes。

> **请记住**
> 1. 支持"编译依赖最小化"的一般构想是：相依于声明式，不要相依于定义式。基于此构想的两个手段是Handle classes和Interface classes。
> 2. 程序库头文件应该以"完全且仅有声明式"的形式存在。这种做法不论是否涉及templates都适用。


# Effective C++ - Inheritance and Object-Oriented Design

> OOP面向对象编程（继承，单一继承，多重继承，public/protected/private，virtual/non-virtual，继承的查找规则，virtual函数是最佳选择吗，等等）有哪些坑？

## 1 确定你的public继承是is-a关系

Make sure public inheritance models "is-a".

例子：

```cpp
class Person {
    // ...
};

class Student: public Person {
    // ...
};
```

每个学生都是人，但并非每个人都是学生。人的概念比学生更一般化，学生是人的一种特殊形式。

> 这个论点，只对`public`继承才成立。只有当Student以public形式继承Person，C++的行为才如上述描述。`private`继承的意义与此完全不同，至于`protected`继承，其意义更加困惑。

> 请记住
> public继承意味`is-a`。适用于`base classes`身上的每一件事情一定适用于`derived classes`身上，因为，每一个`derived class`对象也都是一个`base class`对象。

## 2 避免遮掩继承而来的名称

```cpp
#include <iostream>

class Base {
public:
  virtual void f1() = 0;
  virtual void f1(int) {
    std::cout << "virtual void Base::f1(int)\n";
  }
  virtual void f2() {
    std::cout << "virtual void Base::f2()\n";
  }
  void f3();
    
};

class Derived: public Base {
public:

  // 让Base class内名为f1的函数在Derived class作用域内可见，如果不这样声明，下面d.f1(1)会找不到
  using Base::f1;

  virtual void f1() {
    std::cout << "virtual void Derived::f1()\n";
  }
  void f3() {
    std::cout << "void Derived::f3()\n";
  }
};

int main()
{
  Derived d;

  d.f1();
  d.f1(1); // error ?
  d.f2();
  d.f3();

  return 0;
}
/*
virtual void Derived::f1()
virtual void Base::f1(int)
virtual void Base::f2()
void Derived::f3()
 */
```

>  请记住
>  * derived classes内的名称会遮掩base classes内的名称。在public继承下，正常是不希望被遮掩的。
>  * 为了让遮掩的名称可见，可以使用`using`声明来到达目的（如上述例子）。

## 3 区分接口继承和实现继承

Differentiate between inheritance of interface and inheritance of implementation.

表面上直接了当的public继承概念，经过更严密的检查之后，发现它由两部分组成：

* 函数接口（function interfaces）继承
* 函数实现（function implementations）继承

```cpp 
class Shape {
public:
    // 三种被继承的接口
    virtual void draw() const = 0;
    virtual void error(const std::string& msg);
    int objectID() const;
    // ...
};

class Rectangle: public Shape {
    // ...
};

class Ellipse: public Shape {
    // ...
};
```

Shape是一个抽象class，它的pure virtual函数draw使它成为一个抽象class。所以客户不能够创建Shape class的实体，只能创建其derived classes的实体。

三种以public继承的接口，含义是不一样的：

1. 成员函数接口总是会被继承。
2. 声明一个`pure virtual`函数的目的是，为了让derived classes只继承函数接口。
3. 声明简朴的`impure virtual`函数的目的，是让derived classes继承该函数的接口和缺省实现。
4. 声明`non-virtual`函数的目的，是为了令derived classes继承函数的接口，和一份强制性实现。

## 4 考虑virtual函数以外的其他选择

Consider alternatives to virtual functions.

```cpp
class GameCharacter {
public:
    virtual int healthValue() const;  // 返回游戏中人物的健康指数，derived classes可以重新定义此函数
    // ...
};
```

healthValue并未被声明为`pure virtual`，这暗示我们将会有个计算健康指数的**缺省算法**。

一些替代方案：

### 4. 1 Non-Virtual Interface (NVI)

就是，令客户通过public non-virtual成员函数间接调用private virtual函数。这样做的好处是，可以在public non-virtual函数（也就是virutal函数的wrapper函数）中完成一些事前和事后的工作。

```cpp
class GameCharacter {
public:
    int healthValue() const { // 返回游戏中人物的健康指数，derived classes不重新定义此函数

        do_something_before();
        int ret = doHealthValue();   // 做真正的工作
        do_something_after();
    }
    
private:
    virtual int doHealthValue() const { // derived classes可以重新定义它
        // 缺省实现
    }
};
```

### 4.2 Function Pointers 实现 Strategy 模式

这种方法的思路是，人物健康指数的计算与人物类型无关。这样的计算完全不需要人物这个成分。例如，我们可能会要求每个人物的构造函数接受一个指针，指向一个健康计算函数，而我们可以调用该函数进行实际计算。

```cpp
class GameCharacter;   // 前置声明 forward declaration

int defaultHealthCalc(const GameCharacter& gc);

class GameCharacter {
public:
    typedef int (*HealthCalcFunc) (const GameCharacter&);
    explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc) : healthFunc(hcf) {
        // init
    }
    
    int healthValue() const {
        return healthFunc(*this);
    }

    // ...

private:
    HealthCalcFunc healthFunc;   // 函数指针
};
```

这种实现更加具有弹性：

1. 同一个人物类型的不同实例，可以有不同的健康计算函数。

2. 某已知人物的健康指数计算函数，可以在运行时期变更。

> 事物都有两面性：

一般而言，唯一能够解决，需要以`non-member`函数访问class的`non-public`成分的办法就是：**弱化class的封装**。例如，class可声明那个`non-member`函数为`friends`，或是为其实现某一部分提供public访问函数。

运用**函数指针**替换**virtual函数**。其**优点**是，每个对象可各自拥有自己的健康计算函数和可在运行时期改变计算函数；而**缺点**，是可能必须降低类的封装性。

### 4.3 tr1::function 实现 Strategy 模式

一旦习惯了`templates`，以及它们对隐式接口的使用，基于`函数指针`的做法看起来便过分苛刻而死板了。

> 为什么要求“健康指数的计算”必须是函数，而不能是某种"像函数的东西"，例如，函数对象。如果我们不再使用函数指针，而是改用一个类型为`tr1::function`的对象，这些约束就全部挥发不见了。这样的对象，可以持有任何可调用物（也就是，函数指针、函数对象、成员函数指针）。




TODO





