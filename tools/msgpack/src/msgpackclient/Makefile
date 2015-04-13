include ./Inc.mk

CLIENT_OBJS = client.o
CLIENT = client

BINS = $(CLIENT) 

CFLAGS += -Wno-unused-variable -Wno-unused-function 
all: $(BINS)

$(CLIENT): $(CLIENT_OBJS)
	$(CXX) -o $@ $^ $(LIBPATH) $(LIBS)
	@echo "build $(CLIENT) OK!"


install:
	@echo "nothing to install"

clean:
	-rm -f $(CLIENT_OBJS) $(BINS)

