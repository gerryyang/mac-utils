#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <unistd.h>
#include "addressbook.pb.h"
using namespace std;

//#define PB_REPEATED
//#define VECTOR
#define MAP
//#define UNORDERED_MAP

#define TIME_BEG() \
auto start = std::chrono::high_resolution_clock::now();

#define TIME_END() \
auto end = std::chrono::high_resolution_clock::now(); \
std::chrono::duration<double> diff = end - start; \
std::cout << "elapse(" << diff.count() << "s)\n";

int main()
 {
 	std::cout << "pid(" << getpid() << ")\n";

	/*char go_on;
	printf("press any key to go on...");
	scanf("%c", &go_on);
	(void)go_on;*/

	int max = 1000000;
	int cnt = 0;

#ifdef PB_REPEATED
	std::cout << "pb repeated\n";
	tutorial::AddressBook address_book;

	for (int i = 0; i != max; ++i) {
		auto pstPerson = address_book.add_people();
		pstPerson->set_id(i);
		pstPerson->set_name(std::to_string(i));
	}

	
	TIME_BEG();
	std::string strSearch = std::to_string(max - 1);
	auto stAddressBookIter = address_book.people();
	for (auto it = stAddressBookIter.begin(); it != stAddressBookIter.end(); ++it, ++cnt) {
		//std::cout << "id(" << it->id() << ") name(" << it->name() << ")\n";
		if (it->name() == strSearch) {
			std::cout << "find it(" << strSearch << ") cnt(" << cnt << ")\n";
			break;
		}
	}
	TIME_END();

#elif defined(VECTOR)
	std::cout << "vector\n";
	std::vector<std::string> ab;
	for (int i = 0; i != max; ++i) {
		ab.push_back(std::to_string(i));
	}
	
	TIME_BEG();
	std::string strSearch = std::to_string(max - 1);
	for (int i = 0; i != max; ++i, ++cnt) {
		if (ab[i] == strSearch) {
			std::cout << "find it(" << strSearch << ") cnt(" << cnt << ")\n";
			break;
		}
	}
	
	/*for (auto& item : ab) {
		//std::cout << item << "\n";
		if (item == strSearch) {
			std::cout << "find it(" << strSearch << ")\n";
			break;
		}
	}*/
	TIME_END();

#elif defined(UNORDERED_MAP)
	std::cout << "unordered_map\n";
	std::unordered_map<std::string, int> ab;
	for (int i = 0; i != max; ++i) {
		ab.emplace(std::to_string(i), i);
	}
	
	TIME_BEG();
	std::string strSearch = std::to_string(max - 1);
	for (auto iter = ab.begin(); iter != ab.end(); ++iter, ++cnt) {
		//std::cout << "key(" << iter->first << ") value(" << iter->second << ")\n";
		if (iter->first == strSearch) {
			std::cout << "find it(" << strSearch << ") cnt(" << cnt << ")\n";
			break;
		}
	}

	/*for (auto& item : ab) {
		//std::cout << "key(" << item.first << ") value(" << item.second << ")\n";
		if (item.first == strSearch) {
			std::cout << "find it(" << strSearch << ")\n";
			break;
		}
	}*/
	TIME_END();

#elif defined(MAP)
	std::cout << "map\n";
	std::map<std::string, int> ab;
	for (int i = 0; i != max; ++i) {
		ab.emplace(std::to_string(i), i);
	}
	
	TIME_BEG();
	std::string strSearch = std::to_string(max - 1);
	for (auto iter = ab.begin(); iter != ab.end(); ++iter, ++cnt) {
		//std::cout << "key(" << iter->first << ") value(" << iter->second << ")\n";
		if (iter->first == strSearch) {
			std::cout << "find it(" << strSearch << ") cnt(" << cnt << ")\n";
			break;
		}
	}

	/*for (auto& item : ab) {
		//std::cout << "key(" << item.first << ") value(" << item.second << ")\n";
		if (item.first == strSearch) {
			std::cout << "find it(" << strSearch << ")\n";
			break;
		}
	}*/
	TIME_END();

#else

#error "undefine!"
	
#endif
	
	//std::this_thread::sleep_for(std::chrono::seconds(100));

	return 0;
}
