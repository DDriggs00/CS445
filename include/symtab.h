#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdbool.h>

#include "cfuhash.h"    // External hashtable library
#include "node.h"       // For using the tree
#include "list.h"       // For a list of hashtables
#include "varToken.h"

// Returns a tree containing all symbol tables from the given tree
cfuhash_table_t* genSymTab(node_t* tree);

// gets the name contained in the package statement, given a tree
char* getPackageName(node_t* tree);

// gets the name of a function, given a tree rooted at the xfndcl node
char* getFuncName(node_t* tree);

// gets the name of a struct, given a tree rooted at the structtype node
char* getStructName(node_t* tree);

// Fills the main hashtable
void populateHashtableMain(node_t* tree, cfuhash_table_t* ht, char* scope);

// Fills a function or struct's hashtable
void populateHashtable(node_t* tree, cfuhash_table_t* ht, char* scope);

// returns a list of varTokens given a tree rooted at a vardcl
varToken_t** parseVarDcl(node_t* tree, char* scope, bool isConst);

// returns a list of varTokens given a tree rooted at a vardcllist
varToken_t** parseVarDclList(node_t* tree, char* scope, bool isConst);

// returns a list of strings given a tree rooted at a DclName or DclNameList
char** parseDclNameList(node_t* tree);

// Adds the predefined symbol tables when they are imported
varToken_t** getImports(node_t* tree, char* scope);

// Adds function arguments to the functions hashtable
void parseFuncArgList(node_t* tree, cfuhash_table_t* ht, char* scope);

// Checks for undeclared variables
void detectUndeclaredVars(node_t* tree, cfuhash_table_t* rootHT, cfuhash_table_t* funcHT);

// Do the type checking
int typeCheck(node_t* tree);
bool isCompatibleType(int operator, int type1, int type2);

#endif // SYMTAB_H
