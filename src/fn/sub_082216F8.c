#include "global.h"
/* Vec16_Lerp: Blends two s16 vectors with weight t out of 1 << shift */

struct Vec16 {
    s16 x, y, z;
};

#define SHR(v, s) ((v) >= 0 ? (v) >> (s) : -(-(v) >> (s)))

void Vec16_Lerp(struct Vec16 *out, struct Vec16 *a, struct Vec16 *b, s32 t, s32 shift)
{
    s32 w = (1 << shift) - t;
    s32 v;
    v = a->x * w + b->x * t;
    out->x = SHR(v, shift);
    v = a->y * w + b->y * t;
    out->y = SHR(v, shift);
    v = a->z * w + b->z * t;
    out->z = SHR(v, shift);
}
