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

#include <stdlib.h>	    // For basically everything
#include <stdio.h>	    // For NULL, printf
#include <stdbool.h>	// For boolean support

#include "node.h"
#include "nodeTypes.h"
#include "vgo.tab.h"

// #define YYDEBUG true
// yydebug = 1;

extern char* currentFile;

// flex vars
extern int yylineno;
extern char* yytext;
extern int yylex();

// For output
extern node_t* yytree;

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
%token <t>  LPLASN LMIASN LINC LDEC

// Assignment Operator
%token <t>  LASOP

// reserved words
%token <t>	LCONST LELSE LFOR LFUNC LIF LIMPORT LMAP LVAR
%token <t>  LPACKAGE LRETURN LSTRUCT
%token <t>  LNAME
%token <t>  LTYPE LRANGE

// Operators
%token <t>  LANDAND LANDNOT LCOMM LEQ LGE LGT
%token <t>  LIGNORE LLE LLSH LLT LNE LOROR LRSH
%token <t> ';' '(' ')' '{' '}' '[' ']' '+' '-' '='
%token <t> ',' '.' '*' '/' '@' '?' '!' ':' '&' '%'
%token <t> '^' '|' '$'

// Go, Not vGo
%token <t>  LGOKEYWORD LGOOPERATOR
%token <t>  LCOLAS LBREAK LCASE LCHAN LCONTINUE LDDD
%token <t>  LDEFAULT LDEFER LFALL LGO LGOTO LINTERFACE
%token <t>  LSELECT LSWITCH

// My added types
%type <t>   literal

// Non-Terminal symbols
%type <t>	import_here sym packname oliteral
%type <t>	stmt ntype file package imports import import_stmt
%type <t>	arg_type hidden_import ocomma osemi fnlitdcl error
%type <t>	case caseblock hidden_import_list import_stmt_list
%type <t>	compound_stmt dotname embed expr complitexpr bare_complitexpr
%type <t>	expr_or_type import_package import_safety import_there
%type <t>	fndcl hidden_fndcl fnliteral lconst
%type <t>	for_body for_header for_stmt if_header if_stmt non_dcl_stmt
%type <t>	interfacedcl keyval labelname name
%type <t>	name_or_type non_expr_type
%type <t>	new_name dcl_name oexpr typedclname
%type <t>	onew_name
%type <t>	osimple_stmt pexpr pexpr_no_paren
%type <t>	pseudocall range_stmt select_stmt
%type <t>	simple_stmt
%type <t>	switch_stmt uexpr
%type <t>	xfndcl typedcl

%type <t>	xdcl fnbody fnres loop_body dcl_name_list
%type <t>	new_name_list expr_list keyval_list braced_keyval_list expr_or_type_list xdcl_list
%type <t>	oexpr_list caseblock_list elseif elseif_list else stmt_list oarg_type_list_ocomma arg_type_list
%type <t>	interfacedcl_list vardcl vardcl_list structdcl structdcl_list
%type <t>	common_dcl constdcl constdcl1 constdcl_list typedcl_list

%type <t>	convtype comptype
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
%left	    LCOMM	/* outside the usual hierarchy; here for good error messages */
%left	    LOROR
%left	    LANDAND
%left	    LEQ LNE LLE LGE LLT LGT
%left	    '+' '-' '|' '^'
%left	    '*' '/' '%' '&' LLSH LRSH LANDNOT

/*
 * manual override of shift/reduce conflicts.
 * the general form is that we assign a precedence
 * to the token being shifted and then introduce
 * NotToken with lower precedence or PreferToToken with higher
 * and annotate the reducing rule accordingly.
 */
%left	    NotPackage
%left	    LPACKAGE

%left	    NotParen
%left	    '('

%left	    ')'
%left	    PreferToRightParen

%%
file:	
    package	imports	xdcl_list   { $$ = node_create2(NULL, NULL, tag_file, 3, $1, $2, $3); yytree = $$; }
    ;

package:
    %empty %prec NotPackage {
        yyerror("package statement must be first");
        exit(2);
    }
|	LPACKAGE sym ';'	{ $$ = node_create2(NULL, NULL, tag_package, 3, $1, $2, $3); }
    ;

imports:
    %empty              { $$ = node_create(NULL, NULL, tag_empty); }
|	imports import ';'	{ $$ = node_create2(NULL, NULL, tag_imports, 3, $1, $2, $3); }

import:
    LIMPORT import_stmt                     { $$ = node_create2(NULL, NULL, tag_import, 2, $1, $2); }
|	LIMPORT '(' import_stmt_list osemi ')'  { $$ = node_create2(NULL, NULL, tag_import, 5, $1, $2, $3, $4, $5); }
|	LIMPORT '(' ')'                         { $$ = node_create2(NULL, NULL, tag_import, 3, $1, $2, $3); }
    ;

import_stmt:
    import_here     { $$ = $1; }
|   import_package  { $$ = $1; }
|   import_there    { $$ = $1; }
    ;

import_stmt_list:
    import_stmt
|	import_stmt_list ';' import_stmt	{ $$ = node_create2(NULL, NULL, tag_import_stmt_list, 3, $1, $2, $3); }
    ;

import_here:
    LLITERAL        { $$ = $1; }
