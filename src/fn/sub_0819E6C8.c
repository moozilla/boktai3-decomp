#include "global.h"
extern const u32 gUnk_08611E0C[];
void Callback_InvokeActor(void *, u32);
void Hresvelgr_UpdateTriggerMode(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08611E0C[p[0xAD]]);
}
