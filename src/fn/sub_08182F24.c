#include "global.h"
void Armor_UpdatePositiveTurn(s32, s32);
void Armor_UpdatePositiveTurnFast(s32 a)
{
    Armor_UpdatePositiveTurn(a, 0x2a);
}
