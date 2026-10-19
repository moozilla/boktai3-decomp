#include "global.h"

void Armor_UpdateStrafeMotion(u32, u32, u32);

void Armor_UpdateStrafeFaster(u32 a)
{
    Armor_UpdateStrafeMotion(a, 0xD, 0x23);
}
