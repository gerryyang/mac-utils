include ./Inc.mk

FIRST_OBJS = first.o
FIRST = first

BINS = $(FIRST) 

CFLAGS += -Wno-unused-variable -Wno-unused-function 
all: $(BINS)

$(FIRST): $(FIRST_OBJS)
	$(CXX) -o $@ $^ $(LIBPATH) $(LIBS)
	@echo "build $(FIRST) OK!"


install:
	@echo "nothing to install"

clean:
	-rm -f $(FIRST_OBJS) $(BINS)

