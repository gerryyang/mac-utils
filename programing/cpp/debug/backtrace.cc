#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BT_BUF_SIZE 100

void myfunc3()
{
    int nptrs;
    void *buffer[BT_BUF_SIZE];
    char **strings;

    nptrs = backtrace(buffer, BT_BUF_SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

    //--nptrs;
    //memmove(buffer, buffer + 1, sizeof(void*) * nptrs);

    for (int i = 0; i < nptrs; ++i)
    {
        printf("%s\n", (char *)buffer[i]);
    }

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
              would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL)
    {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < nptrs; ++j) printf("%s\n", strings[j]);

    free(strings);
}

/* "static" means don't export the symbol... */
static void myfunc2()
{
    myfunc3();
}

void myfunc(int ncalls)
{
    if (ncalls > 1)
        myfunc(ncalls - 1);
    else
        myfunc2();
}

int main(int argc, char *argv[])
{
    /*if (argc != 2) {
        fprintf(stderr, "%s num-calls\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    myfunc(atoi(argv[1]));*/

    myfunc(3);
    exit(EXIT_SUCCESS);
}
