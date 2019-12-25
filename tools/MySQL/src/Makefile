include Inc.mk

LIB_PATH = .
LIB_DB   = $(LIB_PATH)/libmydb.a 

INCLUDE += -I./ 

BIN = resume_refund_tool_x86_64

all: $(LIB_DB) $(BIN)

OBJ_LIMIT = dbbase.o db_handle.o

$(BIN) : resume_refund_tool.o
	$(CXX) -o $@ $^ -L. -lmydb $(LIBPATH) $(LIBS) 

$(LIB_DB): $(OBJ_LIMIT) 
	$(AR) $(AROPT) $@ $?
	$(RANLIB) $@

install:
	echo "install. do nothing..."

clean: 
	@rm *.o $(LIB_DB)


