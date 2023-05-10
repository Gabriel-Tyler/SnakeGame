/* implement a deque of postions to represent the snake */
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "Deque.h"

void deque_init(Deque *dq)
{
    dq->head = NULL;
    dq->tail = NULL;
}

void deque_clear(Deque *dq)
{
    ListNode *curr, *prev;

    curr = dq->head;
    prev = NULL;

    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
        free(prev);
    }

    deque_init(dq);
}

ListNode *deque_newnode(int x, int y, ListNode *prev, ListNode *next)
{
    ListNode *ln;

    ln = (ListNode *)malloc(sizeof(ListNode));

    if (ln == NULL)
        return NULL;

    ln->pos.x = x;
    ln->pos.y = y;
    ln->prev = prev;
    ln->next = next;

    return ln;
}

bool deque_pushfront(Deque *dq, int x, int y)
{
    ListNode *ln;

    if (dq == NULL)
    {
        fprintf(stderr, "pushFront: dq is null\n");
        return false;
    }

    ln = deque_newnode(x, y, NULL, NULL);

    if (ln == NULL)
    {
        fprintf(stderr, "pushFront: could not alloc listnode\n");
        return false;
    }

    if (dq->head == NULL && dq->tail == NULL)
    {
        dq->head = ln;
        dq->tail = ln;
        return true;
    }

    if (dq->head == NULL)
    {
        fprintf(stderr, "pushfront: head is null\n");
        return false;
    }
    if (dq->tail == NULL)
    {
        fprintf(stderr, "pushfront: tail is null\n");
        return false;
    }

    ln->next = dq->head;
    dq->head->prev = ln;
    dq->head = ln;

    return true;
}

bool deque_popback(Deque *dq)
{
    ListNode *temp;
    
    if (dq == NULL)
    {
        fprintf(stderr, "popBack: dq is null\n");
        return false;
    }
    if (dq->head == NULL)
    {
        fprintf(stderr, "popBack: dq->head is null\n");
        return false;
    }
    if (dq->tail == NULL)
    {
        fprintf(stderr, "popBack: dq->tail is null\n");
        return false;
    }


    if (dq->head == dq->tail)
    {
        free(dq->tail);
        dq->head = NULL;
        dq->tail = NULL;
        return true;
    }

    temp = dq->tail;
    dq->tail = dq->tail->prev;
    dq->tail->next = NULL; 
    free(temp);

    return true;
}

void deque_back(Deque *dq, Position *pos)
{
    pos->x = dq->tail->pos.x;
    pos->y = dq->tail->pos.y;
}

void deque_front(Deque *dq, Position *pos)
{
    pos->x = dq->head->pos.x;
    pos->y = dq->head->pos.y;
}

void printlist(Deque *dq)
{
    ListNode *curr = dq->head;
    printf("h");
    while (curr != NULL)
    {
        printf(" - (%d %d)", curr->pos.x, curr->pos.y);
        curr = curr->next;
    }
    printf("\n");
}
