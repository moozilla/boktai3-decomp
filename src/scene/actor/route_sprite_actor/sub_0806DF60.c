#include "scene/motion/route_sprite_actor.h"
static inline u8 Occupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return TRUE;
    return FALSE;
}
struct RouteSpriteActor *RouteSpritePool_AllocateSlot(struct RouteSpritePool *pool)
{
    s32 i;
    struct RouteSpriteActor *slot;
    for (i = 0; i < 4; i++) {
        if (!Occupied(&pool->occupiedMask, i)) {
            slot = &pool->slots[i];
            pool->occupiedMask |= 1 << i;
            Memory_Clear(slot, sizeof(*slot));
            slot->index = i;
            return slot;
        }
    }
    return 0;
}
