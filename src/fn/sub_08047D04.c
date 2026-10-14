#include "temporary_animation_pool.h"
struct TemporaryAnimationSlot *
TemporaryAnimationPool_FindInactive(struct TemporaryAnimationPool *pool)
{
    struct TemporaryAnimationSlot *slot = pool->slots;
    s32 index = 0;
    while (index <= 15) {
        if (!slot->active)
            return slot;
        ++index;
        ++slot;
    }
    return NULL;
}
