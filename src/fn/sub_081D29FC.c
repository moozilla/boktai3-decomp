#include "global.h"

void sub_081D2610(u8 *);
void Actor_UpdateMovementAnimationAndEffects(u32, u8 *);

s32 TaggedSceneNpc_OnFrameUpdate(u32 a, u8 *b)
{
    s32 z;
    sub_081D2610(b);
    Actor_UpdateMovementAnimationAndEffects(a, b);
    z = 0;
    b[5] = z;
    return z;
}
