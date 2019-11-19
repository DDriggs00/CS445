/*
 * list.h
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

#ifndef LIST_H_
#define LIST_H_

#include "object.h"

typedef struct list_t {
	struct list_t* next;
	void* data;
	struct list_t* prev;
} list_t;

list_t* list_create(void* data);
void list_init(struct list_t* list);
void list_destroy(struct list_t* list);

void list_add(struct list_t* list, void* data);
void list_remove(struct list_t* list);

#endif /* LIST_H_ */
