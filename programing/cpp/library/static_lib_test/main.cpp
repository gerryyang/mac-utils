// main.cpp
#include <cstdio>
#include "staticliba.h"
#include "staticlibb.h"

int main()
{
	API_A::samefunc_nonstatic();
	API_B::samefunc_nonstatic();
	//samefunc_static();
	printf("-----------------\n");

	API_A::seta();
	API_A::reada();
	printf("-----------------\n");

	API_B::setb();
	API_B::readb();
	printf("-----------------\n");

	API_A::seta();
	API_B::readb();
	printf("-----------------\n");

	API_B::setb();
	API_A::reada();

	return 0;
}


