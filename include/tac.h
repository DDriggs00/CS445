/*
 * Three Address Code - skeleton for CS 445
 */
#ifndef TAC_H
#define TAC_H

typedef struct address_t {
    int region, offset;
} address_t;

typedef struct tac_t {
    int opcode;
    address_t dest, src1, src2;
    struct tac_t* next;
} tac_t;

/* Regions: */
#define R_GLOBAL 2001 /* can assemble as relative to the pc */
#define R_LOCAL  2002 /* can assemble as relative to the ebp */
#define R_CLASS  2003 /* can assemble as relative to the 'this' register */
#define R_LABEL  2004 /* pseudo-region for labels in the code region */
#define R_CONST  2005 /* pseudo-region for immediate mode constants */

/* Opcodes, per lecture notes */
#define O_ADD   3001
#define O_SUB   3002
#define O_MUL   3003
#define O_DIV   3004
#define O_NEG   3005
#define O_ASN   3006
#define O_ADDR  3007
#define O_LCONT 3008
#define O_SCONT 3009
#define O_GOTO  3010
#define O_BLT   3011
#define O_BLE   3012
#define O_BGT   3013
#define O_BGE   3014
#define O_BEQ   3015
#define O_BNE   3016
#define O_BIF   3017
#define O_BNIF  3018
#define O_PARM  3019
#define O_CALL  3020
#define O_RET   3021

/* declarations/pseudo instructions */
#define D_GLOB  3051
#define D_PROC  3052
#define D_LOCAL 3053
#define D_LABEL 3054
#define D_END   3055

tac_t* tacGen(int opCode, address_t dest, address_t src1, address_t src2);
tac_t* tacCat(tac_t* l1, tac_t* l2);
tac_t* tacCpy(tac_t* src);

#endif
