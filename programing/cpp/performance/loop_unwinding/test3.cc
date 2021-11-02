#include <iostream>
#include <chrono>

int main()
{
	auto start = std::chrono::system_clock::now();
	int sum = 0;
	int count = 1000000;

	int sum1 = 0, sum2 = 0;
	for (int i = 0 ; i < count; i += 2) {  
		sum1 += i;
		sum2 += i + 1;
	}
	sum = sum1 + sum2;

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> dura = end - start;
	std::cout <<"used: "<< dura.count() << "s" << std::endl;
}
