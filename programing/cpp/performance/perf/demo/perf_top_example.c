#include <stdio.h>
#include <unistd.h>

double compute_pi_baseline(size_t N)
{
    double pi = 0.0;
    double dt = 1.0 / N;
    for (size_t i = 0; i < N; i++)
    {
        double x = (double)i / N;
        pi += dt / (1.0 + x * x);
    }
    return pi * 4.0;
}

int main()
{
    printf("pid: %d\n", getpid());
    sleep(10);
    compute_pi_baseline(50000000);
    return 0;
}
