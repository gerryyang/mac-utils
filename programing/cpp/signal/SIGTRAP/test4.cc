#include <stdio.h>
#include <string>

std::string func()
{
    printf("Name!");
}

int main()
{
    printf("Triggering SIGTRAP signal...\n");
    func();

    return 0;
}