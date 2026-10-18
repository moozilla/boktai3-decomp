#define LinkSession_IncrementCounter sub_081091E4
#define gLinkSessionState gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
s32 sub_081091E4(s32 index)
{
    struct AreaEffectLinkState *p = gUnk_020004B4;
    if (!p || p->unavailable || !p->sessionData) return -1;
    p->counters[index]++;
    return 0;
}
