CC=gcc
CFLAGS=-c -g -std=c99 -Wall
OBJ=lex.yy.o main.o token.o list.o list_iterator.o list_node.o

vgo: $(OBJ)
	$(CC) -o vgo $(OBJ)

main.o: main.c
	$(CC) $(CFLAGS) main.c

token.o: token.c
	$(CC) $(CFLAGS) token.c

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) lex.yy.c

lex.yy.c: vgolex.l vgo.tab.h
	flex vgolex.l

list.o: list.c
	$(CC) $(CFLAGS) list.c

list_iterator.o: list_iterator.c
	$(CC) $(CFLAGS) list_iterator.c

list_node.o: list_node.c
	$(CC) $(CFLAGS) list_node.c

.PHONY: clean
clean:
	@rm -f *.o vgo lex.yy.c
