#include <iostream>
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <vector>
#include <set>
#include <unordered_set>
#include <boost/container/flat_set.hpp>
#include "addressbook.pb.h"

// https://github.com/DigitalInBlue/Celero
#include <celero/Celero.h>

#ifdef ITER
static constexpr int max = ITER;
#else
static constexpr int max = 100000;
#endif


template <typename T>
inline T init(int n)
{
	T t;
	for (int i = 0; i <= n; ++i) {
		t.insert(t.end(), std::to_string(i));
	}
	return t;
}

template <>
inline tutorial::AddressBook init(int n)
{
	tutorial::AddressBook address_book;
	for (int i = 0; i <= n; ++i) {
		auto pstPerson = address_book.add_people();
		pstPerson->set_id(i);
		pstPerson->set_name(std::to_string(i));
	}
	return address_book;
}


static constexpr int samples = 1;
static constexpr int operations = 1;

static std::vector<std::string> vec = init<std::vector<std::string> >(max);
static std::set<std::string> set = init<std::set<std::string> >(max);
static std::unordered_set<std::string> unordered_set = init<std::unordered_set<std::string> >(max);
static boost::container::flat_set<std::string> flat_set = init<boost::container::flat_set<std::string> >(max);

static tutorial::AddressBook address_book = init<tutorial::AddressBook>(max);

static std::string last_v = std::to_string(max);

inline int pb_repeated_find()
{
	auto stAddressBookIter = address_book.people();
	for (auto it = stAddressBookIter.begin(); it != stAddressBookIter.end(); ++it) {
		if (it->name() == last_v) {
			break;
		}
	}
	return 0;
}


CELERO_MAIN

// BASELINE(GroupName, BaselineName, Samples, Operations)
// GroupName - The name of the benchmark group. This is used to batch together runs and results with their corresponding baseline measurement.
// BaselineName - The name of this baseline for reporting purposes.
// Samples - The total number of times you want to execute the given number of operations on the test code.
// Operations - The total number of times you want to run the test code per sample.
//
// The celero::DoNotOptimizeAway template is provided to ensure that the optimizing compiler does not eliminate your function or code. Since this feature is used in all of the sample benchmarks and their baseline, it's time overhead is canceled out in the comparisons.
//
// After the baseline is defined, various benchmarks are then defined. The syntax for the BENCHMARK macro is identical to that of the macro.
BASELINE(find, vector, 1/*samples*/, 1/*operations*/)
{
	celero::DoNotOptimizeAway(std::find(vec.begin(), vec.end(), last_v));
}

BENCHMARK(find, pb_repeated, 1, 1)
{
	celero::DoNotOptimizeAway(pb_repeated_find());
}

BENCHMARK(find, set, samples, operations)
{
	celero::DoNotOptimizeAway(set.find(last_v));
}

BENCHMARK(find, unordered_set, samples, operations)
{
	celero::DoNotOptimizeAway(unordered_set.find(last_v));
}

BENCHMARK(find, flat_set, samples, operations)
{
	celero::DoNotOptimizeAway(flat_set.find(last_v));
}


