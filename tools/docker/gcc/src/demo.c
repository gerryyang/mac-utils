#include <stdio.h>
#include <unistd.h>

void say(const char * dummy)
{
	for (;;)
	{
		printf("[%s] hello docker, from gcc image\n", dummy);
		sleep(1);
	}
}

int main(void)
{
	pid_t childPid;
	switch (childPid = fork()) {
		case -1:
			break;
		case 0:
			say("child");
			break;
		default:
			say("parent");
			break;
	}
	return 0;
}
