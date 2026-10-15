#include "route_sprite_actor.h"
#define RouteSpritePool_AllocateSlot sub_0806DF60
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
            Memory_Clear(slot, 0x178);
            slot->index = i;
            return slot;
        }
    }
    return 0;
}
