#include "global.h"
#include "objects/dungeon_prop/orbit_motion.h"
/* Math_BlendAngleWithTableFactor: Wraps the angle near its target then blends using the indexed table factor */
#define Math_BlendAngleWithTableFactor sub_0819B244

extern const s16 gUnk_08611D24[];

static inline s32 AngleNear(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    s32 x = d;
    if (x > 128)
        d = x - 256;
    return ref + d;
}

s32 sub_0819B244(s32 angle, s32 target, s32 t)
{
    s32 cur;
    if (t > 15)
        t = 15;
    t = gUnk_08611D24[t];
    cur = AngleNear(angle, target);
    if (t == 0)
        return target;
    return (target - cur) * scale8(t) + cur;
}
