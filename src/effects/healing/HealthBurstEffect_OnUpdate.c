#include "global.h"
void FlickerParticle_UpdatePool(void *);
void Callback_InvokeActor(void *, u32);
u32 HealthBurstEffect_OnUpdate(u8 *s)
{
    FlickerParticle_UpdatePool(s);
    Callback_InvokeActor(s, *(u32 *)(s + 0x350));
    return 0;
}
