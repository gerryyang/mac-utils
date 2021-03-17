#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>

struct Base
{
	Base() { std::cout << "  Base::Base()\n"; }
	// Note: non-virtual destructor is OK here
	~Base() { std::cout << "  Base::~Base()\n"; }
};

struct Derived: public Base
{
	Derived() { std::cout << "  Derived::Derived()\n"; }
	~Derived() { std::cout << "  Derived::~Derived()\n"; }
};

void thr(std::shared_ptr<Base>& p)
{
	// In multithreaded environment, the value returned by use_count is approximate

	//static std::mutex io_mutex;
	//std::lock_guard<std::mutex> lk(io_mutex);

	auto lp = p; // thread-safe, even though the shared use_count is incremented

	std::cout << "local pointer in a thread:\n"
		<< "  lp.get() = " << lp.get()
		<< ", lp.use_count() = " << lp.use_count() << '\n';

	std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main()
{
	std::shared_ptr<Base> p = std::make_shared<Derived>();

	std::cout << "Created a shared Derived (as a pointer to Base)\n"
		<< "  p.get() = " << p.get()
		<< ", p.use_count() = " << p.use_count() << '\n';

	auto p2 = p;

	std::cout << "Copy a shared Derived (as a pointer to Base)\n"
		<< "  p.get() = " << p.get()
		<< ", p.use_count() = " << p.use_count() << '\n';

	p.reset(); // release ownership from main

	std::cout << "Release ownership from main\n"
		<< "  p.get() = " << p.get()
		<< ", p.use_count() = " << p.use_count()
		<< "  p2.get() = " << p2.get()
		<< ", p2.use_count() = " << p2.use_count() << '\n';

	std::vector<std::thread> thr_vec;

	for (int i = 0; i != 10; ++i)
	{
		// use std::ref, otherwise tmp object will be created, the result will not clear to see
		std::thread t(thr, std::ref(p2));

		thr_vec.push_back(std::move(t));
	}

	for (auto &t : thr_vec)
	{
		t.join();
	}

	std::cout << "Shared ownership between threads and released\n"
		<< "ownership from main:\n"
		<< "  p2.get() = " << p2.get()
		<< ", p2.use_count() = " << p2.use_count() << '\n';


	std::cout << "All threads completed, the last one deleted Derived\n";

	return 0;
}

