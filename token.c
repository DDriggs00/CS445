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
	t->text = malloc((strlen(text) + 1) * sizeof(char));
	strcpy(t->text, text);

	t->filename = malloc((strlen(filename) + 1) * sizeof(char));
	strcpy(t->filename, filename);

    // Copy value
    
    if (category == LINT) {
        t->ival = atoi(text);
    }
    if (category == LFLOAT) {
        t->dval = atof(text);
    }
    if (category == LLITERAL) {
        // TODO correct string
        t->sval = malloc((strlen(text) + 1) * sizeof(char));
	    snprintf(t->sval, strlen(text) + 1, "%s", text);
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
    printf("Category: %i\tText: %s\t line: %i\t filename: %s\t", t->category, t->text, t->lineno, t->filename);
        
    if (t->category == LINT) {
        printf("content: %i\n", t->ival);
    }
    else if (t->category == LFLOAT) {
        printf("content: %f\n", t->dval);
    }
    else if (t->category == LLITERAL) {
        printf("content: %s\n", t->sval);
    }
    else {
        printf("\n");
    }

}