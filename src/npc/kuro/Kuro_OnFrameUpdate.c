#include "global.h"

void Kuro_DispatchCutsceneCommands(u8 *);
void Actor_UpdateMovementAnimationAndEffects(u32, u8 *);

s32 Kuro_OnFrameUpdate(u32 a, u8 *b)
{
    s32 z;
    Kuro_DispatchCutsceneCommands(b);
    Actor_UpdateMovementAnimationAndEffects(a, b);
    z = 0;
    b[5] = z;
    return z;
}
