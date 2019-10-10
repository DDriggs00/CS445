#include <string.h>
#include <stdlib.h>

#include "symtab.h"
#include "cfuhash.h"        // For symbol tables
#include "token.h"          // For reading the tree's leaf tokens
#include "node.h"           // For using the tree
#include "node_iterator.h"  // For using the tree
#include "node_iterator_full.h"  // For using the tree
#include "list.h"           // For a list of hashtables
#include "nodeTypes.h"      // For easier navigation of the tree
#include "vgo.tab.h"        // For the yytokentype enum

// 1. Find each scope
//  a. Main         "package"
//  b. Functions    "xfndcl"
//  c. Structs      "structtype"
// 2. For each scope, generate an empty hashtable, and link that hashtable in the list
// 2. For each scope, find all varDCLs
// 3. For each varDCL, Check if already exists in master or local scope
// 5. Add vardcl to symbol table
// 6. For each varReference, check master and local symbol tables

node_t* genSymTab(node_t* tree) {
    node_t* htTree;
    
    // Initialize Root
    node_t* root = node_create(NULL, cfuhash_new(), MAIN_TYPE);

    node_iterator_full_t* it = node_iterator_full_create(tree);
    node_t* node;
    while ((node = node_iterator_full_next(it))) {
        if (node->type == tag_xfndcl) {
            
        }
    }


}

node_t* findNode(node_t* root, int tag) {
    if(root->type == tag) {
        return root;
    }
    if (root->count <= 0)
    {
        return NULL;
    }
    node_t* node;
    node_t* result = NULL;
    node_iterator_t* it = node_iterator_create(root->children);
    while ((node = node_iterator_next(it))) {
        result = findNode(node, tag);
        if(result != NULL) {
            return result;
        }
    }
    node_iterator_destroy(it);  // Memory leak sometimes caused here
    
    // If no match found, return NULL
    return NULL;
}

char* getPackageName(node_t* tree) {
    node_t* nodeTemp;   // Will be used throughout

    nodeTemp = findNode(tree, tag_package);
    nodeTemp = findNode(nodeTemp, LNAME);

    char* mainPkgName = malloc(sizeof(char) * (strlen(((token_t*)(nodeTemp->data))->text) + 1));
    strcpy(mainPkgName, ((token_t*)(nodeTemp->data))->text);

    return mainPkgName;
}

// Returns a tree of empty hashtables
node_t* initHashTables(node_t* tree) {
    
    

}

// Fills the main hashtable
void populateHashtableMain(node_t* tree, cfuhash_table_t* htTree) {

}

// Fills a struct's hashtable
void populateHashtableStruct(node_t* tree, cfuhash_table_t* htTree) {

}

// Fills a function's hashtable
void populateHashtableFunction(node_t* tree, cfuhash_table_t* htTree) {

}