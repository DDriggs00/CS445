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
#include "varToken.h"

// 1. Find each scope
//  a. Main         "package"
//  b. Functions    "xfndcl"
//  c. Structs      "structtype"
// 2. For each scope, generate an empty hashtable, and link that hashtable in the list
// 2. For each scope, find all varDCLs
// 3. For each varDCL, Check if already exists in master or local scope
// 5. Add vardcl to symbol table
// 6. For each varReference, check master and local symbol tables

cfuhash_table_t* genSymTab(node_t* tree) {
    // Initialize root
    cfuhash_table_t* htTree = cfuhash_new();

    // Main scope name is used throughout
    char* packageName = getPackageName(tree);
    if (!packageName) { // If name not found
        fprintf(stderr, "Error finding function name.\n");
        return NULL;
    }

    // Add global variables to root ht
    populateHashtableMain(tree, htTree, packageName);

    // Find function declarations and structs
    node_iterator_full_t* it = node_iterator_full_create(tree);
    node_t* node;
    char* name;
    varToken_t* vt;
    cfuhash_table_t* ht;
    while ((node = node_iterator_full_next(it))) {
        switch (node->tag) {
            case tag_xfndcl:
                name = getFuncName(node);
                if (!name) {    // If name could not be found, return NULL
                    fprintf(stderr, "Error finding function name.\n");
                    return NULL;
                }

                // Create own hashtable
                ht = cfuhash_new();

                // Populate own hashtable
                populateHashtableFunction(node, ht, name);

                // Create entry in main hashtable
                vt = varToken_create_func(packageName, name, ht);
                if (cfuhash_put(htTree, name, vt)) {
                    // Key already existed
                    fprintf(stderr, "Error: Redeclaration of function %s\n", name);
                    return NULL;
                }
                
                break;
            case tag_structtype:
                name = getStructName(node);
                if (!name) {    // If name could not be found, return NULL
                    fprintf(stderr, "Error finding struct name.\n");
                    return NULL;
                }

                // Create own hashtable
                ht = cfuhash_new();

                // Populate own hashtable
                populateHashtableStruct(node, ht, name);

                // Create entry in main hashtable
                vt = varToken_create_struct(packageName, name, ht);
                if (cfuhash_put(htTree, name, vt)) {
                    // Key already existed
                    fprintf(stderr, "Error: Redeclaration of struct %s\n", name);
                    return NULL;
                }
                
                break;
            default:
                break;
        }
        
    }

    return htTree;
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

    switch (nodeTemp->children->begin->tag) {
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

    nodeTemp = getSibling(tree, -1);
    
    // if sibling did not exist
    if (!nodeTemp) return NULL;
    
    // if node is not the correct node;
    if (nodeTemp->tag != LNAME) return NULL;

    return ((token_t*)(nodeTemp->data))->text;
}

// Fills the main hashtable
void populateHashtableMain(node_t* tree, cfuhash_table_t* ht, char* scope) {
    bool skip = false;
    node_t* node;
    node_iterator_full_t* it = node_iterator_full_create(tree);
    while ((node = node_iterator_full_next(it))) {
        switch (node->tag) {
            case tag_vardcl_list:
                vartoken_t** newVars = parseVarDclList(node, scope);
                for (int i = 0; newVars[i] != NULL; i++) {
                    cfuhash_put(ht, newvars[i]->name, newVars[i]);
                }
                break;
            case tag_vardcl:
                vartoken_t** newVars = parseVarDcl(node, scope);
                for (int i = 0; newVars[i] != NULL; i++) {
                    cfuhash_put(ht, newvars[i]->name, newVars[i]);
                }
                break;
            // global vars must be before function declarations
            case tag_xfndcl:
            case tag_structtype:
                return;
        }
    }
    
}

// Fills a struct's hashtable
void populateHashtableStruct(node_t* tree, cfuhash_table_t* ht, char* scope) {

}

// Fills a function's hashtable
void populateHashtableFunction(node_t* tree, cfuhash_table_t* ht, char* scope) {

}

varToken_t** parseVarDcl(node_t* tree, char* scope, bool isConst) {
    if (!tree) return NULL;
    if (tree->tag != tag_vardcl) return NULL;

    // get variable name array
    node_t* node = tree->children->begin;
    char** names = parseDclNameList(node);

    // count names
    int numVars;
    for (numVars = 0; names[numVars] != NULL; numVars++);

    // get variable type
    node = node->next;
    int type = tree->children->begin->next->tag;

    varToken_t** vts = malloc(sizeof(vartoken_t*) * (numVars + 1));
    for (int i = 0; names[i] != NULL; i++) {
        vts[i] = varToken_create(scope, names[i], type);
    }

    node = node->next;
    if (node) {
        // variables are assigned
        node = node->next;
        // Not in this homework
    }

    return vts;
}

varToken_t** parseVarDclList(node_t* tree, char* scope, bool isConst) {
    if (!tree) return NULL;
    if (tree->tag != tag_vardcl_list) return NULL;
}

char** parseDclNameList(node_t* tree) {
    if (!tree) return NULL;
    if (tree->tag == LNAME) {
        char** names = malloc(sizeof(char*) * 2);
        names[0] = malloc(sizeof(char) * (1 + strlen(((token_t*)(tree->data))->text)));
        strcpy(names[0], ((token_t*)(tree->data))->text);
        names[1] = NULL;
        return names;
    }
    if (tree->tag != tag_dcl_name_list) return NULL;

    int count = treeCount(tree, LNAME);

    char** names = malloc(sizeof(char*) * (count + 1));

    int i = 0;
    node_t* node;
    node_iterator_full_t* it = node_iterator_full_create(tree);
    while ((node = node_iterator_full_next(it))) {
        if (node->tag == LNAME) {
            names[i] = malloc(sizeof(char) * (1 + strlen(((token_t*)(node->data))->text)));
            strcpy(names[i], ((token_t*)(node->data))->text);
            i++;
        }
    }
    names[i] = NULL;
    return names;
}