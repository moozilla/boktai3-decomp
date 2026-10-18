#include "global.h"
void ActorController_UpdateTargetMovement(s32, s32, s32);
void ActorController_UpdateModeThreeMovement(s32 a)
{
    ActorController_UpdateTargetMovement(a, 3, 0);
}
