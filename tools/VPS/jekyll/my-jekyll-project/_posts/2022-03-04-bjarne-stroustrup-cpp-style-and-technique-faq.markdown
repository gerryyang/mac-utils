---
layout: post
title:  "Bjarne Stroustrup's C++ Style and Technique FAQ (Reading)"
date:   2022-03-04 20:35:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

These are questions about C++ Style and Technique that people ask me often. 

For more general questions, see my [general FAQ](https://www.stroustrup.com/bs_faq.html).

For terminology and concepts, see my [C++ glossary](https://www.stroustrup.com/glossary.html).

Please note that these are just a collection of questions and answers. They are not a substitute for a carefully selected sequence of examples and explanations as you would find in a good textbook. Nor do they offer detailed and precise specifications as you would find in a reference manual or the standard.  See [The Design and Evolution of C++](https://www.stroustrup.com/dne.html) for questions related to the design of C++. See [The C++ Programming Language](https://www.stroustrup.com/3rd.html) for questions about the use of C++ and its standard library.


# Getting started

## How do I write this very simple program?

``` cpp
#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

int main()
{
	vector<double> v;

	double d;
	while(cin>>d) v.push_back(d);	// read elements
	if (!cin.eof()) {		// check if input failed
		cerr << "format error\n";
		return 1;	// error return
	}

	cout << "read " << v.size() << " elements\n";

	reverse(v.begin(),v.end());
	cout << "elements in reverse order:\n";
	for (int i = 0; i<v.size(); ++i) cout << v[i] << '\n';

	return 0; // success return
}
```

Here are a few observations about this program:

* This is a Standard ISO C++ program using the standard library. Standard library facilities are declared in namespace std in headers without a `.h` suffix.
* If you want to compile this on a Windows machine, you need to compile it as a "console application". Remember to give your source file the `.cpp` suffix or the compiler might think that it is C (not C++) source.
* Yes, [main() returns an int](https://www.stroustrup.com/bs_faq2.html#void-main)
* Reading into a standard vector guarantees that you don't overflow some arbitrary buffer. Reading into an [array](https://www.stroustrup.com/bs_faq2.html#arrays) without making a "silly error" is beyond the ability of complete novices - by the time you get that right, you are no longer a complete novice. If you doubt this claim, I suggest you read my paper "Learning Standard C++ as a New Language", which you can download from [my publications list](https://www.stroustrup.com/papers.html).
* The `!cin.eof()` is a test of the stream's format. Specifically, it tests whether the loop ended by finding end-of-file (if not, you didn't get input of the expected type/format). For more information, look up "stream state" in your C++ textbook.
* A vector knows its size, so I don't have to count elements.
* Yes, I know that I could declare `i` to be a `vector<double>::size_type` rather than plain `int` to quiet warnings from some hyper-suspicious compilers, but in this case, I consider that too pedantic and distracting.
* This program contains no explicit memory management, and it does not leak memory. A vector keeps track of the memory it uses to store its elements. When a vector needs more memory for elements, it allocates more; when a vector goes out of scope, it frees that memory. Therefore, the user need not be concerned with the allocation and deallocation of memory for vector elements.
* for reading in strings, see [How do I read a string from input?](https://www.stroustrup.com/bs_faq2.html#read-string).
* The program ends reading input when it sees "end of file". If you run the program from the keybord on a Unix machine "end of file" is `Ctrl-D`. If you are on a Windows machine that because of a bug doesn't recognize an end-of-file character, you might prefer this slightly more complicated version of the program that terminates input with the word "end":

``` cpp
#include<iostream>
#include<vector>
#include<algorithm>
#include<string>
using namespace std;

int main()
{
	vector<double> v;

	double d;
	while(cin>>d) v.push_back(d);	// read elements
	if (!cin.eof()) {		// check if input failed
		cin.clear();		// clear error state
		string s;
		cin >> s;		// look for terminator string
		if (s != "end") {
			cerr << "format error\n";
			return 1;	// error return
		}
	}

	cout << "read " << v.size() << " elements\n";

	reverse(v.begin(),v.end());
	cout << "elements in reverse order:\n";
	for (int i = 0; i<v.size(); ++i) cout << v[i] << '\n';

	return 0; // success return
}
```

For more examples of how to use the standard library to do simple things simply, see the "Tour of the Standard Library" Chapter of [TC++PL4](https://www.stroustrup.com/4th.html).


## Can you recommend a coding standard?

Yes: [The C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md). This is an ambitious project to guide people to an effective style of modern C++ and to provide tool to support its rules. It encourages people to use C++ as [a completely type- and resource-safe language](https://www.stroustrup.com/resource-model.pdf) without compromising performance or adding verbosity. There are [videos](https://www.stroustrup.com/C++.html#videos) describing the guidelines project.

The main point of a C++ coding standard is to provide a set of rules for using C++ for a particular purpose in a particular environment. It follows that there cannot be one coding standard for all uses and all users. For a given application (or company, application area, etc.), a good coding standard is better than no coding standard. On the other hand, I have seen many examples that demonstrate that a bad coding standard is worse than no coding standard.

Don't use C coding standards (even if slightly modified for C++) and don't use ten-year-old C++ coding standards (even if good for their time). C++ isn't (just) C and Standard C++ is not (just) pre-standard C++.


> Comment：a good coding standard > no coding standard > a bad coding standard

## How do I read a string from input?

You can read a single, whitespace terminated word like this:

``` cpp
#include<iostream>
#include<string>
using namespace std;

int main()
{
	cout << "Please enter a word:\n";

	string s;
	cin>>s;

	cout << "You entered " << s << '\n';
}
```

Note that there is no explicit memory management and no fixed-sized buffer that you could possibly overflow.

If you really need a whole line (and not just a single word) you can do this:

``` cpp
#include<iostream>
#include<string>
using namespace std;

int main()
{
	cout << "Please enter a line:\n";

	string s;
	getline(cin,s);

	cout << "You entered " << s << '\n';
}
```

For a brief introduction to standard library facilities, such as iostream and string, see Chaper 3 of [TC++PL3](https://www.stroustrup.com/3rd.html) (available online). For a detailed comparison of simple uses of C and C++ I/O, see "Learning Standard C++ as a New Language", which you can download from my [publications list](https://www.stroustrup.com/papers.html)


## How do I convert an integer to a string?

The simplest way is to use a stringstream:

``` cpp
#include<iostream>
#include<string>
#include<sstream>
using namespace std;

string itos(int i)	// convert int to string
{
	stringstream s;
	s << i;
	return s.str();
}

int main()
{
	int i = 127;
	string ss = itos(i);
	const char* p = ss.c_str();

	cout << ss << " " << p << "\n";
}
```

Naturally, this technique works for converting any type that you can output using << to a string. For a description of string streams, see 21.5.3 of [The C++ Programming Language](https://www.stroustrup.com/3rd.html).


# Classes

## How are C++ objects laid out in memory?

Like C, C++ doesn't define layouts, just semantic constraints that must be met. Therefore different implementations do things differently. Unfortunately, the best explanation I know of is in a book that is otherwise outdated and doesn't describe any current C++ implementation: [The Annotated C++ Reference Manual](https://www.stroustrup.com/arm.html) (usually called the ARM). It has diagrams of key layout examples. There is a very brief explanation in Chapter 2 of [TC++PL](https://www.stroustrup.com/3rd.html).

Basically, C++ constructs objects simply by concatenating sub objects. Thus

``` cpp
struct A { int a,b; };
```

is represented by two ints next to each other, and

``` cpp
struct B : A { int c; };
```

is represented by an A followed by an int; that is, by three ints next to each other.

Virtual functions are typically implemented by adding a pointer (the vptr) to each object of a class with virtual functions. This pointer points to the appropriate table of functions (the vtbl). Each class has its own vtbl shared by all objects of that class.

> Comment: C++对象的布局结构

## Why is "this" not a reference?

Because "this" was introduced into C++ (really into C with Classes) before references were added. Also, I chose "this" to follow Simula usage, rather than the (later) Smalltalk use of "self".

> Comment: this 比 reference 出现的还要早

## Why is the size of an empty class not zero?

To ensure that the addresses of two different objects will be different. For the same reason, "new" always returns pointers to distinct objects. Consider:

``` cpp
class Empty { };

void f()
{
	Empty a, b;
	if (&a == &b) cout << "impossible: report error to compiler supplier";

	Empty* p1 = new Empty;
	Empty* p2 = new Empty;
	if (p1 == p2) cout << "impossible: report error to compiler supplier";
}
```

There is an interesting rule that says that an empty base class need not be represented by a separate byte:

``` cpp
struct X : Empty {
	int a;
	// ...
};

void f(X* p)
{
	void* p1 = p;
	void* p2 = &p->a;
	if (p1 == p2) cout << "nice: good optimizer";
}
```

This optimization is safe and can be most useful. It allows a programmer to use empty classes to represent very simple concepts without overhead. Some current compilers provide this "empty base class optimization".

> Comment: 空类分配空间大小不为0，但是在继承场景下，编译器会对空类的分配优化为0

## How do I define an in-class constant?

If you want a constant that you can use in a constant expression, say as an array bound, you have two choices:

``` cpp
class X {
	static const int c1 = 7;
	enum { c2 = 19 };

	char v1[c1];
	char v2[c2];

	// ...
};
```

At first glance, the declaration of c1 seems cleaner, but note that to use that in-class initialization syntax, the constant must be a static const of integral or enumeration type initialized by a constant expression. That's quite restrictive:

``` cpp
class Y {
	const int c3 = 7;		// error: not static
	static int c4 = 7;		// error: not const
	static const float c5 = 7;	// error: not integral
};
```

I tend to use the "enum trick" because it's portable and doesn't tempt me to use non-standard extensions of the in-class initialization syntax.

**So why do these inconvenient restrictions exist?** A class is typically declared in a header file and a header file is typically included into many translation units. However, to avoid complicated linker rules, C++ requires that every object has a unique definition. That rule would be broken if C++ allowed in-class definition of entities that needed to be stored in memory as objects. See [D&E](https://www.stroustrup.com/dne.html) for an explanation of C++'s design tradeoffs.

You have more flexibility if the const isn't needed for use in a constant expression:

``` cpp
class Z {
	static char* p;		// initialize in definition
	const int i;		// initialize in constructor
public:
	Z(int ii) :i(ii) { }
};

char* Z::p = "hello, there";
```

You can take the address of a static member if (and only if) it has an out-of-class definition:

``` cpp
class AE {
// ...
public:
	static const int c6 = 7;
	static const int c7 = 31;
};

const int AE::c7;	// definition

int f()
{
	const int* p1 = &AE::c6;	// error: c6 not an lvalue
	const int* p2 = &AE::c7;	// ok
	// ...
}
```

## Why isn't the destructor called at the end of scope? TODO

The simple answer is "of course it is!", but have a look at the kind of example that often accompany that question:

``` cpp
void f()
{
	X* p = new X;
	// use p
}
```

That is, there was some (mistaken) assumption that the object created by "new" would be destroyed at the end of a function.

Basically, you should only use "new" if you want an object to live beyond the lifetime of the scope you create it in. That done, you need to use "delete" to destroy it. For example:

``` cpp
X* g(int i) { /* ... */ return new X(i); }	// the X outlives the call of g()

void h(int i)
{
	X* p = g(i);
	// ...
	delete p;
}
```

If you want an object to live in a scope only, don't use "new" but simply define a variable:

``` cpp
{
	ClassName x;
	// use x
}
```

The variable is implicitly destroyed at the end of the scope.

Code that creates an object using new and then deletes it at the end of the same scope is ugly, error-prone, and inefficient. For example:

``` cpp
void fct()	// ugly, error-prone, and inefficient
{
	X* p = new X;
	// use p
	delete p;
}
```

## Does "friend" violate encapsulation?

No. It does not. "Friend" is an explicit mechanism for granting access, just like membership. You cannot (in a standard conforming program) grant yourself access to a class without modifying its source. For example:

``` cpp
class X {
	int i;
public:
	void m();		// grant X::m() access
	friend void f(X&);	// grant f(X&) access
	// ...
};

void X::m() { i++; /* X::m() can access X::i */ }

void f(X& x) { x.i++; /* f(X&) can access X::i */ }
```

For a description on the C++ protection model, see [D&E](https://www.stroustrup.com/dne.html) sec 2.10 and [TC++PL](https://www.stroustrup.com/3rd.html) sec 11.5, 15.3, and C.11.


## Why doesn't my constructor work right?

This is a question that comes in many forms. Such as:

* Why does the compiler copy my objects when I don't want it to?
* How do I turn off copying?
* How do I stop implicit conversions?
* How did my int turn into a complex number?

By default a class is given a copy constructor and a copy assignment that copy all elements. For example:

``` cpp
struct Point {
	int x,y;
	Point(int xx = 0, int yy = 0) :x(xx), y(yy) { }
};

Point p1(1,2);
Point p2 = p1;	
```

Here we get `p2.x==p1.x` and `p2.y==p1.y`. That's often exactly what you want (and essential for C compatibility), but consider:

``` cpp
class Handle {
private:
	string name;
	X* p;
public:
	Handle(string n)
		:name(n), p(0) { /* acquire X called "name" and let p point to it */ }
	~Handle() { delete p; /* release X called "name" */ }
	// ...
};

void f(const string& hh)
{
	Handle h1(hh);
	Handle h2 = h1;	// leads to disaster!
	// ...
}
```

Here, the default copy gives us `h2.name==h1.name` and `h2.p==h1.p`. This leads to disaster: when we exit f() the destructors for h1 and h2 are invoked and the object pointed to by `h1.p` and `h2.p` is deleted twice.

How do we avoid this? The simplest solution is to prevent copying by making the operations that copy private:

``` cpp
class Handle {
private:
	string name;
	X* p;

	Handle(const Handle&);	// prevent copying
	Handle& operator=(const Handle&);
public:
	Handle(string n)
		:name(n), p(0) { /* acquire the X called "name" and let p point to it */ }
	~Handle() { delete p; /* release X called "name" */ }
	// ...
};

void f(const string& hh)
{
	Handle h1(hh);
	Handle h2 = h1;	// error (reported by compiler)
	// ...
}
```

If we need to copy, we can of course define the copy initializer and the copy assignment to provide the desired semantics.

Now return to `Point`. For Point the default copy semantics is fine, the problem is the constructor:

``` cpp
struct Point {
	int x,y;
	Point(int xx = 0, int yy = 0) :x(xx), y(yy) { }
};

void f(Point);

void g()
{
	Point orig;	// create orig with the default value (0,0)
	Point p1(2);	// create p1 with the default y-coordinate 0
	f(2);		// calls Point(2,0);
}
```

People provide default arguments to get the convenience used for orig and p1. Then, some are surprised by the conversion of 2 to Point(2,0) in the call of f(). A constructor taking a single argument defines a conversion. By default that's an implicit conversion. To require such a conversion to be explicit, declare the constructor `explicit`:

``` cpp
struct Point {
	int x,y;
	explicit Point(int xx = 0, int yy = 0) :x(xx), y(yy) { }
};

void f(Point);

void g()
{
	Point orig;	// create orig with the default value (0,0)
	Point p1(2);	// create p1 with the default y-coordinate 0
			// that's an explicit call of the constructor
	f(2);		// error (attmpted implicit conversion)
	Point p2 = 2;	// error (attmpted implicit conversion)
	Point p3 = Point(2);	// ok (explicit conversion)
}
```

> Comment: 关于类的初始化建议

# Class hierarchies

## Why do my compiles take so long?

You may have a problem with your compiler. It may be old, you may have it installed wrongly, or your computer might be an antique. I can't help you with such problems.

However, it is more likely that the program that you are trying to compile is poorly designed, so that compiling it involves the compiler examining hundreds of header files and tens of thousands of lines of code. In principle, this can be avoided. If this problem is in your library vendor's design, there isn't much you can do (except changing to a better library/vendor), but you can structure your own code to minimize re-compilation after changes. Designs that do that are typically better, more maintainable, designs because they exhibit better separation of concerns.

Consider a classical example of an object-oriented program:

``` cpp
class Shape {
public:		// interface to users of Shapes
	virtual void draw() const;
	virtual void rotate(int degrees);
	// ...
protected:	// common data (for implementers of Shapes)
	Point center;
	Color col;
	// ...
};

class Circle : public Shape {
public:	
	void draw() const;
	void rotate(int) { }
	// ...
protected:
	int radius;
	// ...
};

class Triangle : public Shape {
public:	
	void draw() const;
	void rotate(int);
	// ...
protected:
	Point a, b, c;
	// ...
};
```

The idea is that users manipulate shapes through Shape's public interface, and that implementers of derived classes (such as Circle and Triangle) share aspects of the implementation represented by the protected members.

There are three serious problems with this apparently simple idea:

* It is not easy to define shared aspects of the implementation that are helpful to all derived classes. For that reason, the set of protected members is likely to need changes far more often than the public interface. For example, even though "center" is arguably a valid concept for all Shapes, it is a nuisance(麻烦事) to have to maintain a point "center" for a Triangle - for triangles, it makes more sense to calculate the center if and only if someone expresses interest in it.
* The protected members are likely to depend on "implementation" details that the users of Shapes would rather not have to depend on. For example, many (most?) code using a Shape will be logically independent of the definition of "Color", yet the presence of Color in the definition of Shape will probably require compilation of header files defining the operating system's notion of color.
* When something in the protected part changes, users of Shape have to recompile - even though only implementers of derived classes have access to the protected members.


Thus, the presence of "information helpful to implementers" in the base class that also acts as the interface to users is the source of instability in the implementation, spurious recompilation of user code (when implementation information changes), and excess inclusion of header files into user code (because the "information helpful to implementers" needs those headers). This is sometimes known as the "brittle(脆弱的) base class problem."

The obvious solution is to omit the "information helpful to implemeters" for classes that are used as interfaces to users. That is, to make interfaces, pure interfaces. That is, to represent interfaces as abstract classes:

``` cpp
class Shape {
public:		// interface to users of Shapes
	virtual void draw() const = 0;
	virtual void rotate(int degrees) = 0;
	virtual Point center() const = 0;
	// ...

	// no data
};

class Circle : public Shape {
public:	
	void draw() const;
	void rotate(int) { }
	Point center() const { return cent; }
	// ...
protected:
	Point cent;
	Color col;
	int radius;
	// ...
};

class Triangle : public Shape {
public:	
	void draw() const;
	void rotate(int);
	Point center() const;
	// ...
protected:
	Color col;
	Point a, b, c;
	// ...
};
```

The users are now insulated(隔离) from changes to implementations of derived classes. **I have seen this technique decrease build times by orders of magnitudes.**

But what if there really is some information that is common to all derived classes (or simply to several derived classes)? Simply make that information a class and derive the implementation classes from that also:

``` cpp
class Shape {
public:		// interface to users of Shapes
	virtual void draw() const = 0;
	virtual void rotate(int degrees) = 0;
	virtual Point center() const = 0;
	// ...

	// no data
};

struct Common {
	Color col;
	// ...
};
	
class Circle : public Shape, protected Common {
public:	
	void draw() const;
	void rotate(int) { }
	Point center() const { return cent; }
	// ...
protected:
	Point cent;
	int radius;
};

class Triangle : public Shape, protected Common {
public:	
	void draw() const;
	void rotate(int);
	Point center() const;
	// ...
protected:
	Point a, b, c;
};
```

> Comment: 继承体系的合理性设计，会影响编译器的执行时常。

## Why do I have to put the data in my class declarations?

You don't. If you don't want data in an interface, don't put it in the class that defines the interface. Put it in derived classes instead. See, [Why do my compiles take so long?](https://www.stroustrup.com/bs_faq2.html#abstract-class).

Sometimes, you do want to have representation data in a class. Consider class complex:

``` cpp
template<class Scalar> class complex {
public:
	complex() : re(0), im(0) { }
	complex(Scalar r) : re(r), im(0) { }
	complex(Scalar r, Scalar i) : re(r), im(i) { }
	// ...

	complex& operator+=(const complex& a)
		{ re+=a.re; im+=a.im; return *this; }
	// ...
private:
	Scalar re, im;
};
```

This type is designed to be used much as a built-in type and the representation is needed in the declaration to make it possible to create genuinely local objects (i.e. objects that are allocated on the stack and not on a heap) and to ensure proper inlining of simple operations. Genuinely local objects and inlining is necessary to get the performance of complex close to what is provided in languages with a built-in complex type.

## Why are member functions not virtual by default?

Because many classes are not designed to be used as base classes. For example, see [class complex](https://www.stroustrup.com/bs_faq2.html#data-in-class).

Also, objects of a class with a virtual function require space needed by the virtual function call mechanism - typically one word per object. This overhead can be significant, and can get in the way of layout compatibility with data from other languages (e.g. C and Fortran).

See [The Design and Evolution of C++](https://www.stroustrup.com/dne.html) for more design rationale(基本原理).


## Why don't we have virtual constructors?

A virtual call is a mechanism(方法) to get work done given partial information. In particular, "virtual" allows us to call a function knowing only an interfaces and not the exact type of the object. To create an object you need complete information. **In particular, you need to know the exact type of what you want to create. Consequently, a "call to a constructor" cannot be virtual.**

Techniques for using an indirection when you ask to create an object are often referred to as "Virtual constructors". For example, see TC++PL3 15.6.2.

For example, here is a technique for generating an object of an appropriate type using an abstract class:

``` cpp
struct F {	// interface to object creation functions
	virtual A* make_an_A() const = 0;
	virtual B* make_a_B() const = 0;
};

void user(const F& fac)
{
	A* p = fac.make_an_A();	// make an A of the appropriate type
	B* q = fac.make_a_B();	// make a B of the appropriate type
	// ...
}

struct FX : F {
	A* make_an_A() const { return new AX();	} // AX is derived from A
	B* make_a_B() const { return new BX();	} // BX is derived from B
};

struct FY : F {
	A* make_an_A() const { return new AY();	} // AY is derived from A
	B* make_a_B() const { return new BY();	} // BY is derived from B
};

int main()
{
	FX x;
	FY y;
	user(x);	// this user makes AXs and BXs
	user(y);	// this user makes AYs and BYs

	user(FX());	// this user makes AXs and BXs
	user(FY());	// this user makes AYs and BYs
	// ...
}
```

**This is a variant of what is often called "the factory pattern"(工厂模式). The point is that user() is completely isolated from knowledge of classes such as AX and AY**.

> Comment: 因为创建对象需要知道类的完整信息，因此构造函数不能为virtual

## Why are destructors not virtual by default?

Because many classes are not designed to be used as base classes. Virtual functions make sense only in classes meant to act as interfaces to objects of derived classes (typically allocated on a heap and accessed through pointers or references).

So when should I declare a destructor virtual? **Whenever the class has at least one virtual function. Having virtual functions indicate that a class is meant to act as an interface to derived classes, and when it is, an object of a derived class may be destroyed through a pointer to the base.** For example:


``` cpp
class Base {
	// ...
	virtual ~Base();
};

class Derived : public Base {
	// ...
	~Derived();
};

void f()
{
	Base* p = new Derived;
	delete p;	// virtual destructor used to ensure that ~Derived is called
}
```

**Had Base's destructor not been virtual, Derived's destructor would not have been called - with likely bad effects, such as resources owned by Derived not being freed.**

> Comment: 正确地设置析构函数为virtual，否则可能出现资源泄漏。

## What is a pure virtual function?

``` cpp
#include <iostream>

class Base
{
public:
    virtual void f() = 0;
};

void Base::f() { std::cout << "Base::f()\n"; }

class Derived : public Base 
{
public:
	void f();
};

void Derived::f() { std::cout << "Derived::f()\n"; }

int main()
{
    Derived d;  
    d.f();  // Derived::f()

    Base* pb = new Derived();
    pb->f();  // Derived::f()

    pb->Base::f(); // Base::f() 
}
```

A pure virtual function is a function that must be overridden in a derived class and need not be defined. A virtual function is declared to be "pure" using the curious "=0" syntax. For example:

``` cpp
class Base {
public:
	void f1();		// not virtual
	virtual void f2();	// virtual, not pure
	virtual void f3() = 0;	// pure virtual
};

Base b;	// error: pure virtual f3 not overridden
```

Here, Base is an abstract class (because it has a pure virtual function), so no objects of class Base can be directly created: Base is (explicitly) meant to be a base class. For example:

``` cpp
class Derived : public Base {
	// no f1: fine
	// no f2: fine, we inherit Base::f2
	void f3();
};

Derived d;	// ok: Derived::f3 overrides Base::f3
```

Abstract classes are immensely useful for defining interfaces. In fact, a class with only pure virtual functions is often called an interface.

You can define a pure virtual function:

``` cpp
Base::f3() { /* ... */ }
```

This is very occasionally useful (to provide some simple common implementation detail for derived classes), but Base::f3() must still be overridden in some derived class.

If you don't override a pure virtual function in a derived class, that derived class becomes abstract:

``` cpp
class D2 : public Base {
	// no f1: fine
	// no f2: fine, we inherit Base::f2
	// no f3: fine, but D2 is therefore still abstract
};

D2 d;	// error: pure virtual Base::f3 not overridden
```

> Comment: 抽象类用于定义接口，抽象类不能够直接实例化对象

## Why doesn't C++ have a final keyword?

[It has, but it is not as useful as you might think.](https://www.stroustrup.com/bs_faq2.html#no-derivation)

## Can I call a virtual function from a constructor?

Yes, but be careful. It may not do what you expect. In a constructor, the virtual call mechanism is disabled because overriding from derived classes hasn't yet happened. Objects are constructed from the base up, "base before derived".

Consider:

``` cpp
#include<string>
#include<iostream>
using namespace std;

class B {
public:
	B(const string& ss) { cout << "B constructor\n"; f(ss); }
	virtual void f(const string&) { cout << "B::f\n";}
};

class D : public B {
public:
	D(const string & ss) :B(ss) { cout << "D constructor\n";}
	void f(const string& ss) { cout << "D::f\n"; s = ss; }
private:
	string s;
};

int main()
{
	D d("Hello");
}
```

the program compiles and produce

```
B constructor
B::f
D constructor
```

Note not `D::f`. Consider what would happen if the rule were different so that `D::f()` was called from `B::B()`: Because the constructor `D::D()` hadn't yet been run, **`D::f()` would try to assign its argument to an uninitialized string s. The result would most likely be an immediate crash**.

Destruction is done "derived class before base class", so virtual functions behave as in constructors: Only the local definitions are used - and no calls are made to overriding functions to avoid touching the (now destroyed) derived class part of the object.

For more details see [D&E](https://www.stroustrup.com/dne.html) 13.2.4.2 or [TC++PL3](https://www.stroustrup.com/3rd.html) 15.4.3.

It has been suggested that this rule is an implementation artifact. It is not so. In fact, it would be noticeably easier to implement the unsafe rule of calling virtual functions from constructors exactly as from other functions. However, that would imply that no virtual function could be written to rely on invariants established by base classes. That would be a terrible mess.

> Comment: 为了不给自己找麻烦，最好不要在构造函数里调用虚函数，及时可以调用但是行为也与预期不同

## Can I stop people deriving from my class?

Yes, but why do you want to? There are two common answers:

* for efficiency: to avoid my function calls being virtual
* for safety: to ensure that my class is not used as a base class (for example, to be sure that I can copy objects without fear of slicing)

In my experience, the efficiency reason is usually misplaced fear. In C++, virtual function calls are so fast that their real-world use for a class designed with virtual functions does not to produce measurable run-time overheads compared to alternative solutions using ordinary function calls. Note that the virtual function call mechanism is typically used only when calling through a pointer or a reference. When calling a function directly for a named object, the virtual function class overhead is easily optimized away.

If there is a genuine need for "capping" a class hierarchy to avoid virtual function calls, one might ask why those functions are virtual in the first place. I have seen examples where performance-critical functions had been made virtual for no good reason, just because "that's the way we usually do it".

The other variant of this problem, **how to prevent derivation for logical reasons, has a solution in C++11**. For example:

``` cpp
struct Base {
	virtual void f();
};

struct Derived final : Base {	// now Derived is final; you cannot derive from it
	void f() override;
};

struct DD: Derived {// error: Derived is final

	// ...
};
```

For older compilers, you can use a somewhat clumsy technique:

``` cpp
class Usable;

class Usable_lock {
	friend class Usable;
private:
	Usable_lock() {}
	Usable_lock(const Usable_lock&) {}
};

class Usable : public virtual Usable_lock {
	// ...
public:
	Usable();
	Usable(char*);
	// ...
};

Usable a;

class DD : public Usable { };

DD dd;  // error: DD::DD() cannot access
		// Usable_lock::Usable_lock(): private  member
```

## Why doesn't C++ have a universal class Object?

* We don't need one: generic programming provides statically type safe alternatives in most cases. Other cases are handled using multiple inheritance.
* There is no useful universal class: a truly universal carries no semantics of its own.
* A "universal" class encourages sloppy thinking about types and interfaces and leads to excess run-time checking.
* Using a universal base class implies cost: Objects must be heap-allocated to be polymorphic; that implies memory and access cost. Heap objects don't naturally support copy semantics. Heap objects don't support simple scoped behavior (which complicates [resource management](https://www.stroustrup.com/bs_faq2.html#finally)). A universal base class encourages use of dynamic_cast and other run-time checking.

Yes. I have simplified the arguments; this is an FAQ, not an academic paper.


## Do we really need multiple inheritance?

Not really. We can do without multiple inheritance by using workarounds(变通的方法), exactly as we can do without single inheritance by using workarounds. We can even do without classes by using workarounds. C is a proof of that contention. However, every modern language with static type checking and inheritance provides some form of multiple inheritance. In C++, abstract classes often serve as interfaces and a class can have many interfaces. Other languages - often deemed "not MI" - simply has a separate name for their equivalent to a pure abstract class: an interface. The reason languages provide inheritance (both single and multiple) is that language-supported inheritance is typically superior to workarounds (e.g. use of forwarding functions to sub-objects or separately allocated objects) for ease of programming, for detecting logical problems, for maintainability, and often for performance.

## Why doesn't overloading work for derived classes?

That question (in many variations) are usually prompted by an example like this:

``` cpp
#include<iostream>
using namespace std;

class B {
public:
	int f(int i) { cout << "f(int): "; return i+1; }
	// ...
};

class D : public B {
public:
	double f(double d) { cout << "f(double): "; return d+1.3; }
	// ...
};

int main()
{
	D* pd = new D;

	cout << pd->f(2) << '\n';
	cout << pd->f(2.3) << '\n';
}
```

which will produce:

```
f(double): 3.3
f(double): 3.6
```

rather than the

```
f(int): 3
f(double): 3.6
```

that some people (wrongly) guessed.

In other words, there is no overload resolution between D and B. The compiler looks into the scope of D, finds the single function "double f(double)" and calls it. It never bothers with the (enclosing) scope of B. In C++, there is no overloading across scopes - derived class scopes are not an exception to this general rule. (See [D&E](https://www.stroustrup.com/dne.html) or [TC++PL3](https://www.stroustrup.com/3rd.html) for details).

But what if I want to create an overload set of all my f() functions from my base and derived class? That's easily done using a using-declaration:

``` cpp
class D : public B {
public:
	using B::f;	// make every f from B available
	double f(double d) { cout << "f(double): "; return d+1.3; }
	// ...
};
```

Give that modification, the output will be

```
f(int): 3
f(double): 3.6
```

**That is, overload resolution was applied to B's f() and D's f() to select the most appropriate f() to call.**

> Comment: 注意重载在继承层次下的使用规则

## Can I use "new" just as in Java?

Sort of, but don't do it blindly and there are often superior alternatives. Consider:

``` cpp
void compute(cmplx z, double d)
{
	cmplx z2 = z+d;	// c++ style
	z2 = f(z2);		// use z2

	cmplx& z3 = *new cmplx(z+d);	// Java style (assuming Java could overload +)
	z3 = f(z3);
	delete &z3;	
}
```

The clumbsy use of "new" for z3 is unnecessary and slow compared with the idiomatic use of a local variable (z2). You don't need to use "new" to create an object if you also "delete" that object in the same scope; such an object should be a local variable.

# Templates and generic programming


## Why can't I define constraints for my template parameters?

Well, you can, and it's quite easy and general.

Consider:

``` cpp
template<class Container>
void draw_all(Container& c)
{
	for_each(c.begin(),c.end(),mem_fun(&Shape::draw));
}
```

If there is a type error, it will be in the resolution of the fairly complicated for_each() call. For example, if the element type of the container is an int, then we get some kind of obscure error related to the for_each() call (because we can't invoke Shape::draw() for an int).


see: https://www.stroustrup.com/bs_faq2.html#constraints


## Why can't I assign a vector<Apple*> to a vector<Fruit*>?

Because that would open a hole in the type system. 

see: https://www.stroustrup.com/bs_faq2.html#conversion


## Is "generics"(泛型) what templates should have been?

No. generics are primarily syntactic sugar for abstract classes; that is, with generics (whether Java or C# generics), you program against precisely defined interfaces and typically pay the cost of virtual function calls and/or dynamic casts to use arguments.

Templates supports generic programming, template metaprogramming, etc. through a combination of features such as integer template arguments, specialization, and uniform treatment of built-in and user-defined types. The result is flexibility, generality, and performance unmatched by "generics". The STL is the prime example.

A less desirable result of the flexibility is late detection of errors and horrendously(可怕地) bad error messages. This is currently being addressed indirectly with [constraints classes](https://www.stroustrup.com/bs_faq2.html#constraints).


## Why use sort() when we have "good old qsort()"?

To a novice,

``` cpp
qsort(array,asize,sizeof(elem),elem_compare);
```

looks pretty weird, and is harder to understand than

``` cpp
sort(vec.begin(),vec.end());
```

To an expert, the fact that `sort()` tends to be faster than `qsort()` for the same elements and the same comparison criteria is often significant. Also, `sort()` is generic, so that it can be used for any reasonable combination of container type, element type, and comparison criterion. For example:

``` cpp
struct Record {
	string name;
	// ...
};

struct name_compare {	// compare Records using "name" as the key
	bool operator()(const Record& a, const Record& b) const
		{ return a.name<b.name; }
};

void f(vector<Record>& vs)
{
	sort(vs.begin(), vs.end(), name_compare());
	// ...
}
```

In addition, most people appreciate that `sort()` is type safe, that no casts are required to use it, and that they don't have to write a compare() function for standard types.

For a more detailed explanation, see my paper "Learning C++ as a New language", which you can download from my [publications list](https://www.stroustrup.com/papers.html).

The primary reason that `sort()` tends to outperform(胜过) `qsort()` is that the comparison inlines better.


## What is a function object?


``` cpp
#include<iostream>
#include<vector>
using namespace std;

class Sum {
	int val;
public:
	Sum(int i) :val(i) { }
	operator int() const { return val; }		// extract value

	int operator()(int i) { return val+=i; }	// application
};

void f(vector<int>& v)
{
	Sum s = 0;	// initial value 0
	s = for_each(v.begin(), v.end(), s);	// gather the sum of all elements
	cout << "the sum is " << s << "\n";
	
	// or even:
	cout << "the sum is " << for_each(v.begin(), v.end(), Sum(0)) << "\n";
}

int main()
{
    vector<int> v = {1, 2, 3, 4, 5};
    f(v);
}
/*
the sum is 15
the sum is 15
*/
```

An object that in some way behaves like a function, of course. Typically, that would mean an object of a class that defines the application operator - operator().

A function object is a more general concept than a function because a function object can have state that persist across several calls (like a static local variable) and can be initialized and examined from outside the object (unlike a static local variable). For example:

**Note that a function object with an inline application operator inlines beautifully because there are no pointers involved that might confuse optimizers**. To contrast: current optimizers are rarely (never?) able to inline a call through a pointer to function.

Function objects are extensively used to provide flexibility in the standard library.

关于`for_each`的用法说明：

``` cpp
// https://en.cppreference.com/w/cpp/algorithm/for_each
template< class InputIt, class UnaryFunction >
UnaryFunction for_each( InputIt first, InputIt last, UnaryFunction f );

/* 
f	-	function object, to be applied to the result of dereferencing every iterator in the range [first, last)
        The signature of the function should be equivalent to the following:
		void fun(const Type &a);
		The signature does not need to have const &.
		The type Type must be such that an object of type InputIt can be dereferenced and then implicitly converted to Type.
*/
```

> Comment: 仿函数的用法


## What is an auto_ptr and why isn't there an auto_array?


An `auto_ptr` is an example of very simple handle class, defined in `<memory>`, supporting exception safety using the [resource acquisition is initialization](https://www.stroustrup.com/bs_faq2.html#finally) technique. An auto_ptr holds a pointer, can be used as a pointer, and deletes the object pointed to at the end of its scope. For example:

``` cpp
#include<memory>
using namespace std;

struct X {
	int m;
	// ..
};

void f()
{
	auto_ptr<X> p(new X);
	X* q = new X;

	p->m++;		// use p just like a pointer
	q->m++;
	// ...

	delete q;
}
```

**If an exception is thrown in the ... part, the object held by p is correctly deleted by auto_ptr's destructor while the X pointed to by q is leaked. See TC++PL 14.4.2 for details.**

Auto_ptr is a very lightweight class. In particular, it is *not* a reference counted pointer. If you "copy" one auto_ptr into another, the assigned to auto_ptr holds the pointer and the assigned auto_ptr holds 0. For example:

使用`-Wno-deprecated-declarations`编译选项屏蔽编译错误。


``` cpp
#include<memory>
#include<iostream>
using namespace std;

struct X {
	int m;
	// ..
};

int main()
{
	auto_ptr<X> p(new X);
	auto_ptr<X> q(p);
	cout << "p " << p.get() << " q " << q.get() << "\n";
}
/*
p 0 q 0xa208f0
*/
```

should print a 0 pointer followed by a non-0 pointer. `auto_ptr::get()` returns the held pointer.

**This "move semantics" differs from the usual "copy semantics", and can be surprising. In particular, never use an auto_ptr as a member of a standard container. The standard containers require the usual copy semantics.** For example:

``` cpp
std::vector<auto_ptr<X> >v;	// error
```

An auto_ptr holds a pointer to an individual element, not a pointer to an array:

``` cpp
void f(int n)
{
	auto_ptr<X> p(new X[n]);	// error
	// ...
}
```

This is an error because the destructor will delete the pointer using `delete` rather than `delete[]` and will fail to invoke the destructor for the last n-1 Xs.

So should we use an auto_array to hold arrays? No. **There is no auto_array**. The reason is that there isn't a need for one. A better solution is to use a vector:

``` cpp
void f(int n)
{
	vector<X> v(n);
	// ...
}
```

Should an exception occur in the ... part, v's destructor will be correctly invoked.

In [C++11](https://www.stroustrup.com/C++11FAQ.html) use a [Unique_ptr](http://www.stroustrup.com/C++11FAQ.html#std-unique_ptr) instead of auto_ptr.

## Why doesn't C++ provide heterogenous(不同种类的) containers?

The C++ standard library provides a set of useful, statically type-safe, and efficient containers. Examples are `vector`, `list`, and `map`:

``` cpp
vector<int> vi(10);
vector<Shape*> vs;
list<string> lst;
list<double> l2
map<string,Record*> tbl;
map< Key,vector<Record*> > t2;
```

These containers are described in all good C++ textbooks, and should be preferred over [arrays](https://www.stroustrup.com/bs_faq2.html#arrays) and "home cooked" containers unless there is a good reason not to.

These containers are homogeneous(相同种类的); that is, they hold elements of the same type. If you want a container to hold elements of several different types, you must express that either as a `union` or (usually much better) as a container of pointers to a polymorphic type. The classical example is:

``` cpp
vector<Shape*> vi;	// vector of pointers to Shapes
```

Here, vi can hold elements of any type derived from Shape. That is, vi is homogeneous in that all its elements are Shapes (to be precise, pointers to Shapes) and heterogeneous in the sense that vi can hold elements of a wide variety of Shapes, such as Circles, Triangles, etc.

So, in a sense all containers (in every language) are homogenous because to use them there must be a common interface to all elements for users to rely on. Languages that provide containers deemed heterogenous simply provide containers of elements that all provide a standard interface. For example, Java collections provide containers of (references to) Objects and you use the (common) Object interface to discover the real type of an element.

The C++ standard library provides homogeneous containers because those are the easiest to use in the vast majority of cases, gives the best compile-time error message, and imposes no unnecessary run-time overheads.

**If you need a heterogeneous container in C++, define a common interface for all the elements and make a container of those.** For example:

``` cpp
class Io_obj { /* ... */ };	// the interface needed to take part in object I/O

	vector<Io_obj*> vio;		// if you want to manage the pointers directly
	vector< Handle<Io_obj> > v2;	// if you want a "smart pointer" to handle the objects
```

Don't drop to the lowest level of implementation detail unless you have to:

``` cpp
vector<void*> memory;	// rarely needed
```

A good indication that you have "gone too low level" is that your code gets littered with casts.

Using an Any class, such as Boost::Any, can be an alternative in some programs:

``` cpp
vector<Any> v;
```

## Why are the standard containers so slow?











# Refer

* [Bjarne Stroustrup's C++ Style and Technique FAQ - Modified November 22, 2019](https://www.stroustrup.com/bs_faq2.html)
* [Bjarne Stroustrup's FAQ](https://www.stroustrup.com/bs_faq.html)
* [C++ Style and Technique FAQ （中文版）](https://www.stroustrup.com/bstechfaq.htm)
* [Bjarne Stroustrup 的 C++ 风格与技术 FAQ（中文版）](https://www.stroustrup.com/bsfaq2cn.html)
* [isocpp.org C++ FAQ](https://isocpp.org/faq)





  

	
	