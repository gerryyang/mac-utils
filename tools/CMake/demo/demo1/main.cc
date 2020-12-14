// safe_add.cpp
#include <iostream>
#include <memory>
#define INT_MAX 2147483647
#define ERROR_DATA_OVERFLOW 2

int SafeIntAdd(std::unique_ptr<int> &sum, int a, int b)
{
	if (a > INT_MAX - b)
	{
		*sum = INT_MAX;
		return ERROR_DATA_OVERFLOW;
	}
	*sum = a + b;
	return EXIT_SUCCESS;
}

int main()
{
	int a, b;
	std::cin >> a >> b;
	std::unique_ptr<int> sum(new int(1));
	int res = SafeIntAdd(sum, a, b);
	std::cout << *sum << std::endl;
	return res;
}
