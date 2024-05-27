#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

int main()
{
    std::vector<std::string> strings = {"Geeks", "for", "Geeks", "A", "Computer", "Science", "Portal", "for", "Geeks"};

    std::vector<int> ints = {12345, 67890, 111213, 141516, 171819, 202122, 232425};

    std::unordered_map<size_t, int> string_bucket_count;
    std::unordered_map<size_t, int> int_bucket_count;

    size_t num_buckets = 10;

    std::hash<std::string> string_hash;
    std::hash<int> int_hash;

    for (const auto& str : strings)
    {
        size_t hash_value = string_hash(str) % num_buckets;
        ++string_bucket_count[hash_value];
    }

    for (const auto& num : ints)
    {
        size_t hash_value = int_hash(num) % num_buckets;
        ++int_bucket_count[hash_value];
    }

    std::cout << "String hash bucket counts:" << std::endl;
    for (const auto& bucket : string_bucket_count)
    {
        std::cout << "Bucket " << bucket.first << ": " << bucket.second << std::endl;
    }

    std::cout << "Int hash bucket counts:" << std::endl;
    for (const auto& bucket : int_bucket_count)
    {
        std::cout << "Bucket " << bucket.first << ": " << bucket.second << std::endl;
    }
}