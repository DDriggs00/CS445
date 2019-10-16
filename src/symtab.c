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

cfuhash_table_t* genSymTab(node_t* tree) {
    if (!tree) return NULL;
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
                    exit(3);
                }

                // Create own hashtable
                ht = cfuhash_new();

                // Populate own hashtable
                populateHashtable(node, ht, name);

                // Create entry in main hashtable
                vt = varToken_create_func(packageName, name, ht);
                if (cfuhash_put(htTree, name, vt)) {
                    // Key already existed
                    token_t* firstToken = getFirstTerminal(node)->data;
                    fprintf(stderr, "%s:%i: Error: Redeclaration of function %s\n", firstToken->filename, firstToken->lineno, name);
                    exit(3);
                }
                
                break;
            case tag_structtype:
                name = getStructName(node);
                if (!name) {    // If name could not be found, return NULL
                    fprintf(stderr, "Error finding struct name.\n");
                    exit(3);
                }

                // Create own hashtable
                ht = cfuhash_new();

                // Populate own hashtable
                populateHashtable(node, ht, name);

                // Create entry in main hashtable
                vt = varToken_create_struct(packageName, name, ht);
                if (cfuhash_put(htTree, name, vt)) {
                    // Key already existed
                    token_t* firstToken = getFirstTerminal(node)->data;
                    fprintf(stderr, "%s:%i: Error: Redeclaration of struct %s\n", firstToken->filename, firstToken->lineno, name);
                    exit(3);
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
    node_t* node;
    node_iterator_full_t* it = node_iterator_full_create(tree);
    varToken_t** newVars;
    
    newVars = getImports(tree, scope);
    node = node_iterator_full_next(it);
    for (int i = 0; newVars[i] != NULL; i++) {
        if (cfuhash_put(ht, newVars[i]->name, newVars[i])) {
            // Key already existed
            token_t* firstToken = getFirstTerminal(node)->data;
            fprintf(stderr, "%s:%i: Error: Redeclaration of variable %s\n", firstToken->filename, firstToken->lineno, newVars[i]->name);
            exit(3);
        }
    }
    while (node) {
        switch (node->tag) {
            case tag_constdcl:
                newVars = parseVarDcl(node, scope, true);
                for (int i = 0; newVars[i] != NULL; i++) {
                    if (cfuhash_put(ht, newVars[i]->name, newVars[i])) {
                        // Key already existed
                        token_t* firstToken = getFirstTerminal(node)->data;
                        fprintf(stderr, "%s:%i: Error: Redeclaration of variable %s\n", firstToken->filename, firstToken->lineno, newVars[i]->name);
                        exit(3);
                    }
                }
                free(newVars);
                break;
            case tag_vardcl:
                newVars = parseVarDcl(node, scope, false);
                for (int i = 0; newVars[i] != NULL; i++) {
                    if (cfuhash_put(ht, newVars[i]->name, newVars[i])) {
                        // Key already existed
                        token_t* firstToken = getFirstTerminal(node)->data;
                        fprintf(stderr, "%s:%i: Error: Redeclaration of variable %s\n", firstToken->filename, firstToken->lineno, newVars[i]->name);
                        exit(3);
                    }
                }
                free(newVars);
                break;
            // global vars must be before function declarations
            case tag_xfndcl:
            case tag_structtype:
                return;
        }
        node = node_iterator_full_next(it);
    }
    
}

