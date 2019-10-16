#ifndef VARTOKEN_H
#define VARTOKEN_H

#include <stdbool.h>

#include "cfuhash.h"

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

typedef struct varToken_t {
    
    char* scope;

    char* name;
    int type;
    int lineDeclared;

    // For array and map
    int subType1, subType2;
    int arrSize;

    bool isConst;
    bool isInitialized;
    
    int ival;
    char* sval;
    double dval;
    cfuhash_table_t* symTab;

} varToken_t;

// Creates a varToken with the given parameters
varToken_t* varToken_create(char* scope, char* name, int type, int line);
varToken_t* varToken_create_int(char* scope, char* name, int val, int line, bool isConst);
varToken_t* varToken_create_str(char* scope, char* name, char* val, int line, bool isConst);
varToken_t* varToken_create_float(char* scope, char* name, double val, int line, bool isConst);
varToken_t* varToken_create_struct(char* scope, char* name, int line, cfuhash_table_t* ht);
varToken_t* varToken_create_func(char* scope, char* name, int line, cfuhash_table_t* ht);

// Assign the given value to the given variable.
// Returns true upon success, false upon failure.
bool varToken_set_int(varToken_t* token, int val);
bool varToken_set_str(varToken_t* token, char* val);
bool varToken_set_float(varToken_t* token, double val);
bool varToken_set_symTab(varToken_t* token, cfuhash_table_t* ht);

// Prints token details
void varToken_print(varToken_t* token);

// Removes the token
void varToken_remove();

// Converts flex symbol to proper value
int getProperTypeInt(int oldType);

#endif // VARTOKEN_H