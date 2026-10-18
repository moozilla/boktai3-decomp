#define LinkSession_IncrementAlternateCounter sub_08109224
#define gLinkSessionState gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
s32 sub_08109224(s32 index)
{
    struct AreaEffectLinkState *p = gUnk_020004B4;
    if (!p || p->unavailable || !p->sessionData) return -1;
    p->alternateCounters[index]++;
    return 0;
}
