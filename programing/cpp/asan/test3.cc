#include <iostream>
#include <memory>

class B;

class A
{
public:
    std::shared_ptr<B> b_ptr;
    ~A()
    {
        std::cout << "A destructor called" << std::endl;
    }
};

class B
{
public:
    std::shared_ptr<A> a_ptr;
    ~B()
    {
        std::cout << "B destructor called" << std::endl;
    }
};

int main()
{
    {
        std::shared_ptr<A> a = std::make_shared<A>();
        std::shared_ptr<B> b = std::make_shared<B>();

        std::cout << "sizeof(A): " << sizeof(a) << std::endl;

        a->b_ptr = b;
        b->a_ptr = a;
    }  // a 和 b 的析构函数在这里应该被调用，但由于循环引用，它们不会被调用

    std::cout << "Main function ends" << std::endl;
    return 0;
}
/*
$ ./test3
sizeof(A): 16
Main function ends

=================================================================
==4167602==ERROR: LeakSanitizer: detected memory leaks

Indirect leak of 32 byte(s) in 1 object(s) allocated from:
    #0 0x4f2c18 in operator new(unsigned long) /data/home/gerryyang/tools/clang/llvm-project-11.0.0/compiler-rt/lib/asan/asan_new_delete.cpp:99:3
    #1 0x4f763e in __gnu_cxx::new_allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> >::allocate(unsigned long, void const*) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/ext/new_allocator.h:111:27
    #2 0x4f75ba in std::allocator_traits<std::allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> > >::allocate(std::allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> >&, unsigned long) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/alloc_traits.h:436:20
    #3 0x4f72e9 in std::__allocated_ptr<std::allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> > > std::__allocate_guarded<std::allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> > >(std::allocator<std::_Sp_counted_ptr_inplace<B, std::allocator<B>, (__gnu_cxx::_Lock_policy)2> >&) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/allocated_ptr.h:97:21
    #4 0x4f7122 in std::__shared_count<(__gnu_cxx::_Lock_policy)2>::__shared_count<B, std::allocator<B> >(B*&, std::_Sp_alloc_shared_tag<std::allocator<B> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr_base.h:675:19
    #5 0x4f7020 in std::__shared_ptr<B, (__gnu_cxx::_Lock_policy)2>::__shared_ptr<std::allocator<B> >(std::_Sp_alloc_shared_tag<std::allocator<B> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr_base.h:1342:14
    #6 0x4f6fe8 in std::shared_ptr<B>::shared_ptr<std::allocator<B> >(std::_Sp_alloc_shared_tag<std::allocator<B> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:359:4
    #7 0x4f6fbd in std::shared_ptr<B> std::allocate_shared<B, std::allocator<B> >(std::allocator<B> const&) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:705:14
    #8 0x4f5ed0 in std::shared_ptr<B> std::make_shared<B>() /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:721:14
    #9 0x4f59f3 in main /data/home/gerryyang/github/mac-utils/programing/cpp/asan/test3.cc:30:32
    #10 0x7f8ac3206f92 in __libc_start_main (/lib64/libc.so.6+0x26f92)

Indirect leak of 32 byte(s) in 1 object(s) allocated from:
    #0 0x4f2c18 in operator new(unsigned long) /data/home/gerryyang/tools/clang/llvm-project-11.0.0/compiler-rt/lib/asan/asan_new_delete.cpp:99:3
    #1 0x4f69fe in __gnu_cxx::new_allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> >::allocate(unsigned long, void const*) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/ext/new_allocator.h:111:27
    #2 0x4f697a in std::allocator_traits<std::allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> > >::allocate(std::allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> >&, unsigned long) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/alloc_traits.h:436:20
    #3 0x4f66a9 in std::__allocated_ptr<std::allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> > > std::__allocate_guarded<std::allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> > >(std::allocator<std::_Sp_counted_ptr_inplace<A, std::allocator<A>, (__gnu_cxx::_Lock_policy)2> >&) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/allocated_ptr.h:97:21
    #4 0x4f64e2 in std::__shared_count<(__gnu_cxx::_Lock_policy)2>::__shared_count<A, std::allocator<A> >(A*&, std::_Sp_alloc_shared_tag<std::allocator<A> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr_base.h:675:19
    #5 0x4f63e0 in std::__shared_ptr<A, (__gnu_cxx::_Lock_policy)2>::__shared_ptr<std::allocator<A> >(std::_Sp_alloc_shared_tag<std::allocator<A> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr_base.h:1342:14
    #6 0x4f63a8 in std::shared_ptr<A>::shared_ptr<std::allocator<A> >(std::_Sp_alloc_shared_tag<std::allocator<A> >) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:359:4
    #7 0x4f636d in std::shared_ptr<A> std::allocate_shared<A, std::allocator<A> >(std::allocator<A> const&) /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:705:14
    #8 0x4f5d80 in std::shared_ptr<A> std::make_shared<A>() /usr/lib/gcc/x86_64-redhat-linux/8/../../../../include/c++/8/bits/shared_ptr.h:721:14
    #9 0x4f59e1 in main /data/home/gerryyang/github/mac-utils/programing/cpp/asan/test3.cc:29:32
    #10 0x7f8ac3206f92 in __libc_start_main (/lib64/libc.so.6+0x26f92)

SUMMARY: AddressSanitizer: 64 byte(s) leaked in 2 allocation(s).
*/