---
layout: post
title:  "C-ares in Action"
date:   2022-05-12 11:30:00 +0800
categories: [TCP/IP]
---

* Do not remove this line (it will not be displayed)
{:toc}

# getaddrinfo

In C programming, the functions `getaddrinfo()` and `getnameinfo()` convert **domain names**, **hostnames**, and **IP addresses** between human-readable text representations and structured binary formats for the operating system's networking API. Both functions are contained in the POSIX standard application programming interface (API).

`getaddrinfo` and `getnameinfo` are inverse functions of each other. They are network protocol agnostic, and support both **IPv4** and **IPv6**. It is the recommended interface for name resolution in building protocol independent applications and for transitioning legacy **IPv4** code to the **IPv6** Internet.

Internally, the functions perform resolutions using the [Domain Name System](https://en.wikipedia.org/wiki/Domain_Name_System) (**DNS**) by calling other, lower level functions, such as `gethostbyname()`.

``` cpp
int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);

void freeaddrinfo(struct addrinfo *res);

const char *gai_strerror(int errcode);
```

The addrinfo structure used by `getaddrinfo()` contains the following fields:

``` cpp
struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    socklen_t        ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};
```

The `getaddrinfo()` function allocates and initializes a linked list of addrinfo structures, one for each network address that matches node and service, subject to any restrictions imposed by `hints`, and returns a pointer to the start of the list in `res`. The items in the linked list are linked by the `ai_next` field.

**There are several reasons why the linked list may have more than one addrinfo structure**, including: the network host is multihomed, accessible over multiple protocols (e.g., both **AF_INET** and **AF_INET6**); or the same service is available from multiple socket types (one **SOCK_STREAM** address and another **SOCK_DGRAM** address, for example). Normally, the application should try using the addresses in the order in which they are returned. The sorting function used within `getaddrinfo()` is defined in RFC 3484; the order can be tweaked for a particular system by editing `/etc/gai.conf` (available since glibc 2.5).



* [getaddrinfo wiki](https://en.wikipedia.org/wiki/Getaddrinfo)

## getaddrinfo overhead

The `getaddrinfo` function call alone causes over 100 system calls! But getaddrinfo does a lot before these system calls, and it does quite a bit after them, too. See more: [What does getaddrinfo do?](https://jameshfisher.com/2018/02/03/what-does-getaddrinfo-do/)

`getaddrinfo` doesn’t know anything about files, DNS, or any other way to find the address for a host. Instead, `getaddrinfo` gets a list of these “sources” at runtime from another file, `/etc/nsswitch.conf`, the “Name Service Switch”. 

```
$cat /etc/nsswitch.conf
#
# /etc/nsswitch.conf
#
# An example Name Service Switch config file. This file should be
# sorted with the most-used services at the beginning.
#
# The entry '[NOTFOUND=return]' means that the search for an
# entry should stop if the search in the previous entry turned
# up nothing. Note that if the search failed due to some other reason
# (like no NIS server responding) then the search continues with the
# next entry.
#
# Valid entries include:
#
#       nisplus                 Use NIS+ (NIS version 3)
#       nis                     Use NIS (NIS version 2), also called YP
#       dns                     Use DNS (Domain Name Service)
#       files                   Use the local files
#       db                      Use the local database (.db) files
#       compat                  Use NIS on compat mode
#       hesiod                  Use Hesiod for user lookups
#       [NOTFOUND=return]       Stop searching if not found so far
#

# To use db, put the "db" in front of "files" for entries you want to be
# looked up first in the databases
#
# Example:
#passwd:    db files nisplus nis
#shadow:    db files nisplus nis
#group:     db files nisplus nis

#initgroups: files sss

#hosts:     db files nisplus nis dns
hosts:      files dns

...
```

Notice the line `hosts`: files dns. This says, “to find a host, first ask the library `libnss_files.so`. If that fails, ask the library `libnss_dns.so`.” The C standard library interpolates files, dns and so on into the pattern **libnss_%s.so** to find the libraries. As such, you could write a new library `libnss_imfeelinglucky`, and change your `nsswitch.conf` to hosts: `imfeelinglucky`. Enjoy the chaos.

You might think we’re done. Not yet! Before getaddrinfo does any of this, we have these system calls:


```
socket(PF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0) = 3
connect(3, {sa_family=AF_LOCAL, sun_path="/var/run/nscd/socket"}, 110) = -1 ENOENT (No such file or directory)
close(3)                                = 0
```

What is `/var/run/nscd/socket` ..? Linux tells us, with `ENOENT`, that I don’t have that file! What is this supposed to be? As Google will tell you, this is a socket to talk to the **Northern School of Contemporary Dance**. But before your process can go to class, you have to install the daemon:

```
$ sudo apt-get install nscd
...
Setting up nscd (2.19-0ubuntu6.14) ...
 * Starting Name Service Cache Daemon
```

Sorry, `nscd` is actually the “**name service cache daemon**”, “a daemon that provides a cache for the most common name service requests”. After installing it, the daemon starts, and your process can dance:

As everyone on the internet will tell you, `nscd` is “shit”, “unstable”, and “badly designed”. But no worries: as part of the C standard library, `nscd` is not this program but a standard protocol, part of the standard UNIX specification, with many alternative implementations!


One last thing. You might think that `getaddrinfo` caches answers, so subsequent calls aren’t so expensive. It does not! It does this entire procedure every time!

## Example


``` cpp
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(void)
{
  char strIP[INET_ADDRSTRLEN] = {0};

  struct addrinfo* addr;
  struct addrinfo* res;

  int result = getaddrinfo("google.com", NULL, NULL, &addr);
  if (result != 0) {
    printf("Error from getaddrinfo: %s\n", gai_strerror(result));
    return 1;
  }

  //struct sockaddr_in* internet_addr = (struct sockaddr_in*) addr->ai_addr;
  //printf("google.com is at: %s\n", inet_ntoa(internet_addr->sin_addr));

  for (res = addr; res != NULL; res = res->ai_next) { 
    struct sockaddr_in* psa = (struct sockaddr_in*) res->ai_addr;
    inet_ntop(AF_INET, &(psa->sin_addr), strIP, INET_ADDRSTRLEN);
    printf("%s\n", strIP);
  }

  freeaddrinfo(addr); 
  return 0;
}
```

```
$./a.out 
142.251.42.238
142.251.42.238
142.251.42.238
```

``` cpp
int SelectIP(const std::string& strHostName, std::string& strIP)
{
    char szIP[INET_ADDRSTRLEN] = {0};

    // block operation
    struct addrinfo* addr;
    struct addrinfo* res;
    int result = getaddrinfo(strHostName.c_str(), nullptr, nullptr, &addr);
    if (0 != result)
    {
        LOG_ERROR("getaddrinfo host(%s) err(%s)\n", strHostName.c_str(), gai_strerror(result));
        return 1;
    }
    
    for (res = addr; res != NULL; res = res->ai_next)
    {
        struct sockaddr_in* psa = (struct sockaddr_in*)res->ai_addr;
        inet_ntop(AF_INET, &(psa->sin_addr), szIP, INET_ADDRSTRLEN);
        strIP = szIP;
        break; // get first IP address
    }

    freeaddrinfo(addr);
    return 0;
}
```

``` cpp
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifndef   NI_MAXHOST
#define   NI_MAXHOST 1025
#endif

int main(void)
{
    struct addrinfo* result;
    struct addrinfo* res;
    int error;

    /* resolve the domain name into a list of addresses */
    error = getaddrinfo("www.example.com", NULL, NULL, &result);
    if (error != 0) {   
        if (error == EAI_SYSTEM) {
            perror("getaddrinfo");
        } else {
            fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
        }   
        exit(EXIT_FAILURE);
    }   

    /* loop over all returned results and do inverse lookup */
    for (res = result; res != NULL; res = res->ai_next) {   
        char hostname[NI_MAXHOST];
        error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
        if (error != 0) {
            fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(error));
            continue;
        }
        if (*hostname != '\0')
            printf("hostname: %s\n", hostname);
    }   

    freeaddrinfo(result);
    return 0;
}
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

    int resolve(int af, const std::string& domain, int& timeout/*ms*/, void* addr, size_t addr_len)
    {
        dns_res_t res = {NULL, addr, addr_len};
        ares_gethostbyname(channel_, domain.c_str(), af, dns_callback, &res);

        struct timeval last, now;
        gettimeofday(&last, NULL);
        int nfds = 1;
        while (nfds)
        {
            struct timeval *tvp, tv, store = {timeout / 1000, (timeout % 1000) * 1000};

            // return maximum time to wait
            tvp = ares_timeout(channel_, &store, &tv);
            int timeout_ms = tvp->tv_sec * 1000 + tvp->tv_usec / 1000;

            printf("timeout_ms(%d)\n", timeout_ms);
            nfds = dns_wait_resolve(channel_, timeout_ms);
            printf("dns_wait_resolve nfds(%d)\n", nfds);

            gettimeofday(&now, NULL);
            timeout -= (now.tv_sec - last.tv_sec) * 1000 + (now.tv_usec - last.tv_usec) / 1000;
            last = now;
        }

        if (res.error_info)
        {
            err_info_ = res.error_info;
            printf("resolve err(%s)\n", res.error_info);
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
        // TODO: get the first address

        printf("dns_callback status(%d) timeouts(%d)\n", status, timeouts);

        dns_res_t& res = *(dns_res_t*)arg;
        if (status != ARES_SUCCESS)
        {
            res.error_info = ares_strerror(status);
            printf("dns_callback err(%s)\n", res.error_info);
            return;
        }

        if (AF_INET == hptr->h_addrtype)
        {
            char** pptr = hptr->h_addr_list;
            if (*pptr)
            {
                memcpy(res.address, *pptr, res.len);
                return;
            }

            res.error_info = "no invalid address get";
            printf("dns_callback err(%s)\n", res.error_info);
        }
        else
        {
            res.error_info = "addrtype not supported";
            printf("addrtype(%d) not supported\n", hptr->h_addrtype);
        }
    }

    static int dns_wait_resolve(ares_channel channel_, int timeout_ms)
    {
        if (timeout_ms < 0)
        {
            ares_process_fd(channel_, ARES_SOCKET_BAD, ARES_SOCKET_BAD);

            // TODO: 这里若不执行 ares_cancel，channel_ 析构会出现 coredump
            ares_cancel(channel_);
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
            nfds = poll(pfd, num, timeout_ms/*milliseconds */);
        }
        else
        {
            nfds = 0;
        }

        if (!nfds)
        {
            ares_process_fd(channel_, ARES_SOCKET_BAD, ARES_SOCKET_BAD);

            // TODO: 这里若不执行 ares_cancel，在超时错误时，channel_ 析构会出现 coredump
            ares_cancel(channel_);
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
        int ret = dr.resolve(AF_INET, domain, timeout_ms, &sa.sin_addr.s_addr, sizeof(sa.sin_addr.s_addr));
        if (0 != ret)
        {
            printf("dr.resolve ret(%d) err(%s)\n", ret, dr.error_info().c_str());
            return;
        }
        char strIP[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &(sa.sin_addr), strIP, INET_ADDRSTRLEN);
        printf("%s\n", strIP);
    }
    else
    {
        printf("dns_resolver_t init err(%s)\n", dr.error_info().c_str());
        return;
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


# Performance compare

## Result

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


## c-ares 和文件相关的系统调用

```
$strace -s1024 -tt -e trace=file ./performance_compare
11:28:34.980050 execve("./performance_compare", ["./performance_compare"], [/* 788 vars */]) = 0
11:28:34.981232 access("/etc/ld.so.preload", R_OK) = 0
11:28:34.981347 open("/etc/ld.so.preload", O_RDONLY|O_CLOEXEC) = 3
11:28:34.981678 readlink("/proc/self/exe", "/data/home/gerryyang/github/mac-utils/programing/cpp/http/c-ares/performance_compare", 4096) = 84
11:28:34.981808 open("/lib64/libonion.so", O_RDONLY|O_CLOEXEC) = 3
11:28:34.982477 open("/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
11:28:34.982810 open("/lib64/libstdc++.so.6", O_RDONLY|O_CLOEXEC) = 3
11:28:34.983499 open("/lib64/libm.so.6", O_RDONLY|O_CLOEXEC) = 3
11:28:34.984061 open("/lib64/libgcc_s.so.1", O_RDONLY|O_CLOEXEC) = 3
11:28:34.984603 open("/lib64/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
11:28:34.985269 open("/lib64/libdl.so.2", O_RDONLY|O_CLOEXEC) = 3
11:28:34.987771 open("/etc/resolv.conf", O_RDONLY) = 3
11:28:34.988396 open("/etc/nsswitch.conf", O_RDONLY) = 3
11:28:34.988933 open("/dev/urandom", O_RDONLY) = 3
11:28:34.989195 open("/etc/hosts", O_RDONLY) = 3
resolve1 : 4248534 ns
11:28:34.992087 +++ exited with 0 +++
```

## c-ares 和网络相关的调用情况

```
$strace -s1024 -tt -e trace=network  ./performance_compare 
11:24:19.110979 socket(PF_INET, SOCK_DGRAM, IPPROTO_IP) = 3
11:24:19.111332 connect(3, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, 16) = 0
11:24:19.111470 sendto(3, "s\224\1\0\0\1\0\0\0\0\0\0\tgerryyang\3com\7default\3svc\7cluster\5local\0\0\1\0\1", 57, MSG_NOSIGNAL, NULL, 0) = 57
11:24:19.111864 recvfrom(3, "s\224\205\3\0\1\0\0\0\1\0\0\tgerryyang\3com\7default\3svc\7cluster\5local\0\0\1\0\1\7cluster\5local\0\0\6\0\1\0\0\0\21\0D\2ns\3dns\7cluster\5local\0\nhostmaster\7cluster\5local\0b}\3177\0\0\34 \0\0\7\10\0\1Q\200\0\0\0\36", 4097, 0, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, [16]) = 150
11:24:19.111962 sendto(3, "0h\1\0\0\1\0\0\0\0\0\0\tgerryyang\3com\3svc\7cluster\5local\0\0\1\0\1", 49, MSG_NOSIGNAL, NULL, 0) = 49
11:24:19.112057 recvfrom(3, 0x7ffe20c14bb0, 4097, 0, 0x7ffe20c14b90, 0x7ffe20c14b8c) = -1 EAGAIN (Resource temporarily unavailable)
11:24:19.112196 recvfrom(3, "0h\205\3\0\1\0\0\0\1\0\0\tgerryyang\3com\3svc\7cluster\5local\0\0\1\0\1\7cluster\5local\0\0\6\0\1\0\0\0\21\0D\2ns\3dns\7cluster\5local\0\nhostmaster\7cluster\5local\0b}\3177\0\0\34 \0\0\7\10\0\1Q\200\0\0\0\36", 4097, 0, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, [16]) = 142
11:24:19.112274 sendto(3, ">\277\1\0\0\1\0\0\0\0\0\0\tgerryyang\3com\7cluster\5local\0\0\1\0\1", 45, MSG_NOSIGNAL, NULL, 0) = 45
11:24:19.112356 recvfrom(3, 0x7ffe20c14bb0, 4097, 0, 0x7ffe20c14b90, 0x7ffe20c14b8c) = -1 EAGAIN (Resource temporarily unavailable)
11:24:19.112498 recvfrom(3, ">\277\205\3\0\1\0\0\0\1\0\0\tgerryyang\3com\7cluster\5local\0\0\1\0\1\7cluster\5local\0\0\6\0\1\0\0\0\21\0D\2ns\3dns\7cluster\5local\0\nhostmaster\7cluster\5local\0b}\3177\0\0\34 \0\0\7\10\0\1Q\200\0\0\0\36", 4097, 0, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, [16]) = 138
11:24:19.112577 sendto(3, "9\213\1\0\0\1\0\0\0\0\0\0\tgerryyang\3com\0\0\1\0\1", 31, MSG_NOSIGNAL, NULL, 0) = 31
11:24:19.112668 recvfrom(3, 0x7ffe20c14bb0, 4097, 0, 0x7ffe20c14b90, 0x7ffe20c14b8c) = -1 EAGAIN (Resource temporarily unavailable)
11:24:19.112811 recvfrom(3, "9\213\205\200\0\1\0\1\0\0\0\0\tgerryyang\3com\0\0\1\0\1\tgerryyang\3com\0\0\1\0\1\0\0\0\22\0\4w\34)f", 4097, 0, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, [16]) = 60
11:24:19.112894 socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP) = 4
11:24:19.112966 connect(4, {sa_family=AF_INET, sin_port=htons(0), sin_addr=inet_addr("119.28.41.102")}, 16) = 0
11:24:19.113039 getsockname(4, {sa_family=AF_INET, sin_port=htons(59710), sin_addr=inet_addr("9.135.18.186")}, [16]) = 0
resolve1 : 3858319 ns
11:24:19.113629 +++ exited with 0 +++
```

## getaddrinfo 和文件相关的系统调用

```
$strace -s1024 -tt -e trace=file ./performance_compare
11:30:30.549208 execve("./performance_compare", ["./performance_compare"], [/* 788 vars */]) = 0
11:30:30.550343 access("/etc/ld.so.preload", R_OK) = 0
11:30:30.550469 open("/etc/ld.so.preload", O_RDONLY|O_CLOEXEC) = 3
11:30:30.550707 readlink("/proc/self/exe", "/data/home/gerryyang/github/mac-utils/programing/cpp/http/c-ares/performance_compare", 4096) = 84
11:30:30.550800 open("/lib64/libonion.so", O_RDONLY|O_CLOEXEC) = 3
11:30:30.551300 open("/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
11:30:30.551562 open("/lib64/libstdc++.so.6", O_RDONLY|O_CLOEXEC) = 3
11:30:30.552006 open("/lib64/libm.so.6", O_RDONLY|O_CLOEXEC) = 3
11:30:30.552352 open("/lib64/libgcc_s.so.1", O_RDONLY|O_CLOEXEC) = 3
11:30:30.552682 open("/lib64/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
11:30:30.553106 open("/lib64/libdl.so.2", O_RDONLY|O_CLOEXEC) = 3
11:30:30.555178 stat("/etc/resolv.conf", {st_mode=S_IFREG|0644, st_size=105, ...}) = 0
11:30:30.555292 open("/etc/host.conf", O_RDONLY|O_CLOEXEC) = 3
11:30:30.555623 open("/etc/resolv.conf", O_RDONLY|O_CLOEXEC) = 3
11:30:30.556216 open("/etc/nsswitch.conf", O_RDONLY|O_CLOEXEC) = 3
11:30:30.556563 open("/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
11:30:30.556770 open("/lib64/libnss_files.so.2", O_RDONLY|O_CLOEXEC) = 3
11:30:30.557258 open("/etc/hosts", O_RDONLY|O_CLOEXEC) = 3
11:30:30.557588 open("/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
11:30:30.557798 open("/lib64/libnss_dns.so.2", O_RDONLY|O_CLOEXEC) = 3
11:30:30.558124 open("/lib64/libresolv.so.2", O_RDONLY|O_CLOEXEC) = 3
resolve2 : 13396723 ns
11:30:30.568509 +++ exited with 0 +++
```

## getaddrinfo 和网络相关的调用情况

```
$strace -s1024 -tt -e trace=network  ./performance_compare 
11:32:46.239270 socket(PF_NETLINK, SOCK_RAW, 0) = 3
11:32:46.239459 bind(3, {sa_family=AF_NETLINK, pid=0, groups=00000000}, 12) = 0
11:32:46.239733 getsockname(3, {sa_family=AF_NETLINK, pid=3627407, groups=00000000}, [12]) = 0
11:32:46.239809 sendto(3, "\24\0\0\0\26\0\1\3^\321}b\0\0\0\0\0\0\0\0", 20, 0, {sa_family=AF_NETLINK, pid=0, groups=00000000}, 12) = 20
11:32:46.239899 recvmsg(3, {msg_name(12)={sa_family=AF_NETLINK, pid=0, groups=00000000}, msg_iov(1)=[{"D\0\0\0\24\0\2\0^\321}b\217Y7\0\2\10\200\376\1\0\0\0\10\0\1\0\177\0\0\1\10\0\2\0\177\0\0\1\7\0\3\0lo\0\0\24\0\6\0\377\377\377\377\377\377\377\377\34\335\371@\34\335\371@P\0\0\0\24\0\2\0^\321}b\217Y7\0\2 \200\0\3\0\0\0\10\0\1\0\t\207\22\272\10\0\2\0\t\207\22\272\10\0\4\0\t\207\22\272\t\0\3\0eth0\0\0\0\0\24\0\6\0\377\377\377\377\377\377\377\377<\335\371@<\335\371@", 4096}], msg_controllen=0, msg_flags=0}, 0) = 148
11:32:46.239986 recvmsg(3, {msg_name(12)={sa_family=AF_NETLINK, pid=0, groups=00000000}, msg_iov(1)=[{"\24\0\0\0\3\0\2\0^\321}b\217Y7\0\0\0\0\0", 4096}], msg_controllen=0, msg_flags=0}, 0) = 20
11:32:46.241100 socket(PF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0) = 3
11:32:46.241204 connect(3, {sa_family=AF_LOCAL, sun_path="/var/run/nscd/socket"}, 110) = -1 ENOENT (No such file or directory)
11:32:46.241355 socket(PF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0) = 3
11:32:46.241426 connect(3, {sa_family=AF_LOCAL, sun_path="/var/run/nscd/socket"}, 110) = -1 ENOENT (No such file or directory)
11:32:46.244024 socket(PF_INET, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, IPPROTO_IP) = 3
11:32:46.244110 setsockopt(3, SOL_IP, IP_RECVERR, [1], 4) = 0
11:32:46.244187 connect(3, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, 16) = 0
11:32:46.244315 sendto(3, "\356\361\1\0\0\1\0\0\0\0\0\0\tgerryyang\3com\7default\3svc\7cluster\5local\0\0\1\0\1", 57, MSG_NOSIGNAL, NULL, 0) = 57
11:32:46.244792 recvfrom(3, "\356\361\205\3\0\1\0\0\0\1\0\0\tgerryyang\3com\7default\3svc\7cluster\5local\0\0\1\0\1\7cluster\5local\0\0\6\0\1\0\0\0\32\0D\2ns\3dns\7cluster\5local\0\nhostmaster\7cluster\5local\0b}\321>\0\0\34 \0\0\7\10\0\1Q\200\0\0\0\36", 1024, 0, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, [16]) = 150
11:32:46.244974 socket(PF_INET, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, IPPROTO_IP) = 3
11:32:46.245053 setsockopt(3, SOL_IP, IP_RECVERR, [1], 4) = 0
11:32:46.245130 connect(3, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, 16) = 0
11:32:46.245262 sendto(3, "\242P\1\0\0\1\0\0\0\0\0\0\tgerryyang\3com\3svc\7cluster\5local\0\0\1\0\1", 49, MSG_NOSIGNAL, NULL, 0) = 49
11:32:46.245717 recvfrom(3, "\242P\205\3\0\1\0\0\0\1\0\0\tgerryyang\3com\3svc\7cluster\5local\0\0\1\0\1\7cluster\5local\0\0\6\0\1\0\0\0\32\0D\2ns\3dns\7cluster\5local\0\nhostmaster\7cluster\5local\0b}\321>\0\0\34 \0\0\7\10\0\1Q\200\0\0\0\36", 1024, 0, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, [16]) = 142
11:32:46.245921 socket(PF_INET, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, IPPROTO_IP) = 3
11:32:46.246015 setsockopt(3, SOL_IP, IP_RECVERR, [1], 4) = 0
11:32:46.246111 connect(3, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, 16) = 0
11:32:46.246284 sendto(3, "\262$\1\0\0\1\0\0\0\0\0\0\tgerryyang\3com\7cluster\5local\0\0\1\0\1", 45, MSG_NOSIGNAL, NULL, 0) = 45
11:32:46.246819 recvfrom(3, "\262$\205\3\0\1\0\0\0\1\0\0\tgerryyang\3com\7cluster\5local\0\0\1\0\1\7cluster\5local\0\0\6\0\1\0\0\0\32\0D\2ns\3dns\7cluster\5local\0\nhostmaster\7cluster\5local\0b}\321>\0\0\34 \0\0\7\10\0\1Q\200\0\0\0\36", 1024, 0, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, [16]) = 138
11:32:46.247011 socket(PF_INET, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, IPPROTO_IP) = 3
11:32:46.247120 setsockopt(3, SOL_IP, IP_RECVERR, [1], 4) = 0
11:32:46.247255 connect(3, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, 16) = 0
11:32:46.247386 sendto(3, "n\225\1\0\0\1\0\0\0\0\0\0\tgerryyang\3com\0\0\1\0\1", 31, MSG_NOSIGNAL, NULL, 0) = 31
11:32:46.247832 recvfrom(3, "n\225\205\200\0\1\0\1\0\0\0\0\tgerryyang\3com\0\0\1\0\1\tgerryyang\3com\0\0\1\0\1\0\0\0\32\0\4w\34)f", 1024, 0, {sa_family=AF_INET, sin_port=htons(53), sin_addr=inet_addr("9.166.31.254")}, [16]) = 60
resolve2 : 8803496 ns
11:32:46.248537 +++ exited with 0 +++
```


## 测试代码

``` cpp
#include <sys/time.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <ares.h>
#include <pthread.h>
#include <deque>
#include <netdb.h>
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
        //printf("dns_callback status(%d) timeouts(%d)\n", status, timeouts);

        dns_res_t& res = *(dns_res_t*)arg;
        if (status != ARES_SUCCESS)
        {
            res.error_info = ares_strerror(status);
            printf("dns_callback err(%s)\n", res.error_info);
            return;
        }

        if (AF_INET == hptr->h_addrtype)
        {
            char** pptr = hptr->h_addr_list;
            if (*pptr)
            {
                memcpy(res.address, *pptr, res.len);
                return;
            }

            res.error_info = "no invalid address get";
            printf("dns_callback err(%s)\n", res.error_info);
        }
        else
        {
            res.error_info = "addrtype not supported";
            printf("addrtype(%d) not supported\n", hptr->h_addrtype);
        }
    }

    static int dns_wait_resolve(ares_channel channel_, int timeout_ms)
    {
        if (timeout_ms < 0)
        {
            ares_process_fd(channel_, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
            ares_cancel(channel_);
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
            ares_cancel(channel_);
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
    for (auto i = 0; i < MAXCNT; ++i)
    {
        struct sockaddr_in sa = {};
        std::string domain = "gerryyang.com";
        int timeout_ms = 1000;
        dns_resolver_t dr;
        if (dr)
        {
            dr.resolve(AF_INET, domain, timeout_ms, &sa.sin_addr.s_addr, sizeof(sa.sin_addr.s_addr));
            if (0 != ret)
            {
                printf("resolve ret(%d) err(%s)\n", ret, dr.error_info().c_str());
                return;
            }
            inet_ntop(AF_INET, &(sa.sin_addr), strIP, INET_ADDRSTRLEN);
            //printf("%s\n", strIP);
        }
        else
        {
            printf("dns_resolver_t init err(%s)\n", dr.error_info().c_str());
            return;
        }
    }
}

void resolve2()
{
    ScopedTimer timer("resolve2");

    char strIP[INET_ADDRSTRLEN] = {0};
    for (auto i = 0; i < MAXCNT; ++i)
    {
        struct addrinfo* addr;
        int result = getaddrinfo("gerryyang.com", NULL, NULL, &addr);
        if (result != 0)
        {
            printf("Error from getaddrinfo: %s\n", gai_strerror(result));
            return;
        }
        struct sockaddr_in* psa = (struct sockaddr_in*)addr->ai_addr;
        inet_ntop(AF_INET, &(psa->sin_addr), strIP, INET_ADDRSTRLEN);
        //printf("%s\n", strIP);

        freeaddrinfo(addr);
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



