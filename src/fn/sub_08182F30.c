#include "global.h"
void Armor_UpdateNegativeTurn(s32, s32);
void Armor_UpdateNegativeTurnFast(s32 a)
{
    Armor_UpdateNegativeTurn(a, 0x2a);
}
