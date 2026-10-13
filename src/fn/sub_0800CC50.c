#include "falling_objects.h"

u32 sub_0800CC50(struct FallingObject *s)
{
    Sprite_Remove(&s->sprite);
    if (gUnk_0200046C != 0)
        sub_0800CC08(gUnk_0200046C, s);
    sub_0800CC34(s);
    if (s->collider != 0) {
        Collider_Remove(s->collider);
        Heap_Free(s->collider);
    }
    if (s->attack != 0)
        Heap_Free(s->attack);
    Heap_Free(s);
}
