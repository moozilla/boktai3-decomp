#include "scene_actor_control_begin.h"
#include "scene_actor_terrain_reaction.h"
#include "scene_actor_contact_response.h"
#include "global.h"
#include "scene_actor_control_phase.h"
u8 sub_0812F3CC(struct ChaseActor *);
// confidence=low: selects a route return phase from motion and route flags.

void SceneActorRoute_UpdateReturnEntry(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u8 result;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        SceneActorTerrain_InstallControl(actor, actor->base.onEntry, 2);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    {
        u8 *finished = &actor->base.stepDone;
        if (*finished) {
            u16 flags = SceneActorContact_MotionFlags(actor, 0x10);
            if (flags) {
                SceneActorTerrain_EnterState(actor, 0, 0, 8);
                SceneActorControl_Begin(actor, group->routeChoiceControl, 0x13, result);
            } else {
                u32 bits = actor->base.behaviorFlags & 0x10;
                if (bits)
                    SceneActorTerrain_EnterState(actor, 0, 0, 7);
                else
                    SceneActorTerrain_EnterState(actor, 0, 0, 2);
                SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
            }
        } else
            actor->base.counter2AC++;
    }
}
