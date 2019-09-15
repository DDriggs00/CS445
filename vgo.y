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
#include <stdio.h>
#include <libc.h>
%}
// variable contents (manually added)
%token		LLITERAL LBOOL LINT LHEX LOCT LFLOAT LBIN LRUNE
// Variable type names
%token      LTYPEINT LTYPESTRING LTYPEBOOL LTYPEFLOAT64 LTYPERUNE

// Operators manually added (+=, -=)
%token      LPLASN LMIASN

// True and False (manually added)
%token      LTRUE LFALSE

// Assignment Operator
%token		LASOP

// reserved words
%token		LCONST LELSE LFOR LFUNC LIF LIMPORT LMAP LVAR
%token		LPACKAGE LRETURN LSTRUCT
%token      LNAME
%token		LTYPE LRANGE

%token		LANDAND LANDNOT LBODY LCOMM LDEC LEQ LGE LGT
%token		LIGNORE LINC LLE LLSH LLT LNE LOROR LRSH

// Go, Not vGo
%token		LGOKEYWORD LGOOPERATOR
%token	    LCOLAS LBREAK LCASE LCHAN LCONTINUE LDDD
%token      LDEFAULT LDEFER LFALL LGO LGOTO LINTERFACE
%token      LSELECT LSWITCH

/*
%type		lbrace import_here
%type		sym packname
%type		oliteral

%type		stmt ntype
%type		arg_type
%type		case caseblock
%type		compound_stmt dotname embed expr complitexpr bare_complitexpr
%type		expr_or_type
%type		fndcl hidden_fndcl fnliteral
%type		for_body for_header for_stmt if_header if_stmt non_dcl_stmt
%type		interfacedcl keyval labelname name
%type		name_or_type non_expr_type
%type		new_name dcl_name oexpr typedclname
%type		onew_name
%type		osimple_stmt pexpr pexpr_no_paren
%type		pseudocall range_stmt select_stmt
%type		simple_stmt
%type		switch_stmt uexpr
%type		xfndcl typedcl start_complit

%type		xdcl fnbody fnres loop_body dcl_name_list
%type		new_name_list expr_list keyval_list braced_keyval_list expr_or_type_list xdcl_list
%type		oexpr_list caseblock_list elseif elseif_list else stmt_list oarg_type_list_ocomma arg_type_list
%type		interfacedcl_list vardcl vardcl_list structdcl structdcl_list
%type		common_dcl constdcl constdcl1 constdcl_list typedcl_list

%type		convtype comptype dotdotdot
%type		indcl interfacetype structtype ptrtype
%type		recvchantype non_recvchantype othertype fnret_type fntype

%type		hidden_importsym hidden_pkg_importsym

%type		hidden_constant hidden_literal hidden_funarg
%type		hidden_interfacedcl hidden_structdcl

%type		hidden_funres
%type		ohidden_funres
%type		hidden_funarg_list ohidden_funarg_list
%type		hidden_interfacedcl_list ohidden_interfacedcl_list
%type		hidden_structdcl_list ohidden_structdcl_list

%type		hidden_type hidden_type_misc hidden_pkgtype
%type		hidden_type_func
%type		hidden_type_recv_chan hidden_type_non_recv_chan
*/

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

%define parse.error verbose

%%
file:	package	imports	xdcl_list ;

package:
	%prec NotPackage 
	{
		yyerror("package statement must be first");
		exit(1);
	}
|	LPACKAGE sym ';'
	;

imports:
|	imports import ';'

import:
	LIMPORT import_stmt
|	LIMPORT '(' import_stmt_list osemi ')'
|	LIMPORT '(' ')'
	;

import_stmt:
	import_here import_package import_there
	;

import_stmt_list:
	import_stmt
|	import_stmt_list ';' import_stmt
	;

import_here:
	LLITERAL
|	sym LLITERAL
|	'.' LLITERAL
	;

import_package:
	LPACKAGE LNAME import_safety ';'
	;

import_safety:
|	LNAME
	;

import_there:
	hidden_import_list '$' '$'

/*
 * declarations
 */
xdcl:
	{
		yyerror("empty top-level declaration");
		$$ = nil;
	}
|	common_dcl
|	xfndcl
|	non_dcl_stmt
	{
		yyerror("non-declaration statement outside function body");
		$$ = nil;
	}
