#include "global.h"

void Armor_UpdateStrafeMotion(u32, u32, u32);

void Armor_UpdateStrafeFast(u32 a)
{
    Armor_UpdateStrafeMotion(a, 0xA, 0x2A);
}
