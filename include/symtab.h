#ifndef SYMTAB_H
#define SYMTAB_H

#define NULL_TYPE    1000000
#define INT_TYPE     1000001
#define STRUCT_TYPE  1000002
#define ARRAY_TYPE   1000003
#define FLOAT64_TYPE 1000004
#define FUNC_TYPE    1000005
#define MAP_TYPE     1000006

// #include "hashtable.h"  // External hashtable library
#include "node.h"       // For using the tree
#include "list.h"       // For a list of hashtables

// Returns a list of hashtables, containing all declared variables
list_t* genSymTab(node_t* tree);

// Traverses the given tree, and returns the node with *tag* 
// Ignores the first n entries with *tag*
// If no match (or not enough matches) found, returns NULL
node_t* findNode(node_t* root, char* tag, int *n);


#endif					/* SYMTAB_H */
