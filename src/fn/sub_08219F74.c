#include "global.h"
#include "engine_globals.h"

struct Task {
    struct Task *prev, *next;
    u8 pad[0xC];
    u8 f14;
};
struct List {
    struct Task *head;
    u32 pad;
};

void Task_LinkToPriorityList(struct Task *t)
{
    struct List *l = &gTaskLists[t->f14];
    struct Task *h = l->head;
    if (h != 0)
        h->prev = t;
    t->next = h;
    l->head = t;
}
