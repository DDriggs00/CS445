#ifndef SYMTAB_H
#define SYMTAB_H

#define NULL_TYPE    10000
#define INT_TYPE     10001
#define STRUCT_TYPE  10002
#define ARRAY_TYPE   10003
#define FLOAT64_TYPE 10004
#define FUNC_TYPE    10005
#define MAP_TYPE     10006
#define MAIN_TYPE    10007

#include "cfuhash.h"    // External hashtable library
#include "node.h"       // For using the tree
#include "list.h"       // For a list of hashtables

// Returns a tree containing all symbol tables from the given tree
node_t* genSymTab(node_t* tree);

// gets the name contained in the package statement, given a tree
char* getPackageName(node_t* tree);

// gets the name of a function, given a tree rooted at the xfndcl node
char* getFuncName(node_t* tree);

// gets the name of a struct, given a tree rooted at the structtype node
char* getStructName(node_t* tree);

// Fills the main hashtable
void populateHashtableMain(node_t* tree, cfuhash_table_t* ht);

// Fills a struct's hashtable
void populateHashtableStruct(node_t* tree, cfuhash_table_t* ht);

// Fills a function's hashtable
void populateHashtableFunction(node_t* tree, cfuhash_table_t* ht);

#endif // SYMTAB_H
