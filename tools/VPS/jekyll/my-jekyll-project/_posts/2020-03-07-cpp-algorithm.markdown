---
layout: post
title:  "CPP Algorithm"
date:   2020-04-25 14:00:00 +0800
categories: [C/C++, Data Structure/Algorithm]
---

* Do not remove this line (it will not be displayed)
{: toc}


# 2021 TPC腾讯程序设计竞赛 Beautiful Sequence (正赛)

一个序列 b1,b2, ... ,bn 是美丽序列，当对所有 1 <= i < n 有 b<sub>i</sub> * b<sub>i+1</sub> <= 0，给定一个整数序列 a1,a2, ... ,an 问是否可以通过重新排列其中的元素使其变为美丽序列。

Input:

有多组测试数据，第一行输入一个整数 T 代表测试数据组数。对于每组测试数据：
第一行输入一个整数 n (2 <= n <= 10^5) 表示序列的长度
第二行输入 n 个整数 a1,a2, ... , an （-10^9 <= a<sub>i</sub> <= 10^9）表示序列的元素
保证所以数据 n 之和不超过 10^6

Output:

每组数据输出一行。若可以通过重新排列使得序列变为美丽序列输出 Yes，否则输出 No

Sample Input:

```
3
3
0 1 -1
5
2 3 4 -2 -3
5
5 5 5 5 5
```

Sample Output:

```
Yes
Yes
No
```

解题思路：记`x`为序列中正数的数量，`y`为序列中负数的数量，`z`为序列中零的数量，判断`max(x,y) - min(x,y) <= z + 1`即可。

``` cpp
#include <iostream>

int main()
{
    int T;
    scanf("%d", &T);

    while (T--)
    {
        int n;
        scanf("%d", &n);

        int x, y, z;
        x = y = z = 0;
        for (int i = 0; i != n; ++i)
        {
            int m;
            scanf("%d", &m);
            if (m == 0) ++z;
            else if (m < 0) ++x;
            else if (m > 0) ++y;
        }

        auto diff = 0;
        if (x < y) diff = y - x;
        else if (x > y) diff = x - y;

        if (diff <= z + 1) printf("Yes\n");
        else printf("No\n");
    }

    return 0;
}
```

``` golang
package main

import (
    "bufio"
    "fmt"
    "os"
    "strconv"
    "strings"
)

func ReadLine(reader *bufio.Reader) string {
    line, _ := reader.ReadString('\n')
    return strings.TrimRight(line, "\n")
}

func ReadInt(reader *bufio.Reader) int {
    num, _ := strconv.Atoi(ReadLine(reader))
    return num
}

func ReadArray(reader *bufio.Reader) []int {
    line := ReadLine(reader)
    strs := strings.Split(line, " ")
    nums := make([]int, len(strs))
    for i, s := range strs {
        nums[i], _ = strconv.Atoi(s)
    }
    return nums
}

func main() {
    reader := bufio.NewReader(os.Stdin)
    T := ReadInt(reader)
    for i := 0; i < T; i++ {

        x := 0
        y := 0
        z := 0
        diff := 0
        ReadInt(reader)
        m := ReadArray(reader)
        for _, t := range m {
            if t == 0 {
                z++
            } else if t > 0 {
                x++
            } else {
                y++
            }
        }
        if x > y {
            diff = x - y
        } else {
            diff = y - x
        }
        if diff <= z+1 {
            fmt.Println("Yes")
        } else {
            fmt.Println("No")
        }
    }
}
```

# 2020 TPC腾讯程序设计竞赛 Source of Happiness (正赛)

The famous instant message software QQ has recently released its badge system. Users are awarded with different types of badges when using the software if they meet certain conditions. One of the badge, the Source of Happiness badge, requires the users to send at least m memes in total during 3 continuous days. The user will be awarded with Source of Happiness at the end of the 3rd day.


![tpc_2r_1](/assets/images/202008/tpc_2r_1.png)

