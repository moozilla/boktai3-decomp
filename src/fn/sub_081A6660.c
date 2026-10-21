#include "global.h"

#define gEffectSineTable gUnk_086149C4
struct EffectVector {
    s16 x, y, z, pad;
};
struct EffectArcNode {
    u32 flags;
    u8 pad4[0x18];
    struct EffectVector position;
    u8 pad24[8];
};
struct PositionedEffect {
    struct EffectArcNode nodes[6];
    u8 pad108[0x1c];
    struct EffectVector *start, *end;
};
extern const s16 gEffectSineTable[];
s32 __divsi3(s32, s32);
static inline void SubVector(struct EffectVector *v, struct EffectVector *a, struct EffectVector *b)
{
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}
static inline void SetVector(struct EffectVector *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
void PositionedEffect_InterpolateArcNodes(struct PositionedEffect *effect)
{
    struct EffectVector diff;
    s32 i, dx, dy, dz, z, x;
    struct EffectArcNode *node;
    struct EffectVector *d;
    SubVector(&diff, effect->end, effect->start);
    i = 0;
    *(&d) = &diff;
    dx = d->x;
    dy = d->y;
    dz = d->z;
    z = 0;
    x = 0;
    node = effect->nodes;
    do {
        struct EffectVector *start = effect->start;
        s32 px = start->x;
        s32 ox = (x / 5);
        s32 py = start->y;
        s32 angle = ((i << 6) / 5);
        s32 oy = (dy * gEffectSineTable[angle & 255]) >> 12;
        s32 pz = start->z;
        s32 oz = (z / 5);
        SetVector(&node->position, px + ox, py + oy, pz + oz);
        node->flags &= ~1;
        node++;
        z += dz;
        x += dx;
    } while (++i <= 5);
}
