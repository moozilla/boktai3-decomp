#include "global.h"

void Armor_UpdateStrafeMotion(u32, u32, u32);

void Armor_UpdateStrafeNormal(u32 a)
{
    Armor_UpdateStrafeMotion(a, 0x7, 0x40);
}
