/*
*  Three Address Code - skeleton for CS 445
*/
#include <stdio.h>
#include <stdlib.h>
#include "tac.h"

instruction_t* tacGen(int opCode, address_t* dest, address_t* src1, address_t* src2) {
    instruction_t* rv = malloc(sizeof(instruction_t));
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

instruction_t* tacGenChild(int opCode, address_t* dest, address_t* src1, address_t* src2, instruction_t* parent) {
    instruction_t* child = tacGen(opCode, dest, src1, src2);
    parent->next = child;
    return child;
}

instruction_t* tacCpy(instruction_t* src) {
    if (!src) return NULL;

    instruction_t* retVal = tacGen(src->opcode, src->dest, src->src1, src->src2);
    retVal->next = tacCpy(src->next);
    return retVal;
}

instruction_t* tacCat(instruction_t* l1, instruction_t* l2) {
    if (l1 == NULL) return l2;
    if (l2 == NULL) return l1;

    instruction_t* ltmp = l1;
    while(ltmp->next != NULL) ltmp = ltmp->next;
    ltmp->next = l2;
    return l1;
}
