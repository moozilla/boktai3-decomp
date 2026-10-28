#include "global.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"

extern const s16 gUnk_086149C4[];
void DirectionalEffect_AddParticle(void *, void *, void *, void *);

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
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}

void Player_EmitScatterEffect(void *task, struct SceneVec *center, u32 yaw, u32 pitch, void *arg)
{
    struct Vec16 pos, vel;
    s32 r, v;
    pos.x = (randomValue(gRandomTable) & 0x1F) + center->x - 15;
    pos.y = (randomValue(gRandomTable) & 0x1F) + center->y - 15;
    pos.z = (randomValue(gRandomTable) & 0x1F) + center->z - 15;
    r = (randomValue(gRandomTable) & 3) + 5;
    vel.y = scale(gUnk_086149C4[(yaw + 64) & 255] * r);
    v = -scale(gUnk_086149C4[yaw & 255] * r);
    vel.x = scale(gUnk_086149C4[(pitch + 64) & 255] * v);
    vel.z = scale(gUnk_086149C4[pitch & 255] * v);
    DirectionalEffect_AddParticle(task, &pos, &vel, arg);
}
