#include <stdlib.h>         // for NULL, MALLOC
#include <stdbool.h>

#include "node.h"               // for pretty much everything
#include "node_iterator.h"      // for pretty much everything
#include "node_iterator_full.h" // for count function

node_t* findNode(node_t* tree, int tag) {
    if (!tree) return NULL;
    if(tree->tag == tag) {
        return tree;
    }
    if (tree->count <= 0) {
        return NULL;
    }
    node_t* node;
    node_t* result = NULL;
    node_iterator_t* it = node_iterator_create(tree->children);
    while ((node = node_iterator_next(it))) {
        result = findNode(node, tag);
        if(result != NULL) {
            return result;
        }
    }
    node_iterator_destroy(it);  // Memory leak sometimes caused here

    // If no match found, return NULL
    return NULL;
}

node_t* findNodeShallow(node_t* tree, int tag) {
    if (!tree) return NULL;
    if (tree->count <= 0) return NULL;

    node_t* node;
    node_iterator_t* it = node_iterator_create(tree->children);
    while ((node = node_iterator_next(it))) {
        if (node->tag == tag) {
            node_iterator_destroy(it);
            return node;
        }
    }

    // If no match found, return NULL
    node_iterator_destroy(it);
    return NULL;
}

node_t* getSibling(node_t* tree, int nthSibling) {
    if (!tree) return NULL;
    if (nthSibling == 0) return tree;

    node_t* node = tree;
    if (nthSibling > 0) {
        for (int i = nthSibling; i > 0; i--) {
            node = node->next;
            if (!node) return NULL;
        }
        return node;
    }
    if (nthSibling < 0) {
        for (int i = nthSibling; i < 0; i++) {
            node = node->prev;
            if (!node) return NULL;
        }
        return node;
    }

    // Should never be reached
    return NULL;
}

int treeCount(node_t* tree, int tag) {
    if (!tree) return -1;

    int count = 0;
    node_t* node;
    node_iterator_full_t* it = node_iterator_full_create(tree);
    while ((node = node_iterator_full_next(it))) {
        if (node->tag == tag) {
            count++;
        }
    }

    return count;
}

node_t* getFirstTerminal(node_t* node) {
    if (!node) return NULL;
    if (node->hasData) return node;

    node_iterator_full_t* it = node_iterator_full_create(node);
    while ((node = node_iterator_full_next(it))) {
        if (node->hasData) return node;
    }
    return NULL;
}

bool hasParent(node_t* node, int tag) {
    if (!node) return NULL;

    node_t* node2 = node;
    while((node2 = node2->parent)) {
        if (node2->tag == tag) {
            return true;
        }
    }
    return false;
}
