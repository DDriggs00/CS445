#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type.h"
#include "vgo.tab.h"    // For lexing tokens
#include "token.h"      // for tokens in getLeafType
#include "varToken.h"   // for tokens in getLeafType#include "cfuhash.h"

extern cfuhash_table_t* hashTable; 

type_t* type_obj_create(int type) {
    type_t* typeObj = malloc(sizeof(type_t));
    typeObj->type = getProperTypeInt(type);
    return typeObj;
}

type_t* type_obj_createArr(int type, int size) {
    type_t* typeObj = type_obj_create(ARRAY_TYPE);
    typeObj->subType1 = getProperTypeInt(type);
    typeObj->arrSize = getProperTypeInt(size);
    return typeObj;
}

type_t* type_obj_createMap(int typeFrom, int typeTo) {
    type_t* typeObj = type_obj_create(ARRAY_TYPE);
    typeObj->subType1 = getProperTypeInt(typeFrom);
    typeObj->subType2 = getProperTypeInt(typeTo);
    return typeObj;
}


type_t* getLeafType(node_t* leaf, char* scope) {
    if (!leaf) return NULL;
    
    // skip if not leaf 
    if (leaf->count > 0) return NULL;

    token_t* token = leaf->data;

    // skip empty nodes
    if (!token) return NULL;

    varToken_t* vt; // Must be oustide switch
    varToken_t* func; // Must be oustide switch
    switch (token->category) {
        case LINT:      return type_obj_create(INT_TYPE);
        case LFLOAT:    return type_obj_create(FLOAT64_TYPE);
        case LLITERAL:  return type_obj_create(STRING_TYPE);
        case LBOOL:     return type_obj_create(BOOL_TYPE);
        case LRUNE:     return type_obj_create(RUNE_TYPE);
        case LNAME:
            if (scope) {
                func = cfuhash_get(hashTable, scope);
                vt = cfuhash_get(func->symTab, token->text);
                if (vt) return vt->type2;
            }
            vt = cfuhash_get(hashTable, token->text);
            if (!vt) {
                printf("%s\n", token->text);
                return NULL;
            }
            return vt->type2;
        default:        return type_obj_create(token->category);
    }

    // impossible state
    return 0;

}

char* getTypeName(type_t* type) {
    char* buffer = calloc(sizeof(char), 50);

    switch(type->type) {
        case INT_TYPE:
            buffer = strcat(buffer, "Integer");
            break;

        case RUNE_TYPE:
            buffer = strcat(buffer, "Rune");
            break;

        case BOOL_TYPE:
            buffer = strcat(buffer, "Boolean");
            break;

        case FLOAT64_TYPE:
            buffer = strcat(buffer, "Float");
            break;

        case STRING_TYPE:
            buffer = strcat(buffer, "String");
            break;

        case FUNC_TYPE:
            buffer = strcat(buffer, "Function");
            break;

        case STRUCT_TYPE:
            buffer = strcat(buffer, "Struct");
            break;

        case ARRAY_TYPE:
            buffer = strcat(buffer, "Array");
            switch (type->subType1) {
                case INT_TYPE:
                    buffer = strcat(buffer, " (int)");
                    break;
                case FLOAT64_TYPE:
                    buffer = strcat(buffer, " (float)");
                    break;
                case STRING_TYPE:
                    buffer = strcat(buffer, " (str)");
                    break;
                case RUNE_TYPE:
                    buffer = strcat(buffer, " (rune)");
                    break;
                case BOOL_TYPE:
                    buffer = strcat(buffer, " (bool)");
                    break;
                default:
                    printf(" (typedef'd other)");
            }
            break;

        case MAP_TYPE:
            buffer = strcat(buffer, "Map");
            switch (type->subType1) {
                case INT_TYPE:
                    buffer = strcat(buffer, " (int->");
                    break;
                case FLOAT64_TYPE:
                    buffer = strcat(buffer, " (float->");
                    break;
                case STRING_TYPE:
                    buffer = strcat(buffer, " (str->");
                    break;
                case RUNE_TYPE:
                    buffer = strcat(buffer, " (rune->");
                    break;
                case BOOL_TYPE:
                    buffer = strcat(buffer, " (bool->");
                    break;
                default:
                    printf(" (typedef'd other->");
            }
            switch (type->subType2) {
                case INT_TYPE:
                    buffer = strcat(buffer, "int)");
                    break;
                case FLOAT64_TYPE:
                    buffer = strcat(buffer, "float)");
                    break;
                case STRING_TYPE:
                    buffer = strcat(buffer, "str)");
                    break;
                case RUNE_TYPE:
                    buffer = strcat(buffer, "rune)");
                    break;
                case BOOL_TYPE:
                    buffer = strcat(buffer, "bool)");
                    break;
                default:
                    printf("typedef'd other)");
            }
            break;
    }
    return buffer;
}

bool type_obj_equals(type_t* a, type_t* b) {
    if (a->type != b->type) return false;
    if (a->subType1 != b->subType1) return false;
    if (a->subType2 != b->subType2) return false;
    if (a->arrSize != b->arrSize) return false;
    return true;
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