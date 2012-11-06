CXX?=g++
CXXFLAGS?=-O0 -g -std=c++0x

all: main

main:
	../greg/greg astgen.peg > astgen.cpp
	$(CXX) $(CXXFLAGS) -o astgen astgen.cpp
