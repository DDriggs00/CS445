// Devin Driggs

#include <stdlib.h>     // For basically everything
#include <stdio.h>      // printf, NULL
#include <string.h>     // strlen, strcpy
#include <stdbool.h>    // Adds boolean support

#include "token.h"
#include "vgo.tab.h"

#include "node.h"
#include "node_list.h"
#include "node_iterator.h"

// extern char *yytext;
extern FILE *yyin;          // For sending the file to flex+bison
extern int yylineno;        // For resetting lineno in each new file

struct node_t* yytree;      // For receiving the tree from bison

char** fileNames;
char* currentFile;
bool insertSemicolon = false;

int yyparse();
void treePrint(node_t* root);
bool endsWith(const char *str, const char *suffix);
bool hasExtention(const char* filename);

// Return Codes:
// 0: Success
// 1: Lexical Error
// 2: Parsing Error
// -1: Other errors

int main(int argc, char* argv[]) {
    
    int returnval = 0;

    if(argc == 1){
        fprintf(stderr, "No files were given\n");
        return -1;
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
            fileNames[i - 1] = (char*)malloc(sizeof(char) * (strlen(argv[i]) + 4)); // Allocate memory for individual string (+3 chars for )
            strcpy(fileNames[i - 1], argv[i]);
            fileNames[i - 1] = strcat(fileNames[i - 1], ".go");
        }
    }
    // Create an array of pointers to parse trees
    node_t** treeRoots = malloc(sizeof(node_t*) * (argc - 1));

    // For each filename, parse file
    for(int i = 0; i < argc - 1; i++) {
        yylineno = 1; // Reset line counter to 0
        
        // prepare input file
        currentFile = fileNames[i]; //Needed for yylex
        yyin = fopen(currentFile, "r");
        if(yyin == NULL){
            printf("invalid File: %s\n", currentFile);
            return -1;
        }

        // Parse with Bison
        yyparse();
        treeRoots[i] = yytree;
        printf("Tree for file: %s\n", currentFile);
        treePrint(treeRoots[i]);
    }
    
    for(int i = 0; i < argc - 1; i++) {
        node_destroy(treeRoots[i]);
        // TODO: Destroy tokens as well
    }


    return  returnval;
}

void treePrint(node_t* root) {
    node_t* node;
    for (size_t i = 0; i < root->depth; i++)
    {
        printf("  ");
    }
    if (root->hasData) {
        struct token* t = root->data;
        printf("TOKEN %i: %s\n", t->category, t->text);
    }
    else {
        printf("%s: %i\n", root->type, root->count);
    }
    if (root->count <= 0)
    {
        return;
    }
    node_iterator_t* it = node_iterator_create(root->children);
    while ((node = node_iterator_next(it))) {
        node->depth = root->depth + 1;
        treePrint(node);
    }
    node_iterator_destroy(it);
}


// https://stackoverflow.com/questions/744766
bool endsWith(const char *str, const char *suffix) {
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
