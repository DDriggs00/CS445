/* Referred to here: https://github.com/SethForrest/Compilers_HW_1/blob/master/clex.l and here: https://github.com/andschwa/partial-cpp-compiler/blob/hw1/clex.l */
%option warn nounput noinput
%option noyywrap yylineno

%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
// #include <string.h>
// #include <ctype.h>
// #include <libgen.h>

#include "token.h"
#include "vgo.tab.h"
#include "node.h"

// extern struct node_t* yylval;
extern char** fileNames;
extern char* currentFile;
extern bool insertSemicolon;

/* allocates and prepares token, saving pointer into yylval.t */
void nodenize(int category);

%}

%%

((\/\/[^\n]*\n)|([ \r\t\v\f])|(\/\*[^*]*\*+(?:[^\/*][^*]*\*+)*\/))* { insertSemicolon = false; } /* eat whitespace and  comments (but not \n) */

\n  {
    if (insertSemicolon) {
        insertSemicolon = false;
        yytext = ";";
        yylval.t = node_create(NULL, tokenNew(';', yytext, yylineno - 1, currentFile), yytext);
        return ';';
    }
}  

    /* Reserved words */
"func"          {nodenize(LFUNC); insertSemicolon = false; return LFUNC;}
"map"           {nodenize(LMAP); insertSemicolon = false; return LMAP;}
"struc"         {nodenize(LSTRUCT); insertSemicolon = false; return LSTRUCT;}
"else"          {nodenize(LELSE); insertSemicolon = false; return LELSE;}
"package"       {nodenize(LPACKAGE); insertSemicolon = false; return LPACKAGE;}
"const"         {nodenize(LCONST); insertSemicolon = false; return LCONST;}
"if"            {nodenize(LIF); insertSemicolon = false; return LIF;}
"type"          {nodenize(LTYPE); insertSemicolon = false; return LTYPE;}
"for"           {nodenize(LFOR); insertSemicolon = false; return LFOR;}
"import"        {nodenize(LIMPORT); insertSemicolon = false; return LIMPORT;}
"return"        {nodenize(LRETURN); insertSemicolon = false; return LRETURN;}
"var"           {nodenize(LVAR); insertSemicolon = false; return LVAR;}
"true"          {nodenize(LBOOL); insertSemicolon = true; return LBOOL;}
"false"         {nodenize(LBOOL); insertSemicolon = true; return LBOOL;}


    /* Bad reserved words */
"break"         {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"case"          {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"chan"          {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"fallthrough"   {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"continue"      {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"default"       {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"defer"         {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"goto"          {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"range"         {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"interface"     {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"go"            {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"switch"        {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}
"select"        {nodenize(LGOKEYWORD); insertSemicolon = false; return LGOKEYWORD;}

    /* Type Names */
"bool"          {nodenize(LTYPEBOOL); insertSemicolon = true; return LTYPEBOOL;}
"string"        {nodenize(LTYPESTRING); insertSemicolon = true; return LTYPESTRING;}
"int"           {nodenize(LTYPEINT); insertSemicolon = true; return LTYPEINT;}
"rune"          {nodenize(LTYPERUNE); insertSemicolon = true; return LTYPERUNE;}
"float64"       {nodenize(LTYPEFLOAT64); insertSemicolon = true; return LTYPEFLOAT64;}

    /* Invalid Operators A */
"..."           {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"&^="           {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"<<="           {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
">>="           {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"%="            {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"|="            {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"<-"            {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
">>"            {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"<<"            {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"&^"            {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"&="            {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"*="            {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"/="            {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
":="            {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}

    /* Valid Operators */
"<="            {nodenize(LLE); insertSemicolon = false; return LLE;}
">="            {nodenize(LGE); insertSemicolon = false; return LGE;}
"=="            {nodenize(LEQ); insertSemicolon = false; return LEQ;}
"!="            {nodenize(LNE); insertSemicolon = false; return LNE;}
"&&"            {nodenize(LANDAND); insertSemicolon = false; return LANDAND;}
"||"            {nodenize(LOROR); insertSemicolon = false; return LOROR;}
"+="            {nodenize(LPLASN); insertSemicolon = false; return LPLASN;}
"-="            {nodenize(LMIASN); insertSemicolon = false; return LMIASN;}
"++"            {nodenize(LINC); insertSemicolon = true; return LINC;}
"--"            {nodenize(LDEC); insertSemicolon = true; return LDEC;}
"="             {nodenize(LASOP); insertSemicolon = false; return LASOP;}
"("             {nodenize('('); insertSemicolon = false; return '(';}
")"             {nodenize(')'); insertSemicolon = true; return ')';}
"["             {nodenize('['); insertSemicolon = false; return '[';}
"]"             {nodenize(']'); insertSemicolon = false; return ']';}
"{"             {nodenize('{'); insertSemicolon = false; return '{';}
"}"             {nodenize('}'); insertSemicolon = true; return '}';}
"<"             {nodenize(LLT); insertSemicolon = false; return LLT;}
">"             {nodenize(LGT); insertSemicolon = false; return LGT;}
"."             {nodenize('.'); insertSemicolon = false; return '.';}
","             {nodenize(','); insertSemicolon = false; return ',';}
"!"             {nodenize('!'); insertSemicolon = false; return '!';}
"+"             {nodenize('+'); insertSemicolon = false; return '+';}
"-"             {nodenize('-'); insertSemicolon = false; return '-';}
"*"             {nodenize('*'); insertSemicolon = false; return '*';}
"/"             {nodenize('/'); insertSemicolon = false; return '/';}
"%"             {nodenize('%'); insertSemicolon = false; return '%';}
";"             {nodenize(';'); insertSemicolon = false; return ';';}

    /* Invalid Operators B */
":"             {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"&"             {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"^"             {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}
"|"             {nodenize(LGOOPERATOR); insertSemicolon = false; return LGOOPERATOR;}

    /* Literals */
0[0-7]+                         { nodenize(LOCT); insertSemicolon = true; return LOCT; }            /* Octal */
0|[1-9][0-9]*                   { nodenize(LINT); insertSemicolon = true; return LINT; }            /* Int */
(([1-9][0-9]*)?\.[0-9]+)        { nodenize(LFLOAT); insertSemicolon = true; return LFLOAT; }        /* Float */
"0x"[a-fA-F0-9]+                { nodenize(LHEX); insertSemicolon = true; return LHEX; }            /* Hex */
"0b"[01]+     			        { nodenize(LBIN); insertSemicolon = true; return LBIN; }            /* Binary */
[a-zA-Z_][a-zA-Z0-9_]*	        { nodenize(LNAME); insertSemicolon = true; return LNAME; }          /* variable names */
\"(\\|[^\n"\\])*\"		        { nodenize(LLITERAL); insertSemicolon = true; return LLITERAL; }    /* string literals */
\'[^\\\n]\'|\'\\[ntNT'\\]\'     { nodenize(LRUNE); insertSemicolon = true; return LRUNE; }          /* Char (rune) */
    /* non-accepted runed caught here */

    /* Unrecognized tokens */
. { 
    fprintf(stderr, "File %s line %d, token %s: %s\n", currentFile, yylineno, yytext, "Invalid Token");
    return 1;
}

%%

void nodenize(int category)
{
    yylval.t = node_create(NULL, tokenNew(category, yytext, yylineno, currentFile), yytext);
    
}
