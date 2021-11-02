#include <iostream>
#include <chrono>

int main()
{
	auto start = std::chrono::system_clock::now();
	int sum = 0;
	int count = 1000000;

	for (int i = 0 ; i < count; i += 2) {  
		sum += i;
		sum += i + 1;
	}

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> dura = end - start;
	std::cout <<"used: "<< dura.count() << "s" << std::endl;
}
