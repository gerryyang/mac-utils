
XMLTEST = xmltest
DEMO = demo 

BINS = $(XMLTEST) $(DEMO)

all: $(BINS)

$(XMLTEST):
	g++ -o $(XMLTEST) xmltest.cpp tinyxml2.cpp
	@echo "make $(XMLTEST) ok"

$(DEMO):
	g++ -o $(DEMO) demo.cpp tinyxml2.cpp
	@echo "make $(DEMO) ok"

clean:
	-rm -f $(BINS)


