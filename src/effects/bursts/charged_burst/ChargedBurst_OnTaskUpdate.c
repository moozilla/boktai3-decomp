#include "global.h"
void Callback_InvokeActor(void *, u32);
u32 ChargedBurst_OnTaskUpdate(u8 *s)
{
    Callback_InvokeActor(s, *(u32 *)(s + 0x20c));
    return 0;
}
