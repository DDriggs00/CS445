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

    // Check main ht for undeclared variables
    detectUndeclaredVars(tree, htTree, NULL);

    // Find function declarations and structs
    node_iterator_full_t* it = node_iterator_full_create(tree);
    node_t* node;
    char* name;
    varToken_t* vt;
    token_t* firstToken;
    cfuhash_table_t* ht;
    while ((node = node_iterator_full_next(it))) {
        switch (node->tag) {
            case tag_xfndcl:
                name = getFuncName(node);
                if (!name) {    // If name could not be found, return NULL
                    fprintf(stderr, "Error finding function name.\n");
                    exit(3);
                }

                // get line number
                firstToken = getFirstTerminal(node)->data;


                // Create own hashtable
                ht = cfuhash_new();

                // Populate own hashtable
                populateHashtable(node, ht, name);

                // Check ht for undeclared variables
                detectUndeclaredVars(node, htTree, ht);

                // Create entry in main hashtable
                vt = varToken_create_func(packageName, name, firstToken->lineno, ht);
                if (cfuhash_put(htTree, name, vt)) {
                    // Key already existed
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

                firstToken = getFirstTerminal(node)->data;
                
                // Create own hashtable
                ht = cfuhash_new();

                // Populate own hashtable
                populateHashtable(node, ht, name);

                // Check ht for undeclared variables
                detectUndeclaredVars(node, htTree, ht);

                // Create entry in main hashtable
                vt = varToken_create_struct(packageName, name, firstToken->lineno, ht);
                if (cfuhash_put(htTree, name, vt)) {
                    // Key already existed
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

    char* mainPkgName = calloc(sizeof(char), (strlen(((token_t*)(nodeTemp->data))->text) + 1));
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
            name = calloc(sizeof(char), strlen(((token_t*)nodeTemp->children->begin->data)->text) + 1);
            strcpy(name, ((token_t*)nodeTemp->children->begin->data)->text);
            return name;
        case '(':
            // Rule 2
            if (!(nodeTemp = findNodeShallow(nodeTemp, LNAME))) return NULL;    // LNAME not found
            name = calloc(sizeof(char), strlen(((token_t*)nodeTemp->data)->text) + 1);
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
    if (nodeTemp->children->begin->tag != LNAME) return NULL;

    return ((token_t*)(nodeTemp->children->begin->data))->text;
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

    // get Line number
    node_t* node = getFirstTerminal(tree);
    int line = ((token_t*)(node->data))->lineno;

    // get variable name array
    node = tree->children->begin;
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

    varToken_t** vts = calloc(sizeof(varToken_t*), (numVars + 1));
    for (int i = 0; names[i] != NULL; i++) {
        vts[i] = varToken_create(scope, names[i], type, line);
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
    return NULL; //TODO
}

char** parseDclNameList(node_t* tree) {
    if (!tree) return NULL;
    if (tree->tag == LNAME) {
        char** names = calloc(sizeof(char*), 2);
        names[0] = calloc(sizeof(char), (strlen(((token_t*)(tree->data))->text)) + 1);
        strcpy(names[0], ((token_t*)(tree->data))->text);
        return names;
    }
    if (tree->tag == tag_dcl_name || tree->tag == tag_new_name || tree->tag == tag_typedclname || tree->tag == tag_name) {
        char** names = calloc(sizeof(char*), 2);
        names[0] = calloc(sizeof(char), (strlen(((token_t*)(tree->children->begin->data))->text)) + 1);
        strcpy(names[0], ((token_t*)(tree->children->begin->data))->text);
        return names;
    }
    if (tree->tag != tag_dcl_name_list) return NULL;

    int count = treeCount(tree, LNAME);

    char** names = calloc(sizeof(char*), count + 1);

    int i = 0;
    node_t* node;
    node_iterator_full_t* it = node_iterator_full_create(tree);
    while ((node = node_iterator_full_next(it))) {
        if (node->tag == LNAME) {
            names[i] = calloc(sizeof(char), (1 + strlen(((token_t*)(node->data))->text)));
            strcpy(names[i], ((token_t*)(node->data))->text);
            i++;
        }
    }
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
                    vts[pos] = varToken_create_func("fmt", "Println", token->lineno, cfuhash_new());
                    pos++;
                }
                else if (!strcmp(token->text, "\"time\"")) {
                    vts[pos] = varToken_create_func("time", "Now", token->lineno, cfuhash_new());
                    pos++;
                }
                else if (!strcmp(token->text, "\"math/rand\"")) {
                    vts[pos] = varToken_create_func("rand", "Intn", token->lineno, cfuhash_new());
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

void detectUndeclaredVars(node_t* tree, cfuhash_table_t* rootHT, cfuhash_table_t* funcHT) {
    if (!tree) return;
    if (!rootHT) return;

    token_t* token;
    varToken_t* vt;
    node_t* node;
    node_iterator_full_t* it = node_iterator_full_create(tree);
    node = node_iterator_full_next(it);
    if (node->tag == tag_structtype || node->tag == tag_xfndcl) {
        // prevent false flagging of nested structdcl
        node = node_iterator_full_next(it);
    }
    while((node = node_iterator_full_next(it))) {
        switch (node->tag) {
            case LNAME:
                token = node->data;
                if(funcHT && cfuhash_exists(funcHT, token->text)) {
                    vt = cfuhash_get(funcHT, token->text);
                }
                else if(cfuhash_exists(rootHT, token->text)) {
                    vt = cfuhash_get(rootHT, token->text);
                }
                else if (hasParent(node, tag_package)) {
                    break;
                }
                else if (hasParent(node, tag_fndcl)) {
                    break;
                }
                else if (hasParent(node, tag_typedcl)) {
                    break;
                }
                else if (hasParent(node, tag_pexpr_no_paren_dot)) {
                    break;
                }
                else {
                    fprintf(stderr, "%s:%i Error: Usage of variable %s before declaration\n", token->filename, token->lineno, token->text);
                    exit(3);
                }
                if (vt->lineDeclared > token->lineno) {
                    fprintf(stderr, "%s:%i Error: Usage of variable %s before declaration\n", token->filename, token->lineno, token->text);
                    exit(3);
                }
                break;
            case tag_xfndcl:
                return;
            case tag_structtype:
                return;
        }
    }

}

// Do the type checking
int typeCheck(node_t* tree) {
    if (!tree) return 0;

    if (tree->count == 1) return typeCheck(tree->children->begin);

    int t1 = 0, t2 = 0;
    int op = 0;
    switch (tree->tag) {
        case tag_uexpr:     // Unary expression (Operator typeval)
            // OP VAL
            t1 = typeCheck(tree->children->end);
            op = tree->children->begin->tag;
            
            if (isCompatibleType(op, t1, 0)) return t1;
            else typeErr(tree->children->begin, t1);

            break;
        case tag_expr:      // Normal Expression
            // VAL OP VAL
            
            t1 = typeCheck(tree->children->begin);
            op = typeCheck(tree->children->begin->next);
            t2 = typeCheck(tree->children->end);

            if (isCompatibleType(op, t1, 0)) return t1;
            else typeErr(tree->children->begin, t1, 0);

            break;
        default:
            // node didn't need checked (but it's children might)
            switch (tree->count) {
                case 0: return 0;
                case 1: return typeCheck(tree->children->begin);
                default:
                    node_iterator_t* it = node_iterator_create(tree->children);
                    node_t* node;
                    while ((node = node_iterator_next(it))) {
                        typeCheck(node);
                    }
                    return 0;
            }
            break;
                    
    }
    fprintf(stderr, "Error Occured (Impossible state reached during type checking)\n");
    exit(3);
}

typeErr(node_t* operatorTree, int type, int type2) {
    token_t* first = getFirstTerminal(operatorTree)->data;
    if (type2 == 0) {
        // unary
        fprintf(stderr, "%s:%d Type %s is not compatible with operator %s\n", first->filename, first->lineno, getTypeName(type), first->text);
    }
    else {
        fprintf(stderr, "%s:%d Type %s is not compatible with type %s using operator %s\n", first->filename, first->lineno, getTypeName(type), getTypeName(type2), first->text);
    }
    exit(3);
}

bool isCompatibleType(int operator, int type1, int type2);

int getLeafType(node_t* leaf) {
    if (!leaf) return 0;
    if (leaf->count > 0) return 0;

    token_t* token = leaf->data;

    if (token->category == LLITERAL) {
        // Probably a variable
        
    }

}

char* getTypeName(int typeInt) {
    switch (typeInt) {
        case LINT:
        case INT_TYPE:
            return "Int";
        case LFLOAT:
        case FLOAT64_TYPE:
            return "Float64";
        case LLITERAL:
        case STRING_TYPE:
            return "String";
        case LBOOL:
        case BOOL_TYPE:
            return "Boolean";
        case LRUNE:
        case RUNE_TYPE:
            return "Rune";
    }

}