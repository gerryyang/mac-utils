#include <stdio.h>


#define NTHREADS 10
#define ITERATIONS 10000

volatile unsigned int g_var = 0;

int atom_op()
{
	unsigned int old_var = __sync_fetch_and_add(&g_var, 1);
	printf("%d\n", old_var);
}

void* thread_run(void* arg)
{
	int totalIterations = ITERATIONS / NTHREADS;
	for (int i = 1; i <= totalIterations; ++i) {
		atom_op();
	}
}

int main(int argc, char** argv)
{
	pthread_t* thread_ids;
	thread_ids = (pthread_t*)calloc(NTHREADS, sizeof(pthread_t));
	/* create threads */
	int t = 0;
	for (t = 0; t < NTHREADS; t++) {
		pthread_create(&thread_ids[t], NULL, &thread_run, NULL);
	}
	for (t = 0; t < NTHREADS; t++) {
		pthread_join(thread_ids[t], NULL);
	}
	free(thread_ids);

	return 0;
}

