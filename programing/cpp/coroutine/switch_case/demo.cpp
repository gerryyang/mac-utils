#include <stdio.h>

int main() 
{
	int state = 1;
	switch (state) {
	case 1:
		printf("case 1\n");
	case 2:
		printf("case 2\n");
	}
	printf("out switch-case\n");

	return 0;
}

