# generate make file to build main.cpp in program by using clang
#
CC = clang
CFLAGS = -Wall -Wextra -std=c++11

all: program

program: main.cpp
	$(CC) $(CFLAGS) -o program main.cpp

run: program
	./program

clean:
	rm -f program
