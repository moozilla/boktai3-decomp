#include "global.h"
void sub_08235294(void *);
void Callback_InvokeActor(void *, u32);
u32 HealthBurstEffect_OnUpdate(u8 *s)
{
    sub_08235294(s);
    Callback_InvokeActor(s, *(u32 *)(s + 0x350));
    return 0;
}
