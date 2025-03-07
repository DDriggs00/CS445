/*
 * node.c
 *
 *  Created on: Mar 7, 2011
 *      Author: posixninja
 *
 * Copyright (c) 2011 Joshua Hill. All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "node.h"
#include "node_list.h"
#include "node_iterator.h"

void node_destroy(node_t* node) {
	if(!node) return;

	if (node->children && node->children->count > 0) {
		node_t* ch;
		while ((ch = node->children->begin)) {
			node_list_remove(node->children, ch);
			node_destroy(ch);
		}
	}
	node_list_destroy(node->children);
	node->children = NULL;

	free(node);
}

node_t* node_create(node_t* parent, void* data, int tag) {
	int error = 0;

	node_t* node = (node_t*) malloc(sizeof(node_t));
	if(node == NULL) {
		return NULL;
	}
	memset(node, '\0', sizeof(node_t));

	node->data = data;
	node->tac = NULL;
	node->address = NULL;
	node->depth = 0;
	node->next = NULL;
	node->prev = NULL;
	node->count = 0;
	node->isLeaf = TRUE;
	node->isRoot = TRUE;
	node->hasName = FALSE;
	node->parent = NULL;
	node->children = node_list_create(node);
	node->tag = tag;

	// Pass NULL to create a root node
	if(parent != NULL) {
		// This is a child node so attach it to it's parent
		error = node_attach(parent, node);
		if(error < 0) {
			// Unable to attach nodes
			printf("ERROR: %d \"Unable to attach nodes\"\n", error);
			node_destroy(node);
			return NULL;
		}
	}

	if(data == NULL) {
		node->hasData = 0;
	}
	else
	{
		node->hasData = 1;
	}
	

	return node;
}

node_t* node_create2(struct node_t* parent, void* data, int tag, int count, ...) {
	node_t* node = node_create(parent, data, tag);
	
	// for each additional arg (in the ...), add it as a child
	va_list va;
	va_start(va, count);
	for (int i = 0; i < count; i++) {
		node_attach(node, va_arg(va, node_t*));
	}

	return node;
}

int node_attach(node_t* parent, node_t* child) {
	if (!parent || !child) return -1;
	child->isLeaf = TRUE;
	child->isRoot = FALSE;
	child->parent = parent;
	child->depth = parent->depth + 1;
	if(parent->isLeaf == TRUE) {
		parent->isLeaf = FALSE;
	}
	int res = node_list_add(parent->children, child);
	if (res == 0) {
		parent->count++;
	}
	return res;
}

int node_detach(node_t* parent, node_t* child) {
	if (!parent || !child) return 0;
	if (node_list_remove(parent->children, child) == 0) {
		parent->count--;
	}
	return 0;
}

int node_insert(node_t* parent, unsigned int index, node_t* child)
{
	if (!parent || !child) return -1;
	child->isLeaf = TRUE;
	child->isRoot = FALSE;
	child->parent = parent;
	child->depth = parent->depth + 1;
	if(parent->isLeaf == TRUE) {
		parent->isLeaf = FALSE;
	}
	int res = node_list_insert(parent->children, index, child);
	if (res == 0) {
		parent->count++;
	}
	return res;
}

void node_debug(node_t* node) {
	int i = 0;
	node_t* current = NULL;
	node_iterator_t* iter = NULL;
	for(i = 0; i < node->depth; i++) {
		printf("\t");
	}
	if(node->isRoot) {
		printf("ROOT\n");
	}

	if(node->isLeaf && !node->isRoot) {
		printf("LEAF\n");

	} else {
		if(!node->isRoot) {
			printf("NODE\n");
		}
		iter = node_iterator_create(node->children);
		for(current = iter->begin; current != NULL; current = iter->next(iter)) {
			node_debug(current);
		}
	}

}

unsigned int node_n_children(struct node_t* node)
{
	if (!node) return 0;
	return node->count;
}

node_t* node_nth_child(struct node_t* node, unsigned int n)
{
	if (!node || !node->children || !node->children->begin) return NULL;
	int index = 0;
	int found = 0;
	node_t *ch;
	for (ch = node_first_child(node); ch; ch = node_next_sibling(ch)) {
		if (index++ == n) {
			found = 1;
			break;
		}
	}
	if (!found) {
		return NULL;
	}
	return ch;
}

node_t* node_first_child(struct node_t* node)
{
	if (!node || !node->children) return NULL;
	return node->children->begin;
}

node_t* node_prev_sibling(struct node_t* node)
{
	if (!node) return NULL;
	return node->prev;
}

node_t* node_next_sibling(struct node_t* node)
{
	if (!node) return NULL;
	return node->next;
}

int node_child_position(struct node_t* parent, node_t* child)
{
	if (!parent || !parent->children || !parent->children->begin || !child) return -1;
	int index = 0;
	int found = 0;
	node_t *ch;
	for (ch = node_first_child(parent); ch; ch = node_next_sibling(ch)) {
		if (ch == child) {
			found = 1;
			break;
		}
		index++;
	}
	if (!found) {
		return -1;
	}
	return index;
}

node_t* node_copy_deep(node_t* node, copy_func_t copy_func)
{
	if (!node) return NULL;
	void *data = NULL;
	if (copy_func) {
		data = copy_func(node->data);
	}
	int tag = node->tag;
	node_t* copy = node_create(NULL, data, tag);
	node_t* ch;
	for (ch = node_first_child(node); ch; ch = node_next_sibling(ch)) {
		node_t* cc = node_copy_deep(ch, copy_func);
		node_attach(copy, cc);
	}
	return copy;
}

void node_add_name(node_t* node, char* name) {
	node->name = calloc(sizeof(char), (strlen(name) + 1));
	strcpy(node->name, name);
	node->hasName = TRUE;
}