#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

#define NTHREADS 1
#define ITERATIONS 10000

#define MY_SHM_ID 0x33b5

int atom_op()
{
	int shmid = shmget(MY_SHM_ID, sizeof(int), IPC_CREAT | 0666);
	if (shmid == -1) {
		printf("shmget failed, %s(%d,id=%x)", strerror(errno), errno, MY_SHM_ID);
		return -1;
	}

	unsigned int * p_var = (unsigned int *)shmat(shmid, NULL, 0);
	if (p_var == (unsigned int *) - 1) {
		printf("shmat failed, %s(%d,id=%x)", strerror(errno), errno, MY_SHM_ID);
		return -1;
	}

	unsigned int old_var = __sync_fetch_and_add(p_var, 1);
	if (shmdt(p_var) == -1) {
		printf("shmdt failed, %s(%d,id=%x)", strerror(errno), errno, MY_SHM_ID);
		return -1;
	}
	printf("%d\n", old_var);

	return 0;
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

