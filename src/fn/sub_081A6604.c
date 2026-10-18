#include "global.h"
#define PositionedEffect_UpdateVectors sub_081A6604
struct EffectVector {
    s16 x, y, z, pad;
};
struct EffectAnimation {
    u8 unknown0[0x1c];
    struct EffectVector combined;
    u8 unknown24[0x58];
    struct EffectVector *offset;
};
struct PositionedEffect {
    u8 unknown0[0x20];
    struct EffectVector position;
    u8 unknown28[0x58];
    struct EffectVector source;
    struct EffectVector shifted;
    u8 unknown90[0x18];
    struct EffectAnimation animation;
    struct EffectVector base;
};
static inline void SetVector(struct EffectVector *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
static inline void AddVector(struct EffectVector *p, struct EffectVector *a, struct EffectVector *b)
{
    p->x = a->x + b->x;
    p->y = a->y + b->y;
    p->z = a->z + b->z;
}
static inline void UpdateAnimation(struct EffectAnimation *a, struct PositionedEffect *p)
{
    struct EffectVector *out = &a->combined;
    struct EffectVector *offset = a->offset;
    AddVector(out, offset, &p->base);
}
void sub_081A6604(struct PositionedEffect *p)
{
    p->position = p->source;
    SetVector(&p->shifted, p->source.x - 160, p->source.y, p->source.z - 160);
    UpdateAnimation(&p->animation, p);
}