// Fills a function or struct's hashtable
void populateHashtable(node_t* tree, cfuhash_table_t* ht, char* scope) {
    node_t* node;
    node_iterator_full_t* it = node_iterator_full_create(tree);
    varToken_t** newVars;
    token_t* firstToken; // for Errors
    node = node_iterator_full_next(it);
    if (node->tag == tag_structtype || node->tag == tag_xfndcl) {
        // prevent false flagging of nested structdcl
        node = node_iterator_full_next(it);
    }
    while (node) {
        switch (node->tag) {
            case tag_constdcl:
                newVars = parseVarDcl(node, scope, true);
                for (int i = 0; newVars[i] != NULL; i++) {
                    if (cfuhash_put(ht, newVars[i]->name, newVars[i])) {
                        // Key already existed
                        token_t* firstToken = getFirstTerminal(node)->data;
                        fprintf(stderr, "%s:%i: Error: Redeclaration of variable %s\n", firstToken->filename, firstToken->lineno, newVars[i]->name);
                        exit(3);
                    }
                }
                break;
            case tag_structdcl:
            case tag_arg_type:
            case tag_vardcl:
                newVars = parseVarDcl(node, scope, false);
                for (int i = 0; newVars[i] != NULL; i++) {
                    if (cfuhash_put(ht, newVars[i]->name, newVars[i])) {
                        // Key already existed
                        token_t* firstToken = getFirstTerminal(node)->data;
                        fprintf(stderr, "%s:%i: Error: Redeclaration of variable %s\n", firstToken->filename, firstToken->lineno, newVars[i]->name);
                        exit(3);
                    }
                }
                break;
            // no nested functions/structs
            case tag_xfndcl:
                firstToken = getFirstTerminal(node)->data;
                fprintf(stderr, "%s:%i Error: Nested function declaration\n", firstToken->filename, firstToken->lineno);
                exit(3);
            case tag_structtype:
                firstToken = getFirstTerminal(node)->data;
                fprintf(stderr, "%s:%i Error: Nested struct declaration\n", firstToken->filename, firstToken->lineno);
                exit(3);
        }
        node = node_iterator_full_next(it);
    }
}

varToken_t** parseVarDcl(node_t* tree, char* scope, bool isConst) {
    if (!tree) return NULL;
    if (tree->tag != tag_vardcl
        && tree->tag != tag_constdcl
        && tree->tag != tag_structdcl
        && tree->tag != tag_arg_type) return NULL;

    // get variable name array
    node_t* node = tree->children->begin;
    char** names = parseDclNameList(node);

    // count names
    int numVars;
    for (numVars = 0; names[numVars] != NULL; numVars++);

    // get variable type
    node = node->next;
    int type = node->tag;
    int subTypeA = 0, subTypeB = 0;
    if(type == tag_othertype) {
        switch (node->children->begin->tag) {
            case '[':
                // Array
                type = ARRAY_TYPE;
                subTypeA = node->children->begin->next->next->next->tag;
                break;
            case LMAP:
                type = MAP_TYPE;
                subTypeA = node->children->begin->next->next->tag;
                subTypeB = node->children->begin->next->next->next->next->tag;
                break;
            default:
                break;
        }
    }

    varToken_t** vts = malloc(sizeof(varToken_t*) * (numVars + 1));
    for (int i = 0; names[i] != NULL; i++) {
        vts[i] = varToken_create(scope, names[i], type);
        vts[i]->subType1 = getProperTypeInt(subTypeA);
        vts[i]->subType2 = getProperTypeInt(subTypeB);
        if (isConst) {
            vts[i]->isConst = true;
        }
    }

    node = node->next;
    if (node && node->tag != tag_empty) {
        // variables are assigned
        node = node->next;
        // Not in this homework
    }

    return vts;
}

varToken_t** parseVarDclList(node_t* tree, char* scope, bool isConst) {
    if (!tree) return NULL;
    if (tree->tag != tag_vardcl_list) return NULL;
    return NULL;
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

varToken_t** getImports(node_t* tree, char* scope) {
    
    varToken_t** vts = calloc(sizeof(char*), 4);
    u_int8_t pos = 0;
    token_t* token;
    
    node_t* node;
    node_iterator_full_t* it = node_iterator_full_create(tree);
    while((node = node_iterator_full_next(it))) {
        switch (node->tag) {
            case tag_import:
                token = node->children->begin->next->data;
                if (!strcmp(token->text, "\"fmt\"")) {
                    vts[pos] = varToken_create_func("fmt", "Println", cfuhash_new());
                    pos++;
                }
                else if (!strcmp(token->text, "\"time\"")) {
                    vts[pos] = varToken_create_func("time", "Now", cfuhash_new());
                    pos++;
                }
                else if (!strcmp(token->text, "\"math/rand\"")) {
                    vts[pos] = varToken_create_func("rand", "Intn", cfuhash_new());
                    pos++;

                }
                else {
                    fprintf(stderr, "%s:%i Error: %s is not a valid import in VGo\n", token->filename, token->lineno, token->text);
                    exit(3);
                }
                break;
            case tag_xdcl_list:
                return vts;
                // All imports have been imported
        }
    }
    return vts;
}
