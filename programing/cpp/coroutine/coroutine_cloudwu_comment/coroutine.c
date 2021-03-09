#include "coroutine.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)
#define DEFAULT_COROUTINE 16

struct coroutine;

// 调度器
struct schedule {
	char stack[STACK_SIZE];   // 所有协程的public stack
	ucontext_t main;          // 主线程的context
	int nco;                  // 当前启用的协程数量
	int cap;                  // 支持的协程数量
	int running;              // 协程id
	struct coroutine **co;    // 协程对象集
};

// 协程对象
struct coroutine {
	coroutine_func func;      // 每个协程的回调函数
	void *ud;                 // 每个协程的用户数据
	ucontext_t ctx;           // 每个协程的context
	struct schedule *sch;     // 每个协程从属的调度器
	ptrdiff_t cap;            // 每个协程private stack的最大分配空间
	ptrdiff_t size;           // 每个协程private stack的实际分配空间
	int status;               // 每个协程的当前运行状态
	char *stack;              // 协程的private stack
};

struct coroutine * 
_co_new(struct schedule *S , coroutine_func func, void *ud) {
	struct coroutine * co = malloc(sizeof(*co));
	co->func = func;
	co->ud = ud;
	co->sch = S;
	co->cap = 0;
	co->size = 0;
	co->status = COROUTINE_READY;
	co->stack = NULL;
	return co;
}

void
_co_delete(struct coroutine *co) {

	// 只释放协程自己动态分配的空间, 并注意释放顺序
	free(co->stack);
	free(co);
}

struct schedule * 
coroutine_open(void) {
	struct schedule *S = malloc(sizeof(*S));
	S->nco = 0;
	S->cap = DEFAULT_COROUTINE;
	S->running = -1;
	S->co = malloc(sizeof(struct coroutine *) * S->cap);
	memset(S->co, 0, sizeof(struct coroutine *) * S->cap);
	return S;
}

void 
coroutine_close(struct schedule *S) {
	int i;
	for (i = 0; i < S->cap; i++) {
		struct coroutine * co = S->co[i];
		if (co) {
			_co_delete(co);
		}
	}

	// 最后释放调度器
	free(S->co);
	S->co = NULL;
	free(S);
}

int 
coroutine_new(struct schedule *S, coroutine_func func, void *ud) {
	struct coroutine *co = _co_new(S, func , ud);
	if (S->nco >= S->cap) {
		int id = S->cap;
		S->co = realloc(S->co, S->cap * 2 * sizeof(struct coroutine *));
		memset(S->co + S->cap , 0 , sizeof(struct coroutine *) * S->cap);
		S->co[S->cap] = co;
		S->cap *= 2;
		++S->nco;
		return id;
	} else {
		int i;
		for (i = 0; i < S->cap; i++) {
			int id = (i+S->nco) % S->cap;
			if (S->co[id] == NULL) {
				S->co[id] = co;
				++S->nco;

				// 返回创建好的协程id
				return id;
			}
		}
	}
	assert(0);
	return -1;
}

static void
mainfunc(uint32_t low32, uint32_t hi32) {
	// resume param
	uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)hi32 << 32);
	struct schedule *S = (struct schedule *)ptr;
	int id = S->running;

	// debug
	printf("mainfunc: coroutine id[%d]\n", S->running);

	struct coroutine *C = S->co[id];
	C->func(S,C->ud);

	_co_delete(C);

	S->co[id] = NULL;
	--S->nco;
	S->running = -1;
}

void 
coroutine_resume(struct schedule * S, int id) {

	assert(S->running == -1);
	assert(id >= 0 && id < S->cap);

	struct coroutine *C = S->co[id];
	if (C == NULL)
		return;

	int status = C->status;
	switch(status) {

	case COROUTINE_READY:
		getcontext(&C->ctx);
		C->ctx.uc_stack.ss_sp = S->stack;       // stack top起始位置(SP)
		C->ctx.uc_stack.ss_size = STACK_SIZE;   // 用于计算stack bottom(数据从stack bottom开始存放)
		C->ctx.uc_link = &S->main;              // 协程执行完切回的context
		S->running = id;                        // 调度器记录当前准备调度的协程id
		C->status = COROUTINE_RUNNING;          // 将准备调度的协程状态置为"待运行状态"

		uintptr_t ptr = (uintptr_t)S;
		// 需要考虑跨平台指针大小不同的问题
		makecontext(&C->ctx, (void (*)(void)) mainfunc, 2, (uint32_t)ptr, (uint32_t)(ptr>>32));
		// 开始执行mainfunc回调, 执行完继续fall through, 即执行下一个协程
		swapcontext(&S->main, &C->ctx);
		// debug
		printf("COROUTINE_READY: coroutine id[%d] return\n", S->running);
		break;

	case COROUTINE_SUSPEND:
		// stack从高地址向低地址生长, 即从stack bottom向stack top存储数据
		memcpy(S->stack + STACK_SIZE - C->size, C->stack, C->size);
		S->running = id;
		C->status = COROUTINE_RUNNING;
		swapcontext(&S->main, &C->ctx);
		// debug
		sleep(2);
		printf("COROUTINE_SUSPEND: coroutine id[%d] return\n", S->running);
		break;

	default:
		assert(0);
	}

	printf("break switch: coroutine id[%d] return\n", S->running);
}

static void
_save_stack(struct coroutine *C, char *top) {

	// 在stack上创建一个局部变量, 标识当前栈顶(SP)的位置(低地址)
	char dummy  = 0;
	printf("_save_stack: &C[%p] top[%p] &dummy[%p] top - &dummy[%d] STACK_SIZE[%d]\n", &C, top, &dummy, top - &dummy, STACK_SIZE);

	// 检查stack是否有溢出
	assert(top - &dummy <= STACK_SIZE);

	// 按需保存当前协程的stack (begin)
	// 判断协程栈的空间是否足够, 若不够则重新分配
	if (C->cap < top - &dummy) {
		free(C->stack);
		C->cap = top - &dummy;
		C->stack = malloc(C->cap);
	}
	C->size = top - &dummy;
	memcpy(C->stack, &dummy, C->size);
	// 按需保存当前协程的stack (end)
}

void
coroutine_yield(struct schedule * S) {

	int id = S->running;
	// debug
	printf("coroutine_yield: coroutine id[%d] into\n", S->running);
	assert(id >= 0);

    // 注意makecontext()时指定ss_sp为S->stack, 当C->ctx执行时, 协程的栈是存储在S->stack上的, 即把堆上分配的一块空间虚拟成栈来使用
	struct coroutine * C = S->co[id];

	// 与栈顶S->stack的位置进行比较
    printf("coroutine_yield: &C[%p] S->stack[%p]\n", &C, S->stack);
	assert((char *)&C > S->stack);

	// 按需动态保存协程的private stack
	_save_stack(C, S->stack + STACK_SIZE);

	C->status = COROUTINE_SUSPEND;
	S->running = -1;

	swapcontext(&C->ctx, &S->main);
}

int 
coroutine_status(struct schedule * S, int id) {
	assert(id >= 0 && id < S->cap);
	if (S->co[id] == NULL) {
		return COROUTINE_DEAD;
	}
	return S->co[id]->status;
}

int 
coroutine_running(struct schedule * S) {
	return S->running;
}
