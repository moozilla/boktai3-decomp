#include "global.h"
// confidence=low: follows the route probe, then selects a new route point outside link mode.

#include "scene_actor_control_phase.h"
u8 SceneActor_IsFacingNearbyTarget(struct SceneActorRegistryResetView *);
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorPhase_SelectLinkedAnimation(void);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

void sub_080A4798(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
static inline void State(struct SceneActorRegistryResetView *a, u32 behavior)
{
    u32 off = 0x2A8;
    u32 phase;
    u32 zero;
    {
        u8 *q = &FIELD(a, off, u8);
        zero = 0;
        *q = zero;
    }
    phase = 0x2A9;
    FIELD(a, phase, u8) = zero;
    off += 2;
    FIELD(a, off, u8) = behavior;
    phase += 3;
    FIELD(a, phase, u32) = zero;
    {
        u32 b = 0x2B1;
        FIELD(a, b, u8) = 1;
    }
}
static inline void Control(struct SceneActorRegistryResetView *a, void *target)
{
    u32 off = 0x2B2;
    FIELD(a, off, u8) = 1;
    off -= 2;
    FIELD(a, off, u8) = 0;
    off += 5;
    FIELD(a, off, u8) = 1;
    a->target = target;
    a->field2C8 = 0;
    {
        u32 *p = &a->flags114;
        s32 m = ~1;
        *p &= m;
    }
    a->word104 = 0;
}
// confidence=low: updates probe navigation and switches between turn, approach, and idle behaviors.
void Spider_UpdateWanderGoal(void *);
struct SpiderProbeFlags {
    s16 bits : 16;
};
struct SpiderProbeContext {
    u8 pad[0x628];
    s16 timer;
    s16 delay;
    struct SpiderProbeFlags flags;
};
static inline u8 HasFlag(struct SpiderProbeContext *c, u32 mask)
{
    if (*(u16 *)&c->flags & mask)
        return 1;
    return 0;
}
static inline void ClearFlag(struct SpiderProbeFlags *p, u32 mask)
{
    p->bits &= mask;
}
static inline u32 Flags(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u32 GetFlags(struct SceneActorRegistryResetView *a, u32 mask, u32 **out)
{
    u32 *p = &a->flags2CC;
    u32 v = *p;
    v &= mask;
    *out = p;
    return v;
}
struct SpiderGroupView {
    u8 pad[0x1FC];
    void *mesh;
};
void Spider_UpdateProbeNavigation(struct SceneActorRegistryResetView *actor)
{
    struct SpiderProbeContext *ctx = actor->context;
    struct SpiderGroupView *group = (void *)&actor->group.common;
    s32 speed;
    u32 *flags;
    u16 *scale;
    u32 probe;
    if (actor->field2B1)
        actor->field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, FIELD(actor, 0x270, void *)))
        return;
    speed = Callback_InvokeActor(actor, FIELD(actor, 0x260, void *));
    {
        u32 mask = 0xA02F;
        if (actor->flags2D4 & mask) {
            u16 *p = &actor->field2C2;
            *p = 64;
            scale = p;
        } else {
            u16 *p = &actor->field2C2;
            u32 v = *p;
            scale = p;
            if (!v)
                *p = 64;
        }
    }
    if (!actor->drawableKind)
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
    if (!GetFlags(actor, 0x4000, &flags) && !HasFlag(actor->context, 4))
        Spider_UpdateWanderGoal(actor);
    probe = NavMesh_ConsumeProbeEvent(group->mesh);
    if (!probe) {
        void *descriptor = group->mesh;
        if (SceneActorBehavior_AccelerateTowardDescriptor(actor, descriptor, 32, 32, speed)) {
            State(actor, 5);
            {
                u32 mask = ~0x4000;
                *flags &= mask;
            }
            ClearFlag(&((struct SpiderProbeContext *)actor->context)->flags, ~2);
            return;
        }
    } else {
        void *descriptor = group->mesh;
        if (SceneActorBehavior_RequestDescriptorTurn(actor, descriptor, 32, 32, speed))
            State(actor, 6);
    }
    if (!Flags(flags, 0x4000) && ctx->timer > 0) {
        u32 mask = 0x2000;
        *flags |= mask;
    }
    actor->counter2AC++;
}
