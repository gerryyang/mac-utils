// have ten successive calls to the function return the numbers 0 through 9. 
#include <stdio.h>

int function(void) 
{
	static int i = 0;
	for (; i < 10; ) {
		return i++;
	}

	return 0;
}

int function2(void) {
	static int i, state = 0;
	switch (state) {
	case 0: goto LABEL0;
	case 1: goto LABEL1;
	}
LABEL0: /* start of function */
	for (i = 0; i < 10; i++) {
		state = 1; /* so we will come back to LABEL1 */
		return i;
LABEL1:; /* resume control straight after the return */
	}

	return -1;
}

int function3(void) 
{
	static int i, state = 0;
	switch (state) {
	case 0: /* start of function */
		for (i = 0; i < 10; i++) {
			state = 1; /* so we will come back to "case 1" */
			return i;
		case 1:; /* resume control straight after the return */
		}
	}

	return -1;
}

int function4(void) 
{
	static int i, state = 0;
	switch (state) {
	case 0: /* start of function */
		for (i = 0; i < 10; i++) {
			state = __LINE__ + 2; /* so we will come back to "case __LINE__" */
			return i;
		case __LINE__:; /* resume control straight after the return */
		}
	}

	return -1;
}

#define Begin() static int state=0; switch(state) { case 0:
#define Yield(x) do { state=__LINE__; return x; case __LINE__:; } while (0)
#define End() }
int function5(void) 
{
	static int i;
	Begin();
	for (i = 0; i < 10; i++)
		Yield(i);
	End();

	return -1;
}

int main(int argc, char *argv[])
{
	printf("function() return [%d]\n", function());
	printf("function() return [%d]\n", function());

	printf("function2() return [%d]\n", function2());
	printf("function2() return [%d]\n", function2());

	printf("function3() return [%d]\n", function3());
	printf("function3() return [%d]\n", function3());

	printf("function4() return [%d]\n", function4());
	printf("function4() return [%d]\n", function4());

	printf("function5() return [%d]\n", function5());
	printf("function5() return [%d]\n", function5());

	return 0;
}
