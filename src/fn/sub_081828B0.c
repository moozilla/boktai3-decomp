#include "global.h"

void Armor_UpdateWalk(u32, u32, u32);

void Armor_UpdateWalkFaster(u32 a)
{
    Armor_UpdateWalk(a, 0x12, 0x23);
}
