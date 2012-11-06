all: main

main:
	../greg/greg astgen.peg > astgen.cpp
	$(CXX) -std=c++0x -O0 -g -o astgen astgen.cpp
