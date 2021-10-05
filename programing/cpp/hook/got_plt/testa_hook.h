#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void say_hello_hook();

    void* malloc_hook(size_t size);

#ifdef __cplusplus
}; /* extern "C" */
#endif
