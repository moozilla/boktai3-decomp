#include "global.h"
void ActorController_UpdateTargetMovement(s32, s32, s32);
void ActorController_UpdateModeTwoMovement(s32 a)
{
    ActorController_UpdateTargetMovement(a, 2, 0);
}
