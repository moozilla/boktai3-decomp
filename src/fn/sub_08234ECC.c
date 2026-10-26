#include "global.h"
#include "render.h"
void HealthRestoreEffect_UpdateParticles(void *);
void Callback_InvokeActor(void *, u32);
u32 HealthRestoreEffect_OnUpdate(u8 *s)
{
    HealthRestoreEffect_UpdateParticles(s);
    Callback_InvokeActor(s, *(u32 *)(s + 0x274));
    gGrayscaleControl = 2;
    return 0;
}
