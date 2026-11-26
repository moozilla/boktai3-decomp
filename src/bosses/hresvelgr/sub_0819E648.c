#include "global.h"
extern const u32 gUnk_08611DF4[];
void Callback_InvokeActor(void *, u32);
void Hresvelgr_UpdateTriggerBehavior(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08611DF4[p[0xae]]);
}
