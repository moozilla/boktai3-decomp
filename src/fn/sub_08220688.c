#include "global.h"

s32 Div(s32, s32);

// Clips segment a->b against the box (center c, half-extents d) on its near
// face along axis 2; writes the hit point to out and returns 1 when the
// interpolated axes 0 and 1 fall inside the box.
u32 Collider_TestSegmentZAxis(s16 *a, s16 *b, s16 *c, s16 *d, s16 *out)
{
    s16 start = a[2];
    s16 delta = b[2] - start;
    s16 p, q, r;
    s16 center;

    if (delta == 0)
        return 0;
    center = c[2];
    if (center < start) {
        p = center + d[2];
        if (p > start)
            p = start;
    } else {
        p = center - d[2];
        if (p < start)
            p = start;
    }
    q = a[0] + Div((b[0] - a[0]) * (p - start), delta);
    if (q < c[0] - d[0] || q > c[0] + d[0])
        return 0;
    r = a[1] + Div((b[1] - a[1]) * (p - start), delta);
    if (r < c[1] - d[1] || r > c[1] + d[1])
        return 0;
    out[2] = p;
    out[0] = q;
    out[1] = r;
    return 1;
}
