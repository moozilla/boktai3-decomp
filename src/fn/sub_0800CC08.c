#include "falling_objects.h"

u32 sub_0800CC08(struct FallingObjectManager *head, struct FallingObject *n)
{
    struct FallingObject *prev = n->previous;
    struct FallingObject *next = n->next;
    if (prev) {
        prev->next = next;
    } else {
        head->head = next;
    }
    if (next) {
        next->previous = prev;
    }
    return 0;
}
