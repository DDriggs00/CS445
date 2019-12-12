#include <stdio.h>
#include "node.h"
#include "node_list.h"
#include "node_iterator.h"
#include "traversals.h"

void codegen(node_t* tree)
{
   if (!tree) return;

   // Process children
   node_t* node;
   node_iterator_t* it = node_iterator_create(tree->children);
   while (node = node_iterator_next(it)) {
      codegen(node);
   }

   /*
    * back from children, consider what we have to do with
    * this node. The main thing we have to do, one way or
    * another, is assign t->code
    */
   switch (tree->tag) {
   case O_ADD: {
      struct instr *g;
      t->code = concat(t->child[0]->code, t->child[1]->code);
      g = gen(O_ADD, t->address,
              t->child[0]->address, t->child[1]->address);
      t->code = concat(t->code, g);
      break;
      }
   /*
    * ... really, a bazillion cases, up to one for each
    * production rule (in the worst case)
    */
   default:
      /* default is: concatenate our children's code */
      t->code = NULL;
      for(i=0;i<t->nkids;i++)
         t->code = concat(t->code, t->child[i]->code);
   }
}
