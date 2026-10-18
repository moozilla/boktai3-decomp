#include "global.h"
void ActorEffect_UpdateFadeFrame(u8 *);
void VanargandEffect_UpdateLifetime(u8 *);
void VanargandEffect_Update(u8 *p)
{
    u16 *c = (u16 *)(p + 0x98);
    if (*c) {
        (*c)--;
        if (*c == 0)
            *(u32 *)(p + 0x18) &= ~1;
    } else {
        ActorEffect_UpdateFadeFrame(p);
        VanargandEffect_UpdateLifetime(p);
    }
}
