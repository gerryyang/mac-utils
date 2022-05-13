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

const int MAXCNT = 100000;

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
