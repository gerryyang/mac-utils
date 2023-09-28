#include <stdio.h>

extern int duplicate;

int run() {
	duplicate = 'd';
	printf("dynamic sees duplicate from libdep as:  %c\n", duplicate);
	printf("but libdep sees duplicate from main as: %c\n", get_duplicate());
	return 0;
}
