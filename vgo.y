// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file
// (a copy is at www2.cs.uidaho.edu/~jeffery/courses/445/go/LICENSE).

/*
 * Go language grammar adapted from Go 1.2.2.
 *
 * The Go semicolon rules are:
 *
 *  1. all statements and declarations are terminated by semicolons.
 *  2. semicolons can be omitted before a closing ) or }.
 *  3. semicolons are inserted by the lexer before a newline
 *      following a specific list of tokens.
 *
 * Rules #1 and #2 are accomplished by writing the lists as
 * semicolon-separated lists with an optional trailing semicolon.
 * Rule #3 is implemented in yylex.
 */

%{

#include <stdlib.h>		// For basically everything
#include <stdio.h>		// For NULL, printf
#include <stdbool.h>	// For boolean support

#include "node.h"

#define YYDEBUG true

// extern struct token *yytoken;
extern char** fileNames;
extern char* currentFile;

int yylex();    // Parser

int yyerror(char* s);
%}

%union {
	struct node_t* t;
}

// variable contents (manually added)
%token <t>	LLITERAL LBOOL LINT LHEX LOCT LFLOAT LBIN LRUNE

// Variable type names
%token <t>  LTYPEINT LTYPESTRING LTYPEBOOL LTYPEFLOAT64 LTYPERUNE

// Operators manually added (+=, -=)
%token <t>  LPLASN LMIASN

// Assignment Operator
%token <t>  LASOP

// reserved words
%token <t>	LCONST LELSE LFOR LFUNC LIF LIMPORT LMAP LVAR
%token <t>  LPACKAGE LRETURN LSTRUCT
%token <t>  LNAME
%token <t>  LTYPE LRANGE

// Operators
%token <t>  LANDAND LANDNOT LCOMM LDEC LEQ LGE LGT
%token <t>  LIGNORE LINC LLE LLSH LLT LNE LOROR LRSH

// Go, Not vGo
%token <t>  LGOKEYWORD LGOOPERATOR
%token <t>  LCOLAS LBREAK LCASE LCHAN LCONTINUE LDDD
%token <t>  LDEFAULT LDEFER LFALL LGO LGOTO LINTERFACE
%token <t>  LSELECT LSWITCH

// My added types
%type <t>   literal

// Non-Terminal symbols
%type <t>	lbrace import_here
%type <t>	sym packname
%type <t>	oliteral

%type <t>	stmt ntype
%type <t>	arg_type
%type <t>	case caseblock
%type <t>	compound_stmt dotname embed expr complitexpr bare_complitexpr
%type <t>	expr_or_type
%type <t>	fndcl hidden_fndcl fnliteral
%type <t>	for_body for_header for_stmt if_header if_stmt non_dcl_stmt
%type <t>	interfacedcl keyval labelname name
%type <t>	name_or_type non_expr_type
%type <t>	new_name dcl_name oexpr typedclname
%type <t>	onew_name
%type <t>	osimple_stmt pexpr pexpr_no_paren
%type <t>	pseudocall range_stmt select_stmt
%type <t>	simple_stmt
%type <t>	switch_stmt uexpr
%type <t>	xfndcl typedcl start_complit

%type <t>	xdcl fnbody fnres loop_body dcl_name_list
%type <t>	new_name_list expr_list keyval_list braced_keyval_list expr_or_type_list xdcl_list
%type <t>	oexpr_list caseblock_list elseif elseif_list else stmt_list oarg_type_list_ocomma arg_type_list
%type <t>	interfacedcl_list vardcl vardcl_list structdcl structdcl_list
%type <t>	common_dcl constdcl constdcl1 constdcl_list typedcl_list

%type <t>	convtype comptype dotdotdot
%type <t>	indcl interfacetype structtype ptrtype
%type <t>	recvchantype non_recvchantype othertype fnret_type fntype

%type <t>	hidden_importsym hidden_pkg_importsym

%type <t>	hidden_constant hidden_literal hidden_funarg
%type <t>	hidden_interfacedcl hidden_structdcl