Hint:

For the first sample test case, one valid record array after filling the unknown integers is {5, 5, 15, 0, 5}, so the answer is 5 + 5 + 15 + 0 + 5 = 30.

``` cpp
#include <bits/stdc++.h>
#include <algorithm>

typedef struct A {
    long long ori;
    long long cur;
} t_a;

int main()
{
    int T;
    scanf("%d", &T);
    while (T--) {
        int n, m;
        scanf("%d %d", &n, &m);

        std::list<t_a> v;
        long long res = 0;

        for (int i = 0; i < n; ++i) {
            int x;
            scanf("%d", &x);

            t_a a;
            a.ori = a.cur = x;
            v.emplace_back(a);

            if (a.ori != -1) {
                res += a.ori;
            }
        }

        bool impos = false;
        while (v.size() > 2) {

            std::list<int> lost_pos_list;
            long long has = 0;

            auto iter = v.begin();
            for (int i = 1; iter != v.end(); ++iter, ++i) {

                if (iter->ori == -1) {
                    lost_pos_list.emplace_back(i - 1);
                }

                if (iter->cur != -1)  {
                    has += iter->cur;
                }

                if (i % 3 == 0) {
                    if (has < m) {
                        if (lost_pos_list.empty()) {
                            impos = true;
                            break;
                        }

                        auto update_idx = lost_pos_list.back();
                        auto update_iter = v.begin();
                        for (int i = 0; i != update_idx; ++i, ++update_iter);

                        int left = m - has;
                        res += left;

                        update_iter->cur = std::max(update_iter->cur, 0LL) + left;

                    }
                    v.pop_front();
                    break;
                }
            }// for

            if (impos) {
                break;
            }
        }// while

        if (impos) {
            printf("Impossible\n");
        } else {
            printf("%lld\n", res);
        }

    }// while

    return 0;
}
```


# 2020 TPC腾讯程序设计竞赛 Not Fibonacc (正赛)

二进制字符串是由数字 0 和 1 组成的字符串。给定一个二进制字符串 s=s1s2...sn(n >= 3)，您需要将里面的数字重新排列，使得重新排列后的字符串对于所有 3 <= i <= n 满足 s(i-2) + s(i-1) != s(i)。
​
输入格式：

有多组测试数据。第一行输入一个整数 T 代表测试数据组数。对于每组测试数据：

第一行输入一个二进制字符串 s (3 <= num(s) <= 10^5, s(i) ∈ {0, 1})，保证所有数据中 num(s) 之和不超过 10^6。

​输出格式：

对于每组数据，若存在符合要求的重排列，输出一行重排列后的字符串（若有多种合理答案，您可以输出任意一种），否则输出 "Impossible"（不输出引号）。

样例输入：

```
2
01110
000
```

样例输出：

```
11100
Impossible
```


``` cpp
#include <iostream>
#include <algorithm>
#include <string>

std::string proc(const std::string &str)
{
  int cnt1 = 0, cnt0 = 0;
  for (auto &c : str) {
    if (c == '0') {
      ++cnt0;

    } else if (c == '1') {
      ++cnt1;
    }
  }

  std::string ans;
  while (cnt0 >= 2 && cnt1 >= 1) {
    ans += "001";
    cnt0 -= 2;
    --cnt1;
  }

  if (cnt0 > 2 && cnt1 == 0) {
    return "Impossible";
  }

  ans += std::string(cnt0, '0');
  ans.insert(0, cnt1, '1');

  return ans;
}

int main()
{
  int n = 0;
  std::cin >> n;

  while (n-- > 0) {
    std::string str;
    std::cin >> str;
    std::cout << proc(str) << std::endl;
  }

  return 0;
}
```


# 2020 TPC腾讯程序设计竞赛 A IPv6 (正赛)

网际协议第 6 版（IPv6）是网际协议（IP）的最新版本。该协议用以鉴别并定位网络上的计算机，并对网络流量进行路由。该协议使用 128 个二进制位代表一个网络地址，其设计目的是为了替换第 4 版协议。

