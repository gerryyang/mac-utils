---
layout: post
title:  "Inside the C++ Object Model (深度探索 C++对象模型)"
date:   2024-05-13 12:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}


# 简单对象内存分布

``` cpp
#include <iostream>
using namespace std;

class Basic
{
public:
    int a;
    double b;
};

int main()
{
    Basic tmp;
    tmp.a = 1;
    return 0;
}
```

使用 GDB 查看对象 `tmp` 的内存布局如下：`int` 类型在 **x86-64** 平台上占用 **4** 个字节，而 `double` 成员的起始地址与 `int` 成员的起始地址之间相差 **8** 个字节，说明在 `a` 之后存在内存对齐填充，具体取决于编译器的实现细节和平台的对齐要求。内存对齐要求数据的起始地址在某个特定大小的倍数上，这样可以优化硬件和操作系统访问内存的效率。**因为许多处理器访问对齐的内存地址比访问非对齐地址更快**。在不进行内存对齐的情况下，较大的数据结构可能会跨越多个缓存行或内存页边界，这会导致额外的缓存行或页的加载，降低内存访问效率。不过大多时候我们不需要手动管理内存对齐，编译器和操作系统会自动处理这些问题。

![object_model](/assets/images/202405/object_model.png)

# 带方法的对象内存分布

``` cpp
#include <iostream>

class Basic
{
public:
    int a;
    double b;

    void setB(double value)
    {
        b = value;
    }
};

int main()
{
    Basic tmp;
    tmp.a = 1;
    tmp.setB(3.14);
    return 0;
}
```

文本段 (代码段) 是存储程序执行代码的内存区域，通常是只读的，以防止程序在运行时意外或恶意修改其执行代码。

![object_model2](/assets/images/202405/object_model2.png)

这里 `setB` 方法地址就是位于程序的文本段内，可以在 GDB 中用 `info target` 验证一下。

![object_model3](/assets/images/202405/object_model3.png)

其中 `setB` 刚好在 `.text` 段的地址范围。因此，成员方法存储在进程的文本段，添加成员方法不会改变类实例对象的内存布局大小，它们也不占用对象实例的内存空间。


# 成员变量寻址

`setB` 的函数原型为 ` (void (*)(Basic * const, double))`，函数的第一个参数是 `Basic*` 指针，然而在代码中的调用是 `temp.setB(3.14)`，这种用法其实是一种语法糖，编译器在调用成员函数时自动将当前对象的地址作为 `this` 指针传递给了函数。参数传递了对象的地址，但是在函数里面是怎么拿到成员变量 `b` 的地址呢？在调用 `setB` 的地方打断点，执行到断点后，用 `step` 进入到函数，然后查看相应**寄存器的值**和汇编代码。

```
(gdb) p &Basic::setB(double)
$7 = (void (*)(Basic * const, double)) 0x4011c4 <Basic::setB(double)>
```

![object_model4](/assets/images/202405/object_model4.png)

汇编代码展示了如何通过 `this` 指针和偏移量访问 `b`。可以分为两部分：

* 第一部分是处理 `this` 指针和参数。

参数传递部分。通过 `mov %rdi,-0x8(%rbp)` 将 `this` 指针（通过 `rdi` 寄存器传入）保存到**栈**上。将 `double` 类型的参数 `value` 通过 `xmm0` 寄存器传入保存到**栈**上。这是 x86_64 机器下 GCC 编译器的传参规定，可以通过打印 `$rdi` 保存的地址来验证是 `tmp` 对象的开始地址。

* 第二部分是找到成员 `b` 的内存位置然后进行赋值。

对象赋值部分。`mov -0x8(%rbp),%rax` 将 `this` 指针从**栈**上加载到 `rax` 寄存器中。类似的，`movsd -0x10(%rbp),%xmm0` 将参数 value 从**栈**上重新加载到 `xmm0` 寄存器中。`movsd %xmm0,0x8(%rax)` 将 `value` 写入到 `this` 对象的 `b` 成员。这里 `0x8(%rax)` 表示 `rax`（即 `this` 指针）加上 **8 字节的偏移**，这个偏移正是成员变量 `b` 在 `Basic` 对象中的位置。

