#include "route_sprite_actor.h"
static inline u8 Occupied(u32 *mask, s32 index, u32 one)
{
    if (*mask & (one << index))
        return TRUE;
    return FALSE;
}
void RouteSpritePool_RequestFinishAll(void)
{
    struct RouteSpritePool *pool = gRouteSpritePool;
    struct RouteSpriteActor *slot;
    s32 i;
    u32 one;
    u32 stride;
    if (!pool) {
        pool = RouteSpritePool_Create();
        if (!pool)
            return;
    }
    i = 0;
    one = 1;
    slot = pool->slots;
    stride = 0x178;
    for (; i < 4;) {
        if (Occupied(&pool->occupiedMask, i, one)) {
            if (slot->state != 5) {
                slot->state = 5;
                slot->entered = one;
            }
        }
        slot = (void *)((u8 *)slot + stride);
        i++;
    }
}
