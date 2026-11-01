#include "global.h"

void StateActor_ProcessDirectionMessages(u8 *);
void Actor_UpdateMovementAnimationAndEffects(u32, u8 *);

s32 StateActor_OnUpdateDisplay(u32 a, u8 *b)
{
    s32 z;
    StateActor_ProcessDirectionMessages(b);
    Actor_UpdateMovementAnimationAndEffects(a, b);
    z = 0;
    b[5] = z;
    return z;
}
