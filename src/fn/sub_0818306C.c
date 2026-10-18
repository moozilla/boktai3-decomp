#include "global.h"
void ActorController_UpdateResetMotionTransition(s32, s32);
void ActorController_UpdateResetMotionSpeed120(s32 a)
{
    ActorController_UpdateResetMotionTransition(a, 1);
}
