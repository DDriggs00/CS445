/* Referred to here: https://github.com/SethForrest/Compilers_HW_1/blob/master/clex.l and here: https://github.com/andschwa/partial-cpp-compiler/blob/hw1/clex.l */
%option warn nounput noinput
%option noyywrap yylineno

%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <string.h>
// #include <ctype.h>
// #include <libgen.h>

#include "token.h"
#include "vgo.tab.h"

extern struct token *yytoken;
extern char** fileNames;
extern char* currentFile;

/* allocates and prepares token, saving pointer into yytoken */
void tokenize(int category)
{
    yytoken = tokenNew(category, yytext, yylineno, currentFile);
}

%}

%%

((\/\/[^\n]*\n)|([ \r\t\v\f\n])|(\/\*[^*]*\*+(?:[^\/*][^*]*\*+)*\/))* {} /* eat whitespace and  comments */

    /* Reserved words */
"func"          {tokenize(LFUNC); return LFUNC;}
"map"           {tokenize(LMAP); return LMAP;}
"struc"         {tokenize(LSTRUCT); return LSTRUCT;}
"else"          {tokenize(LELSE); return LELSE;}
"package"       {tokenize(LPACKAGE); return LPACKAGE;}
"const"         {tokenize(LCONST); return LCONST;}
"if"            {tokenize(LIF); return LIF;}
"type"          {tokenize(LTYPE); return LTYPE;}
"for"           {tokenize(LFOR); return LFOR;}
"import"        {tokenize(LIMPORT); return LIMPORT;}
"return"        {tokenize(LRETURN); return LRETURN;}
"var"           {tokenize(LVAR); return LVAR;}
"true"          {tokenize(LTRUE); return LTRUE;}
"false"         {tokenize(LFALSE); return LFALSE;}


    /* Bad reserved words */
"break"         {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"case"          {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"chan"          {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"fallthrough"   {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"continue"      {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"default"       {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"defer"         {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"goto"          {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"range"         {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"interface"     {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"go"            {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"switch"        {tokenize(LGOKEYWORD); return LGOKEYWORD;}
"select"        {tokenize(LGOKEYWORD); return LGOKEYWORD;}

    /* Type Names */
"bool"          {tokenize(LTYPEBOOL); return LTYPEBOOL;}
"string"        {tokenize(LTYPESTRING); return LTYPESTRING;}
"int"           {tokenize(LTYPEINT); return LTYPEINT;}
"rune"          {tokenize(LTYPERUNE); return LTYPERUNE;}
"float64"       {tokenize(LTYPEFLOAT64); return LTYPEFLOAT64;}

    /* Invalid Operators A */
"..."           {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"&^="           {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"<<="           {tokenize(LGOOPERATOR); return LGOOPERATOR;}
">>="           {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"%="            {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"|="            {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"<-"            {tokenize(LGOOPERATOR); return LGOOPERATOR;}
">>"            {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"<<"            {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"&^"            {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"&="            {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"*="            {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"/="            {tokenize(LGOOPERATOR); return LGOOPERATOR;}
":="            {tokenize(LGOOPERATOR); return LGOOPERATOR;}

    /* Valid Operators */
"<="            {tokenize(LLE); return LLE;}
">="            {tokenize(LGE); return LGE;}
"=="            {tokenize(LEQ); return LEQ;}
"!="            {tokenize(LNE); return LNE;}
"&&"            {tokenize(LANDAND); return LANDAND;}
"||"            {tokenize(LOROR); return LOROR;}
"+="            {tokenize(LPLASN); return LPLASN;}
"-="            {tokenize(LMIASN); return LMIASN;}
"="             {tokenize(LASOP); return LASOP;}
"("             {tokenize('('); return '(';}
")"             {tokenize(')'); return ')';}
"["             {tokenize('['); return '[';}
"]"             {tokenize(']'); return ']';}
"{"             {tokenize('{'); return '{';}
"}"             {tokenize('}'); return '}';}
"<"             {tokenize(LLT); return LLT;}
">"             {tokenize(LGT); return LGT;}
"."             {tokenize('.'); return '.';}
","             {tokenize(','); return ',';}
"!"             {tokenize('!'); return '!';}
"+"             {tokenize('+'); return '+';}
"-"             {tokenize('-'); return '-';}
"*"             {tokenize('*'); return '*';}
"/"             {tokenize('/'); return '/';}
"%"             {tokenize('%'); return '%';}
";"             {tokenize(';'); return ';';}

    /* Invalid Operators B */
":"             {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"&"             {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"^"             {tokenize(LGOOPERATOR); return LGOOPERATOR;}
"|"             {tokenize(LGOOPERATOR); return LGOOPERATOR;}

    /* Literals */
0[0-7]+                         { tokenize(LOCT); return LOCT; }            /* Octal */
0|[1-9][0-9]*                   { tokenize(LINT); return LINT; }            /* Int */
(([1-9][0-9]*)?\.[0-9]+)        { tokenize(LFLOAT); return LFLOAT; }        /* Float */
"0x"[a-fA-F0-9]+                { tokenize(LHEX); return LHEX; }            /* Hex */
"0b"[01]+     			        { tokenize(LBIN); return LBIN; }            /* Binary */
[a-zA-Z_][a-zA-Z0-9_]*	        { tokenize(LNAME); return LNAME; }          /* variable names */
\"(\\|[^\n"\\])*\"		        { tokenize(LLITERAL); return LLITERAL; }    /* string literals */
\'[^\\\n]\'|\'\\[ntNT'\\]\'     { tokenize(LRUNE); return LRUNE; }          /* Char (rune) */
    /* non-accepted runed caught here */


    /* Unrecognized tokens */
. { 
    fprintf(stderr, "File %s line %d, token %s: %s\n", currentFile, yylineno, yytext, "Invalid Token");
    return 1;
}

%%