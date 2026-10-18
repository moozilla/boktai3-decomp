#define gLinkSessionState gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
s32 LinkSession_IncrementAlternateCounter(s32 index)
{
    struct AreaEffectLinkState *p = gUnk_020004B4;
    if (!p || p->unavailable || !p->sessionData)
        return -1;
    p->alternateCounters[index]++;
    return 0;
}
