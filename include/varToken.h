#ifndef VARTOKEN_H
#define VARTOKEN_H

#include <stdbool.h>

#include "cfuhash.h"

#define NULL_TYPE    10000
#define INT_TYPE     10001
#define STRUCT_TYPE  10002
#define ARRAY_TYPE   10003
#define FLOAT64_TYPE 10004
#define FUNC_TYPE    10005
#define MAP_TYPE     10006
#define MAIN_TYPE    10007
#define STRING_TYPE  10008

typedef struct varToken_t {
    
    char* scope;

    char* name;
    int type;

    bool isConst;
    bool isInitialized;
    
    int ival;
    char* sval;
    double dval;
    cfuhash_table_t* symTab;

} varToken_t;

// Creates a varToken with the given parameters
varToken_t* varToken_create(char* scope, char* name, int type);
varToken_t* varToken_create_int(char* scope, char* name, int val, bool isConst);
varToken_t* varToken_create_str(char* scope, char* name, char* val, bool isConst);
varToken_t* varToken_create_float(char* scope, char* name, double val, bool isConst);
varToken_t* varToken_create_struct(char* scope, char* name, cfuhash_table_t* ht);
varToken_t* varToken_create_func(char* scope, char* name, cfuhash_table_t* ht);

// Assign the given value to the given variable.
// Returns true upon success, false upon failure.
bool varToken_set_int(varToken_t* token, int val);
bool varToken_set_str(varToken_t* token, char* val);
bool varToken_set_float(varToken_t* token, double val);

cfuhash_table_t* varToken_set_symTab(varToken_t* token, cfuhash_table_t* ht);

// Prints token details
varTokenPrint(varToken_t* token);

// Removes the token
void varToken_remove();

#endif // VARTOKEN_H