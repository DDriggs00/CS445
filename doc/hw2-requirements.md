# CS 445 HW#2

Due: Sunday September 22 11:59pm.

- Turnin on bblearn a .zip file named hw2.zip.
- The .zip should unpack in the current directory, include a makefile and all files necessary to compile to executable.
- The compile should be free of warnings, other than shift/reduce conflicts.
- Your makefile must use -Wall gcc option for all compiles
- Do not leave yydebug/YYDEBUG turned on. Do not leave any spurious or debugging output in your submitted version.
- Execution should be valgrind-clean, meaning no illegal memory reads or writes, and no use of uninitialized variables.

Write or adapt a VGo grammar that works with Bison, and use it to produce a syntax tree for your input VGo source files. The recommended starting point is
- [go.y](go.y), adapted from version 1.2.2 of the Go compiler, along with your lexical analyzer.
- I will eventually finish a [go.l](go.l) reference lexical analyzer; it is still under construction.
- Similarly, you have to produce an adaptation of this [Makefile](makefile.vgo.hw2) to work with Bison for this homework.

To the extent that you modify the grammar, you are advised to ignore/allow any number of shift/reduce conflicts, but to not tolerate any reduce/reduce conflicts -- they generally require grammar changes such as refactoring common sequences within two or more production rules. Whichever grammar/parser you use, you are responsible for making it work for VGo and for any bugs. You may write your own grammar from scratch if you prefer.

Your parser should include at least the following:

- To use your hw#1 flex, rename terminal symbols from your HW#1 or from the grammar (.y file), or vice-versa, so the two use the same names/#defines.
- Add terminal symbols required by the .y file, possibly adapting missing symbols' regular expressions from the sample go.l file. Note that the .y file uses many single-character operators and punctuation symbols as terminal symbols whose integer codes are the same as their ASCII value.
- Resolve matters regarding type names (for example, the names of classes) and the need to return different terminal symbol codes for "identifier" regular expressions, depending on syntactic context.
- For every production rule in the grammar:
    - if the right hand side has > 1 children, add a semantic action to construct a construct a tree node and assign $$ to point at it
    - if the right hand side has 0 or 1 children, add a semantic action to set $$ to NULL (0 children) or to the child if there is 1 child ($$ = $1).
- Place a pointer to your token in a yylval field on each call to yylex(), and ensure that those tokens get inserted as leaves into your syntax tree.
- If the parse succeeds, assign the root of the tree to a global variable from the semantic action associated with your start symbol.
- Revise the main() procedure from HW#1 to call yyparse() and use its return value and/or global variables to ascertain whether the parse succeeded.
- If the parse succeeded, traverse/print your syntax tree as described below.

If there is a syntax error, you should report the filename and line number. Your program's exit status should be 0 if there are no errors, and a nonzero number to indicate errors. For lexical errors, use the exit status 1. For syntax errors, the exit status should be 2.

If there are no errors, you should print a text representation of a syntax tree.

Suppose we have a tree structure that looks something like:
```c
struct tree {
   int prodrule;
   int nkids;
   struct tree *kids[9];
   struct token *leaf;
}

int treeprint(struct tree *t, int depth)
{
  int i;

  printf("%*s %s: %d\n", depth*2, " ", humanreadable(t->prodrule), t->nkids);
  
  for(i=0; i<t->nkids; i++)
    treeprint(t->kids[i], depth+1);

}
```
you might get output that looks something like
```
program: 1
 function: 2
  function header: 3
   typedecl: 1
   functioname: 1
   parmlist: 3
    ....
```

For leaves, print out the integer code and the lexeme (yytext\[\]) for the token, if any.