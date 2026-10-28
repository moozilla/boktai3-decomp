#include "global.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"

struct OrbitEffect {
    u8 unk_00[0x50];
    struct SceneVec position;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                   s32, s32, s32, s32, s32, s32);
static inline u16 RandomValue(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void Polar(struct SceneVec *v, s32 angle, s32 speed)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle] * speed);
}
void OrbitEffect_SpawnInwardParticle(struct OrbitEffect *effect)
{
    struct SceneVec position, velocity;
    struct SceneVec *pos = &position;
    struct SceneVec *origin = &effect->position;
    u32 angle;
    *pos = *origin;
    pos->x += -512 + (u8)RandomValue(gRandomTable) * 4;
    pos->z += -512 + (u8)RandomValue(gRandomTable) * 4;
    angle = (u8)Math_Atan2(origin->x - pos->x, origin->z - pos->z);
    Polar(&velocity, angle, 32);
    SpriteEffectController_Create(&position, &velocity, 0x1C1E, 0xD43F, 0, 0, 17, 0, 2, 6, 0, 5, 0,
                                  0, 0, 0, -4, -4);
}
