#define LinkSession_CountPlayerKind sub_08109134
#define gLinkSessionState gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
s32 sub_08109134(s32 kind)
{
    struct AreaEffectLinkState *p = gUnk_020004B4;
    s32 count, i;
    if (!p || p->unavailable || !p->sessionData) return -1;
    count = 0;
    for (i = 0; i <= 4; i++) {
        if (p->playerKinds[i] == kind) count++;
    }
    return count;
}
