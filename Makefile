CFLAGS=-c -Wall -Wextra -Wpedantic -Werror -I./src

all: bin/cformat

run: bin/cformat
	bin/cformat
	
bin/cformat: obj/main.o obj/stack/stack.o  obj/cformat.o
	$(CC) $^ -o $@

obj/cformat.o: src/cformat.c 
	$(CC) $(CFLAGS) -c $^ -o $@

obj/stack/stack.o: src/stack/stack.c
	$(CC) $(CFLAGS) -c $^ -o $@

obj/main.o: src/main.c 
	$(CC) $(CFLAGS) -c $^ -o $@


