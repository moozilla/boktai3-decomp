#include "effects/sprites/sprite_effect_controller.h"
static inline u8 SpriteEffectController_IsUsed(struct SpriteEffectControllerPool *pool, s32 index)
{
    if (pool->used & (1 << index))
        return TRUE;
    return FALSE;
}
static inline void SpriteEffectController_QueueCollider(struct SpriteEffectController *controller)
{
    controller->colliderDelay = 3;
    controller->flags |= 0x100;
}
void SpriteEffectController_QueueColliders(struct SpriteEffectControllerPool *pool)
{
    struct SpriteEffectController *slots = pool->slots;
    s32 scheduled = 0;
    s32 i;
    for (i = 0; i <= 23; i++) {
        if (SpriteEffectController_IsUsed(pool, i) && (slots->flags & 4)) {
            if (slots[i].colliderDelay)
                slots[i].colliderDelay--;
            else if (scheduled <= 5) {
                SpriteEffectController_QueueCollider(&slots[i]);
                scheduled++;
            }
        }
    }
}
