#include "global.h"
#include "engine.h"

struct ContactVector {
    u32 x : 16, y : 16, z : 16;
};
struct EffectVector {
    s16 x, y, z, pad;
};
union ColliderOffset {
    struct ContactVector bits;
    struct EffectVector vector;
};
struct EffectCollider {
    u8 unk_00[0x54];
};
struct PositionedEffect {
    u8 unk_00[0x584];
    u16 attackId;
    u8 unk_586[0x96];
    struct EffectCollider colliders[2];
};
extern const s16 gUnk_086149C4[];
void sub_081A6104(void);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void SetPolar(struct EffectVector *vector, s32 angle, s32 radius)
{
    vector->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    vector->y = 0;
    vector->z = Scale(gUnk_086149C4[angle & 255] * radius);
}

void PositionedEffect_InitRadialAttackColliders(struct PositionedEffect *actor)
{
    struct ContactVector size;
    union ColliderOffset offset;
    struct EffectCollider *collider;
    s32 angle, i;
    size.x = 128;
    size.y = 128;
    size.z = 128;
    offset.bits.y = 0;
    collider = actor->colliders;
    for (i = 0; i < 2; i++) {
        angle = 240 + i * 96;
        SetPolar(&offset.vector, angle & 255, 416);
        Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
        Collider_SetCallback(collider, sub_081A6104, actor);
        Collider_SetAttackProperties(collider, actor->attackId, 80, 0, 0, 30);
        collider++;
    }
}
