#include "global.h"

// confidence=low: writes eight cubic-blend samples from the actor's point toward the target point into its buffer, then stores the target.
static inline s32 sub0809345C_Shift10(s32 n)
{
    if (n >= 0)
        return n >> 10;
    return -((-n) >> 10);
}

#ifdef NONMATCHING
// fuzzy: 25.6%
// scope shape 154 (differences once low registers are set aside)
// confidence=low: writes eight cubic-blend samples from the actor's point toward the target point into its buffer, then stores the target.
void sub_0809345C(void *arg0, void *arg1)
{
    u8 *actor = arg0;
    u8 *pool;
    u32 zeroSrc;
    u32 start[2];
    u32 end[2];
    s16 delta[4];
    s16 zv[4];
    s32 unit;
    s32 sc1;
    s32 sc2;
    s32 i;
    u16 *a16;
    u16 *b16;

    pool = *(u8 **)(actor + 0x3D0);
    start[0] = *(u32 *)(actor + 0x5C);
    start[1] = *(u32 *)(actor + 0x60);
    end[0] = ((u32 *)arg1)[0];
    end[1] = ((u32 *)arg1)[1];
    start[0] &= 0xFFFF;
    end[0] &= 0xFFFF;
    unit = Div(0x400, 8);
    sc1 = *(s16 *)(pool + 0x480) << 4;
    sc2 = *(s16 *)(pool + 0x484) << 4;
    a16 = (u16 *)end;
    b16 = (u16 *)start;

    for (i = 0; i < 8; i++) {
        s32 t = unit * (i + 1);
        s16 *out = (s16 *)(pool + 0x3E0 + i * 8);
        s32 s1;
        s32 s2;
        s32 h00;
        s32 h01;
        s32 h10;
        s32 h11;

        delta[0] = a16[0] - b16[0];
        delta[1] = a16[1] - b16[1];
        delta[2] = a16[2] - b16[2];
        zeroSrc = 0;
        CpuSet(&zeroSrc, zv, 0x05000002);
        s1 = sub0809345C_Shift10(t * t);
        s2 = sub0809345C_Shift10(s1 * t);
        h00 = 2 * s2 - 3 * s1 + 1;
        h01 = 3 * s1 - 2 * s2;
        h10 = s2 - 2 * s1 + t;
        h11 = s2 - s1;
        out[0] =
            b16[0] + sub0809345C_Shift10(sc1 * (h00 * zv[0]) + h01 * delta[0] + sc2 * h10 + h11);
        out[1] =
            b16[1] + sub0809345C_Shift10(sc1 * (h00 * zv[1]) + h01 * delta[1] + sc2 * h10 + h11);
        out[2] =
            b16[2] + sub0809345C_Shift10(sc1 * (h00 * zv[2]) + h01 * delta[2] + sc2 * h10 + h11);
    }
    *(u32 *)(pool + 0x420) = end[0];
    *(u32 *)(pool + 0x424) = end[1];
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0809345C);
#endif
