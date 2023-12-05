---
layout: post
title:  "MultiThread in Action"
date:   2023-07-24 12:00:00 +0800
categories: Linux
---

* Do not remove this line (it will not be displayed)
{:toc}


# 关键接口

## [shm_open](https://man7.org/linux/man-pages/man3/shm_open.3.html) / shm_unlink

shm_open, shm_unlink - create/open or unlink POSIX shared memory objects

``` c
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

int shm_open(const char *name, int oflag, mode_t mode);
int shm_unlink(const char *name);
```

`shm_open()` creates and opens a new, or opens an existing, **POSIX shared memory object**. A POSIX shared memory object is in effect a handle which can be used by unrelated processes to `mmap(2)` the same region of shared memory.  The `shm_unlink()` function performs the converse operation, removing an object previously created by `shm_open()`.

The operation of `shm_open()` is analogous to that of `open(2)`. `name` specifies the shared memory object to be created or opened. For portable use, a shared memory object should be identified by a name of the form `/somename`; that is, a null-terminated string of up to `NAME_MAX` (i.e., 255) characters consisting of an initial **slash**, followed by one or more characters, none of which are slashes.



## [pthread_mutex_init](https://man7.org/linux/man-pages/man3/pthread_mutex_init.3p.html)

pthread_mutex_init — destroy and initialize a mutex

``` c
#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```

## [pthread_condattr_init](https://man7.org/linux/man-pages/man3/pthread_condattr_init.3p.html)

pthread_condattr_init — initialize the condition variable attributes object

``` c
#include <pthread.h>

int pthread_condattr_init(pthread_condattr_t *attr);
```


## [pthread_condattr_setpshared](https://man7.org/linux/man-pages/man3/pthread_condattr_setpshared.3p.html)

pthread_condattr_setpshared — set the process-shared condition variable attribute

``` c
#include <pthread.h>

int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);
```

## [pthread_cond_init](https://linux.die.net/man/3/pthread_cond_init) / pthread_cond_destroy

pthread_cond_destroy, pthread_cond_init - destroy and initialize condition variables

``` c
#include <pthread.h>

int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr);
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```


## [pthread_mutexattr_init](https://man7.org/linux/man-pages/man3/pthread_mutexattr_init.3.html) / pthread_mutexattr_destroy

pthread_mutexattr_init, pthread_mutexattr_destroy - initialize and destroy a mutex attributes object

``` c
#include <pthread.h>

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
```

The `pthread_mutexattr_init()` function initializes the mutex attributes object pointed to by `attr` with default values for all attributes defined by the implementation.

The results of initializing an already initialized mutex attributes object are undefined.

The `pthread_mutexattr_destroy()` function destroys a mutex attribute object (making it uninitialized). Once a mutex attributes object has been destroyed, it can be reinitialized with `pthread_mutexattr_init()`.

The results of destroying an uninitialized mutex attributes object are undefined.




## pthread_mutexattr_getpshared / [pthread_mutexattr_setpshared](https://linux.die.net/man/3/pthread_mutexattr_setpshared)

``` c
#include <pthread.h>

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *restrict attr, int *restrict pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
```

The `pthread_mutexattr_getpshared()` function shall **obtain** the value of the process-shared attribute from the attributes object referenced by `attr`.

The `pthread_mutexattr_setpshared()` function shall **set** the process-shared attribute in an initialized attributes object referenced by `attr`.

The process-shared attribute is set to `PTHREAD_PROCESS_SHARED` to permit a mutex to be operated upon by any thread that has access to the memory where the mutex is allocated, even if the mutex is allocated in memory that is shared by multiple processes.

If the process-shared attribute is `PTHREAD_PROCESS_PRIVATE`, the mutex shall only be operated upon by threads created within the same process as the thread that initialized the mutex; if threads of differing processes attempt to operate on such a mutex, the behavior is undefined. The default value of the attribute shall be `PTHREAD_PROCESS_PRIVATE`.


## [pthread_cond_signal](https://linux.die.net/man/3/pthread_cond_signal)

pthread_cond_broadcast, pthread_cond_signal - broadcast or signal a condition

