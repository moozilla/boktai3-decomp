#include "scene/actor/scene_actor_status.h"
#include "scene/actor/scene_actor_control_begin.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "scene/actor/scene_actor_control_motion.h"
#include "debug.h"
#include "scene/collision/scene_actor_contact_response.h"
u8 SceneActorReaction_BeginContactResponse(struct ChaseActor *);
s32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, u32);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, u32);
// confidence=low: selects control behavior after a route descriptor completes.

void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorRoute_UpdateRouteCompletion(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 speed, event;
    u16 *scale;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        DEBUG_PRINT();
    }
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
    {
        void **descriptor = &group->route;
        event = NavMesh_ConsumeProbeEvent(*descriptor);
        if (event) {
            if (SceneActorBehavior_RequestDescriptorTurn(actor, *descriptor, 32, 32, speed)) {
                SceneActorTerrain_EnterState(actor, 0, 0, 6);
                SceneActorTerrain_InstallControl(actor, actor->base.onStanding, 0);
            }
        } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, *descriptor, 32, 32,
                                                                 speed)) {
            struct ContactPosition **destination;
            group->common.control.timer = 0;
            destination = &group->destination;
            if ((*destination)->pad != 0) {
                struct SceneActorRegistryControl *control;
                SceneActorTerrain_EnterState(actor, 0, 0, 2);
                SceneActorMovement_ResolveRoutePosition(actor);
                control = &group->common.control;
                {
                    u32 mask = 0x200000;
                    if (actor->base.behaviorFlags & mask)
                        ActorStatus_ClearInteraction(actor, control);
                }
                {
                    u32 *flags = &actor->base.behaviorFlags;
                    u32 mask = ~0x90;
                    *flags &= mask;
                }
                actor->base.field2C2 = 64;
                Memory_Clear(*destination, 8);
            } else {
                SceneActorTerrain_EnterState(actor, 0, 0, 5);
            }
            SceneActorControl_Begin(actor, actor->base.onStanding, 0, 0);
            return;
        }
    }
    actor->base.counter2AC++;
}
