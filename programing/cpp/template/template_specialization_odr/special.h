#pragma once

#include "class_template.h"
#include <iostream>
#include <type_traits>

//static_assert(false, "should not be included");

template<>
inline void A<int>::f(int a)
{
        std::cout << "A<T>::f(int a)\n";
}

