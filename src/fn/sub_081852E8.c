#include "global.h"

void DirectionalActor_TryBeginState(u8 *p, s32 q);

void ActorController_RequestIdleAction(u8 *p)
{
    DirectionalActor_TryBeginState(p, 0);
}
