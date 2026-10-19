#include "global.h"
void Armor_UpdateRunMotion(u8 *, u32, u32);
void Armor_SpawnScatterEffect(u8 *);
void Armor_UpdateRunWithScatterEffect(u8 *p)
{
    Armor_UpdateRunMotion(p, 20, 0x20);
    if ((p[0xB42] & 0x7) == 0)
        Armor_SpawnScatterEffect(p);
}
