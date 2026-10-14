#include "global.h"

struct Hdr0821607C {
    u32 w[16];
};
void *memcpy(void *, const void *, u32);

u8 *sub_0821607C(u8 *base, s32 i)
{
    struct Hdr0821607C h;
    memcpy(&h, base, 64);
    h.w[7] += (u32)base;
    h.w[9] += (u32)base;
    h.w[3] += (u32)base;
    h.w[5] += (u32)base;
    return (u8 *)h.w[3] + h.w[11] * i * 2;
}
