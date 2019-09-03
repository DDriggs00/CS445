/* Based on the version for c++ here: */
%option warn nounput noinput
%option header-file="vgolex.h"
%option yylineno noyywrap
%x COMMENT STR CHR CHREND INC

DIGIT    [0-9]
LETTER   [a-zA-Z_]
HEX      [a-fA-F0-9]
FS       (f|F|l|L)
IS       (u|U|l|L)*

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

bool cstdlib  = false;
bool cmath    = false;
bool ctime    = false;
bool cstring  = false;
bool fstream  = false;
bool iostream = false;
bool string   = false;
bool iomanip  = false;

/* print error message to stderr and terminate (for now) */
void yyerror(const char *s)
{
	fprintf (stderr, "File %s line %d, token %s: %s\n", list_peek(filenames).filename, yylineno, yytext, s);
	exit(1);
}

/* allocates and prepares token, saving pointer into yytoken */
void new_token(int category)
{
	yytoken = token_create(category, yylineno, yytext, list_peek(filenames).filename);
}

void handle_include(const char *s);
%}

%%

[ \r\t\v\f\n]*          { /* eat whitespace */ }

    /* Reserved words */
"func"          {new_token(); return ;}
"map"           {new_token(); return ;}
"struc"         {new_token(); return ;}
"else"          {new_token(); return ;}
"package"       {new_token(); return ;}
"const"         {new_token(); return ;}
"if"            {new_token(); return ;}
"type"          {new_token(); return ;}
"for"           {new_token(); return ;}
"import"        {new_token(); return ;}
"return"        {new_token(); return ;}
"var"           {new_token(); return ;}

    /* Bad reserved words */
"break"         {new_token(BADKEYWORD); return BADKEYWORD;}
"case"          {new_token(BADKEYWORD); return BADKEYWORD;}
"chan"          {new_token(BADKEYWORD); return BADKEYWORD;}
"fallthrough"   {new_token(BADKEYWORD); return BADKEYWORD;}
"continue"      {new_token(BADKEYWORD); return BADKEYWORD;}
"default"       {new_token(BADKEYWORD); return BADKEYWORD;}
"defer"         {new_token(BADKEYWORD); return BADKEYWORD;}
"goto"          {new_token(BADKEYWORD); return BADKEYWORD;}
"range"         {new_token(BADKEYWORD); return BADKEYWORD;}
"interface"     {new_token(BADKEYWORD); return BADKEYWORD;}
"go"            {new_token(BADKEYWORD); return BADKEYWORD;}
"switch"        {new_token(BADKEYWORD); return BADKEYWORD;}
"select"        {new_token(BADKEYWORD); return BADKEYWORD;}

    /* Type Names */
"bool"          {new_token(); return ;}
"string"        {new_token(); return ;}
"int"           {new_token(); return ;}
"float64"       {new_token(); return ;}

    /* Operators */
"("             {new_token(LP); return LP;}
")"             {new_token(RP); return RP;}
"["             {new_token(LB); return LB;}
"]"             {new_token(RB); return RB;}
"."             {new_token(DOT); return DOT;}
"-"             {new_token(MINUS); return MINUS;}
"!"             {new_token(BANG); return BANG;}
"*"             {new_token(MUL); return MUL;}
"/"             {new_token(DIV); return DIV;}
"%"             {new_token(MOD); return MOD;}
"+"             {new_token(PLUS); return PLUS;}
"<"             {new_token(LT); return LT;}
"<="            {new_token(LE); return LE;}
">"             {new_token(GT); return GT;}
">="            {new_token(GE); return GE;}
"=="            {new_token(EQ); return EQ;}
"!="            {new_token(NE); return NE;}
"&&"            {new_token(ANDAND); return ANDAND;}
"||"            {new_token(OROR); return OROR;}
"="             {new_token(ASN); return ASN;}
"+="            {new_token(PLASN); return PLASN;}
"-="            {new_token(MIASN); return MIASN;}

    /* Invalid Operators */
"&"             {new_token(BADOPERATOR); return BADOPERATOR;}
"^"             {new_token(BADOPERATOR); return BADOPERATOR;}
">>"            {new_token(BADOPERATOR); return BADOPERATOR;}
"<<"            {new_token(BADOPERATOR); return BADOPERATOR;}
"&^"            {new_token(BADOPERATOR); return BADOPERATOR;}
"&="            {new_token(BADOPERATOR); return BADOPERATOR;}
"*="            {new_token(BADOPERATOR); return BADOPERATOR;}
"/="            {new_token(BADOPERATOR); return BADOPERATOR;}
"|"             {new_token(BADOPERATOR); return BADOPERATOR;}
"%="            {new_token(BADOPERATOR); return BADOPERATOR;}
"&^="           {new_token(BADOPERATOR); return BADOPERATOR;}
"<<="           {new_token(BADOPERATOR); return BADOPERATOR;}
">>="           {new_token(BADOPERATOR); return BADOPERATOR;}
"|="            {new_token(BADOPERATOR); return BADOPERATOR;}
"<-"            {new_token(BADOPERATOR); return BADOPERATOR;}
":="            {new_token(BADOPERATOR); return BADOPERATOR;}
":"             {new_token(BADOPERATOR); return BADOPERATOR;}

    /* Literals */
{DIGIT}+               { new_token(ICON); return ICON; }
{DIGIT}*"."{DIGIT}+    { new_token(FCON); return FCON; }
         


  /* constants */
{D}+{FS}?               { new_token(FCON); return FCON; }
{D}+"."{D}*{FS}?        { new_token(FCON); return FCON; }

