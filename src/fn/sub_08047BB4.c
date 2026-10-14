#include "temporary_animation_pool.h"
#define TemporaryAnimationPool_Update sub_08047BB4
s32 TemporaryAnimationPool_Update(struct TemporaryAnimationPool *pool)
{
    struct TemporaryAnimationSlot *slot = pool->slots;
    s32 remaining = 15;
    do {
        if (slot->active) {
            --slot->lifetime;
            if (slot->lifetime <= 0) {
                Anim_Remove(&slot->animation);
                slot->active = 0;
            }
        }
        --remaining;
        ++slot;
    } while (remaining >= 0);
    return 0;
}
