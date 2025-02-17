//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest.h"
#include <cstdio>
#include <iostream>
#include <string>

int factorial(const int number)
{
    return number < 1 ? 1 : number <= 1 ? number : factorial(number - 1) * number;
}

class Foo
{
public:
    std::string Name() {
        return "Foo";
    }
};

TEST_CASE("[math] basic stuff") {
    CHECK(6 > 5);
    WARN(6 > 7);
}

TEST_CASE("[function] factorial")
{
    CHECK(factorial(0) == 1);
    CHECK(factorial(1) == 1);
    CHECK(factorial(2) == 2);
    CHECK(factorial(3) == 6);
    CHECK(factorial(10) == 3628800);
}

TEST_CASE("[string] std::string") {
    std::string a("omg");
    CHECK(a == "omg");
}

TEST_CASE("[class] Foo Name") {
    Foo o;
    CHECK(o.Name() == "Foo");
}

TEST_CASE("[vector] vectors can be sized and resized") {
    std::vector<int> v(5);

    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 5);

    SUBCASE("adding to the vector increases it's size") {
        v.push_back(1);

        CHECK(v.size() == 6);
        CHECK(v.capacity() >= 6);
    }

    SUBCASE("reserving increases just the capacity") {
        v.reserve(6);

        CHECK(v.size() == 5);
        CHECK(v.capacity() >= 6);
    }
}

TEST_CASE("lots of nested subcases") {
    std::cout << std::endl << "root" << std::endl;
    SUBCASE("") {
        std::cout << "1" << std::endl;
        SUBCASE("") { std::cout << "1.1" << std::endl; }
    }
    SUBCASE("") {   
        std::cout << "2" << std::endl;
        SUBCASE("") { std::cout << "2.1" << std::endl; }
        SUBCASE("") {
            std::cout << "2.2" << std::endl;
            SUBCASE("") {
                std::cout << "2.2.1" << std::endl;
                SUBCASE("") { std::cout << "2.2.1.1" << std::endl; }
                SUBCASE("") { std::cout << "2.2.1.2" << std::endl; }
            }
        }
        SUBCASE("") { std::cout << "2.3" << std::endl; }
        SUBCASE("") { std::cout << "2.4" << std::endl; }
    }
}

int main(int argc, char** argv) 
{
    printf("main\n");

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int res = context.run();
    if (context.shouldExit()) {
        return res;
    }
    return 0;
}