> 这个 8 字节的偏移是怎么算出来的呢？其实成员变量的地址相对于对象地址是固定的，对象的地址加上成员变量在对象内的偏移量就是成员变量的实际地址。编译器在编译时，基于类定义中成员变量的声明顺序和编译器的内存布局规则，计算每个成员变量相对于对象起始地址的偏移量。然后在运行时，通过基地址（即对象的地址）加上偏移量，就能够计算出每个成员变量的准确地址。这个计算过程由编译器和运行时系统自动处理。

# 函数调用约定与优化

上面的汇编代码中，`setB` 的两个参数，都是从寄存器先放到栈上，接着又从栈上放到寄存器进行操作，为什么要移来移去多此一举呢？要回答这个问题，需要先了解函数的调用约定和寄存器使用。在 x86-64 架构的系统调用约定中，前几个整数或指针参数通常通过寄存器（如 `rdi`, `rsi`, `rdx`, 等）传递，而浮点参数通过 `xmm0` 到 `xmm7` 寄存器传递。这种约定目的是为了提高函数调用的效率，因为使用寄存器传递参数比使用栈更快。

而将寄存器上的参数又移动到栈上，是为了保证寄存器中的值不被覆盖。因为寄存器是有限的资源，在函数中可能会被多次用于不同的目的。将值保存到栈上可以让函数内部自由地使用寄存器，而不必担心覆盖调用者的数据。

接着又将` -0x8(%rbp)` 放到 `rax` 寄存器，然后再通过 `movsd %xmm0,0x8(%rax)` 写入成员变量 `b` 的值，为啥不直接从 `xmm0` 寄存器写到基于 `rbp` 的偏移地址呢？这是因为 x86-64 的指令集和其操作模式通常支持使用寄存器间接寻址方式访问数据。使用 `rax` 等通用寄存器作为中间步骤，是一种更通用和兼容的方法。

**当然上面编译过程没有开启编译优化，所以编译器采用了直接但效率不高的代码生成策略，包括将参数和局部变量频繁地在栈与寄存器间移动**。而编译器的优化策略可能会影响参数的处理方式。如果开启编译优化，生成的 `main` 函数汇编部分如下：

![object_model5](/assets/images/202405/object_model5.png)

在 `O2` 优化级别下，编译器认定 `main` 函数中的所有操作（ 包括创建 `Basic` 对象和对其成员变量的赋值操作）对程序的最终结果没有影响，因此它们都被优化掉了。这是编译器的"死代码消除"，直接移除那些不影响程序输出的代码部分。

# 私有成员内存分布

``` cpp
#include <iostream>

class Basic
{
public:
    int a;
    double b;

    void setB(double value)
    {
        b = value;
    }

private:
    int c;
    double d;

    void secret(int t)
    {
        d = t + c;
    }
};

int main()
{
    Basic tmp;
    tmp.a = 1;
    tmp.setB(3.14);
    return 0;
}
```

通过 GDB 可以打印出所有成员变量的地址，发现这里私有变量的内存布局并没有什么特殊地方，也是依次顺序存储在对象中。私有的方法也没有特殊地方，一样存储在文本段。

![object_model6](/assets/images/202405/object_model6.png)

那么 `private` 怎么进行可见性控制的呢？首先编译期肯定是有保护的，无法通过 tmp 对象直接调用 secret 方法，否则会编译报错。那么运行期是否有保护呢？可以测试下，前面已经验证 `private` 成员变量也是根据偏移来找到内存位置的，可以在代码中直接根据偏移找到内存位置并更改里面的值。

