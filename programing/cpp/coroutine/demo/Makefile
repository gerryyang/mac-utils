
BIN = demo_loop_simple demo_loop_setcontext

all: $(BIN) 

demo_loop_simple: demo_loop_simple.c
	gcc -g -Wall -D_XOPEN_SOURCE -Wno-deprecated-declarations -o $@ $^

demo_loop_setcontext: demo_loop_setcontext.c
	gcc -g -Wall -D_XOPEN_SOURCE -Wno-deprecated-declarations -o $@ $^

clean:
	-rm $(BIN) 
