/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef __BM_LIST_H__
#define __BM_LIST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct list_node
{
    struct list_node *next;                         /**< point to next node. */
    struct list_node *prev;                         /**< point to prev node. */
    void * data;
};
typedef struct list_node list_t;                    /**< Type for lists. */



void list_init(list_t *l);
void list_insert_after(list_t *l, list_t *n);
void list_insert_before(list_t *l, list_t *n);
void list_remove(list_t *n);
int list_isempty(const list_t *l);
int list_get_count(const list_t *l);

#ifdef __cplusplus
}
#endif

#endif

