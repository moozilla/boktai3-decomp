#include "global.h"

void StoryCharacter_ProcessDirectionMessages(u8 *);
void Actor_UpdateMovementAnimationAndEffects(u32, u8 *);

s32 StoryCharacter_OnUpdateDisplay(u32 a, u8 *b)
{
    s32 z;
    StoryCharacter_ProcessDirectionMessages(b);
    Actor_UpdateMovementAnimationAndEffects(a, b);
    z = 0;
    b[5] = z;
    return z;
}
