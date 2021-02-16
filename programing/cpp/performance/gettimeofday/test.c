#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

/*
 * Checked against the Intel manual and GCC --hpreg
 *
 * volatile because the tsc always changes without the compiler knowing it.
 */
#define RDTSC() ({ register uint64_t tim; __asm__ __volatile__( "rdtsc" : "=A" (tim) ); tim; })
#define RDTSCP(aux) ({ register uint64_t tim; __asm__ __volatile__( "rdtscp" : "=A" (tim),"=c" (aux) ); tim; })

// Static variables are initialized to 0
static struct timeval walltime;
static uint64_t walltick;
static int cpuspeed_mhz;
static int has_rdtscp;
static int cpuid;

static inline int test_rdtscp()
{
	register uint32_t edx;
	__asm__ __volatile__(
			"movl $0x80000001, %%eax \n\t"
			"cpuid"
			: "=d" (edx) : : "eax","ebx","ecx"
			);
	return (edx & (1U<<27));
}


/*
 * Returns CPU clock in mhz
 * Notice that the function will cost the calling thread to sleep wait_us us
 */
static inline int getcpuspeed_mhz(unsigned int wait_us)
{
	uint64_t tsc1, tsc2;
	struct timespec t;

	t.tv_sec = 0;
	t.tv_nsec = wait_us * 1000;

	tsc1 = RDTSC();

	// If sleep failed, result is unexpected
	if(nanosleep(&t, NULL))
		return -1;

	tsc2 = RDTSC();

	return (tsc2 - tsc1)/(wait_us);
}

static int getcpuspeed()
{
	static int speed = -1;

	while(speed<100)
		speed = getcpuspeed_mhz(50*1000);

	return speed;
}

#define REGET_TIME_US	1

//#define getcpuspeed() 2000

# define TIME_ADD_US(a, usec)                      \
	do {                                             \
		(a)->tv_usec += usec;                        \
		while((a)->tv_usec >= 1000000)               \
		{                                            \
			(a)->tv_sec ++;                            \
			(a)->tv_usec -= 1000000;                   \
		}                                            \
	} while (0)


//
// Compile with -O2 to optimize mul/div instructions
// The performance is restricted by 2 factors:
//    1, the rdtsc instruction
//    2, division
//
// Another restriction for this function:
//    The caller thread should run on one CPU or on SMP with sinchronized TSCs,
// otherwise, rdtsc instruction will differ between multiple CPUs.
//    The good thing is that most multicore CPUs are shipped with sinchronized TSCs.
//
static int my_gettimeofday(struct timeval *tv)
{
	uint64_t tick = 0;
	// max_time_us = max_ticks / cpuspeed_mhz > RELOAD_TIME_US us
	static unsigned int max_ticks = 2000*REGET_TIME_US;

	if(walltime.tv_sec==0 || cpuspeed_mhz==0 ||
			// If we are on a different cpu with unsynchronized tsc,
			// RDTSC() may be smaller than walltick
			// in this case tick will be a negative number,
			// whose unsigned value is much larger than max_ticks
			(tick=RDTSC()-walltick) > max_ticks)
	{
		if(tick==0 || cpuspeed_mhz==0)
		{
			cpuspeed_mhz = getcpuspeed();
			max_ticks = cpuspeed_mhz*REGET_TIME_US;
		}

		gettimeofday(tv, NULL);
		memcpy(&walltime, tv, sizeof(walltime));
		walltick = RDTSC();
		return 0;
	}

	memcpy(tv, &walltime, sizeof(walltime));

	// if REGET_TIME_US==1, we are currently in the same us, no need to adjust tv
#if REGET_TIME_US > 1
	{
		uint32_t t;
		t = ((uint32_t)tick) / cpuspeed_mhz;
		TIME_ADD_US(tv, t);
	}
#endif
	return 0;
}


int main(int argc, char *argv[])
{
	int i;
	unsigned int loops = 10000000;
	struct timeval t1, t2, t3;

	if(argc<2)
	{
		printf("Please input an argument:\n\t1 compare time\n\t2 syscall\n\t3 fastcall\n");
		return 0;
	}

	if(argc>2)
		loops = strtoul(argv[2], NULL, 10);

	if(argv[1][0]=='1')
	{
		int spd = getcpuspeed();
		uint64_t max = 0, diff = 0, nr_diff1 = 0, nr_diff10 = 0;
		uint64_t tsc = RDTSC();
		uint64_t af = 1;
		int hastscp = 0;
		uint32_t aux = 0;

		if((hastscp=test_rdtscp()))
			printf("This machine supports rdtscp.\n");
		else
			printf("This machine does not support rdtscp.\n");

		if(hastscp)
			tsc = RDTSCP(aux);
		else
			tsc = RDTSC();

		if(sched_setaffinity(0, sizeof(af), &af))
			perror("failed to set affinity");

		printf("tsc=%llu, aux=0x%x, cpu speed %d mhz\n", tsc, aux, spd);
		printf("getspeed_010:%d\n", getcpuspeed_mhz(10*1000));
		printf("getspeed_100:%d\n", getcpuspeed_mhz(100*1000));
		printf("getspeed_500:%d\n", getcpuspeed_mhz(500*1000));

		for(i=0; i<loops; i++)
		{
#if 1
			my_gettimeofday(&t1);
#else
			gettimeofday(&t1, NULL);
#endif
			gettimeofday(&t2, NULL);

			if(timercmp(&t1, &t2, >))
				timersub(&t1, &t2, &t3);
			else
				timersub(&t2, &t1, &t3);

			//			printf("t1=%u.%06u\t", t1.tv_sec, t1.tv_usec);
			//			printf("t2=%u.%06u\t", t2.tv_sec, t2.tv_usec);
			//			printf("diff=%u.%06u\n", t3.tv_sec, t3.tv_usec);

			if(max<t3.tv_usec) max = t3.tv_usec;
			if(t3.tv_usec>1) nr_diff1 ++;
			if(t3.tv_usec>10) nr_diff10 ++;
			diff += t3.tv_usec;
			//			if(i%20==0) usleep((i%10)<<5);
		}
		printf("max diff=%llu, ave diff=%llu, diff1 count=%llu, diff10 count=%llu.\n", max, diff/i, nr_diff1, nr_diff10);
	}
	else if(argv[1][0]=='2')
	{
		for(i=0; i<loops; i++)
			gettimeofday(&t1, NULL);
	}
	else if(argv[1][0]=='3')
	{
		for(i=0; i<loops; i++)
			my_gettimeofday(&t1);
	}
}

