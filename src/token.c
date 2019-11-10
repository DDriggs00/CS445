#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "token.h"
#include "vgo.tab.h"

// Exit codes:
// -1: Memory Error

char* formatString(char* s) {
    if (!s) return NULL;

	char* iterator = s;
	char* newString = calloc(sizeof(char), strlen(s) + 1);

    int pos = 0;
	while(*iterator != '\0')
    {
        if(*iterator == '\"') {
            // Ignore
        }
        else if(*iterator == '\\') {
            iterator++;
            switch(*iterator) {
                case 'n':
                    newString[pos] = '\n';
                    pos++;
                    break;
                case 't':
                    newString[pos] = '\t';
                    pos++;
                    break;
                case '"':
                    newString[pos] = '"';
                    pos++;
                    break;
                case '\'':
                    newString[pos] = '\'';
                    pos++;
                    break;
                default:
                    newString[pos] = '\\';
                    pos++;
                    newString[pos] = *iterator;
                    pos++;
                    break;
            }
        }
        else {
            newString[pos] = *iterator;
            pos++;
        }
        iterator++;
    }
    newString[pos] = '\0';

	return newString;
}

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
	t->text = calloc(sizeof(char), strlen(text) + 1);
	strcpy(t->text, text);

	t->filename = calloc(sizeof(char), strlen(filename) + 1);
	strcpy(t->filename, filename);

    // Copy value

    if (category == LINT) {
        t->ival = atoi(text);
    }
    if (category == LFLOAT) {
        t->dval = atof(text);
    }
    if (category == LLITERAL) {
        t->sval = formatString((text));
    }

	return t;
}

void tokenRemove(struct token *t) {
    if (!t) return;

    // First free text values
	free(t->text);
	free(t->filename);
	if (t->category == LLITERAL)
		free(t->sval);

    // Then free the token
	free(t);
}

void tokenPrint(struct token *t) {
    if (!t) return;

    printf("%-i\t %-20s\t%-3i\t%-20s\t", t->category, t->text, t->lineno, t->filename);

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