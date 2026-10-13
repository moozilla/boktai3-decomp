#include "global.h"
struct Object {
    u8 active;
    u8 pad1[0x5b];
    int id;
    u8 pad60[0x288];
    struct Object *next, *prev;
};
struct Manager {
    u8 pad[0x1c];
    struct Object *head;
};
void sub_0802D654(struct Manager *m, struct Object *p)
{
    if (m->head)
        m->head->next = p;
    p->next = 0;
    p->prev = m->head;
    m->head = p;
    p->active = 1;
}
