#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "varToken.h"


varToken_t* varToken_create(char* scope, char* name, int type, int line) {

    // Create token
    varToken_t* vt = malloc(sizeof(*vt));
    if (vt == NULL) return NULL;

    vt->scope = calloc(sizeof(char), (strlen(scope) + 1));
    strcpy(vt->scope, scope);

    vt->name = calloc(sizeof(char), (strlen(name) + 1));
    strcpy(vt->name, name);

    vt->type = type_obj_create(type);

    vt->lineDeclared = line;
    vt->isInitialized = false;
    vt->isConst = false;
    vt->ival = 0;
    vt->dval = 0.0f;
    vt->sval = NULL;
    vt->symTab = NULL;

    return vt;
}

varToken_t* varToken_create_int(char* scope, char* name, int val, int line, bool isConst) {
    varToken_t* vt = varToken_create(scope, name, INT_TYPE, line);

    vt->isConst = isConst;
    vt->isInitialized = true;
    vt->ival = val;

    return vt;
}
varToken_t* varToken_create_str(char* scope, char* name, char* val, int line, bool isConst) {
    varToken_t* vt = varToken_create(scope, name, STRING_TYPE, line);

    vt->isConst = isConst;
    vt->isInitialized = true;
    vt->sval = calloc(sizeof(char), (strlen(val) + 1));
    strcpy(vt->sval, val);

    return vt;
}

varToken_t* varToken_create_float(char* scope, char* name, double val, int line, bool isConst) {
    varToken_t* vt = varToken_create(scope, name, FLOAT64_TYPE, line);

    vt->isConst = isConst;
    vt->dval = val;

    return vt;
}

varToken_t* varToken_create_map(char* scope, char* name, int from, int to, int line) {
    varToken_t* vt = varToken_create(scope, name, MAP_TYPE, line);
    vt->type->subType1 = getProperTypeInt(from);
    vt->type->subType2 = getProperTypeInt(to);
    return vt;
}

varToken_t* varToken_create_arr(char* scope, char* name, int type, int line) {
    varToken_t* vt = varToken_create(scope, name, ARRAY_TYPE, line);
    vt->type->subType1 = getProperTypeInt(type);
    return vt;
}

varToken_t* varToken_create_struct(char* scope, char* name, int line, cfuhash_table_t* ht) {
    varToken_t* vt = varToken_create(scope, name, STRUCT_TYPE, line);

    vt->isConst = false;
    vt->isInitialized = true;
    vt->symTab = ht;

    return vt;
}

varToken_t* varToken_create_struct_instance(char* scope, char* name, char* structName, int line, cfuhash_table_t* ht) {
    varToken_t* vt = varToken_create(scope, name, STRUCT_INSTANCE_TYPE, line);

    vt->isConst = false;
    vt->isInitialized = true;
    vt->symTab = ht;
    vt->type->structName = calloc(strlen(structName) + 1, sizeof(char));
    strcpy(vt->type->structName, structName);

    return vt;
}

varToken_t* varToken_create_func(char* scope, char* name, type_t* returnType, int line, cfuhash_table_t* ht) {
    varToken_t* vt = varToken_create(scope, name, FUNC_TYPE, line);

    vt->isConst = false;
    vt->isInitialized = true;
    vt->symTab = ht;
    vt->type->funcType = returnType;

    return vt;
}

varToken_t* varToken_create_lib(char* scope, char* name, int line, cfuhash_table_t* ht) {
    varToken_t* vt = varToken_create(scope, name, LIB_TYPE, line);

    vt->isConst = true;
    vt->isInitialized = true;
    vt->symTab = ht;

    return vt;
}

// Assign the given value to the given variable.
// Returns true upon success, false upon failure.
bool varToken_set_int(varToken_t* token, int val) {
    if (!token) return false;
    if (token->type->type != INT_TYPE) return false;
    if (token->isConst) return false;

    token->isInitialized = true;
    token->ival = val;

    return true;
}

bool varToken_set_str(varToken_t* token, char* val) {
    if (!token) return false;
    if (token->type->type != STRING_TYPE) return false;
    if (token->isConst) return false;

    token->isInitialized = true;
    token->sval = calloc(sizeof(char), (strlen(val) + 1));
    strcpy(token->sval, val);

    return true;
}

bool varToken_set_float(varToken_t* token, double val) {
    if (!token) return false;
    if (token->type->type != FLOAT64_TYPE) return false;
    if (token->isConst) return false;

    token->isInitialized = true;
    token->dval = val;

    return true;
}

bool varToken_set_symTab(varToken_t* token, cfuhash_table_t* ht) {
    if (!token) return false;
    if (token->type->type != STRUCT_TYPE && token->type->type != FUNC_TYPE) return false;
    if (token->isConst) return false;

    token->isInitialized = true;
    token->symTab = ht;

    return true;
}

void varToken_print(varToken_t* token) { //TODO cleanup
    if (!token) return;

    printf("Scope: %-10s Name: %-10s ", token->scope, token->name);
    printf("%s ", getTypeName(token->type));
    if (token->isConst) {
        printf("\tConstant");
    }
    printf("\n");
}

void varToken_remove(varToken_t* token) {
    if (!token) return;
    free(token->name);
    free(token->scope);
    free(token->type);
    if (token->type->type == STRING_TYPE) {
        free(token->sval);
    }

    free(token);
}
