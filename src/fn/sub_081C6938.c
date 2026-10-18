#include "global.h"

void Actor_UpdateVisibilityAndShadow(u32, u32);

s32 StateActor_OnRender(u32 a, u32 b)
{
    Actor_UpdateVisibilityAndShadow(a, b);
    return 0;
}
