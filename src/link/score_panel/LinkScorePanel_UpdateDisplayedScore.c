#include "global.h"
// confidence=low: copies the local link result and clamps its displayed score.
#include "scene/areas/area_effect_link.h"
#include "ui/save/save_menu.h"

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

static inline s32 ClampScore(struct SaveMenu *menu)
{
    s32 v = menu->remaining;
    if (v > 9999)
        v = 9999;
    return v;
}
void LinkScorePanel_UpdateDisplayedScore(struct SaveMenu *menu)
{
    struct AreaEffectLinkState *link = gLinkSession;
    struct AreaEffectLinkState *link2;
    s32 i, v;

    if (link == NULL || link->unavailable != 1)
        goto zero;
    link2 = gLinkSession;
    i = link2->localPlayer;
    if (i >= 0)
        goto hi;
    goto lo;
zero:
    menu->remaining = 0;
    goto clamp;
lo:
    v = 0;
hi:
    if (!validSelection(i))
        v = 0;
    else
        v = link2->menuResults[i];
    menu->remaining = v;
clamp:
    menu->remaining = ClampScore(menu);
    menu->skip = 0x5a;
}
