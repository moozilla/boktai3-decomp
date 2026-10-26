#include "global.h"
#include "render.h"
struct Vec50 {
    s32 x : 16, y : 16, z : 16;
};
extern u16 gUnk_03005418[];
extern u32 gUnk_030053F4;
void sub_082359CC(u8 *);
void HealthBurstEffect_InitParticlePool(u8 *);
void sub_08235B34(u8 *);
void HealthBurstEffect_InitSprites(u8 *);
void sub_082358C0(u8 *);
static inline s32 scale50(s32 v)
{
    if (v >= 0)
        v = v >> 5;
    else
        v = -((-v) >> 5);
    return v;
}
// confidence=low: HealthRestoreEffect_Init's placement for the health burst; also hides the
// owner's sprite and keeps the mode argument at 0x34C (mode 0 creates the sprites).
struct BurstEffect {
    u8 pad0[0x18];
    u8 *owner;
    u8 pad1C[0x34c - 0x1c];
    u16 mode;
};
s32 sub_08235C30(u8 *p, u8 *owner, u16 mode)
{
    struct Vec50 v;
    u16 *dest;
    s16 *coords;
    s32 x, z, sum, vertical, y, depth;
    ((struct BurstEffect *)p)->owner = owner;
    ((struct BurstEffect *)p)->mode = mode;
    v = *(struct Vec50 *)(((struct BurstEffect *)p)->owner + 0x30);
    v.y += 150;
    dest = (u16 *)(p + 0x84);
    coords = (s16 *)&v;
    x = coords[0];
    z = coords[2];
    dest[0] = scale50((x - z) * 3);
    sum = scale50((x + z) * 3);
    vertical = scale50(coords[1] * 3);
    y = sum - vertical;
    depth = sum + vertical;
    dest[0] = dest[0] - gUnk_03005418[0] + 120;
    dest[1] = y - gUnk_03005418[1] + 80;
    dest[2] = depth - gUnk_03005418[2];
    sub_082359CC(p);
    HealthBurstEffect_InitParticlePool(p);
    sub_08235B34(p);
    {
        u8 *o = *(u8 **)(p + 0x18);
        if (o[0x419] == 0)
            *(u32 *)(o + 0x94) &= ~1;
        else
            *(u32 *)(o + 0x14c) &= ~1;
    }
    *(u16 *)(*(u8 **)(p + 0x18) + 0x274) = 0;
    gUnk_030053F4 |= 1;
    {
        u32 mask = 4;
        gEnginePauseFlags |= mask;
    }
    if (*(u16 *)(p + 0x34c) == 0)
        HealthBurstEffect_InitSprites(p);
    *(void (**)(u8 *))(p + 0x350) = sub_082358C0;
    *(u16 *)(p + 0x34e) = 0;
    return 0;
}
