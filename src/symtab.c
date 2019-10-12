#include <string.h>
#include <stdlib.h>

#include "symtab.h"
#include "cfuhash.h"        // For symbol tables
#include "token.h"          // For reading the tree's leaf tokens
#include "node.h"           // For using the tree
// #include "node_iterator.h"  // For using the tree
#include "node_iterator_full.h"  // For using the tree
#include "list.h"           // For a list of hashtables
#include "nodeTypes.h"      // For easier navigation of the tree
#include "vgo.tab.h"        // For the yytokentype enum
#include "traversals.h"     

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

    // Create pointer to root ht for easier access.
    cfuhash_table_t* mainHT = root->data;

    // Add global variables to root ht
    populateHashtableMain(tree, mainHT);

    // Find function declarations and structs
    node_iterator_full_t* it = node_iterator_full_create(tree);
    node_t* node, * node2;
    char* name;
    while ((node = node_iterator_full_next(it))) {
        switch (node->type) {
            case tag_xfndcl:
                // Create entry in main hashtable
                if (!(name = getFuncName(node))) {
                    fprintf(stderr, "Error finding function name.\n");
                    return NULL;
                }
                if (cfuhash_put(mainHT, name, FUNC_TYPE)) {
                    // Key already existed
                    fprintf(stderr, "Error: Redeclaration of function %s\n", name);
                    return NULL;
                }

                // Build own hashtable
                node2 = node_create(root, cfuhash_new(), FUNC_TYPE);
                
                // Populate own hashtable
                populateHashtableFunction(node, node2->data);
                break;
            case tag_structtype:
                // Create entry in main hashtable
                if (!(name = getStructName(node))) {
                    fprintf(stderr, "Error finding struct name.\n");
                    return NULL;
                }
                if (cfuhash_put(mainHT, name, STRUCT_TYPE)) {
                    // Key already existed
                    fprintf(stderr, "Error: Redeclaration of struct %s\n", name);
                    return NULL;
                }

                // Build own hashtable
                node2 = node_create(root, cfuhash_new(), FUNC_TYPE);
                
                // Populate own hashtable
                populateHashtableFunction(node, node2->data);
                break;
            default:
                break;
        }
        
    }


}

char* getPackageName(node_t* tree) {
    node_t* nodeTemp;   // Will be used throughout

    nodeTemp = findNode(tree, tag_package);
    nodeTemp = findNode(nodeTemp, LNAME);

    char* mainPkgName = malloc(sizeof(char) * (strlen(((token_t*)(nodeTemp->data))->text) + 1));
    strcpy(mainPkgName, ((token_t*)(nodeTemp->data))->text);

    return mainPkgName;
}

char* getFuncName(node_t* tree) {
    node_t* nodeTemp;   // Will be used throughout
    char* name;

    nodeTemp = findNode(tree, tag_fndcl);

    switch (nodeTemp->children->begin->type) {
        case LNAME:
            // Rule 1
            name = malloc(sizeof(char) * strlen(((token_t*)nodeTemp->children->begin->data)->text) + 1);
            strcpy(name, ((token_t*)nodeTemp->children->begin->data)->text);
            return name;
        case '(':
            // Rule 2
            if (!(nodeTemp = findNodeShallow(nodeTemp, LNAME))) return NULL;    // LNAME not found
            name = malloc(sizeof(char) * strlen(((token_t*)nodeTemp->data)->text) + 1);
            strcpy(name, ((token_t*)nodeTemp->data)->text);
            return name;
        default:
            return NULL;
    }
}

char* getStructName(node_t* tree) {
    node_t* nodeTemp;   // Will be used throughout
    char* name;

    nodeTemp = getSibling(tree, -1);
    
    // if sibling did not exist
    if (!nodeTemp) return NULL;
    
    // if node is not the correct node;
    if (nodeTemp->type != LNAME) return NULL;

    return ((token_t*)(nodeTemp->data))->text;
}

// Fills the main hashtable
void populateHashtableMain(node_t* tree, cfuhash_table_t* ht) {

}

// Fills a struct's hashtable
void populateHashtableStruct(node_t* tree, cfuhash_table_t* ht) {

}

// Fills a function's hashtable
void populateHashtableFunction(node_t* tree, cfuhash_table_t* ht) {

}