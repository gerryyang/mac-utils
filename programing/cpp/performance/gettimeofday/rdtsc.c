unsigned long long rdtsc(void)
{
    unsigned long long tick;
    __asm__ __volatile__("rdtsc" : "=A"(tick));
    return tick;
}

int main()
{
    rdtsc();
}
// gcc -m32 rdtsc.c
