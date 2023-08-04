/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "list.h"



/**
 * @brief initialize a list
 *
 * @param l list to be initialized
 */
void list_init(list_t *l)
{
    l->next = l->prev = l;
}

/**
 * @brief insert a node after a list
 *
 * @param l list to insert it
 * @param n new node to be inserted
 */
void list_insert_after(list_t *l, list_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/**
 * @brief insert a node before a list
 *
 * @param n new node to be inserted
 * @param l list to insert it
 */
void list_insert_before(list_t *l, list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

/**
 * @brief remove node from list.
 * @param n the node to remove from the list.
 */
void list_remove(list_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

/**
 * @brief tests whether a list is empty
 * @param l the list to test.
 * @return 0:not empty, 1:empty
 */
int list_isempty(const list_t *l)
{
    return l->next == l;
}

int list_get_count(const list_t *l)
{
    int i;
    list_t *n = l->next;
    i = 0;
    
    while(n != l)
    {
        //printf("%s:0x%08X  data:0x%08X\r\n", "n", (uint32_t)n, n->data);
        n = n->next;
        i++;
    }
    return i;
}





