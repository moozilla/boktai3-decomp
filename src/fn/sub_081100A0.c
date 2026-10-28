#include "global.h"
// confidence=low: copies the local link result and clamps its displayed score.
#define LinkScorePanel_UpdateDisplayedScore sub_081100A0
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

static inline s32 ClampScore(struct LinkScorePanel *panel)
{
    s32 v = panel->score;
    s32 limit = 9999;
    if (v > limit)
        v = limit;
    return v;
}
void LinkScorePanel_UpdateDisplayedScore(struct LinkScorePanel *panel)
{
    struct AreaEffectLinkState *link = gLinkSession;
    struct AreaEffectLinkState *link2;
    s32 i, v;
    u8 ok;

    if (link == NULL || link->unavailable != 1)
        goto zero;
    link2 = gLinkSession;
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
    panel->score = ClampScore(panel);
    panel->timer = 0x5a;
}
