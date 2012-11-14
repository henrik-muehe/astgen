GREG?=../greg-cpp/greg
CXX?=g++
CXXFLAGS=-O0 -g -std=c++0x
LDFLAGS=-lctemplate

all: astgen

astgen: astgen.cpp
	$(CXX) $(CXXFLAGS) -o astgen astgen.cpp $(LDFLAGS)

astgen.cpp: astgen.peg
	$(GREG) astgen.peg > astgen.cpp
	
ast:
	cat astgen_ast.ast | ./astgen > ast.hpp

clean:
	rm -f astgen astgen.cpp

.PHONY: clean all ast