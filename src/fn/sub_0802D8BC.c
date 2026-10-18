#include "global.h"
void ActorShadow_Enable(void *);
void ScaledShadow_Enable(void *);
u32 Actor_EnableShadow(u8 *p)
{
    u8 v = p[0x218];
    if (v == 1)
        ActorShadow_Enable(p + 0x244);
    else if (v == 2)
        ScaledShadow_Enable(p + 0x244);
    return 0;
}
