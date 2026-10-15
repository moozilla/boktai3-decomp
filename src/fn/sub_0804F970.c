#include "repeated_effect.h"
#define RepeatedEffect_TickUpperState sub_0804F970
static inline u8 tst(u32 *a, u32 m)
{
    if (*a & m)
        return TRUE;
    return FALSE;
}
void RepeatedEffect_TickUpperState(struct RepeatedEffect *p)
{
    u32 m = 0x1000;
    if (tst((u32 *)((u8 *)gUnk_02000710 + 0x868), m)) {
        u16 *c = (u16 *)&p->balance;
        u32 v = *c + 1;
        *c = v;
        if ((s32)(v << 16) > 0x012B0000)
            sub_0804F7D0(p, 0, sub_0804FB0C);
    }
}
