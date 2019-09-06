// Devin Driggs


#include <stdlib.h>
#include <stdio.h>      // printf
#include <string.h>     // strlen, strcpy
#include <stdbool.h>    // Adds boolean support

#include "token.h"
#include "vgo.tab.h"
#include "list.h"

extern FILE *yyin;
extern char *yytext;

struct token* yytoken;
list_t* tokenList;
char** fileNames;
char* currentFile;
// Return Codes:
// 0: Success
// 1: Lexical Error
// 2: No Input

// https://stackoverflow.com/questions/744766
bool endsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return false;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return false;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

// derived from https://stackoverflow.com/questions/5309471/
bool hasExtention(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if(!dot || dot == filename) return false;
    char* pPosition = strchr(dot + 1, '/');
    if(pPosition) return false;
    return true;
}

// // Based on https://github.com/andschwa/partial-cpp-compiler/blob/hw1/main.c
// void runLexer()
// {
// 	while (true) {
// 		int category = yylex();
// 		if (category == 0) {
// 			break;
// 		}
// 		list_push(tokens, (union data)yytoken);
// 	}
// 	yylex_destroy();
// 	return;
// }

int main(int argc, char* argv[]) {
    
    if(argc == 1){
		fprintf(stderr, "No files were given\n");
		return 1;
	}

    // create new array in memory, containing filenames
	fileNames = (char**)malloc(sizeof(char*) * (argc - 1)); // Allocate memory for array
	for (int i = 1; i < argc; i++) {
        if (endsWith(argv[i], ".go")) {
            fileNames[i - 1] = (char*)malloc(sizeof(char) * (strlen(argv[i]) + 1)); // Allocate memory for individual string
            strcpy(fileNames[i - 1], argv[i]);
        }
        else {
            // does not have .go extention
            if (hasExtention(argv[i])) {
                fprintf(stderr, "Only files with a .go extention are allowed\n");
                return -1;
            }
            fileNames[i - 1] = (char*)malloc(sizeof(char) * (strlen(argv[i]) + 3)); // Allocate memory for individual string (+3 chars for )
            strcpy(fileNames[i - 1], argv[i]);
            fileNames[i - 1] = strcat(fileNames[i - 1], ".go");
        }
	}
    int yyreturn;
    tokenList = list_new();
    printf("List of input files:\n");
    for(int i = 0; i < argc - 1; i++) {
        
        currentFile = fileNames[i];
        
        printf("%s\n", currentFile);
        yyin = fopen(currentFile, "r");
		if(yyin == NULL){
			printf("invalid File: %s", currentFile);
			return -1;
		}
        while((yyreturn = yylex()) != 0) {
            list_rpush(tokenList, list_node_new(yytoken));
        }
    }
    
    // Initialize token list


    // yylex();

    list_node_t *node;
    list_iterator_t *it = list_iterator_new(tokenList, LIST_HEAD);
    while ((node = list_iterator_next(it))) {
        tokenPrint((node->val));
    }

    return 0;
}
