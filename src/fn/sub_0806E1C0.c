// MATCHING_CONCESSION: register-pinning
#include "route_sprite_actor.h"
#define RouteSpritePool_Destroy sub_0806E1C0
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
                sub_08013B74(slot->shadow);
        }
        slot++;
        i++;
    }
    {
        register u32 zero asm("r1") = 0;
        gUnk_02000198 = (void *)zero;
    }
    return 0;
}
