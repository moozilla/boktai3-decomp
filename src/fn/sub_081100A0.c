#include "global.h"
#ifdef NONMATCHING
// fuzzy: 88.1%
#include "area_effect_link.h"

struct LinkScorePanel {
    u8 unknown0[0x102E];
    u16 timer;
    u16 score;
};

static inline u8 Id(u8 x)
{
    return x;
}

void sub_081100A0(struct LinkScorePanel *panel)
{
    struct AreaEffectLinkState *link = gUnk_020004B4;
    struct AreaEffectLinkState *link2;
    s32 i, v;
    u8 ok;

    if (link == NULL || link->unavailable != 1)
        goto zero;
    link2 = gUnk_020004B4;
    i = link2->localPlayer;
    if (i >= 0)
        goto hi;
    goto bad;
zero:
    panel->score = 0;
    goto clamp;
hi:
    if (i <= 1)
        goto good;
bad:
    ok = 0;
    goto sel;
good:
    ok = 1;
sel:
    if (!Id(ok))
        v = 0;
    else
        v = link2->menuResults[i];
    panel->score = v;
clamp:
    v = panel->score;
    if (v > 9999)
        v = 9999;
    panel->score = v;
    panel->timer = 0x5a;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081100A0);
#endif
