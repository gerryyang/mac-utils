
XMLTEST = xmltest

BINS = $(XMLTEST)

all: $(BINS)

$(XMLTEST):
	g++ -o $(XMLTEST) xmltest.cpp tinyxml2.cpp
	@echo "make $(XMLTEST) ok"

clean:
	-rm -f $(BINS)


