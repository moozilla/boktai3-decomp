#include "link/link_score_panel.h"
#define LinkMenu_DrawFourDigitNumber LinkScorePanel_SetNumber
#define gRandomTableIndex            gEffectRandomTableIndex
#define gRandomValues                gUnk_086141BC
#include "global.h"
#include "engine/ram.h"
extern const u16 gUnk_086141BC[];
void LinkScorePanel_SetNumber(struct LinkScorePanel *, s32, s32);
static inline u32 nextRandom(void)
{
    u32 value;
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
    value = gUnk_086141BC[gEffectRandomTableIndex];
    return value;
}
void LinkMenu_DrawRandomizedFourDigitNumber(struct LinkScorePanel *menu, s32 row)
{
    s32 number = (menu->h * menu->h) >> 2;
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
