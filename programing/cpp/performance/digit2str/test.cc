#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <charconv>
#include <chrono>

// https://en.cppreference.com/w/cpp/compiler_support
// there are currently no compilers that support: Text formatting (P0645R10, std::format)
// #include <format>

// https://github.com/fmtlib/fmt
#define FMT_HEADER_ONLY
#include <fmt/core.h>

const int MAXCNT = 10000;
const int LEN = 8;

class ScopedTimer {
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

void sprintf_string() 
{
	ScopedTimer timer("sprintf_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		char str[LEN] = {0};
		sprintf(str, "%d", i);
	}
}

void ss_string() 
{
	ScopedTimer timer("ss_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		std::stringstream ss;
		ss << i;
	}
}

void to_string()
{
	ScopedTimer timer("to_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto str = std::to_string(i);
	}
}

void fmt_string() 
{
	ScopedTimer timer("fmt_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto str = fmt::format("{}", i);
	}
}

#if 0
void std_fmt_string() 
{
	ScopedTimer timer("std_fmt_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto str = std::format("{}", i);
	}
}
#endif

void tc_string() 
{
	ScopedTimer timer("tc_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		char str[LEN] = {0};
		std::to_chars(str, str + LEN, i);
	}
}

void compilea_string()
{
#define STR(x) #x
#define TOSTRING(x) STR(x)

	ScopedTimer timer("compilea_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto str = TOSTRING(10000);
		(void)str;
	}
#undef TOSTRING
#undef STR
}

namespace detail
{
	template<uint8_t... digits> struct positive_to_chars {
		static const char value[];
		static constexpr size_t size = sizeof...(digits);
	};
	template<uint8_t... digits> const char positive_to_chars<digits...>::value[] = {('0' + digits)..., 0};

	template<uint8_t... digits> struct negative_to_chars {
		static const char value[];
	};
	template<uint8_t... digits> const char negative_to_chars<digits...>::value[] = {'-', ('0' + digits)..., 0};

	template<bool neg, uint8_t... digits>
		struct to_chars : positive_to_chars<digits...> {};

	template<uint8_t... digits>
		struct to_chars<true, digits...> : negative_to_chars<digits...> {};

	template<bool neg, uintmax_t rem, uint8_t... digits>
		struct explode : explode<neg, rem / 10, rem % 10, digits...> {};

	template<bool neg, uint8_t... digits>
		struct explode<neg, 0, digits...> : to_chars<neg, digits...> {};

	template<typename T>
		constexpr uintmax_t cabs(T num) {
			return (num < 0) ? -num : num;
		}
}

template<typename T, T num>
struct string_from : ::detail::explode<num < 0, ::detail::cabs(num)> {};

void compileb_string() 
{
	ScopedTimer timer("compileb_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto str = string_from<unsigned, 10000>::value;
		(void)str;
	}
}

int main() 
{
	sprintf_string();
	ss_string();
	to_string();
	fmt_string();
	//std_fmt_string();
	tc_string();

	compilea_string();
	compileb_string();
}
