
CFLAGS = -ggdb -O2
LDFLAGS = -ggdb

all: clean basic_parser

run: all
	./basic_parser

basic_parser: basic_parser.o list.o
	gcc basic_parser.o list.o -o basic_parser

basic_parser.o: 
	gcc -c -g basic_parser.c

list.o: list.c list.h
	gcc -c -g list.c

clean:
	rm *.o basic_parser
