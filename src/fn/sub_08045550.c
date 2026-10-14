#include "pooled_motion.h"
void MotionPool_HideSlot(struct MotionPool *pool, struct MotionPoolSlot *slot, s32 index)
{
    slot->sprite.flags |= 1;
    Sprite_Remove(&slot->sprite);
    pool->activeMask &= ~(1 << index);
}
