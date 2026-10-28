#include "global.h"
#include "scene_vec.h"

extern const s16 gUnk_086149C4[];
extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];

struct BitVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};

void DirectionalEffect_AddParticle(void *, struct BitVec *, struct BitVec *, u32);

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

#define RAND() (*(u16 *)((u8 *)gUnk_0203B400 + ((gRandomIndex = (gRandomIndex + 1) & 0x3FF) << 1)))

void Player_EmitSweepSpark(void *p, struct SceneVec *in, u32 angle, u32 side, u32 index)
{
    struct BitVec a, b;
    u32 t = RAND();
    s32 k = (t & 0x7F) + 0x70;
    a.x = in->x + scale(gUnk_086149C4[(angle + 64) & 255] * k);
    a.y = in->y;
    a.z = in->z + scale(gUnk_086149C4[angle & 255] * k);
    b.x = scale(gUnk_086149C4[angle & 255] * 6);
    b.y = 0;
    b.z = -scale(gUnk_086149C4[(angle + 64) & 255] * 6);
    if (side) {
        b.x = -b.x;
        b.z = -b.z;
    }
    DirectionalEffect_AddParticle(p, &a, &b, index);
}
