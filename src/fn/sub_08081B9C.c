#include "scene_actor_terrain_reaction.h"
#include "scene_actor_reaction.h"

void SceneActorReaction_ResumeAfterAnimation(struct ChaseActor *actor, s32 unusedFrame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 7);
        SceneActorTerrain_ClearCollisionTimers((void *)actor);
        {
            u16 *flags = &actor->base.flags2D4;
            s32 mask = ~2;
            *flags &= mask;
        }
    }
    if (actor->base.restart) {
        actor->base.fieldAA = 0;
        actor->base.field2B0 = 1;
        {
            u32 mask = 16;
            actor->base.flags2CC |= mask;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorTerrain_EnterState(actor, 3, 1, 0);
        {
            void *control = group->field5E4;
            SceneActorTerrain_InstallControl(actor, control, 15);
        }
    }
}
