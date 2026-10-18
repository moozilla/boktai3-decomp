#include "global.h"

void ActorController_UpdateResetMotionTransition(u8 *p, s32 q);

void ActorController_UpdateResetMotion(u8 *p)
{
    ActorController_UpdateResetMotionTransition(p, 0);
}
