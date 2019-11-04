---
layout: post
title:  "CPP in Action"
date:   2019-05-06 17:00:00 +0800
categories: cpp
---

* Do not remove this line (it will not be displayed)
{:toc}


# STL

[range-for](https://zh.cppreference.com/w/cpp/language/range-for)

[lambda](https://zh.cppreference.com/w/cpp/language/lambda)

[std::chrono](https://blog.csdn.net/u013390476/article/details/50209603)

# Time

https://linux.die.net/man/3/strptime

http://www.cplusplus.com/reference/ctime/mktime/

[How to convert a string variable containing time to time_t type in c++?](https://stackoverflow.com/questions/11213326/how-to-convert-a-string-variable-containing-time-to-time-t-type-in-c/11213640#11213640)

[Date/time conversion: string representation to time_t](https://stackoverflow.com/questions/321793/date-time-conversion-string-representation-to-time-t/321847#321847)

```
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main()
{
        struct tm tm = {0};
        char *res = strptime("2019-10-01 18:00:00", "%Y-%m-%d %H:%M:%S", &tm);
        if (res == NULL) {
                printf("strptime err\n");
                return -1;
        }
        time_t t = mktime(&tm); 

        // equal to `date -d "2019-10-01 18:00:00" +%s`
        printf("t[%ld]\n", t);

        return 0;
}
```


# Regex

[How do I write a regular expression that matches an IPv4 dotted address?](https://devblogs.microsoft.com/oldnewthing/20060522-08/?p=31113)

``` cpp
#include <boost/regex.hpp>

/*
作用: 使用boost库, 实现IPv4匹配
10.0.0.0 - 10.255.255.255
172.16.0.0 - 172.31.255.255
192.168.0.0 - 192.168.255.255
*/
void test_regex_ip()
{
	std::string ip[] = {
		std::string("10.0.0.1"),
		std::string("172.16.0.1"),
		std::string("192.168.0.1"),
		std::string("10.*.*.*"),
		std::string("172.16.*.*"),
		std::string("192.168.*.*")
	};

	for (int i = 0; i != sizeof(ip) / sizeof(ip[0]); ++i) {
		auto match = [&] {
			//boost::regex pattern("(\\d{1,3}(\\.\\d{1,3}){3})");
			//boost::regex pattern("(\\d{1,3}(\\..*){3})");
			boost::regex pattern("(172(\\.\\d{1,3}){2}(\\..*))");
			boost::smatch match;
			try {
				if (boost::regex_search(ip[i], match, pattern)) {
					std::cout << "ip: " << match[1] << std::endl;
				} else {
					std::cout << ip[i] << " did not match\n";
				}

			} catch (boost::regex_error &e) {
				std::cout << "err: " << e.what() << std::endl;
			}
		};

		match();
	}
}
```

# 编译器

[is-pragma-once-a-safe-include-guard](https://stackoverflow.com/questions/787533/is-pragma-once-a-safe-include-guard)


# Docs

1. [GCC online documentation]


[GCC online documentation]: https://gcc.gnu.org/onlinedocs/




