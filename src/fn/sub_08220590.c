#include "global.h"

s32 Div(s32, s32);

// Clips segment a->b against the box (center c, half-extents d) on its near
// face along axis 1; writes the hit point to out and returns 1 when the
// interpolated axes 2 and 0 fall inside the box.
u32 Collider_TestSegmentYAxis(s16 *a, s16 *b, s16 *c, s16 *d, s16 *out)
{
    s16 start = a[1];
    s16 delta = b[1] - start;
    s16 p, q, r;
    s16 center;

    if (delta == 0)
        return 0;
    center = c[1];
    if (center < start) {
        p = center + d[1];
        if (p > start)
            p = start;
    } else {
        p = center - d[1];
        if (p < start)
            p = start;
    }
    q = a[2] + Div((b[2] - a[2]) * (p - start), delta);
    if (q < c[2] - d[2] || q > c[2] + d[2])
        return 0;
    r = a[0] + Div((b[0] - a[0]) * (p - start), delta);
    if (r < c[0] - d[0] || r > c[0] + d[0])
        return 0;
    out[1] = p;
    out[2] = q;
    out[0] = r;
    return 1;
}
