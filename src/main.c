// Devin Driggs

#include <stdlib.h>     // For basically everything
#include <stdio.h>      // printf, NULL
#include <string.h>     // strlen, strcpy
#include <stdbool.h>    // Adds boolean support

#include "vgo.tab.h"    // Bison and flex (autogenerated)

#include "symtab.h"     // for generating symbol table

#include "token.h"          // For flex-generated tokens
#include "node.h"           // For tree
#include "list.h"           // For list of symbol tables
#include "node_list.h"      // For tree
#include "node_iterator.h"  // For printing out tree
#include "node_iterator_full.h" // testing
#include "nodeTypes.h"      // For more easily navigating the tree
#include "cfuhash.h"        // For symbol tables

// extern char *yytext;
extern FILE *yyin;          // For sending the file to flex+bison
extern int yylineno;        // For resetting lineno in each new file

struct node_t* yytree;      // For receiving the tree from bison

char** fileNames;
char* currentFile;
bool insertSemicolon = false;

int yyparse();
void treeFix(node_t* root);
void treePrint(node_t* root, int name);
void createFileList(int count, char** args);
bool endsWith(const char *str, const char *suffix);
bool hasExtention(const char* filename);

// Return Codes:
// 0: Success
// 1: Lexical Error
// 2: Parsing Error
// 3. Semantic Error (including Go-Not-VGo)
// -1: Other errors

int main(int argc, char* argv[]) {
    
    int returnval = 0;
    int nonFileArguments = 1;
 
    // ========================================
    // ===== Step 0: File list generation =====
    // ========================================
   
    if (argc == 1){
        fprintf(stderr, "No files were given\n");
        return -1;
    }
    createFileList(argc, argv);

    // ========================================
    // ======= Step 1: Tree Generation ========
    // ========================================

    // Create an array of pointers to parse trees
    node_t** treeRoots = malloc(sizeof(node_t*) * (argc - nonFileArguments));

    // For each filename, parse file
    for (int i = 0; i < argc - nonFileArguments; i++) {
        yylineno = 1; // Reset line counter to 0
        
        // Prepare and validate input file
        currentFile = fileNames[i]; //Needed for yylex
        yyin = fopen(currentFile, "r");
        if (yyin == NULL){
            printf("invalid File: %s\n", currentFile);
            return -1;
        }

        // Parse with Bison (which calls flex)
        yyparse();
        treeRoots[i] = yytree;
        treeFix(treeRoots[i]);
    }

    // ========================================
    // ===== Step 2: Variable Extraction ======
    // ========================================

    node_iterator_full_t* it2 = node_iterator_full_create(treeRoots[0]);
    node_t* node;
    while ((node = node_iterator_full_next(it2))) {
        for (int i = 0; i < node->depth; i++)
        {
            printf("  ");
        }
        if (node->hasData) {
            struct token* t = node->data;
            printf("TOKEN %i: %s\n", t->category, t->text);
        }
        else {
            printf("%i: %i\n", node->type, node->count);
        }
    }


    // cfuhash_table_t *ht = cfuhash_new();
    // cfuhash_put(ht, "var1", "value1");
    // cfuhash_delete(ht, "var3");
    // cfuhash_pretty_print(ht, stdout);
    // char* val = cfuhash_get(ht, "var2");

    // Create table of hashtable pointers
    // node_t** hashTables = malloc(sizeof(cfuhash_table_t*) * (argc - nonFileArguments));
    
    // Parse out variables and put them into hashtables
    for (int i = 0; i < argc - nonFileArguments; i++) {
        // hashTables[i] = genSymTab(treeRoots[i]);
        // Generate symbol tables
        
        // treePrint(treeRoots[i], tag_vardcl);
        // TODO print list
    }
    
    // ========================================
    // =============== Cleanup ================
    // ========================================

    for (int i = 0; i < argc - nonFileArguments; i++) {
        node_destroy(treeRoots[i]);
        // TODO: Destroy tokens as well
    }

    return  returnval;
}

void treePrint(node_t* root, int type) {
    node_t* node;
    if (type == 0 || type == root->type) {
        for (int i = 0; i < root->depth; i++)
        {
            printf("  ");
        }
        if (root->hasData) {
            struct token* t = root->data;
            printf("TOKEN %i: %s\n", t->category, t->text);
        }
        else {
            printf("%i: %i\n", root->type, root->count);
        }
    }
    if (root->count <= 0)
    {
        return;
    }
    node_iterator_t* it = node_iterator_create(root->children);
    while ((node = node_iterator_next(it))) {
        node->depth = root->depth + 1;
        
        if (type == 0 || type == root->type) {
            treePrint(node, 0);
        }
        else {
            treePrint(node, type);
        }
    }
    node_iterator_destroy(it);
}

// Fixes the depth variable in trees built backwards
void treeFix(node_t* root) {
    node_t* node;
    if (root->count <= 0)
    {
        return;
    }
    node_iterator_t* it = node_iterator_create(root->children);
    while ((node = node_iterator_next(it))) {
        node->depth = root->depth + 1;
        node->isLeaf = (node->count == 0);
        node->isRoot = (node->parent == NULL);
        treeFix(node);
    }
    node_iterator_destroy(it);
}

// Uses global variable as output
void createFileList(int count, char** args) {
    // create new array in memory, containing filenames
    fileNames = (char**)malloc(sizeof(char*) * (count - 1)); // Allocate memory for array
    for (int i = 1; i < count; i++) {
        if (endsWith(args[i], ".go")) {
            fileNames[i - 1] = (char*)malloc(sizeof(char) * (strlen(args[i]) + 1)); // Allocate memory for individual string
            strcpy(fileNames[i - 1], args[i]);
        }
        else {
            // does not have .go extentions
            if (hasExtention(args[i])) {
                fprintf(stderr, "Only files with a .go extention are allowed\n");
                exit(-1);
            }
            fileNames[i - 1] = (char*)malloc(sizeof(char) * (strlen(args[i]) + 4)); // Allocate memory for individual string (+3 chars for)
            strcpy(fileNames[i - 1], args[i]);
            fileNames[i - 1] = strcat(fileNames[i - 1], ".go");
        }
    }
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
    if (!dot || dot == filename) return false;
    char* pPosition = strchr(dot + 1, '/');
    if (pPosition) return false;
    return true;
}
