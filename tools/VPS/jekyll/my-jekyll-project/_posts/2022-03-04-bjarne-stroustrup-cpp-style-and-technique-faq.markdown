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

The users are now insulated from changes to implementations of derived classes. **I have seen this technique decrease build times by orders of magnitudes.**

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







# Refer

* [Bjarne Stroustrup's C++ Style and Technique FAQ - Modified November 22, 2019](https://www.stroustrup.com/bs_faq2.html)
* [C++ Style and Technique FAQ （中文版）](https://www.stroustrup.com/bstechfaq.htm)
* [Bjarne Stroustrup 的 C++ 风格与技术 FAQ（中文版）](https://www.stroustrup.com/bsfaq2cn.html)
* [isocpp.org C++ FAQ](https://isocpp.org/faq)





  

	
	