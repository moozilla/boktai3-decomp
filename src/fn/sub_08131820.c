#include "scene_actor_terrain_reaction.h"
#include "global.h"
// confidence=low: follows the route probe, then selects a new route point outside link mode.

#include "scene_actor_control_phase.h"
#include "scene_actor_chase_shared.h"
#include "engine.h"
#include "ram.h"
u8 SceneActor_IsFacingNearbyTarget(struct ChaseActor *);
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(struct ChaseActor *);
void SceneActorPhase_SelectLinkedAnimation(void);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

static inline void BeginRouteSelection(struct ChaseActor *actor, struct SceneActorChaseGroup *nav)
{
    SceneActorTerrain_EnterState(actor, 0, 0, 8);
    SceneActorTerrain_InstallControl(actor, nav->routeChoiceControl, 0x13);
}
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorControl_UpdateRouteApproach(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *nav = &actor->group;
    s32 speed;
    u32 event;
    u16 *scale;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    {
        if (SceneActorBehavior_MotionBits(0xA02F, actor)) {
            u16 *p = &actor->base.field2C2;
            *p = 0x40;
            scale = p;
        } else {
            u16 *q = &actor->base.field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0)
                *scale = 0x40;
        }
    }
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence,
            *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
    event = NavMesh_ConsumeProbeEvent(nav->route);
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, nav->route, 0x20, 0x20, speed)) {
            SceneActorTerrain_EnterState(actor, 0, 0, 6);
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, nav->route, 0x20, 0x20,
                                                             speed)) {
        BeginRouteSelection(actor, nav);
        if (!SceneActorChase_Test(gEngineFlags, 0x4800)) {
            SceneActorChase_ChooseRoute(actor);
            SceneActorMovement_ResolveRoutePosition(actor);
        }
        return;
    }
    actor->base.counter2AC++;
}
