#include "global.h"
// confidence=low: follows the route probe, then selects a new route point outside link mode.

#include "scene_actor_control_phase.h"
#include "scene_actor_chase_shared.h"
#include "engine.h"
#include "ram.h"
u8 SceneActor_IsFacingNearbyTarget(struct SceneActorRegistryResetView *);
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorPhase_SelectLinkedAnimation(void);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

static inline void SetState(struct SceneActorRegistryResetView *actor)
{
    u32 behavior = 6;
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void BeginRouteSelection(struct SceneActorRegistryResetView *actor,
                                       struct SceneActorChaseGroup *nav)
{
    void *control;
    u32 kind;
    u32 behavior = 8;
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
    /* The route callback is installed after the reset fields. */
    control = nav->routeChoiceControl;
    kind = 0x13;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
void SceneActorMovement_ResolveRoutePosition(void *);
static inline u8 InLinkMode(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void SceneActorControl_UpdateRouteApproach(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *nav = &actor->group;
    s32 speed;
    u32 event;
    u16 *scale;
    if (actor->field2B1)
        actor->field2B1 = 0;
    speed = Callback_InvokeActor(actor, actor->animationQuery.opaque);
    {
        u32 mask = 0xA02F;
        u16 *flagAddress = &actor->flags2D4;
        if (*flagAddress & mask) {
            u16 *p = &actor->field2C2;
            *p = 0x40;
            scale = p;
        } else {
            u16 *q = &actor->field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0)
                *scale = 0x40;
        }
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale(
            ((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
    event = NavMesh_ConsumeProbeEvent(nav->route);
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, nav->route, 0x20, 0x20, speed)) {
            SetState(actor);
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, nav->route, 0x20, 0x20,
                                                             speed)) {
        BeginRouteSelection(actor, nav);
        if (!InLinkMode(gEngineFlags, 0x4800)) {
            SceneActorChase_ChooseRoute(actor);
            SceneActorMovement_ResolveRoutePosition(actor);
        }
        return;
    }
    actor->counter2AC++;
}
