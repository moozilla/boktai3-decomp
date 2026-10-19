#include "global.h"

void Armor_UpdateRunMotion(u32, u32, u32);

void Armor_UpdateRun(u32 a)
{
    Armor_UpdateRunMotion(a, 0x14, 0x20);
}
