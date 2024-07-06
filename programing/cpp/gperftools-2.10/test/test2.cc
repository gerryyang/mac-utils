#include <iostream>
#include <dlfcn.h> 
#include <unistd.h>
using namespace std;


// ProfilerStart / HeapProfilerStart
typedef void (*libfunc1)(const char* fname);

// ProfilerFlush / ProfilerStop / HeapProfilerStop 
typedef void (*libfunc2)(void);

void f1() {
	int i;
	for (i = 0; i < 1024* 1024; ++i) {
		short* p = new short;
		//delete p;
	}
}

void f2()  { 
	int i;
	for (i = 0; i < 1024 * 1024; ++i) {
		int* p = new int;
		//delete p;
	}
}

int main() { 
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

	//fProfilerStart("cpu.prof");
	
	fHeapProfilerStart("heap.prof");

	f1();
	f2();

	//fProfilerFlush();
	//fProfilerStop();

	fHeapProfilerDump("over");
	fHeapProfilerStop();

	dlclose(handle1);
	dlclose(handle2);

	sleep(-1);

	return 0;
}
