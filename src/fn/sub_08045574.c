// MATCHING_CONCESSION: register-pinning
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
    s32 index = 0;
    do {
        u32 bit = 1;
        register struct MotionPool *current asm("r3");
        s32 bitIndex;
        bitIndex = index;
        bit <<= bitIndex;
        current = pool;
        asm("" : "+r"(current));
        if (current->activeMask & bit) {
            struct ContactSwarmSprite *sprite;
            u32 spriteOffset;
            struct ContactCollider *collider;
            u32 colliderOffset;
            struct MotionPoolParams *params;
            struct ContactPosition *velocity;
            struct ContactPosition *position;
            spriteOffset = (u8 *)&slot->sprite - (u8 *)slot;
            sprite = (void *)((u8 *)slot + spriteOffset);
            colliderOffset = (u8 *)&slot->collider - (u8 *)slot;
            collider = (void *)((u8 *)slot + colliderOffset);
            params = &slot->params;
            if (sub_08249240(slot, (void *)index, (void *)params->callback) == 1) {
                MotionPool_HideSlot(pool, slot, index);
                goto next;
            }
            {
                u32 moving = slot->flags;
                register u32 one asm("r2");
                one = 1;
                moving &= one;
                velocity = &slot->velocity;
                if (moving) {
                    struct ContactPosition *output = velocity;
                    s32 direction = params->direction;
                    s32 speed = params->speed;
                    s32 xVelocity;
                    xVelocity = scaleMotion(gUnk_086149C4[(direction + 64) & 255] * speed);
                    output->x = xVelocity;
                    output->y = 0;
                    output->z = scaleMotion(gUnk_086149C4[direction & 255] * speed);
                    asm("" : : "r"(xVelocity));
                }
            }
            position = &sprite->position;
            {
                u32 dx = (u16)slot->velocity.x;
                register struct ContactSwarmSprite *view asm("r3") = sprite;
                register u32 x asm("r3");
                struct ContactSwarmSprite *store;
                asm("" : "+r"(view));
                x = (u16)view->position.x;
                dx += x;
                store = sprite;
                store->position.x = dx;
            }
            {
                u32 dy = (u16)velocity->y;
                register u32 y asm("r3") = (u16)position->y;
                dy += y;
                position->y = dy;
            }
            position->z += velocity->z;
            {
                u32 flag = 2;
                u32 enabled;
                enabled = slot->flags & flag;
                if (enabled) {
                    Collider_SetPosition(collider, position, 0);
                    Collider_Refresh(collider);
                }
            }
            slot->frame++;
        }
    next:
        index++;
        {
            register u32 stride asm("r3") = sizeof(*slot);
            asm("" : "+r"(stride));
            slot = (void *)((u8 *)slot + stride);
        }
        {
            s32 check = index;
            if (check > 15)
                break;
        }
    } while (1);
    return 0;
}
