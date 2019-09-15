# CS 445 Assignment 1: A Lexical Analyzer

Due: Thursday September 5, 11:59pm

In this assignment you will write a lexical analyzer in `flex(1)`, for a subset of Go known as the `VGo` language that is documented in its own reference document, with a link given below.

## **Engineering Requirements**

In this and all subsequent assignments in 445, please meet the following engineering requirements. Points will be assigned in grading for them.

Mandatory .zip file unpacks to the current directory.
- Turnin Must be a .zip containing a valid compressed archive that can be uncompressed on Linux via unzip. It may not be a .tar or a .rar or a .bzip or whatever, whether disguised or renamed or not. The .zip must unpack into the current directory, not a subdirectory.

"make" just works on `cs-445.cs.uidaho.edu`; it builds a `vgo` executable.  
- You have to supply a makefile that contains build rules as follows.

Separate Compilation.
- The C compiler must be invoked separately on each .c file. All .c files must be linked together via .o object files, no using `#include` of .c files. No including any code (function bodies) in .h files.

No Warnings.
- The compiler must be invoked with -Wall on all compilation lines. Points will be lost if you don't fix warnings. There are some common lex/flex warnings, such as about not using `input()` that are no big deal, but use
- `%option noinput`
- `%option nounput`
- to shut them up. See the instructor if you are unable to fix a warning.

Valgrind validation

- You should test your work on cs-445 both with and without valgrind. Valgrind output should be free of memory errors. You will probably also become intimate with gdb in this class, but valgrind is your first line of defense. For the purposes of this class, a "memory error" is a message from valgrind indicating a read or write of one or more bytes of illegal, out-of-bounds, or uninitialized memory. Other valgrind messages may be useful but will not cost you points.

## **Feature Requirements**

In this document, the term "must" indicates a feature that is required for passing grade, while the term "should" indicates a feature that is expected for a grade of "A" or "B". If you do not know what something means, or don't know how to do it, you are encouraged to ask and find out rather than turning in a homework that does not meet the specifications.

Your program executable must be named `vgo`. Your program should read in any number of source files named on the command line and write output with one line for each token, described below. Source files must accept the extension `.go`. The compiler should automatically add `.go` to the end of filenames if no other extension is given. (Eventually in a later homework, the compiler will automatically names the executable the same name as the first argument. For this assignment there is no output executable.)

Compilers and related tools are used by programs such as `make(1)` that read the process exit status to tell whether all is well. Your program's exit status should return 0 if there are no errors, and a nonzero number to indicate errors. For lexical errors, return 1.

## **Language Details**

The `VGo` language is described at [http://www2.cs.uidaho.edu/~jeffery/courses/445/vgo.html](http://www2.cs.uidaho.edu/~jeffery/courses/445/vgo.html). As this is a new language this semester, these details will be corrected and amended as needed in response to student questions.

## **Lexical Attributes**

In your `yylex()`, you should compute attributes for each token, and store them in a global variable named `yytoken`. Note that this is not part of the lex/yacc public interface, although it is named so as to be a recognizable extension of said interface. You should use the following token type, or a compatible extension of it.

```c
struct token {
   int category;    /* the integer code returned by yylex */
   char *text;     /* the actual string (lexeme) matched */
   int lineno;      /* the line number on which the token occurs */
   char *filename; /* the source file in which the token occurs */
   int ival;        /* for integer constants, store binary value here */
   double dval;	    /* for real constants, store binary value here */
   char *sval;     /* for string constants, malloc space, de-escape, store */
                    /*     the string (less quotes and after escapes) here */
   }
```

In this homework your `main()` procedure should build a LINKED LIST of all the token structs, each of which is created by `yylex()`. In the next assignment, we will discard the linked list and instead insert all these tokens into a tree.

Example linked list structure:

```c
   struct tokenlist {
      struct token *t;
      struct tokenlist *next;
      }
```

Use the `malloc()` function to allocate chunks of memory for `struct token` and `struct tokenlist`.

### **yylex() and main()**

Your `yylex()` should return a different unique integer > 257 for each reserved word, and for each other token category (identifier, integer literal constant, string literal constant, addition operator, etc). Numbers > 257 are required for the sake of compatibility with the parser generator tool. For each such number, you must `#define` a symbol, as in

`#define IDENTIFIER 260`

This is required for the sake of readability. Your `yylex()` should return -1 when it hits end of file.

In this assignment, there should be (at least) two separately-compiled .c files, a .h file and a makefile. The `yylex()` function must be called by a main() procedure in a loop. The main() procedure should for each token, write out a line containing the token category (an integer > 257) and lexical attributes.

## **Turn in...**

An electronic copy via bblearn.uidaho.edu. The electronic copy should be a compressed archive .zip file, containing makefile, flex clex.l file, main.c file, and ytab.h file. If you add any new source files, be sure you add it to the set of files that you turn in.

## Example

For the input file

```go
package main
import "fmt"
func main() {
     fmt.Printf("Hello\\tworld\\n")
}
```

your output should look something like the following. Integer categories are for illustration purposes; yours may be different.
```
Category	Text		Lineno		Filename	Ival/Sval
-------------------------------------------------------------------------
295		    package		1		    hello.go
271		    main		1		    hello.go
293		    import		2		    hello.go
271		    "fmt"		2		    hello.go	fmt
294		    func		3		    hello.go
271		    main		3		    hello.go
290		    (		    3		    hello.go
291		    )		    3		    hello.go
292		    {		    3		    hello.go
271		    fmt		    4		    hello.go
296		    .		    4		    hello.go
271		    Printf		4		    hello.go
290		    (		    4		    hello.go
271		    "Hello\\tworld\\n" 4	hello.go	Hello	world

291		    )		    4		    hello.go
293		    }		    5		    hello.go
```