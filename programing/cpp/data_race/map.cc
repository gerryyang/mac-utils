#include <iostream>     
#include <thread>         
#include <chrono>
#include <map>
#include <ctime>

std::map<int, int> g_m = {{0,0}};

void f1(int x) 
{
	for (;;) {
		auto i = std::rand();
		g_m.emplace(i, i);

		//std::chrono::nanoseconds ns(100 * 1000 * 1000);
		//std::this_thread::sleep_for(ns);
	}
}

void f2(int x)
{
	for (;;) {
		auto iter = g_m.find(0);
		if (iter == g_m.end()) {
			std::cout << "no find\n";
		} else {
			//std::cout << "find\n";
		}
	}
}

int main(int argc, char**argv) 
{
	std::srand(std::time(nullptr));

	std::thread thrd1 (f1, 0);     
	std::thread thrd2 (f2, 0);  

	thrd1.join();                
	thrd2.join();               

	std::cout << "done\n";

}
