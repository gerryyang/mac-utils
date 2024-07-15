#include <stdio.h>

int main()
{
    printf("Triggering SIGTRAP signal...\n");

    // 使用 int3 指令触发 SIGTRAP 信号
    __asm__("int3");

    printf("SIGTRAP signal triggered, but not terminated. Continuing...\n");

    return 0;
}