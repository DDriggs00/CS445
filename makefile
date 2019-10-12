CC=gcc
CFLAGS=-c -g -std=c99 -D_POSIX_C_SOURCE=1 -Wall -Iinclude -I.
# -D_POSIX_C_SOURCE=1 allows use of fileno in lex.yy.c
OBJ=lex.yy.o vgo.tab.o main.o token.o list.o node.o iterator.o node_list.o node_iterator.o node_iterator_full.o cfu.o cfustring.o cfuhash.o traversals.o symtab.o
ASSIGN=2

SRC_DIR=src

vgo: $(OBJ)
	$(CC) -o vgo $(OBJ)

main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c

token.o: $(SRC_DIR)/token.c
	$(CC) $(CFLAGS) $(SRC_DIR)/token.c

traversals.o: $(SRC_DIR)/traversals.c
	$(CC) $(CFLAGS) $(SRC_DIR)/traversals.c

symtab.o: $(SRC_DIR)/symtab.c
	$(CC) $(CFLAGS) $(SRC_DIR)/symtab.c

# Flex
lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) lex.yy.c

lex.yy.c: $(SRC_DIR)/vgolex.l vgo.tab.h
	flex $(SRC_DIR)/vgolex.l

# Bison
vgo.tab.o: vgo.tab.c
	$(CC) $(CFLAGS) vgo.tab.c

vgo.tab.c vgo.tab.h: $(SRC_DIR)/vgo.y
	bison -d $(SRC_DIR)/vgo.y

# tree/list
list.o: $(SRC_DIR)/list.c
	$(CC) $(CFLAGS) $(SRC_DIR)/list.c

node.o: $(SRC_DIR)/node.c
	$(CC) $(CFLAGS) $(SRC_DIR)/node.c

iterator.o: $(SRC_DIR)/iterator.c
	$(CC) $(CFLAGS) $(SRC_DIR)/iterator.c

node_iterator.o: $(SRC_DIR)/node_iterator.c
	$(CC) $(CFLAGS) $(SRC_DIR)/node_iterator.c

node_iterator_full.o: $(SRC_DIR)/node_iterator_full.c
	$(CC) $(CFLAGS) $(SRC_DIR)/node_iterator_full.c

node_list.o: $(SRC_DIR)/node_list.c
	$(CC) $(CFLAGS) $(SRC_DIR)/node_list.c

# hashtable
cfu.o: $(SRC_DIR)/cfu.c
	$(CC) $(CFLAGS) $(SRC_DIR)/cfu.c

cfustring.o: $(SRC_DIR)/cfustring.c
	$(CC) $(CFLAGS) $(SRC_DIR)/cfustring.c

cfuhash.o: $(SRC_DIR)/cfuhash.c
	$(CC) $(CFLAGS) $(SRC_DIR)/cfuhash.c

# Clean
.PHONY: clean
clean:
	@rm -f *.o vgo lex.yy.c vgo.tab.c vgo.tab.h vgo.output

# Create zip file
.PHONY: zip
zip:
	make clean
	@rm -f hw$(ASSIGN).zip
	zip -r hw$(ASSIGN).zip . -x *.git* *.vscode* "tests*" "doc*" LICENSE README.md vgo.tab.h vgo.tab.c