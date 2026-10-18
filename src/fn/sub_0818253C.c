#include "global.h"

u32 ActorController_SetAnimation(u32, u32, u32);

void Armor_SetAnimationFromStart(u32 a, u32 b)
{
    ActorController_SetAnimation(a, b, 0);
}