IPv6 的 128 位地址可以分为 8 组，每组各 16 位。我们可以将每组二进制位写成 4 个十六进制数，各组之间用冒号（:）分隔。例如，2001:0db8:0000:0000:0000:ff00:0042:8329 就是一个 IPv6 地址。

方便起见，一个 IPv6 地址可以按以下规则缩写为一个更加简短的表现形式：

* 每一组十六进制数的前导零会被去除。例如，0042 将变为 42。
* 连续多组十六进制表示的 0 会被一对双冒号（::）替换。请注意，一个地址中双冒号至多出现一次，否则该缩写对应的完整 IPv6 地址可能无法确定。

以下是这些简写规则的使用范例：

* 完整 IPv6 地址：2001:0db8:0000:0000:0000:ff00:0042:8329
* 去除每组十六进制数中的前导零后：2001:db8:0:0:0:ff00:42:8329
* 省略连续的十六进制零后：2001:db8::ff00:42:8329

根据上述规则，回环地址 0000:0000:0000:0000:0000:0000:0000:0001 可以被简写为 ::1

你的任务就是将一个缩写后的 IPv6 地址改写为一个完整的地址。

输入格式：

有多组测试数据。第一行输入一个整数 T（约 1000）代表测试数据组数。对于每组测试数据：

第一行输入一个字符串 s 代表一个合法的缩写后的 IPv6 地址，保证 s 只包含数字、小写字母和冒号。

输出格式：

每组数据输出一行一个字符串，代表完整的 IPv6 地址。

样例输入：

```
4
7abc::00ff:fffc
fc:0:0:8976:0:0:0:ff
2c0f:9981::
::
```

样例输出：

```
7abc:0000:0000:0000:0000:0000:00ff:fffc
00fc:0000:0000:8976:0000:0000:0000:00ff
2c0f:9981:0000:0000:0000:0000:0000:0000
0000:0000:0000:0000:0000:0000:0000:0000
```


``` cpp
#include <cstdio>
#include <string>
#include <vector>

bool g_find_double_colon = false;
std::vector<std::string> g_res_vec;

void split_str(const std::string& str, const std::string& delimiters, std::vector<std::string>& tokens)
{
        // skip delimiters at beginning
        // ,a,b,c, -> [a, b, c]
        std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

        while (std::string::npos != pos || std::string::npos != lastPos)
        {
                tokens.push_back(str.substr(lastPos, pos - lastPos));
                lastPos = str.find_first_not_of(delimiters, pos);
                pos = str.find_first_of(delimiters, lastPos);
        }
        return;
}

void replace(std::string& str)
{
        // :: -> :*:
        size_t index = str.find("::");
        if (index != std::string::npos) {
                str.replace(index, 2, ":*:");
        }
        return;
}

void print_res(const std::string& s)
{
        // print ab -> 00ab
        size_t s_len = s.length();
        if (s_len != 4) {
                for (size_t i = 0; i != 4 - s_len; ++i) {
                        printf("0");
                }
                printf("%s", s.c_str());
        } else {
                printf("%s", s.c_str());

        }
        return;
}

void func()
{
        // ex. 0000:0000:0000:0000:0000:0000:0000:0000
        char s[64] = {0};
        scanf("%s", s);

        std::string src = s;
        size_t src_len = src.length();

        // :: has only one in IPv6
        size_t found = src.find("::");
        if (found != std::string::npos) {

                // a::b:c -> a:*:b:c
                replace(src);

                std::vector<std::string> colon_vec;
                split_str(src, ":", colon_vec);

                size_t colon_vec_size = colon_vec.size();
                size_t left_size = 8 - (colon_vec_size - 1);

                for (auto& item : colon_vec) {
                        //printf("item[%s]\n", item.c_str());

                        if (item != "*") {
                                g_res_vec.push_back(item);

                        } else {
                                for (size_t i = 0; i != left_size; ++i) {
                                        g_res_vec.push_back("0000");
                                }
                        }
                }

        } else {

                // no find ::
                std::vector<std::string> colon_vec;
                split_str(src, ":", colon_vec);

                for (auto& item : colon_vec) {
                        g_res_vec.push_back(item);
                }
        }

        size_t i = 1;
        for (auto& res : g_res_vec) {
                if (i < g_res_vec.size()) {
                        print_res(res);
                        printf(":");
                } else {
                        print_res(res);
                        printf("\n");

                }
                ++i;
        }
        return;
}

int main()
{
        int group_cnt = 0;
        scanf("%d", &group_cnt);

        for (int i = 0; i != group_cnt; ++i) {
                g_res_vec.clear();
                func();
        }

        return 0;
}
```


