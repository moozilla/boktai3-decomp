#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_reaction.h"
#include "engine/word_flags.h"

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
            WordFlags_Set(16, &actor->base.behaviorFlags);
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorTerrain_EnterState(actor, 3, 1, 0);
        {
            void *control = group->routeResumeControl;
            SceneActorTerrain_InstallControl(actor, control, 15);
        }
    }
}