|	error
	{
		$$ = nil;
	}
	;

common_dcl:
	LVAR vardcl
|	LVAR '(' vardcl_list osemi ')'
|	LVAR '(' ')'
|	lconst constdcl
|	lconst '(' constdcl osemi ')'
|	lconst '(' constdcl ';' constdcl_list osemi ')'
|	lconst '(' ')'
|	LTYPE typedcl
|	LTYPE '(' typedcl_list osemi ')'
|	LTYPE '(' ')'
	;

lconst:
	LCONST
	;

vardcl:
	dcl_name_list ntype
|	dcl_name_list ntype '=' expr_list
|	dcl_name_list '=' expr_list
	;

constdcl:
	dcl_name_list ntype '=' expr_list
|	dcl_name_list '=' expr_list
	;

constdcl1:
	constdcl
|	dcl_name_list ntype
|	dcl_name_list
	;

typedclname:
	sym
	{
		// the name becomes visible right here, not at the end
		// of the declaration.
	}
	;

typedcl:
	typedclname ntype
	;

simple_stmt:
	expr
|	expr LASOP expr
|	expr_list '=' expr_list
|	expr_list LCOLAS expr_list
|	expr LINC
|	expr LDEC
	;

case:
	LCASE expr_or_type_list ':'
|	LCASE expr_or_type_list '=' expr ':'
|	LCASE expr_or_type_list LCOLAS expr ':'
|	LDEFAULT ':'
	;

compound_stmt:
	'{'
	stmt_list '}'
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
		yylast = yychar;
	}
	stmt_list
	{
		int last;

		// This is the only place in the language where a statement
		// list is not allowed to drop the final semicolon, because
		// it's the only place where a statement list is not followed 
		// by a closing brace.  Handle the error for pedantry.

		// Find the final token of the statement list.
		// yylast is lookahead; yyprev is last of stmt_list
		last = yyprev;

		if(last > 0 && last != ';' && yychar != '}')
			yyerror("missing statement after label");
	}

caseblock_list:
|	caseblock_list caseblock
	;

loop_body:
	LBODY
	stmt_list '}'
	;

range_stmt:
	expr_list '=' LRANGE expr
|	expr_list LCOLAS LRANGE expr
	;

for_header:
	osimple_stmt ';' osimple_stmt ';' osimple_stmt
|	osimple_stmt
|	range_stmt
	;

for_body:
	for_header loop_body
	;

for_stmt:
	LFOR
	for_body
	;

if_header:
	osimple_stmt
|	osimple_stmt ';' osimple_stmt
	;

/* IF cond body (ELSE IF cond body)* (ELSE block)? */
if_stmt:
	LIF
	if_header
	loop_body
	elseif_list else
	;

elseif:
	LELSE LIF 
	if_header loop_body
	;

elseif_list:
|	elseif_list elseif
	;

else:
|	LELSE compound_stmt
	;

switch_stmt:
	LSWITCH
	if_header
	LBODY caseblock_list '}'
	;

select_stmt:
	LSELECT
	LBODY caseblock_list '}'
	;

/*
 * expressions
 */
expr:
	uexpr
|	expr LOROR expr
|	expr LANDAND expr
|	expr LEQ expr
|	expr LNE expr
|	expr LLT expr
|	expr LLE expr
|	expr LGE expr
|	expr LGT expr
|	expr '+' expr
|	expr '-' expr
|	expr '|' expr
|	expr '^' expr
|	expr '*' expr
|	expr '/' expr
|	expr '%' expr
|	expr '&' expr
|	expr LANDNOT expr
|	expr LLSH expr
|	expr LRSH expr
	/* not an expression anymore, but left in so we can give a good error */
|	expr LCOMM expr
	;

uexpr:
	pexpr
|	'*' uexpr
|	'&' uexpr
|	'+' uexpr
|	'-' uexpr
|	'!' uexpr
|	'~' uexpr
	{
		yyerror("the bitwise complement operator is ^");
	}
|	'^' uexpr
|	LCOMM uexpr
	;

/*
 * call-like statements that
 * can be preceded by 'defer' and 'go'
 */
pseudocall:
	pexpr '(' ')'
