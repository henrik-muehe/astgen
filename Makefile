all: main

main:
	../greg/greg astgen.peg > astgen.cpp
	g++ -std=c++11 -O0 -g -o astgen astgen.cpp
