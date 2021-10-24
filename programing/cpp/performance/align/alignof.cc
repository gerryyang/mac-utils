#include <iostream>
#include <memory>
#include <chrono>
#include <string.h>

#define MAXLOOP 10000000

static inline void *__movsb(void *d, const void *s, size_t n)
{
	asm volatile ("rep movsb"
			: "=D" (d),
			"=S" (s),
			"=c" (n)
			: "0" (d),
			"1" (s),
			"2" (n)
			: "memory");
	return d;
}

int main(int argc, char *argv[])
{
	constexpr int align_size = 32;
	//constexpr int align_size = 64;

	constexpr int alloc_size = 10000; 
	constexpr int buff_size = align_size + alloc_size;

	char dst[buff_size] = {0};
	char src[buff_size] = {0};

	void *dst_ori_ptr = dst;
	void *src_ori_ptr = src;

	size_t dst_size = sizeof(dst);
	size_t src_size = sizeof(src);

	char *dst_ptr = static_cast<char *>(std::align(align_size, alloc_size, dst_ori_ptr, dst_size));
	char *src_ptr = static_cast<char *>(std::align(align_size, alloc_size, src_ori_ptr, src_size));
	std::cout << "dst_ptr: " << (void*)dst_ptr << ", src_ptr: " << (void*)src_ptr << std::endl;

	if (argc == 2 && argv[1][0] == '1') {
		++dst_ptr;
		++src_ptr;
	}

	auto beg = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < MAXLOOP; ++i) {
		//__movsb(dst_ptr, src_ptr, alloc_size);
		memcpy(dst_ptr, src_ptr, alloc_size);
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - beg;
	std::cout << "elapsed time: " << diff.count() << std::endl;
}
