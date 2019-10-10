#ifndef TOKEN_H
#define TOKEN_H 

typedef struct token {
    int category;   /* the integer code returned by yylex */
    char *text;     /* the actual string (lexeme) matched */
    int lineno;     /* the line number on which the token occurs */
    char *filename; /* the source file in which the token occurs */
    int ival;       /* for integer constants, store binary value here */
    double dval;	   /* for real constants, store binary value here */
    char *sval;     /* for string constants, malloc space, de-escape, store */
                   /*    the string (less quotes and after escapes) here */
} token_t;

// Allocates and creates a new token
// Category is the enum for that character, defined in vgo.tab.h
// text is the text to use as the token body
// Lineno: The line the text is found on
// filename: the name of the file 
struct token* tokenNew(int category, char* text, int lineno, char* filename);

// Frees a token
void tokenRemove(struct token* t);

// Prints out a token to the console
void tokenPrint(struct token* t);

// converts /t, /n, etc. to escaped versions
// Used internally
char* formatString(char* s);

#endif /* TOKEN_H */
