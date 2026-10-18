#include "global.h"
struct Q {
    u8 f[0x10];
    u16 v;
    u8 g[0x1C - 0x12];
};
void Projectile_SetUpdateCallback(void *, void (*)(void));
void sub_081D92F0(void);
static inline void clr(u32 *a, u32 m)
{
    *a = m & *a;
}
s32 Projectile_InitArcFlight(struct Q *p)
{
    Projectile_SetUpdateCallback((u8 *)p + 0x18, sub_081D92F0);
    (p + 5)->v = 0x19;
    clr((u32 *)((u8 *)p + 4), ~1);
    {
        u16 t = *(u16 *)((u8 *)p + 0xaa);
        *(u16 *)((u8 *)p + 0x17e) = t;
    }
    return 0;
}