|	sym LLITERAL    { $$ = node_create2(NULL, NULL, tag_import_here, 2, $1, $2); }
|	'.' LLITERAL    { $$ = node_create2(NULL, NULL, tag_import_here, 2, $1, $2); }
    ;

import_package:
    LPACKAGE LNAME import_safety ';'	{ $$ = node_create2(NULL, NULL, tag_import_package, 4, $1, $2, $3, $4); }
    ;

import_safety:
    %empty  { $$ = node_create(NULL, NULL, tag_empty); }
|	LNAME	{ $$ = $1; }
    ;

import_there:
    hidden_import_list '$' '$'	{ $$ = node_create2(NULL, NULL, tag_import_there, 3, $1, $2, $3); }

/*
 * declarations
*/
xdcl:
    %empty {
        yyerror("empty top-level declaration");
        $$ = 0;
    }
|	common_dcl	    { $$ = $1; }
|	xfndcl	    	{ $$ = $1; }
|	non_dcl_stmt {
        yyerror("non-declaration statement outside function body");
        $$ = 0;
    }
|	error {
        $$ = 0;
    }
    ;

common_dcl:
    LVAR vardcl	    	    	    	    	    	{ $$ = node_create2(NULL, NULL, tag_common_dcl, 2, $1, $2); }
|	LVAR '(' vardcl_list osemi ')'	    	    	    { $$ = node_create2(NULL, NULL, tag_common_dcl, 5, $1, $2, $3, $4, $5); }
|	LVAR '(' ')'	    	    	    	    	    { $$ = node_create2(NULL, NULL, tag_common_dcl, 3, $1, $2, $3); }
|	lconst constdcl	    	    	    	    	    { $$ = node_create2(NULL, NULL, tag_common_dcl, 2, $1, $2); }
|	lconst '(' constdcl osemi ')'	    	    	    { $$ = node_create2(NULL, NULL, tag_common_dcl, 5, $1, $2, $3, $4, $5); }
|	lconst '(' constdcl ';' constdcl_list osemi ')'	    { $$ = node_create2(NULL, NULL, tag_common_dcl, 6, $1, $2, $3, $4, $5, $6); }
|	lconst '(' ')'	    	    	    	    	    { $$ = node_create2(NULL, NULL, tag_common_dcl, 3, $1, $2, $3); }
|	LTYPE typedcl	    	    	    	    	    { $$ = node_create2(NULL, NULL, tag_common_dcl, 2, $1, $2); }
|	LTYPE '(' typedcl_list osemi ')'	    	    	{ $$ = node_create2(NULL, NULL, tag_common_dcl, 5, $1, $2, $3, $4, $5); }
|	LTYPE '(' ')'	    	    	    	    	    { $$ = node_create2(NULL, NULL, tag_common_dcl, 3, $1, $2, $3); }
    ;

lconst:
    LCONST	{ $$ = $1; }
    ;

vardcl:
    dcl_name_list ntype	    	    	{ $$ = node_create2(NULL, NULL, tag_vardcl, 2, $1, $2); }
|	dcl_name_list ntype '=' expr_list	{ $$ = node_create2(NULL, NULL, tag_vardcl_init, 4, $1, $2, $3, $4); }
    ;

constdcl:
    dcl_name_list ntype '=' expr_list	{ $$ = node_create2(NULL, NULL, tag_constdcl, 4, $1, $2, $3, $4); }
|	dcl_name_list '=' expr_list	    	{ $$ = node_create2(NULL, NULL, tag_constdcl, 3, $1, $2, $3); }
    ;

constdcl1:
    constdcl	    	{ $$ = $1; }
|	dcl_name_list ntype	{ $$ = node_create2(NULL, NULL, tag_constdcl1, 2, $1, $2); }
|	dcl_name_list	    { $$ = $1; }
    ;

typedclname:
    sym { $$ = node_create2(NULL, NULL, tag_typedclname, 1, $1); }
    ;

typedcl:
    typedclname ntype	{ $$ = node_create2(NULL, NULL, tag_typedcl, 2, $1, $2); }
    ;

simple_stmt:
    expr	    	    	    { $$ = $1; }
|	expr_list '=' expr_list	    { $$ = node_create2(NULL, NULL, tag_assignment, 3, $1, $2, $3); }
|	expr LINC	    	    	{ $$ = node_create2(NULL, NULL, tag_simple_stmt, 2, $1, $2); }
|	expr LDEC	    	    	{ $$ = node_create2(NULL, NULL, tag_simple_stmt, 2, $1, $2); }
|	expr LPLASN expr            { $$ = node_create2(NULL, NULL, tag_simple_stmt2, 3, $1, $2, $3); }
|	expr LMIASN expr            { $$ = node_create2(NULL, NULL, tag_simple_stmt2, 3, $1, $2, $3); }
    ;

case:
    LCASE expr_or_type_list ':'	    	    { $$ = node_create2(NULL, NULL, tag_case, 3, $1, $2, $3); }
|	LCASE expr_or_type_list '=' expr ':'	{ $$ = node_create2(NULL, NULL, tag_case, 5, $1, $2, $3, $4, $5); }
|	LCASE expr_or_type_list LCOLAS expr ':' { $$ = node_create2(NULL, NULL, tag_case, 5, $1, $2, $3, $4, $5); }
|	LDEFAULT ':'	    	    	    	{ $$ = node_create2(NULL, NULL, tag_case, 2, $1, $2); }
    ;

