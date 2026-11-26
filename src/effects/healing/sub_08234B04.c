#include "global.h"
void Callback_InvokeActor(void *, u32);
void HealthRestoreEffect_UpdateParticles(u8 *s)
{
    u8 *p = s + 0x8c;
    u32 o = 0;
    s32 i = 7;
    do {
        if (*(u16 *)(p + 0x2a) != 0) {
            u8 *q = s + 0xc4;
            Callback_InvokeActor(p, *(u32 *)(q + o));
        }
        p += 0x3c;
        o += 0x3c;
        i--;
    } while (i >= 0);
}
