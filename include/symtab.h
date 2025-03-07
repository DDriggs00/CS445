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
void populateHashtable(node_t* tree, cfuhash_table_t* rootHT, cfuhash_table_t* ht, char* scope);

// returns a list of varTokens given a tree rooted at a vardcl
varToken_t** parseVarDcl(node_t* tree, cfuhash_table_t* rootHT, char* scope, bool isConst);

// returns a list of varTokens given a tree rooted at a vardcllist
varToken_t** parseVarDclList(node_t* tree, cfuhash_table_t* rootHT, char* scope, bool isConst);

// returns a list of strings given a tree rooted at a DclName or DclNameList
char** parseDclNameList(node_t* tree);

// Adds the predefined symbol tables when they are imported
varToken_t** getImports(node_t* tree, char* scope);

// Adds function arguments to the functions hashtable
void parseFuncArgList(node_t* tree, cfuhash_table_t* ht, char* scope);

// Checks for undeclared variables
void detectUndeclaredVars(node_t* tree, cfuhash_table_t* rootHT, cfuhash_table_t* funcHT);

// Do the type checking
type_t* typeCheck(node_t* tree, cfuhash_table_t* rootHT, char* scope, bool isFunc);

// type checking for parameter lists
list_t* getParamTypeList(node_t* tree, cfuhash_table_t* rootHT, char* scope);

// Exist and prints an error message.
// To error with only 1 type, set the second type to zero
void typeErr(node_t* tree, type_t* type, type_t* type2);

// Returns the appropriate variable token, depending on the scope
varToken_t* getVarToken(char* varName, char* scope, cfuhash_table_t* rootHT);

// Helper function that returns the first terminal's token's text field
char* getFirstText(node_t* tree);

#endif // SYMTAB_H
