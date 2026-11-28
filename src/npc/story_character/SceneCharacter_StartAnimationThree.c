#include "npc/manager/scene_actor_lifecycle.h"
void SceneCharacter_StartAnimationThree(void *context, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Actor_PlayAnimationEntry(actor, 3, 1, 0, 4);
        actor->animationPhase = 0;
        actor->stateFrame = 0;
        actor->finished = 1;
    }
}
