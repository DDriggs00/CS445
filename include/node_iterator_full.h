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

// returns an object that performs a full deep iteration through a tree
node_iterator_full_t* node_iterator_full_create(node_t* root);

// frees memory claimed by the iterator object
void node_iterator_full_destroy(node_iterator_full_t* iterator);

// returns the next node in the tree
struct node_t* node_iterator_full_next(struct node_iterator_full_t* iterator);

// Returns the next node in the tree, skipping all children of the current node
struct node_t* node_iterator_full_skip_subtree(struct node_iterator_full_t* iterator);

#endif /* NODE_ITERATOR_FULL_H */
