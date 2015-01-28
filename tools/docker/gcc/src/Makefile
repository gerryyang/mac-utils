BIN = hello 

all: $(BIN)

$(BIN): hello.c
	gcc -g -Wall -o $@ $^

clean:
	-rm $(BIN)
