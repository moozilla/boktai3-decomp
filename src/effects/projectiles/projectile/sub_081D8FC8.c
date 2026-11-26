#include "global.h"
struct Q {
    u8 f[0x10];
    u16 v;
    u8 g[0x1C - 0x12];
};
void Projectile_SetUpdateCallback(void *, void *);
void TimedEffect_TestDuration(void);
s32 Projectile_InitAlternatingExpiry(struct Q *p)
{
    Projectile_SetUpdateCallback((u8 *)p + 0x18, TimedEffect_TestDuration);
    (p + 5)->v = 0x19;
    return 0;
}
