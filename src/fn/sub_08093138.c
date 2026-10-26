#include "global.h"
#include "engine.h"

extern const u16 gUnk_0203B400[];

#ifdef NONMATCHING
// fuzzy: 21.7%
// scope shape 201 (differences once low registers are set aside)
// confidence=low: advances a bee's eased move from its start point to its end point and writes the new position at +0x450; returns 1 when the move has run out.
s32 sub_08093138(u8 *arg0)
{
    u8 *r = *(u8 **)(arg0 + 0x3d0);
    u32 a[2];
    u32 b[2];
    u32 acc[2];
    s16 d[3];
    s32 t;
    s32 s;
    s32 q;
    s32 e;
    s32 h;
    s32 w;
    s32 tan;
    s32 idx;
    s32 step;

    a[0] = *(u32 *)(r + 0x470);
    a[1] = *(u32 *)(r + 0x474);
    b[0] = *(u32 *)(r + 0x478);
    b[1] = *(u32 *)(r + 0x47C);
    a[0] &= 0xFFFF;
    b[0] &= 0xFFFF;
    t = *(u16 *)(r + 0x430);
    s = *(s16 *)(r + 0x434);
    d[0] = ((u16 *)b)[0] - ((u16 *)a)[0];
    d[1] = ((u16 *)b)[1] - ((u16 *)a)[1];
    d[2] = ((u16 *)b)[2] - ((u16 *)a)[2];
    ZeroFill8(acc);

    q = (t * t) / 1024;
    e = (t * q) / 1024;
    h = 2 * e - 3 * q + 1;
    w = 3 * q - 2 * e;
    tan = s * (e - 2 * q + t);

    step = (((s16 *)acc)[0] * h + d[0] * w + tan) / 1024;
    *(u16 *)(r + 0x450) = ((u16 *)a)[0] + step;
    step = (((s16 *)acc)[1] * h + d[1] * w + tan) / 1024;
    *(u16 *)(r + 0x452) = ((u16 *)a)[1] + step;
    step = (((s16 *)acc)[2] * h + d[2] * w + tan) / 1024;
    *(u16 *)(r + 0x454) = ((u16 *)a)[2] + step;

    if (*(u16 *)(r + 0x430) > 0x3FF) {
        *(u16 *)(r + 0x430) = 0;
        return 1;
    }
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    idx = gRandomIndex;
    t = *(u16 *)(r + 0x430) + (u8)gUnk_0203B400[idx] + 0x100;
    if (t > 0x400) {
        t = 0x400;
    }
    *(u16 *)(r + 0x430) = t;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08093138);
#endif
