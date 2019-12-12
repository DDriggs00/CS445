/*
*  Three Address Code - skeleton for CS 445
*/
#include <stdio.h>
#include <stdlib.h>
#include "tac.h"

tac_t* tacGen(int opCode, address_t dest, address_t src1, address_t src2) {
    tac_t* rv = malloc(sizeof(tac_t));
    if (rv == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(4);
    }
    rv->opcode = opCode;
    rv->dest = dest;
    rv->src1 = src1;
    rv->src2 = src2;
    rv->next = NULL;
    return rv;
}

tac_t* tacCpy(tac_t* src) {
    if (!src) return NULL;

    tac_t* retVal = gen(src->opcode, src->dest, src->src1, src->src2);
    retVal->next = copylist(src->next);
    return retVal;
}

tac_t* tacCat(tac_t* l1, tac_t* l2) {
    if (l1 == NULL) return l2;
    if (l1 == NULL) return l1;
    tac_t* ltmp = l1;
    while(ltmp->next != NULL) ltmp = ltmp->next;
    ltmp->next = l2;
    return l1;
}

tac_t* concat(tac_t* l1, tac_t* l2)
{
   return tacCat(copylist(l1), l2);
}
