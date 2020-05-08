---
layout: post
title:  "CPP Algorithm"
date:   2020-04-25 14:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{: toc}


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
}

void replace(std::string& str)
{
        // :: -> :*:
        size_t index = str.find("::", index);
        if (index != std::string::npos) {
                str.replace(index, 2, ":*:");
        }
}

void print_res(const std::string& s)
{
        // print ab -> 00ab
        size_t s_len = s.length();
        if (s_len != 4) {
                for (int i = 0; i != 4 - s_len; ++i) {
                        printf("0");
                }
                printf("%s", s.c_str());
        } else {
                printf("%s", s.c_str());

        }
}

void func()
{
        // ex. 0000:0000:0000:0000:0000:0000:0000:0000
        char s[64] = {0};
        scanf("%s", s);

        std::string src = s;
        int src_len = src.length();

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
                                for (int i = 0; i != left_size; ++i) {
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

        int i = 1;
        for (auto& res : g_res_vec) {
                if (i < g_res_vec.size()) {
                        print_res(res);
                        printf(".");
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
/*
4
7abc::00ff:fffc
7abc.0000.0000.0000.0000.0000.00ff.fffc
fc:0:0:8976:0:0:0:ff
00fc.0000.0000.8976.0000.0000.0000.00ff
2c0f:9981::
2c0f.9981.0000.0000.0000.0000.0000.0000
::
0000:0000:0000:0000:0000:0000:0000:0000
*/
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


``` cpp
#include <cstdio>
#include <vector>
#include <cmath>

#define MIN_VAL (-1 * pow(10, 6))
#define MAX_VAL (pow(10, 6))

// case1:
// 0 3 5
// 5 3 5
// 2 2 2

// case2:
// -1 1 2
// 3 1 3

void calc_func(std::vector<int> &integer_vec)
{
	// 使用 sort 排序会更简单
	int min = MIN_VAL;
	int max = MAX_VAL;
	for (auto &i : integer_vec) {
		if (min == MIN_VAL) min = i;
		if (max == MAX_VAL) max = i;
		if (i <= min) {
			min = i;
		}
		if (i >= max) {
			max = i;
		}
	}
	//printf("min[%d] max[%d]\n", min, max);

	bool first = true;
	int first_val = 0;
	bool final = true;

	for (auto &i : integer_vec) {
		if (first) {
			first = false;
			first_val = i;
		} else if (first_val == i) {
			continue;
		} else {
			final = false;
			break;
		}
	}

	if (final) {
		printf("%d\n", first_val);
		return;
	}

	for (auto &i : integer_vec) {
		if (i == min || i == max) {
			i = max - min;
		}
	}

	calc_func(integer_vec);

	return;
}

void start_func()
{
	int integer_cnt = 0;
	scanf("%d", &integer_cnt);

	std::vector<int> integer_vec;
	for (int i = 0; i != integer_cnt; ++i) {
		int item = 0;
		scanf("%d", &item);
		integer_vec.push_back(item);
	}

	calc_func(integer_vec);

	return;
}

int main(int argc, char **argv)
{
	int test_group_cnt = 0;
	scanf("%d", &test_group_cnt);

	for (int i = 0; i != test_group_cnt; ++i) {
		start_func();
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

