#include "global.h"
#include "player/player.h"
#include "monsters/chase_actor.h"

void SceneActor_UpdateActiveCallbacks(struct ChaseActor *);

u32 Centipede_InitPlayerReference(struct ChaseActor *actor)
{
    actor->base.tracking.target = (void *)&gPlayers[0]->positionalEffectGroup;
    SceneActor_UpdateActiveCallbacks(actor);
    return 0;
}
