#pragma once

#include <iostream>

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
}

#if 0
template<>
void A<int>::f(int a)
{
	std::cout << "A<T>::f(int a)\n";
}
#endif


