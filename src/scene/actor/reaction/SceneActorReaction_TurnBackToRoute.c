#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/collision/scene_actor_delayed_response.h"
#include "scene/actor/scene_actor_reaction.h"
void SceneActorReaction_PlayAnimationBUntilComplete(void *);
void SceneActorReaction_TurnBackToRoute(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 mask;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->base.body.direction = (((actor->base.body.direction + 128) & 255) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    }
    mask = 0x10;
    if (actor->base.statusFlags & mask) {
        struct SceneActorDelayedResponse *command;
        void *callback;
        u8 one;
        actor->base.behaviorFlags |= 0x10;
        command = (void *)&group->saved;
        callback = (void *)SceneActorReaction_PlayAnimationBUntilComplete;
        one = 1;
        command->delay = 30;
        command->control = callback;
        command->state = 0;
        command->phase = one;
        command->behavior = 1;
        SceneActorTerrain_InstallControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorTerrain_EnterState(actor, 3, 1, 0);
        actor->base.field2C2 = 64;
    }
}
