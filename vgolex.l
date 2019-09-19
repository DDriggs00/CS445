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
#include "node.h"

extern struct node_t *yytoken;
extern char** fileNames;
extern char* currentFile;

/* allocates and prepares token, saving pointer into yytoken */
void nodenize(int category);

%}

%%

((\/\/[^\n]*\n)|([ \r\t\v\f\n])|(\/\*[^*]*\*+(?:[^\/*][^*]*\*+)*\/))* {} /* eat whitespace and  comments */

    /* Reserved words */
"func"          {nodenize(LFUNC); return LFUNC;}
"map"           {nodenize(LMAP); return LMAP;}
"struc"         {nodenize(LSTRUCT); return LSTRUCT;}
"else"          {nodenize(LELSE); return LELSE;}
"package"       {nodenize(LPACKAGE); return LPACKAGE;}
"const"         {nodenize(LCONST); return LCONST;}
"if"            {nodenize(LIF); return LIF;}
"type"          {nodenize(LTYPE); return LTYPE;}
"for"           {nodenize(LFOR); return LFOR;}
"import"        {nodenize(LIMPORT); return LIMPORT;}
"return"        {nodenize(LRETURN); return LRETURN;}
"var"           {nodenize(LVAR); return LVAR;}
"true"          {nodenize(LBOOL); return LBOOL;}
"false"         {nodenize(LBOOL); return LBOOL;}


    /* Bad reserved words */
"break"         {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"case"          {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"chan"          {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"fallthrough"   {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"continue"      {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"default"       {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"defer"         {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"goto"          {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"range"         {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"interface"     {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"go"            {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"switch"        {nodenize(LGOKEYWORD); return LGOKEYWORD;}
"select"        {nodenize(LGOKEYWORD); return LGOKEYWORD;}

    /* Type Names */
"bool"          {nodenize(LTYPEBOOL); return LTYPEBOOL;}
"string"        {nodenize(LTYPESTRING); return LTYPESTRING;}
"int"           {nodenize(LTYPEINT); return LTYPEINT;}
"rune"          {nodenize(LTYPERUNE); return LTYPERUNE;}
"float64"       {nodenize(LTYPEFLOAT64); return LTYPEFLOAT64;}

    /* Invalid Operators A */
"..."           {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"&^="           {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"<<="           {nodenize(LGOOPERATOR); return LGOOPERATOR;}
">>="           {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"%="            {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"|="            {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"<-"            {nodenize(LGOOPERATOR); return LGOOPERATOR;}
">>"            {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"<<"            {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"&^"            {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"&="            {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"*="            {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"/="            {nodenize(LGOOPERATOR); return LGOOPERATOR;}
":="            {nodenize(LGOOPERATOR); return LGOOPERATOR;}

    /* Valid Operators */
"<="            {nodenize(LLE); return LLE;}
">="            {nodenize(LGE); return LGE;}
"=="            {nodenize(LEQ); return LEQ;}
"!="            {nodenize(LNE); return LNE;}
"&&"            {nodenize(LANDAND); return LANDAND;}
"||"            {nodenize(LOROR); return LOROR;}
"+="            {nodenize(LPLASN); return LPLASN;}
"-="            {nodenize(LMIASN); return LMIASN;}
"="             {nodenize(LASOP); return LASOP;}
"("             {nodenize('('); return '(';}
")"             {nodenize(')'); return ')';}
"["             {nodenize('['); return '[';}
"]"             {nodenize(']'); return ']';}
"{"             {nodenize('{'); return '{';}
"}"             {nodenize('}'); return '}';}
"<"             {nodenize(LLT); return LLT;}
">"             {nodenize(LGT); return LGT;}
"."             {nodenize('.'); return '.';}
","             {nodenize(','); return ',';}
"!"             {nodenize('!'); return '!';}
"+"             {nodenize('+'); return '+';}
"-"             {nodenize('-'); return '-';}
"*"             {nodenize('*'); return '*';}
"/"             {nodenize('/'); return '/';}
"%"             {nodenize('%'); return '%';}
";"             {nodenize(';'); return ';';}

    /* Invalid Operators B */
":"             {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"&"             {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"^"             {nodenize(LGOOPERATOR); return LGOOPERATOR;}
"|"             {nodenize(LGOOPERATOR); return LGOOPERATOR;}

    /* Literals */
0[0-7]+                         { nodenize(LOCT); return LOCT; }            /* Octal */
0|[1-9][0-9]*                   { nodenize(LINT); return LINT; }            /* Int */
(([1-9][0-9]*)?\.[0-9]+)        { nodenize(LFLOAT); return LFLOAT; }        /* Float */
"0x"[a-fA-F0-9]+                { nodenize(LHEX); return LHEX; }            /* Hex */
"0b"[01]+     			        { nodenize(LBIN); return LBIN; }            /* Binary */
[a-zA-Z_][a-zA-Z0-9_]*	        { nodenize(LNAME); return LNAME; }          /* variable names */
\"(\\|[^\n"\\])*\"		        { nodenize(LLITERAL); return LLITERAL; }    /* string literals */
\'[^\\\n]\'|\'\\[ntNT'\\]\'     { nodenize(LRUNE); return LRUNE; }          /* Char (rune) */
    /* non-accepted runed caught here */


    /* Unrecognized tokens */
. { 
    fprintf(stderr, "File %s line %d, token %s: %s\n", currentFile, yylineno, yytext, "Invalid Token");
    return 1;
}

%%

void nodenize(int category)
{
    yytoken = node_create(NULL, tokenNew(category, yytext, yylineno, currentFile));
    
}