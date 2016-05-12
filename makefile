#the makefile creates a bin directory and calls the other
#makefile from src
all: src/rshell.h src/rshell.cpp
	g++ -Wall -Werror -ansi -pedantic -o rshell src/rshell.cpp 
	mkdir bin
	mv rshell bin
rshell: src/rshell.h src/rshell.cpp
	g++ -Wall -Werror -ansi -pedantic -o rshell  src/rshell.cpp 
	mkdir bin
	mv rshell bin