# 2020 TPC腾讯程序设计竞赛 Group the Integers (正赛)

请将从 1 至 n （包括 1 与 n）的所有整数分成若干组，使得每一组内的整数互质。每个整数应恰好属于一个分组。求最少的分组数。

输入格式：

有多组测试数据。第一行输入一个整数 T（约 10^5）代表测试数据组数。对于每组测试数据：第一行输入一个整数 n（1 ≤ n ≤ 10^9）。

输出格式：

每组数据输出一行一个整数，代表最少分组数。

样例输入：

```
2
2
5
```

样例输出：

```
1
2
```

样例说明：

对于第二组样例数据，我们可以将整数划分为 {1, 2, 3, 5} 和 {4} 两组。

``` cpp
#include <cstdio>
#include <cmath>

void func()
{
        int last = 0;
        scanf("%d", &last);
        if (last == 1) printf("%d\n", last);
        else printf("%d\n", last / 2);

        return;
}

int main()
{
        int group_cnt = 0;
        scanf("%d", &group_cnt);

        for (int i = 0; i != group_cnt; ++i) {
                func();
        }

        return 0;
}
```

``` cpp
#include<bits/stdc++.h>

int main()
{
    int T, n;
    scanf("%d", &T);
    while(T--) {
        scanf("%d", &n);
        printf("%d\n", std::max(1, n >> 1));
    }
    return 0;
}
```

使用GoLang实现：

``` golang
package main
import (
    "bufio"
    "fmt"
    "os"
    "strconv"
    "strings"
)
func ReadLine(reader *bufio.Reader) string {
    line, _ := reader.ReadString('\n')
    return strings.TrimRight(line, "\n")
}
func ReadInt(reader *bufio.Reader) int {
    num, _ := strconv.Atoi(ReadLine(reader))
    return num
}
func ReadArray(reader *bufio.Reader) []int {
    line := ReadLine(reader);
    strs := strings.Split(line, " ")
    nums := make([]int, len(strs))
    for i, s := range strs {
        nums[i], _ = strconv.Atoi(s)
    }
    return nums
}
func main() {
    reader := bufio.NewReader(os.Stdin)
    T := ReadInt(reader)
    for casi := int(0); casi < T; casi++ {
        n := ReadInt(reader)
        n = n / 2
        if n == 0 {
            n = 1
        }
        fmt.Println(n)
    }
}
```

# 2020 TPC腾讯程序设计竞赛 A Easy Task (热身赛)

给定 n 个整数，每次可以进行如下操作：选择其中最大的整数 a 与最小的整数 b，并将它们都替换为 (a−b)。是否可以在有限次操作内使得所有整数都相等？如果是，最后的结果是什么？

输入格式：

有多组测试数据。第一行输入一个整数 T（约 20）代表测试数据组数。对于每组测试数据：
第一行输入一个整数 n（2≤n≤10）代表给定整数的数量。
第二行输入 n 个整数 a1, a2, ..., an (-10 ^ 5 <= ai <= 10 ^ 5) 代表给定的整数。

输出格式：

