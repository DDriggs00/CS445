#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>

#include "node.h"

#define NULL_TYPE    10000
#define STRUCT_TYPE  10002
#define ARRAY_TYPE   10003
#define FUNC_TYPE    10005
#define MAP_TYPE     10006
#define MAIN_TYPE    10007
#define INT_TYPE     10266
#define STRING_TYPE  10267
#define BOOL_TYPE    10268
#define FLOAT64_TYPE 10269
#define RUNE_TYPE    10270

typedef struct type_t {
    int type;       // The main type (int, char, array)
    int subType1;   // The primary subtype (INT array, map FROM string)
    int subType2;   // Secondary subtype (map TO int)
    int arrSize;
} type_t;

type_t* type_obj_create(int type);
type_t* type_obj_createArr(int type, int size);
type_t* type_obj_createMap(int typeFrom, int typeTo);

// returns the string representing the typename
char* getTypeName(type_t* type);

// returns the datatype of the leaf node
type_t* getLeafType(node_t* leaf, char* scope);

// Converts flex symbol to proper value
int getProperTypeInt(int oldType);

bool type_obj_equals(type_t* a, type_t* b);

#endif // TYPE_H