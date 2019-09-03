CC=gcc
CFLAGS=-c -g -Wall
OBJ=main.o token.o #lex.yy.o

vgo: $(OBJ)
	$(CC) -o vgo $(OBJ)

main.o: main.c
	$(CC) $(CFLAGS) main.c

token.o: token.c
	$(CC) $(CFLAGS) token.c

# lex.yy.o: lex.yy.c
# 	$(CC) $(CFLAGS) lex.yy.c

# lex.yy.c: vgolex.l vgo.tab.h
# 	flex voglex.l