每组数据输出一行。若可以将所有整数变为相等，输出最终产生的那个整数；否则输出 "Impossible"（不输出引号）。

样例输入：

```
2
3
1 2 3
2
5 5
```

样例输出：

```
2
5
```

``` cpp
#include <bits/stdc++.h>

int main()
{
        int cas;
        scanf("%d", &cas);
        while (cas--) {
                int n;
                scanf("%d", &n);
                std::multiset<int> a;
                for (int i = 0; i < n; ++i) {
                        int x;
                        scanf("%d", &x);
                        a.insert(x);
                }
                while (*a.begin() != *a.rbegin()) {
                        auto p1 = a.begin();
                        auto p2 = std::prev(a.end());
                        a.erase(p1);
                        a.erase(p2);
                        int nval = *p2 - *p1;
                        a.insert(nval);
                        a.insert(nval);
                }
                printf("%d\n", *a.begin());
        }
        return 0;
}
```


# 单链表逆序

已知链表的头结点head, 写一个函数把这个链表逆序（即1->2->3转变为3->2->1）

``` cpp
#include <cstdio>
#include <iostream>

#define E_OK 0
#define E_FAIL 1

struct _Node
{
	_Node(int i) { data = i; }
	int data;
	_Node *next;
};
typedef struct _Node Node;


Node* createList(int len)
{
	Node* head = NULL;
	for (int i = 0; i != len; ++i) {
		Node* node = new Node(len - i);
		node->next = head;
		head = node;
	}
	return head;
}

void prinNode(Node* head)
{
	Node* cur = head;
	while (cur != NULL) {
		printf("%d ", cur->data);
		cur = cur->next;
	}
	printf("\n");
}

void reverseList(Node** head)
{
	Node* last = *head;
	Node* cur = (*head)->next;
	Node* pre = NULL;
	while (cur != NULL) {
		pre = cur->next;
		cur->next = last;
		last = cur;
		cur = pre;
	}
	(*head)->next = NULL;
	*head = last;
}

void deleteList(Node* head)
{
	Node* cur;
	while (head != NULL) {
		cur = head;
		head = head->next;
		delete cur;
	}
}

void test(int cnt)
{
	Node* head = createList(cnt);
	prinNode(head);
	reverseList(&head);
	prinNode(head);
	deleteList(head);
}

int main(int argc, char **argv)
{
	test(3);
	test(5);

	printf("done\n");
	return E_OK;
}
```

# LRU简单实现

设计和实现一种`LRU cache（Least Recently Used cache）`, 支持`get`和`set`操作。
* `get(key)`: 如果key存在key中则返回对应的value, 否则返回 -1
* `set(key, value)`: 更新key对应的value, 如果key不存在则插入对应的key, value。如果cache达到容量则淘汰最久未使用的键值对之后再插入


``` cpp
#include <cstdio>
#include <iostream>
#include <list>
#include <unordered_map>

#define E_OK 0
#define E_FAIL 1
#define E_NOT_FOUND -1

class LRUCache
{
public:

	typedef typename std::pair<int, int> kv_t;
	typedef typename std::list<kv_t>::iterator list_iterator_t;

	LRUCache(int capacity)
	{
		m_max_size = capacity;
	}

	int get(int key)
	{
		auto it = m_cache_map.find(key);
		if (it == m_cache_map.end()) {
			return E_NOT_FOUND;

		} else {
			// move get item to begin position
			m_cache_list.splice(m_cache_list.begin(), m_cache_list, it->second);
			return it->second->second;
		}
	}

	void set(int key, int value)
	{
		m_cache_list.push_front(kv_t(key, value));

		// find it from cache
		auto it = m_cache_map.find(key);
		if (it != m_cache_map.end()) {
			// delete origin item
			m_cache_list.erase(it->second);
			m_cache_map.erase(it);
		}
		m_cache_map[key] = m_cache_list.begin();

		// check size and delete last one
		if (m_cache_map.size() > m_max_size) {
			auto last = m_cache_list.end();
			last--;
			m_cache_map.erase(last->first);
			m_cache_list.pop_back();
		}
	}

private:
	std::list<kv_t> m_cache_list;
	std::unordered_map<int, list_iterator_t> m_cache_map;

	int m_max_size;
};


int main(int argc, char **argv)
{
	LRUCache lru(2);
	lru.set(1, 1);
	printf("lru.get(1) = %d\n", lru.get(1));
	printf("lru.get(2) = %d\n", lru.get(2));

	lru.set(2, 2);
	printf("lru.get(2) = %d\n", lru.get(2));

	lru.set(3, 3);
	printf("lru.get(1) = %d\n", lru.get(1));

	printf("done\n");
	return E_OK;
}
/*
g++ -std=c++11 -o lru_cache lru_cache.cpp
lru.get(1) = 1
lru.get(2) = -1
lru.get(2) = 2
lru.get(1) = -1
done
*/
```

