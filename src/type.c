#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type.h"
#include "vgo.tab.h"    // For lexing tokens
#include "nodeTypes.h"  // for types
#include "token.h"      // for tokens in getType
#include "varToken.h"   // for tokens in getType

type_t* type_obj_create(int type) {
    type_t* typeObj = malloc(sizeof(type_t));
    typeObj->type = getProperTypeInt(type);
    typeObj->subType1 = 0;
    typeObj->subType2 = 0;
    typeObj->arrSize = 0;
    typeObj->structName = NULL;
    typeObj->funcType = NULL;
    return typeObj;
}

type_t* type_obj_createArr(int type, int size) {
    type_t* typeObj = type_obj_create(ARRAY_TYPE);
    typeObj->subType1 = getProperTypeInt(type);
    typeObj->arrSize = getProperTypeInt(size);
    return typeObj;
}

type_t* type_obj_createMap(int typeFrom, int typeTo) {
    type_t* typeObj = type_obj_create(MAP_TYPE);
    typeObj->subType1 = getProperTypeInt(typeFrom);
    typeObj->subType2 = getProperTypeInt(typeTo);
    return typeObj;
}

type_t* type_obj_createStruct(char* structName) {
    type_t* typeObj = type_obj_create(STRUCT_INSTANCE_TYPE);
    char* name = calloc(strlen(structName), sizeof(char));
    strcpy(name, structName);
    typeObj->structName = name;
    return typeObj;
}

type_t* type_obj_createFunc(type_t* funcType) {
    type_t* typeObj = type_obj_create(FUNC_TYPE);
    typeObj->funcType = funcType;
    return typeObj;
}

type_t* type_obj_copy(type_t* source) {
    type_t* typeObj = malloc(sizeof(type_t));
    typeObj->type = source->type;
    typeObj->subType1 = source->subType1;
    typeObj->subType2 = source->subType2;
    typeObj->arrSize = source->arrSize;
    typeObj->funcType = source->funcType;
    return typeObj;
}

type_t* getType(node_t* leaf, cfuhash_table_t* rootHT, char* scope) {

    // skip if not leaf
    if (!leaf) return NULL;
    // if (leaf->count > 0) return NULL;

    // skip empty leaves
    token_t* token = leaf->data;;

    varToken_t* vt; // Must be oustide switch
    varToken_t* func; // Must be oustide switch

    switch (leaf->tag) {
        // Base types
        case LTYPEINT:
        case LINT:          return type_obj_create(INT_TYPE);
        case LTYPEFLOAT64:
        case LFLOAT:        return type_obj_create(FLOAT64_TYPE);
        case LTYPESTRING:
        case LLITERAL:      return type_obj_create(STRING_TYPE);
        case LTYPEBOOL:
        case LBOOL:         return type_obj_create(BOOL_TYPE);
        case LTYPERUNE:
        case LRUNE:         return type_obj_create(RUNE_TYPE);
        
        case tag_empty:     return type_obj_create(VOID_TYPE);
        
        
        // structs and imports
        case LNAME:
            if (!token) return NULL;

            if (scope) {
                func = cfuhash_get(rootHT, scope);
                vt = cfuhash_get(func->symTab, token->text);
                if (vt) return type_obj_copy(vt->type);
            }
            vt = cfuhash_get(rootHT, token->text);
            if (!vt) {
                return NULL;
            }
            if (vt->type->type == FUNC_TYPE) return type_obj_copy(vt->type->funcType);
            else return type_obj_copy(vt->type);
        
        // functions
        default:  
            if (!token) return NULL;      
            return type_obj_create(token->category);
    }

    // impossible state
    return 0;
}

char* getTypeName(type_t* type) {
    char* buffer = calloc(sizeof(char), 50);

    switch(type->type) {
        case VOID_TYPE:
            buffer = strcat(buffer, "Void");
            break;

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
            buffer = strcat(buffer, "Function (");
            buffer = strcat(buffer, getTypeName(type->funcType));
            buffer = strcat(buffer, ")");
            break;

        case STRUCT_TYPE:
            buffer = strcat(buffer, "Struct");
            break;

        case STRUCT_INSTANCE_TYPE:
            buffer = strcat(buffer, type->structName);
            break;

        case LIB_TYPE:
            buffer = strcat(buffer, "Library");
            break;

        case ARRAY_TYPE:
            buffer = strcat(buffer, "Array");
            switch (type->subType1) {
                case INT_TYPE:
                    buffer = strcat(buffer, " int)");
                    break;
                case FLOAT64_TYPE:
                    buffer = strcat(buffer, "(float)");
                    break;
                case STRING_TYPE:
                    buffer = strcat(buffer, "(str)");
                    break;
                case RUNE_TYPE:
                    buffer = strcat(buffer, "(rune)");
                    break;
                case BOOL_TYPE:
                    buffer = strcat(buffer, "(bool)");
                    break;
                default:
                    buffer = strcat(buffer, "(typedef'd other)");
            }
            break;

        case MAP_TYPE:
            buffer = strcat(buffer, "Map");
            switch (type->subType1) {
                case INT_TYPE:
                    buffer = strcat(buffer, "(int->");
                    break;
                case FLOAT64_TYPE:
                    buffer = strcat(buffer, "(float->");
                    break;
                case STRING_TYPE:
                    buffer = strcat(buffer, "(str->");
                    break;
                case RUNE_TYPE:
                    buffer = strcat(buffer, "(rune->");
                    break;
                case BOOL_TYPE:
                    buffer = strcat(buffer, "(bool->");
                    break;
                default:
                    buffer = strcat(buffer, "(typedef'd other->");
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
                    buffer = strcat(buffer, "typedef'd other)");
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
        case 266:   return INT_TYPE;
        case 267:   return STRING_TYPE;
        case 268:   return BOOL_TYPE;
        case 269:   return FLOAT64_TYPE;
        case 270:   return RUNE_TYPE;
        case 1136:  return MAP_TYPE;
        case 1135:  return ARRAY_TYPE;
        default:    return oldType;
    }
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