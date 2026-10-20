#define gSineTable gUnk_086149C4
#include "orbit_motion.h"
/* Orbit_OffsetFromAngle: Writes a point on the horizontal circle around base + 0x20 at the given angle and radius */
#define Orbit_OffsetFromAngle sub_0822223C

void sub_0822223C(u8 *owner, struct OrbitVector *out, u8 angle, u16 radius)
{
    struct OrbitVector tmp;
    polar(&tmp, angle, radius);
    out->x = *(s16 *)(owner + 0x20) + tmp.x;
    out->y = *(s16 *)(owner + 0x22) + tmp.y;
    out->z = *(s16 *)(owner + 0x24) + tmp.z;
}
