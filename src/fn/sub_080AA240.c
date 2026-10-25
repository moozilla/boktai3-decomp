#include "global.h"

// confidence=low: trail tail; moves host position and odd trail segments with sine offsets, then relaxes even segments toward their neighbours
extern const s16 gUnk_086149C4[];

static inline s32 Trail240Shr12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

static inline s32 Trail240Shr10(s32 v)
{
    if (v >= 0)
        return v >> 10;
    return -((-v) >> 10);
}

struct Pos240 {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));

struct Seg240 {
    u8 unknown0[0x1C];
    struct Pos240 pos;
    u8 unknown24[0x105 - 0x24];
    u8 flags;
    u8 unknown106[0x109 - 0x106];
    u8 phase;
    u8 unknown10A[0x128 - 0x10A];
};

struct Trail240 {
    u8 unknown0[0x3D4];
    struct Seg240 seg[4];
    u8 unknown874[0x8D2 - 0x874];
    u8 phase;
    u8 unknown8D3[0x8DA - 0x8D3];
    s16 count;
    u8 unknown8DC[0x8EA - 0x8DC];
    u8 active;
};

struct Host240 {
    u8 unknown0[0x1C];
    struct Pos240 pos;
};

struct Actor240 {
    u8 unknown0[4];
    struct Host240 *host;
    u8 unknown8[0x3D0 - 8];
    struct Trail240 *trail;
};

#ifdef NONMATCHING
// fuzzy: 25.1%
// scope shape 321 (differences once low registers are set aside)
static void sub_080AA4B8(struct Actor240 *actor)
{
    struct Trail240 *trail = actor->trail;
    s32 live = 0;
    s32 i;

    for (i = 0; i < trail->count; i++) {
        struct Seg240 *seg = &trail->seg[i];
        if (!(seg->flags & 1)) {
            if (live == 1) {
                live = 0;
            } else {
                seg->flags &= ~2;
                live++;
            }
        }
    }
}

void sub_080AA240(struct Actor240 *actor)
{
    struct Trail240 *trail = actor->trail;
    struct Host240 *host = actor->host;
    struct Pos240 prev[1];
    struct Pos240 out[1];
    struct Pos240 delta[1];
    struct Pos240 acc[1];
    struct Pos240 *cur;
    struct Pos240 *dst;
    u8 *ph;
    s32 sign = 1;
    s32 i;
    s32 j;
    s32 dx;
    s32 dz;

    dx = Trail240Shr12(gUnk_086149C4[(u8)(trail->phase + 0x40)] * 2);
    host->pos.x += dx;
    dz = Trail240Shr12(gUnk_086149C4[trail->phase] * 16);
    host->pos.z += dz;
    trail->phase += 8;

    ph = &trail->seg[1].phase;
    cur = &trail->seg[1].pos;
    for (i = 1; i < trail->count - 1; i += 2) {
        s32 step = Trail240Shr12(gUnk_086149C4[*ph] * 4);
        cur->x += sign * step;
        *ph += 8;
        sign = -sign;
        ph += 0x250;
        cur = (struct Pos240 *)((u8 *)cur + 0x250);
    }

    prev[0] = host->pos;
    cur = &trail->seg[1].pos;
    dst = &trail->seg[0].pos;
    for (j = 0; j < trail->count - 1; j += 2) {
        delta[0].x = cur->x - prev[0].x;
        delta[0].y = cur->y - prev[0].y;
        delta[0].z = cur->z - prev[0].z;
        ZeroFill8(&acc[0]);
        out[0].x = prev[0].x + Trail240Shr10(acc[0].x * -0x17F + delta[0].x * 0x180);
        out[0].y = prev[0].y + Trail240Shr10(acc[0].y * -0x17F + delta[0].y * 0x180);
        out[0].z = prev[0].z + Trail240Shr10(acc[0].z * -0x17F + delta[0].z * 0x180);
        *dst = out[0];
        prev[0] = *cur;
        cur = (struct Pos240 *)((u8 *)cur + 0x250);
        dst = (struct Pos240 *)((u8 *)dst + 0x250);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080AA240);
#endif
