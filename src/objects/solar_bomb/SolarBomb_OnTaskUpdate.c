#include "global.h"
void Callback_InvokeActor(void *, u32);
u32 SolarBomb_OnTaskUpdate(u8 *s)
{
    Callback_InvokeActor(s, *(u32 *)(s + 0x1dc));
    return 0;
}
