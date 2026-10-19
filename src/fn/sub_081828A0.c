#include "global.h"

void Armor_UpdateWalk(u32, u32, u32);

void Armor_UpdateWalkFast(u32 a)
{
    Armor_UpdateWalk(a, 0xF, 0x2A);
}
