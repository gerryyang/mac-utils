#include <functional>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>

int main()
{
    // Generate 4000 8-digit numbers
    std::vector<int> ints(4000);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(10000000, 99999999);

    for (int i = 0; i < 4000; ++i)
    {
        ints[i] = dis(gen);
    }

    std::unordered_map<size_t, int> int_bucket_count;

    size_t num_buckets = 10;

    std::hash<int> int_hash;

    for (const auto& num : ints)
    {
        size_t hash_value = int_hash(num) % num_buckets;
        ++int_bucket_count[hash_value];
    }

    std::cout << "Int hash bucket counts:" << std::endl;
    for (const auto& bucket : int_bucket_count)
    {
        std::cout << "Bucket " << bucket.first << ": " << bucket.second << std::endl;
    }
}