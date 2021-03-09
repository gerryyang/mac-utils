/*
 * This makes an infinite loop because context holds the program counter.
 */

#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>

int main(int argc, const char *argv[]){
	ucontext_t context;

	getcontext(&context);
	puts("Hello world");
	sleep(1);
	setcontext(&context);

	return 0;
}
/*
   gcc -o demo_loop_simple demo_loop_simple.c -D_XOPEN_SOURCE -Wno-deprecated-declarations

*/
