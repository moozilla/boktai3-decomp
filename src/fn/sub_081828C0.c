#include "global.h"
void sub_081826C8(u8 *, u32, u32);
void Armor_SpawnScatterEffect(u8 *);
void Armor_UpdateWalkNormalWithScatterEffect(u8 *p)
{
    sub_081826C8(p, 10, 0x40);
    if ((p[0xB42] & 0x1f) == 0)
        Armor_SpawnScatterEffect(p);
}
