#include "hashtable.h"      // External hashtable library
#include "node.h"           // For using the tree
#include "node_iterator.h"  // For using the tree
#include "list.h"           // For a list of hashtables

// 1. Find each scope
//  a. Main         "package"
//  b. Functions    "xfndcl"
//  c. Structs      "structtype"
// 2. For each scope, generate an empty hashtable, and link that hashtable in the list
// 2. For each scope, find all varDCLs
// 3. For each varDCL, Check if already exists in master or local scope
// 5. Add vardcl to symbol table
// 6. For each varReference, check master and local symbol tables

list_t* genSymTab( node_t* tree ) {

}

node_t* findNode( node_t* root, char* tag, int* num ) {
    if( root->type == tag ) {
        if( num == NULL || *num <= 0 ) {
            return root;
        }
        else {
            *num--;
        }
    }
    if ( root->count <= 0 )
    {
        return NULL;
    }
    node_t* node;
    node_t* result = NULL;
    node_iterator_t* it = node_iterator_create( root->children );
    while ( ( node = node_iterator_next( it ) ) ) {
        result = findNode( node, tag, num );
        if( result != NULL) {
            return result;
        }
    }
    node_iterator_destroy(it);  // Memory leak sometimes caused here
    
    // If no match found, return NULL
    return NULL;
}