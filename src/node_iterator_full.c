#include <stdlib.h>

#include "node.h"
#include "node_iterator_full.h"
#include "node_list.h"

void node_iterator_full_destroy(node_iterator_full_t* iterator) {
    if(iterator) {
        free(iterator);
    }
}

node_iterator_full_t* node_iterator_full_create(node_t* root) {
    if (!root) return NULL;

    node_iterator_full_t* it = (node_iterator_full_t*)malloc(sizeof(node_iterator_full_t));
    if(it == NULL) {
		return NULL;
	}

    it->current = root;
    it->root = root;
    it->count = 0;

    return it;
}

struct node_t* node_iterator_full_next(struct node_iterator_full_t* iterator) {
    if (!iterator) return NULL;

    // Return first token first
    if(iterator->count == 0) {
        iterator->count++;
        return iterator->current;
    }

    if(iterator->current->count > 0) {
        iterator->current = iterator->current->children->begin;
        iterator->count++;
        return iterator->current;
    }
    else {
        if (iterator->current->parent == NULL) {
            return NULL;
        }
        node_iterator_t* it = node_iterator_create(iterator->current->parent->children);
        node_t* temp = iterator->current;
        node_t* temp2;
        while ((temp2 = node_iterator_next(it))) {
            if (temp2 == temp) {
                // Iterator has reached current child
                if ((temp2 = node_iterator_next(it))) {
                    // item was not last in parent
                    iterator->current = temp2;
                    iterator->count++;
                    node_iterator_destroy(it);
                    return iterator->current;
                }
                else {
                    // Node was last in parent tree

                    // replace the currently iterating iterator with the parent
                    temp = temp->parent;
                    if (temp->parent == NULL) {
                        node_iterator_destroy(it);
                        return NULL;
                    }
                    node_iterator_destroy(it);
                    it = node_iterator_create(temp->parent->children);
                    // the same while loop will move to the parent
                }
            }
        }
    }
    return NULL;
}

struct node_t* node_iterator_full_skip_subtree(struct node_iterator_full_t* iterator) {
    if (!iterator) return NULL;
    if (iterator->current->parent == NULL) {
        return NULL;
    }
    
    node_iterator_t* it = node_iterator_create(iterator->current->parent->children);
    node_t* temp = iterator->current;
    node_t* temp2;
    while ((temp2 = node_iterator_next(it))) {
        if (temp2 == temp) {
            // Iterator has reached current child
            if ((temp2 = node_iterator_next(it))) {
                // item was not last in parent
                iterator->current = temp2;
                iterator->count++;
                node_iterator_destroy(it);
                return iterator->current;
            }
            else {
                // Node was last in parent tree

                // replace the currently iterating iterator with the parent
                temp = temp->parent;
                if (temp->parent == NULL) {
                    node_iterator_destroy(it);
                    return NULL;
                }
                node_iterator_destroy(it);
                it = node_iterator_create(temp->parent->children);
                // the same while loop will move to the parent
            }
        }
    }
    return NULL;
}
