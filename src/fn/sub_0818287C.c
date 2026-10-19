#include "global.h"

void Armor_UpdateWalk(u32, u32, u32);

void Armor_UpdateWalkNormal(u32 a)
{
    Armor_UpdateWalk(a, 0xA, 0x40);
}
