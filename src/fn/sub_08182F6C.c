#include "global.h"
void Armor_UpdatePositiveTurn(s32, s32);
void ActorController_UpdatePositiveTurnSpeed32(s32 a)
{
    Armor_UpdatePositiveTurn(a, 0x20);
}