``` c
#include <pthread.h>

int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
```

## [pthread_cond_wait](https://linux.die.net/man/3/pthread_cond_wait) / [pthread_cond_timedwait](https://linux.die.net/man/3/pthread_cond_timedwait)

pthread_cond_timedwait, pthread_cond_wait - wait on a condition

``` c
#include <pthread.h>

int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime);
int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
```

The `pthread_cond_timedwait()` and `pthread_cond_wait()` functions shall block on a condition variable. They shall be called with `mutex` locked by the calling thread or undefined behavior results.

These functions **atomically** release `mutex` and cause the calling thread to block on the condition variable `cond`; **atomically** here means "atomically with respect to access by another thread to the mutex and then the condition variable". That is, if another thread is able to acquire the mutex after the about-to-block thread has released it, then a subsequent call to `pthread_cond_broadcast()` or `pthread_cond_signal()` in that thread shall behave as if it were issued after the about-to-block thread has blocked.

Upon successful return, the mutex shall have been locked and shall be owned by the calling thread.


1. 在调用 pthread_cond_timedwait() 或 pthread_cond_wait() 时，必须先锁定互斥锁，否则会导致未定义的行为。
2. 这两个函数会原子性地释放互斥锁并阻塞调用线程。原子性意味着相对于其他线程访问互斥锁和条件变量，这两个操作是连续的。
3. 函数成功返回时，互斥锁将被重新锁定，并由调用线程拥有。
4. 当使用条件变量时，与每个条件等待相关联的布尔谓词涉及到共享变量。这个谓词在线程应该继续执行时为 true。由于从 pthread_cond_timedwait() 或 pthread_cond_wait() 返回并不意味着谓词的值有任何含义，所以在返回时应重新评估谓词。
5. 在同一个条件变量上并发使用多个互斥锁进行 pthread_cond_timedwait() 或 pthread_cond_wait() 操作的效果是未定义的。
6. 条件等待（无论是否计时）是一个取消点。当线程的可取消性启用状态设置为 PTHREAD_CANCEL_DEFERRED 时，在条件等待中处理取消请求的副作用是在调用第一个取消清理处理程序之前重新获取互斥锁。
7. 在调用 pthread_cond_timedwait() 或 pthread_cond_wait() 被取消的线程不应消耗可能同时针对条件变量的任何条件信号，如果有其他线程阻塞在条件变量上。
8. pthread_cond_timedwait() 函数与 pthread_cond_wait() 函数的区别在于，如果在条件变量被发信号或广播之前超过了 abstime 指定的绝对时间，则返回错误。此外，pthread_cond_timedwait() 函数还是一个取消点。
9. 如果信号发送到等待条件变量的线程，那么在信号处理程序返回后，线程将恢复等待条件变量，就像它没有被中断一样，或者由于虚假唤醒而返回零。

这些解释有助于理解 pthread_cond_timedwait() 和 pthread_cond_wait() 函数的工作原理以及如何在多线程环境中使用它们进行同步。



# 测试代码

``` c
// kbhit.h

#pragma once

#include <termios.h>
#include <unistd.h>

int kbhit()
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt); // 获取终端属性
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // 修改终端属性，禁用规范模式和回显
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // 设置终端属性
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0); // 获取文件状态标志
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); // 设置文件状态标志为非阻塞模式

    ch = getchar(); // 尝试从标准输入中读取一个字符

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // 恢复原始终端属性
    fcntl(STDIN_FILENO, F_SETFL, oldf); // 恢复原始文件状态标志

    if(ch != EOF)
    {
        ungetc(ch, stdin); // 如果读取到字符，将其放回输入流
        return 1;
    }

    return 0;
}
```

`kbhit()` 函数的实现涉及以下步骤：

