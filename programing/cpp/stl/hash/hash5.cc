// https://en.cppreference.com/w/cpp/utility/hash

#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_set>

struct S
{
    std::string first_name;
    std::string last_name;

    //bool operator==(const S&) const = default;  // since C++20

    bool operator==(const S& other) const
    {
        return first_name == other.first_name && last_name == other.last_name;
    }
};

// Before C++20.
// bool operator==(const S& lhs, const S& rhs)
// {
//     return lhs.first_name == rhs.first_name && lhs.last_name == rhs.last_name;
// }

// Custom hash can be a standalone function object.
struct MyHash
{
    std::size_t operator()(const S& s) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(s.first_name);
        std::size_t h2 = std::hash<std::string>{}(s.last_name);
        return h1 ^ (h2 << 1);  // or use boost::hash_combine
    }
};

// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<S>
{
    std::size_t operator()(const S& s) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(s.first_name);
        std::size_t h2 = std::hash<std::string>{}(s.last_name);
        return h1 ^ (h2 << 1);  // or use boost::hash_combine
    }
};

int main()
{
    std::string str = "Meet the new boss...";
    std::size_t str_hash = std::hash<std::string>{}(str);
    std::cout << "hash(" << std::quoted(str) << ") =\t" << str_hash << '\n';

    S obj = {"Hubert", "Farnsworth"};
    // Using the standalone function object.
    std::cout << "hash(" << std::quoted(obj.first_name) << ", " << std::quoted(obj.last_name) << ") =\t" << MyHash{}(obj)
              << " (using MyHash) or\n\t\t\t\t" << std::hash<S>{}(obj) << " (using injected specialization)\n";

    // Custom hash makes it possible to use custom types in unordered containers.
    // The example will use the injected std::hash<S> specialization above,
    // to use MyHash instead, pass it as a second template argument.
    std::unordered_set<S> names = {obj, {"Bender", "Rodriguez"}, {"Turanga", "Leela"}};
    for (auto const& s : names) std::cout << std::quoted(s.first_name) << ' ' << std::quoted(s.last_name) << '\n';
}
/*
$ ./hash5
hash("Meet the new boss...") =  10656026664466977650
hash("Hubert", "Farnsworth") =  12922914235676820612 (using MyHash) or
                                12922914235676820612 (using injected specialization)
"Turanga" "Leela"
"Bender" "Rodriguez"
"Hubert" "Farnsworth"
*/