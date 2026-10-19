#include "global.h"

void Armor_UpdateAxisChase(u8 *p, s32 q);

void Armor_UpdatePlayerAlignmentAction25(u8 *p)
{
    Armor_UpdateAxisChase(p, 0);
}
