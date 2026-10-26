#include "repeated_effect.h"
static inline u8 tst(u32 *a, u32 m)
{
    if (*a & m)
        return TRUE;
    return FALSE;
}
void RepeatedEffect_TickUpperState(struct RepeatedEffect *p)
{
    u32 m = 0x1000;
    if (tst((u32 *)((u8 *)gGameState + 0x868), m)) {
        u16 *c = (u16 *)&p->balance;
        u32 v = *c + 1;
        *c = v;
        if ((s32)(v << 16) > 0x012B0000)
            RepeatedEffect_SetState(p, 0, RepeatedEffect_TickUpperRecovery);
    }
}
