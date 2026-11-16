#include "scene_actor_terrain_reaction.h"
#include "scene_actor_reaction.h"

void SceneActorReaction_ResumeAfterAnimation(struct ChaseActor *actor, s32 unusedFrame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 7);
        SceneActorTerrain_ClearCollisionTimers((void *)actor);
        {
            u16 *flags = &actor->base.motionFlags;
            s32 mask = ~2;
            *flags &= mask;
        }
    }
    if (actor->base.restart) {
        actor->base.attachmentTimer = 0;
        actor->base.stepDone = 1;
        {
            u32 mask = 16;
            actor->base.behaviorFlags |= mask;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorTerrain_EnterState(actor, 3, 1, 0);
        {
            void *control = group->routeResumeControl;
            SceneActorTerrain_InstallControl(actor, control, 15);
        }
    }
}
