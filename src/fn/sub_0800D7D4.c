#include "falling_objects.h"

u32 sub_0800D7D4(struct FallingObjectManager *s)
{
    struct FallingObject *n = s->head;
    while (n != 0) {
        struct FallingObject *next = n->next;
        sub_0800CC50(n);
        n = next;
    }
    return gUnk_0200046C = 0;
}
