#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/collision/scene_actor_restoration.h"
/* Clears restart while the delayed reaction is in progress. */
#define SceneActorReaction_RestoreAndClearRestart sub_0807FE80
void SceneActorReaction_RestoreAndClearRestart(struct ChaseActor *actor, s32 frame)
{
    actor->base.restart = 0;
    if (actor->group.restoration.threshold <= frame) {
        SceneActorTerrain_InstallControl(actor, actor->group.restoration.control, 14);
        SceneActorTerrain_EnterState(actor, actor->group.restoration.state,
                                     actor->group.restoration.phase,
                                     actor->group.restoration.behavior);
        SceneActorReaction_ResetResponseState(&actor->group.saved, gUnk_0824E128, 0x8B);
    }
}
