#include "pooled_motion.h"
struct MotionPool *MotionPool_Create(void);
struct MotionPoolSlot *MotionPool_FindFreeSlot(struct MotionPool *, s32 *);
void *sub_0821A2C4(u32);
void sub_0821FF58(void *, u32, u32, u32, u32, u32);
void sub_08214374(void *, u32);
void *sub_08225984(u32);
static inline s32 scaleMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
s32 MotionPool_Spawn(const struct MotionPoolParams *params, const struct ContactPosition *position)
{
    struct MotionPool *pool = sub_0821A2C4(0x5B1B);
    struct MotionPoolSlot *slot;
    struct ContactPosition *velocity;
    struct ContactCollider *collider;
    struct ContactSwarmGraphics *graphics;
    struct ContactSwarmSprite *sprite;
    s32 index;
    s32 direction;
    s32 speed;
    if (!pool) {
        MotionPool_Create();
        return -1;
    }
    slot = MotionPool_FindFreeSlot(pool, &index);
    if (!slot)
        return -1;
    Memory_Copy(&slot->params, params, sizeof(*params));
    slot->frame = 0;
    velocity = &slot->velocity;
    direction = params->direction;
    speed = params->speed;
    velocity->x = scaleMotion(gUnk_086149C4[(direction + 64) & 255] * speed);
    velocity->y = 0;
    velocity->z = scaleMotion(gUnk_086149C4[direction & 255] * speed);
    collider = &slot->collider;
    sub_0821FF58(collider, params->resourceId, params->cost, params->contactMode,
                 params->contactFlags, params->option);
    collider->bounds = params->bounds;
    collider->offset = params->offset;
    Collider_SetPosition(collider, position, 0);
    slot->sprite.position = *position;
    graphics = &slot->graphics;
    AnimData_Load(graphics, params->graphicResource);
    sprite = &slot->sprite;
    sprite->frame = params->frame;
    if (params->tile >= 0)
        AnimData_SelectTiles(graphics, params->tile);
    sprite->flags &= ~1;
    sub_08214374(sprite, 0);
    if (params->trackingFrames) {
        void *target = sub_08225984(params->targetId);
        if (target)
            slot->target = (void *)((u8 *)target + 12);
    }
    pool->activeMask |= 1 << index;
    return 0;
}
