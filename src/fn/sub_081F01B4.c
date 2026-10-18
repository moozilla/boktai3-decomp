#include "global.h"
extern const u32 gUnk_08613DCC[];
void Callback_InvokeActor(void *, u32);
void SingletonSlot7_DispatchPhaseCallback(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08613DCC[p[0xae]]);
}
