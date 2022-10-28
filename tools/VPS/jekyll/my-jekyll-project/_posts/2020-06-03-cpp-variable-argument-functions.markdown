---
layout: post
title:  "CPP Variable Argument Functions"
date:   2020-05-28 20:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{: toc}

实现一个变参的函数，可能的方式有：

* 函数参数是一个指向数组的指针
* 实现一个类似[printf](http://www.cplusplus.com/reference/cstdio/printf/)

``` cpp
int printf ( const char * format, ... );
```

* C++11的变参模版(variadic templates)



# stdarg 变参实现原理

Whenever a function is declared to have an indeterminate number of arguments, in place of the last argument you should place an `ellipsis` (which looks like '...'), so, `int a_function ( int x, ... );` would tell the compiler the function should accept however many arguments that the programmer uses, as long as it is equal to at least one, the one being the first, x.

We'll need to use some `macros` (which work much like functions, and you can treat them as such) from the **stdarg.h** header file to extract the values stored in the variable argument list--`va_start`, which initializes the list, `va_arg`, which returns the next argument in the list, and `va_end`, which cleans up the variable argument list.

To use these functions, we need a variable capable of storing a variable-length argument list--this variable will be of **type** `va_list`. `va_list` is like any other type. For example, the following code declares a list that can be used to store a variable number of arguments.

``` cpp
va_list a_list;
```

`va_start` is a macro which **accepts two arguments**, a va_list and the name of the variable that directly precedes the ellipsis ("..."). So in the function a_function, to initialize a_list with va_start, you would write `va_start ( a_list, x )`;

``` cpp
int a_function ( int x, ... )
{
    va_list a_list;
    va_start( a_list, x );
}
```

`va_arg` takes a va_list and a variable type, and returns the next argument in the list in the form of whatever variable type it is told. It then moves down the list to the next argument. For example, `va_arg ( a_list, double )` will return the next argument, assuming it exists, in the form of a double. The next time it is called, it will return the argument following the last returned number, if one exists. Note that you need to know the type of each argument--that's part of why printf requires a format string! Once you're done, use `va_end` to clean up the list: `va_end( a_list )`;

# Example 1

To show how each of the parts works, take an example function:

``` cpp
#include <stdarg.h>
#include <stdio.h>

/* this function will take the number of values to average
   followed by all of the numbers to average */
double average ( int num, ... )
{
    va_list arguments;
    double sum = 0;

    /* Initializing arguments to store all values after num */
    va_start ( arguments, num );

    /* Sum all the inputs; we still rely on the function caller to tell us how
     * many there are */
    for ( int x = 0; x < num; x++ )
    {
        sum += va_arg ( arguments, double ); // If the next argument is not of the specified type, the behaviour is undefined
                                             // The behaviour is also undefined if va_arg is called when there were no further arguments
    }

    va_end ( arguments );                  // Cleans up the list
                                           // If va_end is not used, the behaviour is undefined
    return sum / num;
}

int main()
{
    /* this computes the average of 13.2, 22.3 and 4.5 (3 indicates the number of values to average) */
    printf( "%f\n", average ( 3, 12.2, 22.3, 4.5 ) );
    /* here it computes the average of the 5 values 3.3, 2.2, 1.1, 5.5 and 3.3 */
    printf( "%f\n", average ( 5, 3.3, 2.2, 1.1, 5.5, 3.3 ) );

    return 0;
}
```

It isn't necessarily a good idea to use a variable argument list at all times; the potential exists for assuming a value is of one type, while it is in fact another, such as a null pointer being assumed to be an integer. Consequently, variable argument lists should be used sparingly.

``` cpp
#include <iostream>
#include <stdarg.h>

int MyPrintf (const char *fmt, ...)
{
  va_list arg;
  int done;

  va_start (arg, fmt);
  done = vfprintf (stdout, fmt, arg);
  va_end (arg);

  return done;
}

int main()
{
    std::cout << sizeof(va_list) << std::endl; // 24
    int a = 1;
    MyPrintf("hello %d\n", a); // hello 1
}
```

# Example 2

The following example shows the use of va_start, va_arg, and va_end to implement a function that returns the biggest of its integer arguments.

``` cpp
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

int maxof(int, ...) ;
void f(void);

int main()
{
        f();
        exit(EXIT_SUCCESS);
}

int maxof(int n_args, ...)
{
        register int i;
        int max, a;
        va_list ap;

        va_start(ap, n_args);
        max = va_arg(ap, int);
        for(i = 2; i <= n_args; i++) {
                if((a = va_arg(ap, int)) > max)
                        max = a;
        }
        va_end(ap);

        return max;
}

void f(void) {
        int i = 5;
        int j[256];
        j[42] = 24;
        printf("%d\n", maxof(3, i, j[42], 0));
}
```

# Example 3

The function foo takes a string of format characters and prints out the argument associated with each format character based on the type.

``` cpp
#include <stdio.h>
#include <stdarg.h>

void
foo(char *fmt, ...)
{
    va_list ap;
    int d;
    char c, *s;

   va_start(ap, fmt);
    while (*fmt)
        switch (*fmt++) {
        case 's':              /* string */
            s = va_arg(ap, char *);
            printf("string %s\n", s);
            break;
        case 'd':              /* int */
            d = va_arg(ap, int);
            printf("int %d\n", d);
            break;
        case 'c':              /* char */
            /* need a cast here since va_arg only
               takes fully promoted types */
            c = (char) va_arg(ap, int);
            printf("char %c\n", c);
            break;
        }
    va_end(ap);
}
```

# Example 4

``` cpp
#include<stdio.h>
#include<stdarg.h>

void Myprintf(char *, ...);              //Our printf function
char* convert(unsigned int, int);       //Convert integer number into octal, hex, etc.

int main()
{
    Myprintf("hello world\n %d", 9);
    return 0;
}

void Myprintf(char* format, ...)
{
    char *traverse;
    unsigned int i;
    char *s;

    //Module 1: Initializing Myprintf's arguments
    va_list arg;
    va_start(arg, format);

    for (traverse = format; *traverse != '\0'; traverse++)
    {
        while ( *traverse != '%' )
        {
            putchar(*traverse);
            traverse++;
        }

        traverse++;

        //Module 2: Fetching and executing arguments
        switch (*traverse)
        {
            case 'c' : i = va_arg(arg, int);     //Fetch char argument
                        putchar(i);
                        break;

            case 'd' : i = va_arg(arg, int);     //Fetch Decimal/Integer argument
                        if (i < 0)
                        {
                            i = -i;
                            putchar('-');
                        }
                        puts(convert(i, 10));
                        break;

            case 'o': i = va_arg(arg,unsigned int); //Fetch Octal representation
                        puts(convert(i, 8));
                        break;

            case 's': s = va_arg(arg, char *);      //Fetch string
                        puts(s);
                        break;

            case 'x': i = va_arg(arg, unsigned int); //Fetch Hexadecimal representation
                        puts(convert(i, 16));
                        break;
        }
    }

    //Module 3: Closing argument list to necessary clean-up
    va_end(arg);
}

char *convert(unsigned int num, int base)
{
    static char representation[]= "0123456789ABCDEF";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do {
        *--ptr = representation[num % base];
        num /= base;
    } while(num != 0);

    return(ptr);
}
```


# stdarg 用法总结

In order to access the arguments within the called function, the functions declared in the `<stdarg.h>` header file must be included. This introduces a new type, called a `va_list`, and three functions that operate on objects of this type, called `va_start`, `va_arg`, and `va_end`.

``` cpp
#include <stdarg.h>

void va_start(va_list ap, parmN);

type va_arg(va_list ap, type);

void va_end(va list ap);
```

# 问题示例

If there is no next argument, or if type is not compatible with the type of the actual next argument (as promoted according to the default argument promotions), random errors will occur.

``` cpp
#include <cstdio>

int main()
{
        char str[] = "aaaaaaaaaaaaaaaaaaaa";
        printf("%s %s\n", str);
}
```

可以验证异常情况，出现乱码或者core dumped。


# GNU libc printf 源码

* [printf](http://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/printf.c;h=4c8f3a2a0c38ab27a2eed4d2ff3b804980aa8f9f;hb=HEAD)
* [vfprintf](http://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/vfprintf.c;h=6e0e85cd7cca9f4dfc9e86fb702db131ab2e1639;hb=HEAD)

# C++11 Variadic Templates

``` cpp
template <typename ... Ts>
void format_string(char *fmt, Ts ... ts) {}

template <typename ... Ts>
void debug_print(int dbg_lvl, char *fmt, Ts ... ts)
{
  format_string(fmt, ts...);
}
```

# 在变参函数中传递变参参数

``` cpp
void func2(const char *fmt, ...)
{
        va_list argptr;
        va_start(argptr, fmt);
        printf("fmt(%s)\n", fmt);
        va_end(argptr);
}

void func1(const char *fmt, ...)
{
        va_list argptr;
        va_start(argptr, fmt);
        printf("fmt(%s)\n", fmt);
        func2(fmt, argptr);
        va_end(argptr);
}

int main()
{
        int a = 1;
        char b = '2';
        func1("a=%d&b=%c", a, &b);
}
```

## Variadic Macros (可变参宏)

https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

``` cpp
#ifdef DEBUG_THRU_UART0
#   define DEBUG(...)  printString (__VA_ARGS__)
#else
void dummyFunc(void);
#   define DEBUG(...)  dummyFunc()
#endif
DEBUG(1,2,3); //calls printString(1,2,3) or dummyFunc() depending on
              //-DDEBUG_THRU_UART0 compiler define was given or not, when compiling.
```

`##__VA_ARGS__`的作用？

``` cpp
#define FOO(...)       printf(__VA_ARGS__)
#define BAR(fmt, ...)  printf(fmt, __VA_ARGS__)

FOO("this works fine");
BAR("this breaks!");
```

* https://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick



``` cpp
#include <cstdio>
#include <string.h>

#define LOG_INNER(format, ...) \
do \
{ \
        char szBuf[1024]; \
        snprintf(szBuf, sizeof(szBuf), "[%s:%s:%d]" format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        FILE* __fp = fopen("./log_inner.log", "a+"); \
        fwrite(szBuf, strlen(szBuf), 1, __fp); \
        fclose(__fp); \
} while (0)

#define LOG(...) \
do \
{ \
        LOG_INNER("inner: " __VA_ARGS__); \
} while (0)

int main()
{
    //LOG_INNER("hi %s\n", "gerry");
    LOG("hi %s\n", "gerry");
}
```


# Refer

* [Variable Argument Lists in C using va_list](https://www.cprogramming.com/tutorial/c/lesson17.html)
* [Variable numbers of arguments](https://publications.gbdirect.co.uk/c_book/chapter9/stdarg.html)
* [Formatted I/O](https://publications.gbdirect.co.uk/c_book/chapter9/formatted_io.html)
* [How to write my own printf() in C?](https://stackoverflow.com/questions/1735236/how-to-write-my-own-printf-in-c)
* [va_start(3) - Linux man page](https://linux.die.net/man/3/va_start)
* [An example of use of varargs in C](https://stackoverflow.com/questions/15784729/an-example-of-use-of-varargs-in-c)
* [Passing variable number of arguments around](https://stackoverflow.com/questions/205529/passing-variable-number-of-arguments-around)


