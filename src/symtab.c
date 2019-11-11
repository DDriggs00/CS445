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
#include "type.h"

extern cfuhash_table_t* hashTable;

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
            case tag_vardcl_init:
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
            case tag_vardcl_init:
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
        && tree->tag != tag_vardcl_init
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
    if (type == tag_othertype_arr) {
        // Array
        type = ARRAY_TYPE;
        subTypeA = node->children->begin->next->next->next->tag;
    }
    else if (type == tag_othertype_map) {
        type = MAP_TYPE;
        subTypeA = node->children->begin->next->next->tag;
        subTypeB = node->children->begin->next->next->next->next->tag;
    }
    else if (type == tag_dotname) {
        type = STRUCT_INSTANCE_TYPE;
        char* sName = ((token_t*)(getFirstTerminal(node)->data))->text;
    }

    varToken_t** vts = calloc(numVars + 1, sizeof(varToken_t*));
    for (int i = 0; names[i] != NULL; i++) {
        if (type == ARRAY_TYPE) {
            vts[i] = varToken_create_arr(scope, names[i], subTypeA, line);
        }
        else if (type == MAP_TYPE) {
            vts[i] = varToken_create_map(scope, names[i], subTypeA, subTypeB, line);

        }
        else if (type == STRUCT_INSTANCE_TYPE) {
            vts[i] = varToken_create_struct(scope, names[i], subTypeA, subTypeB, line);

        }
        else {
            vts[i] = varToken_create(scope, names[i], type, line);
        }
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
                    cfuhash_table_t* ht = cfuhash_new();
                    cfuhash_put(ht, "Println", varToken_create_func("fmt", "Println", token->lineno, cfuhash_new()));
                    cfuhash_put(ht, "Scan", varToken_create_func("fmt", "Scan", token->lineno, cfuhash_new()));
                    vts[pos] = varToken_create_lib("main", "fmt", token->lineno, ht);
                    pos++;
                }
                else if (!strcmp(token->text, "\"time\"")) {
                    cfuhash_table_t* ht = cfuhash_new();
                    cfuhash_put(ht, "Now", varToken_create_func("time", "Now", token->lineno, cfuhash_new()));
                    vts[pos] = varToken_create_lib("main", "time", token->lineno, ht);
                    pos++;
                }
                else if (!strcmp(token->text, "\"math/rand\"")) {
                    cfuhash_table_t* ht = cfuhash_new();
                    cfuhash_put(ht, "Intn", varToken_create_func("rand", "Intn", token->lineno, cfuhash_new()));
                    vts[pos] = varToken_create_lib("main", "rand", token->lineno, ht);
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
                else if (hasParent(node, tag_pexpr_no_paren_dot)) {
                    node = node->parent;
                    if (node->tag != tag_pexpr_no_paren_dot) {
                        node = node->parent;
                    }
                    char* htName = ((token_t*)(getFirstTerminal(node->children->begin)->data))->text;
                    char* fName = ((token_t*)(getFirstTerminal(node->children->end)->data))->text;
                    vt = cfuhash_get(rootHT, htName);
                    if (!vt) {
                        fprintf(stderr, "%s:%i Error: Undeclared dot operator scope: %s\n", token->filename, token->lineno, htName);
                        exit(3);
                    }
                    if (vt->type->type != LIB_TYPE && vt->type->type != STRUCT_TYPE ) {
                        fprintf(stderr, "%s:%i Error: %s is not a valid scope\n", token->filename, token->lineno, htName);
                        exit(3);
                    }
                    
                    vt = cfuhash_get(vt->symTab, fName);
                    if (!vt) {
                        fprintf(stderr, "%s:%i Error: %s is not a valid member of %s\n", token->filename, token->lineno, fName, htName);
                        exit(3);
                    }
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
                
                else {
                    fprintf(stderr, "%s:%i Error: Usage of variable %s before declaration\n", token->filename, token->lineno, token->text);
                    exit(3);
                }
                if (!vt || vt->lineDeclared > token->lineno) {
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
type_t* typeCheck(node_t* tree, char* scope) {
    if (!tree) return NULL;

    if (tree->count == 1) return typeCheck(tree->children->begin, scope);

    type_t* t1;
    type_t* t2;
    type_t* op;
    type_t* returnType;
    varToken_t* vt;

    // These need to be declared outside the switch
    node_iterator_t* it;
    node_t* node;
    int arraylen;

    switch (tree->tag) {
        case tag_uexpr:     // Unary expression (Operator typeval)
            // OP VAL
            t1 = typeCheck(tree->children->end, scope);
            op = typeCheck(tree->children->begin, scope);

            if (isCompatibleType(op, t1, 0)) {free(op); return t1;}
            else typeErr(tree->children->begin, t1, NULL);

            break;
        case tag_simple_stmt:   // ++, --
            // VAL OP
            t1 = typeCheck(tree->children->begin, scope);
            op = typeCheck(tree->children->end, scope);

            if (isCompatibleType(op, t1, 0)) {free(op); return t1;}
            else typeErr(tree->children->begin, t1, NULL);

            break;
        case tag_expr_list:     // Chain of normal expressions
        case tag_expr:          // Normal Expression
        case tag_assignment:    // Assignment
        case tag_simple_stmt2:  // +=, -=
            // VAL OP VAL

            t1 = typeCheck(tree->children->begin, scope);
            op = typeCheck(tree->children->begin->next, scope);
            t2 = typeCheck(tree->children->end, scope);

            returnType = isCompatibleType(op, t1, t2);
            if (returnType)  {free(t2); free(op); return returnType;}
            else typeErr(tree->children->begin->next, t1, t2);

            break;
        case tag_vardcl_init:
            // FLUFF VAL OP VAL

            t1 = typeCheck(tree->children->begin->next, scope);
            op = typeCheck(tree->children->begin->next->next, scope);
            t2 = typeCheck(tree->children->end, scope);

            returnType = isCompatibleType(op, t1, t2);
            if (returnType)  {free(t2); free(op); return returnType;}
            else typeErr(tree->children->begin, t1, t2);

            break;
        case tag_othertype_arr:
            // [ VAL ] VAL
            t1 = typeCheck(tree->children->begin->next, scope); // Index
            t2 = typeCheck(tree->children->end, scope);         // ArrayType
            if (!t1) {
                token_t* first = getFirstTerminal(tree)->data;
                fprintf(stderr, "%s:%i Array initializer must have size (dynamic array NYI)\n", first->filename, first->lineno);
                exit(3);
            }
            if (t1->type != INT_TYPE) typeErr(tree->children->begin, t1, NULL);
            if (t2->type != INT_TYPE
                && t2->type != FLOAT64_TYPE
                && t2->type != RUNE_TYPE
                && t2->type != BOOL_TYPE
                && t2->type != STRING_TYPE ) typeErr(tree->children->begin, t1, NULL);

            arraylen = ((token_t*)(getFirstTerminal(tree->children->begin->next)->data))->ival;
            returnType = type_obj_createArr(t2->type, arraylen);
            free(t1);
            free(t2);
            return returnType;
        case tag_othertype_map:
            // Fluff [ VAL ] VAL

            t1 = typeCheck(tree->children->begin->next->next, scope);   // FromType
            t2 = typeCheck(tree->children->end, scope);                 // ToType

            if (!t1) {
                token_t* first = getFirstTerminal(tree)->data;
                fprintf(stderr, "%s:%i Map initializer must have \"from\" value\n", first->filename, first->lineno);
                exit(3);
            }
            if (t1->type != INT_TYPE
                && t1->type != FLOAT64_TYPE
                && t1->type != RUNE_TYPE
                && t1->type != BOOL_TYPE
                && t1->type != STRING_TYPE ) typeErr(tree->children->begin, t1, t2);
            if (t2->type != INT_TYPE
                && t2->type != FLOAT64_TYPE
                && t2->type != RUNE_TYPE
                && t2->type != BOOL_TYPE
                && t2->type != STRING_TYPE ) typeErr(tree->children->begin, t1, t2);

            returnType = type_obj_createMap(t1->type, t2->type);
            free(t1);
            free(t2);
            return returnType;
        case tag_pexpr_no_paren_dot:
            // Lib/Struct DOT Func
            vt = cfuhash_get(hashTable, ((token_t*)(getFirstTerminal(tree->children->begin)->data))->text);
            vt = cfuhash_get(vt->symTab, ((token_t*)(getFirstTerminal(tree->children->end)->data))->text);
            return type_obj_copy(vt->type);

        case tag_xfndcl:
            scope = getFuncName(tree);
            // No break here
        default:
            // node didn't need checked (but it's children might)
            switch (tree->count) {
                case 0: return getLeafType(tree, scope);
                case 1: return typeCheck(tree->children->begin, scope);
                default:
                    it = node_iterator_create(tree->children);
                    while ((node = node_iterator_next(it))) {
                        typeCheck(node, scope);
                    }
                    return 0;
            }
            break;

    }
    fprintf(stderr, "Error Occured (Impossible state reached during type checking)\n");
    exit(3);
}

void typeErr(node_t* operatorTree, type_t* type, type_t* type2) {
    token_t* first = getFirstTerminal(operatorTree)->data;
    if (type2 == NULL) {
        // unary
        fprintf(stderr, "%s:%d Type %s is not compatible with operator '%s'\n", first->filename, first->lineno, getTypeName(type), first->text);
    }
    else {
        // Binary
        fprintf(stderr, "%s:%d Type %s is not compatible with type %s using operator '%s'\n", first->filename, first->lineno, getTypeName(type), getTypeName(type2), first->text);
    }
    exit(3);
}

type_t* isCompatibleType(type_t* operator, type_t* type1, type_t* type2) {
    switch (operator->type) {
        case LINC:
        case LDEC:
            if (type1->type == INT_TYPE) return type1;
            else return 0;
        case ',':
        case '=':
            if (type_obj_equals(type1, type2)) return type1;
            else return 0;
        case LLT:
        case LLE:
        case LGT:
        case LGE:
        case LEQ:
            if ((type_obj_equals(type1, type2)) && (type1->type == INT_TYPE || type1->type == FLOAT64_TYPE || type1->type == BOOL_TYPE || type1->type ==RUNE_TYPE)) return type_obj_create(BOOL_TYPE);
            else return 0;
        case LANDAND:
        case LOROR:
            if (type_obj_equals(type1, type2) && type1->type == BOOL_TYPE) return type_obj_create(BOOL_TYPE);
            else return 0;
        case '+':
        case LPLASN:
            if (type_obj_equals(type1, type2) && (type1->type == INT_TYPE || type1->type == FLOAT64_TYPE || type1->type == STRING_TYPE)) return type1;
            else return 0;
        case LMIASN:
        case '-':
        case '*':
        case '/':
            if (type_obj_equals(type1, type2) && (type1->type == INT_TYPE || type1->type == FLOAT64_TYPE)) return type1;
            else return 0;
        case '!':
            if (type1->type == BOOL_TYPE) return type_obj_create(BOOL_TYPE);
            else return 0;
        default:
            fprintf(stderr, "Unknown operator: %i\n", operator->type);
            exit(3);
    }

    return 0;
}
