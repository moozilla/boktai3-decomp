#include "route_sprite_actor.h"
/* RouteSpritePool_IsFull: Ensures the pool exists and returns true if creation fails or all four slots are occupied */
#define RouteSpritePool_IsFull sub_0806E308
#define RouteSpritePool_IsFull sub_0806E308
static inline u8 Occupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return TRUE;
    return FALSE;
}
u8 RouteSpritePool_IsFull(void)
{
    struct RouteSpritePool *pool = gRouteSpritePool;
    s32 i;
    if (!pool) {
        pool = RouteSpritePool_Create();
        if (!pool)
            return TRUE;
    }
    for (i = 0; i < 4; i++) {
        if (!Occupied(&pool->occupiedMask, i))
            return FALSE;
    }
    return TRUE;
}
