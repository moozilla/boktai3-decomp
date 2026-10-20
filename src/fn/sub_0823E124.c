#include "global.h"
#include "scene_vec.h"

#define Player_EmitRandomSpark sub_0823E124

extern const s16 gUnk_086149C4[];
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];

struct BitVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};

void sub_08239098(void *, struct BitVec *, struct BitVec *, u32);

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

#define RAND() (*(u16 *)((u8 *)tbl + ((gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF) << 1)))

void Player_EmitRandomSpark(void *p, struct SceneVec *in, u32 angle, u32 extra)
{
    struct BitVec a, b;
    s32 k;
    u16 *tbl = gUnk_0203B400;
    a.x = (RAND() % 32) + in->x - 15;
    a.y = (RAND() % 32) + in->y - 15;
    a.z = (RAND() % 32) + in->z - 15;
    k = (RAND() % 4) + 3;
    b.x = -scale(gUnk_086149C4[(angle + 64) & 255] * k);
    b.y = 0;
    b.z = -scale(gUnk_086149C4[angle & 255] * k);
    sub_08239098(p, &a, &b, extra);
}
