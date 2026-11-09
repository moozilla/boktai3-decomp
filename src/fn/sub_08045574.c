#include "pooled_motion.h"
extern const s16 gUnk_086149C4[];
void MotionPool_HideSlot(struct MotionPool *, struct MotionPoolSlot *, s32);
static inline s32 scaleMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
s32 MotionPool_Update(struct MotionPool *pool)
{
    struct MotionPoolSlot *slot = pool->slots;
    s32 index;
    for (index = 0; index < 16; index++, slot++) {
        if (pool->activeMask & (1 << index)) {
            struct ContactSwarmSprite *sprite = &slot->sprite;
            struct ContactCollider *collider = &slot->collider;
            struct MotionPoolParams *params = &slot->params;
            struct ContactPosition *velocity;
            struct ContactPosition *position;
            if (params->callback(slot, index) == 1) {
                MotionPool_HideSlot(pool, slot, index);
                continue;
            }
            {
                u32 moving = slot->flags & 1;
                velocity = &slot->velocity;
                if (moving) {
                    struct ContactPosition *output = velocity;
                    s32 direction = params->direction;
                    s32 speed = params->speed;
                    output->x = scaleMotion(gUnk_086149C4[(direction + 64) & 255] * speed);
                    output->y = 0;
                    output->z = scaleMotion(gUnk_086149C4[direction & 255] * speed);
                }
            }
            position = &sprite->position;
            sprite->position.x += slot->velocity.x;
            position->y += velocity->y;
            position->z += velocity->z;
            {
                u32 flag = 2;
                if (slot->flags & flag) {
                    Collider_SetPosition(collider, position, 0);
                    Collider_Refresh(collider);
                }
            }
            slot->frame++;
        }
    }
    return 0;
}
