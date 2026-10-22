#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.2%
// Open: the clamp should load the score into r1 with a signed compare and recompute
// the store address (ROM puts 9999 in r0).
#include "area_effect_link.h"

struct LinkScorePanel {
    u8 unknown0[0x102E];
    u16 timer;
    u16 score;
};

static inline u8 validSelection(s32 i)
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
    goto lo;
zero:
    panel->score = 0;
    goto clamp;
lo:
    v = 0;
hi:
    ok = validSelection(i);
sel:
    if (!ok)
        v = 0;
    else
        v = link2->menuResults[i];
    panel->score = v;
clamp:
    if (panel->score <= 9999)
        v = panel->score;
    else
        v = 9999;
    panel->score = v;
    panel->timer = 0x5a;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081100A0);
#endif
