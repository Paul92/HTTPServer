all: server
parser: parser.h parser.c
	gcc -std=gnu99 -Wall -O2 -c -lm parser.c -o parser.o
static: static.h static.c
	gcc -std=gnu99 -Wall -O2 -c -lm static.c -o static.o
dynamic: dynamic.h dynamic.c
	gcc -std=gnu99 -Wall -O2 -c -lm dynamic.c -o dynamic.o
server: server.c parser.o static.o dynamic.o
	gcc -std=gnu99 -Wall -O2 -c -lm server.c -o server.o
	gcc -Wall -O2 -lm parser.o static.o dynamic.o server.o -o server

