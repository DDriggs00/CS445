#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "token.h"
#include "vgo.tab.h"

// Exit codes:
// -1: Memory Error

struct token *tokenNew(int category,
                       char* text,
                       int lineno,
                       char* filename) {

	struct token *t = malloc(sizeof(*t));
	if (t == NULL)
		exit(-1);

    // Copy ints
	t->category = category;
	t->lineno = lineno;

    // Copy strings
	t->text = malloc(strlen(text) * sizeof(char));
	strcpy(t->text, text);

	t->filename = malloc(strlen(filename) * sizeof(char));
	strcpy(t->filename, filename);

    // Copy value
    
    if (category == LINT) {
        t->ival = atoi(text);
    }
    if (category == LDOUBLE) {
        t->ival = atof(text);
    }
    if (category == LLITERAL) {
        // TODO correct string
        t->sval = malloc(strlen(text) * sizeof(char));
	    sprintf(t->sval, "%s", text);
    }

	return t;
}

void tokenRemove(struct token *t) {
    // First free text values
	free(t->text);
	free(t->filename);
	if (t->category == LLITERAL)
		free(t->sval);

    // Then free the token
	free(t);
}

void tokenPrint(struct token *t) {
    return;
}