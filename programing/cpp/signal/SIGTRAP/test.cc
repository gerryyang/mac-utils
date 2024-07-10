#include <csignal>
#include <iostream>

#define TEST_TF

// 信号处理函数，什么也不做
void signal_handler(int signal)
{
    std::cout << signal << " catched, do nothing\n";

#ifdef TEST_TF
    __asm__ __volatile__("pushq $0x0\npopfq");
#endif
}

int main()
{
    // 设置 SIGTRAP 信号处理程序为 signal_handler
    std::signal(SIGTRAP, signal_handler);

    std::cout << "Raising SIGTRAP signal..." << std::endl;

#ifdef TEST_TF
    __asm__ __volatile__("pushq $0x100\npopfq");
#endif

    // 触发 SIGTRAP 信号
    std::raise(SIGTRAP);

    std::cout << "SIGTRAP signal raised, but not terminated. Continuing..." << std::endl;

    return 0;
}