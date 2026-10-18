#include "global.h"
extern const u32 gUnk_08611CC0[];
void sub_0824923C(void *, u32);
void BossSlot2_RunPhaseExitCallback(u8 *p)
{
    sub_0824923C(p, gUnk_08611CC0[p[0xad]]);
}
