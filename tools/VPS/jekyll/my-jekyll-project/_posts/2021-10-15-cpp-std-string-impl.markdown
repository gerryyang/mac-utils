---
layout: post
title:  "CPP std::string Implentation (COW/SSO)"
date:   2021-10-15 12:30:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# 示例

``` cpp
#include <cstdio>
#include <string>

int main()
{
    printf("%lu\n", sizeof(std::string));
    
    std::string s1 = "a";
    std::string s2 = s1;
    printf("%p\n%p", s1.c_str(), s2.c_str());
}
```

GCC 4.8.5 输出：

```
8
0x109cd88
0x109cd88
```

GCC 5.1.0 输出：

```
32
0x7ffe01afc030
0x7ffe01afc010
```

内存分配：

``` cpp
#include <iostream>
#include <cstdlib>
#include <string>

// rewrite operator new to print log
void* operator new(std::size_t n)
{
    //std::cout << "allocate " << n << " bytes" << std::endl;
    return malloc(n);
}

void operator delete(void *p) throw()
{
    //std::cout << "delete " << p << std::endl;
    free(p);
}

void print(const std::string& scene, const std::string& s)
{
    std::cout << "[" << scene << "] sizeof: " << sizeof(s) << " size: " << s.size() << " capacity: " << s.capacity() << std::endl;
}

int main()
{
    std::string s("hello");
    print("init", s);
    
    for (size_t i = 0; i != 24; ++i) {
        s.push_back('+');
        //std::cout << i << ":" << s << std::endl;
    }
    print("after push_back", s);
}
```

# 实现

* 实现标准：ISO C++ 14882: 21 Strings library

* `GCC 4.8.5`的`std::string`通过`Reference-counted COW string implentation`方式实现，采用了写时复制的引用计数（`_Rep`类），内部仅有一个实际持有的数据指针`_M_dataplus`，因此`sizeof(std::string) == 8`，而且由`COW`实现也可见多线程下使用不安全。

* 自从`GCC 5.1`开始，`std::string`引入了遵从`C++11`标准的新实现，默认使用`SSO`(`small string optimization`)特性，禁用了写时复制（`COW`）引用计数机制，这也带来了与旧版本`std::string`的`ABI`兼容性问题。`GCC 5.1.`下`std::string`中使用了三个私有成员：实际持有的数据指针`_M_dataplus`、数据实际大小`_M_string_length`、针对短小字符串优化的默认16字节大小的匿名`union`对应的栈上内存，因此`sizeof(std::string) == 32`，而且已不再采用引用计数机制，每个对象真正持有实际数据，线程安全。

* 对于新的`std::string`实现导致的不同版本GCC编译导致的二进制不兼容问题，GCC5 提供了相应的二进制兼容宏开关`_GLIBCXX_USE_CXX11_ABI`。若`_GLIBCXX_USE_CXX11_ABI=0`，则链接旧版本；若`_GLIBCXX_USE_CXX11_ABI=1`，则链接新版本。


``` cpp
// GCC 4.8.5
template<typename _CharT, typename _Traits = char_traits<_CharT>,
         typename _Alloc = allocator<_CharT> >
class basic_string;
typedef basic_string<char> string;
 
// refer:
// gcc-releases-gcc-4.8.5/libstdc++-v3/include/bits/basic_string.h

template<typename _CharT, typename _Traits, typename _Alloc>
class basic_string
{
 private:
  struct _Rep_base
  {
    size_type _M_length;
    size_type _M_capacity;
    _Atomic_word _M_refcount;
  };
 
  struct _Rep : _Rep_base
  {
  };
  // Use empty-base optimization: http://www.cantrip.org/emptyopt.html
  struct _Alloc_hider : _Alloc
  {
    _Alloc_hider(_CharT* __dat, const _Alloc& __a)
        : _Alloc(__a), _M_p(__dat) { }

    _CharT* _M_p; // The actual data.
  };
 
  // Data Members (private)
  mutable _Alloc_hider _M_dataplus;
  
  _CharT* _M_data() const
  { return _M_dataplus._M_p; }
 
  _Rep* _M_rep() const
  { return &((reinterpret_cast<_Rep*> (_M_data()))[-1]); }
};
```

``` cpp
// GCC 5.1
template<typename _CharT, typename _Traits, typename _Alloc>
class basic_string
{
 private:
  // Use empty-base optimization: http://www.cantrip.org/emptyopt.html
  struct _Alloc_hider : allocator_type // TODO check __is_final
  {
    _Alloc_hider(pointer __dat, const _Alloc& __a = _Alloc())
        : allocator_type(__a), _M_p(__dat) { }
 
    pointer _M_p; // The actual data.
  };
 
  _Alloc_hider	_M_dataplus;
  size_type     _M_string_length;
 
  enum { _S_local_capacity = 15 / sizeof(_CharT) };
 
  union
  {
      _CharT           _M_local_buf[_S_local_capacity + 1];
      size_type        _M_allocated_capacity;
  };
};
```

# libstdc++ vs libc++ 

> libc++ is not binary compatible with gcc's libstdc++ (except for some low level stuff such as operator new). For example the std::string in gcc's libstdc++ is refcounted, whereas in libc++ it uses the "short string optimization". If you were to accidentally mix these two strings in the same program (and mistake them for the same data structure), you would inevitably get a run time crash.

refer: 

* [Why can't clang with libc++ in c++0x mode link this boost::program_options example?](https://stackoverflow.com/questions/8454329/why-cant-clang-with-libc-in-c0x-mode-link-this-boostprogram-options-examp/8457799#8457799)
* [Is it safe to link 2 different standard c++ library in one project](https://stackoverflow.com/questions/27894380/is-it-safe-to-link-2-different-standard-c-library-in-one-project)
* [How do you find what version of libstdc++ library is installed on your linux machine?](https://stackoverflow.com/questions/10354636/how-do-you-find-what-version-of-libstdc-library-is-installed-on-your-linux-mac)
* [ABI Policy and Guidelines - Versioning](https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html#abi.versioning)


# Refer

* [GCC5 std::string新变化](http://www.pandademo.com/2017/04/new-changes-of-gcc5-std-string/)
* [C++的string-两手抓的内存分配](https://zhuanlan.zhihu.com/p/187499607)

  

	
	