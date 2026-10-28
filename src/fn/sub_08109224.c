#include "global.h"
#include "area_effect_link.h"
s32 LinkSession_IncrementAlternateCounter(s32 index)
{
    struct AreaEffectLinkState *p = gLinkSession;
    if (!p || p->unavailable || !p->sessionData)
        return -1;
    p->alternateCounters[index]++;
    return 0;
}
