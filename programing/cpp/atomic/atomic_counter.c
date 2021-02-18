#include <sys/shm.h>
#include "atomic_counter.h"

int get_shm(void **ptr, int key, int size)
{
	int id;
	int ret = 0;

	if ((id = shmget(key, size, 0666)) < 0) {
		if((id = shmget(key, size, 0666|IPC_CREAT)) < 0) {
			perror("shmget");
			return -1;
		}
		ret = 1; // created
	}
	if ((*ptr = shmat(id, NULL ,0)) == (void *) -1) {
		perror("shmat");
		return -2;
	}
	return 0;
}

int atomic_counter_init(atomic_counter * pstCounter, key_t shmkey, uint32_t iVal)
{
    int iRet;
    void * ptr;
    if (pstCounter == NULL)
    {
        return -1;
    }
    if ( (iRet = get_shm(&ptr, shmkey, sizeof(atomic_counter_t))) < 0)
    {
        return -20;
    }
    pstCounter->ptr = (atomic_counter_t *)ptr;
    if (iRet == 1)  /*新建了共享内存*/
    {
        (pstCounter)->ptr->dwCounter = iVal;
    }
    return 0;
}

int atomic_counter_inc(atomic_counter * pstCounter)
{
    if (pstCounter == NULL)
    {
        return -1;
    }

    // 用户需要指定lock前缀
    asm volatile( "lock; incl %0" : "+m" (pstCounter->ptr->dwCounter));
    return 0;
}

/*
 * Atomic compare and exchange.  Compare OLD with MEM, if identical,
 * store NEW in MEM.  Return the initial value in MEM.  Success is
 * indicated by comparing RETURN with OLD.
 */
#define cmpxchg(ptr, old, new)            	     		\
({								\
	uint32_t __ret;	        				\
	uint32_t __old = (old);					\
	uint32_t __new = (new);					\
	volatile uint32_t *__ptr = (volatile uint32_t *)(ptr);	\
	asm volatile("lock; cmpxchgl %2,%1"			\
		     : "=a" (__ret), "+m" (*__ptr)		\
		     : "r" (__new), "0" (__old)			\
		     : "memory");				\
	__ret;							\
})

int atomic_counter_dec(atomic_counter * pstCounter)
{
    uint32_t c, old, dec;
    if (pstCounter == NULL)
    {
        return -1;
    }

    // 这里实际上是dec_if_nonzero，使用cmpxchg保证原子操作
    c = pstCounter->ptr->dwCounter;
    for (;;)
    {
        dec = c - 1;
        if (dec == (uint32_t)-1) // c is zero, return
        {
            break;
        }

        old = cmpxchg(&pstCounter->ptr->dwCounter, c, dec);
        if (old == c) // value is not changed by others, operation successful
        {
            break;
        }

        c = old; // someone is accessing the counter, do it again
    }
    
    return 0;
}

int atomic_counter_set(atomic_counter * pstCounter, uint32_t val)
{
    if (pstCounter == NULL)
    {
        return -1;
    }

    (pstCounter->ptr->dwCounter) = val;
    return 0;
}

int atomic_counter_get(atomic_counter * pstCounter, uint32_t *pval)
{
    if (pstCounter == NULL || pval == NULL)
    {
        return -1;
    }

    *pval = (pstCounter->ptr->dwCounter) ;
    return 0;
}

int atomic_counter_add(atomic_counter * pstCounter, uint32_t val)
{
    if (pstCounter == NULL)
    {
        return -1;
    }

    // 用户需要指定lock前缀
    __asm__ __volatile__( "lock ; addl %1,%0" :"+m" (pstCounter->ptr->dwCounter) :"ir" (val));
    return 0;
}
