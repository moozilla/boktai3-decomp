#include "global.h"
void sub_08182918(u8 *, u32, u32);
void Armor_SpawnScatterEffect(u8 *);
void Armor_UpdateRunWithScatterEffect(u8 *p)
{
    sub_08182918(p, 20, 0x20);
    if ((p[0xB42] & 0x7) == 0)
        Armor_SpawnScatterEffect(p);
}
