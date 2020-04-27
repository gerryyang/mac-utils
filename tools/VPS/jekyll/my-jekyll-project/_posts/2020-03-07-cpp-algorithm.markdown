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
using second_map_t = std::map<std::string, record>;

class A
{

public:
        record m_var{{"a", "b"}, {"c", "d"}};

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

#if 0
        for (auto& v : a.m_var) {
                std::cout << v.first << "\n";
        }
#endif

        B b(&a.m_var);
        for (auto& v : b) {
                std::cout << v << "\n";
        }

        return 0;
}
```

refer:

* https://stackoverflow.com/questions/8054273/how-to-implement-an-stl-style-iterator-and-avoid-common-pitfalls/8054856
* http://www.cplusplus.com/reference/iterator/
* https://stackoverflow.com/questions/7758580/writing-your-own-stl-container/7759622#7759622


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

