#include "global.h"

void TaggedSceneNpc_DispatchCutsceneCommands(u8 *);
void Actor_UpdateMovementAnimationAndEffects(u32, u8 *);

s32 TaggedSceneNpc_OnFrameUpdate(u32 a, u8 *b)
{
    s32 z;
    TaggedSceneNpc_DispatchCutsceneCommands(b);
    Actor_UpdateMovementAnimationAndEffects(a, b);
    z = 0;
    b[5] = z;
    return z;
}
