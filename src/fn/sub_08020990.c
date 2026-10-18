#include "global.h"

struct N {
    u8 f[0x2c];
    struct N *prev;
    struct N *next;
};
struct S {
    u8 f[0x74];
    struct N *head;
};
u32 SceneMessages_LinkReceiver(struct S *p, struct N *n)
{
    if (p->head)
        p->head->prev = n;
    n->prev = 0;
    n->next = p->head;
    p->head = n;
    return 0;
}
