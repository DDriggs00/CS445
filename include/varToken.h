#ifndef VARTOKEN_H
#define VARTOKEN_H

#include <stdbool.h>

#include "cfuhash.h"
#include "type.h"

typedef struct varToken_t {

    char* scope;

    char* name;
    type_t* type;
    int lineDeclared;

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
varToken_t* varToken_create_arr(char* scope, char* name, int type, int line);
varToken_t* varToken_create_map(char* scope, char* name, int from, int to, int line);
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
char* varToken_typeString(varToken_t* token);
// Removes the token
void varToken_remove();

#endif // VARTOKEN_H