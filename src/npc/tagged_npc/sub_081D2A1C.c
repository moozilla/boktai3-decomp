#include "global.h"

void Actor_UpdateVisibilityAndShadow(u32, u32);

s32 TaggedSceneNpc_OnVisibilityUpdate(u32 a, u32 b)
{
    Actor_UpdateVisibilityAndShadow(a, b);
    return 0;
}
