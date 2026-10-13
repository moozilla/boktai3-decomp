#include "global.h"
// MATCHING_CONCESSION: register-pinning
static inline void copy32(const void *source, void *dest, s32 size)
{
    CpuSet(source, dest, (size / 4 & 0x1fffff) | 0x04000000);
}
#define Palette_BlendRange sub_08002AE0
void Palette_BlendRange(u16 *dest, const u16 *source, const u16 *target, u32 amount, s32 first,
                        s32 end)
{
    s32 count;
    u32 inverse;
    amount >>= 1;
    inverse = 32 - amount;
    count = end - first;
    if (count) {
        if (amount >= 32) {
            const u16 *to = target + first;
            dest += first;
            copy32(to, dest, count * 2);
        } else {
            const u16 *a = source + first, *b = target + first;
            u16 *out;
            s32 i;
            dest += first;
            out = dest;
            if (count > 0) {
                register u32 greenMask asm("r10") = 0x03E0;
                i = count;
                do {
                    register u32 from asm("r0") = *a;
                    register u32 fromRB asm("r3");
                    u32 fromG;
                    u32 to;
                    fromRB = 0x7C1F & from;
                    fromG = greenMask & from;
                    to = *b;
                    {
                        u32 rb = (((to & 0x7C1F) * amount) + (inverse * fromRB)) & 0xF83E0;
                        u32 g = (((to & greenMask) * amount) + (inverse * fromG)) & 0x7C00;
                        *out = (rb | g) >> 5;
                    }
                    a++;
                    b++;
                    out++;
                } while (--i);
            }
        }
    }
}
