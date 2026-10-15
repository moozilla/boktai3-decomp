#include "repeated_effect.h"
#define RepeatedEffect_TickLowerState sub_0804F888

void RepeatedEffect_TickLowerState(struct RepeatedEffect *p)
{
    if (*(s16 *)((u8 *)gUnk_02000710 + 0x876) > 0) {
        u16 *c = (u16 *)&p->balance;
        u32 v = *c + 1;
        *c = v;
        if ((s32)(v << 16) > 0x012B0000)
            sub_0804F7D0(p, 0, sub_0804F91C);
    }
}
