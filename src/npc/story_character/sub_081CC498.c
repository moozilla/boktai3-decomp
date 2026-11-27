#include "npc/story_character/state_actor.h"
void SceneCharacter_StartAnimationThree(void *context, struct StateActor *actor)
{
    if (actor->enter) {
        actor->enter = 0;
        actor->done = 0;
        Actor_PlayAnimationEntry(actor, 3, 1, 0, 4);
        actor->phase = 0;
        actor->stateFrame = 0;
        actor->done = 1;
    }
}
