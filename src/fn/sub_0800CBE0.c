#include "falling_objects.h"

u32 sub_0800CBE0(struct FallingObjectManager *head, struct FallingObject *n)
{
    struct FallingObject **slot;
    struct FallingObject *t;
    n->previous = 0;
    slot = &n->next;
    *slot = head->head;
    t = *slot;
    if (t) {
        t->previous = n;
    }
    head->head = n;
    return 0;
}
