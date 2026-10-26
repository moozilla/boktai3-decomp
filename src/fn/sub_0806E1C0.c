#include "route_sprite_actor.h"
static inline u8 Occupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return TRUE;
    return FALSE;
}
s32 RouteSpritePool_Destroy(struct RouteSpritePool *pool)
{
    s32 i = 0;
    struct RouteSpriteActor *slot = pool->slots;
    for (; i < 4;) {
        if (Occupied(&pool->occupiedMask, i)) {
            if (slot->sprite.unknown4[0])
                Sprite_Remove(&slot->sprite);
            if (slot->shadowAbsent == 0)
                ScaledShadow_Destroy(slot->shadow);
        }
        slot++;
        i++;
    }
    {
        u32 zero = 0;
        gRouteSpritePool = (void *)zero;
    }
    return 0;
}
