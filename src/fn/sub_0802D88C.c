#include "global.h"
void ActorShadow_Disable(void *);
void ScaledShadow_Disable(void *);
u32 Actor_DisableShadow(u8 *p)
{
    u8 v = p[0x218];
    if (v == 1)
        ActorShadow_Disable(p + 0x244);
    else if (v == 2)
        ScaledShadow_Disable(p + 0x244);
    return 0;
}
