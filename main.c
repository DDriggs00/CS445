// Devin Driggs


#include <stdlib.h>
#include <stdio.h>      // printf
#include <string.h>     // strlen, strcpy
#include <stdbool.h>    // Adds boolean support

#include "token.h"

struct token* yytoken;
struct tokenList* tokens;
char** filenames;

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

int main(int argc, char* argv[]) {
    
    if(argc == 1){
		// No file exists
		return 1;
	}

    // create new array in memory, containing filenames
    char** fileNames;
	fileNames = (char**)malloc(sizeof(char*) * (argc - 1)); // Allocate memory for array
	for (int i = 1; i < argc; i++)
	{
        if (endsWith(argv[i], ".go")) {
            fileNames[i - 1] = (char*)malloc(sizeof(char) * (strlen(argv[i]))); // Allocate memory for each string
            strcpy(fileNames[i - 1], argv[i]);
        }
        else {
            // does not have .go extention
            fileNames[i - 1] = (char*)malloc(sizeof(char) * (strlen(argv[i]) + 3)); // Allocate memory for each string
            strcpy(fileNames[i - 1], argv[i]);
            fileNames[i - 1] = strcat(fileNames[i - 1], ".go");
        }
        printf("%s\n", fileNames[i - 1]);
	}

    return 0;
}
