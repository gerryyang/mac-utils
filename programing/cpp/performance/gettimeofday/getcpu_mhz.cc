#include <iostream>

unsigned long long rdtsc1(void)
{
    unsigned long long tick;
    __asm__ __volatile__("rdtsc" : "=A"(tick));
    return tick;
}

unsigned long long rdtsc2(void)
{
    unsigned int tickl, tickh;
    __asm__ __volatile__("rdtsc" : "=a"(tickl), "=d"(tickh));
    return ((unsigned long long)tickh << 32) | tickl;
}

#ifdef __x86_64__
#define RDTSC()                                                       \
    (                                                                 \
        {                                                             \
            unsigned int tickl, tickh;                                \
            __asm__ __volatile__("rdtsc" : "=a"(tickl), "=d"(tickh)); \
            ((unsigned long long)tickh << 32) | tickl;                \
        })
#else
#define RDTSC()                                         \
    (                                                   \
        {                                               \
            unsigned long long tick;                    \
            __asm__ __votatile__("rdtsc" : "=A"(tick)); \
            tick;                                       \
        })
#endif

#if 0
#ifdef __x86_64__
#define RDTSC() rdtsc1()
#else
#define RDTSC() rdtsc1()
#endif
#endif

int getcpu_mhz(unsigned int wait_us)
{
    unsigned long long tsc1, tsc2;
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = wait_us * 1000;
    tsc1 = RDTSC();
    if (nanosleep(&t, NULL))
    {
        return -1;
    }
    tsc2 = RDTSC();
    return (tsc2 - tsc1) / wait_us;
}

int main()
{
    for (;;)
    {
        std::cout << getcpu_mhz(500 * 1000) << std::endl;
    }
}
//g++ getcpu_mhz.cc
//g++ -m32 getcpu_mhz.cc
