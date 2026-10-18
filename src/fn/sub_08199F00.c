#include "global.h"

void ExplosionEffect_Create(u8 *, u32, u32, u32, u32);
void Callback_InvokeActor(u8 *, u32);

void CannonBattery_OnProjectileCollision(u8 *p, u32 unused, u8 *q)
{
    u8 *c = p + 0xC;
    u16 a2 = *(u16 *)(p + 0x3C);
    u16 a3 = *(u16 *)(p + 0x40);
    u32 z = 0;
    u32 w;
    ExplosionEffect_Create(c, 1, a2, a3, z);
    *(u32 *)q = z;
    w = *(u32 *)(q + 4);
    if (w) {
        Callback_InvokeActor(q, w);
        *(u32 *)(q + 4) = z;
    }
}
