#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reset.h"
// confidence=low: restores the common Skeleton route and reaction state after snapshot recovery
extern const u8 gUnk_0824E250[];
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorAnimation_SelectDirectionalEntry(void *, u32);
u8 SceneActorRoute_IsOutsidePointBounds(void *, void *, u32, u32);
struct SkeletonResetContext {
    u8 unknown0[0x6D0];
    u8 timer;
    u8 unknown6D1[0xB];
    u32 savedFlags;
    u8 unknown6E0[0x24];
    u16 counter;
};
void Skeleton_ResetRouteReaction(struct SceneActorRegistryResetView *actor)
{
    struct SkeletonResetContext *context = actor->context;
    u8 *ready;
    context->timer = 0;
    context->counter = 0;
    context->savedFlags = actor->saved.privateFlags;
    Memory_Clear(actor->state5D4, 8);
    SceneActorReaction_ResetResponseState(&actor->word460, gUnk_0824E250, 0x120);
    actor->field2C2 = 64;
    actor->group.flags414 = 0;
    actor->group.field418 = 0;
    {
        u16 *motionFlags = &actor->flags2D4;
        {
            u32 mask = ~0x380;
            *motionFlags = mask & *motionFlags;
        }
        {
            u8 *flags = (u8 *)&actor->tracking.target;
            s32 mask = ~0x18;
            flags[15] = mask & flags[15];
        }
        {
            register struct SceneActorRegistryControl *control asm("r3") = &actor->group.control;
            u32 mask = 0x200000;
            if (actor->flags2CC & mask) {
                {
                    u32 *flags = &actor->flags2D0;
                    u32 mask = ~0x1000000;
                    *flags = mask & *flags;
                }
                {
                    u32 mask = ~0x200;
                    *motionFlags = mask & *motionFlags;
                }
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
        }
        {
            u32 mask = ~0x400;
            *motionFlags = mask & *motionFlags;
        }
    }
    actor->field5DA = 255;
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (SceneActorRoute_IsOutsidePointBounds(&actor->route, &actor->body.position, 32, 32)) {
        u32 behavior = 2;
        register u32 offset asm("r2") = 0x2A8;
        *((u8 *)actor + offset) = 0;
        offset++;
        *((u8 *)actor + offset) = 0;
        offset++;
        *((u8 *)actor + offset) = behavior;
        actor->counter2AC = 0;
        offset += 7;
        ready = (u8 *)actor + offset;
    } else {
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        ready = &actor->field2B1;
    }
    *ready = 1;
    Anim_Step((u8 *)actor->drawable + 32, actor->drawable);
}
