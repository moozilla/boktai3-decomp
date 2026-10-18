#include "global.h"
extern const u32 gUnk_08611CBC[];
void sub_0824923C(void *, u32);
void BossSlot2_RunPhaseEntryCallback(u8 *p)
{
    sub_0824923C(p, gUnk_08611CBC[p[0xad]]);
}
