CC=gcc
CFLAGS=-c -g -Wall
OBJ=main.o #lex.yy.o

vgo: $(OBJ)
	$(CC) -o vgo $(OBJ)

main.o: main.c
	$(CC) $(CFLAGS) main.c

# lex.yy.o: lex.yy.c
# 	$(CC) $(CFLAGS) lex.yy.c

# lex.yy.c: clex.l ytab.h
# 	flex clex.l
