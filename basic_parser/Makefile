
CFLAGS = -ggdb -O2
LDFLAGS = -ggdb

all: basic_parser

basic_parser: basic_parser.o list.o
	gcc basic_parser.o list.o -o basic_parser

basic_parser.o: 
	gcc -c basic_parser.c

list.o: list.c list.h
	gcc -c list.c

clean:
	rm *.o basic_parser
