#define LinkMenu_BeginResultReveal sub_081100A0
#define gLinkSession               gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
struct LinkMenuResult {
    u8 unknown0[0x102E];
    u16 revealTimer;
    u16 result;
};
static inline u8 validPlayer(s32 i)
{
    if (i < 0)
        goto no;
    if (i <= 1)
        goto yes;
no:
    return 0;
yes:
    return 1;
}
static inline s32 getResult(void)
{
    struct AreaEffectLinkState *link = gUnk_020004B4;
    s32 i = link->localPlayer;
    if (!validPlayer(i))
        return 0;
    return link->menuResults[i];
}
void sub_081100A0(struct LinkMenuResult *menu)
{
    s32 value;
    if (gUnk_020004B4 && gUnk_020004B4->unavailable == 1)
        menu->result = getResult();
    else
        menu->result = 0;
    value = menu->result;
    if (value > 9999)
        value = 9999;
    {
        u16 result = value;
        menu->result = result;
    }
    menu->revealTimer = 90;
}