%type <t>	hidden_funres
%type <t>	ohidden_funres
%type <t>	hidden_funarg_list ohidden_funarg_list
%type <t>	hidden_interfacedcl_list ohidden_interfacedcl_list
%type <t>	hidden_structdcl_list ohidden_structdcl_list

%type <t>	hidden_type hidden_type_misc hidden_pkgtype
%type <t>	hidden_type_func
%type <t>	hidden_type_recv_chan hidden_type_non_recv_chan

// Type order precedence, top is first
%left		LCOMM	/* outside the usual hierarchy; here for good error messages */
%left		LOROR
%left		LANDAND
%left		LEQ LNE LLE LGE LLT LGT
%left		'+' '-' '|' '^'
%left		'*' '/' '%' '&' LLSH LRSH LANDNOT

/*
 * manual override of shift/reduce conflicts.
 * the general form is that we assign a precedence
 * to the token being shifted and then introduce
 * NotToken with lower precedence or PreferToToken with higher
 * and annotate the reducing rule accordingly.
 */
%left		NotPackage
%left		LPACKAGE

%left		NotParen
%left		'('

%left		')'
%left		PreferToRightParen

// %start program

%%
file:	
    package	imports	xdcl_list	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
    ;

package:
	%prec NotPackage 
	{
		yyerror("package statement must be first");
		exit(1);
	}
|	LPACKAGE sym ';'	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

imports:
|	imports import ';'	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }

