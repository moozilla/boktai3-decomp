#include "global.h"
void Armor_UpdateNegativeTurn(s32, s32);
void ActorController_UpdateNegativeTurnSpeed32(s32 a)
{
    Armor_UpdateNegativeTurn(a, 0x20);
}
