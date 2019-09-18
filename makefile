CC=gcc
CFLAGS=-c -g -std=c99 -D_POSIX_C_SOURCE=1 -Wall -I./include
# -D_POSIX_C_SOURCE=1 allows use of fileno in lex.yy.c
OBJ=lex.yy.o vgo.tab.o main.o token.o list.o iterator.o node_iterator.o node_list.o node.o
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
	$(CC) $(CFLAGS) vgo.tab.c

vgo.tab.c vgo.tab.h: vgo.y
	bison -d vgo.y

# List

# Clean
.PHONY: clean
clean:
	@rm -f *.o vgo lex.yy.c vgo.tab.c vgo.tab.h

# Create zip file
.PHONY: zip
zip:
	make clean
	@rm -f Driggs-Assign-$(ASSIGN).zip
	zip -r hw$(ASSIGN).zip . -x *.git* LICENSE README.md vgo.tab.h vgo.tab.c "tests*"