* 使用 `tcgetattr` 函数获取当前终端的属性。
* 修改终端属性，禁用规范模式（`ICANON`）和回显（`ECHO`）。这样，输入字符将立即可用，而不是等待换行符或回车符，且输入的字符不会在终端上显示。
* 使用 `tcsetattr` 函数设置修改后的终端属性。
* 使用 `fcntl` 函数获取标准输入的文件状态标志。
* 修改文件状态标志，将其设置为非阻塞模式（`O_NONBLOCK`）。这样，`getchar()` 函数将立即返回，而不是等待输入。
* 使用 `getchar()` 函数尝试从标准输入中读取一个字符。
* 恢复原始终端属性和文件状态标志。
* 如果 `getchar()` 函数读取到一个字符（即 `ch != EOF`），则使用 `ungetc()` 函数将该字符放回输入流，并返回 1。否则，返回 0。

这个 `kbhit()` 函数的实现可以在 Linux 系统上检测按键输入，而不会阻塞程序的执行。在主循环中使用此函数，可以实现在按下任意键时退出循环。


## 生产者

``` c
// producer.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "kbhit.h"

typedef struct share_data_s {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    volatile int val;
} share_data_t;

int main() {
    const char * shm_name = "my_shm_object";

    struct timeval tv;
    int shm_fd;
    int size;
    share_data_t * shm_data;
    pthread_mutexattr_t attrmutex;
    pthread_condattr_t attrcond;
    int cnt = 0;

    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR | O_TRUNC, 0777);
    if (shm_fd < 0) {
        printf("shm_open failed\n");
        return -1;
    }

    size = sizeof(share_data_t);
    if (ftruncate(shm_fd, size) == -1) {
        printf("ftruncate failed\n");
        return -1;
    }

    shm_data = (share_data_t * ) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_data == MAP_FAILED) {
        printf("mmap failed\n");
        return -1;
    }

    pthread_mutexattr_init( & attrmutex);
    pthread_mutexattr_setpshared( & attrmutex, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init( & shm_data -> mutex, & attrmutex);
    pthread_condattr_init( & attrcond);
    pthread_condattr_setpshared( & attrcond, PTHREAD_PROCESS_SHARED);
    pthread_cond_init( & shm_data -> cond, & attrcond);
    shm_data -> val = 0;

    for (;;) {
        pthread_mutex_lock( & shm_data -> mutex);
        if (shm_data -> val == 0) {
            shm_data -> val = cnt++;

            gettimeofday( & tv, NULL);
            printf("producer now(%ld) set share value(%d)\n", tv.tv_sec, shm_data -> val);

            pthread_cond_signal( & shm_data -> cond);
        }
        pthread_mutex_unlock( & shm_data -> mutex);

        if (kbhit()) {
            break;
        }
    }

    // clean up
    munmap(shm_data, size);
    close(shm_fd);
    shm_unlink(shm_name);

    printf("producer exit\n");
    return 0;
}
```

这段代码定义了一个名为 share_data_t 的结构体，该结构体包含一个互斥锁（pthread_mutex_t）、一个条件变量（pthread_cond_t）和一个易失性整数（int volatile val）。这个结构体将用于在进程之间共享数据。

在 main 函数中，首先创建了一个共享内存对象（shm_fd），并将其映射到进程的虚拟地址空间（mmap）。然后，为互斥锁和条件变量设置了进程共享属性，这样它们就可以在多个进程之间共享和同步。

接下来，程序进入一个无限循环，在循环中，程序首先锁定互斥锁，然后检查 shm_data->val 是否为 0。如果为 0，程序将更新 shm_data->val 的值，发送条件变量信号（pthread_cond_signal），然后解锁互斥锁。这个循环将一直持续下去，直到手动终止程序。

这段代码的主要目的是在多个进程之间共享数据，并使用互斥锁和条件变量进行同步。在这个例子中，程序扮演了生产者的角色，不断地生成新的值并将其存储在共享内存中。另一个进程（消费者）可以使用类似的方法访问共享内存，并在条件变量上等待生产者的信号。


## 消费者

