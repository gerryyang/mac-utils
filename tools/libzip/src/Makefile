include ./Inc.mk

DEMO_OBJS = demo.o
DEMO = demo

MC_OBJS = midas_comment.o
MC = midas_comment

BINS = $(DEMO) $(MC)

#CFLAGS += -Wno-deprecated-declarations

all: $(BINS)

$(DEMO): $(DEMO_OBJS)
	$(CXX) -o $@ $^ $(LIBPATH) $(LIBS)
	@echo "build $(DEMO) OK!"

$(MC): $(MC_OBJS)
	$(CXX) -o $@ $^ $(LIBPATH) $(LIBS)
	@echo "build $(MC) OK!"

install:
	@echo "nothing to install"

clean:
	-rm -f $(DEMO_OBJS) $(MC_OBJS) $(BINS)

