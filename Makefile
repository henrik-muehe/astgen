GREG?=../greg-cpp/greg
CXX?=g++
CXXFLAGS=-O0 -g -std=c++0x
LDFLAGS=-lctemplate

all: main

main:
	$(GREG) astgen.peg > astgen.cpp
	$(CXX) $(CXXFLAGS) -o astgen astgen.cpp $(LDFLAGS)

ast:
	cat astgen_ast.ast | ./astgen > ast.hpp
