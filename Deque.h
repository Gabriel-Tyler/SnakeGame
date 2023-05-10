#ifndef DEQUE_H
#define DEQUE_H

#include <stdbool.h>

struct Position {
    int x, y;
};

struct ListNode {
    struct Position pos;
    struct ListNode *prev, *next;
};

struct Deque {
    struct ListNode *head, *tail;
};

typedef struct ListNode ListNode;
typedef struct Deque Deque;
typedef struct Position Position;

void deque_init(Deque *);
void deque_clear(Deque *);
bool deque_pushfront(Deque *, int, int);
bool deque_popback(Deque *);
ListNode *deque_newnode(int, int, ListNode *, ListNode *);
void deque_back(Deque *, Position *);
void deque_front(Deque *, Position *);
void printlist(Deque *);

#endif /* DEQUE_H */