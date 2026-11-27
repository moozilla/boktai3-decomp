#include "npc/story_character/state_actor.h"

void StoryCharacter_ProcessDirectionMessages(struct StateActor *);
void Actor_UpdateMovementAnimationAndEffects(u32, struct StateActor *);

s32 StoryCharacter_OnUpdateDisplay(u32 a, struct StateActor *b)
{
    StoryCharacter_ProcessDirectionMessages(b);
    Actor_UpdateMovementAnimationAndEffects(a, b);
    b->nextState = 0;
    return 0;
}
