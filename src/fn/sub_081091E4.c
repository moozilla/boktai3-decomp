#include "global.h"
#include "area_effect_link.h"
s32 LinkSession_IncrementCounter(s32 index)
{
    struct AreaEffectLinkState *p = gLinkSession;
    if (!p || p->unavailable || !p->sessionData)
        return -1;
    p->counters[index]++;
    return 0;
}
