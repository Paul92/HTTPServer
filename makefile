all: server
structs.o: src/include/structs.h src/structs.c
	gcc -std=gnu99 -Wall -O2 -c -lm src/structs.c -o structs.o
parser.o: src/include/parser.h src/parser.c
	gcc -std=gnu99 -Wall -O2 -c -lm src/parser.c -o parser.o
static.o: src/include/static.h src/static.c
	gcc -std=gnu99 -Wall -O2 -c -lm src/static.c -o static.o
dynamic.o: src/include/dynamic.h src/dynamic.c
	gcc -std=gnu99 -Wall -O2 -c -lm src/dynamic.c -o dynamic.o
server: src/server.c structs.o parser.o static.o dynamic.o
	gcc -std=gnu99 -Wall -O2 -c -lm src/server.c -o server.o
	gcc -Wall -O2 -lm structs.o parser.o static.o dynamic.o server.o -o server
clean:
	rm *.o

