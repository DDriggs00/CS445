#include <string.h>
#include <stdbool.h>

#include "strutils.h"

// https://stackoverflow.com/a/4770992
bool startsWith(char* str, char* prefix)
{
    return strncmp(prefix, str, strlen(prefix)) == 0;
}

// https://stackoverflow.com/a/744822
bool endsWith(char* str, char* suffix) {
    if (!str || !suffix)
        return false;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return false;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

// derived from https://stackoverflow.com/a/5309508
bool hasExtention(char* filename) {
    char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return false;
    char* pPosition = strchr(dot + 1, '/');
    if (pPosition) return false;
    return true;
}