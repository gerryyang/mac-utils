#include <stdio.h>
#include <stdint.h>

int main()
{
	intptr_t i = 10;
	printf("i[%d]\n", i);// 10
	return 0;
}
/*
 gcc -Wno-format -o intptr_t_test intptr_t_test.c 
 */
