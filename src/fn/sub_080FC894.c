#include "global.h"
// MATCHING_CONCESSION: register-pinning

struct S080FC894 {
    u8 filler[0x3d0];
    u8 *q;
};

void Axe_CopyAuxiliaryVector(struct S080FC894 *s, u32 *src)
{
    register u32 *dst asm("r2");
    u32 off = 0xa54;
    u32 a, b;
    dst = (u32 *)s->q;
    asm("" : "+r"(off));
    dst = (u32 *)((u8 *)dst + off);
    a = src[0];
    b = src[1];
    dst[0] = a;
    dst[1] = b;
}