|	pexpr '(' expr_or_type_list ocomma ')'
|	pexpr '(' expr_or_type_list LDDD ocomma ')'
	;

pexpr_no_paren:
	LLITERAL
|	name
|	pexpr '.' sym
|	pexpr '.' '(' expr_or_type ')'
|	pexpr '.' '(' LTYPE ')'
|	pexpr '[' expr ']'
|	pexpr '[' oexpr ':' oexpr ']'
|	pexpr '[' oexpr ':' oexpr ':' oexpr ']'
|	pseudocall
|	convtype '(' expr ocomma ')'
|	comptype lbrace start_complit braced_keyval_list '}'
|	pexpr_no_paren '{' start_complit braced_keyval_list '}'
|	'(' expr_or_type ')' '{' start_complit braced_keyval_list '}'
	{
		yyerror("cannot parenthesize type in composite literal");
	}
|	fnliteral

start_complit:
	{
		// composite expression.
		// make node early so we get the right line number.
	}
	;

keyval:
	expr ':' complitexpr
	;

bare_complitexpr:
	expr
|	'{' start_complit braced_keyval_list '}'
	;

complitexpr:
	expr
|	'{' start_complit braced_keyval_list '}'
	;

pexpr:
	pexpr_no_paren
|	'(' expr_or_type ')'
	;

expr_or_type:
	expr
|	non_expr_type	%prec PreferToRightParen

name_or_type:
	ntype

lbrace:
	LBODY
|	'{'
	;

/*
 * names and types
 *	newname is used before declared
 *	oldname is used after declared
 */
new_name:
	sym
	;

dcl_name:
	sym
	;

onew_name:
|	new_name
	;

sym:
	LNAME
|	hidden_importsym
|	'?'
	;

hidden_importsym:
	'@' LLITERAL '.' LNAME
|	'@' LLITERAL '.' '?'
	;

name:
	sym	%prec NotParen
	;

labelname:
	new_name
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
|	LDDD ntype
	;

ntype:
	recvchantype
|	fntype
|	othertype
|	ptrtype
|	dotname
|	'(' ntype ')'
	;

non_expr_type:
	recvchantype
|	fntype
|	othertype
|	'*' non_expr_type
	;

non_recvchantype:
	fntype
|	othertype
|	ptrtype
|	dotname
|	'(' ntype ')'
	;

convtype:
	fntype
|	othertype

comptype:
	othertype
	;

fnret_type:
	recvchantype
|	fntype
|	othertype
|	ptrtype
|	dotname
	;

dotname:
	name
|	name '.' sym
	;

othertype:
	'[' oexpr ']' ntype
|	'[' LDDD ']' ntype
|	LCHAN non_recvchantype
|	LCHAN LCOMM ntype
|	LMAP '[' ntype ']' ntype
|	structtype
|	interfacetype
	;

ptrtype:
	'*' ntype
	;

recvchantype:
	LCOMM LCHAN ntype
	;

structtype:
	LSTRUCT lbrace structdcl_list osemi '}'
|	LSTRUCT lbrace '}'
	;

interfacetype:
	LINTERFACE lbrace interfacedcl_list osemi '}'
|	LINTERFACE lbrace '}'
	;

/*
 * function stuff
 * all in one place to show how crappy it all is
 */
xfndcl:
	LFUNC fndcl fnbody
	;

fndcl:
	sym '(' oarg_type_list_ocomma ')' fnres
|	'(' oarg_type_list_ocomma ')' sym '(' oarg_type_list_ocomma ')' fnres
	;

hidden_fndcl:
	hidden_pkg_importsym '(' ohidden_funarg_list ')' ohidden_funres
|	'(' hidden_funarg_list ')' sym '(' ohidden_funarg_list ')' ohidden_funres
	;

fntype:
	LFUNC '(' oarg_type_list_ocomma ')' fnres
	;

fnbody:
|	'{' stmt_list '}'
	;

fnres:
	%prec NotParen
|	fnret_type
|	'(' oarg_type_list_ocomma ')'
	;

fnlitdcl:
	fntype
	;

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
|	LLITERAL
	;

/*
 * import syntax from package header
 */
hidden_import:
	LIMPORT LNAME LLITERAL ';'
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
|	'[' LLITERAL ']' hidden_type
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
	LLITERAL
|	'-' LLITERAL
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