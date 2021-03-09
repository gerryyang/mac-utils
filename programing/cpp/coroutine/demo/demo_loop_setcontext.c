/* http://en.wikipedia.org/wiki/Setcontext

NOTE: this example is not consistent with the manual page or the specification.[1] The function makecontext requires additional parameters to be type int, but the example passes pointers. Thus, the example may fail on 64-bit machines (specifically LP64-architectures, where sizeof(void*) > sizeof(int)). This problem can be worked around by breaking up and reconstructing 64-bit values, but that introduces a performance penalty.

"On architectures where int and pointer types are the same size (e.g., x86-32, where both types are 32 bits), you may be able to get away with passing pointers as arguments to makecontext() following argc. However, doing this is not guaranteed to be portable, is undefined according to the standards, and won't work on architectures where pointers are larger than ints. Nevertheless, starting with version 2.8, glibc makes some changes to makecontext(3), to permit this on some 64-bit architectures (e.g., x86-64)."
*/

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

/* The three contexts:
 *    (1) main_context1 : The point in main to which loop will return.
 *    (2) main_context2 : The point in main to which control from loop will
 *                        flow by switching contexts.
 *    (3) loop_context  : The point in loop to which control from main will
 *                        flow by switching contexts. */
ucontext_t main_context1, main_context2, loop_context;

/* The iterator return value. */
volatile int i_from_iterator;

/* This is the iterator function. It is entered on the first call to
 * swapcontext, and loops from 0 to 9. Each value is saved in i_from_iterator,
 * and then swapcontext used to return to the main loop.  The main loop prints
 * the value and calls swapcontext to swap back into the function. When the end
 * of the loop is reached, the function exits, and execution switches to the
 * context pointed to by main_context1. */
void loop(
		ucontext_t *loop_context,
		ucontext_t *other_context,
		int *i_from_iterator)
{
	printf("---4\n");
	int i;

	for (i=0; i < 10; ++i) {
		/* Write the loop counter into the iterator return location. */
		*i_from_iterator = i;

		/* Save the loop context (this point in the code) into ''loop_context'',
		 * and switch to other_context. */
		swapcontext(loop_context, other_context);
	}

	/* The function falls through to the calling context with an implicit
	 * ''setcontext(&loop_context->uc_link);'' */
} 

int main(void)
{
	/* The stack for the iterator function. */
	char iterator_stack[SIGSTKSZ];

	/* Flag indicating that the iterator has completed. */
	volatile int iterator_finished;

	getcontext(&loop_context);
	/* Initialise the iterator context. uc_link points to main_context1, the
	 * point to return to when the iterator finishes. */
	loop_context.uc_link          = &main_context1;
	loop_context.uc_stack.ss_sp   = iterator_stack;
	loop_context.uc_stack.ss_size = sizeof(iterator_stack);

	/* Fill in loop_context so that it makes swapcontext start loop. The
	 * (void (*)(void)) typecast is to avoid a compiler warning but it is
	 * not relevant to the behaviour of the function. */
	makecontext(&loop_context, (void (*)(void)) loop,
			3, &loop_context, &main_context2, &i_from_iterator);

	/* Clear the finished flag. */      
	iterator_finished = 0;

	printf("---1\n");
	/* Save the current context into main_context1. When loop is finished,
	 * control flow will return to this point. */
	getcontext(&main_context1);
	printf("---2\n");

	if (!iterator_finished) {
		/* Set iterator_finished so that when the previous getcontext is
		 * returned to via uc_link, the above if condition is false and the
		 * iterator is not restarted. */
		iterator_finished = 1;
		printf("---3\n");

		while (1) {
			/* Save this point into main_context2 and switch into the iterator.
			 * The first call will begin loop.  Subsequent calls will switch to
			 * the swapcontext in loop. */
			swapcontext(&main_context2, &loop_context);
			printf("---5\n");
			printf("i_from_iterator[%d]\n", i_from_iterator);
		}
	}

	return 0;
}

/*
   gcc -o demo_loop_setcontext demo_loop_setcontext.c -D_XOPEN_SOURCE -Wno-deprecated-declarations

output:
gerryyang@mba:demo$./demo_loop_setcontext
---1
---2
---3
---4
---5
i_from_iterator[0]
---5
i_from_iterator[1]
---5
i_from_iterator[2]
---5
i_from_iterator[3]
---5
i_from_iterator[4]
---5
i_from_iterator[5]
---5
i_from_iterator[6]
---5
i_from_iterator[7]
---5
i_from_iterator[8]
---5
i_from_iterator[9]
---2
*/


