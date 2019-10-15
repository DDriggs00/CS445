#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "varToken.h"


varToken_t* varToken_create(char* scope, char* name, int type) {
    
    // Create token
    varToken_t* vt = malloc(sizeof(*vt));
    if (vt == NULL) return NULL;

    vt->scope = malloc(sizeof(char) * (strlen(scope) + 1));
    strcpy(vt->scope, scope);

    vt->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(vt->name, name);
    vt->type = getProperTypeInt(type);

    vt->isInitialized = false;

    return vt;
}

varToken_t* varToken_create_int(char* scope, char* name, int val, bool isConst) {
    varToken_t* vt = varToken_create(scope, name, INT_TYPE);

    vt->isConst = isConst;
    vt->isInitialized = true;
    vt->ival = val;

    return vt;
}
varToken_t* varToken_create_str(char* scope, char* name, char* val, bool isConst) {
    varToken_t* vt = varToken_create(scope, name, STRING_TYPE);

    vt->isConst = isConst;
    vt->isInitialized = true;
    vt->sval = malloc(sizeof(char) * (strlen(val) + 1));
    strcpy(vt->sval, val);

    return vt;
}

varToken_t* varToken_create_float(char* scope, char* name, double val, bool isConst) {
    varToken_t* vt = varToken_create(scope, name, FLOAT64_TYPE);

    vt->isConst = isConst;
    vt->isInitialized = true;
    vt->dval = val;

    return vt;
}

varToken_t* varToken_create_struct(char* scope, char* name, cfuhash_table_t* ht) {
    varToken_t* vt = varToken_create(scope, name, STRUCT_TYPE);

    vt->isConst = false;
    vt->isInitialized = true;
    vt->symTab = ht;

    return vt;
}

varToken_t* varToken_create_func(char* scope, char* name, cfuhash_table_t* ht) {
    varToken_t* vt = varToken_create(scope, name, FUNC_TYPE);

    vt->isConst = false;
    vt->isInitialized = true;
    vt->symTab = ht;

    return vt;
}

// Assign the given value to the given variable.
// Returns true upon success, false upon failure.
bool varToken_set_int(varToken_t* token, int val) {
    if (!token) return false;
    if (token->type != INT_TYPE) return false;
    if (token->isConst) return false;

    token->isInitialized = true;
    token->ival = val;

    return true;
}

bool varToken_set_str(varToken_t* token, char* val) {
    if (!token) return false;
    if (token->type != STRING_TYPE) return false;
    if (token->isConst) return false;

    token->isInitialized = true;
    token->sval = malloc(sizeof(char) * (strlen(val) + 1));
    strcpy(token->sval, val);

    return true;
}

bool varToken_set_float(varToken_t* token, double val) {
    if (!token) return false;
    if (token->type != FLOAT64_TYPE) return false;
    if (token->isConst) return false;

    token->isInitialized = true;
    token->dval = val;

    return true;
}

bool varToken_set_symTab(varToken_t* token, cfuhash_table_t* ht) {
    if (!token) return false;
    if (token->type != STRUCT_TYPE && token->type != FUNC_TYPE) return false;
    if (token->isConst) return false;

    token->isInitialized = true;
    token->symTab = ht;

    return true;
}

void varToken_print(varToken_t* token) {
    if (!token) return;

    printf("Scope: %s, Name: %s, ", token->scope, token->name);
    switch(token->type) {
        case INT_TYPE:
            printf("Type: Integer");
            if (token->isInitialized) {
                printf(", Value: %i", token->ival);
            }
            break;

        case RUNE_TYPE:
            printf("Type: Rune");
            if (token->isInitialized) {
                printf(", Value: %s", token->sval);
            }
            break;

        case BOOL_TYPE:
            printf("Type: Boolean");
            if (token->isInitialized) {
                printf(", Value: %i", token->ival);
            }
            break;

        case FLOAT64_TYPE:
            printf("Type: Float");
            if (token->isInitialized) {
                printf(", Value: %f", token->dval);
            }
            break;

        case STRING_TYPE:
            printf("Type: String");
            if (token->isInitialized) {
                printf("Type: String, Value: %s", token->sval);
            }
            break;

        case FUNC_TYPE:
            printf("Type: Function");
            break;

        case STRUCT_TYPE:
            printf("Type: Struct");
            break;

        case ARRAY_TYPE:
            printf("Type: Array");
            break;

        case MAP_TYPE:
            printf("Type: Map");
            break;

        default:
            printf("Unknown Type: %i", token->type);
            break;
    }
    if (token->isConst) {
        printf(", Constant");
    }
    printf("\n");
}


void varToken_remove(varToken_t* token) {
    if (!token) return;
    free(token->name);
    free(token->scope);
    if (token->type == STRING_TYPE) {
        free(token->sval);
    }

    free(token);
}

int getProperTypeInt(int oldType) {
    switch (oldType) {
        case 266: return INT_TYPE;
        case 267: return STRING_TYPE;
        case 268: return BOOL_TYPE;
        case 269: return FLOAT64_TYPE;
        case 270: return RUNE_TYPE;
        default:  return oldType;
    }
}
