#include "falling_objects.h"

void sub_0800CBAC(struct FallingHit *a, u32 unused, struct FallingObject *b)
{
    struct FallingObject *c = b;
    if (c->type == 0) {
        u32 fl = a->flags;
        u32 d;
        d = a->damage;
        if (fl & 8) {
            d <<= 1;
            b->health -= d;
        }
        if (c->health < 0)
            c->health = 0;
    }
}
