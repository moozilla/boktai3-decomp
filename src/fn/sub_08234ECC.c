#include "global.h"
void HealthRestoreEffect_UpdateParticles(void *);
void Callback_InvokeActor(void *, u32);
extern u32 gUnk_030051E4;
u32 HealthRestoreEffect_OnUpdate(u8 *s)
{
    HealthRestoreEffect_UpdateParticles(s);
    Callback_InvokeActor(s, *(u32 *)(s + 0x274));
    gUnk_030051E4 = 2;
    return 0;
}
