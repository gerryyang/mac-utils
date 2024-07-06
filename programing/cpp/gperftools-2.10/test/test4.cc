#include <iostream>
#include <dlfcn.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

// ProfilerStart / HeapProfilerStart
typedef void (*libfunc1)(const char* fname);

// ProfilerFlush / ProfilerStop / HeapProfilerStop 
typedef void (*libfunc2)(void);

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

void f4() { 
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

int test()
{
	// ProfilerStart / ProfilerFlush / ProfilerStop
	const char* library_path1 = "lib/libprofiler.so";
	void* handle1 = dlopen(library_path1, RTLD_NOW);
	if (!handle1) {
		std::cerr << "Error loading library: " << dlerror() << std::endl;
		return 1;
	}
	dlerror();

	libfunc1 fProfilerStart = reinterpret_cast<libfunc1>(dlsym(handle1, "ProfilerStart"));	
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Error loading symbol 'ProfilerStart': " << dlsym_error << std::endl;
		dlclose(handle1);
		return 1;
	}
	dlerror();

	libfunc2 fProfilerFlush= reinterpret_cast<libfunc2>(dlsym(handle1, "ProfilerFlush"));	
	dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Error loading symbol 'ProfilerFlush': " << dlsym_error << std::endl;
		dlclose(handle1);
		return 1;
	}
	dlerror();

	libfunc2 fProfilerStop= reinterpret_cast<libfunc2>(dlsym(handle1, "ProfilerStop"));	
	dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Error loading symbol 'ProfilerStop': " << dlsym_error << std::endl;
		dlclose(handle1);
		return 1;
	}
	dlerror();

	// HeapProfilerStart / HeapProfilerStop
	const char* library_path2 = "lib/libtcmalloc.so";
	void* handle2 = dlopen(library_path2, RTLD_NOW);
	if (!handle2) {
		std::cerr << "Error loading library: " << dlerror() << std::endl;
		return 1;
	}
	dlerror();

	libfunc1 fHeapProfilerStart = reinterpret_cast<libfunc1>(dlsym(handle2, "HeapProfilerStart"));	
	dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Error loading symbol 'HeapProfilerStart': " << dlsym_error << std::endl;
		dlclose(handle1);
		dlclose(handle2);
		return 1;
	}
	dlerror();

	libfunc1 fHeapProfilerDump = reinterpret_cast<libfunc1>(dlsym(handle2, "HeapProfilerDump"));	
	dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Error loading symbol 'HeapProfilerDump': " << dlsym_error << std::endl;
		dlclose(handle1);
		dlclose(handle2);
		return 1;
	}
	dlerror();


	libfunc2 fHeapProfilerStop= reinterpret_cast<libfunc2>(dlsym(handle2, "HeapProfilerStop"));	
	dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Error loading symbol 'HeapProfilerStop': " << dlsym_error << std::endl;
		dlclose(handle1);
		dlclose(handle2);
		return 1;
	}
	dlerror();

	// cpu
	fProfilerStart("cpu.prof");

	// heap
	fHeapProfilerStart("heap.prof");

	f3();
	f4();

	// cpu
	fProfilerFlush();
	fProfilerStop();

	// heap
	fHeapProfilerDump("over");
	fHeapProfilerStop();

	dlclose(handle1);
	dlclose(handle2);

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

		test();

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
