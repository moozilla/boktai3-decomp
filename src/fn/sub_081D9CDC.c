#include "global.h"
struct S {
    u8 f[0x18];
    u16 fl;
    u8 g[0x8c - 0x1a];
    u32 a;
    u8 h[0xc];
    u16 b;
};
void Projectile_SetUpdateCallback(void *, void (*)(void));
void Projectile_UpdateResetDelay(void);
s32 Projectile_InitResetDelay(u8 *p)
{
    u8 *q;
    Projectile_SetUpdateCallback(p + 0x18, Projectile_UpdateResetDelay);
    if (!(*(u16 *)(p + 0x18) & 0x2000)) {
        q = p + 0x8c;
        *(u16 *)(q + 0x10) = 0x19;
        *(u32 *)q &= ~0xc;
    }
    return 0;
}
