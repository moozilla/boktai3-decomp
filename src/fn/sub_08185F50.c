#include "global.h"
void ActorController_UpdateTargetMovement(s32, s32, s32);
void Armor_UpdateTrackingAction31(s32 a)
{
    ActorController_UpdateTargetMovement(a, 0, 1);
}
