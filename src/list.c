/*
 * list.c
 *
 *  Created on: Mar 8, 2011
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

// Heavily modified by Devin Driggs - Same license

#include <stdio.h>
#include <stdlib.h>

#include "list.h"

list_t* list_create(void* data) {
	list_t* list = malloc(sizeof(list_t));
	list->data = data;
	list->next = NULL;
	list->prev = NULL;
	return  list;
}

void list_init(list_t* list) {
	list->next = NULL;
	list->prev = list;
}


void list_destroy(list_t* list) {
	if(list) {
		free(list);
	}
}

void list_add(list_t* list, void* data) {
	list_t* next = list_create(data);
	list_t* l2 = list;
	while(l2->next) l2 = l2->next;
	l2->next = next;
	next->prev = l2;
}

void list_remove(list_t* list) {
	list_t* prev = list->prev;
	list_t* next = list->next;
	if (prev) prev->next = next;
	if (next) next->prev = prev;
	list_destroy(list);
}
