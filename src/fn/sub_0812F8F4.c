#include "scene_actor_control_begin.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_terrain_reaction.h"
#include "global.h"

u8 sub_0812F3CC(struct ChaseActor *);
void SceneActorControl_UpdateEntryTransition(struct ChaseActor *actor)
{
    u8 result;
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorTerrain_InstallControl(actor, actor->base.onEntry, 2);
    }
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        result = sub_0812F3CC(actor);
        if (!result) {
            if (actor->base.stepDone) {
                SceneActorAnimation_SelectDirectionalEntry(actor, 0);
                SceneActorBehavior_WriteSelector(12, &actor->base.behavior2AA);
                actor->base.counter2AC = result;
                actor->base.field2B1 = 1;
                SceneActorControl_Begin(actor, group->gate.control, 26, result);
            } else {
                ++actor->base.counter2AC;
            }
        }
    }
}
