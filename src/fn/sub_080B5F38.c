#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_chase.h"
// confidence=low: restores the common Skeleton route and reaction state after snapshot recovery
extern const u8 gUnk_0824E250[];
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void SceneActorMovement_ResolveRoutePosition(void *);
u8 SceneActorRoute_IsOutsidePointBounds(void *, void *, u32, u32);
static inline void clearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
}
static inline u32 testFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline void clearBits16(u16 *field, u32 bits)
{
    u32 mask = ~bits;
    mask &= *field;
    *field = mask;
}
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
    context->timer = 0;
    context->counter = 0;
    context->savedFlags = actor->saved.privateFlags;
    Memory_Clear(actor->group.destination, 8);
    SceneActorReaction_ResetResponseState(&actor->group.saved.control, gUnk_0824E250, 0x120);
    actor->field2C2 = 64;
    actor->group.common.gate.flags = 0;
    actor->group.common.gate.counter = 0;
    {
        struct SceneActorRegistryControl *control;
        clearBits16(&actor->flags2D4, 0x380);
        {
            u8 *motion = (u8 *)&actor->tracking.target;
            s32 mask = ~0x18;
            mask &= motion[15];
            motion[15] = mask;
        }
        control = &actor->group.common.control;
        if (testFlags(actor, 0x200000)) {
            clearRouteFlag(&actor->flags2D0);
            clearBits16(&actor->flags2D4, 0x200);
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        clearBits16(&actor->flags2D4, 0x400);
    }
    actor->group.turn.turnsRemaining = 255;
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (SceneActorRoute_IsOutsidePointBounds(&actor->group.homeRoute, &actor->body.position, 32,
                                             32))
        Chase_SetBehavior(actor, 2);
    else
        Chase_SetBehavior(actor, 0);
    Anim_Step((u8 *)actor->drawable + 32, actor->drawable);
}
