#include "global.h"
#include "player/player.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
s32 SceneActorTarget_FindNearestEligiblePlayer(void *);
s32 Callback_InvokeActor(void *, void *);

u8 SceneActorTarget_TrySelectNearestParticipant(struct ChaseActor *actor)
{
    s32 player = SceneActorTarget_FindNearestEligiblePlayer(&actor->base.body.position);
    u8 found;
    if (player >= 0) {
        actor->base.tracking.target = (void *)&gPlayers[player]->positionalEffectGroup;
        found = 1;
    } else {
        Callback_InvokeActor(actor, actor->group.fallback);
        found = 0;
    }
    return found;
}
