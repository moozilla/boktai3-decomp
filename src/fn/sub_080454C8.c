#include "pooled_motion.h"
void MotionPool_InitializeParams(struct MotionPoolParams *);
void MotionPool_FinishOnResourceContact(void *, void *, struct MotionPoolSlot *);
void Sprite_InitUnlinked(void *, void *, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
static inline void CpuSetSwapped(int mode, const u32 *src, struct ContactPosition *dst)
{
    CpuSet(src, dst, mode);
}
void MotionPool_InitializeSlot(struct MotionPool *pool, struct MotionPoolSlot *slot, s32 index)
{
    struct MotionPoolSlot *slotCopy;
    struct MotionPoolParams *params = &slot->params;
    struct ContactSwarmGraphics *graphics;
    struct ContactCollider *collider;
    u32 zero;
    MotionPool_InitializeParams(params);
    slot->frame = 0;
    {
        struct ContactPosition *velocity = &slot->velocity;
        const u32 *source;
        zero = 0;
        source = &zero;
        CpuSetSwapped(0x05000002, source, velocity);
    }
    graphics = &slot->graphics;
    AnimData_Load(graphics, params->graphicResource);
    Sprite_InitUnlinked(&slot->sprite, graphics, 1);
    slotCopy = slot;
    collider = &slotCopy->collider;
    Collider_Init(collider, 0, 0x2001, 0, 0x10, &params->bounds, &params->offset);
    Collider_SetCallback(collider, MotionPool_FinishOnResourceContact, slot);
    Collider_SetAttackProperties(collider, params->resourceId, params->cost, params->contactMode,
                                 params->contactFlags, params->option);
}
