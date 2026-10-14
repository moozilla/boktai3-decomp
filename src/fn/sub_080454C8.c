// MATCHING_CONCESSION: register-pinning
#include "pooled_motion.h"
#define MotionPool_InitializeSlot          sub_080454C8
#define MotionPool_InitializeParams        sub_080458C8
#define MotionPool_FinishOnResourceContact sub_080454C0
void MotionPool_InitializeParams(struct MotionPoolParams *);
void MotionPool_FinishOnResourceContact(void *, void *, struct MotionPoolSlot *);
void sub_082144E4(void *, void *, u32);
void sub_0821FEB4(void *, u32, u32, u32, u32, void *, void *);
void sub_0821FF58(void *, u32, u32, u32, u32, u32);
void MotionPool_InitializeSlot(struct MotionPool *pool, struct MotionPoolSlot *slot, s32 index)
{
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
        asm("" : "+r"(source));
        CpuSet(source, velocity, 0x05000002);
    }
    graphics = &slot->graphics;
    AnimData_Load(graphics, params->graphicResource);
    sub_082144E4(&slot->sprite, graphics, 1);
    collider = &slot->collider;
    sub_0821FEB4(collider, 0, 0x2001, 0, 0x10, &params->bounds, &params->offset);
    Collider_SetCallback(collider, MotionPool_FinishOnResourceContact, slot);
    sub_0821FF58(collider, params->resourceId, params->cost, params->contactMode,
                 params->contactFlags, params->option);
}
