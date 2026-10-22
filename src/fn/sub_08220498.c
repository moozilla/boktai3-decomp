#include "global.h"

s32 Div(s32, s32);

// Clips segment a->b against the box (center c, half-extents d) on its near
// face along axis 0; writes the hit point to out and returns 1 when the
// interpolated axes 1 and 2 fall inside the box.
u32 Collider_TestSegmentXAxis(s16 *a, s16 *b, s16 *c, s16 *d, s16 *out)
{
    s16 start = a[0];
    s16 delta = b[0] - start;
    s16 p, q, r;
    s16 center;

    if (delta == 0)
        return 0;
    center = c[0];
    if (center < start) {
        p = center + d[0];
        if (p > start)
            p = start;
    } else {
        p = center - d[0];
        if (p < start)
            p = start;
    }
    q = a[1] + Div((b[1] - a[1]) * (p - start), delta);
    if (q < c[1] - d[1] || q > c[1] + d[1])
        return 0;
    r = a[2] + Div((b[2] - a[2]) * (p - start), delta);
    if (r < c[2] - d[2] || r > c[2] + d[2])
        return 0;
    out[0] = p;
    out[1] = q;
    out[2] = r;
    return 1;
}
