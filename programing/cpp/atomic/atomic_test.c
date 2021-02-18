#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "atomic_counter.h"

// gcc -o ac atomic_test.c atomic_counter.c

int test_dec()
{
	printf("test_dec:\n");

	int i, j, p = 0;

	atomic_counter ac;
	atomic_counter_init(&ac, SHM_KEY, 0);
	atomic_counter_set(&ac, 10000000);

	// fork 9 child process
	for (i = 0; i < 9; ++i)
	{
		if (fork() == 0)
		{
			p = i + 1;
			break;
		}
	}

	for (j = 0; j < 1000000; ++j)
	{
		atomic_counter_dec(&ac);
	}
	
	if (p == 0)
	{
		// parent process

		// parent waits for child to teminate
		while (wait(NULL) != -1);

		// result
		printf("dec%d: val=%u\n", p, ac.ptr->dwCounter);
	}
	else
	{
		// 9 child process

		printf("dec%d: val=%u\n", p, ac.ptr->dwCounter);
		exit(0);
	}

	return 0;
}

int test_inc()
{
	printf("test_inc:\n");

	int i, j, p = 0;

	atomic_counter ac;
	atomic_counter_init(&ac, SHM_KEY, 0);
	atomic_counter_set(&ac, 0);

	for (i = 0; i < 9; ++i)
	{
		if (fork() == 0)
		{
			p = i + 1;
			break;
		}
	}

	for (j = 0; j < 1000000; ++j)
	{
		atomic_counter_inc(&ac);
	}
	
	printf("inc%d: val1=%u\n", p, ac.ptr->dwCounter);

	for (j = 0; j < 1000100; ++j)
	{
		atomic_counter_inc(&ac);
	}
	printf("inc%d: val2=%u\n", p, ac.ptr->dwCounter);

	if (p == 0)
	{
		while (wait(NULL) != -1);
	}
	else
	{
		exit(0);
	}

	return 0;
}

int test_add()
{
	printf("test_add:\n");

	int i, j, p = 0;

	atomic_counter ac;
	atomic_counter_init(&ac, SHM_KEY, 0);
	atomic_counter_set(&ac, 0);

	for (i = 0; i < 9; ++i)
	{
		if (fork() == 0)
		{
			p = i + 1;
			break;
		}
	}

	for (j = 0; j < 1000000; ++j)
	{
		atomic_counter_add(&ac, 1);
	}
	
	printf("add%d: val=%u\n", p, ac.ptr->dwCounter);

	if (p == 0)
	{
		while (wait(NULL) != -1);
	}
	else
	{
		exit(0);
	}

	return 0;
}

int test_error_inc()
{
	printf("test_error_inc:\n");

	int i, j, p = 0;

	atomic_counter ac;
	atomic_counter_init(&ac, SHM_KEY, 0);
	atomic_counter_set(&ac, 0);

	for (i = 0; i < 9; ++i)
	{
		if (fork() == 0)
		{
			p = i + 1;
			break;
		}
	}

	for (j = 0; j < 1000000; ++j)
	{
		// error, no atomic
		++(ac.ptr->dwCounter);
	}
	
	printf("inc%d: val=%u\n", p, ac.ptr->dwCounter);

	if (p == 0)
	{
		while (wait(NULL) != -1);
	}
	else
	{
		exit(0);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	test_error_inc();

	test_dec();

	test_inc();

	test_add();

	return 0;
}