compound_stmt:
    '{' stmt_list '}'	{ $$ = node_create2(NULL, NULL, tag_compound_stmt, 3, $1, $2, $3); }
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
    %empty                      { $$ = node_create(NULL, NULL, tag_empty); }
|	caseblock_list caseblock	{ $$ = node_create2(NULL, NULL, tag_caseblock_list, 2, $1, $2); }
    ;

loop_body:
    '{' stmt_list '}'   { $$ = node_create2(NULL, NULL, tag_loop_body, 3, $1, $2, $3); }
    ;

range_stmt:
    expr_list '=' LRANGE expr	    { $$ = node_create2(NULL, NULL, tag_range_stmt, 4, $1, $2, $3, $4); }
|	expr_list LCOLAS LRANGE expr	{ $$ = node_create2(NULL, NULL, tag_range_stmt, 4, $1, $2, $3, $4); }
    ;

for_header:
    osimple_stmt ';' osimple_stmt ';' osimple_stmt	{ $$ = node_create2(NULL, NULL, tag_for_header, 5, $1, $2, $3, $4, $5); }
|	osimple_stmt	    	    	    	    	{ $$ = $1; }
|	range_stmt	    	    	    	    	    { $$ = $1; }
    ;

for_body:
    for_header loop_body	{ $$ = node_create2(NULL, NULL, tag_for_body, 2, $1, $2); }
    ;

for_stmt:
    LFOR for_body   { $$ = node_create2(NULL, NULL, tag_for_stmt, 2, $1, $2); }
    ;

if_header:
    osimple_stmt	    	    	{ $$ = $1; }
|	osimple_stmt ';' osimple_stmt	{ $$ = node_create2(NULL, NULL, tag_if_header, 3, $1, $2, $3); }
    ;

/* IF cond body (ELSE IF cond body)* (ELSE block)? */
if_stmt:
    LIF if_header loop_body elseif_list else    { $$ = node_create2(NULL, NULL, tag_if_stmt, 5, $1, $2, $3, $4, $5); }
    ;

elseif:
    LELSE LIF if_header loop_body   { $$ = node_create2(NULL, NULL, tag_elseif, 4, $1, $2, $3, $4); }
    ;

elseif_list:
    %empty              { $$ = node_create(NULL, NULL, tag_empty); }
|	elseif_list elseif	{ $$ = node_create2(NULL, NULL, tag_elseif_list, 2, $1, $2); }
    ;

else:
    %empty              { $$ = node_create(NULL, NULL, tag_empty); }
|	LELSE compound_stmt	{ $$ = node_create2(NULL, NULL, tag_else, 2, $1, $2); }
    ;

switch_stmt:
    LSWITCH if_header '{' caseblock_list '}'    { $$ = node_create2(NULL, NULL, tag_switch_stmt, 5, $1, $2, $3, $4, $5); }
    ;

select_stmt:
    LSELECT '{' caseblock_list '}'	{ $$ = node_create2(NULL, NULL, tag_select_stmt, 4, $1, $2, $3, $4); }
    ;

/*
 * expressions
 */
expr:
    uexpr	    	    { $$ = $1; }
