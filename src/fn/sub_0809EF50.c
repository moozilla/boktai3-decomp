#include "scene_actor_chase.h"
#include "scene_actor_turn.h"
// confidence=low: plays profile entry two when the action begins.
void Crow_PlayEntry2(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorTurn_SelectProfileAnimation((void *)actor, 2, 0, 0);
}
