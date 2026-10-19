#define SpriteEffect_CreateAtActor sub_08113534
#define SpriteEffect_Create sub_08113398
#include "global.h"
struct SpriteEffectOrigin {
    u8 pad0[0x10]; s32 x, y, z; u8 pad1C[0x38-0x1C]; s32 elevation;
};
s32 sub_08113398(s16 *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
s32 sub_08113534(struct SpriteEffectOrigin *s, s16 *off, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g, u32 h, u32 i, u32 j, u32 k, u32 l, u32 m)
{
    s16 v[3];
    s32 y, z;
    u16 *q = (u16 *)&v;
    v[0] = s->x >> 12;
    y = s->y + s->elevation;
    z = s->z;
    v[1] = ((z + y) >> 12) + 0x60;
    v[2] = z >> 12;
    if (off) {
        v[0] = off[0] + q[0];
        v[1] = q[1] + off[1];
        v[2] = off[2] + q[2];
    }
    return sub_08113398(v, a, b, c, d, e, f, g, h, i, j, k, l, m);
}
