#include "global.h"
#include "scene_vec.h"

extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void sub_08239098(void *, void *, void *, void *);

#define Player_EmitScatterEffect sub_0823E810

struct Vec16 {
    u32 x : 16, y : 16, z : 16;
};

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

static inline u32 randomValue(const u16 *table)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)table);
}

void Player_EmitScatterEffect(void *task, struct SceneVec *center, u32 yaw, u32 pitch, void *arg)
{
    struct Vec16 pos, vel;
    s32 r, v;
    pos.x = (randomValue(gUnk_0203B400) & 0x1F) + center->x - 15;
    pos.y = (randomValue(gUnk_0203B400) & 0x1F) + center->y - 15;
    pos.z = (randomValue(gUnk_0203B400) & 0x1F) + center->z - 15;
    r = (randomValue(gUnk_0203B400) & 3) + 5;
    vel.y = scale(gUnk_086149C4[(yaw + 64) & 255] * r);
    v = -scale(gUnk_086149C4[yaw & 255] * r);
    vel.x = scale(gUnk_086149C4[(pitch + 64) & 255] * v);
    vel.z = scale(gUnk_086149C4[pitch & 255] * v);
    sub_08239098(task, &pos, &vel, arg);
}
