#include "util.h"
#include <iostream>
#include "special.h"
#include "class_template.h"

void g()
{
	std::cout << "void g()\n";

	int a = 1;
	A<int>::f(a);
}
