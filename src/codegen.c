#include <stdio.h>

#include "codegen.h"
#include "tac.h"
#include "node.h"
#include "node_list.h"
#include "node_iterator.h"
#include "traversals.h"
#include "nodeTypes.h"

void codegen(node_t* tree)
{
    if (!tree) return;

    // Process children
    node_t* node;
    node_iterator_t* it = node_iterator_create(tree->children);
    while ((node = node_iterator_next(it))) {
        codegen(node);
    }

   /*
    * back from children, consider what we have to do with
    * this node. The main thing we have to do, one way or
    * another, is assign tree->tac
    */
    instruction_t* tac;
    switch (tree->tag) {
        case tag_expr:
            switch (tree->children->begin->next->tag) {
                case '+':
                    tac = tacGen(O_ADD, tree->address, tree->children->begin->address, tree->children->end->address);
                    break;
                case '-':
                    tac = tacGen(O_SUB, tree->address, tree->children->begin->address, tree->children->end->address);
                    break;
                case '*':
                    tac = tacGen(O_MUL, tree->address, tree->children->begin->address, tree->children->end->address);
                    break;
                case '/':
                    tac = tacGen(O_DIV, tree->address, tree->children->begin->address, tree->children->end->address);
                    break;
            }
            tree->tac = tacCat(tree->children->begin->tac, tree->children->end->tac);
            tree->tac = tacCat(tree->tac, tac);
            break;

        default:
            // concatenate our children's code
            it = node_iterator_create(tree->children);
            while ((node = node_iterator_next(it))) {
                tree->tac = tacCat(tree->tac, node->tac);
            }

   }
}