``` cpp
#include <iostream>

class Basic
{
public:
    int a;
    double b;

    void setB(double value)
    {
        b = value;
        secret(b);
    }

private:
    int c;
    double d;

    void secret(int t)
    {
        d = t + c;
    }
};

int main()
{
    Basic tmp;
    tmp.a = 1;
    tmp.setB(3.14);

    int* p = reinterpret_cast<int*>(reinterpret_cast<char*>(&tmp) + 16);
    *p = 12;  // 绕过编译器检查，修改 c 的值

    return 0;
}
```

![object_model7](/assets/images/202405/object_model7.png)

私有方法和普通成员方法一样存储在文本段，如果可以拿到其地址后，就可以通过这个地址调用。在类定义中添加额外的接口来暴露私有成员方法的地址，然后通过成员函数指针来调用私有成员函数。

![object_model8](/assets/images/202405/object_model8.png)

因此对于成员函数来说，只是编译期不让直接调用，运行期并没有保护，可以绕过编译限制在对象外部调用。当然实际开发中，千万不要直接通过地址偏移来访问私有成员变量，也不要通过各种骚操作来访问私有成员方法，这样不仅破坏了类的封装性，而且是不安全的。


# 静态成员 (数据和函数)

``` cpp
#include <iostream>

class Basic
{
public:
    int a;
    double b;

    void setB(double value)
    {
        b = value;
    }

public:
    static int c;
    static int d;
    static void show()
    {
        std::cout << c << std::endl;
    }
};

int Basic::c = 1;
int Basic::d = 0;
int main()
{
    Basic tmp;
    tmp.a = 1;
    tmp.setB(3.14);

    tmp.show();
    return 0;
}
```

静态成员变量在类的所有实例之间共享，不管创建了多少个类的对象，静态成员变量只有一份数据。静态成员变量的生命周期从它们被定义的时刻开始，直到程序结束。静态成员方法不依赖于类的任何实例来执行，主要用在工厂方法、单例模式的实例获取方法、或其他与类的特定实例无关的工具函数。

![object_model9](/assets/images/202405/object_model9.png)