import:
	LIMPORT import_stmt	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	LIMPORT '(' import_stmt_list osemi ')'	{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
|	LIMPORT '(' ')'		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

import_stmt:
	import_here import_package import_there	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

import_stmt_list:
	import_stmt
|	import_stmt_list ';' import_stmt	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

import_here:
	literal		{ $$ = $1; }
|	sym literal	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	'.' literal	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

import_package:
	LPACKAGE LNAME import_safety ';'	{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
	;

import_safety:
|	LNAME	{ $$ = $1; }
	;

import_there:
	hidden_import_list '$' '$'	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }

/*
 * declarations
*/
xdcl:
	{
		yyerror("empty top-level declaration");
		$$ = 0;
	}
|	common_dcl		{ $$ = $1; }
|	xfndcl			{ $$ = $1; }
|	non_dcl_stmt {
		yyerror("non-declaration statement outside function body");
		$$ = 0;
	}
|	error {
		$$ = 0;
	}
	;

common_dcl:
	LVAR vardcl											{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	LVAR '(' vardcl_list osemi ')'						{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	LVAR '(' ')'										{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	lconst constdcl										{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	lconst '(' constdcl osemi ')'						{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	lconst '(' constdcl ';' constdcl_list osemi ')'		{ $$ = node_create(NULL, NULL, 6, $1, $2, $3, $4, $5, $6); }
|	lconst '(' ')'										{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	LTYPE typedcl										{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	LTYPE '(' typedcl_list osemi ')'					{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	LTYPE '(' ')'										{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

lconst:
	LCONST	{ $$ = $1; }
	;

vardcl:
	dcl_name_list ntype					{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	dcl_name_list ntype '=' expr_list	{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
|	dcl_name_list '=' expr_list			{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

constdcl:
	dcl_name_list ntype '=' expr_list	{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
|	dcl_name_list '=' expr_list			{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

constdcl1:
	constdcl			{ $$ = $1; }
|	dcl_name_list ntype	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	dcl_name_list		{ $$ = $1; }
	;

typedclname:
	sym
	{
		// the name becomes visible right here, not at the end
		// of the declaration.
	}
	;

typedcl:
	typedclname ntype	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

simple_stmt:
	expr						{ $$ = $1; }
|	expr LASOP expr				{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr_list '=' expr_list		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr_list LCOLAS expr_list	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LINC					{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	expr LDEC					{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

case:
	LCASE expr_or_type_list ':'				{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	LCASE expr_or_type_list '=' expr ':'	{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	LCASE expr_or_type_list LCOLAS expr ':' { $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	LDEFAULT ':'							{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

compound_stmt:
	'{'				{ $$ = $1; }
	stmt_list '}'	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

caseblock:
	case
	{
		// If the last token read by the lexer was consumed
		// as part of the case, clear it (parser has cleared yychar).
		// If the last token read by the lexer was the lookahead
		// leave it alone (parser has it cached in yychar).
		// This is so that the stmt_list action doesn't look at
		// the case tokens if the stmt_list is empty.
		// yylast = yychar;
	}
	stmt_list
	{
		// int last;

		// This is the only place in the language where a statement
		// list is not allowed to drop the final semicolon, because
		// it's the only place where a statement list is not followed 
		// by a closing brace.  Handle the error for pedantry.

		// Find the final token of the statement list.
		// yylast is lookahead; yyprev is last of stmt_list
		// last = yyprev;

		// if(last > 0 && last != ';' && yychar != '}')
		// 	yyerror("missing statement after label");
	}

caseblock_list:
|	caseblock_list caseblock	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

loop_body:
	'{'				{ $$ = $1; }
	stmt_list '}'	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

range_stmt:
	expr_list '=' LRANGE expr		{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
|	expr_list LCOLAS LRANGE expr	{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
	;

for_header:
	osimple_stmt ';' osimple_stmt ';' osimple_stmt	{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	osimple_stmt									{ $$ = $1; }
|	range_stmt										{ $$ = $1; }
	;

for_body:
	for_header loop_body	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

for_stmt:
	LFOR		{ $$ = $1; }
	for_body	{ $$ = $1; }
	;

if_header:
	osimple_stmt					{ $$ = $1; }
|	osimple_stmt ';' osimple_stmt	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

/* IF cond body (ELSE IF cond body)* (ELSE block)? */
if_stmt:
	LIF					{ $$ = $1; }
	if_header			{ $$ = $1; }
	loop_body			{ $$ = $1; }
	elseif_list else	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

elseif:
	LELSE LIF 			{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	if_header loop_body	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

elseif_list:
|	elseif_list elseif	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

else:
|	LELSE compound_stmt	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

switch_stmt:
	LSWITCH					{ $$ = $1; }
	if_header				{ $$ = $1; }
	'{' caseblock_list '}'	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

select_stmt:
	LSELECT					{ $$ = $1; }
	'{' caseblock_list '}'	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

/*
 * expressions
 */
expr:
	uexpr				{ $$ = $1; }
|	expr LOROR expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LANDAND expr	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LEQ expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LNE expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LLT expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LLE expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LGE expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LGT expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr '+' expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr '-' expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr '|' expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr '^' expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr '*' expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr '/' expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr '%' expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr '&' expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LANDNOT expr	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LLSH expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	expr LRSH expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }

	/* not an expression anymore, but left in so we can give a good error */
|	expr LCOMM expr		{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

uexpr:
	pexpr		{ $$ = $1; }
|	'*' uexpr	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	'&' uexpr	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	'+' uexpr	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	'-' uexpr	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	'!' uexpr	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	'~' uexpr
	{
		yyerror("the bitwise complement operator is ^");
	}
|	'^' uexpr
|	LCOMM uexpr	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

/*
 * call-like statements that
 * can be preceded by 'defer' and 'go'
 */
pseudocall:
	pexpr '(' ')'								{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	pexpr '(' expr_or_type_list ocomma ')'		{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	pexpr '(' expr_or_type_list LDDD ocomma ')'	{ $$ = node_create(NULL, NULL, 6, $1, $2, $3, $4, $5, $6); }
	;

pexpr_no_paren:
	literal													{ $$ = $1; }
|	name													{ $$ = $1; }
|	pexpr '.' sym											{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	pexpr '.' '(' expr_or_type ')'							{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	pexpr '.' '(' LTYPE ')'									{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	pexpr '[' expr ']'										{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
|	pexpr '[' oexpr ':' oexpr ']'							{ $$ = node_create(NULL, NULL, 6, $1, $2, $3, $4, $5, $6); }
|	pexpr '[' oexpr ':' oexpr ':' oexpr ']'					{ $$ = node_create(NULL, NULL, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
|	pseudocall												{ $$ = $1; }
|	convtype '(' expr ocomma ')'							{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	comptype lbrace start_complit braced_keyval_list '}'	{ $$ = node_create(NULL, NULL, 6, $1, $2, $3, $4, $5, $6); }
|	fnliteral												{ $$ = $1; }

start_complit:
	{
		// composite expression.
		// make node early so we get the right line number.
	}
	;

keyval:
	expr ':' complitexpr	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

bare_complitexpr:
	expr										{ $$ = $1; }
|	'{' start_complit braced_keyval_list '}'	{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
	;

complitexpr:
	expr										{ $$ = $1; }
|	'{' start_complit braced_keyval_list '}'	{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
	;

pexpr:
	pexpr_no_paren			{ $$ = $1; }
|	'(' expr_or_type ')'	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

expr_or_type:
	expr			{ $$ = $1; }
|	non_expr_type	{ $$ = $1; }	%prec PreferToRightParen

name_or_type:
	ntype	{ $$ = $1; }

lbrace:
	'{'		{ $$ = $1; }
	;

/*
 * names and types
 *	newname is used before declared
 *	oldname is used after declared
 */
new_name:
	sym	{ $$ = $1; }
	;

dcl_name:
	sym	{ $$ = $1; }
	;

onew_name:
|	new_name	{ $$ = $1; }
	;

sym:
	LNAME				{ $$ = $1; }
|	hidden_importsym	{ $$ = $1; }
|	'?'					{ $$ = $1; }
	;

hidden_importsym:
	'@' literal '.' LNAME	{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
|	'@' literal '.' '?'		{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
	;

name:
	sym	{ $$ = $1; }	%prec NotParen
	;

labelname:
	new_name	{ $$ = $1; }
	;

/*
 * to avoid parsing conflicts, type is split into
 *	channel types
 *	function types
 *	parenthesized types
 *	any other type
 * the type system makes additional restrictions,
 * but those are not implemented in the grammar.
 */
dotdotdot:
	LDDD
	{
		yyerror("final argument in variadic function missing type");
	}
|	LDDD ntype	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

ntype:
	recvchantype	{ $$ = $1; }
|	fntype			{ $$ = $1; }
|	othertype		{ $$ = $1; }
|	ptrtype			{ $$ = $1; }
|	dotname			{ $$ = $1; }
|	'(' ntype ')'	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

non_expr_type:
	recvchantype		{ $$ = $1; }
|	fntype				{ $$ = $1; }
|	othertype			{ $$ = $1; }
|	'*' non_expr_type	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

non_recvchantype:
	fntype			{ $$ = $1; }
|	othertype		{ $$ = $1; }
|	ptrtype			{ $$ = $1; }
|	dotname			{ $$ = $1; }
|	'(' ntype ')'	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

convtype:
	fntype		{ $$ = $1; }
|	othertype	{ $$ = $1; }

comptype:
	othertype	{ $$ = $1; }
	;

fnret_type:
	recvchantype	{ $$ = $1; }
|	fntype			{ $$ = $1; }
|	othertype		{ $$ = $1; }
|	ptrtype			{ $$ = $1; }
|	dotname			{ $$ = $1; }
	;

dotname:
	name			{ $$ = $1; }
|	name '.' sym	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

othertype:
	'[' oexpr ']' ntype			{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
|	'[' LDDD ']' ntype			{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
|	LCHAN non_recvchantype		{ $$ = node_create(NULL, NULL, 2, $1, $2); }
|	LCHAN LCOMM ntype			{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
|	LMAP '[' ntype ']' ntype	{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	structtype					{ $$ = $1; }
|	interfacetype				{ $$ = $1; }
	;

ptrtype:
	'*' ntype	{ $$ = node_create(NULL, NULL, 2, $1, $2); }
	;

recvchantype:
	LCOMM LCHAN ntype	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

structtype:
	LSTRUCT lbrace structdcl_list osemi '}'	{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	LSTRUCT lbrace '}'						{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

interfacetype:
	LINTERFACE lbrace interfacedcl_list osemi '}'	{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	LINTERFACE lbrace '}'							{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

/*
 * function stuff
 * all in one place to show how crappy it all is
 */
xfndcl:
	LFUNC fndcl fnbody	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

fndcl:
	sym '(' oarg_type_list_ocomma ')' fnres									{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	'(' oarg_type_list_ocomma ')' sym '(' oarg_type_list_ocomma ')' fnres	{ $$ = node_create(NULL, NULL, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
	;

hidden_fndcl:
	hidden_pkg_importsym '(' ohidden_funarg_list ')' ohidden_funres				{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
|	'(' hidden_funarg_list ')' sym '(' ohidden_funarg_list ')' ohidden_funres	{ $$ = node_create(NULL, NULL, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
	;

fntype:
	LFUNC '(' oarg_type_list_ocomma ')' fnres	{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
	;

fnbody:
|	'{' stmt_list '}'	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

fnres:
	%prec NotParen
|	fnret_type						{ $$ = $1; }
|	'(' oarg_type_list_ocomma ')'	{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
	;

fnlitdcl:
	fntype	{ $$ = $1; }
	;
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
{ $$ = node_create(NULL, NULL, 5, $1, $2, $3, $4, $5); }
{ $$ = node_create(NULL, NULL, 4, $1, $2, $3, $4); }
{ $$ = node_create(NULL, NULL, 3, $1, $2, $3); }
{ $$ = node_create(NULL, NULL, 2, $1, $2); }
{ $$ = $1; }

fnliteral:
	fnlitdcl lbrace stmt_list '}'
|	fnlitdcl error
	;

/*
 * lists of things
 * note that they are left recursive to conserve yacc stack. they need to
 * be reversed to interpret correctly
 */
xdcl_list:
|	xdcl_list xdcl ';'
	;

vardcl_list:
	vardcl
|	vardcl_list ';' vardcl
	;

constdcl_list:
	constdcl1
|	constdcl_list ';' constdcl1
	;

typedcl_list:
	typedcl
|	typedcl_list ';' typedcl
	;

structdcl_list:
	structdcl
|	structdcl_list ';' structdcl
	;

interfacedcl_list:
	interfacedcl
|	interfacedcl_list ';' interfacedcl
	;

structdcl:
	new_name_list ntype oliteral
|	embed oliteral
|	'(' embed ')' oliteral
|	'*' embed oliteral
|	'(' '*' embed ')' oliteral
|	'*' '(' embed ')' oliteral
	;

packname:
	LNAME
|	LNAME '.' sym
	;

embed:
	packname
	;

interfacedcl:
	new_name indcl
|	packname
|	'(' packname ')'
	;

indcl:
	'(' oarg_type_list_ocomma ')' fnres
	;

/*
 * function arguments.
 */
arg_type:
	name_or_type
|	sym name_or_type
|	sym dotdotdot
|	dotdotdot
	;

arg_type_list:
	arg_type
|	arg_type_list ',' arg_type
	;

oarg_type_list_ocomma:
|	arg_type_list ocomma
	;

/*
 * statement
 */
stmt:
|	compound_stmt
|	common_dcl
|	non_dcl_stmt
|	error
	;

literal:
    LINT        { $$ = $1; }
|   LHEX        { $$ = $1; }
|   LOCT        { $$ = $1; }
|   LBIN        { $$ = $1; }
|   LFLOAT      { $$ = $1; }
|   LBOOL       { $$ = $1; }
|   LLITERAL    { $$ = $1; }
|   LRUNE       { $$ = $1; }
    ;

non_dcl_stmt:
	simple_stmt
|	for_stmt
|	switch_stmt
|	select_stmt
|	if_stmt
|	labelname ':'
	stmt
|	LFALL
|	LBREAK onew_name
|	LCONTINUE onew_name
|	LGO pseudocall
|	LDEFER pseudocall
|	LGOTO new_name
|	LRETURN oexpr_list
	;

stmt_list:
	stmt
|	stmt_list ';' stmt
	;

new_name_list:
	new_name
|	new_name_list ',' new_name
	;

dcl_name_list:
	dcl_name
|	dcl_name_list ',' dcl_name
	;

expr_list:
	expr
|	expr_list ',' expr
	;

expr_or_type_list:
	expr_or_type
|	expr_or_type_list ',' expr_or_type
	;

/*
 * list of combo of keyval and val
 */
keyval_list:
	keyval
|	bare_complitexpr
|	keyval_list ',' keyval
|	keyval_list ',' bare_complitexpr
	;

braced_keyval_list:
|	keyval_list ocomma
	;

/*
 * optional things
 */
osemi:
|	';'
	;

ocomma:
|	','
	;

oexpr:
|	expr
	;

oexpr_list:
|	expr_list
	;

osimple_stmt:
|	simple_stmt
	;

ohidden_funarg_list:
|	hidden_funarg_list
	;

ohidden_structdcl_list:
|	hidden_structdcl_list
	;

ohidden_interfacedcl_list:
|	hidden_interfacedcl_list
	;

oliteral:
|	literal
	;

/*
 * import syntax from package header
 */
hidden_import:
	LIMPORT LNAME literal ';'
|	LVAR hidden_pkg_importsym hidden_type ';'
|	LCONST hidden_pkg_importsym '=' hidden_constant ';'
|	LCONST hidden_pkg_importsym hidden_type '=' hidden_constant ';'
|	LTYPE hidden_pkgtype hidden_type ';'
|	LFUNC hidden_fndcl fnbody ';'
	;

hidden_pkg_importsym:
	hidden_importsym
	;

hidden_pkgtype:
	hidden_pkg_importsym
	;

/*
 *  importing types
 */

hidden_type:
	hidden_type_misc
|	hidden_type_recv_chan
|	hidden_type_func
	;

hidden_type_non_recv_chan:
	hidden_type_misc
|	hidden_type_func
	;

hidden_type_misc:
	hidden_importsym
|	LNAME
|	'[' ']' hidden_type
|	'[' literal ']' hidden_type
|	LMAP '[' hidden_type ']' hidden_type
|	LSTRUCT '{' ohidden_structdcl_list '}'
|	LINTERFACE '{' ohidden_interfacedcl_list '}'
|	'*' hidden_type
|	LCHAN hidden_type_non_recv_chan
|	LCHAN '(' hidden_type_recv_chan ')'
|	LCHAN LCOMM hidden_type
	;

hidden_type_recv_chan:
	LCOMM LCHAN hidden_type
	;

hidden_type_func:
	LFUNC '(' ohidden_funarg_list ')' ohidden_funres
	;

hidden_funarg:
	sym hidden_type oliteral
|	sym LDDD hidden_type oliteral
	;

hidden_structdcl:
	sym hidden_type oliteral
	;

hidden_interfacedcl:
	sym '(' ohidden_funarg_list ')' ohidden_funres
|	hidden_type
	;

ohidden_funres:
|	hidden_funres
	;

hidden_funres:
	'(' ohidden_funarg_list ')'
|	hidden_type
	;

/*
 *  importing constants
 */

hidden_literal:
	literal
|	'-' literal
|	sym
	;

hidden_constant:
	hidden_literal
|	'(' hidden_literal '+' hidden_literal ')'
	;

hidden_import_list:
|	hidden_import_list hidden_import
	;

hidden_funarg_list:
	hidden_funarg
|	hidden_funarg_list ',' hidden_funarg
	;

hidden_structdcl_list:
	hidden_structdcl
|	hidden_structdcl_list ';' hidden_structdcl
	;

hidden_interfacedcl_list:
	hidden_interfacedcl
|	hidden_interfacedcl_list ';' hidden_interfacedcl
	;

%%

// Error handling
int yyerror(char* s) {
   	fprintf(stderr, "%s:%d: %s before '%s' token\n", yyfilename, yylineno, s, yytext);
    exit(2);
}