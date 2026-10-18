#include "global.h"
void ActorController_UpdateTargetMovement(s32, s32, s32);
void ActorController_UpdateModeOneMovement(s32 a)
{
    ActorController_UpdateTargetMovement(a, 1, 0);
}