\'                      { new_token(CCON); BEGIN(CHR); }
<CHR>{
	\'              { yyerror("Empty char literal"); }
	"\\'"           { yytoken->ival = '\''; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\\""          { yytoken->ival = '"';  token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\?"           { yytoken->ival = '\?'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\a"           { yytoken->ival = '\a'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\b"           { yytoken->ival = '\b'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\f"           { yytoken->ival = '\f'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\n"           { yytoken->ival = '\n'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\r"           { yytoken->ival = '\r'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\t"           { yytoken->ival = '\t'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\v"           { yytoken->ival = '\v'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\0"           { yytoken->ival = '\0'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\\\"          { yytoken->ival = '\\'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	[^\\"'"]        { yytoken->ival = *yytext; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	.               { yyerror("Unrecognized token in char literal"); }
}
<CHREND>{
	\'              { token_append_text(yytoken, yytext); BEGIN(INITIAL); return CCON; }
	\n              { yyerror("Unexpected newline in char literal"); }
	.               { yyerror("Too many symbols in char literal"); }
}


L?\"                    { new_token(SCON); BEGIN(STR); }
<STR>{
	\"              { token_finish_sval(yytoken); token_append_text(yytoken, yytext); BEGIN(INITIAL); return SCON; }
	"\\'"           { token_append_sval_char(yytoken, '\''); token_append_text(yytoken, yytext); }
	"\\\""          { token_append_sval_char(yytoken, '"');  token_append_text(yytoken, yytext); }
	"\\?"           { token_append_sval_char(yytoken, '\?'); token_append_text(yytoken, yytext); }
	"\\a"           { token_append_sval_char(yytoken, '\a'); token_append_text(yytoken, yytext); }
	"\\b"           { token_append_sval_char(yytoken, '\b'); token_append_text(yytoken, yytext); }
	"\\f"           { token_append_sval_char(yytoken, '\f'); token_append_text(yytoken, yytext); }
	"\\n"           { token_append_sval_char(yytoken, '\n'); token_append_text(yytoken, yytext); }
	"\\r"           { token_append_sval_char(yytoken, '\r'); token_append_text(yytoken, yytext); }
	"\\t"           { token_append_sval_char(yytoken, '\t'); token_append_text(yytoken, yytext); }
	"\\v"           { token_append_sval_char(yytoken, '\v'); token_append_text(yytoken, yytext); }
	"\\0"           { token_append_sval_char(yytoken, '\0'); token_append_text(yytoken, yytext); }
	"\\\\"          { token_append_sval_char(yytoken, '\\'); token_append_text(yytoken, yytext); }
	[^\\\"\n]+      { token_append_sval_string(yytoken, yytext); token_append_text(yytoken, yytext); }
	\n              { yyerror("Unexpected newline in string literal"); }
	.               { yyerror("Unrecognized token in string literal"); }
	<<EOF>>         { yyerror("Unterminated string literal"); }
}

<<EOF>>                 {
	/* pop the current buffer and filename */
	yypop_buffer_state();
	list_pop(filenames);

	/* if there's another file to process, chdir to it */
	char *filename = current_filename();
	if (filename) {
		chdir(dirname(filename));
		free(filename);
	}

	yylineno = 1; /* restart line numbering count */

	if (!YY_CURRENT_BUFFER) /* if buffer stack is empty, stop */
		yyterminate();
  }
%%

void handle_include(const char *s)
{
	/* get the include path */
	size_t len = strlen(s) - 2; /* size without surrounding quotes */
	char *include = calloc(len+1, sizeof(char));
	if (include == NULL)
		handle_error("handle include buffer");
	strncpy(include, s+1, len); /* copy substring */
	include[len] = '\0';

	/* get the real path */
	char *filename = realpath(include, NULL);
	free(include);
	if (filename == NULL) {
		fprintf(stderr, "Could not resolve path to included file: %s, included from %s\n", include, current_filename());
		exit(EXIT_FAILURE);
	}
	list_push(filenames, (union data)filename);

	/* open file and push buffer */
	yyin = fopen(filename, "r");
	if (yyin == NULL) {
		fprintf(stderr, "Could not open included file: %s, include from %s\n", filename, current_filename());
		exit(EXIT_FAILURE);
	}
	yylineno = 1;
	yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));

	return;
}
