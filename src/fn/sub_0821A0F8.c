#include "global.h"

struct Task {
    u8 pad[4];
    struct Task *next;
    u8 pad8[0xE];
    u8 f16;
};
struct List {
    struct Task *head;
    u32 pad;
};
extern struct List gTaskLists[];
s32 Task_Kill(struct Task *);

void TaskSystem_KillBelowPersistenceLevel(s32 prio)
{
    struct List *l = gTaskLists;
    s32 i = 0;
    do {
        struct Task *t = l->head;
        s32 ni = i + 1;
        struct List *nl = l + 1;
        for (; t != 0; t = t->next) {
            if (t->f16 < prio)
                Task_Kill(t);
        }
        i = ni;
        l = nl;
    } while (i <= 13);
}
