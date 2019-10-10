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

struct token* tokenNew(int category,    // Allocates and creates a new token
                       char* text,
                       int lineno,
                       char* filename);
void tokenRemove(struct token* t);      // Deletes a token and clears its memory
void tokenPrint(struct token* t);       // Prints out a token to the console
char* formatString(char* s);            // converts /t, /n, etc. to escaped versions

#endif /* TOKEN_H */
