#include "global.h"
extern const u32 gUnk_08613DF8[];
void Callback_InvokeActor(void *, u32);
void SingletonSlot7_DispatchPrimaryModeCallback(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08613DF8[p[0xAD]]);
}
