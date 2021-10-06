#include <stdlib.h>
#include <string.h>

int gethostname(char *name, size_t len)
{
    char *p = getenv("FAKE_HOSTNAME");
    if (p == NULL)
    {
        p = "localhost";
    }
    strncpy(name, p, len - 1);
    name[len - 1] = '\0';

    return 0;
}
