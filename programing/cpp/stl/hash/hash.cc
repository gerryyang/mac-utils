// functional header
// for hash<class template> class
#include <functional>
#include <iostream>
#include <string>

int main()
{
    // Get the string to get its hash value
    std::string hashing = "Geeks";

    // Instantiation of Object
    std::hash<std::string> mystdhash;

    // Using operator() to get hash value
    std::cout << "String hash values: " << mystdhash(hashing) << std::endl;

    int hashing2 = 12345;
    std::hash<int> mystdhash2;
    std::cout << "Int hash values: " << mystdhash2(hashing2) << std::endl;
}
/*
String hash values: 4457761756728957899
Int hash values: 12345
*/