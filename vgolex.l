/* Referred to here: https://github.com/SethForrest/Compilers_HW_1/blob/master/clex.l and here: https://github.com/andschwa/partial-cpp-compiler/blob/hw1/clex.l */
%option warn nounput noinput
%option noyywrap
%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <libgen.h>

#include "token.h"
#include "vgo.tab.h"

extern struct token *yytoken;
extern char** filenames;

/* print error message to stderr and terminate (for now) */
void yyerror(const char *s)
{
    fprintf(stderr, "File %s line %d, token %s: %s\n", "test", yylineno, yytext, s);
    exit(1);
}

/* allocates and prepares token, saving pointer into yytoken */
void tokenize(int category)
{
    yytoken = tokenNew(category, yytext, yylineno, "test");
}

%}

%%

((\/\/[^\n]*\n)|([ \r\t\v\f\n])|(\/\*[^*]*\*+(?:[^\/*][^*]*\*+)*\/))* {} /* eat whitespace and  comments */

    /* Reserved words */
"func"          {tokenize(LFUNC); return LFUNC;}
"map"           {tokenize(LMAP); return LMAP;}
"struc"         {tokenize(LSTRUC); return LSTRUC;}
"else"          {tokenize(LELSE); return LELSE;}
"package"       {tokenize(LPACKAGE); return LPACKAGE;}
"const"         {tokenize(LCONST); return LCONST;}
"if"            {tokenize(LIF); return LIF;}
"type"          {tokenize(LTYPE); return LTYPE;}
"for"           {tokenize(LFOR); return LFOR;}
"import"        {tokenize(LIMPORT); return LIMPORT;}
"return"        {tokenize(LRETURN); return LRETURN;}
"var"           {tokenize(LVAR); return LVAR;}

    /* Bad reserved words */
"break"         {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"case"          {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"chan"          {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"fallthrough"   {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"continue"      {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"default"       {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"defer"         {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"goto"          {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"range"         {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"interface"     {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"go"            {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"switch"        {tokenize(LBADKEYWORD); return LBADKEYWORD;}
"select"        {tokenize(LBADKEYWORD); return LBADKEYWORD;}

    /* Type Names */
"bool"          {tokenize(LTYPEBOOL); return LTYPEBOOL;}
"string"        {tokenize(LTYPESTRING); return LTYPESTRING;}
"int"           {tokenize(LTYPEINT); return LTYPEINT;}
"rune"          {tokenize(LTYPERUNE); return LTYPERUNE;}
"float64"       {tokenize(LTYPEFLOAT64); return LTYPEFLOAT64;}

    /* Invalid Operators A */
"&^="           {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"<<="           {tokenize(LBADOPERATOR); return LBADOPERATOR;}
">>="           {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"%="            {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"|="            {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"<-"            {tokenize(LBADOPERATOR); return LBADOPERATOR;}
">>"            {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"<<"            {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"&^"            {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"&="            {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"*="            {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"/="            {tokenize(LBADOPERATOR); return LBADOPERATOR;}
":="            {tokenize(LBADOPERATOR); return LBADOPERATOR;}

    /* Valid Operators */
"<="            {tokenize(LLE); return LLE;}
">="            {tokenize(LGE); return LGE;}
"=="            {tokenize(LEQ); return LEQ;}
"!="            {tokenize(LNE); return LNE;}
"&&"            {tokenize(LANDAND); return LANDAND;}
"||"            {tokenize(LOROR); return LOROR;}
"+="            {tokenize(LPLASN); return LPLASN;}
"-="            {tokenize(LMIASN); return LMIASN;}
"="             {tokenize(LASN); return LASN;}
"("             {tokenize(LLP); return LLP;}
")"             {tokenize(LRP); return LRP;}
"["             {tokenize(LLB); return LLB;}
"]"             {tokenize(LRB); return LRB;}
"<"             {tokenize(LLT); return LLT;}
">"             {tokenize(LGT); return LGT;}
"."             {tokenize(LDOT); return LDOT;}
"!"             {tokenize(LBANG); return LBANG;}
"+"             {tokenize(LPLUS); return LPLUS;}
"-"             {tokenize(LMINUS); return LMINUS;}
"*"             {tokenize(LMUL); return LMUL;}
"/"             {tokenize(LDIV); return LDIV;}
"%"             {tokenize(LMOD); return LMOD;}

    /* Invalid Operators B */
":"             {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"&"             {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"^"             {tokenize(LBADOPERATOR); return LBADOPERATOR;}
"|"             {tokenize(LBADOPERATOR); return LBADOPERATOR;}

    /* Literals */
0[0-7]+                                         { tokenize(LOCT); return LOCT; }            /* Octal */
[1-9][0-9]*                                     { tokenize(LINT); return LINT; }            /* Int */
(([1-9][0-9]*)?\.[0-9]+)                        { tokenize(LFLOAT); return LFLOAT; }        /* Float */
"0x"[a-fA-F0-9]+                                { tokenize(LHEX); return LHEX; }            /* Hex */
"0b"[01]+     			                        { tokenize(LBIN); return LBIN; }            /* Binary */
[a-zA-Z_][a-zA-Z0-9_]*	                        { tokenize(LVARNAME); return LVARNAME; }    /* variable names */
\"(\\|[^\n"\\])*\"		                        { tokenize(LLITERAL); return LLITERAL; }    /* string literals */
\'[^\\\n]\'|\'\\[0abfnrtveABFNRTVE"&'\\?]\'     { tokenize(LRUNE); return LRUNE; }          /* Char (rune) */

%%