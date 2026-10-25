#include "global.h"

#define TRUNC_SHIFT(v) ((v) >= 0 ? (v) >> 10 : -(-(v) >> 10))

#define TRUNC_SHIFT(v) ((v) >= 0 ? (v) >> 10 : -(-(v) >> 10))

// confidence=low: moves an actor toward a curved target and advances its timer

#define TRUNC_SHIFT(v) ((v) >= 0 ? (v) >> 10 : -(-(v) >> 10))

union AxeVector {
    struct {
        s16 x, y, z, unk_06;
    } c;
    struct {
        u32 lo, hi;
    } words;
};
struct AxeMoveContext {
    u8 unk_00[0xEA8];
    u16 time;
    u8 unk_EAA[2];
    s16 curve;
    u8 unk_EAE[0xF04 - 0xEAE];
    union AxeVector start, end;
};
struct AxeMoveNode {
    u8 unk_00[12];
    union AxeVector position;
};
struct AxeMoveActor {
    u8 unk_00[0x3D0];
    struct AxeMoveContext *context;
    u8 unk_3D4[0x5D0 - 0x3D4];
    struct AxeMoveNode *node;
};
void CpuSet(const void *, void *, u32);
#define TRUNC_SHIFT(v) ((v) >= 0 ? (v) >> 10 : -(-(v) >> 10))
static inline void Subtract(union AxeVector *out, union AxeVector *a, union AxeVector *b)
{
    out->c.x = b->c.x - a->c.x;
    out->c.y = b->c.y - a->c.y;
    out->c.z = b->c.z - a->c.z;
}
static inline void Interpolate(union AxeVector *out, union AxeVector *a, union AxeVector *b,
                               union AxeVector *d, s32 t, s32 curve)
{
    s32 square, cube, offset, value, weightB, weightD;
    value = t * t;
    square = TRUNC_SHIFT(value);
    value = square * t;
    cube = TRUNC_SHIFT(value);
    weightB = 2 * cube - 3 * square + 1;
    weightD = 3 * square - 2 * cube;
    offset = (cube - 2 * square + t) * curve;
    value = weightB * b->c.x + weightD * d->c.x + offset;
    out->c.x = a->c.x + TRUNC_SHIFT(value);
    value = weightB * b->c.y + weightD * d->c.y + offset;
    out->c.y = a->c.y + TRUNC_SHIFT(value);
    value = weightB * b->c.z + weightD * d->c.z + offset;
    out->c.z = a->c.z + TRUNC_SHIFT(value);
}
#ifdef NONMATCHING
// fuzzy: 35.8%
// scope shape 130 (differences once low registers are set aside)
s32 sub_080FAA5C(struct AxeMoveActor *actor)
{
    struct AxeMoveContext *context = actor->context;
    u32 fill;
    union AxeVector start, end, delta, zero;
    union AxeVector *out;
    s32 t, curve;
    start = context->start;
    start.words.lo &= 0xFFFF;
    end = context->end;
    end.words.lo &= 0xFFFF;
    out = &actor->node->position;
    t = context->time;
    curve = context->curve;
    Subtract(&delta, &start, &end);
    fill = 0;
    CpuSet(&fill, &zero, 0x05000002);
    Interpolate(out, &start, &zero, &delta, t, curve);
    if (context->time <= 1023) {
        context->time += 128;
        return 0;
    }
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080FAA5C);
#endif
