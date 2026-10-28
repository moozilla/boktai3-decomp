#include "global.h"
#include "engine.h"
#include "render.h"
struct Vec50 {
    s32 x : 16, y : 16, z : 16;
};
void HealthRestoreEffect_InitAnimation(u8 *);
void HealthRestoreEffect_InitParticlePool(u8 *);
void sub_08234DC8(u8 *);
static inline s32 scale50(s32 v)
{
    if (v >= 0)
        v = v >> 5;
    else
        v = -((-v) >> 5);
    return v;
}
s32 HealthRestoreEffect_Init(u8 *p, u8 *owner)
{
    struct Vec50 v;
    u16 *dest;
    s16 *coords;
    s32 x, z, sum, vertical, y, depth;
    u32 zero;
    *(u8 **)(p + 0x18) = owner;
    v = *(struct Vec50 *)(owner + 0x30);
    v.y += 150;
    dest = (u16 *)(p + 0x84);
    coords = (s16 *)&v;
    x = coords[0];
    z = coords[2];
    dest[0] = scale50((x - z) * 3);
    sum = scale50((x + z) * 3);
    vertical = scale50(coords[1] * 3);
    y = sum - vertical;
    zero = 0;
    depth = sum + vertical;
    dest[0] = dest[0] - gCameraOrigin[0] + 120;
    dest[1] = y - gCameraOrigin[1] + 80;
    dest[2] = depth - gCameraOrigin[2];
    HealthRestoreEffect_InitAnimation(p);
    HealthRestoreEffect_InitParticlePool(p);
    *(void (**)(u8 *))(p + 0x274) = sub_08234DC8;
    *(u16 *)(p + 0x270) = zero;
    gEngineFlags |= 1;
    return 0;
}
