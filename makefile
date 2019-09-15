CC=gcc
CFLAGS=-c -g -std=c99 -D_POSIX_C_SOURCE=1 -Wall
# -D_POSIX_C_SOURCE=1 allows use of fileno in lex.yy.c
OBJ=lex.yy.o main.o token.o list.o list_iterator.o list_node.o
ASSIGN=2


vgo: $(OBJ)
	$(CC) -o vgo $(OBJ)

main.o: main.c
	$(CC) $(CFLAGS) main.c

token.o: token.c
	$(CC) $(CFLAGS) token.c

# Flex
lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) lex.yy.c

lex.yy.c: vgolex.l vgo.tab.h
	flex vgolex.l

# Bison
vgo.tab.o: vgo.tab.c
	$(CC) $(CFLAGS) go.tab.c

vgo.tab.c vgo.tab.h: vgo.y
	bison -d vgo.y

# List
list.o: list.c
	$(CC) $(CFLAGS) list.c

list_iterator.o: list_iterator.c
	$(CC) $(CFLAGS) list_iterator.c

list_node.o: list_node.c
	$(CC) $(CFLAGS) list_node.c

# Clean
.PHONY: clean
clean:
	@rm -f *.o vgo lex.yy.c

# Create zip file
.PHONY: zip
zip:
	make clean
	@rm -f Driggs-Assign-$(ASSIGN).zip
	zip -r hw$(ASSIGN).zip . -x *.git* LICENSE README.md vgo.tab.h vgo.tab.c "tests*"