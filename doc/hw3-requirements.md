# CS 445 HW#3

Due: Tuesday October 15 11:59pm

Perform the first part of semantic analysis for VGo. This shall consist of the following items. Please turnin via bblearn an electronic copy of your whole project in a .zip. Please create your .zip file in such a way that it unpacks into the current directory, rather than a subdirectory.

Symbol tables

> Build a symbol table data type that you can instantiate for each scope: one "global" scope (a.k.a. package main), one scope per struct type for member variables , and one scope per function for parameters and locals. You do not have to support nested local scopes. For each symbol, you should insert a symbol table entry, which is a struct consisting of enough information to support the semantic analysis. Typically this will include symbol name, reference to which scope (containing symbol table) the symbol is defined in, declared data type (required in the next homework), and any auxiliary flags, e.g. const. For full credit, implement a hash table; a binary tree would be a fine alternative (90% credit); a linked list would work but not be so great (80% credit).

Predefined symbol tables

> In addition to the symbol tables for package main, we need to allow the little toy VGo programs to at least import fmt, containing a Println and maybe a bit more.

Variable declarations

> You must emit an error message if a variable is redeclared in the same scope. You must detect and report an error for undeclared variables. Both of these error message types should includ filename, line number, and offending variable name.

## Output

For this Assignment, you should implement a _command-line option_ `-symtab` and print your symbol tables in a human readable format if it is given on the command line. The symbol table output should look like:

```
--- symbol table for: package main ---
    x int
    y float64
    f func
---
--- symbol table for: func f ---
    a int
    b int
---
```
## Notes

*   your executable must be named vgo
*   your makefile must compile all your .c files using the -Wall option, and produce .o files for them.
*   your makefile must have a separate link step that builds vgo from .o files
*   your program should accept and process an arbitrary number of source filenames on the command line
*   write out the name of the file to standard out when you open it
*   do NOT write out the tree, or other debugging information, by default; you may add command line options to print that info if you want.
*   error messages should be written to _standard error_ (`stderr`) not stdout
*   if ANY file has a lexical error, your process exit status should return 1, for a syntax error 2, for a semantic error 3, and for no errors, return 0.