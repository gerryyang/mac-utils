#include <iostream>
#include <chrono>
#include <charconv>
#include <string>
#include <sstream>
#include <fmt/core.h>
#include <stdio.h>

class ScopedTimer {
 public:
  ScopedTimer(const char* name)
      : m_name(name),
        m_beg(std::chrono::high_resolution_clock::now()) { }
  ~ScopedTimer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_beg);
    std::cout << m_name << " : " << dur.count() << " ns\n";
  }
 private:
  const char* m_name;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_beg;
};

void to_string(){
    ScopedTimer timer("to_string");
    for(size_t i=0; i<10000; i++) {
        std::to_string(i);
    }
}

void ss_to_string() {
    ScopedTimer timer("ss_to_string");
    for(size_t i=0; i<10000; i++) {
        std::stringstream ss;
        ss<<i;
    }
}

void fmt_string() {
    ScopedTimer timer("fmt_to_string");
    for(size_t i=0; i<10000; i++) {
        fmt::format("{}", i);
    }
}

void printf_string() {
    ScopedTimer timer("sprintf_to_string");
    for(size_t i=0; i<10000; i++) {
      char str[10];
      sprintf(str, "%d", i);
    }
}

void conv_string() {
    ScopedTimer timer("conv_to_string");
    for(size_t i=0; i<10000; i++) {
      char str[10];
      std::to_chars(str, str + 10, i);
    }
}

int main() {
  to_string();
  ss_to_string();
  fmt_string();
  printf_string();
  conv_string();
}
