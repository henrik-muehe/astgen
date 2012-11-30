GREG?=../greg-cpp/greg
CXX?=g++
CTEMPLATE_LDFLAGS=-L../ctemplate/built/lib -lctemplate_nothreads
CTEMPLATE_IFLAGS=-I../ctemplate/built/include
CXXFLAGS=-static -O0 -g -std=c++0x $(CTEMPLATE_IFLAGS)
LDFLAGS=$(CTEMPLATE_LDFLAGS)

all: astgen

astgen: astgen.cpp ast.hpp
	$(CXX) $(CXXFLAGS) -o astgen astgen.cpp $(LDFLAGS)

astgen.cpp: astgen.peg
	$(GREG) astgen.peg > astgen.cpp
	
ast:
	cat astgen_ast.ast | ./astgen > ast.hpp

clean:
	rm -f astgen astgen.cpp

.PHONY: clean all ast
