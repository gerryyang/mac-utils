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
		thread_local char buf[LEN] = {0};
		sprintf(buf, "%d", i);
		thread_local std::string str;
		str = buf;
	}
}

void ss_string() 
{
	ScopedTimer timer("ss_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		thread_local std::stringstream ss;
		ss.clear();
		ss << i;
		thread_local std::string str;
		str = ss.str();
	}
}

void to_string()
{
	ScopedTimer timer("to_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		thread_local std::string str;
		str = std::to_string(i);
	}
}

void fmt_string() 
{
	ScopedTimer timer("fmt_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		thread_local std::string str;
		str = fmt::format("{}", i);
	}
}

void fmt_int_string() 
{
	ScopedTimer timer("fmt_int_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		auto fi = fmt::format_int(i);
		// fi.data() is the data, fi.size() is the size
		thread_local std::string str;
		str = fi.data();
	}
}

#if 0
void std_fmt_string() 
{
	ScopedTimer timer("std_fmt_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		thread_local std::string str;
		str = std::format("{}", i);
	}
}
#endif

void tc_string() 
{
	ScopedTimer timer("tc_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		thread_local char buf[LEN] = {0};
		std::to_chars(buf, buf + LEN, i);
		thread_local std::string str;
		str = buf;
	}
}

char* uint2str(uint32_t n, char *buf)
{
	static const char tbl[200] = {
		'0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9',
		'1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9',
		'2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9',
		'3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9',
		'4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9',
		'5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9',
		'6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9',
		'7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9',
		'8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9',
		'9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9'
	};
	char *p = buf;

	if (n < 10000) {
		uint32_t a = (n / 100) << 1;
		uint32_t b = (n % 100) << 1;

		if (n >= 1000) { *p++ = tbl[a]; }
		if (n >= 100) { *p++ = tbl[a + 1]; }
		if (n >= 10) { *p++ = tbl[b]; }

		*p++ = tbl[b + 1];
	}
	else if (n < 100000000) {
		uint32_t w = (n / 10000);
		uint32_t x = (n % 10000);

		uint32_t a = (w / 100) << 1;
		uint32_t b = (w % 100) << 1;
		uint32_t c = (x / 100) << 1;
		uint32_t d = (x % 100) << 1;

		if (n >= 10000000) { *p++ = tbl[a]; }
		if (n >= 1000000) { *p++ = tbl[a + 1]; }
		if (n >= 100000) { *p++ = tbl[b]; }

		*p++ = tbl[b + 1];
		*p++ = tbl[c];
		*p++ = tbl[c + 1];
		*p++ = tbl[d];
		*p++ = tbl[d + 1];
	}
	else {
		uint32_t m = n / 100000000;
		n %= 100000000;

		if (m >= 10) {
			uint32_t a = m << 1;
			*p++ = tbl[a];
			*p++ = tbl[a + 1];
		}
		else {
			*p++ = '0' + (char)m;
		}

		uint32_t w = (n / 10000);
		uint32_t x = (n % 10000);

		uint32_t a = (w / 100) << 1;
		uint32_t b = (w % 100) << 1;
		uint32_t c = (x / 100) << 1;
		uint32_t d = (x % 100) << 1;

		*p++ = tbl[a];
		*p++ = tbl[a + 1];
		*p++ = tbl[b];
		*p++ = tbl[b + 1];
		*p++ = tbl[c];
		*p++ = tbl[c + 1];
		*p++ = tbl[d];
		*p++ = tbl[d + 1];
	}

	return p;
}

void table_string() 
{
	ScopedTimer timer("table_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		thread_local char buf[16];
		char *p = uint2str(i, buf);
		thread_local std::string str;
		str.assign(buf, p - buf);
	}
}

void compilea_string()
{
#define STR(x) #x
#define TOSTRING(x) STR(x)

	ScopedTimer timer("compilea_string");
	for (auto i = 0; i < MAXCNT; ++i) {
		thread_local std::string str;
		str = TOSTRING(10000);
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

	// 对 num 每位进行展开，例如，num = 123 则展开为 explode<neg, 0, 1, 2, 3>
	template<bool neg, uintmax_t rem, uint8_t... digits>
		struct explode : explode<neg, rem / 10, rem % 10, digits...> {};

	// 展开终止
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
		thread_local std::string str;
		str = string_from<unsigned, 10000>::value;
	}
}

int main() 
{
	sprintf_string();
	ss_string();
	to_string();
	fmt_string();
	fmt_int_string();
	//std_fmt_string();
	tc_string();
	table_string();

	compilea_string();
	compileb_string();
}

