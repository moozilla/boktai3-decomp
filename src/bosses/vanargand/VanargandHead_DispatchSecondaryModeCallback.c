#include "global.h"
extern const u32 gUnk_08613DFC[];
void Callback_InvokeActor(void *, u32);
void VanargandHead_DispatchSecondaryModeCallback(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08613DFC[p[0xAD]]);
}
