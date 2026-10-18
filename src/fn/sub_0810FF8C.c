#define LinkMenu_DrawFourDigitNumber LinkScorePanel_SetNumber
#define gRandomTableIndex                      gUnk_03005304
#define gRandomValues                          gUnk_086141BC
#include "global.h"
struct LinkMenuCounter {
    u8 unknown0[0x1026];
    u16 counter;
};
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
void LinkScorePanel_SetNumber(struct LinkMenuCounter *, s32, s32);
static inline u32 nextRandom(void)
{
    u32 value;
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3FF;
    value = gUnk_086141BC[gUnk_03005304];
    return value;
}
void LinkMenu_DrawRandomizedFourDigitNumber(struct LinkMenuCounter *menu, s32 row)
{
    s32 number = (menu->counter * menu->counter) >> 2;
    {
        u32 value = nextRandom();
        value &= 7;
        number += value * 10;
    }
    {
        u32 value = nextRandom();
        value &= 7;
        number += value * 100;
    }
    {
        u32 value = nextRandom();
        value &= 7;
        number += value * 1000;
    }
    if (number > 9999)
        number = 9999;
    LinkScorePanel_SetNumber(menu, row, number);
}
