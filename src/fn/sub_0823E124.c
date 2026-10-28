#include "global.h"
#include "scene_vec.h"
#include "engine_globals.h"
#include "ram.h"

extern const s16 gUnk_086149C4[];

struct BitVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};

void DirectionalEffect_AddParticle(void *, struct BitVec *, struct BitVec *, u32);

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

#define RAND() (*(u16 *)((u8 *)tbl + ((gRandomIndex = (gRandomIndex + 1) & 0x3FF) << 1)))

void Player_EmitRandomSpark(void *p, struct SceneVec *in, u32 angle, u32 extra)
{
    struct BitVec a, b;
    s32 k;
    u16 *tbl = gRandomTable;
    a.x = (RAND() % 32) + in->x - 15;
    a.y = (RAND() % 32) + in->y - 15;
    a.z = (RAND() % 32) + in->z - 15;
    k = (RAND() % 4) + 3;
    b.x = -scale(gUnk_086149C4[(angle + 64) & 255] * k);
    b.y = 0;
    b.z = -scale(gUnk_086149C4[angle & 255] * k);
    DirectionalEffect_AddParticle(p, &a, &b, extra);
}
