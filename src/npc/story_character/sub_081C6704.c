#include "global.h"

void StateActor_DispatchCommand(u32, u32);

s32 StateActor_OnCommand(u32 a, u32 b)
{
    StateActor_DispatchCommand(a, b);
    return 0;
}
