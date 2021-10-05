#include "testb_hook.h"
#include "testb.h"
#include <stdio.h>

void say_world_hook()
{
    printf("say_world_hook ...\n");

    say_world();

    printf("call orig func\n");
}
