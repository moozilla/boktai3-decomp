#include "global.h"

static inline s32 Sub0809CCF4Shift10(s32 x)
{
    if (x >= 0)
        return x >> 10;
    return -((-x) >> 10);
}
struct Sub0809CCF4Pos {
    u16 x, y, z, pad;
};
struct Sub0809CCF4Vec {
    s16 x, y, z, pad;
};
#ifdef NONMATCHING
// fuzzy: 36.8%
// scope shape 142 (differences once low registers are set aside)
// confidence=low: steps a Hermite-blended offset from a start point toward an end point and reports when the timer runs out.
u8 sub_0809CCF4(void *arg0)
{
    u8 *body;
    u32 startWords[2];
    u32 endWords[2];
    struct Sub0809CCF4Pos *start;
    struct Sub0809CCF4Pos *end;
    struct Sub0809CCF4Vec delta, tangent;
    s32 speed, t, q, c, s, tang, off;

    body = *(u8 **)((u8 *)arg0 + 0x3D0);
    *(u32 *)startWords = *(u32 *)(body + 0x414);
    startWords[1] = *(u32 *)(body + 0x418);
    startWords[0] &= 0xFFFF;
    endWords[0] = *(u32 *)(body + 0x41C);
    endWords[1] = *(u32 *)(body + 0x420);
    endWords[0] &= 0xFFFF;
    start = (struct Sub0809CCF4Pos *)startWords;
    end = (struct Sub0809CCF4Pos *)endWords;
    t = *(u16 *)(body + 0x3E0);
    speed = *(s16 *)(body + 0x3E4);
    delta.x = end->x - start->x;
    delta.y = end->y - start->y;
    delta.z = end->z - start->z;
    ZeroFill8(&tangent);
    q = Sub0809CCF4Shift10(t * t);
    c = Sub0809CCF4Shift10(t * q);
    s = 3 * q - 2 * c;
    tang = speed * (t + c - 2 * q);
    off = Sub0809CCF4Shift10(tangent.x * (1 - s) + delta.x * s + tang);
    *(u16 *)(body + 0x3F4) = start->x + off;
    off = Sub0809CCF4Shift10(tangent.y * (1 - s) + delta.y * s + tang);
    *(u16 *)(body + 0x3F6) = start->y + off;
    off = Sub0809CCF4Shift10(tangent.z * (1 - s) + delta.z * s + tang);
    *(u16 *)(body + 0x3F8) = start->z + off;
    if (*(u16 *)(body + 0x3E0) > 0x3FF)
        return 1;
    *(u16 *)(body + 0x3E0) += 0x80;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0809CCF4);
#endif
