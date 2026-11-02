#include "transient_sprite_pool.h"

struct TemporarySpriteSlot *TransientSpritePool_FindFreeSlot(struct TemporarySpritePool *pool)
{
    struct TemporarySpriteSlot *slot = pool->slots;
    s32 i = 0;
    while (i <= 15) {
        if (!slot->active)
            return slot;
        i++;
        slot++;
    }
    return 0;
}
