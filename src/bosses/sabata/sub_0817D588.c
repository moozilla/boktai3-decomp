#include "global.h"
extern const u32 gUnk_08611CBC[];
void Callback_InvokeActor(void *, u32);
void BossSlot2_RunPhaseEntryCallback(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08611CBC[p[0xad]]);
}
