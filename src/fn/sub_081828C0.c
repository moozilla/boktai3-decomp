#include "global.h"
void Armor_UpdateWalk(u8 *, u32, u32);
void Armor_SpawnScatterEffect(u8 *);
void Armor_UpdateWalkNormalWithScatterEffect(u8 *p)
{
    Armor_UpdateWalk(p, 10, 0x40);
    if ((p[0xB42] & 0x1f) == 0)
        Armor_SpawnScatterEffect(p);
}
