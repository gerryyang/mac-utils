#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

void f1() {
	// test heap
	int i;
	for (i = 0; i < 1024* 1024; ++i) {
		short* p = new short;
		//delete p;
	}

	// test cpu
	i = 0;
	while (i < 100000) {
		++i;
	}
}

void f2()  { 
	// test heap
	int i;
	for (i = 0; i < 1024 * 1024; ++i) {
		int* p = new int;
		//delete p;
	}

	// test cpu
	i = 0;
	while (i < 200000) {
		++i;
	}
}

void f3() {
	// test heap
	int i;
	for (i = 0; i < 1024* 1024; ++i) {
		short* p = new short;
		//delete p;
	}

	// test cpu
	i = 0;
	while (i < 100000) {
		++i;
	}
}

void f4()  { 
	// test heap
	int i;
	for (i = 0; i < 1024 * 1024; ++i) {
		int* p = new int;
		//delete p;
	}

	// test cpu
	i = 0;
	while (i < 200000) {
		++i;
	}

}

int main() {
	int shared_var = 42;

	//f1();
	//f2();

	pid_t pid = fork();
	if (pid == -1) {
		std::cerr << "Fork failed." << std::endl;
		return 1;
	}

	if (pid == 0) {
		// child
		std::cout << "Child process (pid: " << getpid() << "): shared_var = " << shared_var << std::endl;
		shared_var = 13;
		std::cout << "Child process (pid: " << getpid() << "): shared_var changed to " << shared_var << std::endl;

		f3();
		f4();

		// stop
		//std::cout << "Child process (pid: " << getpid() << "): Press Enter to exit..." << std::endl;
		//std::cin.get();
		sleep(1);

	} else {
		// parent
		waitpid(pid, NULL, 0);
		std::cout << "Parent process (pid: " << getpid() << "): shared_var = " << shared_var << std::endl;
	}

	return 0;
}
