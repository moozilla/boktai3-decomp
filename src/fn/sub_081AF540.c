// MATCHING_CONCESSION: register-pinning
#include "global.h"

#define Vanargand_ProjectEffectOffset sub_081AF540
struct VanargandEffectVec { s16 x, y, z; };
static inline s32 SignedHalf(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
void Vanargand_ProjectEffectOffset(struct VanargandEffectVec *out, s16 x, s16 y, s32 height)
{
    union {
        u32 packed;
        s16 coordinate[3];
    } point;
    s16 *base;
    register s32 a asm("r5");
    register s32 b asm("r6");
    {
        u32 low = (u32)x << 16;
        u32 high = (u32)y << 16;
        asm("" : "+r"(low), "+r"(high));
        point.packed = (low >> 16) | high;
    }
    base = point.coordinate;
    a = point.coordinate[0];
    asm("" : "+r"(a));
    b = base[1];
    asm("" : "+r"(b));
    out->x = SignedHalf(((a + b) << 5) / 3 + height);
    out->y = height;
    out->z = SignedHalf(((b - a) << 5) / 3 + height);
}
