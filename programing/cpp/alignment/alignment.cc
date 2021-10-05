#include <stdio.h>

/*
|   1   |   2   |   3   |   4   |

| AA(1) | pad.................. |
| BB(1) | BB(2) | BB(3) | BB(4) |
| CC(1) | pad.................. |
*/
struct Test1
{
    char AA;
    int BB;
    char CC;
};

/*
|   1   |

| AA(1) |
| BB(1) |
| BB(2) |
| BB(3) |
| BB(4) |
| CC(1) |
*/
#pragma pack(push, 1)
struct Test2
{
    char AA;
    int BB;
    char CC;
};
#pragma pack(pop)

/*
|   1   |   2   |

| AA(1) | pad.. |
| BB(1) | BB(2) |
| BB(3) | BB(4) |
| CC(1) | pad.. |
*/
#pragma pack(push, 2)
struct Test3
{
    char AA;
    int BB;
    char CC;
};
#pragma pack(pop)

/*
|   1   |   2   |

| BB(1) | BB(2) |
| BB(3) | BB(4) |
| AA(1) | CC(1) |
*/
#pragma pack(push, 2)
struct Test4
{
    int BB;
    char AA;
    char CC;
};
#pragma pack(pop)

/* define simple structure */
struct
{
    unsigned int widthValidated;
    unsigned int heightValidated;
} status1;

/* define a structure with bit fields */
struct
{
    unsigned int widthValidated : 1;
    unsigned int heightValidated : 1;
} status2;

struct
{
    unsigned int age : 3;
} Age;

int main()
{
    printf("sizeof(Test1)=%lu\n", sizeof(Test1));
    printf("sizeof(Test2)=%lu\n", sizeof(Test2));
    printf("sizeof(Test3)=%lu\n", sizeof(Test3));
    printf("sizeof(Test4)=%lu\n", sizeof(Test4));

    printf("Memory size occupied by status1 : %lu\n", sizeof(status1));
    printf("Memory size occupied by status2 : %lu\n", sizeof(status2));

    // The above structure definition instructs the C compiler that the age variable is going to use only 3 bits to store the value.
    // If you try to use more than 3 bits, then it will not allow you to do so.
    Age.age = 4;
    printf("Sizeof( Age ) : %lu\n", sizeof(Age));
    printf("Age.age : %d\n", Age.age);

    Age.age = 7;
    printf("Age.age : %d\n", Age.age);

    Age.age = 8;  // warning: large integer implicitly truncated to unsigned type [-Woverflow]
    printf("Age.age : %d\n", Age.age);
}
/*
sizeof(Test1)=12
sizeof(Test2)=6
sizeof(Test3)=8
sizeof(Test4)=6
Memory size occupied by status1 : 8
Memory size occupied by status2 : 4
Sizeof( Age ) : 4
Age.age : 4
Age.age : 7
Age.age : 0
*/
