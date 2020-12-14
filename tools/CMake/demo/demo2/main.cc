#include <iostream>
#include "math.h"
#include "error_code.h"

int main()
{
	int a{ 0 }, b{ 0 }, c{ 0 };
	std::cin >> a >> b >> c;
	int sum{ 0 };
	int ret_val = SafeAdd(sum, a, b, c);
	std::cout << sum << std::endl;
	return ret_val;
}
