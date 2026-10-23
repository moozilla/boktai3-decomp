#include "global.h"

struct S080FC894 {
    u8 filler[0x3d0];
    u8 *q;
};

void Axe_CopyAuxiliaryVector(struct S080FC894 *s, u32 *src)
{
    u32 *dst;
    u32 off = 0xa54;
    u32 a, b;
    dst = (u32 *)s->q;
    dst = (u32 *)((u8 *)dst + off);
    do {
        a = src[0];
        b = src[1];
        dst[0] = a;
    } while (0);
    dst[1] = b;
}