``` c
// consumer.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "kbhit.h"

typedef struct share_data_s {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    volatile int val;
} share_data_t;

int main() {
    const char * shm_name = "my_shm_object";

    struct timeval tv;
    int shm_fd;
    share_data_t * shm_data;

    shm_fd = shm_open(shm_name, O_RDWR, 0777);
    if (shm_fd < 0) {
        printf("shm_open  failed\n");
        return -1;
    }

    shm_data = (share_data_t * ) mmap(NULL, sizeof(share_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_data == MAP_FAILED) {
        printf("mmap info failed\n");
        return -1;
    }

    for (;;) {
        pthread_mutex_lock( & shm_data -> mutex);

        gettimeofday( & tv, NULL);
        printf("consumer now(%ld) get lock\n", tv.tv_sec);

        // wait producer to change shm_data->cond to non-zero, if not will block here
        while (0 == shm_data -> val) {
#if 0
            pthread_cond_wait( & shm_data -> cond, & shm_data -> mutex);
#else
            struct timespec timeout;
            clock_gettime(CLOCK_REALTIME, & timeout);
            timeout.tv_nsec += 100000000; // Add 100ms to the current time
            if (timeout.tv_nsec >= 1000000000) {
                timeout.tv_nsec -= 1000000000;
                timeout.tv_sec++;
            }
            int ret = pthread_cond_timedwait( & shm_data -> cond, & shm_data -> mutex, & timeout);
            if (ret == ETIMEDOUT) {
                printf("timeout\n");
                break;
            }
#endif
        }

        shm_data -> val = 0;
        printf("consumer now(%ld) set share value(%d)\n", tv.tv_sec, shm_data -> val);

        pthread_mutex_unlock( & shm_data -> mutex);
        printf("consumer out lock\n");

        if (kbhit()) {
            break;
        }
    }

    // clean up
    munmap(shm_data, sizeof(share_data_t));
    close(shm_fd);

    printf("consumer exit\n");
    return 0;
}
```

这段代码是一个消费者程序，它与生产者程序一起工作。消费者程序使用共享内存来访问生产者程序中的 share_data_t 结构体。结构体包含一个互斥锁，一个条件变量和一个易失性整数值。生产者和消费者之间通过这个结构体进行数据共享和同步。

消费者程序的主要步骤如下：

* 打开已经由生产者创建的共享内存对象（shm_open）。共享内存对象的名称为 "my_shm_object"。
* 使用 mmap 将共享内存映射到消费者进程的虚拟地址空间。
* 在无限循环中进行以下操作：
  + 锁定互斥锁以确保对共享数据的互斥访问。
  + 获取当前时间，并打印消费者获得锁的时间。
  + 使用 pthread_cond_wait 函数在条件变量上等待，直到 shm_data->val 不为 0。这意味着生产者已经生成了一个新值并发出了条件变量信号。
  + 将 shm_data->val 重置为 0，表示消费者已经获取了生产者生成的值。
  + 解锁互斥锁。
  + 打印消费者退出锁的消息。
* 由于这是一个无限循环，程序将一直运行，直到手动终止。要正确清理资源，您可以添加一个条件来中断循环，然后执行清理操作，如取消映射共享内存（munmap），关闭共享内存文件描述符（close）。

这个消费者程序与生产者程序一起实现了一个简单的生产者-消费者模型。生产者生成数据并将其存储在共享内存中，同时发出条件变量信号。消费者等待条件变量信号，然后获取并处理共享内存中的数据。通过互斥锁和条件变量，生产者和消费者之间实现了同步和互斥访问共享数据。


## 编译

Makefile 内容如下：

```
CXXFLAGS = -Wall -g -pipe -O2
CFLAGS = -Wall -g -pipe -O2

INCLUDE = -I.
LIBPATH = -L.
LIBS = -lpthread -lrt

PRODUCER = producer
PRODUCER_OBJS = producer.o

CONSUMER = consumer
CONSUMER_OBJS = consumer.o

.PHONY: clean

all: $(PRODUCER) $(CONSUMER)

$(PRODUCER): $(PRODUCER_OBJS)
        $(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS)
        @echo "build $(PRODUCER) ok"

$(CONSUMER): $(CONSUMER_OBJS)
        $(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS)
        @echo "build $(CONSUMER) ok"

clean:
        rm -f $(PRODUCER) $(CONSUMER) *.o

%.o: %.cpp
        $(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
        $(CC) $(CFLAGS) $(INCLUDE) -c $<
```


# Refer

* https://docs.oracle.com/cd/E19253-01/819-7051/index.html













