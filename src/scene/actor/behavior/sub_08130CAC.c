#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/collision/scene_actor_contact_response.h"
#include "global.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "scene/actor/scene_actor_control_motion.h"
u8 SceneActorReaction_BeginContactResponse(struct ChaseActor *);
s32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, u32);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, u32);
// confidence=low: chooses idle or turn behavior after advancing a route descriptor.

static inline void State(struct ChaseActor *a, u32 state, u32 phase, u32 behavior, u32 zero)
{
    a->base.state2A8 = state;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}
void SceneActorRoute_UpdateAdvancePhase(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 speed, event;
    void **routeField;
    u16 *scale;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((u8)SceneActorReaction_BeginContactResponse(actor))
        return;
    SceneActorAnimation_SelectDirectionalEntry(actor, 1);
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    if (SceneActorContact_MotionFlags(actor, 0xA02F)) {
        u16 *p = &actor->base.field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *p = &actor->base.field2C2;
        u16 value = *p;
        scale = p;
        if (!value)
            *p = 64;
    }
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);

    routeField = &group->route;
    event = NavMesh_ConsumeProbeEvent(*routeField);
    if (!event) {
        void *descriptor = *routeField;
        if (SceneActorBehavior_AccelerateTowardDescriptor(actor, descriptor, 32, 32, speed)) {
            State(actor, event, event, event, event);
            return;
        }
    } else {
        void *descriptor = *routeField;
        if (SceneActorBehavior_RequestDescriptorTurn(actor, descriptor, 32, 32, speed))
            SceneActorTerrain_EnterState(actor, 0, 0, 6);
    }
    actor->base.counter2AC++;
}
