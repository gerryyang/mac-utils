#include <cstdio>
#include <string_view>
#include <functional>
#include <cstring>

void test_hash()
{
    char str1[] = "hello world";
    char str2[] = "hello world";

    printf("str1=%p, str2=%p\n", str1, str2);

    using my_hash = std::hash<std::string_view>;

    auto h1 = my_hash{}(std::string_view(str1, sizeof(str1) - 1));
    auto h2 = my_hash{}(std::string_view(str1, sizeof(str1) - 1));
    auto h3 = my_hash{}(std::string_view(str2, sizeof(str2) - 1));
    auto h4 = my_hash{}(std::string_view(str2, sizeof(str2) - 1));

    strncpy(str1, "AAAAAAAA", sizeof(str1) - 1);
    str1[sizeof(str1) - 1] = '\0';
    auto h5 = my_hash{}(std::string_view(str1, sizeof(str1) - 1));

    printf("h1=%zd h2=%zd h3=%zd h4=%zd h5=%zd\n", h1, h2, h3, h4, h5);
}

int main()
{
    test_hash();
    return 0;
}