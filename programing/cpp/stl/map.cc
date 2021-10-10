
#include <iostream>
#include <map>

struct S
{
    S(int i, int j) : a(i), b(j) {}
    int a;
    int b;

    // ok, use operator< to sort
    bool operator<(const S& rhs) const
    {
        if (a < rhs.a)
        {
            return true;
        }
        else if (a == rhs.a)
        {
            if (b < rhs.b)
            {
                return true;
            }
            return false;
        }
        else
        {
            return false;
        }
    }
};

// ok, use operator< to sort
bool operator<(const S& lhs, const S& rhs)
{
    if (lhs.a < rhs.a)
    {
        return true;
    }
    else if (lhs.a == rhs.a)
    {
        if (lhs.b < rhs.b)
        {
            return true;
        }
        return false;
    }
    else
    {
        return false;
    }
}

// ok
struct SCompare
{
    bool operator()(const S& lhs, const S& rhs) const
    {
        if (lhs.a < rhs.a)
        {
            return true;
        }
        else if (lhs.a == rhs.a)
        {
            if (lhs.b < rhs.b)
            {
                return true;
            }
            return false;
        }
        else
        {
            return false;
        }
    }
};

// ok
namespace std
{
template<>
struct less<S>
{
    bool operator()(const S& lhs, const S& rhs) const
    {
        if (lhs.a < rhs.a)
        {
            return true;
        }
        else if (lhs.a == rhs.a)
        {
            if (lhs.b < rhs.b)
            {
                return true;
            }
            return false;
        }
        else
        {
            return false;
        }
    }
};
}

//using Map = std::map<S, std::string, SCompare>;
using Map = std::map<S, std::string>;

int main()
{
    Map m;

    auto f1 = [](bool b) { b ? std::cout << "true\n" : std::cout << "false\n"; };

    f1(m.emplace(std::make_pair(S(1, 1), "a")).second);
    f1(m.emplace(std::make_pair(S(1, 2), "b")).second);
    f1(m.emplace(std::make_pair(S(2, 1), "c")).second);

    auto f2 = [&m]()
    {
        for (auto& n : m)
        {
            std::cout << n.second << " ";
        }
    };
    f2();

    return 0;
}
/*
true
true
true
a b c
*/
