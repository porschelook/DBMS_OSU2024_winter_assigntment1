# generate make file to build main.cpp in program by using clang
#
CC = g++
CFLAGS = -Wall -Wextra -std=c++11

all: program

program: main.cpp
	$(CC) $(CFLAGS) -o program main.cpp

run: program
	./program

clean:
	rm -f program

t:
		$(CC) $(CFLAGS) -o t test.cpp && ./t && rm -f ./t

test: program
	./program