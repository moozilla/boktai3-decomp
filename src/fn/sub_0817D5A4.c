#include "global.h"
extern const u32 gUnk_08611CC0[];
void Callback_InvokeActor(void *, u32);
void BossSlot2_RunPhaseExitCallback(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08611CC0[p[0xad]]);
}
