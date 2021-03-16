#include <stdio.h>

/*
|   1   |   2   |   3   |   4   |  

| AA(1) | pad.................. |
| BB(1) | BB(2) | BB(3) | BB(4) | 
| CC(1) | pad.................. |
*/
struct Test
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

int main()
{
        printf("sizeof(Test)=%lu\n", sizeof(Test));
        printf("sizeof(Test2)=%lu\n", sizeof(Test2));
        printf("sizeof(Test3)=%lu\n", sizeof(Test3));
        printf("sizeof(Test3)=%lu\n", sizeof(Test4));
}
/*
sizeof(Test)=12
sizeof(Test2)=6
sizeof(Test3)=8
sizeof(Test3)=6
*/
