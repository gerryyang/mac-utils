#include "coroutine.h"
#include <stdio.h>

// 每个协程的用户数据
struct args {
	int n;
};

static void
foo(struct schedule * S, void *ud) {
	struct args * arg = ud;
	int start = arg->n;
	int i;
	for (i = 0; i < 5; i++) {
		printf("coroutine %d : %d\n", coroutine_running(S), start + i);
		coroutine_yield(S);
	}
}

static void
test(struct schedule *S) {
	struct args arg1 = { 0 };
	struct args arg2 = { 100 };

	int co1 = coroutine_new(S, foo, &arg1);
	int co2 = coroutine_new(S, foo, &arg2);
	printf("main start\n");
	while (coroutine_status(S,co1) && coroutine_status(S,co2)) {
		coroutine_resume(S,co1);
		coroutine_resume(S,co2);
	} 
	printf("main end\n");
}

int 
main() {
	struct schedule * S = coroutine_open();
	test(S);
	coroutine_close(S);
	
	return 0;
}
/*
output:

MAC OSX 64: (error)
gerryyang@mba:coroutine_cloudwu$./main
main start
mainfunc: coroutine id[0]
coroutine 0 : 0
COROUTINE_READY: coroutine id[-1] return
break switch: coroutine id[-1] return
mainfunc: coroutine id[1]
coroutine 1 : 100
COROUTINE_READY: coroutine id[-1] return
break switch: coroutine id[-1] return

COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
^C

LINUX SUSE 32: (ok)
main start
mainfunc: coroutine id[0]
coroutine 0 : 0
coroutine_yield: coroutine id[0] into
coroutine_yield: &C[0xb7df4f9c] S->stack[0xb7cf5008]
_save_stack: &C[0xb7df4f7c] top[0xb7df5008] &dummy[0xb7df4f67] top - &dummy[161] STACK_SIZE[1048576]
COROUTINE_READY: coroutine id[-1] return
break switch: coroutine id[-1] return
mainfunc: coroutine id[1]
coroutine 1 : 100
coroutine_yield: coroutine id[1] into
coroutine_yield: &C[0xb7df4f9c] S->stack[0xb7cf5008]
_save_stack: &C[0xb7df4f7c] top[0xb7df5008] &dummy[0xb7df4f67] top - &dummy[161] STACK_SIZE[1048576]
COROUTINE_READY: coroutine id[-1] return
break switch: coroutine id[-1] return
coroutine 0 : 1
coroutine_yield: coroutine id[0] into
coroutine_yield: &C[0xb7df4f9c] S->stack[0xb7cf5008]
_save_stack: &C[0xb7df4f7c] top[0xb7df5008] &dummy[0xb7df4f67] top - &dummy[161] STACK_SIZE[1048576]
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
coroutine 1 : 101
coroutine_yield: coroutine id[1] into
coroutine_yield: &C[0xb7df4f9c] S->stack[0xb7cf5008]
_save_stack: &C[0xb7df4f7c] top[0xb7df5008] &dummy[0xb7df4f67] top - &dummy[161] STACK_SIZE[1048576]
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
coroutine 0 : 2
coroutine_yield: coroutine id[0] into
coroutine_yield: &C[0xb7df4f9c] S->stack[0xb7cf5008]
_save_stack: &C[0xb7df4f7c] top[0xb7df5008] &dummy[0xb7df4f67] top - &dummy[161] STACK_SIZE[1048576]
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
coroutine 1 : 102
coroutine_yield: coroutine id[1] into
coroutine_yield: &C[0xb7df4f9c] S->stack[0xb7cf5008]
_save_stack: &C[0xb7df4f7c] top[0xb7df5008] &dummy[0xb7df4f67] top - &dummy[161] STACK_SIZE[1048576]
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
coroutine 0 : 3
coroutine_yield: coroutine id[0] into
coroutine_yield: &C[0xb7df4f9c] S->stack[0xb7cf5008]
_save_stack: &C[0xb7df4f7c] top[0xb7df5008] &dummy[0xb7df4f67] top - &dummy[161] STACK_SIZE[1048576]
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
coroutine 1 : 103
coroutine_yield: coroutine id[1] into
coroutine_yield: &C[0xb7df4f9c] S->stack[0xb7cf5008]
_save_stack: &C[0xb7df4f7c] top[0xb7df5008] &dummy[0xb7df4f67] top - &dummy[161] STACK_SIZE[1048576]
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
coroutine 0 : 4
coroutine_yield: coroutine id[0] into
coroutine_yield: &C[0xb7df4f9c] S->stack[0xb7cf5008]
_save_stack: &C[0xb7df4f7c] top[0xb7df5008] &dummy[0xb7df4f67] top - &dummy[161] STACK_SIZE[1048576]
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
coroutine 1 : 104
coroutine_yield: coroutine id[1] into
coroutine_yield: &C[0xb7df4f9c] S->stack[0xb7cf5008]
_save_stack: &C[0xb7df4f7c] top[0xb7df5008] &dummy[0xb7df4f67] top - &dummy[161] STACK_SIZE[1048576]
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
COROUTINE_SUSPEND: coroutine id[-1] return
break switch: coroutine id[-1] return
main end

*/

