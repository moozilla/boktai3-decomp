#include "global.h"

extern const s16 gUnk_086149C4[];

struct RbPos {
    s16 x, y, z;
};
struct RbParticle {
    u32 flags;
    u8 unknown4[0x14];
    s16 x, y, z;
    u8 unknown1E[0xA];
    u8 active;
    u8 unknown29;
    u8 angle;
    u8 speed;
    u16 timer;
    u8 unknown2E[4];
    u16 radius;
    struct RbPos *origin;
};
s32 Div(s32, s32);
void SparkParticle_Spawn(void *, u32, const void *, u32, u32, u32, u32, u32);

static inline s32 Scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void RadialBurstEffect_UpdateBurstParticle(struct RbParticle *p)
{
    if (++p->timer > 15) {
        p->flags |= 1;
        p->active = 0;
        SparkParticle_Spawn(&p->x, 1, 0, 0, 0, 0, 2, 1);
    } else if (p->timer > 11) {
        p->x = p->origin->x + Scale12(gUnk_086149C4[(p->angle + 0x40) & 0xFF] * 10);
        p->y = p->origin->y;
        p->z = p->origin->z + Scale12(gUnk_086149C4[p->angle] * 10);
    } else {
        s32 n = 12 - p->timer;
        s32 d = n + 1;
        p->angle += p->speed;
        p->radius = Div(p->radius * n, d);
        p->x = p->origin->x + Scale12(p->radius * gUnk_086149C4[(p->angle + 0x40) & 0xFF]);
        p->y = p->origin->y;
        p->z = p->origin->z + Scale12(p->radius * gUnk_086149C4[p->angle]);
    }
}
