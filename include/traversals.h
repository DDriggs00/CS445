#ifndef TRAVERSALS_H
#define TRAVERSALS_H

#include <stdbool.h>

#include "node.h"



// Traverses the given tree, and returns the first node with *tag*
// If no match found, returns NULL
node_t* findNode(node_t* tree, int tag);

// Searches only direct children of the tree for *tag*
// If no match is found, returns NULL
node_t* findNodeShallow(node_t* tree, int tag);

// Returns the nth sibling of the given tree node
// Eg. (-1 returns the prior sibling, 1 returns the next sibling)
// If the desired sibling does not exist, returns NULL
node_t* getSibling(node_t* tree, int nthSibling);

// counts occurances of *tag* in the given tree
int treeCount(node_t* tree, int tag);

// traverses the tree to get the first node containing a token symbol
node_t* getFirstTerminal(node_t* node);

// returns true of the node has a parent with *tag*, otherwise false
bool hasParent(node_t* node, int tag);

#endif // TRAVERSALS_H