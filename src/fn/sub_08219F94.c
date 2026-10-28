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

void Task_UnlinkFromPriorityList(struct Task *t)
{
    struct List *l = &gTaskLists[t->f14];
    struct Task *prev = t->prev;
    struct Task *next = t->next;
    if (prev != 0)
        prev->next = next;
    else
        l->head = next;
    if (next != 0)
        next->prev = prev;
}
