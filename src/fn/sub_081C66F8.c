#include "global.h"

void Actor_DispatchPrimaryState(u32, u32);

void StateActor_DispatchCommand(u32 a, u32 b)
{
    Actor_DispatchPrimaryState(a, b);
}