refer: https://github.com/lamerman/cpp-lru-cache/blob/master/include/lrucache.hpp

# 封装迭代器

``` cpp
#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <iterator>

using record = std::map<std::string, std::string>;

class A
{

public:
        record m_var{
            {"a", "b"},
            {"c", "d"}
        };

};

class B
{
public:
        class iterator {

        public:

                using value_type = std::string;
                using reference = const value_type&;
                using pointer = const value_type*;

                iterator(const std::string tag, record *ref) : m_ref(ref), m_iter(ref->end()) {}
                iterator(record *ref) : m_ref(ref), m_iter(ref->begin()) {
                }

                reference operator*() const noexcept {
                        return m_iter->first;
                }
                iterator& operator++() {
                        ++m_iter;
                        return *this;
                }

                bool operator==(const iterator& rhs) const noexcept {
                        return m_iter == rhs.m_iter;
                }

                bool operator!=(const iterator& rhs) const noexcept {
                        return !operator==(rhs);
                }

        private:
                record* m_ref;
                record::iterator m_iter;
        };

        B(record* ref) : m_ref(ref) {
        }

        iterator begin() {
                return iterator(m_ref);
        }

        iterator end() const noexcept {
                return iterator("END", m_ref);
        }

private:
        record* m_ref;

};

int main()
{
        A a;
        for (auto& v : a.m_var) {
                std::cout << v.first << "\n";
        }

        B b(&a.m_var);
        for (auto& v : b) {
                std::cout << v << "\n";
        }

        return 0;
}
```

refer:

* http://www.cplusplus.com/reference/iterator/
* https://zh.cppreference.com/w/cpp/iterator/iterator_tags
* https://stackoverflow.com/questions/8054273/how-to-implement-an-stl-style-iterator-and-avoid-common-pitfalls/8054856
* https://stackoverflow.com/questions/7758580/writing-your-own-stl-container/7759622#7759622
* https://www.fluentcpp.com/2018/05/08/std-iterator-deprecated/


# 判断是否是闰年

判断闰年时有两个条件：

1. `year % 4 == 0 && year % 100 != 0`：这一条件用于判断非整百年份。**能被4整除的年份通常是闰年，但整百年份是个例外**。所以需要排除能被100整除的情况，确保只有非整百年份且能被4整除的才是闰年。例如，2024年能被4整除且不能被100整除，是闰年；而1900年能被4整除，但也能被100整除，不符合该条件，不是闰年。

2. `year % 400 == 0`：这是针对整百年份的判断条件。整百年份必须能被400整除才是闰年。例如，2000年能被400整除，是闰年；1900年不能被400整除，不是闰年。

通过这两个条件的组合使用，就可以准确判断任意一个年份是否为闰年。

``` c
#include <stdio.h>

int main() {
    int year;
    printf("请输入一个年份: ");
    scanf("%d", &year);

    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        printf("%d年是闰年。\n", year);
    } else {
        printf("%d年不是闰年。\n", year);
    }

    return 0;
}
```



