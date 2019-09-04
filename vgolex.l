/* Based on the version for c++ here: https://github.com/SethForrest/Compilers_HW_1/blob/master/clex.l and here: https://github.com/andschwa/partial-cpp-compiler/blob/hw1/clex.l */
%option warn nounput noinput
%option header-file="vgolex.h"
%option yylineno noyywrap

%{
#include <stdbool.h>
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
    fprintf (stderr, "File %s line %d, token %s: %s\n", list_peek(filenames).filename, yylineno, yytext, s);
    exit(1);
}

/* allocates and prepares token, saving pointer into yytoken */
void tokenize(int category)
{
    yytoken = tokenNew(category, yylineno, yytext, list_peek(filenames).filename);
}

%}

%%

((\/\/[^\n]*\n)|([ \r\t\v\f\n])|(\/\*(.|\n)*\*\/))*	{} /* eat whitespace and  comments */

    /* Reserved words */
"func"          {tokenize(); return ;}
"map"           {tokenize(); return ;}
"struc"         {tokenize(); return ;}
"else"          {tokenize(); return ;}
"package"       {tokenize(); return ;}
"const"         {tokenize(); return ;}
"if"            {tokenize(); return ;}
"type"          {tokenize(); return ;}
"for"           {tokenize(); return ;}
"import"        {tokenize(); return ;}
"return"        {tokenize(); return ;}
"var"           {tokenize(); return ;}

    /* Bad reserved words */
"break"         {tokenize(BADKEYWORD); return BADKEYWORD;}
"case"          {tokenize(BADKEYWORD); return BADKEYWORD;}
"chan"          {tokenize(BADKEYWORD); return BADKEYWORD;}
"fallthrough"   {tokenize(BADKEYWORD); return BADKEYWORD;}
"continue"      {tokenize(BADKEYWORD); return BADKEYWORD;}
"default"       {tokenize(BADKEYWORD); return BADKEYWORD;}
"defer"         {tokenize(BADKEYWORD); return BADKEYWORD;}
"goto"          {tokenize(BADKEYWORD); return BADKEYWORD;}
"range"         {tokenize(BADKEYWORD); return BADKEYWORD;}
"interface"     {tokenize(BADKEYWORD); return BADKEYWORD;}
"go"            {tokenize(BADKEYWORD); return BADKEYWORD;}
"switch"        {tokenize(BADKEYWORD); return BADKEYWORD;}
"select"        {tokenize(BADKEYWORD); return BADKEYWORD;}

    /* Type Names */
"bool"          {tokenize(); return ;}
"string"        {tokenize(); return ;}
"int"           {tokenize(); return ;}
"float64"       {tokenize(); return ;}

    /* Operators */
"<="            {tokenize(LE); return LE;}
">="            {tokenize(GE); return GE;}
"=="            {tokenize(EQ); return EQ;}
"!="            {tokenize(NE); return NE;}
"&&"            {tokenize(ANDAND); return ANDAND;}
"||"            {tokenize(OROR); return OROR;}
"+="            {tokenize(PLASN); return PLASN;}
"-="            {tokenize(MIASN); return MIASN;}
"="             {tokenize(ASN); return ASN;}
"("             {tokenize(LP); return LP;}
")"             {tokenize(RP); return RP;}
"["             {tokenize(LB); return LB;}
"]"             {tokenize(RB); return RB;}
"."             {tokenize(DOT); return DOT;}
"-"             {tokenize(MINUS); return MINUS;}
"!"             {tokenize(BANG); return BANG;}
"*"             {tokenize(MUL); return MUL;}
"/"             {tokenize(DIV); return DIV;}
"%"             {tokenize(MOD); return MOD;}
"+"             {tokenize(PLUS); return PLUS;}
"<"             {tokenize(LT); return LT;}
">"             {tokenize(GT); return GT;}

    /* Invalid Operators */
"&^="           {tokenize(BADOPERATOR); return BADOPERATOR;}
"<<="           {tokenize(BADOPERATOR); return BADOPERATOR;}
">>="           {tokenize(BADOPERATOR); return BADOPERATOR;}
"%="            {tokenize(BADOPERATOR); return BADOPERATOR;}
"|="            {tokenize(BADOPERATOR); return BADOPERATOR;}
"<-"            {tokenize(BADOPERATOR); return BADOPERATOR;}
">>"            {tokenize(BADOPERATOR); return BADOPERATOR;}
"<<"            {tokenize(BADOPERATOR); return BADOPERATOR;}
"&^"            {tokenize(BADOPERATOR); return BADOPERATOR;}
"&="            {tokenize(BADOPERATOR); return BADOPERATOR;}
"*="            {tokenize(BADOPERATOR); return BADOPERATOR;}
"/="            {tokenize(BADOPERATOR); return BADOPERATOR;}
":="            {tokenize(BADOPERATOR); return BADOPERATOR;}
":"             {tokenize(BADOPERATOR); return BADOPERATOR;}
"&"             {tokenize(BADOPERATOR); return BADOPERATOR;}
"^"             {tokenize(BADOPERATOR); return BADOPERATOR;}
"|"             {tokenize(BADOPERATOR); return BADOPERATOR;}

    /* Literals */
0[0-7]+                     { tokenize(ICON); return ICON; }   /* Octal */
[1-9][0-9]*                 { tokenize(ICON); return ICON; }   /* Int */
(([1-9][0-9]*)?\.[0-9]+)    { tokenize(FCON); return FCON; }   /* Float */
"0x"[a-fA-F0-9]+            { tokenize(FCON); return FCON; }   /* Hex */
"0b"[01]+     			    { tokenize(FCON); return FCON; }   /* Binary */
[a-zA-Z_][a-zA-Z0-9_]*	    { tokenize(FCON); return FCON; }   /* variable names */
\"(\\|[^\n"\\])*\"		    { tokenize(FCON); return FCON; }   /* string literals */
\'[^\\\n]\'|\'\\[0abfnrtveABFNRTVE"&'\\?]\' { tokenize(FCON); return FCON; }   /* Char (rune) */


<<EOF>>                 { 

  yypop_buffer_state();
  popNode(&fStack);
  yylineno = 1;
	line_num = 1;
  if (fStack == NULL) {
    yyterminate();    
    printf("ALL FILES PARSED!\n");
  }

  if (!YY_CURRENT_BUFFER) {
    printf("NO MORE BUFFERS!\n");
    yyterminate();
  } 
}
%%


void lexerr(char *s)
{
	errors++;

	fprintf(stderr, "%s: lexical error", s);

	/* to do: add mechanism for reporting file name and line number */

	fprintf(stderr, ", token = \"%s\"\n", yytext);
}

/*
 * Return 1 if done, 0 if yyin points at more input
 */
int yywrap()
{
   return 1;
}

void handle_include()
{
   char *newfilename = malloc(strlen(yytext)+1-strlen("#include \"\""));
   saved_yyin = yyin;
   char *fname = strchr(yytext, '\"')+1;
   fname[strlen(fname)-1] = '\0';
   fprintf(stdout, "included filename '%s'\n", fname); fflush(stdout);
   yyin = fopen(fname,"r");
   if (yyin == NULL) {
     lexerr("cannot open include file");
     exit(1);
   }
}