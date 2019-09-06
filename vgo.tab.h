/* adapted from the go 1.2.2 distribution */

/* Literals */
#define LLITERAL        300 // String Literal
#define LINT            301 // Integer
#define LFLOAT          302 // Float
#define LHEX            303 // Hexadecimal number
#define LBIN            304 // Binary number
#define LOCT            305 // Octal number
#define LRUNE           306 // Rune (char)
#define LVARNAME        307 // Variable name

// Keywords
#define LFUNC           400
#define LMAP            401
#define LSTRUC          402
#define LELSE           403
#define LPACKAGE        404
#define LCONST          405
#define LIF             406
#define LTYPE           407
#define LFOR            408
#define LIMPORT         409
#define LRETURN         410
#define LVAR            411

#define LBADKEYWORD     499

// Variable types
#define LTYPEBOOL       413
#define LTYPESTRING     414
#define LTYPEINT        415
#define LTYPERUNE       416
#define LTYPEFLOAT64    417

#define LLE     500		/* <= */
#define LGE     501		/* >= */
#define LEQ     502		/* == */
#define LNE     503		/* != */
#define LANDAND 504	    /* && */
#define LOROR   505	    /* || */
#define LPLASN  506	    /* += */
#define LMIASN  507	    /* -= */
#define LPLPL   508	    /* ++ */
#define LMIMI   509	    /* -- */
#define LASN    510	    /* = */
#define LLP     511	    /* ( */
#define LRP     512	    /* ) */
#define LLB     513	    /* [ */
#define LRB     514		/* ] */
#define LLC     513	    /* { */
#define LRC     514		/* } */
#define LLT     515	    /* < */
#define LGT     516	    /* > */
#define LDOT    517	    /* . */
#define LCOM    517	    /* , */
#define LBANG   518		/* ! */
#define LPLUS   519	    /* + */
#define LMINUS  520	    /* - */
#define LMUL    521	    /* * */
#define LDIV    522	    /* / */
#define LMOD    523	    /* % */

#define LBADOPERATOR 599