#include "global.h"

// foundry draft (worker best 97.0%); what differs is mostly register allocation and instruction order
#define gEffectSineTable gUnk_086149C4
struct EffectPosition {
    s16 x, y, z, unused;
};
struct EffectTrailSprite {
    struct EffectPosition position;
    u8 pad[0x24];
};
struct ActorEffect {
    u8 pad0[6];
    u16 flags;
    u8 trailIndex;
    u8 pad9[0x1c - 9];
    struct EffectPosition history[6];
    u8 pad4c[0x5c - 0x4c];
    u8 angles[6];
    u8 radii[6];
    u8 pad68[0x12c - 0x68];
    struct EffectTrailSprite trails[6];
};
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void MakeOffset(struct EffectPosition *v, u32 angle, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void AddOffset(struct EffectPosition *p, struct EffectPosition *v)
{
    p->x += v->x;
    p->y += v->y;
    p->z += v->z;
}
void ActorEffect_UpdateRadialTrail(struct ActorEffect *effect)
{
    s32 index = effect->trailIndex - 1;
    s32 i;
    struct EffectPosition delta;
    struct EffectPosition *new_var;
    struct EffectTrailSprite *sprite;
    if (index < 0)
        index = 5;
    for (i = 0, sprite = (struct EffectTrailSprite *)((u32)effect + 0x12c); i <= 5; i++, sprite++) {
        new_var = &sprite->position;
        if (i * 2 > effect->flags) {
            sprite->position = ((struct ActorEffect *)(index * 8 + (u32)effect))->history[0];
            index--;
            if (index < 0)
                index = 5;
        } else {
            MakeOffset(&delta, effect->angles[i], effect->radii[i]);
            AddOffset(new_var, &delta);
        }
    }
    if (!(effect->flags & 1)) {
        ((struct ActorEffect *)(effect->trailIndex * 8 + (u32)effect))->history[0] =
            effect->trails[0].position;
        effect->trailIndex++;
        if (effect->trailIndex > 5)
            effect->trailIndex = 0;
    }
}
