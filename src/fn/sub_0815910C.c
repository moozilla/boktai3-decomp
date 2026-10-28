#include "global.h"
#include "ram.h"
extern u8 *gGameState;
s32 sub_0815910C(u8 *item)
{
    s32 bonus;
    s16 *level;
    if (*(s16 *)(gGameState + 0x5a8) == 6)
        return gPlayerLinkAttackBonus;
    bonus = 0;
    if (item == NULL)
        return (bonus + *(s16 *)(gGameState + 0x40)) >> 1;
    if (item[1] == 0x55)
        bonus = item[2];
    if (bonus > 120)
        bonus = 120;
    bonus = (bonus + *(s16 *)(gGameState + 0x40)) >> 1;
    return bonus;
}
