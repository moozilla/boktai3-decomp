#include "global.h"
#include "area_effect_link.h"
s32 LinkSession_CountPlayerKind(s32 kind)
{
    struct AreaEffectLinkState *p = gLinkSession;
    s32 count, i;
    if (!p || p->unavailable || !p->sessionData)
        return -1;
    count = 0;
    for (i = 0; i <= 4; i++) {
        if (p->playerKinds[i] == kind)
            count++;
    }
    return count;
}
