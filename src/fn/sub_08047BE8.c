#include "temporary_animation_pool.h"
s32 TemporaryAnimationPool_Destroy(struct TemporaryAnimationPool *pool)
{
    struct TemporaryAnimationSlot *slot = pool->slots;
    s32 remaining = 15;
    do {
        if (slot->animation.registered)
            Anim_Remove(&slot->animation);
        --remaining;
        ++slot;
    } while (remaining >= 0);
    gTemporaryAnimationPool = NULL;
    return 0;
}
