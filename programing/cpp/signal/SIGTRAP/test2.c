#include <stdio.h>

int main()
{
    int a = 10;
    int b = 0;

    printf("Dividing by zero...\n");
    int result = a / b;
    printf("Result: %d\n", result);

    return 0;
}