> `.bss` 段用于存储**没有初始化，或者初始化为零**的全局变量或者静态变量，而`.data` 段用于存储**已初始化的**全局变量和静态变量。
>
> **对于全局变量或者静态变量，为什么需要分为这两个段来存储，而不是合并为一个段来存储呢？**
>
> **这里主要是考虑到二进制文件磁盘空间大小以及加载效率**。在磁盘上，`.data` 占用实际的磁盘空间，因为它需要存储具体的初始值数据。`.bss` 段不占用实际的存储空间，只需要在程序加载时由操作系统分配并清零相应的内存即可，这样可以减少可执行文件的大小。在程序启动时，操作系统可以快速地为 `.bss` 段分配内存并将其初始化为零，而无需从磁盘读取大量的零值数据，可以提高程序的加载速度。这里详细的解释也可以参考：[Why is the .bss segment required?](https://stackoverflow.com/questions/9535250/why-is-the-bss-segment-required)。

静态方法通过输出内存地址，发现在 `.text` 代码段，这点和其他成员方法是一样的。不过和成员方法不同的是，第一个参数并不是 `this` 指针了。在实现上它与普通的全局函数类似，主要区别在于它们的作用域是限定在其所属的类中。

```
(gdb) p tmp.setB
$13 = {void (Basic * const, double)} 0x4011f8 <Basic::setB(double)>
(gdb) p Basic::show
$14 = {void (void)} 0x401216 <Basic::show()>
```

# 类继承的内存布局

## 不带虚函数的继承

``` cpp
#include <iostream>

class Base
{
public:
    int a;
    double b;

    void setB(double value)
    {
        b = value;
    }
};

class Derived : public Base
{
public:
    int c;
    void setC(int value)
    {
        c = value;
    }
};

int main()
{
    Derived tmp;
    tmp.a = 1;
    tmp.setB(3.14);
    tmp.c = 2;
    tmp.setC(3);
    return 0;
}
```

用 GDB 打印成员变量的内存分布，发现 `Derived` 类的对象在内存中的布局首先包含其基类 `Base` 的所有成员变量，紧接着是 `Derived` 类自己的成员变量。

其实 C++ 标准并没有规定在继承中，基类和派生类的成员变量之间的排列顺序，编译器可以自由发挥的。但是大部分编译器在实现中，都是基类的成员变量在派生类的成员变量之前，为什么这么做呢？因为这样实现，使对象模型变得更简单和直观。不论是基类还是派生类，对象的内存布局都是连续的，简化了对象创建、复制和销毁等操作的实现。我们通过派生类对象访问基类成员与直接使用基类对象访问时完全一致，一个派生类对象的前半部分就是一个完整的基类对象。

对于成员函数（包括普通函数和静态函数），它们不占用对象实例的内存空间。不论是基类的成员函数还是派生类的成员函数，它们都存储在程序的代码段 `.text` 中。

![object_model10](/assets/images/202405/object_model10.png)

## 带有虚函数的继承

``` cpp
#include <iostream>

class Base
{
public:
    int a;
    double b;

    virtual void printInfo()
    {
        std::cout << "Base::printInfo() a = " << a << ", b = " << b << std::endl;
    }

    virtual void printB()
    {
        std::cout << "Base::printB()" << std::endl;
    }

    void setB(double value)
    {
        b = value;
    }
};

class Derived : public Base
{
public:
    int c;

    void printInfo() override
    {
        std::cout << "Derived::printInfo() a = " << a << ", b = " << b << ", c = " << c << std::endl;
    }

    void setC(int value)
    {
        c = value;
    }
};

int main()
{
    Derived tmp;
    tmp.a = 1;
    tmp.setB(3.14);
    tmp.c = 2;
    tmp.setC(3);

    Base* pBase = &tmp;  // 基类指针指向派生类对象
    pBase->printInfo();  // 多态调用
    pBase->printB();     // 多态调用

    Base base = tmp;
    base.printInfo();  // 无法实现多态调用

    return 0;
}
```

`Base* pBase = &tmp` 用一个基类指针指向派生类对象，当通过基类指针调用虚函数 `pBase->printInfo()` 时，将在运行时解析为 `Derived::printInfo()` 方法，这是就是**运行时多态**。对于 `pBase->printB()` 调用，由于**派生类**中没有定义 `printB()` 方法，所以会调用**基类的** `printB()` 方法。

![object_model11](/assets/images/202405/object_model11.png)

可以看到派生类对象的开始部分有**一个 8 字节的虚函数表指针** `vptr`，这个指针指向一个虚函数表（vtable），虚函数表中存储了虚函数的地址，一共有**两个地址** 4199188 和 4199114，分别对应Derived 类中的两个虚函数 `Derived::printInfo()` 和 `Base::printB()`。


![object_model12](/assets/images/202405/object_model12.png)

虚函数在类对象中的内存布局。在编译器实现中，虚函数表指针是每个对象实例的一部分，占用对象实例的内存空间。对于一个实例对象，通过其地址就能找到对应的虚函数表，然后通过虚函数表找到具体的虚函数地址，实现多态调用。

> **什么必须通过引用或者指针才能实现多态调用呢？**

指针和引用在编译器底层没有区别，ref 和 ptr 的地址一样，就是原来派生类对象的地址，里面的虚函数表指针指向派生类的虚函数表，所以可以调用到派生类的函数。而通过拷贝构造函数生成的基类对象 base，编译器执行了隐式类型转换，从派生类截断了基类部分，生成了一个基类对象。base 中的虚函数表指针指向的是基类的虚函数表，所以调用的是基类的函数。

![object_model13](/assets/images/202405/object_model13.png)

> 从上面拷贝构造的 base 对象的虚函数表指针的输出可以看到，**虚函数表不是每个实例一份，而是所有对象实例共享同一个虚函数表。虚函数表是每个多态类一份，由编译器在编译时创建**。
>
> 当然，**这里是某种编译器对于多态的实现。C++ 标准本身没有规定多态的实现细节，没有说一定要有虚函数表（vtable）和虚函数表指针（vptr）来实现**。这是因为 C++ 标准关注的是行为和语义，确保我们使用多态特性时能够得到正确的行为，但它不规定底层的内存布局或具体的实现机制，这些细节通常由编译器的实现来决定。
>
> **不同编译器的实现也可能不一样，许多编译器为了访问效率，将虚函数表指针放在对象内存布局的开始位置。这样，虚函数的调用可以快速定位到虚函数表，然后找到对应的函数指针**。如果类有多重继承，情况可能更复杂，某些编译器可能会采取不同的策略来安排虚函数表指针的位置，或者一个对象可能有多个虚函数表指针。


# 地址空间布局随机化

如果用 GDB 多次运行程序，对象的虚拟内存地址每次都一样，这是为什么呢？

现代操作系统中，每个运行的程序都使用**虚拟内存地址空间**，通过操作系统的内存管理单元（`MMU`）映射到物理内存的。虚拟内存有很多优势，包括提高安全性、允许更灵活的内存管理等。为了防止缓冲区溢出攻击等安全漏洞，操作系统还会在每次程序启动时随机化进程的地址空间布局，这就是地址空间布局随机化（`ASLR`，[Address Space Layout Randomization](https://en.wikipedia.org/wiki/Address_space_layout_randomization)）。

> **Address space layout randomization (ASLR)** is a computer security technique involved in preventing exploitation of memory corruption vulnerabilities. In order to prevent an attacker from reliably redirecting code execution to, for example, a particular exploited function in memory, ASLR randomly arranges the address space positions of key data areas of a process, including the base of the executable and the positions of the stack, heap and libraries.

在 Linux 操作系统上，可以通过 `cat /proc/sys/kernel/randomize_va_space` 查看当前系统的 `ASLR` 是否启用，基本上默认都是开启状态 (值为 2)，如果是 0，则是禁用状态。

前面使用 GDB 进行调试时，之所以观察到内存地址是固定不变的，这是因为 GDB 默认禁用了 `ASLR`，以便于调试过程中更容易重现问题。可以在使用 GDB 时启用 `ASLR`，从而让调试环境更贴近实际运行环境。启动 GDB 后，可以通过下面命令开启地址空间的随机化。

```
(gdb) set disable-randomization off
```

![object_model14](/assets/images/202405/object_model14.png)

> 若要实现程序里的运行时地址随机，除了需要 Linux 开启 ASLR，同时也需要编译时指定 PIE 编译选项，默认 GCC 是开启状态。


# 总结

C++ 的对象模型是一个复杂的话题，涉及到类的内存布局、成员变量和成员函数的访问、继承、多态等多个方面。

* 对象的内存布局是连续的，成员变量**按照声明的顺序**存储在对象中，编译器会根据类定义计算每个成员变量相对于对象起始地址的偏移量。
* 成员方法存储在进程的文本段 (代码段)，不占用对象实例的内存空间，通过 `this` 指针和偏移量访问成员变量。
* 私有成员变量和方法在运行期并没有保护，可以通过地址偏移绕过编译器的限制进行读写，但是不推荐这样做。
* 静态成员变量和静态成员方法存储在程序的数据段和代码段，不占用对象实例的内存空间。
* 继承类的内存布局，编译器一般会把基类的成员变量放在派生类的成员变量之前，使对象模型变得更简单和直观。
* 带有虚函数的继承，对象的内存布局中包含虚函数表指针，多态调用通过虚函数表实现。虚函数实现比较复杂，这里只考虑简单的单继承。
* 地址空间布局随机化（ASLR）是现代操作系统的安全特性，可以有效防止缓冲区溢出攻击等安全漏洞。GDB 默认禁用 ASLR，可以通过 `set disable-randomization off` 命令开启地址空间的随机化。




# Refer

* 《深度探索 C++对象模型：Inside the C++ Object Model》










