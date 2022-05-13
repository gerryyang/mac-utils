---
layout: post
title:  "C-ares in Action"
date:   2022-05-12 11:30:00 +0800
categories: [TCP/IP]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Question

The `getaddrinfo` function call alone causes over 100 system calls! But getaddrinfo does a lot before these system calls, and it does quite a bit after them, too. See more: [What does getaddrinfo do?](https://jameshfisher.com/2018/02/03/what-does-getaddrinfo-do/)

``` cpp
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(void)
{
  struct addrinfo* addr;
  int result = getaddrinfo("google.com", NULL, NULL, &addr);
  if (result != 0) {
    printf("Error from getaddrinfo: %s\n", gai_strerror(result));
    return 1;
  }
  struct sockaddr_in* internet_addr = (struct sockaddr_in*) addr->ai_addr;
  printf("google.com is at: %s\n", inet_ntoa(internet_addr->sin_addr));
  return 0;
}
```

```
$./a.out 
google.com is at: 172.217.163.46
```

# C-ares

> This is c-ares, an asynchronous resolver library. It is intended for applications which need to perform DNS queries without blocking, or need to perform multiple DNS queries in parallel. 

`c-ares` is a C library for asynchronous DNS requests (including name resolves). C89 compatibility, [MIT licensed](https://c-ares.org/license.html), builds for and runs on POSIX, Windows, Netware, Android and many more operating systems.


# CMake builds

Current releases of c-ares introduce a CMake v3+ build system that has been tested on most platforms including Windows, Linux, FreeBSD, MacOS, AIX and Solaris.

In the most basic form, building with CMake might look like:

```
cd ~/tools/c-ares
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/tools/c-ares/c-ares-1.18.1-install ../c-ares-1.18.1
make
make install
```

Options to CMake are passed on the command line using "-D${OPTION}=${VALUE}". The values defined are all boolean and take values like On, Off, True, False.

* CARES_STATIC - Build the static library (off by default)
* CARES_SHARED - Build the shared library (on by default)
* CARES_INSTALL - Hook in installation, useful to disable if chain building
* CARES_STATIC_PIC - Build the static library as position-independent (off by default)

# Example

## ares_gethostbyaddr

``` cpp
#include <time.h>
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <ares.h>

void dns_callback (void* arg, int status, int timeouts, struct hostent* host)
{
        if (status == ARES_SUCCESS)
            std::cout << host->h_name << "\n";
        else
            std::cout << "lookup failed: " << status << '\n';
}
void main_loop(ares_channel &channel)
{
        int nfds, count;
        fd_set readers, writers;
        timeval tv, *tvp;
        while (1) {
                FD_ZERO(&readers);
                FD_ZERO(&writers);
                nfds = ares_fds(channel, &readers, &writers);
                if (nfds == 0) {
                    break;
                ｝
                tvp = ares_timeout(channel, NULL, &tv);
                count = select(nfds, &readers, &writers, NULL, tvp);
                ares_process(channel, &readers, &writers);
        }

}
int main(int argc, char **argv)
{
        struct in_addr ip;
        int res;
        if(argc < 2 ) {
                std::cout << "usage: " << argv[0] << " ip.address\n";
                return 1;
        }
        inet_aton(argv[1], &ip);
        ares_channel channel;
        if((res = ares_init(&channel)) != ARES_SUCCESS) {
                std::cout << "ares feiled: " << res << '\n';
                return 1;
        }
        ares_gethostbyaddr(channel, &ip, sizeof ip, AF_INET, dns_callback, NULL);
        main_loop(channel);
        return 0;
}
```

g++ demo.cc -I deps/c-ares-1.18.1/include deps/c-ares-1.18.1/lib/libcares.a

```
$./a.out 8.8.8.8
dns.google
```

## ares_gethostbyname

``` cpp
#include <time.h>
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <ares.h>
#include <cstdio>

void dns_callback (void* arg, int status, int timeouts, struct hostent* host)
{
        if (status == ARES_SUCCESS) {
            puts(host->h_name);
            printf("%u.%u.%u.%u\n", host->h_addr_list[0][0],
                            host->h_addr_list[0][1],
                            host->h_addr_list[0][2],
                            host->h_addr_list[0][3]);
        }
        else
            std::cout << "lookup failed: " << status << '\n';
}

void main_loop(ares_channel &channel)
{
        int nfds, count;
        fd_set readers, writers;
        timeval tv, *tvp;
        while (1) {
                FD_ZERO(&readers);
                FD_ZERO(&writers);
                nfds = ares_fds(channel, &readers, &writers);
                if (nfds == 0) {
                    break;
                ｝
                tvp = ares_timeout(channel, NULL, &tv);
                count = select(nfds, &readers, &writers, NULL, tvp);
                ares_process(channel, &readers, &writers);
        }

}

int main(int argc, char **argv)
{
        int res;
        if(argc < 2 ) {
                std::cout << "usage: " << argv[0] << " domain_name\n";
                return 1;
        }

        ares_channel channel;
        if((res = ares_init(&channel)) != ARES_SUCCESS) {
                std::cout << "ares feiled: " << res << '\n';
                return 1;
        }
        ares_gethostbyname(channel, argv[1], AF_INET, dns_callback, NULL);
        main_loop(channel);
        return 0;
}
```

g++ ares_gethostbyname.cc -o ares_gethostbyname -I deps/c-ares-1.18.1/include deps/c-ares-1.18.1/lib/libcares.a

```
$./ares_gethostbyname google.cn
google.cn
58.63.4294967273.98
$./ares_gethostbyname baidu.com
baidu.com
4294967260.4294967221.38.4294967291
s$./ares_gethostbyname gerryyang.com
gerryyang.com
119.28.41.102
```

## ares_process_fd + poll

``` cpp
#include <sys/time.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <ares.h>
#include <pthread.h>
#include <deque>
#include <netdb.h>
#include "ares.h"
#include <string>
#include <cstring>
#include <cstdio>

class dns_resolver_t
{
public:
    dns_resolver_t() : channel_(NULL)
    {
        int ret = ares_init(&channel_);
        if (ret != ARES_SUCCESS)
        {
            err_info_ = ares_strerror(ret);
        }
    }

    ~dns_resolver_t()
    {
        if (channel_)
        {
            ares_destroy(channel_);
        }
    }

    int resolve(int af, const std::string& domain, int& timeout, void* addr, size_t addr_len)
    {
        dns_res_t res = {NULL, addr, addr_len};
        ares_gethostbyname(channel_, domain.c_str(), af, dns_callback, &res);

        struct timeval last, now;
        gettimeofday(&last, NULL);
        int nfds = 1;
        while (nfds)
        {
            struct timeval *tvp, tv, store = {timeout / 1000, (timeout % 1000) * 1000};
            tvp = ares_timeout(channel_, &store, &tv);
            int timeout_ms = tvp->tv_sec * 1000 + tvp->tv_usec / 1000;

            nfds = dns_wait_resolve(channel_, timeout_ms);
            gettimeofday(&now, NULL);
            timeout -= (now.tv_sec - last.tv_sec) * 1000 + (now.tv_usec - last.tv_usec) / 1000;
            last = now;
        }

        if (res.error_info)
        {
            err_info_ = res.error_info;
            return -1;
        }
        return 0;
    }

    operator bool() const
    {
        return channel_;
    }

    const std::string& error_info() const
    {
        return err_info_;
    }

private:
    dns_resolver_t(const dns_resolver_t&);
    dns_resolver_t& operator=(const dns_resolver_t&);

    struct dns_res_t
    {
        const char* error_info;
        void* address;
        size_t len;
    };

    static void dns_callback(void* arg, int status, int timeouts, struct hostent* hptr)
    {
        dns_res_t& res = *(dns_res_t*)arg;
        if (status != ARES_SUCCESS)
        {
            res.error_info = ares_strerror(status);
            return;
        }
        char** pptr = hptr->h_addr_list;
        if (*pptr)
        {
            memcpy(res.address, *pptr, res.len);
            return;
        }

        res.error_info = "no invalid address get";
    }

    static int dns_wait_resolve(ares_channel channel_, int timeout_ms)
    {
        if (timeout_ms < 0)
        {
            ares_process_fd(channel_, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
            return 0;
        }
        int nfds;
        int bitmask;
        ares_socket_t socks[ARES_GETSOCK_MAXNUM];
        struct pollfd pfd[ARES_GETSOCK_MAXNUM];
        int i;
        int num = 0;

        bitmask = ares_getsock(channel_, socks, ARES_GETSOCK_MAXNUM);

        for (i = 0; i < ARES_GETSOCK_MAXNUM; i++)
        {
            pfd[i].events = 0;
            pfd[i].revents = 0;
            if (ARES_GETSOCK_READABLE(bitmask, i))
            {
                pfd[i].fd = socks[i];
                pfd[i].events |= POLLRDNORM | POLLIN;
            }
            if (ARES_GETSOCK_WRITABLE(bitmask, i))
            {
                pfd[i].fd = socks[i];
                pfd[i].events |= POLLWRNORM | POLLOUT;
            }
            if (pfd[i].events != 0)
            {
                num++;
            }
            else
            {
                break;
            }
        }

        if (num)
        {
            nfds = poll(pfd, num, timeout_ms);
        }
        else
        {
            nfds = 0;
        }

        if (!nfds)
        {
            ares_process_fd(channel_, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
        }
        else
        {
            for (i = 0; i < num; i++)
            {
                ares_process_fd(channel_,
                                (pfd[i].revents & (POLLRDNORM | POLLIN)) ? pfd[i].fd : ARES_SOCKET_BAD,
                                (pfd[i].revents & (POLLWRNORM | POLLOUT)) ? pfd[i].fd : ARES_SOCKET_BAD);
            }
        }
        return nfds;
    }

    ares_channel channel_;
    std::string err_info_;
};

int main()
{
    struct sockaddr_in sa = {};
    std::string domain = "gerryyang.com";
    int timeout_ms = 1000;
    dns_resolver_t dr;
    if (dr)
    {
        dr.resolve(AF_INET, domain, timeout_ms, &sa.sin_addr.s_addr, sizeof(sa.sin_addr.s_addr));
        char strIP[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &(sa.sin_addr), strIP, INET_ADDRSTRLEN);
        printf("%s\n", strIP);
    }
    else
    {
        printf("dns_resolver_t init err(%s)\n", dr.error_info().c_str());
    }
}
```

g++ ares_gethostbyname2.cc -o ares_gethostbyname2 -I deps/c-ares-1.18.1/include deps/c-ares-1.18.1/lib/libcares.a

``` bash
$./ares_gethostbyname2
119.28.41.102
```


# Interface

More: [c-ares documentation](https://c-ares.org/docs.html) 


## ares_init

``` cpp
#include <ares.h>
 
int ares_init(ares_channel *channelptr)
```

The `ares_init` function initializes a communications channel for name service lookups. If it returns successfully, `ares_init` will set the variable pointed to by `channelptr` to a handle used to identify the name service channel. The caller should invoke `ares_destroy` on the handle when the channel is no longer needed.

The `ares_init_options` function is provide to offer more init alternatives.

> NOTES

When initializing from `/etc/resolv.conf`, `ares_init(3)` reads the domain and search directives to allow lookups of short names relative to the domains specified. The domain and search directives override one another. If more that one instance of either domain or search directives is specified, the last occurrence wins. For more information, please see the `resolv.conf(5)` manual page.


* https://c-ares.org/ares_init.html

## ares_gethostbyaddr

``` cpp
#include <ares.h>

typedef void (*ares_host_callback)(void *arg, int status, int timeouts, struct hostent *hostent)
void ares_gethostbyaddr(ares_channel channel, const void *addr, int addrlen, int family, ares_host_callback callback, void *arg)
```

The `ares_gethostbyaddr` function initiates a host query by address on the name service channel identified by channel. The parameters addr and addrlen give the address as a series of bytes, and family gives the type of address. When the query is complete or has failed, the ares library will invoke callback. Completion or failure of the query may happen immediately, or may happen during a later call to ares_process(3), ares_destroy(3) or ares_cancel(3).

The callback argument `arg` is copied from the `ares_gethostbyaddr` argument `arg`. The callback argument `status` indicates whether the query succeeded and, if not, how it failed. 

The callback argument `timeouts` reports how many times a query timed out during the execution of the given request.

On successful completion of the query, the callback argument hostent points to a `struct hostent` containing the name of the host returned by the query. The callback need not and should not attempt to free the memory pointed to by hostent; the ares library will free it when the callback returns. If the query did not complete successfully, hostent will be NULL.


* https://linux.die.net/man/3/ares_gethostbyaddr

## ares_gethostbyname

``` cpp
#include <ares.h>

typedef void (*ares_host_callback)(void *arg, int status, int timeouts, struct hostent *hostent)
void ares_gethostbyname(ares_channel channel, const char *name, int family, ares_host_callback callback, void *arg)
```

The `ares_gethostbyname` function initiates a host query by name on the name service channel identified by channel. The parameter `name` gives the hostname as a NUL-terminated C string, and family gives the desired type of address for the resulting host entry. When the query is complete or has failed, the ares library will invoke callback. Completion or failure of the query may happen immediately, or may happen during a later call to ares_process(3), ares_destroy(3) or ares_cancel(3).

The callback argument `arg` is copied from the `ares_gethostbyname` argument `arg`. The callback argument `status` indicates whether the query succeeded and, if not, how it failed.

The callback argument `timeouts` reports how many times a query timed out during the execution of the given request.

On successful completion of the query, the callback argument hostent points to a `struct hostent` containing the name of the host returned by the query. The callback need not and should not attempt to free the memory pointed to by hostent; the ares library will free it when the callback returns. If the query did not complete successfully, hostent will be NULL.


* https://linux.die.net/man/3/ares_gethostbyname

## ares_process

``` cpp
#include <ares.h>

void ares_process(ares_channel channel, fd_set *read_fds, fd_set *write_fds)
void ares_process_fd(ares_channel channel, ares_socket_t read_fd, ares_socket_t write_fd)
```

The `ares_process(3)` function handles input/output events and timeouts associated with queries pending on the name service channel identified by channel. The file descriptor sets pointed to by `read_fds` and `write_fds` should have file descriptors set in them according to whether the file descriptors specified by `ares_fds(3)` are ready for reading and writing. (The easiest way to determine this information is to invoke `select` with a timeout no greater than the timeout given by `ares_timeout(3)` ).

The `ares_process` function will invoke **callbacks** for pending queries if they complete successfully or fail.

`ares_process_fd(3)` works the same way but acts and operates only on the specific file descriptors (sockets) you pass in to the function. Use `ARES_SOCKET_BAD` for "no action". This function is of course provided to allow users of c-ares to void select() in their applications and within c-ares.

The following code fragment waits for all pending queries on a `channel` to complete:

``` cpp
int nfds, count;
fd_set readers, writers;
struct timeval tv, *tvp;

while (1)
{
    FD_ZERO(&readers);
    FD_ZERO(&writers);

    nfds = ares_fds(channel, &readers, &writers);
    if (nfds == 0)
    {
        break;
    }
        
    tvp = ares_timeout(channel, NULL, &tv);
    count = select(nfds, &readers, &writers, NULL, tvp);
    ares_process(channel, &readers, &writers);
}
```

* https://linux.die.net/man/3/ares_process

## ares_fds

``` cpp
#include <ares.h>
int ares_fds(ares_channel channel, fd_set *read_fds, fd_set *write_fds)
```

The `ares_fds` function retrieves the set of file descriptors which the calling application should select on for reading and writing for the processing of name service queries pending on the name service channel identified by channel. File descriptors will be set in the file descriptor sets pointed to by `read_fds` and `write_fds` as appropriate. File descriptors already set in `read_fds` and `write_fds` will remain set; initialization of the file descriptor sets (using `FD_ZERO`) is the responsibility of the caller.

`ares_fds` returns one greater than the number of the highest socket set in either read_fds or write_fds. If no queries are active, ares_fds will return 0.

* https://linux.die.net/man/3/ares_fds

## ares_timeout

``` cpp
#include <ares.h>

struct timeval *ares_timeout(ares_channel channel, struct timeval *maxtv, struct timeval *tv)
```

The `ares_timeout` function determines the maximum time for which the caller should wait before invoking `ares_process(3)` to process `timeouts`. The parameter `maxtv` specifies a existing maximum timeout, or NULL if the caller does not wish to apply a maximum timeout. The parameter `tv` must point to a writable buffer of type `struct timeval`. It is valid for `maxtv` and `tv` to have the same value.

If no queries have timeouts pending sooner than the given maximum timeout, `ares_timeout` returns the value of `maxtv`; otherwise `ares_timeout` stores the appropriate timeout value into the buffer pointed to by `tv` and returns the value of `tv`.

* https://linux.die.net/man/3/ares_timeout


## Performance compare

编译：g++ performance_compare.cc -o performance_compare -I deps/c-ares-1.18.1/include deps/c-ares-1.18.1/lib/libcares.a -std=c++11

Loop: 10000

| Resolve | Time (ns) | Speedup
| -- | -- | --
| getaddrinfo | 10606284252 | 1x
| c-ares | 9253562550 | 1.145x

Loop: 100000

| Resolve | Time (ns) | Speedup
| -- | -- | --
| getaddrinfo | 105582573144 | 1x
| c-ares | 93443656627 | 1.13x

加上 -O2 编译优化：

Loop: 10000

| Resolve | Time (ns) | Speedup
| -- | -- | --
| getaddrinfo | 10878913658 | 1x
| c-ares | 9644415228 | 1.128x

Loop: 100000

| Resolve | Time (ns) | Speedup
| -- | -- | --
| getaddrinfo | 105851071727 | 1x
| c-ares | 93014894039 | 1.138x



测试代码：

``` cpp
#include <sys/time.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <ares.h>
#include <pthread.h>
#include <deque>
#include <netdb.h>
#include "ares.h"
#include <string>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <chrono>

const int MAXCNT = 10000;

class ScopedTimer 
{
	public:
		ScopedTimer(const char* name): m_name(name), m_beg(std::chrono::high_resolution_clock::now()) { }
		~ScopedTimer() {
			auto end = std::chrono::high_resolution_clock::now();
			auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_beg);
			std::cout << m_name << " : " << dur.count() << " ns\n";
		}
	private:
		const char* m_name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_beg;
};

class dns_resolver_t
{
public:
    dns_resolver_t() : channel_(NULL)
    {
        int ret = ares_init(&channel_);
        if (ret != ARES_SUCCESS)
        {
            err_info_ = ares_strerror(ret);
        }
    }

    ~dns_resolver_t()
    {
        if (channel_)
        {
            ares_destroy(channel_);
        }
    }

    int resolve(int af, const std::string& domain, int& timeout, void* addr, size_t addr_len)
    {
        dns_res_t res = {NULL, addr, addr_len};
        ares_gethostbyname(channel_, domain.c_str(), af, dns_callback, &res);

        struct timeval last, now;
        gettimeofday(&last, NULL);
        int nfds = 1;
        while (nfds)
        {
            struct timeval *tvp, tv, store = {timeout / 1000, (timeout % 1000) * 1000};
            tvp = ares_timeout(channel_, &store, &tv);
            int timeout_ms = tvp->tv_sec * 1000 + tvp->tv_usec / 1000;

            nfds = dns_wait_resolve(channel_, timeout_ms);
            gettimeofday(&now, NULL);
            timeout -= (now.tv_sec - last.tv_sec) * 1000 + (now.tv_usec - last.tv_usec) / 1000;
            last = now;
        }

        if (res.error_info)
        {
            err_info_ = res.error_info;
            return -1;
        }
        return 0;
    }

    operator bool() const
    {
        return channel_;
    }

    const std::string& error_info() const
    {
        return err_info_;
    }

private:
    dns_resolver_t(const dns_resolver_t&);
    dns_resolver_t& operator=(const dns_resolver_t&);

    struct dns_res_t
    {
        const char* error_info;
        void* address;
        size_t len;
    };

    static void dns_callback(void* arg, int status, int timeouts, struct hostent* hptr)
    {
        dns_res_t& res = *(dns_res_t*)arg;
        if (status != ARES_SUCCESS)
        {
            res.error_info = ares_strerror(status);
            return;
        }
        char** pptr = hptr->h_addr_list;
        if (*pptr)
        {
            memcpy(res.address, *pptr, res.len);
            return;
        }

        res.error_info = "no invalid address get";
    }

    static int dns_wait_resolve(ares_channel channel_, int timeout_ms)
    {
        if (timeout_ms < 0)
        {
            ares_process_fd(channel_, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
            return 0;
        }
        int nfds;
        int bitmask;
        ares_socket_t socks[ARES_GETSOCK_MAXNUM];
        struct pollfd pfd[ARES_GETSOCK_MAXNUM];
        int i;
        int num = 0;

        bitmask = ares_getsock(channel_, socks, ARES_GETSOCK_MAXNUM);

        for (i = 0; i < ARES_GETSOCK_MAXNUM; i++)
        {
            pfd[i].events = 0;
            pfd[i].revents = 0;
            if (ARES_GETSOCK_READABLE(bitmask, i))
            {
                pfd[i].fd = socks[i];
                pfd[i].events |= POLLRDNORM | POLLIN;
            }
            if (ARES_GETSOCK_WRITABLE(bitmask, i))
            {
                pfd[i].fd = socks[i];
                pfd[i].events |= POLLWRNORM | POLLOUT;
            }
            if (pfd[i].events != 0)
            {
                num++;
            }
            else
            {
                break;
            }
        }

        if (num)
        {
            nfds = poll(pfd, num, timeout_ms);
        }
        else
        {
            nfds = 0;
        }

        if (!nfds)
        {
            ares_process_fd(channel_, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
        }
        else
        {
            for (i = 0; i < num; i++)
            {
                ares_process_fd(channel_,
                                (pfd[i].revents & (POLLRDNORM | POLLIN)) ? pfd[i].fd : ARES_SOCKET_BAD,
                                (pfd[i].revents & (POLLWRNORM | POLLOUT)) ? pfd[i].fd : ARES_SOCKET_BAD);
            }
        }
        return nfds;
    }

    ares_channel channel_;
    std::string err_info_;
};

void resolve1()
{
	ScopedTimer timer("resolve1");

	char strIP[INET_ADDRSTRLEN] = {0};
	for (auto i = 0; i < MAXCNT; ++i) {
		struct sockaddr_in sa = {};
		std::string domain = "gerryyang.com";
		int timeout_ms = 1000;
		dns_resolver_t dr;
		if (dr)
		{
			dr.resolve(AF_INET, domain, timeout_ms, &sa.sin_addr.s_addr, sizeof(sa.sin_addr.s_addr));	
			inet_ntop(AF_INET, &(sa.sin_addr), strIP, INET_ADDRSTRLEN);
			//printf("%s\n", strIP);
		}
		else
		{
			printf("dns_resolver_t init err(%s)\n", dr.error_info().c_str());		
		}
	}

}

void resolve2()
{
	ScopedTimer timer("resolve2");

	char strIP[INET_ADDRSTRLEN] = {0};
	for (auto i = 0; i < MAXCNT; ++i) {
		struct addrinfo* addr;
		int result = getaddrinfo("gerryyang.com", NULL, NULL, &addr);
		if (result != 0) {
			printf("Error from getaddrinfo: %s\n", gai_strerror(result));
			return;
		}
		struct sockaddr_in* psa = (struct sockaddr_in*) addr->ai_addr;
		inet_ntop(AF_INET, &(psa->sin_addr), strIP, INET_ADDRSTRLEN);
		//printf("%s\n", strIP);
	}
}

int main()
{
	resolve1();
	resolve2();
}
```


# Refer

* https://c-ares.org/
* https://github.com/c-ares/c-ares
* https://github.com/c-ares/c-ares/blob/main/INSTALL.md
* [c-ares documentation](https://c-ares.org/docs.html)
* [How do I resolve an IP into host using c-ares?](https://stackoverflow.com/questions/4854284/how-do-i-resolve-an-ip-into-host-using-c-ares)
* [c-ares dns 异步请求库简明教程](https://lrita.github.io/2017/05/01/c-ares/#ares_library_init_mem)



