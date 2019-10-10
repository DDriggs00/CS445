#ifndef NODE_ITERATOR_FULL_H
#define NODE_ITERATOR_FULL_H

#include "node_iterator.h"
#include "node.h"

// This class implements the abstract iterator class
typedef struct node_iterator_full_t {

    node_t* root;
    node_t* current;

	unsigned int count;

	struct node_t* begin;

} node_iterator_full_t;

void node_iterator_full_destroy(node_iterator_full_t* iterator);
node_iterator_full_t* node_iterator_full_create(node_t* root);

struct node_t* node_iterator_full_next(struct node_iterator_full_t* iterator);

#endif /* NODE_ITERATOR_FULL_H */
