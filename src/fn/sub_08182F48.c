#include "global.h"
void Armor_UpdateNegativeTurn(s32, s32);
void Armor_UpdateNegativeTurnFaster(s32 a)
{
    Armor_UpdateNegativeTurn(a, 0x23);
}
