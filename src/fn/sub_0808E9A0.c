#include "scene_actor_chase.h"
#include "scene_actor_turn.h"
void Slime_TickWaitTimer(void);
void Slime_PlayWaitAnimation(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorTurn_SelectProfileAnimation((void *)actor, 15, 1, 0);
    if (actor->field2B3)
        Chase_SetCtl(actor, Slime_TickWaitTimer, 0x24);
}
