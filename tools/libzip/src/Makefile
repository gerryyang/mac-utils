include ./Inc.mk

DEMO_OBJS = demo.o
DEMO = demo

BINS = $(DEMO) 

#CFLAGS += -Wno-deprecated-declarations

all: $(BINS)

$(DEMO): $(DEMO_OBJS)
	$(CXX) -o $@ $^ $(LIBPATH) $(LIBS)
	@echo "build $(DEMO) OK!"

install:
	@echo "nothing to install"

clean:
	-rm -f $(DEMO_OBJS) $(BINS)

