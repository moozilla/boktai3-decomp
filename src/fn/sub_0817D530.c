#include "global.h"
extern const u32 gUnk_08611CA8[];
void Callback_InvokeActor(void *, u32);
void BossSlot2_RunVariantCallback(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08611CA8[p[0xae]]);
}