|	expr LOROR expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr LANDAND expr	{ $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr LEQ expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr LNE expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr LLT expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr LLE expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr LGE expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr LGT expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr '+' expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr '-' expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr '*' expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr '/' expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr '%' expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr '&' expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr LANDNOT expr	{ $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr LLSH expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
|	expr LRSH expr	    { $$ = node_create2(NULL, NULL, tag_expr, 3, $1, $2, $3); }
    ;

uexpr:
    pexpr	    { $$ = node_create2(NULL, NULL, tag_uexpr, 1, $1); }
|	'*' uexpr	{ $$ = node_create2(NULL, NULL, tag_uexpr, 2, $1, $2); }
|	'&' uexpr	{ $$ = node_create2(NULL, NULL, tag_uexpr, 2, $1, $2); }
|	'+' uexpr	{ $$ = node_create2(NULL, NULL, tag_uexpr, 2, $1, $2); }
|	'-' uexpr	{ $$ = node_create2(NULL, NULL, tag_uexpr, 2, $1, $2); }
|	'!' uexpr	{ $$ = node_create2(NULL, NULL, tag_uexpr, 2, $1, $2); }
|	LCOMM uexpr	{ $$ = node_create2(NULL, NULL, tag_uexpr, 2, $1, $2); }
    ;

/*
 * call-like statements that
 * can be preceded by 'defer' and 'go'
 */
pseudocall:
    pexpr '(' ')'	    	    	    	    { $$ = node_create2(NULL, NULL, tag_pseudocall, 3, $1, $2, $3); }
|	pexpr '(' expr_or_type_list ocomma ')'	    { $$ = node_create2(NULL, NULL, tag_pseudocall, 5, $1, $2, $3, $4, $5); }
|	pexpr '(' expr_or_type_list LDDD ocomma ')'	{ $$ = node_create2(NULL, NULL, tag_pseudocall, 6, $1, $2, $3, $4, $5, $6); }
    ;

pexpr_no_paren:
    literal	    	    	    	    	{ $$ = $1; }
|	name	    	    	    	    	{ $$ = $1; }
|	pexpr '.' sym	    	    	    	{ $$ = node_create2(NULL, NULL, tag_pexpr_no_paren_dot, 3, $1, $2, $3); }
|	pexpr '.' '(' expr_or_type ')'	    	{ $$ = node_create2(NULL, NULL, tag_pexpr_no_paren_dot, 5, $1, $2, $3, $4, $5); }
|	pexpr '.' '(' LTYPE ')'	    	    	{ $$ = node_create2(NULL, NULL, tag_pexpr_no_paren_dot, 5, $1, $2, $3, $4, $5); }
|	pexpr '[' expr ']'	    	    	    { $$ = node_create2(NULL, NULL, tag_pexpr_no_paren, 4, $1, $2, $3, $4); }
|	pexpr '[' oexpr ':' oexpr ']'	    	{ $$ = node_create2(NULL, NULL, tag_pexpr_no_paren, 6, $1, $2, $3, $4, $5, $6); }
|	pexpr '[' oexpr ':' oexpr ':' oexpr ']'	{ $$ = node_create2(NULL, NULL, tag_pexpr_no_paren, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
|	pseudocall      	    	    	    { $$ = $1; }
|	convtype '(' expr ocomma ')'	    	{ $$ = node_create2(NULL, NULL, tag_pexpr_no_paren, 5, $1, $2, $3, $4, $5); }
|	comptype '{' braced_keyval_list '}'     { $$ = node_create2(NULL, NULL, tag_pexpr_no_paren, 4, $1, $2, $3, $4); }
|	LNAME '{' expr_list '}'                 { $$ = node_create2(NULL, NULL, tag_pexpr_no_paren, 4, $1, $2, $3, $4); }
|	fnliteral                               { $$ = $1; }

keyval:
    expr ':' complitexpr	{ $$ = node_create2(NULL, NULL, tag_keyval, 3, $1, $2, $3); }
    ;

bare_complitexpr:
    expr	    	    	    	    	    { $$ = $1; }
|	'{' braced_keyval_list '}'	{ $$ = node_create2(NULL, NULL, tag_bare_complicitexpr, 3, $1, $2, $3); }
    ;

complitexpr:
    expr	    	    	    	    	    { $$ = $1; }
|	'{' braced_keyval_list '}'	{ $$ = node_create2(NULL, NULL, tag_complicitexpr, 3, $1, $2, $3); }
    ;

pexpr:
    pexpr_no_paren	    	{ $$ = $1; }
|	'(' expr_or_type ')'	{ $$ = node_create2(NULL, NULL, tag_pexpr, 3, $1, $2, $3); }
    ;

expr_or_type:
    expr	    	{ $$ = $1; }
|	non_expr_type	{ $$ = $1; }	%prec PreferToRightParen

name_or_type:
    ntype	{ $$ = $1; }

/*
 * names and types
 *	newname is used before declared
 *	oldname is used after declared
 */
new_name:
    sym	{ $$ = node_create2(NULL, NULL, tag_new_name, 1, $1); }
    ;

dcl_name:
    sym	{ $$ = node_create2(NULL, NULL, tag_dcl_name, 1, $1); }
    ;

onew_name:
    %empty      { $$ = node_create(NULL, NULL, tag_empty); }
|	new_name	{ $$ = $1; }
    ;

sym:
    LNAME	    	    { $$ = $1; }
|	hidden_importsym	{ $$ = $1; }
|	'?'	    	    	{ $$ = $1; }
    ;

hidden_importsym:
    '@' literal '.' LNAME	{ $$ = node_create2(NULL, NULL, tag_hidden_importsym, 4, $1, $2, $3, $4); }
|	'@' literal '.' '?'	    { $$ = node_create2(NULL, NULL, tag_hidden_importsym, 4, $1, $2, $3, $4); }
    ;

name:
    sym	{ $$ = node_create2(NULL, NULL, tag_name, 1, $1); }	%prec NotParen
    ;

labelname:
    new_name	{ $$ = $1; }
    ;

ntype:
    recvchantype	{ $$ = $1; }
|	fntype	    	{ $$ = $1; }
|	othertype	    { $$ = $1; }
|	ptrtype	    	{ $$ = $1; }
|	dotname	    	{ $$ = $1; }
|	'(' ntype ')'	{ $$ = node_create2(NULL, NULL, tag_ntype, 3, $1, $2, $3); }
    ;

non_expr_type:
    recvchantype	    { $$ = $1; }
|	fntype	    	    { $$ = $1; }
|	othertype	    	{ $$ = $1; }
|	'*' non_expr_type	{ $$ = node_create2(NULL, NULL, tag_non_expr_type, 2, $1, $2); }
    ;

non_recvchantype:
    fntype	    	{ $$ = $1; }
|	othertype	    { $$ = $1; }
|	ptrtype	    	{ $$ = $1; }
|	dotname	    	{ $$ = $1; }
|	'(' ntype ')'	{ $$ = node_create2(NULL, NULL, tag_non_recvchantype, 3, $1, $2, $3); }
    ;

convtype:
    fntype	    { $$ = $1; }
|	othertype	{ $$ = $1; }

comptype:
    othertype	{ $$ = $1; }
    ;

fnret_type:
    recvchantype	{ $$ = $1; }
|	fntype	    	{ $$ = $1; }
|	othertype	    { $$ = $1; }
|	ptrtype	    	{ $$ = $1; }
|	dotname	    	{ $$ = $1; }
    ;

dotname:
    name	    	{ $$ = node_create2(NULL, NULL, tag_dotname, 1, $1); }
|	name '.' sym	{ $$ = node_create2(NULL, NULL, tag_dotname, 3, $1, $2, $3); }
    ;

othertype:
    LTYPEBOOL       { $$ = $1; }
|   LTYPEFLOAT64    { $$ = $1; }
|   LTYPEINT        { $$ = $1; }
|   LTYPESTRING     { $$ = $1; }
|   LTYPERUNE       { $$ = $1; }
|   '[' oexpr ']' ntype	    	{ $$ = node_create2(NULL, NULL, tag_othertype_arr, 4, $1, $2, $3, $4); }
|	LCHAN non_recvchantype	    { $$ = node_create2(NULL, NULL, tag_othertype, 2, $1, $2); }
|	LCHAN LCOMM ntype	    	{ $$ = node_create2(NULL, NULL, tag_othertype, 3, $1, $2, $3); }
|	LMAP '[' ntype ']' ntype	{ $$ = node_create2(NULL, NULL, tag_othertype_map, 5, $1, $2, $3, $4, $5); }
|	structtype	    	    	{ $$ = $1; }
|	interfacetype	    	    { $$ = $1; }
    ;

ptrtype:
    '*' ntype	{ $$ = node_create2(NULL, NULL, tag_ptrtype, 2, $1, $2); }
    ;

recvchantype:
    LCOMM LCHAN ntype	{ $$ = node_create2(NULL, NULL, tag_ptrtype, 3, $1, $2, $3); }
    ;

structtype:
    LSTRUCT '{' structdcl_list osemi '}'	{ $$ = node_create2(NULL, NULL, tag_structtype, 5, $1, $2, $3, $4, $5); }
|	LSTRUCT '{' '}'	    	    	    { $$ = node_create2(NULL, NULL, tag_structtype, 3, $1, $2, $3); }
    ;

interfacetype:
    LINTERFACE '{' interfacedcl_list osemi '}'	{ $$ = node_create2(NULL, NULL, tag_interfacetype, 5, $1, $2, $3, $4, $5); }
|	LINTERFACE '{' '}'	    	    	    	{ $$ = node_create2(NULL, NULL, tag_interfacetype, 3, $1, $2, $3); }
    ;

/*
 * function stuff
 * all in one place to show how crappy it all is
 */
xfndcl:
    LFUNC fndcl fnbody	{ $$ = node_create2(NULL, NULL, tag_xfndcl, 3, $1, $2, $3); }
    ;

fndcl:
    sym '(' oarg_type_list_ocomma ')' fnres	    	    	    	    	{ $$ = node_create2(NULL, NULL, tag_fndcl, 5, $1, $2, $3, $4, $5); }
|	'(' oarg_type_list_ocomma ')' sym '(' oarg_type_list_ocomma ')' fnres	{ $$ = node_create2(NULL, NULL, tag_fndcl, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
    ;

hidden_fndcl:
    hidden_pkg_importsym '(' ohidden_funarg_list ')' ohidden_funres	    	    { $$ = node_create2(NULL, NULL, tag_hidden_fndcl, 5, $1, $2, $3, $4, $5); }
|	'(' hidden_funarg_list ')' sym '(' ohidden_funarg_list ')' ohidden_funres	{ $$ = node_create2(NULL, NULL, tag_hidden_fndcl, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
    ;

fntype:
    LFUNC '(' oarg_type_list_ocomma ')' fnres	{ $$ = node_create2(NULL, NULL, tag_fntype, 5, $1, $2, $3, $4, $5); }
    ;

fnbody:
    %empty              { $$ = node_create(NULL, NULL, tag_empty); }
|	'{' stmt_list '}'	{ $$ = node_create2(NULL, NULL, tag_fnbody, 3, $1, $2, $3); }
    ;

fnres:
    %prec NotParen %empty           { $$ = node_create(NULL, NULL, tag_empty); }
|	fnret_type	    	    	    { $$ = $1; }
|	'(' oarg_type_list_ocomma ')'	{ $$ = node_create2(NULL, NULL, tag_fnres, 3, $1, $2, $3); }
    ;

fnlitdcl:
    fntype	{ $$ = $1; }
    ;

fnliteral:
    fnlitdcl '{' stmt_list '}'	{ $$ = node_create2(NULL, NULL, tag_fnliteral, 4, $1, $2, $3, $4); }
|	fnlitdcl error	    	    	{ $$ = node_create2(NULL, NULL, tag_fnliteral, 2, $1, $2); }
    ;

/*
 * lists of things
 * note that they are left recursive to conserve yacc stack. they need to
 * be reversed to interpret correctly
 */
xdcl_list:
    %empty              { $$ = node_create(NULL, NULL, tag_empty); }
|	xdcl_list xdcl ';'	{ $$ = node_create2(NULL, NULL, tag_xdcl_list, 3, $1, $2, $3); }
    ;

vardcl_list:
    vardcl	    	    	{ $$ = $1; }
|	vardcl_list ';' vardcl	{ $$ = node_create2(NULL, NULL, tag_vardcl_list, 3, $1, $2, $3); }
    ;

constdcl_list:
    constdcl1	    	    	{ $$ = $1; }
|	constdcl_list ';' constdcl1	{ $$ = node_create2(NULL, NULL, tag_constdcl_list, 3, $1, $2, $3); }
    ;

typedcl_list:
    typedcl	    	    	    { $$ = $1; }
|	typedcl_list ';' typedcl	{ $$ = node_create2(NULL, NULL, tag_typedcl_list, 3, $1, $2, $3); }
    ;

structdcl_list:
    structdcl	    	    	    { $$ = $1; }
|	structdcl_list ';' structdcl	{ $$ = node_create2(NULL, NULL, tag_structdcl_list, 3, $1, $2, $3); }
    ;

interfacedcl_list:
    interfacedcl	    	    	    { $$ = $1; }
|	interfacedcl_list ';' interfacedcl	{ $$ = node_create2(NULL, NULL, tag_interfacedcl_list, 3, $1, $2, $3); }
    ;

structdcl:
    new_name_list ntype oliteral	{ $$ = node_create2(NULL, NULL, tag_structdcl, 3, $1, $2, $3); }
|	embed oliteral	    	    	{ $$ = node_create2(NULL, NULL, tag_structdcl, 2, $1, $2); }
|	'(' embed ')' oliteral	    	{ $$ = node_create2(NULL, NULL, tag_structdcl, 4, $1, $2, $3, $4); }
|	'*' embed oliteral	    	    { $$ = node_create2(NULL, NULL, tag_structdcl, 3, $1, $2, $3); }
|	'(' '*' embed ')' oliteral	    { $$ = node_create2(NULL, NULL, tag_structdcl, 5, $1, $2, $3, $4, $5); }
|	'*' '(' embed ')' oliteral	    { $$ = node_create2(NULL, NULL, tag_structdcl, 5, $1, $2, $3, $4, $5); }
    ;

packname:
    LNAME	    	{ $$ = $1; }
|	LNAME '.' sym	{ $$ = node_create2(NULL, NULL, tag_packname, 3, $1, $2, $3); }
    ;

embed:
    packname	{ $$ = $1; }
    ;

interfacedcl:
    new_name indcl	    { $$ = node_create2(NULL, NULL, tag_interfacedcl, 2, $1, $2); }
|	packname	    	{ $$ = $1; }
|	'(' packname ')'	{ $$ = node_create2(NULL, NULL, tag_interfacedcl, 3, $1, $2, $3); }
    ;

indcl:
    '(' oarg_type_list_ocomma ')' fnres	{ $$ = node_create2(NULL, NULL, tag_indcl, 4, $1, $2, $3, $4); }
    ;

/*
 * function arguments.
 */
arg_type:
    dcl_name_list name_or_type	{ $$ = node_create2(NULL, NULL, tag_arg_type, 2, $1, $2); }
    ;

arg_type_list:
    arg_type	    	    	{ $$ = $1; }
|	arg_type_list ',' arg_type	{ $$ = node_create2(NULL, NULL, tag_arg_type_list, 3, $1, $2, $3); }
    ;

oarg_type_list_ocomma:
    %empty                  { $$ = node_create(NULL, NULL, tag_empty); }
|	arg_type_list ocomma	{ $$ = node_create2(NULL, NULL, tag_arg_type_list_ocomma, 2, $1, $2); }
    ;

/*
 * statement
 */
stmt:
    %empty          { $$ = node_create(NULL, NULL, tag_empty); }
|	compound_stmt	{ $$ = $1; }
|	common_dcl	    { $$ = $1; }
|	non_dcl_stmt	{ $$ = $1; }
|	error	    	{ $$ = $1; }
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
    simple_stmt	    	{ $$ = $1; }
|	for_stmt	    	{ $$ = $1; }
|	switch_stmt	    	{ $$ = $1; }
|	select_stmt	    	{ $$ = $1; }
|	if_stmt	    	    { $$ = $1; }
|	labelname ':' stmt  { $$ = node_create2(NULL, NULL, tag_non_dcl_stmt, 2, $1, $2); }
|	LFALL	    	    { $$ = $1; }
|	LBREAK onew_name	{ $$ = node_create2(NULL, NULL, tag_non_dcl_stmt, 2, $1, $2); }
|	LCONTINUE onew_name	{ $$ = node_create2(NULL, NULL, tag_non_dcl_stmt, 2, $1, $2); }
|	LGO pseudocall	    { $$ = node_create2(NULL, NULL, tag_non_dcl_stmt, 2, $1, $2); }
|	LDEFER pseudocall	{ $$ = node_create2(NULL, NULL, tag_non_dcl_stmt, 2, $1, $2); }
|	LGOTO new_name	    { $$ = node_create2(NULL, NULL, tag_non_dcl_stmt, 2, $1, $2); }
|	LRETURN oexpr_list	{ $$ = node_create2(NULL, NULL, tag_non_dcl_stmt, 2, $1, $2); }
    ;

stmt_list:
    stmt	    	    { $$ = $1; }
|	stmt_list ';' stmt	{ $$ = node_create2(NULL, NULL, tag_stmt_list, 3, $1, $2, $3); }
    ;

new_name_list:
    new_name	    	    	{ $$ = $1; }
|	new_name_list ',' new_name	{ $$ = node_create2(NULL, NULL, tag_new_name_list, 3, $1, $2, $3); }
    ;

dcl_name_list:
    dcl_name	    	    	{ $$ = $1; }
|	dcl_name_list ',' dcl_name	{ $$ = node_create2(NULL, NULL, tag_dcl_name_list, 3, $1, $2, $3); }
    ;

expr_list:
    expr	    	    { $$ = $1; }
|	expr_list ',' expr	{ $$ = node_create2(NULL, NULL, tag_expr_list, 3, $1, $2, $3); }
    ;

expr_or_type_list:
    expr_or_type	    	    	    { $$ = $1; }
|	expr_or_type_list ',' expr_or_type	{ $$ = node_create2(NULL, NULL, tag_expr_or_type_list, 3, $1, $2, $3); }
    ;

/*
 * list of combo of keyval and val
 */
keyval_list:
    keyval	    	    	    	    { $$ = $1; }
|	bare_complitexpr	    	    	{ $$ = $1; }
|	keyval_list ',' keyval	    	    { $$ = node_create2(NULL, NULL, tag_keyval_list, 3, $1, $2, $3); }
|	keyval_list ',' bare_complitexpr	{ $$ = node_create2(NULL, NULL, tag_keyval_list, 3, $1, $2, $3); }
    ;

braced_keyval_list:
    %empty              { $$ = node_create(NULL, NULL, tag_empty); }
|	keyval_list ocomma	{ $$ = node_create2(NULL, NULL, tag_bracedkeyval_list, 2, $1, $2); }
    ;

/*
 * optional things
 */
osemi:
    %empty  { $$ = node_create(NULL, NULL, tag_empty); }
|	';'     { $$ = $1; }
    ;

ocomma:
    %empty  { $$ = node_create(NULL, NULL, tag_empty); }
|	','     { $$ = $1; }
    ;

oexpr:
    %empty  { $$ = node_create(NULL, NULL, tag_empty); }
|	expr    { $$ = $1; }
    ;

oexpr_list:
    %empty      { $$ = node_create(NULL, NULL, tag_empty); }
|	expr_list   { $$ = $1; }
    ;

osimple_stmt:
    %empty      { $$ = node_create(NULL, NULL, tag_empty); }
|	simple_stmt { $$ = $1; }
    ;

ohidden_funarg_list:
    %empty              { $$ = node_create(NULL, NULL, tag_empty); }
|	hidden_funarg_list  { $$ = $1; }
    ;

ohidden_structdcl_list:
    %empty                  { $$ = node_create(NULL, NULL, tag_empty); }
|	hidden_structdcl_list   { $$ = $1; }
    ;

ohidden_interfacedcl_list:
    %empty                      { $$ = node_create(NULL, NULL, tag_empty); }
|	hidden_interfacedcl_list    { $$ = $1; }
    ;

oliteral:
    %empty  { $$ = node_create(NULL, NULL, tag_empty); }
|	literal { $$ = $1; }
    ;

/*
 * import syntax from package header
 */
hidden_import:
    LIMPORT LNAME literal ';'	    	    	    	    	    { $$ = node_create2(NULL, NULL, tag_hidden_import, 4, $1, $2, $3, $4); }
|	LVAR hidden_pkg_importsym hidden_type ';'	    	    	    { $$ = node_create2(NULL, NULL, tag_hidden_import, 4, $1, $2, $3, $4); }
|	LCONST hidden_pkg_importsym '=' hidden_constant ';'	    	    { $$ = node_create2(NULL, NULL, tag_hidden_import, 5, $1, $2, $3, $4, $5); }
|	LCONST hidden_pkg_importsym hidden_type '=' hidden_constant ';'	{ $$ = node_create2(NULL, NULL, tag_hidden_import, 6, $1, $2, $3, $4, $5, $6); }
|	LTYPE hidden_pkgtype hidden_type ';'	    	    	    	{ $$ = node_create2(NULL, NULL, tag_hidden_import, 4, $1, $2, $3, $4); }
|	LFUNC hidden_fndcl fnbody ';'	    	    	    	    	{ $$ = node_create2(NULL, NULL, tag_hidden_import, 4, $1, $2, $3, $4); }
    ;

hidden_pkg_importsym:
    hidden_importsym	{ $$ = $1; }
    ;

hidden_pkgtype:
    hidden_pkg_importsym	{ $$ = $1; }
    ;

/*
 *  importing types
 */

hidden_type:
    hidden_type_misc	    { $$ = $1; }
|	hidden_type_recv_chan	{ $$ = $1; }
|	hidden_type_func	    { $$ = $1; }
    ;

hidden_type_non_recv_chan:
    hidden_type_misc	    { $$ = $1; }
|	hidden_type_func	    { $$ = $1; }
    ;

hidden_type_misc:
    hidden_importsym	    	    	    	    { $$ = $1; }
|	LNAME	    	    	    	    	    	{ $$ = $1; }
|	'[' ']' hidden_type	    	    	    	    { $$ = node_create2(NULL, NULL, tag_hidden_type_misc, 3, $1, $2, $3); }
|	'[' literal ']' hidden_type	    	    	    { $$ = node_create2(NULL, NULL, tag_hidden_type_misc, 4, $1, $2, $3, $4); }
|	LMAP '[' hidden_type ']' hidden_type	    	{ $$ = node_create2(NULL, NULL, tag_hidden_type_misc, 5, $1, $2, $3, $4, $5); }
|	LSTRUCT '{' ohidden_structdcl_list '}'	    	{ $$ = node_create2(NULL, NULL, tag_hidden_type_misc, 4, $1, $2, $3, $4); }
|	LINTERFACE '{' ohidden_interfacedcl_list '}'	{ $$ = node_create2(NULL, NULL, tag_hidden_type_misc, 4, $1, $2, $3, $4); }
|	'*' hidden_type	    	    	    	    	{ $$ = node_create2(NULL, NULL, tag_hidden_type_misc, 2, $1, $2); }
|	LCHAN hidden_type_non_recv_chan	    	    	{ $$ = node_create2(NULL, NULL, tag_hidden_type_misc, 2, $1, $2); }
|	LCHAN '(' hidden_type_recv_chan ')'	    	    { $$ = node_create2(NULL, NULL, tag_hidden_type_misc, 4, $1, $2, $3, $4); }
|	LCHAN LCOMM hidden_type	    	    	    	{ $$ = node_create2(NULL, NULL, tag_hidden_type_misc, 3, $1, $2, $3); }
    ;

hidden_type_recv_chan:
    LCOMM LCHAN hidden_type	{ $$ = node_create2(NULL, NULL, tag_hidden_type_recv_chan, 3, $1, $2, $3); }
    ;

hidden_type_func:
    LFUNC '(' ohidden_funarg_list ')' ohidden_funres	{ $$ = node_create2(NULL, NULL, tag_hidden_type_func, 5, $1, $2, $3, $4, $5); }
    ;

hidden_funarg:
    sym hidden_type oliteral	    { $$ = node_create2(NULL, NULL, tag_hidden_funarg, 3, $1, $2, $3); }
|	sym LDDD hidden_type oliteral	{ $$ = node_create2(NULL, NULL, tag_hidden_funarg, 4, $1, $2, $3, $4); }
    ;

hidden_structdcl:
    sym hidden_type oliteral	{ $$ = node_create2(NULL, NULL, tag_hidden_structdcl, 3, $1, $2, $3); }
    ;

hidden_interfacedcl:
    sym '(' ohidden_funarg_list ')' ohidden_funres	{ $$ = node_create2(NULL, NULL, tag_hidden_interfacedcl, 5, $1, $2, $3, $4, $5); }
|	hidden_type	    	    	    	    	    { $$ = $1; }
    ;

ohidden_funres:
    %empty          { $$ = node_create(NULL, NULL, tag_empty); }
|	hidden_funres   { $$ = $1; }
    ;

hidden_funres:
    '(' ohidden_funarg_list ')'	{ $$ = node_create2(NULL, NULL, tag_hidden_funrees, 3, $1, $2, $3); }
|	hidden_type	    	    	{ $$ = $1; }
    ;

/*
 *  importing constants
 */

hidden_literal:
    literal	    { $$ = $1; }
|	'-' literal	{ $$ = node_create2(NULL, NULL, tag_hidden_literal, 2, $1, $2); }
|	sym	    	{ $$ = $1; }
    ;

hidden_constant:
    hidden_literal	    	    	    	    { $$ = $1; }
|	'(' hidden_literal '+' hidden_literal ')'	{ $$ = node_create2(NULL, NULL, tag_hidden_constant, 5, $1, $2, $3, $4, $5); }
    ;

hidden_import_list:
    %empty                              { $$ = node_create(NULL, NULL, tag_empty); }
|	hidden_import_list hidden_import	{ $$ = node_create2(NULL, NULL, tag_hidden_import_list, 2, $1, $2); }
    ;

hidden_funarg_list:
    hidden_funarg	    	    	    	{ $$ = $1; }
|	hidden_funarg_list ',' hidden_funarg	{ $$ = node_create2(NULL, NULL, tag_hidden_funarg_list, 3, $1, $2, $3); }
    ;

hidden_structdcl_list:
    hidden_structdcl	    	    	    	{ $$ = $1; }
|	hidden_structdcl_list ';' hidden_structdcl	{ $$ = node_create2(NULL, NULL, tag_hidden_structdcl_list, 3, $1, $2, $3); }
    ;

hidden_interfacedcl_list:
    hidden_interfacedcl	    	    	    	    	{ $$ = $1; }
|	hidden_interfacedcl_list ';' hidden_interfacedcl	{ $$ = node_create2(NULL, NULL, tag_hidden_interfacedcl_list, 3, $1, $2, $3); }
    ;

%%

// Error handling
int yyerror(char* s) {
    fprintf(stderr, "%s:%d: %s before '%s' token\n", currentFile, yylineno, s, yytext);
    exit(2);
}