#include "global.h"

#include "contact_actor.h"

// confidence=low: steps a 0..1023 phase and writes the cubic blend of the start and end points to dest.

struct Sub08097798Context {
    u8 unk0[0x5B4];
    u16 phase5B4;
    u8 unk5B6[2];
    s16 scale5B8;
    u8 unk5BA[0x5D4 - 0x5BA];
    struct ContactPosition dest5D4;
    u8 unk5DC[0x5EC - 0x5DC];
    struct ContactPosition start5EC;
    struct ContactPosition end5F4;
};

struct Sub08097798Actor {
    u8 unk0[0x3D0];
    struct Sub08097798Context *context;
};

static inline s32 Trunc10_08097798(s32 value)
{
    if (value >= 0)
        return value >> 10;
    return -((-value) >> 10);
}

#ifdef NONMATCHING
// fuzzy: 36.1%
// scope shape 150 (differences once low registers are set aside)
// confidence=low: steps a 0..1023 phase and writes the cubic blend of the start and end points to dest.
u8 sub_08097798(void *arg0)
{
    struct Sub08097798Context *ctx = ((struct Sub08097798Actor *)arg0)->context;
    struct ContactPosition start;
    struct ContactPosition end;
    struct ContactPosition delta;
    struct ContactPosition offset;
    u16 phase;
    s32 q;
    s32 w;
    s32 h00;
    s32 h01;
    s32 tangent;

    start = ctx->start5EC;
    ((u32 *)&start)[0] &= 0xFFFF;
    end = ctx->end5F4;
    ((u32 *)&end)[0] &= 0xFFFF;
    delta.x = end.x - start.x;
    delta.y = end.y - start.y;
    delta.z = end.z - start.z;
    ZeroFill8(&offset);

    phase = ctx->phase5B4;
    q = Trunc10_08097798(phase * phase);
    w = Trunc10_08097798(phase * q);
    h00 = 2 * w - 3 * q + 1;
    h01 = 3 * q - 2 * w;
    tangent = ctx->scale5B8 * (w - 2 * q + phase);

    ctx->dest5D4.x = start.x + Trunc10_08097798(offset.x * h00 + delta.x * h01 + tangent);
    ctx->dest5D4.y = start.y + Trunc10_08097798(offset.y * h00 + delta.y * h01 + tangent);
    ctx->dest5D4.z = start.z + Trunc10_08097798(offset.z * h00 + delta.z * h01 + tangent);

    if (ctx->phase5B4 > 0x3FF)
        return 1;
    ctx->phase5B4 = ctx->phase5B4 + 0x80;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08097798);
#endif
